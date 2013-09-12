/* [c41] File System Path - header file
 * This provides an interface to convert utf8 strings or some escaped utf8 form into the
 * native representation used by a fsi interface.
 * The escape form should be able to generate any file path valid even if there
 * is no utf8 equivalent
 *
 * Changelog:
 *  - 2013/02/13 Costin Ionescu: initial commit
 */

#ifndef _C41_FSP_H_
#define _C41_FSP_H_

typedef struct c41_fspi_s c41_fspi_t;

// @bAB - byte 0xAB (unix)
// @xAB - unicode char 0xAB (win)
// @wABCD - unicode char 0xABCD (win)
// @uABCDEF - unicode char 0xABCDEF

struct c41_fspi_s
{
  /* functions to convert path fill the output only if the function succeeds 
   * return size can be larger than fsp_n if there's not enough room to store
   * the translated path and no data is written */
  ssize_t (C41_CALL * fsp_from_utf8)
    (
      uint8_t *                 fsp_a,
      size_t                    fsp_n,
      uint8_t const *           utf8_a,
      size_t                    utf8_n
    );

  ssize_t (C41_CALL * fsp_from_utf8ice) // inconvenient chars escaped (plus the escape char) - this form can be used to encode any file name
    (
      uint8_t *                 fsp_a,
      size_t                    fsp_n,
      uint8_t const *           utf8ice_a,
      size_t                    utf8ice_n
    );

  ssize_t (C41_CALL * utf8_from_fsp)
    (
      uint8_t *                 utf8_a,
      size_t                    utf8_n,
      uint8_t const *           fsp_a,
      size_t                    fsp_n
    );

  ssize_t (C41_CALL * utf8ice_from_fsp)
    (
      uint8_t *                 utf8ice_a,
      size_t                    utf8ice_n,
      uint8_t const *           fsp_a,
      size_t                    fsp_n
    );

  char sep, alt_sep;
};

C41_API void C41_CALL c41_fsp_mswin (c41_fspi_t * fsp_p);
C41_API void C41_CALL c41_fsp_unix (c41_fspi_t * fsp_p);

#endif /* _C41_FSP_H_ */

