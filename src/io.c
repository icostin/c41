/* [c41] Byte I/O streams - functions
 * Changelog:
 *  - 2013/01/17 Costin Ionescu: added c41_io_p64read()
 *  - 2013/01/04 Costin Ionescu: initial commit
 */

#include <c41.h>

C41_API char const * C41_CALL c41_io_status_name (uint_t status)
{
  switch (status)
  {
    C41_CASE_RET_STR(C41_IO_OK               );
    C41_CASE_RET_STR(C41_IO_FAILED           );
    C41_CASE_RET_STR(C41_IO_EOF              );
    C41_CASE_RET_STR(C41_IO_SIGNAL           );
    C41_CASE_RET_STR(C41_IO_MEDIA_ERROR      );
    C41_CASE_RET_STR(C41_IO_NOT_SEEKABLE     );
    C41_CASE_RET_STR(C41_IO_NOT_READABLE     );
    C41_CASE_RET_STR(C41_IO_NOT_WRITABLE     );
    C41_CASE_RET_STR(C41_IO_ALREADY_CLOSED   );
    C41_CASE_RET_STR(C41_IO_PIPE_ERROR       );
    C41_CASE_RET_STR(C41_IO_WOULD_BLOCK      );
  }
  return "C41_IO_UNKNOWN_STATUS";
}

/* c41_io_read **************************************************************/
C41_API uint_t C41_CALL c41_io_read
(
  c41_io_t *    io_p,
  void *        data,
  size_t        size,
  size_t *      used_size_p
)
{
  size_t tmp;
  uint_t sc;

  if (!used_size_p) used_size_p = &tmp;
  if (!size) { *used_size_p = 0; return 0; }
  sc = io_p->io_type_p->read(io_p, data, size, used_size_p);
  io_p->pos += *used_size_p;
  if (sc) io_p->error = (uint8_t) sc;
  return sc;
}

/*  *************************************************************************/
C41_API uint_t C41_CALL c41_io_write
(
  c41_io_t *    io_p,
  void const *  data,
  size_t        size,
  size_t *      used_size_p
)
{
  size_t tmp;
  uint_t sc;

  if (!used_size_p) used_size_p = &tmp;
  if (!size) { *used_size_p = 0; return 0; }
  sc = io_p->io_type_p->write(io_p, data, size, used_size_p);
  io_p->pos += *used_size_p;
  if (sc) io_p->error = (uint8_t) sc;
  return sc;
}

/* c41_io_seek64 ************************************************************/
C41_API uint_t C41_CALL c41_io_seek64
(
  c41_io_t *    io_p,
  int64_t       disp,
  int           anchor
)
{
  uint_t sc;

  sc = io_p->io_type_p->seek64(io_p, disp, anchor);
  if (!sc)
  {
    if (anchor == C41_IO_END) io_p->size = io_p->pos - disp;
  }
  else io_p->error = (uint8_t) sc;

  return sc;
}

/* c41_io_seek **************************************************************/
C41_API uint_t C41_CALL c41_io_seek
(
  c41_io_t *    io_p,
  ssize_t       disp,
  int           anchor
)
{
  return c41_io_seek64(io_p, (int64_t) disp, anchor);
}

/* c41_io_truncate **********************************************************/
C41_API uint_t C41_CALL c41_io_truncate (c41_io_t * io_p)
{
  uint_t sc;
  sc = io_p->io_type_p->truncate(io_p);
  if (sc) io_p->error = (uint8_t) sc;
  return sc;
}

/* c41_io_close *************************************************************/
C41_API uint_t C41_CALL c41_io_close (c41_io_t * io_p)
{
  uint_t sc;

  sc = io_p->io_type_p->close(io_p);
  if (!sc) io_p->flags |= C41_IO_CLOSED;
  else io_p->error = (uint8_t) sc;
  return sc;
}

/* c41_io_write_full ********************************************************/
C41_API uint_t C41_CALL c41_io_write_full
(
  c41_io_t *    io_p,
  void const *  data,
  size_t        size,
  size_t *      used_size_p
)
{
  size_t ws;
  size_t tmp;
  uint_t iosc;

  if (!used_size_p) used_size_p = &tmp;
  *used_size_p = 0;
  while (size)
  {
    iosc = c41_io_write(io_p, data, size, &ws);
    *used_size_p += ws;
    if (iosc && iosc != C41_IO_SIGNAL) return iosc;
    size -= ws;
    data = C41_PTR_OFS(data, ws);
  }
  return 0;
}

/* c41_io_read_igs **********************************************************/
C41_API uint_t C41_CALL c41_io_read_igs
(
  c41_io_t *    io_p,
  void *        data,
  size_t        size,
  size_t *      used_size_p
)
{
  size_t rs;
  size_t tmp;
  uint_t iosc;

  if (!used_size_p) used_size_p = &tmp;
  *used_size_p = 0;
  do
  {
    iosc = c41_io_read(io_p, data, size, &rs);
    *used_size_p += rs;
    if (iosc && iosc != C41_IO_SIGNAL) return iosc;
    size -= rs;
    data = C41_PTR_OFS(data, rs);
  }
  while (iosc);

  return 0;
}

/* c41_io_str_writer ********************************************************/
C41_API size_t C41_CALL c41_io_str_writer
(
  void * io_p,
  uint8_t const * data,
  size_t len,
  void * ctx
)
{
  uint_t iosc;
  size_t rs;
  (void) ctx;
  iosc = c41_io_write_full(io_p, data, len, &rs);
  return iosc ? 0 : rs;
}

/* c41_io_fmt ***************************************************************/
C41_API ssize_t C41_CALL c41_io_fmt (c41_io_t * io_p, char const * fmt, ...)
{
  int sc;
  va_list va;
  size_t wlen;

  va_start(va, fmt);
  sc = c41_write_vfmt(io_p, c41_io_str_writer, NULL, 
                      c41_term_utf8_str_width_swf, NULL, &wlen, fmt, va);
  va_end(va);
  return sc ? -sc : (ssize_t) wlen;
}

/* c41_io_get_size **********************************************************/
C41_API uint_t C41_CALL c41_io_get_size
(
  c41_io_t *    io_p
)
{
  int64_t pos;
  uint_t c;
  pos = io_p->pos;
  c = c41_io_seek64(io_p, 0, C41_IO_END); // this updates io_p->size
  if (c) return c;
  return c41_io_seek64(io_p, pos, C41_IO_BEGIN);
}

/* c41_io_p64read ***********************************************************/
C41_API uint_t C41_CALL c41_io_p64read
(
  c41_io_t *    io_p,
  void *        data,
  int64_t       pos,
  size_t        len,
  size_t *      used_size_p
)
{
  uint_t c;
  uint8_t * d;
  uint8_t * e;
  size_t rlen;
  c = c41_io_seek64(io_p, pos, C41_IO_BEGIN);
  if (c) { if (used_size_p) *used_size_p = 0; return c; }
  
  for (d = data, e = d + len; d < e; )
  {
    c = c41_io_read(io_p, d, e - d, &rlen);
    if (!rlen) { c = C41_IO_EOF; break; }
    d += rlen;
    if (c && c != C41_IO_SIGNAL) break;
  }
  if (used_size_p) *used_size_p = d - (uint8_t *) data;

  return c;
}

