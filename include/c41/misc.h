/* [c41] Miscellaneous - header file
 * Changelog:
 *  - 2013/01/04 Costin Ionescu: initial commit
 */

#ifndef _C41_MISC_H_
#define _C41_MISC_H_

#include "base.h"

#define C41_LIB_MINOR 3
#define C41_LIB_IS_COMPATIBLE() (C41_LIB_MINOR <= c41_lib_minor())

C41_API char const * C41_CALL c41_lib_name ();
C41_API c41_uint_t C41_CALL c41_lib_minor ();

#endif /* _C41_MISC_H_ */

