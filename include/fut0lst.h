/*****************************************************************************

Copyright (c) 1995, 2021, Oracle and/or its affiliates.

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

/** @file include/fut0lst.h
 File-based list utilities

 Created 11/28/1995 Heikki Tuuri
 ***********************************************************************/

#ifndef fut0lst_h
#define fut0lst_h

#include "include/udef.h"

#include "include/fil0fil.h"
#include "include/fil0types.h"

/* We define the field offsets of a node for the list */
#define FLST_PREV                                   \
  0 /* 6-byte address of the previous list element; \
    the page part of address is FIL_NULL, if no     \
    previous element */
#define FLST_NEXT                              \
  FIL_ADDR_SIZE /* 6-byte address of the next  \
        list element; the page part of address \
        is FIL_NULL, if no next element */

/* We define the field offsets of a base node for the list */
#define FLST_LEN 0 /* 32-bit list length field */
#define FLST_FIRST                         \
  4 /* 6-byte address of the first element \
    of the list; undefined if empty list */
#define FLST_LAST                              \
  (4 + FIL_ADDR_SIZE) /* 6-byte address of the \
          last element of the list; undefined  \
          if empty list */

/* The C 'types' of base node and list node: these should be used to
write self-documenting code. Of course, the sizeof macro cannot be
applied to these types! */

typedef byte flst_base_node_t;
typedef byte flst_node_t;

/* The physical size of a list base node in bytes */
constexpr ulint FLST_BASE_NODE_SIZE = 4 + 2 * FIL_ADDR_SIZE;

/* The physical size of a list node in bytes */
constexpr ulint FLST_NODE_SIZE = 2 * FIL_ADDR_SIZE;


/** Adds a node as the last node in a list.
@param[in] base Pointer to base node of list
@param[in] node Node to add */
void flst_add_last(flst_base_node_t *base, flst_node_t *node);

/** Adds a node as the first node in a list.
@param[in] base Pointer to base node of list
@param[in] node Node to add */
void flst_add_first(flst_base_node_t *base, flst_node_t *node);


/** Get the length of a list.
@param[in]	base	base node
@return length */
static inline ulint flst_get_len(const flst_base_node_t *base);

/** Gets list first node address.
@param[in]	base	Pointer to base node
@return file address */
static inline fil_addr_t flst_get_first(const flst_base_node_t *base);

/** Gets list last node address.
@param[in]	base	Pointer to base node
@return file address */
static inline fil_addr_t flst_get_last(const flst_base_node_t *base);

/** Gets list next node address.
@param[in]	node	Pointer to node
@return file address */
static inline fil_addr_t flst_get_next_addr(const flst_node_t *node);

/** Gets list prev node address.
@param[in]	node	Pointer to node
@return file address */
static inline fil_addr_t flst_get_prev_addr(const flst_node_t *node);


/** Reads a file address.
@param[in]	faddr	Pointer to file faddress
@return file address */
static inline fil_addr_t flst_read_addr(const fil_faddr_t *faddr);


/** In-memory representation of flst_base_node_t */
struct flst_bnode_t {
  ulint len;
  fil_addr_t first;
  fil_addr_t last;

  flst_bnode_t() : len(0) {}

  flst_bnode_t(const flst_base_node_t *base)
      : len(flst_get_len(base)),
        first(flst_get_first(base)),
        last(flst_get_last(base)) {}

  void set(const flst_base_node_t *base) {
    len = flst_get_len(base);
    first = flst_get_first(base);
    last = flst_get_last(base);
  }

  void reset() {
    len = 0;
    first = fil_addr_null;
    last = fil_addr_null;
  }

  std::ostream &print(std::ostream &out) const {
    out << "[flst_base_node_t: len=" << len << ", first=" << first
        << ", last=" << last << "]";
    return (out);
  }
};

inline std::ostream &operator<<(std::ostream &out, const flst_bnode_t &obj) {
  return (obj.print(out));
}


/** Reads a file address.
 @return file address */
static inline fil_addr_t flst_read_addr(
    const fil_faddr_t *faddr) /*!< in: pointer to file faddress */
{
  fil_addr_t addr;


  addr.page = mach_read_from_4(faddr + FIL_ADDR_PAGE);
  addr.boffset = mach_read_from_2(faddr + FIL_ADDR_BYTE);
  ut_a(addr.page == FIL_NULL || addr.boffset >= FIL_PAGE_DATA);
  ut_a(ut_align_offset(faddr, UNIV_PAGE_SIZE) >= FIL_PAGE_DATA);
  return (addr);
}


/** Get the length of a list.
@param[in]	base	base node
@return length */
static inline ulint flst_get_len(const flst_base_node_t *base) {
  return (mach_read_from_4(base + FLST_LEN));
}

/** Gets list first node address.
 @return file address */
static inline fil_addr_t flst_get_first(
    const flst_base_node_t *base) /*!< in: pointer to base node */
{
  return (flst_read_addr(base + FLST_FIRST));
}

/** Gets list last node address.
 @return file address */
static inline fil_addr_t flst_get_last(
    const flst_base_node_t *base) /*!< in: pointer to base node */
{
  return (flst_read_addr(base + FLST_LAST));
}

/** Gets list next node address.
 @return file address */
static inline fil_addr_t flst_get_next_addr(
    const flst_node_t *node) /*!< in: pointer to node */
{
  return (flst_read_addr(node + FLST_NEXT));
}

/** Gets list prev node address.
 @return file address */
static inline fil_addr_t flst_get_prev_addr(
    const flst_node_t *node) /*!< in: pointer to node */
{
  return (flst_read_addr(node + FLST_PREV));
}


#endif /* fut0lst_h */
