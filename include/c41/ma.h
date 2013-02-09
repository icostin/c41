/* [c41] Memory Allocation interface - header file
 * Changelog:
 *  - 2013/02/03 Costin Ionescu: 
 *    - modified array allocs to use at most C41_SSIZE_MAX bytes
 *  - 2013/01/04 Costin Ionescu: initial commit
 */

#ifndef _C41_MA_H_
#define _C41_MA_H_

#define C41_MA_OK               0
#define C41_MA_NO_MEM           1 // not enough memory for alloc/realloc
#define C41_MA_BAD_PTR          2 // bad ptr passed to realloc/free
#define C41_MA_BAD_LEN          3 // bad length passed to realloc/free
#define C41_MA_CORRUPT          4 // corruption detected
#define C41_MA_DENIED           5 // not allowed - perhaps a soft limit was reached
#define C41_MA_NO_SUP           6 // not supported (not implemented or not feasible)
#define C41_MA_LEN_OVERFLOW     7 // used by array allocations

typedef struct c41_ma_s c41_ma_t;
struct c41_ma_s
{
  void * ctx;
  c41_uint_t (C41_CALL * alloc) 
  (
    void * * data_pp,
    size_t len, // should never be 0
    void * ctx
  );
  c41_uint_t (C41_CALL * realloc) 
  (
    void * * data_pp, // if realloc fails it doesn't change *data_pp
    size_t new_len, // should never be 0
    size_t old_len, // should never be 0
    void * ctx
  );
  c41_uint_t (C41_CALL * free)
  (
    void * data_p,
    size_t len, // should never be 0
    void * ctx
  );
};

/* these functions simply call the right ma function and
 * handle also cases where lengths are 0: */
C41_API c41_uint_t C41_CALL c41_ma_alloc
(
  c41_ma_t * ma_p,
  void * * data_pp,
  size_t len
);

C41_API c41_uint_t C41_CALL c41_ma_alloc_zero_fill
(
  c41_ma_t * ma_p,
  void * * data_pp,
  size_t len
);

C41_API c41_uint_t C41_CALL c41_ma_realloc
(
  c41_ma_t * ma_p,
  void * * data_pp, 
  size_t new_len,
  size_t old_len
);

C41_API c41_uint_t C41_CALL c41_ma_realloc_zero_fill
(
  c41_ma_t * ma_p,
  void * * data_pp, 
  size_t new_len,
  size_t old_len
);

C41_API c41_uint_t C41_CALL c41_ma_free
(
  c41_ma_t * ma_p,
  void * data_p,
  size_t len
);

C41_API char const * C41_CALL c41_ma_status_name
(
  c41_uint_t status
);

static __inline c41_uint_t c41_ma_realloc_array
(
  c41_ma_t * ma_p,
  void * * dptr_p,
  size_t item_size,
  size_t new_count,
  size_t old_count
)
{
  c41_uint_t sc;
  if (new_count == old_count) return 0;
  if (new_count > C41_SSIZE_MAX / item_size) return C41_MA_LEN_OVERFLOW;
  sc = c41_ma_realloc(ma_p, dptr_p, item_size * new_count, item_size * old_count);
  return sc;
}

static __inline c41_uint_t c41_ma_realloc_array_zero_fill
(
  c41_ma_t * ma_p,
  void * * dptr_p,
  size_t item_size,
  size_t new_count,
  size_t old_count
)
{
  c41_uint_t sc;
  if (new_count == old_count) return 0;
  if (new_count > C41_SSIZE_MAX / item_size) return C41_MA_LEN_OVERFLOW;
  sc = c41_ma_realloc_zero_fill(ma_p, dptr_p, item_size * new_count, 
                                item_size * old_count);
  return sc;
}

#define C41_VAR_REALLOC(_ma_p, _var, _n, _o) \
  (c41_ma_realloc_array((_ma_p), (void * *) &(_var), \
                        sizeof(*(_var)), (_n), (_o)))

#define C41_VAR_REALLOCZ(_ma_p, _var, _n, _o) \
  (c41_ma_realloc_array_zero_fill((_ma_p), (void * *) &(_var), \
                                  sizeof(*(_var)), (_n), (_o)))


#define C41_VAR_ALLOC(_ma_p, _var, _n) \
  (_var = NULL, C41_VAR_REALLOC((_ma_p), _var, (_n), 0))

#define C41_VAR_ALLOCZ(_ma_p, _var, _n) \
  (_var = NULL, C41_VAR_REALLOCZ((_ma_p), _var, (_n), 0))

#define C41_VAR_ALLOC1(_ma_p, _var) \
  (c41_ma_alloc((_ma_p), (void * *) &(_var), sizeof(*(_var))))

#define C41_VAR_ALLOC1Z(_ma_p, _var) \
  (c41_ma_alloc_zero_fill((_ma_p), (void * *) &(_var), sizeof(*(_var))))

#define C41_VAR_FREE(_ma_p, _var, _o) \
  (c41_ma_free((_ma_p), (_var), (_o) * sizeof(*(_var))))

#define C41_VAR_FREE1(_ma_p, _var) C41_VAR_FREE(_ma_p, _var, 1)

typedef struct c41_ma_counter_s c41_ma_counter_t;
struct c41_ma_counter_s
{
  c41_ma_t ma;
  c41_ma_t const * worker_ma_p;
  size_t count; // number of blocks allocated
  size_t total_size; // sum of sizes of allocated blocks
  size_t total_limit; // size limit (deny allocs raising total_size above this limit)
  size_t block_limit; // size limit per 1 block (do not alloc blocks larger than this)
  size_t count_limit; // limit for number of blocks
};

/* c41_ma_counter_init ******************************************************/
C41_API c41_ma_t * C41_CALL c41_ma_counter_init
(
  c41_ma_counter_t * mac_p,
  c41_ma_t const * worker_ma_p,
  size_t total_limit, 
  size_t block_limit,
  size_t count_limit
);

#endif /* _C41_MA_H_ */

