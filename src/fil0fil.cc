
#include "include/fil0fil.h"

/** Get the predecessor of a file page.
 * @param[in]  page    File page
 * @return FIL_PAGE_PREV */
extern page_no_t fil_page_get_prev(const byte *page) {
    return (mach_read_from_4(page + FIL_PAGE_PREV));
}

/** Get the successor of a file page.
 * @param[in]  page    File page
 * @return FIL_PAGE_NEXT */
extern page_no_t fil_page_get_next(const byte *page) {
    return (mach_read_from_4(page + FIL_PAGE_NEXT));
}

/** Sets the file page type.
 * @param[in,out]  page    File page
 * @param[in]  type    Page type */
extern void fil_page_set_type(byte *page, ulint type) {
    mach_write_to_2(page + FIL_PAGE_TYPE, type);
}
