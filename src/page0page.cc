#include "include/page0page.h"

/** Gets the page number.
 @return page number */
page_no_t page_get_page_no(const page_t *page) /*!< in: page */
{
  return (mach_read_from_4(page + FIL_PAGE_OFFSET));
}

/** Gets the tablespace identifier.
 @return space id */
space_id_t page_get_space_id(const page_t *page) /*!< in: page */
{
  return (mach_read_from_4(page + FIL_PAGE_ARCH_LOG_NO_OR_SPACE_ID));
}


page_t *align_page(const void* ptr) {
  return((page_t*)(((reinterpret_cast<uint64_t>(ptr))) & ~(UNIV_PAGE_SIZE - 1)));
}
