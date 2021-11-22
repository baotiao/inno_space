#include "include/fsp0fsp.h"



uint32_t File_segment_inode::read_not_full_n_used() const {
  uint32_t n_used =
      mach_read_from_4(m_fseg_inode + FSEG_NOT_FULL_N_USED);
  return (n_used);
}

