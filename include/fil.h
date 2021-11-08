
#ifndef inno_space_fil_h
#define inno_space_fil_h


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

const uint32_t FIL_PAGE_DATA = 38;

#endif
