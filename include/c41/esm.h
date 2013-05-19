/* [c41] Element Storage Manager - header file
 * Changelog:
 *  - 2013/05/17 Costin Ionescu: initial version
 */
#ifndef _C41_ESM_H_
#define _C41_ESM_H_

/*
 * Elements are structures of the same size that are never used aggregated 
 * and that have a reference counter associated with them. 
 * Elements are not aggregated means that there is never a need to work with
 * elements consecutive in memory, they are always independent.
 * Allocation happens in blocks that contain multiple elements 
 * (the more the better for management at the cost of having a larger number
 * of allocated but not used elements).
 * Blocks are split in 3 categories: empty, partial, full.
 * Element allocation uses first partial blocks, then tries empty blocks and 
 * as last resort allocates a new block.
 *
 */

#include "ma.h"
#include "smt.h"
#include "array.h"

#define C41_ESM_NO_MEM 1
#define C41_ESM_REF_OVERFLOW 2
#define C41_ESM_MEM_BUG 3
#define C41_ESM_LIMIT 4


typedef struct c41_esm_s                        c41_esm_t;
typedef struct c41_esmx_s                       c41_esmx_t;
typedef struct c41_esmb_s                       c41_esmb_t; // block
C41_VECTOR_DECL(c41_esmbv_t, c41_esmb_t);

struct c41_esmb_s
{
    uint8_t * a; // element array
    uint_t * i; // info array (ref-count for used elements, next-free for free)
    uint_t n; // number of allocated elements
    uint_t u; // number of used elements
    uint_t f; // index of first free element in this block
    uint_t l; // link to next block as index
};

struct c41_esm_s
{
    c41_esmbv_t bv; // block vector
    c41_ma_t * ma; // mem allocator
    void * lep; // last element pointer
    uint_t lbx; // last block index
    uint_t lex; // last element index
    int lsc; // last status code
    uint_t pbc; // partial block chain - index to first partial book
    uint_t ebc; // empty block chain - index to first empty book
    uint_t fbc; // full block chain
    size_t ez; /* element size */
    size_t el; // limit: max number of elements to allocate
    size_t cn; // current number of elems to be used when allocating
    size_t bl; // block limit: max number of elems in one block
    size_t cc; // current capacity - sum of number of allocated elems of all blocks
};

struct c41_esmx_s
{
    c41_esm_t mgr;
    c41_smt_mutex_t * mutex;
    c41_smt_t * smt;
};

/* c41_esm_init *************************************************************
 * returns 0 for success. return code is also stored in esm->lsc;
 * el, fn, bl must be non-zero even numbers
 */
C41_API void C41_CALL c41_esm_init
(
    c41_esm_t * esm,
    c41_ma_t * ma, // mem allocator
    size_t ez, // element size
    size_t el, // element limit: max number of elements to allocate 
               // (C41_SSIZE_MAX for no limit)
    size_t fn, // how many elements to allocate in the first block
               // this gets doubled up to bl
    size_t bl // block limit: max number of elements in any allocated block
);

/* c41_esm_finish ***********************************************************
 * returns 0 for success. return code is also stored in esm->lsc;
 */
C41_API uint_t C41_CALL c41_esm_finish
(
    c41_esm_t * esm
);

/* c41_esm_alloc ************************************************************
 * Allocates 1 element and sets its ref-count to 1.
 * Returns 0 for success and lep, lbx and lex are filled in.
 * To free that element use c41_esm_deref().
 */
C41_API uint_t C41_CALL c41_esm_alloc
(
    c41_esm_t * esm
);

/* c41_esm_ref **************************************************************/
C41_API uint_t C41_CALL c41_esm_ref
(
    c41_esm_t * esm,
    uint_t bx,
    uint_t ex
);

/* c41_esm_deref ************************************************************/
C41_API uint_t C41_CALL c41_esm_deref
(
    c41_esm_t * esm,
    uint_t bx,
    uint_t ex
);

/* c41_esm_is_valid *********************************************************
 * Checks if given indexes are a valid entry into a block (doesn't have to be
 * allocated)
 * Returns 0/1.
 */
C41_API uint_t C41_CALL c41_esm_is_valid
(
    c41_esm_t * esm,
    uint_t bx,
    uint_t ex
);
C41_API uint_t C41_CALL c41_esm_is_used
(
    c41_esm_t * esm,
    uint_t bx,
    uint_t ex
);
C41_API uint_t C41_CALL c41_esm_is_free
(
    c41_esm_t * esm,
    uint_t bx,
    uint_t ex
);

#endif /* _C41_ESM_H_ */

