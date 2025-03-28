/*===-- include/flang/ISO_Fortran_binding.h -----------------------*- C++ -*-===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 * ===-----------------------------------------------------------------------===
 */

#ifndef CFI_ISO_FORTRAN_BINDING_H_
#define CFI_ISO_FORTRAN_BINDING_H_

/* When this header is included into the compiler and runtime implementations,
 * it does so by means of a wrapper header that establishes namespaces and
 * a macro for extra function attributes (RT_API_ATTRS).
 */
#ifndef FORTRAN_COMMON_ISO_FORTRAN_BINDING_WRAPPER_H_
#include <stddef.h>
#define FORTRAN_ISO_NAMESPACE_
#endif

/* Standard interface to Fortran from C and C++.
 * These interfaces are named in subclause 18.5 of the Fortran 2018
 * standard, with most of the actual details being left to the
 * implementation.
 */

#ifndef RT_API_ATTRS
#define RT_API_ATTRS
#endif

/* 18.5.4 */
#define CFI_VERSION 20240719

#if !defined CFI_MAX_RANK || !defined __OVERRIDE_CFI_MAX_RANK
#define CFI_MAX_RANK 15
#endif
typedef unsigned char CFI_rank_t;

/* This type is probably larger than a default Fortran INTEGER
 * and should be used for all array indexing and loop bound calculations.
 */
typedef ptrdiff_t CFI_index_t;

typedef unsigned char CFI_attribute_t;
#define CFI_attribute_pointer 1
#define CFI_attribute_allocatable 2
#define CFI_attribute_other 0 /* neither pointer nor allocatable */

typedef signed char CFI_type_t;
/* These codes are required to be macros (i.e., #ifdef will work).
 * They are not required to be distinct, but neither are they required
 * to have had their synonyms combined.
 */
#define CFI_type_signed_char 1
#define CFI_type_short 2
#define CFI_type_int 3
#define CFI_type_long 4
#define CFI_type_long_long 5
#define CFI_type_size_t 6
#define CFI_type_int8_t 7
#define CFI_type_int16_t 8
#define CFI_type_int32_t 9
#define CFI_type_int64_t 10
#define CFI_type_int128_t 11 /* extension kind=16 */
#define CFI_type_int_least8_t 12
#define CFI_type_int_least16_t 13
#define CFI_type_int_least32_t 14
#define CFI_type_int_least64_t 15
#define CFI_type_int_least128_t 16 /* extension */
#define CFI_type_int_fast8_t 17
#define CFI_type_int_fast16_t 18
#define CFI_type_int_fast32_t 19
#define CFI_type_int_fast64_t 20
#define CFI_type_int_fast128_t 21 /* extension */
#define CFI_type_intmax_t 22
#define CFI_type_intptr_t 23
#define CFI_type_ptrdiff_t 24
#define CFI_type_half_float 25 /* extension: kind=2 */
#define CFI_type_bfloat 26 /* extension: kind=3 */
#define CFI_type_float 27
#define CFI_type_double 28
#define CFI_type_extended_double 29 /* extension: kind=10 */
#define CFI_type_long_double 30
#define CFI_type_float128 31 /* extension: kind=16 */
#define CFI_type_half_float_Complex 32 /* extension: kind=2 */
#define CFI_type_bfloat_Complex 33 /* extension: kind=3 */
#define CFI_type_float_Complex 34
#define CFI_type_double_Complex 35
#define CFI_type_extended_double_Complex 36 /* extension: kind=10 */
#define CFI_type_long_double_Complex 37
#define CFI_type_float128_Complex 38 /* extension: kind=16 */
#define CFI_type_Bool 39
#define CFI_type_char 40
#define CFI_type_cptr 41
#define CFI_type_struct 42
#define CFI_type_char16_t 43 /* extension kind=2 */
#define CFI_type_char32_t 44 /* extension kind=4 */
#define CFI_type_uint8_t 45 /* extension: unsigned */
#define CFI_type_uint16_t 46
#define CFI_type_uint32_t 47
#define CFI_type_uint64_t 48
#define CFI_type_uint128_t 49
#define CFI_TYPE_LAST CFI_type_uint128_t
#define CFI_type_other (-1) // must be negative

/* Error code macros - skip some of the small values to avoid conflicts with
 * other status codes mandated by the standard, e.g. those returned by
 * GET_ENVIRONMENT_VARIABLE (16.9.84) */
#define CFI_SUCCESS 0 /* must be zero */
#define CFI_ERROR_BASE_ADDR_NULL 11
#define CFI_ERROR_BASE_ADDR_NOT_NULL 12
#define CFI_INVALID_ELEM_LEN 13
#define CFI_INVALID_RANK 14
#define CFI_INVALID_TYPE 15
#define CFI_INVALID_ATTRIBUTE 16
#define CFI_INVALID_EXTENT 17
#define CFI_INVALID_DESCRIPTOR 18
#define CFI_ERROR_MEM_ALLOCATION 19
#define CFI_ERROR_OUT_OF_BOUNDS 20

/* 18.5.2 per-dimension information */
typedef struct CFI_dim_t {
  CFI_index_t lower_bound;
  CFI_index_t extent; /* == -1 for assumed size */
  CFI_index_t sm; /* memory stride in bytes */
} CFI_dim_t;

#ifdef __cplusplus
namespace cfi_internal {
// C++ does not support flexible array.
// The below structure emulates a flexible array. This structure does not take
// care of getting the memory storage. Note that it already contains one element
// because a struct cannot be empty.
extern "C++" template <typename T> struct FlexibleArray : T {
  RT_API_ATTRS T &operator[](int index) { return *(this + index); }
  RT_API_ATTRS const T &operator[](int index) const { return *(this + index); }
  RT_API_ATTRS operator T *() { return this; }
  RT_API_ATTRS operator const T *() const { return this; }
};
} // namespace cfi_internal
#endif

/* 18.5.3 generic data descriptor */

/* Descriptor header members */
#define _CFI_CDESC_T_HEADER_MEMBERS \
  /* These three members must appear first, \
   * in exactly this order. */ \
  void *base_addr; \
  size_t elem_len; /* element size in bytes */ \
  int version; /* == CFI_VERSION */ \
  CFI_rank_t rank; /* [0 .. CFI_MAX_RANK] */ \
  CFI_type_t type; \
  CFI_attribute_t attribute; \
  /* This encodes both the presence of the f18Addendum and the index of the \
   * allocator used to managed memory of the data hold by the descriptor. */ \
  unsigned char extra;

typedef struct CFI_cdesc_t {
  _CFI_CDESC_T_HEADER_MEMBERS
#ifdef __cplusplus
  cfi_internal::FlexibleArray<CFI_dim_t> dim;
#else
  CFI_dim_t dim[]; /* must appear last */
#endif
} CFI_cdesc_t;

/* 18.5.4 */
#ifdef __cplusplus
// This struct acquires the additional storage, if any is
// needed, for C++'s CFI_cdesc_t's emulated flexible
// dim[] array.
namespace cfi_internal {
extern "C++" template <int r> struct CdescStorage : public CFI_cdesc_t {
  static_assert((r > 1 && r <= CFI_MAX_RANK), "CFI_INVALID_RANK");
  CFI_dim_t dim[r - 1];
};
extern "C++" template <> struct CdescStorage<1> : public CFI_cdesc_t {};
extern "C++" template <> struct CdescStorage<0> : public CFI_cdesc_t {};
} // namespace cfi_internal
#define CFI_CDESC_T(rank) \
  FORTRAN_ISO_NAMESPACE_::cfi_internal::CdescStorage<rank>
#else
#define CFI_CDESC_T(_RANK) \
  struct { \
    _CFI_CDESC_T_HEADER_MEMBERS \
    CFI_dim_t dim[_RANK]; \
  }
#endif

/* 18.5.5 procedural interfaces*/
#ifdef __cplusplus
extern "C" {
#endif
RT_API_ATTRS void *CFI_address(
    const CFI_cdesc_t *, const CFI_index_t subscripts[]);
RT_API_ATTRS int CFI_allocate(CFI_cdesc_t *, const CFI_index_t lower_bounds[],
    const CFI_index_t upper_bounds[], size_t elem_len);
RT_API_ATTRS int CFI_deallocate(CFI_cdesc_t *);
RT_API_ATTRS int CFI_establish(CFI_cdesc_t *, void *base_addr, CFI_attribute_t,
    CFI_type_t, size_t elem_len, CFI_rank_t, const CFI_index_t extents[]);
RT_API_ATTRS int CFI_is_contiguous(const CFI_cdesc_t *);
RT_API_ATTRS int CFI_section(CFI_cdesc_t *, const CFI_cdesc_t *source,
    const CFI_index_t lower_bounds[], const CFI_index_t upper_bounds[],
    const CFI_index_t strides[]);
RT_API_ATTRS int CFI_select_part(CFI_cdesc_t *, const CFI_cdesc_t *source,
    size_t displacement, size_t elem_len);
RT_API_ATTRS int CFI_setpointer(
    CFI_cdesc_t *, const CFI_cdesc_t *source, const CFI_index_t lower_bounds[]);
#ifdef __cplusplus
} // extern "C"
#endif

#endif /* CFI_ISO_FORTRAN_BINDING_H_ */
