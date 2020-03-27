/*
 * type.h: defines integer/boolean types.
 * this file is part of the LPTSTR common C header collection, and
 * is licensed under the MIT license.
 * (c) Kied Llaentenn and all contributors.
 * https://github.com/lptstr/ccommon
 *
 *
 * why?
 * I miss the Rust type system :P
*/

#ifndef CCOMMON_TYPES_H
#define CCOMMON_TYPES_H
#include <stdint.h>

typedef uint8_t          u8;
typedef uint16_t        u16;
typedef uint32_t        u32;
typedef uint64_t        u64;

typedef int8_t           i8;
typedef int16_t         i16;
typedef int32_t         i32;
typedef int64_t         i64;

/*
 * dynamically define usize and isize
 * based on the target machine's
 * integer size
*/

#if UINTPTR_MAX == 0xffff
typedef uint16_t      usize;
typedef int16_t       isize;
#elif UINTPTR_MAX == 0xffffffff
typedef uint32_t      usize;
typedef int32_t       isize;
#elif UINTPTR_MAX == 0xfffffffffffffff
typedef uint64_t      usize;
typedef int64_t       isize;
#else
typedef uint32_t      usize;
typedef int32_t       isize;
#endif

#endif
