/* [c41] Base header file
 *
 * This file defines basic types and macros to be used uniformly across
 * platforms and compilers.
 *
 * Changelog:
 *  - 2013/01/04 Costin Ionescu: initial commit
 */

#ifndef _C41_BASE_H_
#define _C41_BASE_H_

#if defined(_M_AMD64) || defined(__amd64__)
#define C41_AMD64 1
#endif

#if defined(_M_IX86) || defined(__i386__)
#define C41_IA32 1
#endif

#if C41_AMD64 || C41_IA32
#define C41_LITTLE_ENDIAN 1
#undef C41_BIG_ENDIAN
#else
#endif

/* token pasting macros */
#define C41_P2D(_a, _b) _a ## _b
#define C41_P2(_a, _b) C41_P2D(_a, _b)

#define C41_P3D(_a, _b, _c) _a ## _b ## _c
#define C41_P3(_a, _b, _c) C41_P3D(_a, _b, _c)

#define C41_TS(_x) #_x

#define C41_INLINE static __inline

#if defined(_WIN32) || defined(__CYGWIN__)
# define C41_DL_EXPORT __declspec(dllexport)
# define C41_DL_IMPORT __declspec(dllimport)
# define C41_LOCAL
#elif __GNUC__ >= 4
# define C41_DL_IMPORT __attribute__ ((visibility ("default")))
# define C41_DL_EXPORT __attribute__ ((visibility ("default")))
# define C41_LOCAL __attribute__ ((visibility ("hidden")))
#elif defined(__GNUC__)
# define C41_DL_IMPORT
# define C41_DL_EXPORT
# define C41_LOCAL
#endif

/* include standard types */
#if defined(_MSC_VER)
# if defined(_M_IX86)
#   define C41_CHAR_BITS         8
#   define C41_SHORT_BITS        16
#   define C41_INT_BITS          32
#   define C41_LONG_BITS         32
#   define C41_LONGLONG_BITS     64
#   define C41_PTR_BITS          32
#   define C41_SIZE_BITS         32
# elif defined(_M_AMD64)
#   define C41_CHAR_BITS         8
#   define C41_SHORT_BITS        16
#   define C41_INT_BITS          32
#   define C41_LONG_BITS         32
#   define C41_LONGLONG_BITS     64
#   define C41_PTR_BITS          64
#   define C41_SIZE_BITS         32
# endif
#else
# if defined(__i386__)
#   define C41_CHAR_BITS         8
#   define C41_SHORT_BITS        16
#   define C41_INT_BITS          32
#   define C41_LONG_BITS         32
#   define C41_LONGLONG_BITS     64
#   define C41_PTR_BITS          32
#   define C41_SIZE_BITS         32
# elif defined(__amd64__)
#   define C41_CHAR_BITS         8
#   define C41_SHORT_BITS        16
#   define C41_INT_BITS          32
#   define C41_LONG_BITS         64
#   define C41_LONGLONG_BITS     64
#   define C41_PTR_BITS          64
#   define C41_SIZE_BITS         64
# elif defined(__arm__)
#   define C41_CHAR_BITS         8
#   define C41_SHORT_BITS        16
#   define C41_INT_BITS          32
#   define C41_LONG_BITS         32
#   define C41_LONGLONG_BITS     64
#   define C41_PTR_BITS          32
#   define C41_SIZE_BITS         32
# endif
#endif

#define C41_SSIZE_MAX (((size_t) 1 << (C41_SIZE_BITS - 1)) - 1)
#define C41_SIZE_MAX (C41_SSIZE_MAX + C41_SSIZE_MAX + 1)


#if _MSC_VER

# include <stddef.h>
# include <basetsd.h>

typedef unsigned char       uint8_t;
typedef   signed char        int8_t;
typedef unsigned short int  uint16_t;
typedef   signed short int   int16_t;
typedef unsigned int        uint32_t;
typedef   signed int         int32_t;
typedef unsigned __int64    uint64_t;
typedef   signed __int64     int64_t;

typedef long ssize_t;

# if !UINTPTR_T_DEFINED && _MSC_VER < 1500
#   if C41_PTR_BITS == C41_LONG_BITS
typedef unsigned long int uintptr_t;
#   elif C41_PTR_BITS == 64
typedef unsigned __int64 uintptr_t;
# else
#   error "do not know how to define uintptr_t"
#   endif
# endif

#else /* !_MSC_VER */
# include <stdint.h>
# include <unistd.h>
#endif /* _MSC_VER and !_MSC_VER */

#ifdef _WIN32
#define C41_CALL __fastcall
#elif __GNUC__ >= 3 && __amd64__
#define C41_CALL __attribute__((regparm(6)))
#elif __GNUC__ >= 3 && __i386__
#define C41_CALL __attribute__((regparm(3)))
#else
#define C41_CALL
#endif

typedef   signed           char c41_schar_t;
typedef unsigned           char c41_uchar_t;
typedef   signed short      int c41_sshort_t;
typedef unsigned short      int c41_ushort_t;
typedef   signed            int c41_sint_t;
typedef unsigned            int c41_uint_t;
typedef   signed long       int c41_slong_t;
typedef unsigned long       int c41_ulong_t;
typedef   signed long long  int c41_slonglong_t;
typedef unsigned long long  int c41_ulonglong_t;
typedef intptr_t c41_sptr_t;
typedef uintptr_t c41_uptr_t;

#if !UINT_T_DEFINED
typedef unsigned int uint_t;
#endif

#if !ULONG_T_DEFINED
typedef unsigned long int ulong_t;
#endif

#define C41_CASE_RET_STR(_x) case _x: return #_x

#define C41_PTR_OFS(_p, _o) ((void *) ((uint8_t *) (_p) + (_o)))
#define C41_PTR_DIFF(_a, _b) ((uint8_t const *) (_a) - (uint8_t const *) (_b))

#define C41_FIELD_OFS(_type, _field) ((intptr_t) &((_type *) NULL)->_field)
#define C41_OBJ_FIELD_OFS(_obj_p, _field) C41_PTR_DIFF(&(_obj_p)->_field, (_obj_p))

#define C41_FIELD_TO_OBJECT(_obj_type, _field_name, _field_ptr) \
  ((_obj_type *) C41_PTR_OFS((_field_ptr), \
                             (-C41_FIELD_OFS(_obj_type, _field_name))))

#define C41_ITEM_COUNT(_a) (sizeof(_a) / sizeof((_a)[0]))

#define C41_RU8(_p) (*((uint8_t const *) (_p)))

#define C41_IS_POW2(_x) ((_x) && ((_x) & ((_x) - 1)) == 0)

#endif /* _C41_BASE_H_ */

