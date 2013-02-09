/* [c41] Strings - header file
 * Changelog:
 *  - 2013/01/30 Costin Ionescu: added c41_utf8_char_encode_raw()
 *  - 2013/01/04 Costin Ionescu: initial commit
 */

#ifndef _C41_STR_H_
#define _C41_STR_H_

#include <stdarg.h>

typedef struct c41_int_fmt_s c41_int_fmt_t;
struct c41_int_fmt_s
{
  uint8_t sign_mode; // 0 - use '-' for negatives, 1 - always show sign, 2 - unsigned
  uint8_t radix;
  char radix_mark; // 'x' for 0x prefix, 0 for no prefix
  char group_sep; // char to separate groups
  uint8_t group_len; // 3 for 1,234,567; 4 for 1234,5678; 0 for no grouping
  uint8_t min_digit_count; // for 0 padding
};

C41_API void * C41_CALL c41_hex (void * out, void const * in, size_t len);
/*^ out buffer must contain at least 2 * len bytes */

C41_API void * C41_CALL c41_hexz (void * out, void const * in, size_t len);
/* out buffer must contain at least 2 * len + 1 bytes */


C41_API size_t C41_CALL c41_unhex (void * out, void const * in, size_t len);
/* len is the number of bytes to decode, not the number of bytes in input
 * therefore in must point to at least len * 2 hex digits
 * out must have len bytes available
 * returns: number of bytes decoded successfully
 */

C41_API int C41_CALL c41_utf8_char_len (uint32_t cp);
/* returns number of bytes needed to encode the given codepoint;
 * returns -1 if cp is above range of valid codepoints
 * -3 if cp is in range of surrogate codepoints
 */

/* c41_utf8_char_decode_raw *************************************************/
/** - decodes a single UTF8 code point, without any validation on the
 *    value of the codepoint, or if the encoding is overly long;
 *  - returns the number of bytes used in the encoding
 *  - overly long encodings or invalid codepoints can be detected by comparing
 *    the return value with the result of c41_utf8_char_len(*out)
 *  - error return values:
 *    - -1: len too short
 *    - -2: invalid lead byte
 *    - -(2 + pos): invalid continuation byte at position pos
 */
C41_API int C41_CALL c41_utf8_char_decode_raw
(
  void const * data,
  size_t len,
  uint32_t * out
);

/* c41_utf8_char_decode_strict **********************************************/
C41_API int C41_CALL c41_utf8_char_decode_strict
(
  void const * vdata,
  size_t len,
  uint32_t * out
);
/* same returns as c41_utf8_char_decode_raw() plus the following:
 *  -8  overly long encoded char
 *  -9  invalid code point (> 0x110000)
 *  -10 surrogate code point; encoded length for these code points is 3
 */

/* c41_utf8_char_encode_raw *************************************************/
/* encodes a given code point into buffer out; the buffer must have at least
 * c41_utf8_char_len(cp) bytes available.
 */
C41_API uint_t C41_CALL c41_utf8_char_encode_raw
(
  void * out,
  uint32_t cp
);

/* c41_mutf8_str_decode *****************************************************/
C41_API int C41_CALL c41_mutf8_str_decode
(
  void const * vdata,
  size_t len,
  uint16_t * out_a,
  size_t out_n,
  size_t * in_len_p,
  size_t * out_len_p
);
/* this function allows unpaired or wrongly-paired surrogates;
 * the output is a uint16_t array that can contain any values 
 * return values:
 *  0   decoded ok; *out_len_p is set to number of decoded chars
 *  1   truncated output; 
 *      - *out_len_p has the length needed for the full string
 *      - *in_len_p has the number of bytes read from the input until the 
 *        output was filled;
 *      - you can 'resume' the decoding by using the above 2 lengths:
 *        - input buffer: vdata = (uint8_t const *) vdata + *in_len_p
 *        - output buffer size: (*out_len_p - out_n)
 * -1   incomplete mutf8 char found
 * -2   invalid lead bytes
 * -3..-7 invalid continuation byte 
 * -8   overly long encoded char 
 * -9   invalid code point ( > 0x10000) 
 * -10  plain NUL character encountered (instead of encoded NUL: \xC2\x80) 
 * on error (ret val < 0) *in_len_p is set to position where error occurs
 **/


/* c41_utf16_len_from_utf8 **************************************************/
C41_API ssize_t C41_CALL c41_utf16_len_from_utf8
(
  uint8_t const * in_a,
  size_t in_n, 
  size_t * err_pos_p // can be NULL 
);

/* c41_utf16_from_utf8 ******************************************************/
C41_API size_t C41_CALL c41_utf16_from_utf8
(
  uint16_t * out_a,
  uint8_t const * in_a,
  size_t in_n
);

/* c41_term_char_width ******************************************************/
C41_API int C41_CALL c41_term_char_width (uint32_t cp);
/* cp must be a valid code-point (< 0x110000)
 * returns 0 for zero-width (for ex. combining) chars, 1 normal, 2 wide,
 * -1 non-printable
 */

/* c41_term_char_width_wctx *************************************************/
C41_API int C41_CALL c41_term_char_width_wctx (uint32_t cp, void * ctx);

/* c41_term_utf8_str_width **************************************************/
C41_API ssize_t C41_CALL c41_term_utf8_str_width (void const * s, size_t len);
/*  calculates the terminal width for given string
 *  returns width >= 0 on success, < 0 on bad utf8 or non-printable char found
 */

/* c41_term_utf8_str_width_swf **********************************************/
C41_API ssize_t C41_CALL c41_term_utf8_str_width_swf 
(
  uint8_t const * s, 
  size_t len,
  void * unused
);

/* c41_int64_fmt ************************************************************/
C41_API size_t C41_CALL c41_int64_fmt
(
  void * out,
  int64_t value,
  c41_int_fmt_t const * fmt_p
);

/* c41_str_writer_f *********************************************************/
/* function pointer used by the string formatter
 * the string to be written is guaranteed to be valid UTF8
 * this function should write to output the data speficied by (data, len)
 * len should never be zero
 * on success it returns len meaning all len bytes were written
 * on error it should return how many bytes were succesfully written and
 * an the nature of the error can be indicated in ctx; this should always be
 * less than len
 */
typedef size_t (C41_CALL * c41_str_writer_f)
                                (
                                  void * output,
                                  uint8_t const * data,
                                  size_t len,
                                  void * ctx
                                );

typedef struct c41_sbw_s c41_sbw_t;
struct c41_sbw_s
{
  uint8_t * data;
  size_t len, count_len, buf_limit, count_limit;
};

/* c41_sbw_init *************************************************************/
C41_API void * C41_CALL c41_sbw_init
(
  c41_sbw_t * sbw_p,
  void * buffer,
  size_t buf_limit,
  size_t count_limit
);

/* c41_sbw_write ************************************************************/
C41_API size_t C41_CALL c41_sbw_write
(
  void * output,
  uint8_t const * data,
  size_t len,
  void * ctx
);

/* c41_str_width_f **********************************************************
 * function pointer used by the string formatter
 * this function should calculate the width of the give utf8 string
 * the string passed should be formed of valid unicode codepoints
 * on success, the number of width units is returned
 * on error, a negative number which is discarded; to specify a certain error
 * one should use the context to write that
 * this can be used to return terminal-style widths (0,1,2 for each printable char)
 * or even pixels for variable-width fonts
 */
typedef ssize_t (C41_CALL * c41_str_width_f)
                                (
                                  uint8_t const * str,
                                  size_t len,
                                  void * ctx
                                );

/* c41_str_width_blind ******************************************************/
C41_API ssize_t C41_CALL c41_str_width_blind
(
  uint8_t const * str,
  size_t len,
  void * ctx
);

/* c41_utf8_str_measure *****************************************************/
/* measures the 'width' of a string
 * most likely you want wf = c41_term_char_width_wctx and wf_ctx = NULL
 * returns:
 *  0   all b bytes were parsed successfully
 *  1   reached code-point limit c with no errors
 *  2   reached width limit w with no errors
 * -1   malformed utf8 string
 * -2   non-printable char
 */
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
);

/* c41_write_vfmt ***********************************************************
 * formats a string and writes it (potentially in chunks) to specified output
 * return values:
 *  0 - all ok
 *  1 - writer error
 *  2 - width compute error
 *  3 - bad fmt string
 *  4 - bad grouping count
 *  9 - feature not implemented
 * *used_len_p is filled with how much was successfully written in all cases
 *
 * formatting commands:
 */
C41_API int c41_write_vfmt
(
  void * output,
  c41_str_writer_f writer_func,
  void * writer_ctx,
  c41_str_width_f width_func,
  void * width_ctx,
  size_t * used_len_p,
  char const * fmt,
  va_list va
);

/* c41_write_fmt ************************************************************
 * passes args to vfmt
 */
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
);

/* c41_sfmt *****************************************************************
 * formats into the given buffer; always ends with a NUL char;
 * returns the size needed for the full formatted string
 * if return value is greater than out_size - 1 then the output was truncated
 * it uses the width function c41_term_utf8_str_width_swf
 * on error returns the negated error codes from c41_write_vfmt
 */
C41_API ssize_t c41_sfmt
(
  void * out_str,
  size_t out_size,
  char const * fmt,
  ...
);

#endif /* _C41_STR_H_ */

