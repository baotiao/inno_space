
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include <sys/stat.h>

#include <cstdlib>
#include <iostream>

#include "include/fil.h"
#include "include/page.h"
#include "include/ut0crc32.h"
#include "include/page_crc32.h"
#include "mach_data.h"

static const uint32_t kPageSize = 16384;

char path[1024];
int fd;

unsigned char* read_buf;

static void usage()
{
  fprintf(stderr,
      "Inno space\n"
      "usage: inno [-hv] [-f test/t.ibd]\n"
      "\t-h                -- show this help\n"
      "\t-f test/t.ibd     -- ibd file \n"
      "\t-p                -- page number \n"
      "./inno -f ~/git/primary/dbs2250/test/t1.ibd -p 2\n"
      );
}



void ShowFILHeader(uint32_t page_num) {

  printf("==========================block==========================\n");
  printf("FIL Header:\n");
  uint64_t offset = kPageSize * page_num;

  int ret = pread(fd, read_buf, kPageSize, offset);

  printf("CheckSum: %u\n", mach_read_from_4(read_buf));

  uint32_t cc = buf_calc_page_crc32(read_buf, 0);
  printf("crc %u\n", cc);

  printf("Offset: %u\n", mach_read_from_4(read_buf + FIL_PAGE_OFFSET));
  printf("Previous Page: %u\n", mach_read_from_4(read_buf + FIL_PAGE_PREV));
  printf("Next Page: %u\n", mach_read_from_4(read_buf + FIL_PAGE_NEXT));
  printf("Page LSN: %lu\n", mach_read_from_8(read_buf + FIL_PAGE_LSN));
  printf("Page Type: %hu\n", mach_read_from_2(read_buf + FIL_PAGE_TYPE));
  printf("Flush LSN: %lu\n", mach_read_from_8(read_buf + FIL_PAGE_FILE_FLUSH_LSN));
}

void ShowIndexHeader(uint32_t page_num) {
  printf("Index Header:\n");
  uint64_t offset = kPageSize * page_num;

  int ret = pread(fd, read_buf, kPageSize, offset);

  printf("Number of Directory Slots: %hu\n", mach_read_from_2(read_buf + PAGE_HEADER));
  printf("Garbage Space: %hu\n", mach_read_from_2(read_buf + PAGE_HEADER + PAGE_GARBAGE));
  printf("Number of Records: %hu\n", mach_read_from_2(read_buf + PAGE_HEADER + PAGE_N_RECS));
  printf("Max Trx id: %lu\n", mach_read_from_8(read_buf + PAGE_HEADER + PAGE_MAX_TRX_ID));
  printf("Page level: %hu\n", mach_read_from_2(read_buf + PAGE_HEADER + PAGE_LEVEL));
  printf("Index ID: %lu\n", mach_read_from_8(read_buf + PAGE_HEADER + PAGE_INDEX_ID));

}

void ShowFile() {
  struct stat stat_buf;
  int rc = fstat(fd, &stat_buf);
  printf("File size %lu\n", stat_buf.st_size);

  int block_num = stat_buf.st_size / kPageSize;

  for (int i = 0; i < block_num; i++) {
    ShowFILHeader(i);
    ShowIndexHeader(i);
  }
}

void ModifyPage() {
}

void DeletePage(uint32_t page_num) {
  printf("==========================DeletePage==========================\n");
  uint64_t offset = kPageSize * page_num;

  int ret = pread(fd, read_buf, kPageSize, offset);

  printf("CheckSum: %u\n", mach_read_from_4(read_buf));

  uint32_t cc = buf_calc_page_crc32(read_buf, 0);
  printf("crc %u\n", cc);
  unsigned char prev_buf[16 * 1024];
  unsigned char next_buf[16 * 1024];
  uint32_t prev_page = mach_read_from_4(read_buf + FIL_PAGE_PREV);
  uint32_t next_page = mach_read_from_4(read_buf + FIL_PAGE_NEXT);
  pread(fd, prev_buf, kPageSize, kPageSize * prev_page);
  pread(fd, next_buf, kPageSize, kPageSize * next_page);

  
  mach_write_to_4(prev_buf + FIL_PAGE_NEXT, next_page);
  mach_write_to_4(next_buf + FIL_PAGE_PREV, prev_page);
  printf("prev_page %u next_page %u\n", prev_page, next_page);

  uint32_t prev_cc = buf_calc_page_crc32(prev_buf, 0);
  uint32_t next_cc = buf_calc_page_crc32(next_buf, 0);

  mach_write_to_4(prev_buf, prev_cc);
  mach_write_to_4(next_buf, next_cc);

  mach_write_to_4(prev_buf + UNIV_PAGE_SIZE - FIL_PAGE_END_LSN_OLD_CHKSUM,
      prev_cc);

  mach_write_to_4(next_buf + UNIV_PAGE_SIZE - FIL_PAGE_END_LSN_OLD_CHKSUM,
      next_cc);

  ret = pwrite(fd, prev_buf, kPageSize, kPageSize * prev_page);
  pwrite(fd, next_buf, kPageSize, kPageSize * next_page);
  
  printf("modify ret %d\n");

}

int main(int argc, char *argv[]) {
  if (argc <= 2) {
    usage();
    exit(-1);
  }

  uint32_t user_page = 0;
  bool path_opt = false;
  char c;
  bool show_file = true;
  bool delete_page = false;
  while (-1 != (c = getopt(argc, argv, "hf:p:d:"))) {
    switch (c) {
      case 'f':
        snprintf(path, 1024, "%s", optarg);
        path_opt = true;
        break;
      case 'p':
        show_file = false;
        user_page = std::atol(optarg);
        break;
      case 'd':
        show_file = false;
        delete_page = true;
        user_page = std::atoll(optarg);
        break;
      case 'h':
        usage();
        return 0;
      default:
        usage();
        return 0;
    }
  }

  if (path_opt == false) {
    fprintf (stderr, "Please specify the ibd file path\n" );
    usage();
    exit(-1);
  }

  printf("File path %s path, page num %ld\n", path, user_page);

  printf("page num %ld\n", user_page);
  fd = open(path, O_RDWR, 0644); 
  if (fd == -1) {
    fprintf(stderr, "[ERROR] Open %s failed: %s\n", path, strerror(errno));
    exit(1);
  }

  ut_crc32_init();

  posix_memalign((void**)&read_buf, kPageSize, kPageSize);

  if (show_file == true) {
    ShowFile();
  }

  ShowFILHeader(user_page);

  ShowIndexHeader(user_page);

  if (delete_page) {
    DeletePage(user_page);
  }

  return 0;
}
