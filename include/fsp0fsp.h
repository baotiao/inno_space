#ifndef inno_space_fsp_fsp_h
#define inno_space_fsp_fsp_h

#include "include/udef.h"

#include "include/fil0types.h"
#include "include/fut0lst.h"

/* The data structures in files are defined just as byte strings in C */
typedef byte fsp_header_t;
typedef byte xdes_t;

/** Offset of the space header within a file page */
#define FSP_HEADER_OFFSET FIL_PAGE_DATA

/** The number of bytes required to store SDI root page number(4)
 * and SDI version(4) at Page 0 */
#define FSP_SDI_HEADER_LEN 8

/*			SPACE HEADER
                        ============

File space header data structure: this data structure is contained in the
first page of a space. The space for this header is reserved in every extent
descriptor page, but used only in the first. */

/*-------------------------------------*/
#define FSP_SPACE_ID 0 /* space id */
#define FSP_NOT_USED                      \
  4 /* this field contained a value up to \
    which we know that the modifications  \
    in the database have been flushed to  \
    the file space; not used now */
#define FSP_SIZE                    \
  8 /* Current size of the space in \
    pages */
#define FSP_FREE_LIMIT                       \
  12 /* Minimum page number for which the    \
     free list has not been initialized:     \
     the pages >= this limit are, by         \
     definition, free; note that in a        \
     single-table tablespace where size      \
     < 64 pages, this number is 64, i.e.,    \
     we have initialized the space           \
     about the first extent, but have not    \
     physically allocated those pages to the \
     file */
#define FSP_SPACE_FLAGS               \
  16 /* fsp_space_t.flags, similar to \
     dict_table_t::flags */
#define FSP_FRAG_N_USED                            \
  20                /* number of used pages in the \
                    FSP_FREE_FRAG list */
#define FSP_FREE 24 /* list of free extents */
#define FSP_FREE_FRAG (24 + FLST_BASE_NODE_SIZE)
  /* list of partially free extents not
  belonging to any segment */
#define FSP_FULL_FRAG (24 + 2 * FLST_BASE_NODE_SIZE)
  /* list of full extents not belonging
  to any segment */
#define FSP_SEG_ID (24 + 3 * FLST_BASE_NODE_SIZE)
  /* 8 bytes which give the first unused
  segment id */
#define FSP_SEG_INODES_FULL (32 + 3 * FLST_BASE_NODE_SIZE)
  /* list of pages containing segment
  headers, where all the segment inode
  slots are reserved */
#define FSP_SEG_INODES_FREE (32 + 4 * FLST_BASE_NODE_SIZE)
  /* list of pages containing segment
  headers, where not all the segment
  header slots are reserved */
/*-------------------------------------*/
/* File space header size */
#define FSP_HEADER_SIZE (32 + 5 * FLST_BASE_NODE_SIZE)

#define FSP_FREE_ADD                    \
  4 /* this many free extents are added \
    to the free list from above         \
    FSP_FREE_LIMIT at a time */
/* @} */

/* @defgroup File Segment Inode Constants (moved from fsp0fsp.c) @{ */

/*			FILE SEGMENT INODE
                        ==================

Segment inode which is created for each segment in a tablespace. NOTE: in
purge we assume that a segment having only one currently used page can be
freed in a few steps, so that the freeing cannot fill the file buffer with
bufferfixed file pages. */

typedef byte fseg_inode_t;

#define FSEG_INODE_PAGE_NODE FSEG_PAGE_DATA
/* the list node for linking
segment inode pages */

#define FSEG_ARR_OFFSET (FSEG_PAGE_DATA + FLST_NODE_SIZE)
/*-------------------------------------*/
#define FSEG_ID                             \
  0 /* 8 bytes of segment id: if this is 0, \
    it means that the header is unused */
#define FSEG_NOT_FULL_N_USED 8
/* number of used segment pages in
the FSEG_NOT_FULL list */
#define FSEG_FREE 12
/* list of free extents of this
segment */
#define FSEG_NOT_FULL (12 + FLST_BASE_NODE_SIZE)
/* list of partially free extents */
#define FSEG_FULL (12 + 2 * FLST_BASE_NODE_SIZE)
/* list of full extents */
#define FSEG_MAGIC_N (12 + 3 * FLST_BASE_NODE_SIZE)
/* magic number used in debugging */
#define FSEG_FRAG_ARR (16 + 3 * FLST_BASE_NODE_SIZE)
/* array of individual pages
belonging to this segment in fsp
fragment extent lists */
#define FSEG_FRAG_ARR_N_SLOTS (FSP_EXTENT_SIZE / 2)
/* number of slots in the array for
the fragment pages */
#define FSEG_FRAG_SLOT_SIZE              \
  4 /* a fragment page slot contains its \
    page number within space, FIL_NULL   \
    means that the slot is not in use */
/*-------------------------------------*/
#define FSEG_INODE_SIZE \
  (16 + 3 * FLST_BASE_NODE_SIZE + FSEG_FRAG_ARR_N_SLOTS * FSEG_FRAG_SLOT_SIZE)

#define FSP_SEG_INODES_PER_PAGE(page_size) \
  ((page_size.physical() - FSEG_ARR_OFFSET - 10) / FSEG_INODE_SIZE)
/* Number of segment inodes which fit on a
single page */

#define FSEG_MAGIC_N_VALUE 97937874

#define FSEG_FILLFACTOR                  \
  8 /* If this value is x, then if       \
    the number of unused but reserved    \
    pages in a segment is less than      \
    reserved pages * 1/x, and there are  \
    at least FSEG_FRAG_LIMIT used pages, \
    then we allow a new empty extent to  \
    be added to the segment in           \
    fseg_alloc_free_page. Otherwise, we  \
    use unused pages of the segment. */

#define FSEG_FRAG_LIMIT FSEG_FRAG_ARR_N_SLOTS
/* If the segment has >= this many
used pages, it may be expanded by
allocating extents to the segment;
until that only individual fragment
pages are allocated from the space */

#define FSEG_FREE_LIST_LIMIT              \
  40 /* If the reserved size of a segment \
     is at least this many extents, we    \
     allow extents to be put to the free  \
     list of the extent: at most          \
     FSEG_FREE_LIST_MAX_LEN many */
#define FSEG_FREE_LIST_MAX_LEN 4
/* @} */

/* @defgroup Extent Descriptor Constants (moved from fsp0fsp.c) @{ */

/*			EXTENT DESCRIPTOR
                        =================

File extent descriptor data structure: contains bits to tell which pages in
the extent are free and which contain old tuple version to clean. */

/*-------------------------------------*/
#define XDES_ID                      \
  0 /* The identifier of the segment \
    to which this extent belongs */
#define XDES_FLST_NODE              \
  8 /* The list node data structure \
    for the descriptors */
#define XDES_STATE (FLST_NODE_SIZE + 8)
/* contains state information
of the extent */
#define XDES_BITMAP (FLST_NODE_SIZE + 12)
/* Descriptor bitmap of the pages
in the extent */
/*-------------------------------------*/

#define XDES_BITS_PER_PAGE 2 /* How many bits are there per page */
#define XDES_FREE_BIT                  \
  0 /* Index of the bit which tells if \
    the page is free */
#define XDES_CLEAN_BIT               \
  1 /* NOTE: currently not used!     \
    Index of the bit which tells if  \
    there are old versions of tuples \
    on the page */
/** States of a descriptor */
enum xdes_state_t {

  /** extent descriptor is not initialized */
  XDES_NOT_INITED = 0,

  /** extent is in free list of space */
  XDES_FREE = 1,

  /** extent is in free fragment list of space */
  XDES_FREE_FRAG = 2,

  /** extent is in full fragment list of space */
  XDES_FULL_FRAG = 3,

  /** extent belongs to a segment */
  XDES_FSEG = 4,

  /** fragment extent leased to segment */
  XDES_FSEG_FRAG = 5
};

/** File extent data structure size in bytes. */
#define XDES_SIZE \
  (XDES_BITMAP + UT_BITS_IN_BYTES(FSP_EXTENT_SIZE * XDES_BITS_PER_PAGE))

/** File extent data structure size in bytes for MAX page size. */
#define XDES_SIZE_MAX \
  (XDES_BITMAP + UT_BITS_IN_BYTES(FSP_EXTENT_SIZE_MAX * XDES_BITS_PER_PAGE))

/** File extent data structure size in bytes for MIN page size. */
#define XDES_SIZE_MIN \
  (XDES_BITMAP + UT_BITS_IN_BYTES(FSP_EXTENT_SIZE_MIN * XDES_BITS_PER_PAGE))

/** Offset of the descriptor array on a descriptor page */
#define XDES_ARR_OFFSET (FSP_HEADER_OFFSET + FSP_HEADER_SIZE)

/** The number of reserved pages in a fragment extent. */
const ulint XDES_FRAG_N_USED = 2;

/** A wrapper class to operate on a file segment inode pointer (fseg_inode_t*)
 */
class File_segment_inode {
 public:
  /** Constructor
   @param[in]   space_id  Table space identifier
   @param[in]   inode     File segment inode pointer */
  File_segment_inode(space_id_t space_id,
                     fseg_inode_t *inode)
      : m_space_id(space_id),
        m_fseg_inode(inode)
  {
  }

  /** Get the current value of FSEG_NOT_FULL_N_USED.
   @return the current value of FSEG_NOT_FULL_N_USED. */
  uint32_t read_not_full_n_used() const;

  /** Get the segment identifier value.
   @return the segment identifier value. */
  uint64_t get_seg_id() const {
    return (mach_read_from_8(m_fseg_inode + FSEG_ID));
  }

 private:
  /** Unique tablespace identifier */
  space_id_t m_space_id;

  /** file segment inode pointer that is being wrapped by this object. */
  fseg_inode_t *m_fseg_inode;
};

/* @} */
#endif
