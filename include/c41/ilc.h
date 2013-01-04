/* [c41] Item linear convertor - header file
 * Changelog:
 *  - 2013/01/04 Costin Ionescu: initial commit
 */

#ifndef _C41_ILC_H_
#define _C41_ILC_H_

#define C41_ILC_OK              0
#define C41_ILC_FULL            1 // output is full and there is still some input data
#define C41_ILC_MALFORMED       2 // bad input data
#define C41_ILC_FAIL            3 // failed to compute (for ex. due to lack of resources)
#define C41_ILC_TRUNC           4 // end() called but input still needed
#define C41_ILC_READ_ERROR      5 // when reading from an io
#define C41_ILC_WRITE_ERROR     6 // when writing to an io
#define C41_ILC_NO_CODE         7 // not implemented

typedef struct c41_ilc_s c41_ilc_t;
typedef struct c41_ilc_type_s c41_ilc_type_t;

#define C41_ILC_START           0
#define C41_ILC_CONTINUE        1
#define C41_ILC_STOP            2

#define C41_ILC_OK_OR_FULL(_sc) ((uint_t) (_sc) <= C41_ILC_FULL)

struct c41_ilc_s
{
  uint_t (C41_CALL * convert) (c41_ilc_t * ilc_p, int stage);
  void const * in_crt;
  void const * in_end;
  void * out_crt;
  void * out_end;
  size_t isize, osize; // size in bytes of input/output items
};

/* c41_ilc_status_name ******************************************************/
C41_API char const * C41_CALL c41_ilc_status_name (uint_t sc);

/* C41_ILC_IN ***************************************************************/
#define C41_ILC_IN(_ilc_p, _a, _n) \
  ((_ilc_p)->in_crt = (_a), (_ilc_p)->in_end = (_a) + (_n))

/* C41_ILC_OUT **************************************************************/
#define C41_ILC_OUT(_ilc_p, _a, _n) \
  ((_ilc_p)->out_crt = (_a), (_ilc_p)->out_end = (_a) + (_n))

/* C41_ILC_IS_FULL **********************************************************/
#define C41_ILC_IS_FULL(_ilc_p) ((_ilc_p)->out_crt == (_ilc_p)->out_end)

/* C41_ILC_HAS_INPUT ********************************************************/
#define C41_ILC_HAS_INPUT(_ilc_p) ((_ilc_p)->in_crt != (_ilc_p)->in_end)

/* c41_ilc_start ************************************************************/
C41_INLINE uint_t C41_CALL c41_ilc_start
(
  c41_ilc_t *   ilc_p
)
{
  return ilc_p->convert(ilc_p, C41_ILC_START);
}

/* c41_ilc_continue *********************************************************/
C41_INLINE uint_t C41_CALL c41_ilc_continue
(
  c41_ilc_t * ilc_p
)
{
  return ilc_p->convert(ilc_p, C41_ILC_CONTINUE);
}

/* c41_ilc_stop *************************************************************/
C41_INLINE uint_t C41_CALL c41_ilc_stop (c41_ilc_t * ilc_p)
{
  return ilc_p->convert(ilc_p, C41_ILC_STOP);
}

/* c41_ilc_mini_t ***********************************************************/
typedef struct c41_ilc_mini_s c41_ilc_mini_t;
struct c41_ilc_mini_s
{
  c41_ilc_t ilc;
  uint8_t tlen;
  uint8_t tbuf[7];
};

typedef struct c41_ilc_cse_s c41_ilc_cse_t;

struct c41_ilc_cse_s
{
  c41_ilc_t ilc;
  uint8_t tofs, tend;
  uint8_t ec, xc; // esc char, hex char
  uint8_t tbuf[4];
  uint8_t const * raw_a; // bitmap; which bytes are outputted raw
  uint8_t const * hex_a; // bitmap; which bytes are hex-escaped
  uint8_t const * dir_a; // byte mapping; all non-raw and non-hex are direct-escapes '\a'
};

typedef struct c41_ilc_cse_dec_s c41_ilc_cse_dec_t;
struct c41_ilc_cse_dec_s
{
  c41_ilc_t ilc;
  uint8_t ec, xc; // esc char, hex char
  uint8_t mode, val, vlen;
};

/* c41_ilc_hex_enc **********************************************************/
C41_API c41_ilc_t * C41_CALL c41_ilc_hex_enc (c41_ilc_mini_t * ilc_mini_p);

/* c41_ilc_hex_dec **********************************************************/
C41_API c41_ilc_t * C41_CALL c41_ilc_hex_dec (c41_ilc_mini_t * ilc_mini_p);

/* c41_ilc_cse_std **********************************************************
 * C String Escaper - standard mode (double quotes escaped)
 */
/* c41_ilc_cse_std **********************************************************/
C41_API c41_ilc_t * C41_CALL c41_ilc_cse_std (c41_ilc_cse_t * ilc_cse_p);

/* c41_ilc_cse_safe *********************************************************
 * C String Escaper - 'safe' mode (escape single/double quotes)
 */
C41_API c41_ilc_t * C41_CALL c41_ilc_cse_safe (c41_ilc_cse_t * ilc_cse_p);

/* c41_ilc_cse_dec **********************************************************
 * cse decoder
 */
C41_API c41_ilc_t * C41_CALL c41_ilc_cse_dec (c41_ilc_cse_dec_t * d);

/* c41_ilc_csu **************************************************************/
C41_API c41_ilc_t * C41_CALL c41_ilc_csu (c41_ilc_mini_t * ilc_mini_p);

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
);

/* c41_ilc_io ***************************************************************/
C41_API uint_t C41_CALL c41_ilc_io
(
  c41_ilc_t * ilc_p,
  c41_io_t * in_p,
  c41_io_t * out_p
);

#endif /* _C41_ILC_H_ */

