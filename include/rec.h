/*****************************************************************************

Copyright (c) 1994, 2021, Oracle and/or its affiliates.

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License, version 2.0, as published by the
Free Software Foundation.

This program is also distributed with certain software (including but not
limited to OpenSSL) that is licensed under separate terms, as designated in a
particular file or component or in included license documentation. The authors
of MySQL hereby grant you an additional permission to link the program and
your derivative works with the separately licensed software that they have
included with MySQL.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License, version 2.0,
for more details.

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA

*****************************************************************************/

/** @file rem/rec.h
 Record manager

 Created 5/30/1994 Heikki Tuuri
 *************************************************************************/

/** NOTE: The functions in this file should only use functions from
other files in library. The code in this file is used to make a library for
external tools. */

#ifndef rem_rec_h
#define rem_rec_h

#include "include/mach_data.h"


/* Compact flag ORed to the extra size returned by rec_get_offsets() */
#define REC_OFFS_COMPACT ((ulint)1 << 31)
/* SQL NULL flag in offsets returned by rec_get_offsets() */
#define REC_OFFS_SQL_NULL ((ulint)1 << 31)
/* External flag in offsets returned by rec_get_offsets() */
#define REC_OFFS_EXTERNAL ((ulint)1 << 30)
/* Default value flag in offsets returned by rec_get_offsets() */
#define REC_OFFS_DEFAULT ((ulint)1 << 29)
/* Mask for offsets returned by rec_get_offsets() */
#define REC_OFFS_MASK (REC_OFFS_DEFAULT - 1)

/* The offset of heap_no in a compact record */
#define REC_NEW_HEAP_NO 4
/* The shift of heap_no in a compact record.
The status is stored in the low-order bits. */
#define REC_HEAP_NO_SHIFT 3

/* We list the byte offsets from the origin of the record, the mask,
and the shift needed to obtain each bit-field of the record. */

#define REC_NEXT 2
#define REC_NEXT_MASK 0xFFFFUL
#define REC_NEXT_SHIFT 0

#define REC_OLD_SHORT 3 /* This is single byte bit-field */
#define REC_OLD_SHORT_MASK 0x1UL
#define REC_OLD_SHORT_SHIFT 0

#define REC_OLD_N_FIELDS 4
#define REC_OLD_N_FIELDS_MASK 0x7FEUL
#define REC_OLD_N_FIELDS_SHIFT 1

#define REC_NEW_STATUS 3 /* This is single byte bit-field */
#define REC_NEW_STATUS_MASK 0x7UL
#define REC_NEW_STATUS_SHIFT 0

#define REC_OLD_HEAP_NO 5
#define REC_HEAP_NO_MASK 0xFFF8UL
#if 0 /* defined in rem0rec.h for use of page0zip.cc */
#define REC_NEW_HEAP_NO 4
#define REC_HEAP_NO_SHIFT 3
#endif

#define REC_OLD_N_OWNED 6 /* This is single byte bit-field */
#define REC_NEW_N_OWNED 5 /* This is single byte bit-field */
#define REC_N_OWNED_MASK 0xFUL
#define REC_N_OWNED_SHIFT 0

#define REC_OLD_INFO_BITS 6 /* This is single byte bit-field */
#define REC_NEW_INFO_BITS 5 /* This is single byte bit-field */
#define REC_TMP_INFO_BITS 1 /* This is single byte bit-field */
#define REC_INFO_BITS_MASK 0xF0UL
#define REC_INFO_BITS_SHIFT 0

#if REC_OLD_SHORT_MASK << (8 * (REC_OLD_SHORT - 3)) ^       \
    REC_OLD_N_FIELDS_MASK << (8 * (REC_OLD_N_FIELDS - 4)) ^ \
    REC_HEAP_NO_MASK << (8 * (REC_OLD_HEAP_NO - 4)) ^       \
    REC_N_OWNED_MASK << (8 * (REC_OLD_N_OWNED - 3)) ^       \
    REC_INFO_BITS_MASK << (8 * (REC_OLD_INFO_BITS - 3)) ^ 0xFFFFFFFFUL
#error "sum of old-style masks != 0xFFFFFFFFUL"
#endif
#if REC_NEW_STATUS_MASK << (8 * (REC_NEW_STATUS - 3)) ^ \
    REC_HEAP_NO_MASK << (8 * (REC_NEW_HEAP_NO - 4)) ^   \
    REC_N_OWNED_MASK << (8 * (REC_NEW_N_OWNED - 3)) ^   \
    REC_INFO_BITS_MASK << (8 * (REC_NEW_INFO_BITS - 3)) ^ 0xFFFFFFUL
#error "sum of new-style masks != 0xFFFFFFUL"
#endif

/* Info bit denoting the predefined minimum record: this bit is set
if and only if the record is the first user record on a non-leaf
B-tree page that is the leftmost page on its level
(PAGE_LEVEL is nonzero and FIL_PAGE_PREV is FIL_NULL). */
#define REC_INFO_MIN_REC_FLAG 0x10UL
/* The deleted flag in info bits */
#define REC_INFO_DELETED_FLAG                  \
  0x20UL /* when bit is set to 1, it means the \
         record has been delete marked */
/* The 0x40UL can also be used in the future */
/* The instant ADD COLUMN flag. When it is set to 1, it means this record
was inserted/updated after an instant ADD COLUMN. */
#define REC_INFO_INSTANT_FLAG 0x80UL

/* Number of extra bytes in an old-style record,
in addition to the data and the offsets */
#define REC_N_OLD_EXTRA_BYTES 6
/* Number of extra bytes in a new-style record,
in addition to the data and the offsets */
#define REC_N_NEW_EXTRA_BYTES 5
/* NUmber of extra bytes in a new-style temporary record,
in addition to the data and the offsets.
This is used only after instant ADD COLUMN. */
#define REC_N_TMP_EXTRA_BYTES 1

/* Record status values */
#define REC_STATUS_ORDINARY 0
#define REC_STATUS_NODE_PTR 1
#define REC_STATUS_INFIMUM 2
#define REC_STATUS_SUPREMUM 3

/* The following four constants are needed in page0zip.cc in order to
efficiently compress and decompress pages. */

/* Length of a B-tree node pointer, in bytes */
#define REC_NODE_PTR_SIZE 4

/** SQL null flag in a 1-byte offset of ROW_FORMAT=REDUNDANT records */
#define REC_1BYTE_SQL_NULL_MASK 0x80UL
/** SQL null flag in a 2-byte offset of ROW_FORMAT=REDUNDANT records */
#define REC_2BYTE_SQL_NULL_MASK 0x8000UL

/** In a 2-byte offset of ROW_FORMAT=REDUNDANT records, the second most
significant bit denotes that the tail of a field is stored off-page. */
#define REC_2BYTE_EXTERN_MASK 0x4000UL

#ifdef UNIV_DEBUG
/* Length of the rec_get_offsets() header */
#define REC_OFFS_HEADER_SIZE 4
#else /* UNIV_DEBUG */
/* Length of the rec_get_offsets() header */
#define REC_OFFS_HEADER_SIZE 2
#endif /* UNIV_DEBUG */

/* Number of elements that should be initially allocated for the
offsets[] array, first passed to rec_get_offsets() */
#define REC_OFFS_NORMAL_SIZE 100
#define REC_OFFS_SMALL_SIZE 10

/* Get the base address of offsets.  The extra_size is stored at
this position, and following positions hold the end offsets of
the fields. */
#define rec_offs_base(offsets) (offsets + REC_OFFS_HEADER_SIZE)

/** Number of fields flag which means it occupies two bytes */
static const uint8_t REC_N_FIELDS_TWO_BYTES_FLAG = 0x80;

/** Max number of fields which can be stored in one byte */
static const uint8_t REC_N_FIELDS_ONE_BYTE_MAX = 0x7F;

/** Gets a bit field from within 1 byte. */
static inline ulint rec_get_bit_field_1(
    const rec_t *rec, /*!< in: pointer to record origin */
    ulint offs,       /*!< in: offset from the origin down */
    ulint mask,       /*!< in: mask used to filter bits */
    ulint shift)      /*!< in: shift right applied after masking */
{
  ut_ad(rec);

  return ((mach_read_from_1(rec - offs) & mask) >> shift);
}

/** Gets a bit field from within 2 bytes. */
static inline uint16_t rec_get_bit_field_2(
    const rec_t *rec, /*!< in: pointer to record origin */
    ulint offs,       /*!< in: offset from the origin down */
    ulint mask,       /*!< in: mask used to filter bits */
    ulint shift)      /*!< in: shift right applied after masking */
{
  ut_ad(rec);

  return ((mach_read_from_2(rec - offs) & mask) >> shift);
}
/** The following function retrieves the status bits of a new-style record.
 @return status bits */
static inline ulint rec_get_status(
    const rec_t *rec) /*!< in: physical record */
{
  ulint ret;

  ut_ad(rec);

  ret = rec_get_bit_field_1(rec, REC_NEW_STATUS, REC_NEW_STATUS_MASK,
                            REC_NEW_STATUS_SHIFT);
  ut_ad((ret & ~REC_NEW_STATUS_MASK) == 0);

  return (ret);
}

#ifdef UNIV_DEBUG
/** Check if the info bits are valid.
@param[in]	bits	info bits to check
@return true if valid */
inline bool rec_info_bits_valid(ulint bits) {
  return (0 == (bits & ~(REC_INFO_DELETED_FLAG | REC_INFO_MIN_REC_FLAG |
                         REC_INFO_INSTANT_FLAG)));
}
#endif /* UNIV_DEBUG */

/** The following function is used to retrieve the info bits of a record.
@param[in]	rec	physical record
@param[in]	comp	nonzero=compact page format
@return info bits */
static inline ulint rec_get_info_bits(const rec_t *rec, ulint comp) {
  const ulint val =
      rec_get_bit_field_1(rec, comp ? REC_NEW_INFO_BITS : REC_OLD_INFO_BITS,
                          REC_INFO_BITS_MASK, REC_INFO_BITS_SHIFT);
  ut_ad(rec_info_bits_valid(val));
  return (val);
}

/** The following function is used to retrieve the info bits of a temporary
record.
@param[in]	rec	physical record
@return	info bits */
static inline ulint rec_get_info_bits_temp(const rec_t *rec) {
  const ulint val = rec_get_bit_field_1(
      rec, REC_TMP_INFO_BITS, REC_INFO_BITS_MASK, REC_INFO_BITS_SHIFT);
  ut_ad(rec_info_bits_valid(val));
  return (val);
}

/** The following function is used to get the number of fields
 in an old-style record, which is stored in the rec
 @return number of data fields */
static inline uint16_t rec_get_n_fields_old_raw(
    const rec_t *rec) /*!< in: physical record */
{
  uint16_t ret;

  ut_ad(rec);

  ret = rec_get_bit_field_2(rec, REC_OLD_N_FIELDS, REC_OLD_N_FIELDS_MASK,
                            REC_OLD_N_FIELDS_SHIFT);
  ut_ad(ret <= REC_MAX_N_FIELDS);
  ut_ad(ret > 0);

  return (ret);
}

#endif
