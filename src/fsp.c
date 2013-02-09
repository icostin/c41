#include <c41.h>

/* mswin_fsp_from_utf8 ******************************************************/
static ssize_t C41_CALL mswin_fsp_from_utf8
(
  uint8_t *                 fsp_a,
  size_t                    fsp_n,
  uint8_t const *           utf8_a,
  size_t                    utf8_n
)
{
  ssize_t n; // number of utf16 units (16-bit ints)

  n = c41_utf16_len_from_utf8(utf8_a, utf8_n, NULL);
  if (n < 0) return C41_FSI_BAD_PATH;
  if ((size_t) (n + n) <= fsp_n)
  {
    c41_utf16_from_utf8((uint16_t *) fsp_a, utf8_a, utf8_n);
  }
  return (size_t) (n + n);
}

/* mswin_fsp_from_utf8ice ***************************************************/
static ssize_t C41_CALL mswin_fsp_from_utf8ice
(
  uint8_t *                 fsp_a,
  size_t                    fsp_n,
  uint8_t const *           utf8ice_a,
  size_t                    utf8ice_n
)
{
  (void) fsp_a;
  (void) fsp_n;
  (void) utf8ice_a;
  (void) utf8ice_n;
  return -C41_FSI_NO_CODE;
}

/* c41_fsp_mswin ************************************************************/
C41_API void C41_CALL c41_fsp_mswin (c41_fspi_t * fspi_p)
{
  fspi_p->fsp_from_utf8 = mswin_fsp_from_utf8;
  fspi_p->fsp_from_utf8ice = mswin_fsp_from_utf8ice;
  fspi_p->sep = '\\';
  fspi_p->alt_sep = '/';
}

/* unix_fsp_from_utf8 *******************************************************/
static ssize_t C41_CALL unix_fsp_from_utf8
(
  uint8_t *                 fsp_a,
  size_t                    fsp_n,
  uint8_t const *           utf8_a,
  size_t                    utf8_n
)
{
  (void) fsp_a;
  (void) fsp_n;
  (void) utf8_a;
  (void) utf8_n;
  return -C41_FSI_NO_CODE;
}

/* unix_fsp_from_utf8ice ****************************************************/
static ssize_t C41_CALL unix_fsp_from_utf8ice
(
  uint8_t *                 fsp_a,
  size_t                    fsp_n,
  uint8_t const *           utf8ice_a,
  size_t                    utf8ice_n
)
{
  (void) fsp_a;
  (void) fsp_n;
  (void) utf8ice_a;
  (void) utf8ice_n;
  return -C41_FSI_NO_CODE;
}

/* c41_fsp_unix *************************************************************/
C41_API void C41_CALL c41_fsp_unix (c41_fspi_t * fspi_p)
{
  fspi_p->fsp_from_utf8 = unix_fsp_from_utf8;
  fspi_p->fsp_from_utf8ice = unix_fsp_from_utf8ice;
  fspi_p->sep = '/';
  fspi_p->alt_sep = '/';
}

