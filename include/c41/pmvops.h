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
#define C41_MEM_COPY(_d, _s, _l) (c41_u8a_copy((uint8_t *) (_d), (_s), (_l)))
#define C41_MEM_MOVE(_d, _s, _l) (c41_u8a_move((uint8_t *) (_d), (_s), (_l)))
#define C41_MEM_FILL(_d, _l, _v) (c41_u8a_fill((uint8_t *) (_d), (_l), (_v)))
#define C41_MEM_COMPARE(_a, _b, _l) \
  (c41_u8a_compare((uint8_t const *) (_a), (uint8_t const *) (_b), (_l)))
#define C41_MEM_SCAN(_s, _l, _v) (c41_u8a_scan((_s), (_l), (_v)))
#define C41_STR_LEN(_s) (c41_u8a_scan_ofs_nolim((uint8_t const *) (_s), 0))
#define C41_STR_COMPARE(_a, _b) \
  (c41_u8s_compare((uint8_t const *) (_a), (uint8_t const *) (_b)))
#endif

#define C41_MEM_SCAN_NOLIM(_s, _v) c41_u8a_scan_nolim((_s), (_v))

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

#endif /* _C41_PMV_OPS_H_ */

