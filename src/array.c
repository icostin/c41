/* [c41] Array support - header file
 * Changelog:
 * - 2013/03/21 Costin Ionescu: added c41_u8v_afmt()
 */

#include <c41.h>

/* c41_u8v_extend ***********************************************************/
C41_API uint_t C41_CALL c41_u8v_extend (c41_u8v_t * v, size_t len)
{
  size_t m, o;
  uint_t r;
  m = v->n + len;
  if (m < len) return C41_MA_LEN_OVERFLOW;
  o = (size_t) 1 << v->order;
  if (m >= o) m = (m + o - 1) & (- (size_t) o);
  else
  {
    for (o = 8; o < m; o <<= 1);
    m = o;
  }

  r = c41_ma_realloc(v->ma_p, (void **) &v->a, m, v->m);
  if (r) return (v->ma_rc = r);
  v->m = m;

  return 0;
}

/* c41_u8v_append ***********************************************************/
C41_API uint8_t * C41_CALL c41_u8v_append (c41_u8v_t * v, size_t len)
{
  uint8_t * p;
  if (c41_u8v_extend(v, len)) return NULL;
  p = v->a + v->n;
  v->n += len;
  return p;
}

/* c41_u8v_free *************************************************************/
C41_API uint_t C41_CALL c41_u8v_free (c41_u8v_t * v)
{
  uint_t r;
  if (!v->m) return 0;
  r = c41_ma_free(v->ma_p, v->a, v->m);
  if (r) v->ma_rc = r;
  return r;
}

/* c41_u8v_opt **************************************************************/
C41_API uint_t C41_CALL c41_u8v_opt (c41_u8v_t * v)
{
  uint_t r;
  if (v->n == v->m) return 0;
  r = c41_ma_realloc(v->ma_p, (void * *) &v->a, v->n, v->m);
  if (r) return (v->ma_rc = r);
  v->m = v->n;
  return 0;
}

/* afmt_writer **************************************************************/
static size_t C41_CALL afmt_writer
(
  void * output,
  uint8_t const * data,
  size_t len,
  void * ctx
)
{
  c41_u8v_t * v = output;
  uint8_t * d;

  (void) ctx;
  d = c41_u8v_append(v, len);
  if (!d) return 0;
  C41_MEM_COPY(d, data, len);
  return len;
}

/* c41_u8v_afmt *************************************************************/
C41_API uint_t C41_CALL c41_u8v_afmt (c41_u8v_t * v, char const * fmt, ...)
{
  va_list va;
  int sc;
  size_t wlen;

  va_start(va, fmt);
  sc = c41_write_vfmt(v, afmt_writer, NULL, c41_term_utf8_str_width_swf, NULL,
                      &wlen, fmt, va);
  va_end(va);
  return sc;
}

