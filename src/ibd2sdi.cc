#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <string>
#include <zlib.h>

#include <rapidjson/document.h>

#include "include/my_inttypes.h"
#include "include/fil0fil.h"
#include "include/fil0types.h"
#include "include/page0page.h"
#include "include/page0types.h"
#include "include/fsp0fsp.h"
#include "include/fsp0types.h"
#include "include/rem0types.h"
#include "include/rec.h"
#include "include/ibd2sdi.h"

// Constants
static const uint32_t kPageSize = 16384;

// External references to global variables defined in inno_space.cc
extern int fd;
extern ulint offsets_[];

struct dict_col {
  std::string col_name;
  std::string column_type_utf8;
  int char_length;
};

extern std::vector<dict_col> dict_cols;

// Read SDI data from an SDI BLOB page chain
static std::string read_sdi_blob(uint32_t first_page_no, uint32_t data_len) {
  std::string blob_data;
  blob_data.reserve(data_len);

  uint32_t page_no = first_page_no;
  byte* blob_buf = (byte*)malloc(kPageSize);
  if (!blob_buf) {
    return "";
  }

  while (page_no != FIL_NULL && blob_data.size() < data_len) {
    uint64_t offset = (uint64_t)kPageSize * (uint64_t)page_no;
    int ret = pread(fd, blob_buf, kPageSize, offset);
    if (ret != (int)kPageSize) {
      free(blob_buf);
      return "";
    }

    uint16_t page_type = mach_read_from_2(blob_buf + FIL_PAGE_TYPE);

    if (page_type == FIL_PAGE_SDI_BLOB) {
      // First SDI BLOB page
      uint32_t part_len = mach_read_from_4(blob_buf + FIL_PAGE_DATA + BTR_BLOB_HDR_PART_LEN);
      page_no = mach_read_from_4(blob_buf + FIL_PAGE_DATA + BTR_BLOB_HDR_NEXT_PAGE_NO);
      byte* data_start = blob_buf + FIL_PAGE_DATA + BTR_BLOB_HDR_SIZE;
      blob_data.append((char*)data_start, part_len);
    } else if (page_type == FIL_PAGE_SDI_ZBLOB) {
      // Compressed SDI BLOB page - not typically used for SDI
      free(blob_buf);
      return "";
    } else {
      break;
    }
  }

  free(blob_buf);
  return blob_data;
}

int parse_sdi_json(const std::string& json_str) {
  rapidjson::Document d;
  d.Parse(json_str.c_str());

  if (d.HasParseError()) {
    std::cerr << "Failed to parse SDI JSON" << std::endl;
    return 1;
  }

  // the sysbench offsets array
  // offsets[0] 100   // 这个值默认初始化成100, 数组的元素个数, 没用
  // offsets[1] 6  // 除了默认offsets[0, 1, 2] 三列以后, 剩下的其他列的个数, 是table 里面列的个数+ 2(trx_id 和 rollptr), 从offset[3] 开始
  // offsets[2] 2147483653  //extra size
  // offsets[3] 4  // id 这个列
  // offsets[4] 10  // 根据record format 可以看出, 这个是trx_id
  // offsets[5] 17  // 根据record format 可以看出, 这个是rollptr
  // offsets[6] 21 // k 这个列
  // offsets[7] 141 // c 这个列
  // offsets[8] 201 // pad 这个列,   201 + offset[2](如果有extra size) 就是整个rec 的大小

  // init offsets array from here
  memset(offsets_, 0, sizeof(ulint) * REC_OFFS_NORMAL_SIZE);
  offsets_[0] = REC_OFFS_NORMAL_SIZE;

  const auto& columns = d[0]["object"]["dd_object"]["columns"];

  // Count only user-visible columns (hidden == 1, not system columns with hidden == 2)
  uint32_t user_col_count = 0;
  for (rapidjson::SizeType i = 0; i < columns.Size(); i++) {
    if (columns[i]["hidden"].GetInt() == 1) {
      user_col_count++;
    }
  }

  offsets_[1] = user_col_count + 2;  // user columns + DB_TRX_ID + DB_ROLL_PTR
  // TODO: init extra size
  offsets_[2] = 0;

  dict_cols.resize(REC_OFFS_NORMAL_SIZE);

  // Process only user-visible columns (hidden == 1)
  uint32_t offset_idx = 3;
  uint32_t current_offset = 0;  // Track cumulative byte offset from record origin
  for (rapidjson::SizeType i = 0; i < columns.Size(); i++) {
    if (columns[i]["hidden"].GetInt() != 1) {
      continue;  // Skip system columns (DB_TRX_ID, DB_ROLL_PTR)
    }

    dict_cols[offset_idx].col_name = columns[i]["name"].GetString();
    dict_cols[offset_idx].column_type_utf8 = columns[i]["column_type_utf8"].GetString();

    offsets_[offset_idx] = current_offset;

    if (dict_cols[offset_idx].column_type_utf8 == "int") {
      dict_cols[offset_idx].char_length = 4;
      current_offset += 4;
    } else if (dict_cols[offset_idx].column_type_utf8.length() >= 4 &&
               dict_cols[offset_idx].column_type_utf8.substr(0, 4) == "char") {
      int char_len = columns[i]["char_length"].GetInt();
      dict_cols[offset_idx].char_length = char_len;
      current_offset += char_len;
    } else {
      std::cerr << "Unsupported column type: " << dict_cols[offset_idx].column_type_utf8 << std::endl;
      return -1;
    }

    offset_idx++;
  }

  // Add space for DB_TRX_ID and DB_ROLL_PTR
  offsets_[offset_idx] = current_offset;  // DB_TRX_ID
  current_offset += 6;
  offsets_[offset_idx + 1] = current_offset;  // DB_ROLL_PTR

  return 0;
}

/** TRUE if the record is the supremum record on a page.
 @return true if the supremum record */
static inline bool page_rec_is_supremum_low(
    ulint offset) /*!< in: record offset on page */
{
  return (offset == PAGE_NEW_SUPREMUM || offset == PAGE_OLD_SUPREMUM);
}

// Extract SDI from the IBD file natively (without external ibd2sdi tool)
int extract_sdi_from_ibd(int file_fd, std::string& json_output) {
  // Use the provided file descriptor
  int saved_fd = fd;
  fd = file_fd;

  // Read page 0 to get SDI root page number
  byte* page0_buf = (byte*)malloc(kPageSize);
  if (!page0_buf) {
    std::cerr << "Failed to allocate memory for page 0" << std::endl;
    fd = saved_fd;
    return 1;
  }

  int ret = pread(fd, page0_buf, kPageSize, 0);
  if (ret != (int)kPageSize) {
    std::cerr << "Failed to read page 0" << std::endl;
    free(page0_buf);
    fd = saved_fd;
    return 1;
  }

  // SDI root page number is at offset FSP_SDI_ROOT_PAGE_NUM in page 0
  uint32_t sdi_root_page = mach_read_from_4(page0_buf + FSP_SDI_ROOT_PAGE_NUM);
  free(page0_buf);

  // If SDI root page is 0 or FIL_NULL, try page 3 as default SDI page
  // (MySQL 8.0 single-table tablespaces typically have SDI on page 3)
  if (sdi_root_page == 0 || sdi_root_page == FIL_NULL) {
    sdi_root_page = 3;
  }

  // Read the SDI root page
  byte* sdi_buf = (byte*)malloc(kPageSize);
  if (!sdi_buf) {
    std::cerr << "Failed to allocate memory for SDI page" << std::endl;
    fd = saved_fd;
    return 1;
  }

  uint64_t sdi_offset = (uint64_t)kPageSize * (uint64_t)sdi_root_page;
  ret = pread(fd, sdi_buf, kPageSize, sdi_offset);
  if (ret != (int)kPageSize) {
    std::cerr << "Failed to read SDI page " << sdi_root_page << std::endl;
    free(sdi_buf);
    fd = saved_fd;
    return 1;
  }

  // Verify it's an SDI page
  uint16_t page_type = mach_read_from_2(sdi_buf + FIL_PAGE_TYPE);
  if (page_type != FIL_PAGE_SDI) {
    std::cerr << "Page " << sdi_root_page << " is not an SDI page (type: " << page_type << ")" << std::endl;
    free(sdi_buf);
    fd = saved_fd;
    return 1;
  }

  json_output = "[\n";
  bool first_record = true;

  byte* rec_ptr = sdi_buf + PAGE_NEW_INFIMUM;

  while (1) {
    ulint raw_off = mach_read_from_2(rec_ptr - REC_NEXT);
    ulint current_off = (ulint)(rec_ptr - sdi_buf);
    ulint off = (current_off + raw_off) & (kPageSize - 1);

    if (page_rec_is_supremum_low(off)) {
      break;
    }

    rec_ptr = sdi_buf + off;

    // Check record status - skip non-ordinary records
    ulint rec_status = rec_get_status(rec_ptr);
    if (rec_status != REC_STATUS_ORDINARY) {
      continue;
    }

    // Check if record is deleted
    ulint info_bits = rec_get_bit_field_1(rec_ptr, REC_NEW_INFO_BITS, REC_INFO_BITS_MASK, REC_INFO_BITS_SHIFT);
    if (info_bits & REC_INFO_DELETED_FLAG) {
      continue;
    }

    // SDI record format (compact row format):
    // Record pointer points to the start of fixed-length fields
    // SDI index has: type (INT), id (BIGINT), uncompressed_len (INT), compressed_len (INT), data (BLOB)

    uint32_t sdi_type = mach_read_from_4(rec_ptr);
    uint64_t sdi_id = mach_read_from_8(rec_ptr + 4);
    uint32_t uncomp_len = mach_read_from_2(rec_ptr + 27);
    uint32_t comp_len = mach_read_from_2(rec_ptr + 31);

    byte* data_ptr = rec_ptr + 33;

    std::string compressed_data;

    // Check if data is stored externally (BLOB)
    // External flag is indicated in the variable-length field offsets
    // For simplicity, we check if compressed data fits in the page
    uint32_t rec_offset_in_page = (uint32_t)(rec_ptr - sdi_buf);
    uint32_t space_left = kPageSize - FIL_PAGE_DATA_END - rec_offset_in_page - 33;

    if (comp_len <= space_left && comp_len < 8000) {
      // Data is inline
      compressed_data.assign((char*)data_ptr, comp_len);
    } else {
      // Data is stored externally in BLOB pages
      // External pointer format: space_id (4), page_no (4), offset (4), length (8)
      // We need to read the external data pointer
      uint32_t ext_page_no = mach_read_from_4(data_ptr + 4);
      compressed_data = read_sdi_blob(ext_page_no, comp_len);
      if (compressed_data.empty()) {
        std::cerr << "Failed to read external SDI BLOB" << std::endl;
        continue;
      }
    }

    // Decompress the SDI data using zlib
    std::vector<byte> uncompressed(uncomp_len + 1);
    uLongf dest_len = uncomp_len;

    int zret = uncompress(uncompressed.data(), &dest_len,
                          (const Bytef*)compressed_data.data(), comp_len);
    if (zret != Z_OK) {
      std::cerr << "Failed to decompress SDI data (zlib error: " << zret << ")" << std::endl;
      continue;
    }

    uncompressed[dest_len] = '\0';

    // Add to JSON output
    if (!first_record) {
      json_output += ",\n";
    }
    first_record = false;

    // Build the JSON object matching ibd2sdi output format
    json_output += "{\n";
    json_output += "\"type\": " + std::to_string(sdi_type) + ",\n";
    json_output += "\"id\": " + std::to_string(sdi_id) + ",\n";
    json_output += "\"object\": ";
    json_output += (char*)uncompressed.data();
    json_output += "\n}";
  }

  json_output += "\n]";
  free(sdi_buf);

  fd = saved_fd;

  if (first_record) {
    std::cerr << "No SDI records found" << std::endl;
    return 1;
  }

  return 0;
}
