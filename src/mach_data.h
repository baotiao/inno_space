#ifndef inno_space_mach_data_h
#define inno_space_mach_data_h

#include <stdint.h>

uint16_t mach_read_from_2(const unsigned char *b) {
  return (((uint32_t)(b[0]) << 8) | (uint32_t)(b[1]));
}

uint32_t mach_read_from_4(const unsigned char *b) {
  return ((static_cast<uint32_t>(b[0]) << 24) |
      (static_cast<uint32_t>(b[1]) << 16) |
      (static_cast<uint32_t>(b[2]) << 8) | static_cast<uint32_t>(b[3]));
}
/** The following function is used to fetch data from 8 consecutive
 * bytes. The most significant byte is at the lowest address.
 * @param[in]  b pointer to 8 bytes from where read
 * @return 64-bit integer */
uint64_t mach_read_from_8(const unsigned char *b) {
  uint64_t u64;

  u64 = mach_read_from_4(b);
  u64 <<= 32;
  u64 |= mach_read_from_4(b + 4);

  return (u64);
}

#endif
