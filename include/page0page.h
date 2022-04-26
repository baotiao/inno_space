
#ifndef inno_space_page_page_h
#define inno_space_page_page_h

#include "include/fil0fil.h"
#include "include/page0types.h"
#include "include/fil0types.h"

/*			PAGE HEADER
        ===========

Index page header starts at the first offset left free by the FIL-module */


#define PAGE_HEADER                                  \
  FSEG_PAGE_DATA /* index page header starts at this \
         offset */
/*-----------------------------*/
#define PAGE_N_DIR_SLOTS 0 /* number of slots in page directory */
#define PAGE_HEAP_TOP 2    /* pointer to record heap top */
#define PAGE_N_HEAP                                      \
  4                    /* number of records in the heap, \
                       bit 15=flag: new-style compact page format */
#define PAGE_FREE 6    /* pointer to start of page free record list */
#define PAGE_GARBAGE 8 /* number of bytes in deleted records */
#define PAGE_LAST_INSERT                                                \
  10                      /* pointer to the last inserted record, or    \
                          NULL if this info has been reset by a delete, \
                          for example */
#define PAGE_DIRECTION 12 /* last insert direction: PAGE_LEFT, ... */
#define PAGE_N_DIRECTION                                            \
  14                   /* number of consecutive inserts to the same \
                       direction */
#define PAGE_N_RECS 16 /* number of user records on the page */
#define PAGE_MAX_TRX_ID                             \
  18 /* highest id of a trx which may have modified \
     a record on the page; trx_id_t; defined only   \
     in secondary indexes and in the insert buffer  \
     tree */
#define PAGE_HEADER_PRIV_END                      \
  26 /* end of private data structure of the page \
     header which are set in a page create */
/*----*/
#define PAGE_LEVEL                                 \
  26 /* level of the node in an index tree; the    \
     leaf level is the level 0.  This field should \
     not be written to after page creation. */
#define PAGE_INDEX_ID                          \
  28 /* index id where the page belongs.       \
     This field should not be written to after \
     page creation. */

#define PAGE_BTR_SEG_LEAF                         \
  36 /* file segment header for the leaf pages in \
     a B-tree: defined only on the root page of a \
     B-tree, but not in the root of an ibuf tree */


/** The structure of a BLOB part header */
/* @{ */
/*--------------------------------------*/
#define BTR_BLOB_HDR_PART_LEN		0	/*!< BLOB part len on this
						page */
#define BTR_BLOB_HDR_NEXT_PAGE_NO	4	/*!< next BLOB part page no,
						FIL_NULL if none */
/*--------------------------------------*/
#define BTR_BLOB_HDR_SIZE		8	/*!< Size of a BLOB
						part header, in bytes */

#define UNIV_PAGE_SIZE (16 * 1024)

/** Gets the page number.
 @return page number */
page_no_t page_get_page_no(const page_t *page); /*!< in: page */

/** Gets the tablespace identifier.
 @return space id */
space_id_t page_get_space_id(const page_t *page); /*!< in: page */

page_t *align_page(const void* ptr);

#endif
