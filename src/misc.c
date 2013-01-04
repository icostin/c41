/* [c41] Miscellaneous - functions
 * Changelog:
 *  - 2013/01/04 Costin Ionescu: initial commit
 */

#include <c41.h>

C41_API char const * C41_CALL c41_lib_name ()
{
  return "c41-"
#if C41_STATIC
    "static"
#else
    "dynamic"
#endif
    ;
}

/* c41_lib_minor ************************************************************/
C41_API c41_uint_t C41_CALL c41_lib_minor ()
{
  return C41_LIB_MINOR;
}

/* c41_lib_entry ************************************************************/
C41_API int c41_lib_entry ()
{
  return 0;
}

