/* [c41] File System Interface - header file
 * Changelog:
 *  - 2013/01/04 Costin Ionescu: initial commit
 */

#ifndef _C41_FSI_H_
#define _C41_FSI_H_

#include "base.h"
#include "io.h"

#define C41_FSI_OK              0x00
#define C41_FSI_CLOSE_FAILED    0x01 // file_destroy() fails to close
#define C41_FSI_MISSING_ACCESS  0x02 // no read and no write access requested when calling file_open
#define C41_FSI_MISSING_ACTION  0x03 // file_open with reject open & reject create
#define C41_FSI_BAD_PATH        0x04 // path name invalid
#define C41_FSI_OPEN_FAILED     0x05 // generic error when open fails
#define C41_FSI_NO_RES          0x06

/* open mode flags */
#define C41_FSI_EXF_MASK        0x0003
#define C41_FSI_EXF_REJECT      0x0000
#define C41_FSI_EXF_OPEN        0x0001
#define C41_FSI_EXF_TRUNC       0x0002
#define C41_FSI_NEWF_MASK       0x0004
#define C41_FSI_NEWF_REJECT     0x0000
#define C41_FSI_NEWF_CREATE     0x0004
#define C41_FSI_NON_BLOCKING    0x0008
#define C41_FSI_WRITE_THROUGH   0x0010
#define C41_FSI_INHERITABLE     0x0020
#define C41_FSI_DEL_ON_CLOSE    0x0040
#define C41_FSI_READ            0x0080
#define C41_FSI_WRITE           0x0100

#define C41_FSI_PERM_SHIFT      0x09
#define C41_FSI_UR              (1 << (C41_FSI_PERM_SHIFT + 8))
#define C41_FSI_UW              (1 << (C41_FSI_PERM_SHIFT + 7))
#define C41_FSI_UX              (1 << (C41_FSI_PERM_SHIFT + 6))
#define C41_FSI_GR              (1 << (C41_FSI_PERM_SHIFT + 5))
#define C41_FSI_GW              (1 << (C41_FSI_PERM_SHIFT + 4))
#define C41_FSI_GX              (1 << (C41_FSI_PERM_SHIFT + 3))
#define C41_FSI_OR              (1 << (C41_FSI_PERM_SHIFT + 2))
#define C41_FSI_OW              (1 << (C41_FSI_PERM_SHIFT + 1))
#define C41_FSI_OX              (1 << (C41_FSI_PERM_SHIFT + 0))

typedef struct c41_fsi_s c41_fsi_t;
struct c41_fsi_s
{
  void * context;

  ssize_t (C41_CALL * utf8_to_fsi_name)
    (
      uint8_t *                 fsi_path_a,
      size_t                    fsi_path_n,
      uint8_t const *           utf8_a,
      size_t                    utf8_n
    );
  // returns number of bytes needed to store fsi_path
  // negative values mean path is invalid or could not compute

  ssize_t (C41_CALL * utf8esc_to_fsi_name)
    (
      uint8_t *                 fsi_path_a,
      size_t                    fsi_path_n,
      uint8_t const *           utf8_a,
      size_t                    utf8_n
    );
  // returns number of bytes needed to store fsi_path
  // negative values mean path is invalid or could not compute

  uint_t (C41_CALL * file_open)
    (
      c41_io_t * *              io_pp,
      uint8_t const *           path_a,
      size_t                    path_n,
      uint32_t                  mode,
      void *                    context
    );

  uint_t (C41_CALL * file_destroy)
    (
      c41_io_t *                io_p,
      void *                    context
    );
  /* TODO:
   * dir_enum_start, dir_enum_next, dir_enum_finish
   * dir_create, dir_delete,
   * dir_current_get, dir_current_set
   * file_delete,
   * move
   */
};

C41_API char const * c41_fsi_status_name (uint_t sc);

/* c41_file_open ************************************************************/
C41_API uint_t C41_CALL c41_file_open
(
  c41_fsi_t *                   fsi_p,
  c41_io_t * *                  io_pp,
  uint8_t const *               path_a,
  size_t                        path_n,
  uint32_t                      mode
);

/* c41_file_destroy *********************************************************/
C41_API uint_t C41_CALL c41_file_destroy
(
  c41_fsi_t *                   fsi_p,
  c41_io_t *                    io_p
);

#endif /* _C41_FSI_H_ */

