#ifndef _C41_FSP_H_
#define _C41_FSP_H_

typedef struct c41_fspi_s c41_fspi_t;

// _bAB - byte 0xAB (unix)
// _xAB - unicode char 0xAB (win)
// _wABCD - unicode char 0xABCD (win)
// _uABCDEF - unicode char 0xABCDEF

struct c41_fspi_s
{
  /* functions to convert path fill the output only if the function succeeds */
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

  char sep, alt_sep;
};

C41_API void C41_CALL c41_fsp_mswin (c41_fspi_t * fsp_p);
C41_API void C41_CALL c41_fsp_unix (c41_fspi_t * fsp_p);

#endif /* _C41_FSP_H_ */

