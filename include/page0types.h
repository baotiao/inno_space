
#ifndef page0types_h
#define page0types_h

#include "include/udef.h"
#include <map>

/*			PAGE HEADER
                        ===========

Index page header starts at the first offset left free by the FIL-module */

typedef byte page_header_t;

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
#define PAGE_BTR_IBUF_FREE_LIST PAGE_BTR_SEG_LEAF
#define PAGE_BTR_IBUF_FREE_LIST_NODE PAGE_BTR_SEG_LEAF
/* in the place of PAGE_BTR_SEG_LEAF and _TOP
there is a free list base node if the page is
the root page of an ibuf tree, and at the same
place is the free list node if the page is in
a free list */
#define PAGE_BTR_SEG_TOP (36 + FSEG_HEADER_SIZE)
/* file segment header for the non-leaf pages
in a B-tree: defined only on the root page of
a B-tree, but not in the root of an ibuf
tree */
/*----*/
#define PAGE_DATA (PAGE_HEADER + 36 + 2 * FSEG_HEADER_SIZE)
/* start of data on the page */

#define PAGE_OLD_INFIMUM (PAGE_DATA + 1 + REC_N_OLD_EXTRA_BYTES)
/* offset of the page infimum record on an
old-style page */
#define PAGE_OLD_SUPREMUM (PAGE_DATA + 2 + 2 * REC_N_OLD_EXTRA_BYTES + 8)
/* offset of the page supremum record on an
old-style page */
#define PAGE_OLD_SUPREMUM_END (PAGE_OLD_SUPREMUM + 9)
/* offset of the page supremum record end on
an old-style page */
#define PAGE_NEW_INFIMUM (PAGE_DATA + REC_N_NEW_EXTRA_BYTES)
/* offset of the page infimum record on a
new-style compact page */
#define PAGE_NEW_SUPREMUM (PAGE_DATA + 2 * REC_N_NEW_EXTRA_BYTES + 8)
/* offset of the page supremum record on a
new-style compact page */
#define PAGE_NEW_SUPREMUM_END (PAGE_NEW_SUPREMUM + 8)
/* offset of the page supremum record end on
a new-style compact page */
/*-----------------------------*/

/* Heap numbers */
#define PAGE_HEAP_NO_INFIMUM 0  /* page infimum */
#define PAGE_HEAP_NO_SUPREMUM 1 /* page supremum */
#define PAGE_HEAP_NO_USER_LOW        \
  2 /* first user record in          \
    creation (insertion) order,      \
    not necessarily collation order; \
    this record may have been deleted */

/* Directions of cursor movement */
#define PAGE_LEFT 1
#define PAGE_RIGHT 2
#define PAGE_SAME_REC 3
#define PAGE_SAME_PAGE 4
#define PAGE_NO_DIRECTION 5

/** Eliminates a name collision on HP-UX */
#define page_t ib_page_t
/** Type of the index page */
typedef byte page_t;
/** Index page cursor */
struct page_cur_t;

/** Compressed index page */
typedef byte page_zip_t;

/* The following definitions would better belong to page0zip.h,
but we cannot include page0zip.h from rem0rec.ic, because
page0*.h includes rem0rec.h and may include rem0rec.ic. */

/** Number of bits needed for representing different compressed page sizes */
#define PAGE_ZIP_SSIZE_BITS 3

/** Maximum compressed page shift size */
#define PAGE_ZIP_SSIZE_MAX \
  (UNIV_ZIP_SIZE_SHIFT_MAX - UNIV_ZIP_SIZE_SHIFT_MIN + 1)

/* Make sure there are enough bits available to store the maximum zip
ssize, which is the number of shifts from 512. */
#if PAGE_ZIP_SSIZE_MAX >= (1 << PAGE_ZIP_SSIZE_BITS)
#error "PAGE_ZIP_SSIZE_MAX >= (1 << PAGE_ZIP_SSIZE_BITS)"
#endif

/* Page cursor search modes; the values must be in this order! */
enum page_cur_mode_t {
  PAGE_CUR_UNSUPP = 0,
  PAGE_CUR_G = 1,
  PAGE_CUR_GE = 2,
  PAGE_CUR_L = 3,
  PAGE_CUR_LE = 4,

  /*      PAGE_CUR_LE_OR_EXTENDS = 5,*/ /* This is a search mode used in
                                   "column LIKE 'abc%' ORDER BY column DESC";
                                   we have to find strings which are <= 'abc' or
                                   which extend it */

  /* These search mode is for search R-tree index. */
  PAGE_CUR_CONTAIN = 7,
  PAGE_CUR_INTERSECT = 8,
  PAGE_CUR_WITHIN = 9,
  PAGE_CUR_DISJOINT = 10,
  PAGE_CUR_MBR_EQUAL = 11,
  PAGE_CUR_RTREE_INSERT = 12,
  PAGE_CUR_RTREE_LOCATE = 13,
  PAGE_CUR_RTREE_GET_FATHER = 14
};

#endif
