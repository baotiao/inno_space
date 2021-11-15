
#include "include/ut0crc32.h"
#include "include/fil0fil.h"
#include "include/fil0types.h"

/** Calculates the CRC32 checksum of a page. The value is stored to the page
when it is written to a file and also checked for a match when reading from
the file. When reading we allow both normal CRC32 and CRC-legacy-big-endian
variants. Note that we must be careful to calculate the same value on 32-bit
and 64-bit architectures.
@param[in]  page      buffer page (UNIV_PAGE_SIZE bytes)
@param[in]  use_legacy_big_endian if true then use big endian
byteorder when converting byte strings to integers
@return checksum */
uint32_t buf_calc_page_crc32(const byte *page,
                             bool use_legacy_big_endian /* = false */) {
  /* Since the field FIL_PAGE_FILE_FLUSH_LSN, and in versions <= 4.1.x
  FIL_PAGE_ARCH_LOG_NO_OR_SPACE_ID, are written outside the buffer pool
  to the first pages of data files, we have to skip them in the page
  checksum calculation.
  We must also skip the field FIL_PAGE_SPACE_OR_CHKSUM where the
  checksum is stored, and also the last 8 bytes of page because
  there we store the old formula checksum. */

  ut_crc32_func_t crc32_func =
      use_legacy_big_endian ? ut_crc32_legacy_big_endian : ut_crc32;

  const uint32_t c1 = ut_crc32(page + FIL_PAGE_OFFSET,
                                 FIL_PAGE_FILE_FLUSH_LSN - FIL_PAGE_OFFSET);

  const uint32_t c2 =
      crc32_func(page + FIL_PAGE_DATA,
                 UNIV_PAGE_SIZE - FIL_PAGE_DATA - FIL_PAGE_END_LSN_OLD_CHKSUM);

  return (c1 ^ c2);
}
