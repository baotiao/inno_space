
#ifndef inno_space_fil_fil_h
#define inno_space_fil_fil_h

#include "include/udef.h"
#include "include/api0api.h"
#include "include/my_inttypes.h"
#include "include/mach_data.h"

#include <limits>

/** The byte offsets on a file page for various variables. */

/** MySQL-4.0.14 space id the page belongs to (== 0) but in later
versions the 'new' checksum of the page */
#define FIL_PAGE_SPACE_OR_CHKSUM 0

/** page offset inside space */
#define FIL_PAGE_OFFSET 4

/** if there is a 'natural' predecessor of the page, its offset.
Otherwise FIL_NULL. This field is not set on BLOB pages, which are stored as a
singly-linked list. See also FIL_PAGE_NEXT. */
#define FIL_PAGE_PREV 8

/** On page 0 of the tablespace, this is the server version ID */
#define FIL_PAGE_SRV_VERSION 8

/** if there is a 'natural' successor of the page, its offset. Otherwise
FIL_NULL. B-tree index pages(FIL_PAGE_TYPE contains FIL_PAGE_INDEX) on the
same PAGE_LEVEL are maintained as a doubly linked list via FIL_PAGE_PREV and
FIL_PAGE_NEXT in the collation order of the smallest user record on each
page. */
#define FIL_PAGE_NEXT 12

/** On page 0 of the tablespace, this is the server version ID */
#define FIL_PAGE_SPACE_VERSION 12

/** lsn of the end of the newest modification log record to the page */
#define FIL_PAGE_LSN 16

/** file page type: FIL_PAGE_INDEX,..., 2 bytes. The contents of this field
can only be trusted in the following case: if the page is an uncompressed
B-tree index page, then it is guaranteed that the value is FIL_PAGE_INDEX.
The opposite does not hold.

In tablespaces created by MySQL/InnoDB 5.1.7 or later, the contents of this
field is valid for all uncompressed pages. */
#define FIL_PAGE_TYPE 24

/** this is only defined for the first page of the system tablespace: the file
has been flushed to disk at least up to this LSN. For FIL_PAGE_COMPRESSED
pages, we store the compressed page control information in these 8 bytes. */
#define FIL_PAGE_FILE_FLUSH_LSN 26

#define FSEG_PAGE_DATA FIL_PAGE_DATA

using page_type_t = uint16_t;

/** File page types (values of FIL_PAGE_TYPE) @{ */
/** B-tree node */
constexpr page_type_t FIL_PAGE_INDEX = 17855;

/** R-tree node */
constexpr page_type_t FIL_PAGE_RTREE = 17854;

/** Tablespace SDI Index page */
constexpr page_type_t FIL_PAGE_SDI = 17853;

/** Undo log page */
constexpr page_type_t FIL_PAGE_UNDO_LOG = 2;

/** Index node */
constexpr page_type_t FIL_PAGE_INODE = 3;

/** Insert buffer free list */
constexpr page_type_t FIL_PAGE_IBUF_FREE_LIST = 4;

/* File page types introduced in MySQL/InnoDB 5.1.7 */
/** Freshly allocated page */
constexpr page_type_t FIL_PAGE_TYPE_ALLOCATED = 0;

/** Insert buffer bitmap */
constexpr page_type_t FIL_PAGE_IBUF_BITMAP = 5;

/** System page */
constexpr page_type_t FIL_PAGE_TYPE_SYS = 6;

/** Transaction system data */
constexpr page_type_t FIL_PAGE_TYPE_TRX_SYS = 7;

/** File space header */
constexpr page_type_t FIL_PAGE_TYPE_FSP_HDR = 8;

/** Extent descriptor page */
constexpr page_type_t FIL_PAGE_TYPE_XDES = 9;

/** Uncompressed BLOB page */
constexpr page_type_t FIL_PAGE_TYPE_BLOB = 10;

/** First compressed BLOB page */
constexpr page_type_t FIL_PAGE_TYPE_ZBLOB = 11;

/** Subsequent compressed BLOB page */
constexpr page_type_t FIL_PAGE_TYPE_ZBLOB2 = 12;

/** In old tablespaces, garbage in FIL_PAGE_TYPE is replaced with
this value when flushing pages. */
constexpr page_type_t FIL_PAGE_TYPE_UNKNOWN = 13;

/** Compressed page */
constexpr page_type_t FIL_PAGE_COMPRESSED = 14;

/** Encrypted page */
constexpr page_type_t FIL_PAGE_ENCRYPTED = 15;

/** Compressed and Encrypted page */
constexpr page_type_t FIL_PAGE_COMPRESSED_AND_ENCRYPTED = 16;

/** Encrypted R-tree page */
constexpr page_type_t FIL_PAGE_ENCRYPTED_RTREE = 17;

/** Uncompressed SDI BLOB page */
constexpr page_type_t FIL_PAGE_SDI_BLOB = 18;

/** Commpressed SDI BLOB page */
constexpr page_type_t FIL_PAGE_SDI_ZBLOB = 19;

/** Available for future use */
constexpr page_type_t FIL_PAGE_TYPE_UNUSED = 20;

/** Rollback Segment Array page */
constexpr page_type_t FIL_PAGE_TYPE_RSEG_ARRAY = 21;

/** Index pages of uncompressed LOB */
constexpr page_type_t FIL_PAGE_TYPE_LOB_INDEX = 22;

/** Data pages of uncompressed LOB */
constexpr page_type_t FIL_PAGE_TYPE_LOB_DATA = 23;

/** The first page of an uncompressed LOB */
constexpr page_type_t FIL_PAGE_TYPE_LOB_FIRST = 24;

/** The first page of a compressed LOB */
constexpr page_type_t FIL_PAGE_TYPE_ZLOB_FIRST = 25;

/** Data pages of compressed LOB */
constexpr page_type_t FIL_PAGE_TYPE_ZLOB_DATA = 26;

/** Index pages of compressed LOB. This page contains an array of
z_index_entry_t objects.*/
constexpr page_type_t FIL_PAGE_TYPE_ZLOB_INDEX = 27;

/** Fragment pages of compressed LOB. */
constexpr page_type_t FIL_PAGE_TYPE_ZLOB_FRAG = 28;

/** Index pages of fragment pages (compressed LOB). */
constexpr page_type_t FIL_PAGE_TYPE_ZLOB_FRAG_ENTRY = 29;

/** Used by i_s.cc to index into the text description. */
constexpr page_type_t FIL_PAGE_TYPE_LAST = FIL_PAGE_TYPE_ZLOB_FRAG_ENTRY;

/** Check whether the page type is index (Btree or Rtree or SDI) type */
#define fil_page_type_is_index(page_type)                      \
  (page_type == FIL_PAGE_INDEX || page_type == FIL_PAGE_SDI || \
   page_type == FIL_PAGE_RTREE)

/** Check whether the page is index page (either regular Btree index or Rtree
index */
#define fil_page_index_page_check(page) \
  fil_page_type_is_index(fil_page_get_type(page))

/** Get the file page type.
 * @param[in]  page    File page
 * @return page type */
inline page_type_t fil_page_get_type(const byte *page) {
    return (static_cast<page_type_t>(mach_read_from_2(page + FIL_PAGE_TYPE)));
}


/** Get the predecessor of a file page.
 * @param[in]  page    File page
 * @return FIL_PAGE_PREV */
extern page_no_t fil_page_get_prev(const byte *page);

  /** Get the successor of a file page.
   * @param[in]  page    File page
   * @return FIL_PAGE_NEXT */
extern page_no_t fil_page_get_next(const byte *page);

/** Sets the file page type.
 * @param[in,out]  page    File page
 * @param[in]  type    File page type to set */
extern void fil_page_set_type(byte *page, ulint type);

/** An empty tablespace (CREATE TABLESPACE) has minimum
 * of 4 pages and an empty CREATE TABLE (file_per_table) has 6 pages.
 * Minimum of these two is 4 */
constexpr size_t FIL_IBD_FILE_INITIAL_SIZE_5_7 = 4;

/** 'null' (undefined) page offset in the context of file spaces */
constexpr page_no_t FIL_NULL = std::numeric_limits<page_no_t>::max();

/** Maximum Page Number, one less than FIL_NULL */
constexpr page_no_t PAGE_NO_MAX = std::numeric_limits<page_no_t>::max() - 1;

/** Unknown space id */
constexpr space_id_t SPACE_UNKNOWN = std::numeric_limits<space_id_t>::max();

using fil_faddr_t = byte;

/** File space address */
struct fil_addr_t {
  /* Default constructor */
  fil_addr_t() : page(FIL_NULL), boffset(0) {}

  /** Constructor
  @param[in]	p	Logical page number
  @param[in]	boff	Offset within the page */
  fil_addr_t(page_no_t p, uint32_t boff) : page(p), boffset(boff) {}

  /** Compare to instances
  @param[in]	rhs	Instance to compare with
  @return true if the page number and page offset are equal */
  bool is_equal(const fil_addr_t &rhs) const {
    return (page == rhs.page && boffset == rhs.boffset);
  }

  /** Check if the file address is null.
  @return true if null */
  bool is_null() const { return (page == FIL_NULL && boffset == 0); }

  /** Print a string representation.
  @param[in,out]	out		Stream to write to */
  std::ostream &print(std::ostream &out) const {
    out << "[fil_addr_t: page=" << page << ", boffset=" << boffset << "]";

    return (out);
  }

  /** Page number within a space */
  page_no_t page;

  /** Byte offset within the page */
  uint32_t boffset;
};

/* For printing fil_addr_t to a stream.
@param[in,out]	out		Stream to write to
@param[in]	obj		fil_addr_t instance to write */
inline std::ostream &operator<<(std::ostream &out, const fil_addr_t &obj) {
  return (obj.print(out));
}

/** The null file address */
extern fil_addr_t fil_addr_null;
#endif
