/* [c41] Primitive Operations - header file
 * Changelog:
 *  - 2013/01/04 Costin Ionescu: initial commit
 */

#ifndef _C41_PMV_OPS_H_
#define _C41_PMV_OPS_H_

#if HAVE_STRING_H
#include <string.h>
#define C41_MEM_COPY(_d, _s, _l) (memcpy((_d), (_s), (_l)))
#define C41_MEM_MOVE(_d, _s, _l) (memmove((_d), (_s), (_l)))
#define C41_MEM_FILL(_d, _l, _v) (memset((_d), (_v), (_l)))
#define C41_MEM_COMPARE(_a, _b, _l) (memcmp((_a), (_b), (_l)))
#define C41_MEM_SCAN(_s, _l, _v) (memchr((_s), (_v), (_l)))
#define C41_STR_LEN(_s) (strlen((char const *) (_s)))
#define C41_STR_COMPARE(_a, _b) \
  (strcmp((char const *) (_a), (char const *) (_b)))
#else
#define C41_MEM_COPY(_d, _s, _l) \
    (c41_u8a_copy((uint8_t *) (_d), (uint8_t const *) (_s), (_l)))
#define C41_MEM_MOVE(_d, _s, _l) (c41_u8a_move((uint8_t *) (_d), (_s), (_l)))
#define C41_MEM_FILL(_d, _l, _v) (c41_u8a_fill((uint8_t *) (_d), (_l), (_v)))
#define C41_MEM_COMPARE(_a, _b, _l) \
  (c41_u8a_compare((uint8_t const *) (_a), (uint8_t const *) (_b), (_l)))
#define C41_MEM_SCAN(_s, _l, _v) (c41_u8a_scan((_s), (_l), (_v)))
#define C41_STR_LEN(_s) (c41_u8a_scan_ofs_nolim((uint8_t const *) (_s), 0))
#define C41_STR_COMPARE(_a, _b) \
  (c41_u8s_compare((uint8_t const *) (_a), (uint8_t const *) (_b)))
#endif

#define C41_MEM_SCAN_NOLIM(_s, _v) (c41_u8a_scan_nolim((_s), (_v)))

#define C41_MEM_CMP_LIT(_a, _b) C41_MEM_COMPARE((_a), (_b), sizeof(_b) - 1)

/* c41_u8a_copy *************************************************************/
C41_API void * C41_CALL c41_u8a_copy
(
  uint8_t * d,
  uint8_t const * s,
  size_t l
);

/* c41_u8a_move *************************************************************/
C41_API void * C41_CALL c41_u8a_move
(
  uint8_t * d,
  uint8_t const * s,
  size_t l
);

/* c41_u8a_fill *************************************************************/
C41_API void * C41_CALL c41_u8a_fill
(
  uint8_t * d,
  size_t l,
  uint8_t v
);

/* c41_u8a_scan *************************************************************/
C41_API void * C41_CALL c41_u8a_scan
(
  uint8_t const * s,
  size_t l,
  uint8_t v
);

/* c41_u8a_compare **********************************************************/
C41_API int C41_CALL c41_u8a_compare
(
  uint8_t const * a,
  uint8_t const * b,
  size_t l
);

/* c41_u8s_compare **********************************************************/
C41_API int C41_CALL c41_u8s_compare
(
  uint8_t const * a,
  uint8_t const * b
);

/* c41_u8a_scan_nolim *******************************************************/
C41_API void * C41_CALL c41_u8a_scan_nolim
(
  uint8_t const * s,
  uint8_t v
);

/* c41_u8a_scan_ofs_nolim ***************************************************/
C41_API size_t C41_CALL c41_u8a_scan_ofs_nolim
(
  uint8_t const * s,
  uint8_t v
);

/* c41_mem_zero *************************************************************/
#define C41_MEM_ZERO(_d, _l) (C41_MEM_FILL((_d), (_l), 0))

/* c41_mem_minus_one ********************************************************/
#define C41_MEM_MINUS_ONE(_d, _l) (C41_MEM_FILL((_d), (_l), -1))

/* C41_MEM_EQUAL ************************************************************/
#define C41_MEM_EQUAL(_a, _b, _l) (!C41_MEM_COMPARE((_a), (_b), (_l)))

/* C41_MEM_EQUAL_LIT ********************************************************/
#define C41_MEM_EQUAL_LIT(_mem, _lit) (C41_MEM_EQUAL((_mem), (_lit), sizeof(_lit)))

/* C41_STR_EQUAL ************************************************************/
#define C41_STR_EQUAL(_a, _b) (!C41_STR_COMPARE((_a), (_b)))

/* C41_VAR_ZERO *************************************************************/
#define C41_VAR_ZERO(_v) C41_MEM_ZERO(&(_v), sizeof(_v))

/* C41_VAR_MINUS_ONE ********************************************************/
#define C41_VAR_MINUS_ONE(_v) C41_MEM_MINUS_ONE(&(_v), sizeof(_v))

#define C41_RU8(_p) (*((uint8_t const *) (_p)))

/* c41_read_u16le ***********************************************************/
C41_INLINE uint16_t c41_read_u16le (void const * p)
{
    uint8_t const * b = p;
    return b[0] | ((uint16_t) b[1] << 8);
}
#define c41_read_s16le(_p) ((int16_t) c41_read_u16le((_p)))

/* c41_read_u16be ***********************************************************/
C41_INLINE uint16_t c41_read_u16be (void const * p)
{
    uint8_t const * b = p;
    return b[1] | ((uint16_t) b[0] << 8);
}
#define c41_read_s16be(_p) ((int16_t) c41_read_u16be((_p)))

/* c41_read_u32le ***********************************************************/
C41_INLINE uint32_t c41_read_u32le (void const * p)
{
    uint8_t const * b = p;
    return b[0] | ((uint32_t) b[1] << 8) 
            | ((uint32_t) b[2] << 16) | ((uint32_t) b[3] << 24);
}
#define c41_read_s32le(_p) ((int32_t) c41_read_u32le((_p)))

/* c41_read_u32be ***********************************************************/
C41_INLINE uint32_t c41_read_u32be (void const * p)
{
    uint8_t const * b = p;
    return b[3] | ((uint32_t) b[2] << 8) 
            | ((uint32_t) b[1] << 16) | ((uint32_t) b[0] << 24);
}
#define c41_read_s32be(_p) ((int32_t) c41_read_u32be((_p)))

/* c41_read_u32be_array *****************************************************/
C41_INLINE void c41_read_u32be_array 
(
    uint32_t * dest,
    void const * src,
    size_t count
)
{
    uint8_t const * b = src;
    uint8_t const * e = b + count * 4;
    for (; b < e; b += 4) *dest++ = c41_read_u32be(b);
}

/* c41_bswap16 **************************************************************/
C41_INLINE uint16_t c41_bswap16 (uint16_t v)
{
    return (v << 8) | (v >> 8);
}

/* c41_bswap32 **************************************************************/
C41_INLINE uint32_t c41_bswap32 (uint32_t v)
{
    return (v >> 24) | ((v >> 8) & 0xFF00) | ((v << 8) & 0xFF0000) | (v << 24);
}

/* c41_bswap16_array ********************************************************/
C41_INLINE void c41_bswap16_array (void * p, size_t count)
{
    uint16_t * a = p;
    while (count--) { *a = c41_bswap16(*a); ++a; }
}

/* c41_bswap32_array ********************************************************/
C41_INLINE void c41_bswap32_array (void * p, size_t count)
{
    uint32_t * a = p;
    while (count--) { *a = c41_bswap32(*a); ++a; }
}

#endif /* _C41_PMV_OPS_H_ */

