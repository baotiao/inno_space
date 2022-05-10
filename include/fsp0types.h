
#ifndef inno_space_fsp_types_h
#define inno_space_fsp_types_h

#include "include/udef.h"

/** @name Flags for inserting records in order
If records are inserted in order, there are the following
flags to tell this (their type is made byte for the compiler
to warn if direction and hint parameters are switched in
fseg_alloc_free_page) */
/* @{ */
#define FSP_UP ((byte)111)     /*!< alphabetically upwards */
#define FSP_DOWN ((byte)112)   /*!< alphabetically downwards */
#define FSP_NO_DIR ((byte)113) /*!< no order */
/* @} */

/** File space extent size in pages
page size | file space extent size
----------+-----------------------
   4 KiB  | 256 pages = 1 MiB
   8 KiB  | 128 pages = 1 MiB
  16 KiB  |  64 pages = 1 MiB
  32 KiB  |  64 pages = 2 MiB
  64 KiB  |  64 pages = 4 MiB
*/
#define FSP_EXTENT_SIZE                                                 \
  static_cast<page_no_t>(                                               \
      ((UNIV_PAGE_SIZE <= (16384)                                       \
            ? (1048576 / UNIV_PAGE_SIZE)                                \
            : ((UNIV_PAGE_SIZE <= (32768)) ? (2097152 / UNIV_PAGE_SIZE) \
                                           : (4194304 / UNIV_PAGE_SIZE)))))

/** File space extent size (four megabyte) in pages for MAX page size */
#define FSP_EXTENT_SIZE_MAX (4194304 / UNIV_PAGE_SIZE_MAX)

/** File space extent size (one megabyte) in pages for MIN page size */
#define FSP_EXTENT_SIZE_MIN (1048576 / UNIV_PAGE_SIZE_MIN)

/** On a page of any file segment, data may be put starting from this
offset */
#define FSEG_PAGE_DATA FIL_PAGE_DATA

/** @name File segment header
The file segment header points to the inode describing the file segment. */
/* @{ */
/** Data type for file segment header */
typedef byte fseg_header_t;

#define FSEG_HDR_SPACE 0   /*!< space id of the inode */
#define FSEG_HDR_PAGE_NO 4 /*!< page number of the inode */
#define FSEG_HDR_OFFSET 8  /*!< byte offset of the inode */

#define FSEG_HEADER_SIZE            \
  10 /*!< Length of the file system \
     header, in bytes */
/* @} */

/** The structure of undo page header */
#define FSP_RSEG_ARRAY_PAGE_NO      \
  3 /*!< rollback segment directory \
    page number in each undo tablespace */


#endif
