/* [c41] Strings - functions
 * Changelog:
 *  - 2013/01/04 Costin Ionescu: initial commit
 */

#include <c41.h>

char c41_digits[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
extern uint8_t c41_ucw_ofs_a[];
extern uint8_t c41_ucw_val_a[];

/* c41_hex ******************************************************************/
C41_API void * C41_CALL c41_hex (void * out, void const * in, size_t len)
{
  uint8_t const * i = in;
  uint8_t const * ie = i + len;
  uint8_t * o = out;

  for (; i < ie; ++i)
  {
    *o++ = c41_digits[*i >> 4];
    *o++ = c41_digits[*i & 0x0F];
  }
  return out;
}

/* c41_hexz *****************************************************************/
C41_API void * C41_CALL c41_hexz (void * out, void const * in, size_t len)
{
  uint8_t const * i = in;
  uint8_t const * ie = i + len;
  uint8_t * o = out;

  for (; i < ie; ++i)
  {
    *o++ = c41_digits[*i >> 4];
    *o++ = c41_digits[*i & 0x0F];
  }
  *o = 0;
  return out;
}

/* c41_unhex ****************************************************************/
C41_API size_t C41_CALL c41_unhex (void * out, void const * in, size_t len)
{
  uint8_t * o = out;
  uint8_t * oe = o + len;
  uint8_t const * i = in;
  uint8_t b, c;

  for (; o < oe; ++o)
  {
    b = *i++;
    if (b >= '0' && b <= '9') b -= '0';
    else
    {
      b = (b | 0x20) - 'a';
      if (b > 5) break;
      b += 10;
    }

    c = *i++;
    if (c >= '0' && c <= '9') c -= '0';
    else
    {
      c = (c | 0x20) - 'a';
      if (c > 5) break;
      c += 10;
    }

    *o = (b << 4) | c;
  }
  return o - (uint8_t *) out;
}

/* c41_utf8_char_len ********************************************************/
C41_API int C41_CALL c41_utf8_char_len (uint32_t cp)
{
  if (cp < 0x80) return 1;
  if (cp < 0x800) return 2;
  if (cp < 0x10000) return ((cp & 0xF800) == 0xD800) ? -3 : 3;
  if (cp < 0x110000) return 4;
  return -1;
}

/* c41_utf8_char_decode_raw *************************************************/
C41_API int C41_CALL c41_utf8_char_decode_raw
(
  void const * vdata,
  size_t len,
  uint32_t * out
)
{
  uint8_t const * data = vdata;
  uint8_t b;
#define CC(_pos) if ((data[_pos] & 0xC0) != 0x80) return -2 - (_pos);
  if (!len) return -1;
  b = *data;
  if (b < 0x80) { *out = b; return 1; }
  if (b < 0xC0) return -2;
  if (b < 0xE0)
  {
    if (len < 2) return -1;
    CC(1);
    *out = (((uint32_t) b & 0x1F) << 6) | (data[1] & 0x3F);
    return 2;
  }
  if (b < 0xF0)
  {
    if (len < 3) return -1;
    CC(1);
    CC(2);
    *out =  (((uint32_t) b & 0x0F) << 12) |
            (((uint32_t) data[1] & 0x3F) << 6) |
            ((uint32_t) data[2] & 0x3F);
    return 3;
  }
  if (b < 0xF8)
  {
    if (len < 4) return -1;
    CC(1);
    CC(2);
    CC(3);
    *out =  (((uint32_t) b & 0x07) << 18) |
            (((uint32_t) data[1] & 0x3F) << 12) |
            (((uint32_t) data[2] & 0x3F) << 6) |
            ((uint32_t) data[3] & 0x3F);
    return 4;
  }
  if (b < 0xFC)
  {
    if (len < 5) return -1;
    CC(1);
    CC(2);
    CC(3);
    CC(4);
    *out =  (((uint32_t) b & 0x03) << 24) |
            (((uint32_t) data[1] & 0x3F) << 18) |
            (((uint32_t) data[2] & 0x3F) << 12) |
            (((uint32_t) data[3] & 0x3F) << 6) |
            ((uint32_t) data[4] & 0x3F);
    return 5;
  }
  if (b < 0xFE)
  {
    if (len < 6) return -1;
    CC(1);
    CC(2);
    CC(3);
    CC(4);
    CC(5);
    *out =  (((uint32_t) b & 0x01) << 30) |
            (((uint32_t) data[1] & 0x3F) << 24) |
            (((uint32_t) data[2] & 0x3F) << 18) |
            (((uint32_t) data[3] & 0x3F) << 12) |
            (((uint32_t) data[4] & 0x3F) << 6) |
            ((uint32_t) data[5] & 0x3F);
    return 6;
  }
  return -2;
#undef CC
}

/* c41_utf8_char_decode_strict **********************************************/
C41_API int C41_CALL c41_utf8_char_decode_strict
(
  void const * vdata,
  size_t len,
  uint32_t * out
)
{
  int l;
  l = c41_utf8_char_decode_raw(vdata, len, out);
  if (l < 0) return l;
  if (c41_utf8_char_len(*out) != l) return -8;
  if (l >= 0x110000) return -9;
  if ((l & 0xFFF800) == 0x00D800) return -10;
  return l;
}

/* c41_mutf8_str_decode *****************************************************/
C41_API int C41_CALL c41_mutf8_str_decode
(
  void const * vdata,
  size_t len,
  uint16_t * out_a,
  size_t out_n,
  size_t * in_len_p,
  size_t * out_len_p
)
{
  uint8_t const * d = vdata;
  uint8_t const * e = d + len;
  uint32_t cp;
  int l;
  size_t olen;

  for (olen = 0; d < e && olen < out_n; d += l, ++olen)
  {
    l = c41_utf8_char_decode_raw(d, e - d, &cp);
    if (l < 0) goto l_exitio;
    if (cp >= 0x10000) { l = -9; goto l_exitio; }
    if (!cp)
    {
      if (l != 2) { l = (l == 1) ? -10 : -8; goto l_exitio; }
    }
    else if (c41_utf8_char_len(cp) != l) { l = -8; goto l_exitio; }
    out_a[olen] = (uint16_t) cp;
  }

  if (in_len_p) *in_len_p = d - (uint8_t const *) vdata;
  if (d < e)
  {
    // output full, just count from now on...
    for (; d < e; d += l, ++olen)
    {
      l = c41_utf8_char_decode_raw(d, e - d, &cp);
      if (l < 0) goto l_exitio;
      if (cp >= 0x10000) { l = -9; goto l_exitio; }
      if (!cp)
      {
        if (l != 2) { l = (l == 1) ? -10 : -8; goto l_exitio; }
      }
      else if (c41_utf8_char_len(cp) != l) { l = -8; goto l_exitio; }
    }
  }

  l = 0;

l_exito:
  if (out_len_p) *out_len_p = olen;
  return l;

l_exitio:
  if (in_len_p) *in_len_p = d - (uint8_t const *) vdata;
  goto l_exito;
}

/* c41_term_char_width ******************************************************/
C41_API int C41_CALL c41_term_char_width (uint32_t cp)
{
  int b;

  b = (c41_ucw_val_a[(((uint_t) c41_ucw_ofs_a[cp >> 8]) << 6)
                    + ((cp >> 2) & 0x3F)] >> (2 * (cp & 3))) & 3;
  return b == 3 ? -1 : b;
}

/* c41_term_char_width_wctx *************************************************/
C41_API int C41_CALL c41_term_char_width_wctx (uint32_t cp, void * ctx)
{
  (void) ctx;
  return c41_term_char_width(cp);
}

/* c41_term_utf8_str_width **************************************************/
C41_API ssize_t C41_CALL c41_term_utf8_str_width (void const * s, size_t len)
{
  uint8_t const * b;
  size_t z;
  int l, w;
  uint32_t cp;

  for (b = s, z = 0; len; b += l, len -= l, z += w)
  {
    l = c41_utf8_char_decode_strict(b, len, &cp);
    if (l < 0) return l;
    w = c41_term_char_width(cp);
    if (w < 0) return -11;
  }
  return z;
}

/* c41_term_utf8_str_width_swf **********************************************/
C41_API ssize_t C41_CALL c41_term_utf8_str_width_swf
(
  uint8_t const * s,
  size_t len,
  void * unused
)
{
  (void) unused;
  return c41_term_utf8_str_width(s, len);
}

/* c41_utf8_str_measure *****************************************************/
C41_API int C41_CALL c41_utf8_str_measure
(
  int (C41_CALL * wf) (uint32_t cp, void * ctx),
  void * wf_ctx,
  void const * data,
  size_t b,
  size_t c,
  size_t w,
  size_t * bp,
  size_t * cp,
  size_t * wp
)
{
  int rc, l, k;
  size_t lb, lc, lw, cb, cc, cw;
  uint32_t ch;
  uint8_t const * d;

  lb = lc = lw = cb = cc = cw = 0;
  d = data;
  for (; cb < b; lb = cb, lc = cc, lw = cw)
  {
    l = c41_utf8_char_decode_strict(d, b - cb, &ch);
    if (l <= 0) { rc = -1; goto l_ret; }
    cb += l;
    d += l;
    ++cc;
    if (cc > c) { rc = 1; goto l_ret; }
    k = wf(ch, wf_ctx);
    if (k < 0) { rc = -2; goto l_ret; }
    cw += k;
    if (cw > w) { rc = 2; goto l_ret; }
  }
  rc = 0;

l_ret:
  *bp = lb;
  *cp = lc;
  *wp = lw;
  return rc;
}


/* c41_int64_fmt ************************************************************/
C41_API size_t C41_CALL c41_int64_fmt
(
  void * out,
  int64_t value,
  c41_int_fmt_t const * fmt_p
)
{
  char * o = out;
  char * s;
  uint64_t v;
  uint8_t d, dc;
  size_t l;

  switch (fmt_p->sign_mode)
  {
  case 0:
    if (value < 0) { *o++ = '-'; v = -value; }
    else v = value;
    break;
  case 1:
    if (value < 0) { *o++ = '-'; v = -value; }
    else { *o++ = '+'; v = value; }
    break;
  default:
    v = value;
    break;
  }

  if (fmt_p->radix_mark)
  {
    *o++ = '0';
    *o++ = fmt_p->radix_mark;
  }

  s = o;
  for (dc = 0; dc < fmt_p->min_digit_count || v; v /= fmt_p->radix, ++dc)
  {
    if (dc && fmt_p->group_len && dc % fmt_p->group_len == 0)
      *o++ = fmt_p->group_sep;
    *o++ = c41_digits[v % fmt_p->radix];
  }
  l = o - (char *) out;
  *o-- = 0;

  while (s < o)
  {
    d = *s;
    *s++ = *o;
    *o-- = d;
  }

  return l;
}

/* c41_str_width_blind ******************************************************/
C41_API ssize_t C41_CALL c41_str_width_blind
(
  uint8_t const * str,
  size_t len,
  void * ctx
)
{
  (void) str;
  (void) ctx;
  return len;
}

/* c41_sbw_init *************************************************************/
C41_API void * C41_CALL c41_sbw_init
(
  c41_sbw_t * sbw_p,
  void * buffer,
  size_t buf_limit,
  size_t count_limit
)
{
  sbw_p->data = buffer;
  sbw_p->len = 0;
  sbw_p->count_len = 0;
  if (buf_limit > count_limit) buf_limit = count_limit;
  sbw_p->buf_limit = buf_limit - 1;
  sbw_p->count_limit = count_limit;
  return sbw_p;
}

/* c41_sbw_write ************************************************************/
C41_API size_t C41_CALL c41_sbw_write
(
  void * output,
  uint8_t const * data,
  size_t len,
  void * ctx
)
{
  c41_sbw_t * sbw_p;
  size_t wl;

  (void) ctx;
  sbw_p = output;
  wl = sbw_p->buf_limit - sbw_p->len;
  if (wl > len) wl = len;

  if (wl < len)
  {
    /* strip incomplete char */
    while (wl > 0 && (data[wl] & 0xC0) == 0x80) --wl;
    /* adjust buf_limit so that next calls will not append shorter chars */
    sbw_p->buf_limit = sbw_p->len + wl;
  }

  C41_MEM_COPY(sbw_p->data + sbw_p->len, data, wl);
  sbw_p->len += wl;
  sbw_p->data[sbw_p->len] = 0;
  sbw_p->count_len += len;
  if (sbw_p->count_len > sbw_p->count_limit)
  {
    len -= sbw_p->count_len - sbw_p->count_limit;
    sbw_p->count_len = sbw_p->count_limit;
  }

  return len;
}

/* c41_write_vfmt ***********************************************************/
C41_API int c41_write_vfmt
(
  void *                        output,
  c41_str_writer_f              writer_func,
  void *                        writer_ctx,
  c41_str_width_f               width_func,
  void *                        width_ctx,
  size_t *                      used_len_p,
  char const *                  fmt,
  va_list                       va
)
{
  ssize_t prec;
  size_t used_len;
  char const * fe;
  char c, cmd, pfx;
  int64_t i64;
  uint8_t const * wbuf;
  uint8_t const * e;
  size_t w, wlen;
  c41_int_fmt_t ifmt;
  char tb[0x400];
  int i, rv;

  (void) width_func;
  (void) width_ctx;

  used_len = 0;
  while (*fmt)
  {
    for (fe = fmt; *fe && *fe != '$'; ++fe);
    if (fe != fmt)
    {
      w = writer_func(output, (uint8_t const *) fmt, fe - fmt, writer_ctx);
      used_len += w;
      if (w < (size_t) (fe - fmt)) { rv = 1; goto l_exit; }
    }
    if (!*fe) break;
    fmt = fe + 1;
    /* parse formatting */
    prec = -1;
    pfx = cmd = 0;
    wlen = 0; wbuf = NULL;
    i64 = 0;
    C41_VAR_ZERO(ifmt);
    for (; !cmd;)
    {
      switch (c = *fmt++)
      {
      case 0:
        rv = 4;
        goto l_exit;
      case '.':
        if (*fmt == '*')
        {
          prec = va_arg(va, size_t);
          ifmt.min_digit_count = (uint8_t) prec;
          ++fmt;
          break;
        }
        for (prec = 0; *fmt >= '0' && *fmt <= '9'; ++fmt)
          prec = prec * 10 + *fmt - '0';
        ifmt.min_digit_count = (uint8_t) prec;
        break;
      case 'G':
        c = *fmt++;
        if (c == '*') i = va_arg(va, int);
        else if (c >= '1' || c <= '9') i = c - '0';
        else { rv = 3; goto l_exit; }
        if (i < 1 || i > 9) { rv = 4; goto l_exit; }
        ifmt.group_len = (uint8_t) i;
        break;
      case '_':
        ifmt.group_sep = '_';
        break;
      case ',':
        ifmt.group_sep = ',';
        break;
      case '$':
        cmd = '$';
        break;
      case 'U':
        ifmt.sign_mode = 2;
        break;
      case 'S':
        ifmt.sign_mode &= 1;
        break;
      case '+':
        ifmt.sign_mode = 1;
        break;
      case 'X':
        ifmt.radix = 16;
        ifmt.radix_mark = 'x';
        break;
      case 'D':
        ifmt.radix = 10;
        break;
      case 'H':
        ifmt.radix = 16;
        break;
      case 'O':
        ifmt.radix = 8;
        break;
      case 'B':
        ifmt.radix = 2;
        break;
      case '#':
        pfx = 1;
        break;
      case 'b':
        cmd = 'i';
        i64 = (int8_t) va_arg(va, int);
        if (!ifmt.radix) ifmt.radix = 16;
        if (ifmt.radix == 16 && !ifmt.min_digit_count)
          ifmt.min_digit_count = 2;
        break;
      case 'w':
        cmd = 'i';
        i64 = (int16_t) va_arg(va, int);
        if (!ifmt.radix) ifmt.radix = 16;
        if (ifmt.radix == 16 && !ifmt.min_digit_count)
          ifmt.min_digit_count = 4;
        break;
      case 'd':
        cmd = 'i';
        i64 = (int32_t) va_arg(va, int32_t);
        if (!ifmt.radix) ifmt.radix = 16;
        if (ifmt.radix == 16 && !ifmt.min_digit_count)
          ifmt.min_digit_count = 8;
        break;
      case 'q':
        cmd = 'i';
        i64 = (int64_t) va_arg(va, int64_t);
        if (!ifmt.radix) ifmt.radix = 16;
        if (ifmt.radix == 16 && !ifmt.min_digit_count)
          ifmt.min_digit_count = 16;
        break;
      case 'h':
        cmd = 'i';
        i64 = (short int) va_arg(va, int);
        if (!ifmt.radix) ifmt.radix = 10;
        if (!ifmt.min_digit_count) ifmt.min_digit_count = 1;
        break;
      case 'i':
        cmd = 'i';
        i64 = (int) va_arg(va, int);
        if (!ifmt.radix) ifmt.radix = 10;
        if (!ifmt.min_digit_count) ifmt.min_digit_count = 1;
        break;
      case 'l':
        cmd = 'i';
        i64 = (long int) va_arg(va, long);
        if (!ifmt.radix) ifmt.radix = 10;
        if (!ifmt.min_digit_count) ifmt.min_digit_count = 1;
        break;
      case 'p':
        cmd = 'i';
        i64 = (intptr_t) va_arg(va, intptr_t);
        if (!ifmt.radix) ifmt.radix = 16;
        if (!ifmt.min_digit_count) ifmt.min_digit_count = sizeof(intptr_t) * 2;
        break;
      case 'z':
        cmd = 'i';
        i64 = (ssize_t) va_arg(va, ssize_t);
        if (!ifmt.radix) ifmt.radix = 10;
        if (!ifmt.min_digit_count) ifmt.min_digit_count = 1;
        break;
      case 's':
        cmd = 's';
        wbuf = (uint8_t const *) va_arg(va, uint8_t const *);
        if (prec < 0) e = C41_MEM_SCAN_NOLIM(wbuf, 0);
        else
        {
          e = C41_MEM_SCAN(wbuf, prec, 0);
          if (!e) e = wbuf + prec;
        }
        wlen = e - wbuf;
        break;
      default:
        rv = 3;
        goto l_exit;
      }
    }

    switch (cmd)
    {
    case '$':
      wbuf = (uint8_t const *) "$";
      wlen = 1;
      break;
    case 'i':
      if (pfx && !ifmt.radix_mark)
      {
        switch (ifmt.radix)
        {
        case  2: ifmt.radix_mark = 'b'; break;
        case  8: ifmt.radix_mark = 'o'; break;
        case 10: ifmt.radix_mark = 'd'; break;
        case 16: ifmt.radix_mark = 'x'; break;
        }
      }
      if (!ifmt.group_sep && ifmt.group_len)
      {
        ifmt.group_sep = ifmt.radix == 10 ? ',' : '_';
      }

      if (ifmt.group_sep && !ifmt.group_len)
      {
        ifmt.group_len = (ifmt.radix == 2 || ifmt.radix == 16) ? 4 : 3;
      }

      wlen = c41_int64_fmt(tb, i64, &ifmt);
      wbuf = (uint8_t *) tb;
      // wlen = C41_STR_LEN(wbuf);
      break;
    case 's':
      break;
    }
    if (wlen)
    {
      w = writer_func(output, wbuf, wlen, writer_ctx);
      used_len += w;
      if (w < wlen) { rv = 1; goto l_exit; }
    }
  }
  rv = 0;

l_exit:
  if (used_len_p) *used_len_p = used_len;
  return rv;
}

/* c41_write_fmt ************************************************************/
C41_API ssize_t c41_write_fmt
(
  void * output,
  c41_str_writer_f writer_func,
  void * writer_ctx,
  c41_str_width_f width_func,
  void * width_ctx,
  size_t * used_len_p,
  char const * fmt,
  ...
)
{
  int sc;
  va_list va;
  va_start(va, fmt);
  sc = c41_write_vfmt(output, writer_func, writer_ctx, width_func, width_ctx,
                     used_len_p, fmt, va);
  va_end(va);
  return sc;
}

/* c41_sfmt *****************************************************************/
C41_API ssize_t c41_sfmt
(
  void * out_str,
  size_t out_size,
  char const * fmt,
  ...
)
{
  c41_sbw_t sbw;
  int sc;
  va_list va;
  size_t wlen;

  c41_sbw_init(&sbw, out_str, out_size, C41_SSIZE_MAX);
  va_start(va, fmt);
  sc = c41_write_vfmt(&sbw, c41_sbw_write, NULL, c41_term_utf8_str_width_swf,
                      NULL, &wlen, fmt, va);
  va_end(va);
  return sc ? -sc : (ssize_t) wlen;
}

