#include "../include/c41.h"

C41_VECTOR_FNS(c41_esmbv_t, c41_esmb_t, c41_esmbv);

/* c41_esm_init *************************************************************/
C41_API void C41_CALL c41_esm_init
(
    c41_esm_t * esm,
    c41_ma_t * ma,
    size_t ez,
    size_t el,
    size_t fn,
    size_t bl
)
{
    C41_VAR_ZERO(*esm);
    esm->ma = ma;
    esm->ez = ez;
    esm->el = el;
    esm->cn = fn;
    esm->bl = bl;
    esm->pbc = (uint_t) -1;
    esm->ebc = (uint_t) -1;
    esm->fbc = (uint_t) -1;
    c41_esmbv_init(&esm->bv, ma, 6);
}

/* c41_esm_finish ***********************************************************/
C41_API uint_t C41_CALL c41_esm_finish
(
    c41_esm_t * esm
)
{
    uint_t c;
    c41_esmb_t * b, * e;

    for (b = esm->bv.a, e = b + esm->bv.n; b < e; ++b)
    {
        c = c41_ma_free(esm->ma, b->a, esm->ez * b->n);
        if (c) return C41_ESM_MEM_BUG;
        c = c41_ma_free(esm->ma, b->i, sizeof(uint_t) * b->n);
        if (c) return C41_ESM_MEM_BUG;
    }

    c41_esmbv_free(&esm->bv);
    return 0;
}

/* c41_esm_alloc ************************************************************/
C41_API uint_t C41_CALL c41_esm_alloc
(
    c41_esm_t * esm
)
{
    uint_t bx, ex, i;
    c41_esmb_t * b;
    size_t en;

    if (esm->pbc < esm->bv.n)
    {
        // there is a partial block available so fetch one element from there
        bx = esm->pbc; // grab the block index
        b = &esm->bv.a[bx]; // set a pointer to it
        ex = b->f; // grab element index as the first free element
        b->f = b->i[ex]; // update the free-element list head

        b->u++; // update used count
        if (b->u == b->n)
        {
            // this partial block got full
            esm->pbc = b->l; // mark the next linked block as the list head
            // move it to the full list
            b->l = esm->fbc;
            esm->fbc = bx;
        }
    }
    else if (esm->ebc < esm->bv.n)
    {
        // we have an empty block so use that one and move it to partials
        bx = esm->ebc; // get block index from empty block chain
        b = &esm->bv.a[bx]; // set a pointer to it
        esm->ebc = b->l; // start the empty chain from the linked block
        b->l = (uint_t) -1; // no next since partial chain was empty
        esm->pbc = bx;  // this block is the head of the partial chain
        ex = b->f; // grab first free element index
        b->f = b->i[ex]; // move the head to the next linked index
        b->u = 1; // used elements is now 1
    }
    else
    {
        // no empty block and no partial block so we must allocate a new one
        if (esm->cc == esm->el) return (esm->lsc = C41_ESM_LIMIT);
        bx = esm->bv.n;
        b = c41_esmbv_append(&esm->bv, 1); // append a new block
        if (!b) return (esm->lsc = C41_ESM_NO_MEM);
        en = esm->cn;
        if (en > esm->el - esm->cc) en = esm->el - esm->cc;
        if (c41_ma_alloc(esm->ma, (void * *) &b->a, en * esm->ez))
        {
            esm->bv.n -= 1;
            return (esm->lsc = C41_ESM_NO_MEM);
        }
        if (c41_ma_alloc(esm->ma, (void * *) &b->i, en * sizeof(uint_t)))
        {
            c41_ma_free(esm->ma, b->a, en * esm->ez);
            esm->bv.n -= 1;
            return (esm->lsc = C41_ESM_NO_MEM);
        }

        esm->cc += en;
        if (en == esm->cn && esm->cc < esm->el)
        {
            // adjust the allocation size for the next block
            esm->cn <<= 1;
            if (esm->cn > esm->bl) esm->cn = esm->bl;
            if (esm->cn > esm->el - esm->cc) esm->cn = esm->el - esm->cc;
        }

        ex = 0;
        b->n = en;
        b->u = 1;
        b->f = 1;
        // there were no empty blocks and no partial blocks so this
        // becomes the only partial block
        b->l = (uint_t) -1; // there is no next block
        esm->pbc = bx;
        // init the new block's free links
        for (i = 1; i < en; ++i) b->i[i] = i + 1;
    }
    b->i[ex] = 1; // set the ref count of this element to 1
    esm->lbx = bx;
    esm->lex = ex;
    esm->lep = b->a + esm->ez * ex;
    return 0;
}

/* c41_esm_ref **************************************************************/
C41_API uint_t C41_CALL c41_esm_ref
(
    c41_esm_t * esm,
    uint_t bx,
    uint_t ex
)
{
    if ((int) (esm->bv.a[bx].i[ex] += 1) < 0)
    {
        esm->bv.a[bx].i[ex] -= 1;
        return (esm->lsc = C41_ESM_REF_OVERFLOW);
    }
    return 0;
}

/* c41_esm_deref ************************************************************/
C41_API uint_t C41_CALL c41_esm_deref
(
    c41_esm_t * esm,
    uint_t bx,
    uint_t ex
)
{
    c41_esmb_t * b;
    // decrease refs and exit if there are refs left */
    if ((esm->bv.a[bx].i[ex] -= 1)) return 0;
    // now we must free the element
    b = &esm->bv.a[bx];
    if (b->u == b->n)
    {
        // block was full, move it to partials
        esm->fbc = b->l; // update the head of full chain
        b->l = esm->pbc; // link block to old partials head
        esm->pbc = bx; // this block becomes the new partials head
    }
    b->u -= 1; // decrease number of used elements
    b->i[ex] = b->f; // link next free elem as the old free list head
    b->f = ex; // this elem becomes the head of the free list

    return 0;
}

