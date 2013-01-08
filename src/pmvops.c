/* [c41] Primitive Operations - functions
 * Changelog:
 *  - 2013/01/04 Costin Ionescu: initial commit
 */

#include <c41.h>

/* c41_u8a_copy *************************************************************/
C41_API void * C41_CALL c41_u8a_copy
(
  uint8_t * d,
  uint8_t const * s,
  size_t l
)
{
#if HAVE_STRING_H
  return memcpy(d, s, l);
#else
  uint8_t * t = d;
  while (l--) *t++ = *s++;
  return d;
#endif
}


/* c41_u8a_move *************************************************************/
C41_API void * C41_CALL c41_u8a_move
(
  uint8_t * d,
  uint8_t const * s,
  size_t n
)
{
#if HAVE_STRING_H
  return memmove(d, s, n);
#else
  uint8_t * t;
  if (d < s)
  {
    t = d;
    while (n--) *t++ = *s++;
  }
  else
  {
    t = d + n - 1;
    s += n - 1;
    while (n--) *t-- = *s--;
  }
  return d;
#endif
}


/* c41_u8a_fill *************************************************************/
C41_API void * C41_CALL c41_u8a_fill
(
  uint8_t * d,
  size_t l,
  uint8_t v
)
{
#if HAVE_STRING_H
  return memset(d, v, l);
#else
  uint8_t * b, * e;
  for (b = d, e = b + l; b < e; ++b) *b = v;
  return d;
#endif
}

/* c41_u8a_compare **********************************************************/
C41_API int C41_CALL c41_u8a_compare
(
  uint8_t const * a,
  uint8_t const * b,
  size_t l
)
{
#if HAVE_STRING_H
  return memcmp(a, b, l);
#else
  for (; l; --l, ++a, ++b)
    if (*a != *b) return (int) *a - (int) *b;
  return 0;
#endif
}

/* c41_u8s_compare **********************************************************/
C41_API int C41_CALL c41_u8s_compare
(
  uint8_t const * a,
  uint8_t const * b
)
{
#if HAVE_STRING_H
  return strcmp((char const *) a, (char const *) b);
#else
  for (; *a == *b && *a; ++a, ++b);
  return (int) *a - (int) *b;
#endif
}

/* c41_u8a_scan_nolim *******************************************************/
C41_API void * C41_CALL c41_u8a_scan_nolim
(
  uint8_t const * s,
  uint8_t v
)
{
  while (*s != v) ++s;
  return (void *) s;
}

/* c41_u8a_scan_ofs_nolim ***************************************************/
C41_API size_t C41_CALL c41_u8a_scan_ofs_nolim
(
  uint8_t const * s,
  uint8_t v
)
{
  return (size_t) ((uint8_t const *) c41_u8a_scan_nolim(s, v) - s);
}


/* c41_u8a_scan *************************************************************/
C41_API void * C41_CALL c41_u8a_scan
(
  uint8_t const * s,
  size_t l,
  uint8_t v
)
{
#if HAVE_STRING_H
  return memchr(s, v, l);
#else
  uint8_t const * e;
  for (e = s + l; s != e; ++s)
    if (*s == v) return (void *) s;
  return NULL;
#endif
}

