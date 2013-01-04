/* [c41] Memory Allocation interface - functions
 *
 * This file contains implementation of support functions for handling
 * memory allocation interfaces, it does not contain platform-dependent
 * implementations.
 *
 * Changelog:
 *  - 2013/01/04 Costin Ionescu: initial commit
 */

#include <stdarg.h>
#include <c41.h>

/* c41_ma_alloc *************************************************************/
C41_API c41_uint_t C41_CALL c41_ma_alloc
(
  c41_ma_t * ma_p,
  void * * data_pp,
  size_t len
)
{
  if (!len)
  {
    *data_pp = NULL;
    return 0;
  }

  return ma_p->alloc(data_pp, len, ma_p->ctx);
}

/* c41_ma_alloc_zero_out ****************************************************/
C41_API c41_uint_t C41_CALL c41_ma_alloc_zero_fill
(
  c41_ma_t * ma_p,
  void * * data_pp,
  size_t len
)
{
  c41_uint_t sc;

  if (!len)
  {
    *data_pp = NULL;
    return 0;
  }

  sc = ma_p->alloc(data_pp, len, ma_p->ctx);

  if (sc) return sc;

  C41_MEM_ZERO(*data_pp, len);
  return 0;
}
/* c41_ma_realloc ***********************************************************/
C41_API c41_uint_t C41_CALL c41_ma_realloc
(
  c41_ma_t * ma_p,
  void * * data_pp, 
  size_t new_len,
  size_t old_len
)
{
  void * data_p;

  if (!old_len)
  {
    if (*data_pp) return C41_MA_BAD_PTR;
    return c41_ma_alloc(ma_p, data_pp, new_len);
  }
  if (!new_len) 
  {
    data_p = *data_pp;
    *data_pp = NULL;
    return c41_ma_free(ma_p, data_p, old_len);
  }
  return ma_p->realloc(data_pp, new_len, old_len, ma_p->ctx);
}

/* c41_ma_realloc_zero_fill *************************************************/
C41_API c41_uint_t C41_CALL c41_ma_realloc_zero_fill
(
  c41_ma_t * ma_p,
  void * * data_pp, 
  size_t new_len,
  size_t old_len
)
{
  c41_uint_t sc;

  sc = c41_ma_realloc(ma_p, data_pp, new_len, old_len);
  if (!sc && new_len > old_len) 
    C41_MEM_ZERO(((uint8_t *) *data_pp) + old_len, new_len - old_len);
  return sc;
}


/* c41_ma_free **************************************************************/
C41_API c41_uint_t C41_CALL c41_ma_free
(
  c41_ma_t * ma_p,
  void * data_p,
  size_t len
)
{
  if (!len)
  {
    return data_p ? C41_MA_BAD_PTR : 0;
  }
  return ma_p->free(data_p, len, ma_p->ctx);
}


/* c41_ma_status_name *******************************************************/
C41_API char const * C41_CALL c41_ma_status_name
(
  c41_uint_t status
)
{
#define N(_x) case _x: return #_x
  switch (status)
  {
    N(C41_MA_OK           );
    N(C41_MA_NO_MEM       );
    N(C41_MA_BAD_PTR      );
    N(C41_MA_BAD_LEN      );
    N(C41_MA_CORRUPT      );
    N(C41_MA_DENIED       );
    N(C41_MA_NO_SUP       );
    N(C41_MA_LEN_OVERFLOW );
  default:
    return "C41_MA_UNSPECIFIED";
  }
#undef N
}

/* counter_alloc ************************************************************/
static c41_uint_t C41_CALL counter_alloc
(
  void * * data_pp,
  size_t len, // should never be 0
  void * ctx
)
{
  c41_uint_t sc;
  c41_ma_counter_t * mac_p = ctx;
  size_t new_total;

  if (mac_p->count == mac_p->count_limit) return C41_MA_DENIED;
  if (len > mac_p->block_limit) return C41_MA_DENIED;
  new_total = mac_p->total_size + len;
  if (new_total < len || new_total > mac_p->total_limit)
    return C41_MA_DENIED;

  sc = mac_p->worker_ma_p->alloc(data_pp, len, mac_p->worker_ma_p->ctx);
  if (sc) return sc;
  mac_p->total_size = new_total;
  mac_p->count += 1;
  return 0;
}

/* counter_realloc **********************************************************/
static c41_uint_t C41_CALL counter_realloc 
(
  void * * data_pp, // if realloc fails it doesn't change *data_pp
  size_t new_len, // should never be 0
  size_t old_len, // should never be 0
  void * ctx
)
{
  c41_uint_t sc;
  c41_ma_counter_t * mac_p = ctx;
  size_t new_total;

  if (new_len > mac_p->block_limit) return C41_MA_DENIED;
  new_total = mac_p->total_size + new_len - old_len;
  if (old_len < new_len && 
      (new_total < mac_p->total_size || new_total > mac_p->total_limit))
    return C41_MA_DENIED;

  sc = mac_p->worker_ma_p->realloc(data_pp, new_len, old_len, 
                                   mac_p->worker_ma_p->ctx);
  if (sc) return sc;
  mac_p->total_size = new_total;
  return 0;
}

/* counter_free *************************************************************/
static c41_uint_t C41_CALL counter_free
(
  void * data_p,
  size_t len, // should never be 0
  void * ctx
)
{
  c41_ma_counter_t * mac_p = ctx;
  c41_uint_t sc;

  if (mac_p->total_size < len) return C41_MA_BAD_LEN;
  sc = mac_p->worker_ma_p->free(data_p, len, mac_p->worker_ma_p->ctx);
  if (sc) return sc;
  mac_p->count -= 1;
  mac_p->total_size -= len;
  return 0;
}

/* c41_ma_counter_init ******************************************************/
C41_API c41_ma_t * C41_CALL c41_ma_counter_init
(
  c41_ma_counter_t * mac_p,
  c41_ma_t const * worker_ma_p,
  size_t total_limit, 
  size_t block_limit,
  size_t count_limit
)
{
  mac_p->ma.ctx = mac_p;
  mac_p->ma.alloc = counter_alloc;
  mac_p->ma.realloc = counter_realloc;
  mac_p->ma.free = counter_free;
  mac_p->worker_ma_p = worker_ma_p;
  mac_p->total_limit = total_limit;
  mac_p->block_limit = block_limit;
  mac_p->count_limit = count_limit;
  mac_p->count = 0;
  mac_p->total_size = 0;
  return &mac_p->ma;
}

