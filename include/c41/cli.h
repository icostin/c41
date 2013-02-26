/* [c41] Command line interface - header file
 * Changelog:
 *  - 2013/01/04 Costin Ionescu: initial commit
 *  - 2013/02/26 Costin Ionescu: 
 *    - interfaces linked with pointers
 *    - added fsp/smt interfaces
 */

#ifndef _C41_CLI_H_
#define _C41_CLI_H_

#include "base.h"
#include "ma.h"
#include "io.h"
#include "fsi.h"

typedef struct c41_cli_s c41_cli_t;
struct c41_cli_s
{
  char const * program;
  char const * const * arg_a;
  uint_t arg_n;
  c41_io_t * stdin_p;
  c41_io_t * stdout_p;
  c41_io_t * stderr_p;
  c41_ma_t * ma_p;
  c41_smt_t * smt_p;
  c41_fsi_t * fsi_p;
  c41_fspi_t * fspi_p;
};

#endif /* _C41_CLI_H_ */

