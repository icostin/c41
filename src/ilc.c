/* [c41] Item linear convertor - functions
 * Changelog:
 *  - 2013/01/04 Costin Ionescu: initial commit
 */

#include <c41.h>

extern char c41_digits[];

/* c41_ilc_status_name ******************************************************/
C41_API char const * C41_CALL c41_ilc_status_name (uint_t sc)
{
  switch (sc)
  {
    C41_CASE_RET_STR(C41_ILC_OK              );
    C41_CASE_RET_STR(C41_ILC_FULL            );
    C41_CASE_RET_STR(C41_ILC_MALFORMED       );
    C41_CASE_RET_STR(C41_ILC_FAIL            );
    C41_CASE_RET_STR(C41_ILC_TRUNC           );
    C41_CASE_RET_STR(C41_ILC_READ_ERROR      );
    C41_CASE_RET_STR(C41_ILC_WRITE_ERROR     );
    C41_CASE_RET_STR(C41_ILC_NO_CODE         );
  }
  return "C41_ILC_UNKNOWN_STATUS";
}

/* c41_ilc_io_buf ***********************************************************/
C41_API uint_t C41_CALL c41_ilc_io_buf
(
  c41_ilc_t * ilc_p,
  c41_io_t * in_p,
  c41_io_t * out_p,
  void * in_buf_p,
  size_t in_buf_nb,
  void * out_buf_p,
  size_t out_buf_nb
)
{
  size_t icap, ocap, ilen, olen, isize, osize;
  uint_t ilc_sc, run;
  uint8_t * ibuf = in_buf_p;
  uint8_t * obuf = out_buf_p;

  icap = in_buf_nb / (isize = ilc_p->isize);
  ocap = out_buf_nb / (osize = ilc_p->osize);

  ilc_p->out_crt = obuf;
  ilc_p->out_end = &obuf[ocap * osize];
  ilc_p->in_crt = NULL;
  ilc_p->in_end = NULL;
  ilc_sc = c41_ilc_start(ilc_p);
  if (ilc_sc) return ilc_sc;
  run = 1;
  do
  {
    if (!C41_ILC_IS_FULL(ilc_p) && C41_ILC_HAS_INPUT(ilc_p))
    {
      ilc_sc = c41_ilc_continue(ilc_p);
      if (!C41_ILC_OK_OR_FULL(ilc_sc)) return ilc_sc;
    }
    if (!C41_ILC_HAS_INPUT(ilc_p))
    {
      if (run == 1)
      {
        if (c41_io_read_igs(in_p, ibuf, icap, &ilen)) return C41_ILC_READ_ERROR;
      }
      else ilen = 0;
      if (!ilen) 
      {
        ilc_sc = c41_ilc_stop(ilc_p); 
        if (!C41_ILC_OK_OR_FULL(ilc_sc)) return ilc_sc;
        run = ilc_sc ? 2 : 0;
      }
      ilc_p->in_crt = &ibuf[0];
      ilc_p->in_end = &ibuf[ilen * isize];
    }
    olen = C41_PTR_DIFF(ilc_p->out_crt, obuf) / osize;
    if (c41_io_write_full(out_p, obuf, olen, NULL))
      return C41_ILC_WRITE_ERROR;
    ilc_p->out_crt = obuf;
  }
  while (run);
  return 0;
}
/* c41_ilc_io ***************************************************************/
C41_API uint_t C41_CALL c41_ilc_io
(
  c41_ilc_t * ilc_p,
  c41_io_t * in_p,
  c41_io_t * out_p
)
{
  uint8_t ibuf[0x400];
  uint8_t obuf[0x400];
  return c41_ilc_io_buf(ilc_p, in_p, out_p, 
                        ibuf, sizeof(ibuf), obuf, sizeof(obuf));
}


/* hex_enc_bb ***************************************************************/
static uint_t C41_CALL hex_enc (c41_ilc_t * ilc_p, int stage)
{
  c41_ilc_mini_t * ilc_mini_p = (c41_ilc_mini_t *) ilc_p;
  uint8_t const * ic;
  uint8_t const * ie;
  uint8_t * oc;
  uint8_t * oe;
  uint8_t b;
  size_t n;

  if (stage == C41_ILC_START) 
  {
    ilc_mini_p->tlen = 0;
    return 0;
  }

  oc = ilc_p->out_crt;
  oe = ilc_p->out_end;

  if (ilc_mini_p->tlen)
  {
    if (oc == oe) return C41_ILC_FULL;
    *oc++ = ilc_mini_p->tbuf[0];
    ilc_mini_p->tlen = 0;
  }

  if (stage == C41_ILC_STOP)
  {
    ilc_p->out_crt = oc;
    return 0;
  }

  /* continue: */
  ic = ilc_p->in_crt;
  ie = ilc_p->in_end;
  n = (oe - oc) >> 1;
  if ((size_t) (ie - ic) < n) n = ie - ic;
  c41_hex(oc, ic, n);
  ilc_p->in_crt = ic += n;
  ilc_p->out_crt = oc += n * 2;
  if (ic == ie) return 0;
  if (oc == oe) return C41_ILC_FULL;
  b = *ic++;
  *oc++ = c41_digits[b >> 4];
  ilc_mini_p->tbuf[0] = c41_digits[b & 0x0F];
  ilc_mini_p->tlen = 1;
  ilc_p->in_crt = ic;
  ilc_p->out_crt = oc;
  return C41_ILC_FULL;
}

/* hex_dec ******************************************************************/
static uint_t C41_CALL hex_dec (c41_ilc_t * ilc_p, int stage)
{
  c41_ilc_mini_t * ilc_mini_p = (c41_ilc_mini_t *) ilc_p;
  uint8_t const * ic;
  uint8_t const * ie;
  uint8_t * oc;
  uint8_t * oe;
  size_t n, m;

  if (stage == C41_ILC_START) 
  {
    ilc_mini_p->tlen = 0;
    return 0;
  }

  if (stage == C41_ILC_STOP)
  {
    if (ilc_mini_p->tlen) return C41_ILC_TRUNC;
    return 0;
  }

  ic = ilc_p->in_crt;
  ie = ilc_p->in_end;
  if (ic == ie) return 0;

  oc = ilc_p->out_crt;
  oe = ilc_p->out_end;
  if (oc == oe) return C41_ILC_FULL;

  if (ilc_mini_p->tlen)
  {
    ilc_mini_p->tbuf[1] = *ic++;
    if (!c41_unhex(oc, ilc_mini_p->tbuf, 1)) return C41_ILC_MALFORMED;
    ++oc;
    ilc_mini_p->tlen = 0;
  }

  n = (ie - ic) >> 1;
  if ((size_t) (oe - oc) < n) n = oe - oc;
  m = c41_unhex(oc, ic, n);
  oc += m;
  ic += m * 2;
  if (m != n)
  {
    ilc_p->in_crt = ic;
    ilc_p->out_crt = oc;
    return C41_ILC_MALFORMED;
  }
  if (ic < ie)
  {
    ilc_mini_p->tlen = 1;
    ilc_mini_p->tbuf[0] = *ic++;
  }

  ilc_p->in_crt = ic;
  ilc_p->out_crt = oc;
  return ic == ie ? 0 : C41_ILC_FULL;
}

/* c41_ilc_hex_enc **********************************************************/
C41_API c41_ilc_t * C41_CALL c41_ilc_hex_enc (c41_ilc_mini_t * ilc_mini_p)
{
  ilc_mini_p->ilc.convert = hex_enc;
  ilc_mini_p->ilc.isize = 1;
  ilc_mini_p->ilc.osize = 1;
  return &ilc_mini_p->ilc;
}

/* c41_ilc_hex_dec **********************************************************/
C41_API c41_ilc_t * C41_CALL c41_ilc_hex_dec (c41_ilc_mini_t * ilc_mini_p)
{
  ilc_mini_p->ilc.convert = hex_dec;
  ilc_mini_p->ilc.isize = 1;
  ilc_mini_p->ilc.osize = 1;
  return &ilc_mini_p->ilc;
}

static uint8_t const cse_dir_map[0x80] =
{
  //          0x00, 0x01, 0x02, 0x03,   0x04, 0x05, 0x06, 0x07,
  /* 00-07 */  '0', 0x01, 0x02, 0x03,   0x04, 0x05, 0x06,  'a',
  /* 08-0F */  'b',  't',  'n',  'v',    'f',  'r', 0x0E, 0x0F,
  /* 10-17 */ 0x10, 0x11, 0x12, 0x13,   0x14, 0x15, 0x16, 0x17,
  /* 18-1F */ 0x18, 0x19, 0x1A, 0x1B,   0x1C, 0x1D, 0x1E, 0x1F,
  /* 20-27 */  ' ',  '!',  '"',  '#',    '$',  '%',  '&', '\'',
  /* 28-2F */  '(',  ')',  '*',  '+',    ',',  '-',  '.',  '/',
  /* 30-37 */  '0',  '1',  '2',  '3',    '4',  '5',  '6',  '7',
  /* 38-3F */  '8',  '9',  ':',  ';',    '<',  '=',  '>',  '?',
  /* 40-47 */  '@',  'A',  'B',  'C',    'D',  'E',  'F',  'G',
  /* 48-4F */  'H',  'I',  'J',  'K',    'L',  'M',  'N',  'O',
  /* 50-57 */  'P',  'Q',  'R',  'S',    'T',  'U',  'V',  'W',
  /* 58-5F */  'X',  'Y',  'Z',  '[',   '\\',  ']',  '^',  '_',
  /* 60-67 */  '`',  'a',  'b',  'c',    'd',  'e',  'f',  'g',
  /* 68-6F */  'h',  'i',  'j',  'k',    'l',  'm',  'n',  'o',
  /* 70-77 */  'p',  'q',  'r',  's',    't',  'u',  'v',  'w',
  /* 78-7F */  'x',  'y',  'z',  '{',    '|',  '}',  '~', 0x7F,
};

static uint8_t const cse_std_raw_map[0x20] =
{
  /* 00-3F */ 0x00, 0x00, 0x00, 0x00,   0xFD, 0xFF, 0xFF, 0xFF,
  /* 40-7F */ 0xFF, 0xFF, 0xFF, 0xEF,   0xFF, 0xFF, 0xFF, 0x7F, 
  /* 80-B0 */ 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00,
  /* C0-FF */ 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00,
};

static uint8_t const cse_safe_raw_map[0x20] =
{
  /* 00-3F */ 0x00, 0x00, 0x00, 0x00,   0x7B, 0xFF, 0xFF, 0xFF,
  /* 40-7F */ 0xFF, 0xFF, 0xFF, 0xEF,   0xFF, 0xFF, 0xFF, 0x7F, 
  /* 80-B0 */ 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00,
  /* C0-FF */ 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00,
};

static uint8_t const cse_hex_map[0x20] =
{
  /* 00-3F */ 0x7F, 0xC0, 0xFF, 0xFF,   0x00, 0x00, 0x00, 0x00,
  /* 40-7F */ 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x80,
  /* 80-B0 */ 0xFF, 0xFF, 0xFF, 0xFF,   0xFF, 0xFF, 0xFF, 0xFF,
  /* C0-FF */ 0xFF, 0xFF, 0xFF, 0xFF,   0xFF, 0xFF, 0xFF, 0xFF,
};

/* cse **********************************************************************/
static uint_t C41_CALL cse (c41_ilc_t * ilc_p, int stage)
{
  c41_ilc_cse_t * ilc_cse_p = (c41_ilc_cse_t *) ilc_p;
  uint8_t const * ic;
  uint8_t const * ie;
  uint8_t * oc;
  uint8_t * oe;
  uint_t r;
  uint8_t m;
  uint8_t o;

  if (stage == C41_ILC_START)
  {
    ilc_cse_p->tofs = 0;
    ilc_cse_p->tend = 0;
    return 0;
  }
  oe = ilc_p->out_end;
  for (;;)
  {
    oc = ilc_p->out_crt;
    if (ilc_cse_p->tend)
    {
      uint_t n, o;
      n = ilc_cse_p->tend - (o = ilc_cse_p->tofs);
      if ((uint_t) (oe - oc) < n) 
      {
        n = oe - oc; 
        ilc_cse_p->tofs += (uint8_t) n;
      }
      else
      {
        ilc_cse_p->tofs = 0;
        ilc_cse_p->tend = 0;
      }

      C41_MEM_COPY(oc, &ilc_cse_p->tbuf[o], n);
      oc += n;
      ilc_p->out_crt = oc;
      if (ilc_cse_p->tend) return C41_ILC_FULL;
    }

    if (stage == C41_ILC_STOP) return 0;

    ic = ilc_p->in_crt;
    ie = ilc_p->in_end;
    if (ic == ie) return 0;
    if (oc == oe) return C41_ILC_FULL;

    for (; ic < ie && oc <= oe; ++ic)
    {
      uint8_t m = 1 << (*ic & 7);
      uint8_t o = *ic >> 3;
      if (ilc_cse_p->raw_a[o] & m) oc += 1;
      else oc += (ilc_cse_p->hex_a[o] & m) ? 4 : 2;
    }

    if (oc > oe) { ie = ic - 1; r = C41_ILC_FULL; }
    else r = 0;

#define X                                       \
    {                                             \
      m = 1 << (*ic & 7);                         \
      o = *ic >> 3;                               \
      if (ilc_cse_p->raw_a[o] & m) *oc++ = *ic;   \
      else                                        \
      {                                           \
        *oc++ = ilc_cse_p->ec;                    \
        if (ilc_cse_p->hex_a[o] & m)              \
        {                                         \
          *oc++ = ilc_cse_p->xc;                  \
          *oc++ = c41_digits[*ic >> 4];           \
          *oc++ = c41_digits[*ic & 15];           \
        }                                         \
        else *oc++ = ilc_cse_p->dir_a[*ic];       \
      }                                           \
    }

    ic = ilc_p->in_crt;
    oc = ilc_p->out_crt;
    for (; ic < ie; ++ic) X;
    ilc_p->in_crt = ic;
    ilc_p->out_crt = oc;

    if (!r) return 0;

    oc = ilc_cse_p->tbuf;
    X;
    ilc_cse_p->tofs = 0;
    ilc_cse_p->tend = (uint8_t) (oc - &ilc_cse_p->tbuf[0]);
    ilc_p->in_crt = ic + 1;
  }
}

/* c41_ilc_cse_std **********************************************************/
C41_API c41_ilc_t * C41_CALL c41_ilc_cse_std (c41_ilc_cse_t * ilc_cse_p)
{
  ilc_cse_p->ilc.convert = cse;
  ilc_cse_p->ilc.isize = 1;
  ilc_cse_p->ilc.osize = 1;
  
  ilc_cse_p->tofs = 0;
  ilc_cse_p->tend = 0;
  ilc_cse_p->ec = '\\';
  ilc_cse_p->xc = 'x';
  ilc_cse_p->raw_a = cse_std_raw_map;
  ilc_cse_p->hex_a = cse_hex_map;
  ilc_cse_p->dir_a = cse_dir_map;
  return &ilc_cse_p->ilc;
}

/* c41_ilc_cse_safe *********************************************************/
C41_API c41_ilc_t * C41_CALL c41_ilc_cse_safe (c41_ilc_cse_t * ilc_cse_p)
{
  ilc_cse_p->ilc.convert = cse;
  ilc_cse_p->ilc.isize = 1;
  ilc_cse_p->ilc.osize = 1;
  
  ilc_cse_p->tofs = 0;
  ilc_cse_p->tend = 0;
  ilc_cse_p->ec = '\\';
  ilc_cse_p->xc = 'x';
  ilc_cse_p->raw_a = cse_safe_raw_map;
  ilc_cse_p->hex_a = cse_hex_map;
  ilc_cse_p->dir_a = cse_dir_map;
  return &ilc_cse_p->ilc;
}

static int8_t const cse_dec_dir_map [] =
{
  //          0x00, 0x01, 0x02, 0x03,   0x04, 0x05, 0x06, 0x07,
  /* 00-07 */   -1,   -1,   -1,   -1,     -1,   -1,   -1,   -1,
  /* 08-0F */   -1,   -1,   -1,   -1,     -1,   -1,   -1,   -1,
  /* 10-17 */   -1,   -1,   -1,   -1,     -1,   -1,   -1,   -1,
  /* 18-1F */   -1,   -1,   -1,   -1,     -1,   -1,   -1,   -1,
  /* 20-27 */  ' ',   -1,  '"',   -1,     -1,   -1,   -1, '\'',
  /* 28-2F */   -1,   -1,   -1,   -1,     -1,   -1,   -1,   -1,
  /* 30-37 */   -2,   -2,   -2,   -2,     -2,   -2,   -2,   -2,
  /* 38-3F */   -1,   -1,   -1,   -1,     -1,   -1,   -1,  '?',
  /* 40-47 */   -1,   -1,   -1,   -1,     -1,   -1,   -1,   -1,
  /* 48-4F */   -1,   -1,   -1,   -1,     -1,   -1,   -1,   -1,
  /* 50-57 */   -1,   -1,   -1,   -1,     -1,   -1,   -1,   -1,
  /* 58-5F */   -1,   -1,   -1,   -1,   '\\',   -1,   -1,   -1,
  /* 60-67 */   -1, 0x07, 0x08,   -1,     -1,   -1, 0x0C,   -1,
  /* 68-6F */   -1,   -1,   -1,   -1,     -1,   -1, 0x0A,   -1,
  /* 70-77 */   -1,   -1, 0x0D,   -1,   0x09,   -1, 0x0B,   -1,
  /* 78-7F */   -3,   -1,   -1,   -1,     -1,   -1,   -1,   -1,
  /* 80-87 */   -1,   -1,   -1,   -1,     -1,   -1,   -1,   -1,
  /* 88-8F */   -1,   -1,   -1,   -1,     -1,   -1,   -1,   -1,
  /* 90-97 */   -1,   -1,   -1,   -1,     -1,   -1,   -1,   -1,
  /* 98-9F */   -1,   -1,   -1,   -1,     -1,   -1,   -1,   -1,
  /* A0-A7 */   -1,   -1,   -1,   -1,     -1,   -1,   -1,   -1,
  /* A8-AF */   -1,   -1,   -1,   -1,     -1,   -1,   -1,   -1,
  /* B0-B7 */   -1,   -1,   -1,   -1,     -1,   -1,   -1,   -1,
  /* B8-BF */   -1,   -1,   -1,   -1,     -1,   -1,   -1,   -1,
  /* C0-C7 */   -1,   -1,   -1,   -1,     -1,   -1,   -1,   -1,
  /* C8-CF */   -1,   -1,   -1,   -1,     -1,   -1,   -1,   -1,
  /* D0-D7 */   -1,   -1,   -1,   -1,     -1,   -1,   -1,   -1,
  /* D8-DF */   -1,   -1,   -1,   -1,     -1,   -1,   -1,   -1,
  /* E0-E7 */   -1,   -1,   -1,   -1,     -1,   -1,   -1,   -1,
  /* E8-EF */   -1,   -1,   -1,   -1,     -1,   -1,   -1,   -1,
  /* F0-F7 */   -1,   -1,   -1,   -1,     -1,   -1,   -1,   -1,
  /* F8-FF */   -1,   -1,   -1,   -1,     -1,   -1,   -1,   -1,
};

/* cse_dec ******************************************************************/
static uint_t C41_CALL cse_dec (c41_ilc_t * ilc_p, int stage)
{
  c41_ilc_cse_dec_t * d = (c41_ilc_cse_dec_t *) ilc_p;
  uint8_t const * ic;
  uint8_t const * ice;
  uint8_t const * ie;
  uint8_t * oc;
  uint8_t * oe;
  uint8_t mode;
  uint8_t ec, b;
  int8_t k;
  size_t l;
  uint_t rc;

  if (stage == C41_ILC_START)
  {
    d->mode = 0;
    return 0;
  }
  ic = ilc_p->in_crt;
  ie = ilc_p->in_end;
  mode = d->mode;
  ec = d->ec;
  if (stage == C41_ILC_STOP) return mode ? C41_ILC_TRUNC : 0;
  oc = ilc_p->out_crt;
  oe = ilc_p->out_end;
  for (; ic < ie;)
  {
    switch (mode)
    {
    case 0:
      l = oe - oc;
      if (l > (size_t) (ie - ic)) l = ie - ic;
      ice = C41_MEM_SCAN(ic, l, ec);
      if (!ice) ice = ic + l;
      if (ic < ice)
      {
        l = ice - ic;
        C41_MEM_COPY(oc, ic, l);
        oc += l;
        ic = ice;
      }
      if (ic == ie) goto l_done;
      if (oc == oe) goto l_full;
      // current char must be ec
      //return C41_ILC_NO_CODE;
      ++ic;
      mode = 1;
      continue;
    case 1: // in escape
      k = cse_dec_dir_map[*ic];
      switch (-k)
      {
      case 1:
        rc = C41_ILC_MALFORMED;
        goto l_exit;
      case 2: // octal escape
        d->val = *ic++ - '0';
        d->vlen = 1;
        mode = 2;
        continue;
      case 3: // hex escape
        d->val = 0;
        d->vlen = 0;
        mode = 3;
        ++ic;
        continue;
      default:
        d->val = k;
        ++ic;
        goto l_esc_done;
      }
      //---
    case 2: // in octal escape
      b = *ic - '0';
      if (b > 7) goto l_esc_done; // { rc = C41_ILC_MALFORMED; goto l_exit; }
      ++ic;
      d->val = d->val * 8 + b;
      if (d->vlen == 2) goto l_esc_done;
      d->vlen += 1;
      continue;
    case 3: // hex escape
      b = *ic;
      if (b <= '9') { b -= '0'; if (b > 9) goto l_bad_hex; }
      else { b |= 0x20; if (b < 'a' || b > 'f') goto l_bad_hex; b -= 'a' - 10; }
      d->val = (d->val << 4) | b;
      ++ic;
      if (d->vlen) goto l_esc_done;
      d->vlen += 1;
      continue;
    l_bad_hex:
      if (!d->vlen) goto l_malformed;
    case 4: // flush d->val
    l_esc_done:
      if (oc == oe) { mode = 4; goto l_full; }
      *oc++ = d->val;
      mode = 0;
      continue;
    default:
      return C41_ILC_NO_CODE;
    }
  }

l_done:
  rc = 0;
l_exit:
  d->mode = mode;
  ilc_p->in_crt = ic;
  ilc_p->out_crt = oc;
  return rc;
l_full:
  rc = C41_ILC_FULL;
  goto l_exit;
l_malformed:
  rc = C41_ILC_MALFORMED;
  goto l_exit;
}

/* c41_ilc_cse_dec **********************************************************/
C41_API c41_ilc_t * C41_CALL c41_ilc_cse_dec (c41_ilc_cse_dec_t * d)
{
  d->ilc.convert = cse_dec;
  d->ilc.isize = 1;
  d->ilc.osize = 1;
  d->ec = '\\';
  d->xc = 'x';
  return &d->ilc;
}

