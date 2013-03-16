/* [c41] Byte I/O streams - header file
 * Changelog:
 *  - 2013/01/04 Costin Ionescu: initial commit
 */

#ifndef _C41_IO_H_
#define _C41_IO_H_


typedef struct c41_io_s c41_io_t;
typedef struct c41_io_type_s c41_io_type_t;


#define C41_IO_READABLE         0x01
#define C41_IO_WRITEABLE        0x02
#define C41_IO_SEEKABLE         0x04
#define C41_IO_CLOSED           0x08
#define C41_IO_BLOCKING         0x10 // reads/writes are blocking; if not set means it's either non-blocking or the blocking state is not known 

struct c41_io_s
{
  c41_io_type_t * io_type_p;
  int64_t size; // cached file size - updated by seek to end
  int64_t pos; // cached file position - updated by any seek, read, write
  uint8_t flags;
  uint8_t error; // last error; successful ops do not change this value
};

/* anchors */
#define C41_IO_BEGIN 0
#define C41_IO_CURRENT 1
#define C41_IO_END 2

/* statuses */
#define C41_IO_OK               0
#define C41_IO_FAILED           1 // generic error
#define C41_IO_EOF              2 // end of file
#define C41_IO_SIGNAL           3 // interrupted by signal/apc
#define C41_IO_MEDIA_ERROR      4 // low-level media error
#define C41_IO_NOT_SEEKABLE     5
#define C41_IO_NOT_READABLE     6
#define C41_IO_NOT_WRITABLE     7
#define C41_IO_ALREADY_CLOSED   8
#define C41_IO_PIPE_ERROR       9
#define C41_IO_WOULD_BLOCK      10 // read/write would block on a file opened in non-blocking more

struct c41_io_type_s
{
  uint_t (C41_CALL * read) (c41_io_t * io_p, void * data, size_t size,
                         size_t * used_size_p);
  uint_t (C41_CALL * write) (c41_io_t * io_p, void const * data, size_t size,
                          size_t * used_size_p);
  uint_t (C41_CALL * seek64) (c41_io_t * io_p, int64_t disp, int anchor);
  /*^ updates io_p->pos */
  uint_t (C41_CALL * truncate) (c41_io_t * io_p);
  uint_t (C41_CALL * close) (c41_io_t * io_p);
  size_t unit_size; // bytes per unit; sizes and positions are in number of units
};

C41_API char const * C41_CALL c41_io_status_name (uint_t status);

/* c41_io_read **************************************************************/
C41_API uint_t C41_CALL c41_io_read
(
  c41_io_t *    io_p,
  void *        data,
  size_t        size,
  size_t *      used_size_p
);

/* c41_io_write *************************************************************/
C41_API uint_t C41_CALL c41_io_write
(
  c41_io_t *    io_p,
  void const *  data,
  size_t        size,
  size_t *      used_size_p
);

/* c41_io_write_full ********************************************************
 * repeatedly calls c41_io_write() if interrupted by signals
 */
C41_API uint_t C41_CALL c41_io_write_full
(
  c41_io_t *    io_p,
  void const *  data,
  size_t        size,
  size_t *      used_size_p
);

/* c41_io_read_igs **********************************************************
 * read ignoring signals
 * repeatedly calls read() if interrupted by signals
 */
C41_API uint_t C41_CALL c41_io_read_igs
(
  c41_io_t *    io_p,
  void *        data,
  size_t        size,
  size_t *      used_size_p
);

#define C41_IO_WRITE_BIN_LIT(_io_p, _lit, _used_p) \
  (c41_io_write_full((_io_p), (_lit), sizeof(_lit), (_used_p)))

#define C41_IO_WRITE_STR_LIT(_io_p, _lit, _used_p) \
  (c41_io_write_full((_io_p), (_lit), sizeof(_lit) - 1, (_used_p)))

/* c41_io_seek **************************************************************/
C41_API uint_t C41_CALL c41_io_seek
(
  c41_io_t *    io_p,
  ssize_t       disp,
  int           anchor
);

/* c41_io_seek64 ************************************************************/
C41_API uint_t C41_CALL c41_io_seek64
(
  c41_io_t *    io_p,
  int64_t       disp,
  int           anchor
);

/* c41_io_size **************************************************************
 * seeks to end and back to current position, updating io_p->size with
 * current file size.
 */
C41_API uint_t C41_CALL c41_io_get_size
(
  c41_io_t *    io_p
);

/* c41_io_truncate **********************************************************/
C41_API uint_t C41_CALL c41_io_truncate (c41_io_t * io_p);

/* c41_io_close *************************************************************/
C41_API uint_t C41_CALL c41_io_close (c41_io_t * io_p);

/* c41_io_str_writer ********************************************************/
C41_API size_t C41_CALL c41_io_str_writer
(
  void * io_p,
  uint8_t const * data,
  size_t len,
  void * ctx
);

/* c41_io_fmt ***************************************************************
 * returns number of bytes written or,
 * on error - the negated error code from c41_write_vfmt()
 * if -1 (writer error) inspect io_p->error for the i/o error
 */
C41_API ssize_t C41_CALL c41_io_fmt (c41_io_t * io_p, char const * fmt, ...);

/* c41_io_p64read ***********************************************************
 * seeks to given position and reads; seek & read are NOT done atomically
 * will modify the current position
 * tries to read len bytes at a certain position (looping if interrupted by signals)
 * used_size_p can be NULL
 * returns:
 *  0           len bytes read
 *  C41_IO_EOF  less than len bytes read
 *  other       some I/O error
 */
C41_API uint_t C41_CALL c41_io_p64read
(
  c41_io_t *    io_p,
  void *        data,
  int64_t       pos,
  size_t        len,
  size_t *      used_size_p
);

#endif /* _C41_IO_H_ */

