/* [c41] Common C Code Collection ver. 1 - main header file
 * Changelog:
 *  - 2013/01/04 Costin Ionescu: initial commit
 */

#ifndef _C41_H_
#define _C41_H_

#if C41_STATIC
# define C41_API
#elif C41_DL_BUILD
# define C41_API C41_DL_EXPORT
#else
# define C41_API C41_DL_IMPORT
#endif

#include "c41/base.h"
#include "c41/ma.h"
#include "c41/smt.h"
#include "c41/misc.h"
#include "c41/pmvops.h"
#include "c41/array.h"
#include "c41/str.h"
#include "c41/io.h"
#include "c41/ilc.h"
#include "c41/fsi.h"
#include "c41/cli.h"
#include "c41/rbtree.h"

#endif /* _C41_H_ */

