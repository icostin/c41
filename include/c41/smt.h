/* [c41] Simple Multi-Threading interface - header file
 * Changelog:
 *  - 2013/01/04 Costin Ionescu: initial commit
 */

#ifndef _C41_SIMPLE_MULTITHREADING_H_
#define _C41_SIMPLE_MULTITHREADING_H_

#define C41_SMT_OK              0
#define C41_SMT_FAIL            1 // generic error code
#define C41_SMT_OTHER           2 // trylock returns this when mutex locked by some other thread
#define C41_SMT_NO_RES          3
#define C41_SMT_NO_CODE         4
#define C41_SMT_ALLOC_ERROR     5
#define C41_SMT_INIT_AND_FREE_ERROR 6
#define C41_SMT_FREE_ERROR      7

typedef struct c41_smt_tid_noimpl_s * c41_smt_tid_t;

typedef struct c41_smt_mutex_s c41_smt_mutex_t;
typedef struct c41_smt_cond_s c41_smt_cond_t;

typedef uint8_t (C41_CALL * c41_smt_thread_f) (void * arg);

typedef struct c41_smt_s c41_smt_t;
struct c41_smt_s
{
  uint_t (C41_CALL * thread_create)
    (
      c41_smt_t * smt_p,
      c41_smt_tid_t * tid_p,
      c41_smt_thread_f func,
      void * arg
    );

  int (C41_CALL * thread_join)
    (
      c41_smt_t * smt_p,
      c41_smt_tid_t tid_p
    );

  uint_t (C41_CALL * mutex_init)
    (
      c41_smt_t * smt_p,
      c41_smt_mutex_t * mutex_p
    );
  uint_t (C41_CALL * mutex_finish)
    (
      c41_smt_t * smt_p,
      c41_smt_mutex_t * mutex_p
    );
  uint_t (C41_CALL * mutex_lock)
    (
      c41_smt_t * smt_p,
      c41_smt_mutex_t * mutex_p
    );
  uint_t (C41_CALL * mutex_trylock)
    (
      c41_smt_t * smt_p,
      c41_smt_mutex_t * mutex_p
    );
  uint_t (C41_CALL * mutex_unlock)
    (
      c41_smt_t * smt_p,
      c41_smt_mutex_t * mutex_p
    );

  uint_t (C41_CALL * cond_init)
    (
      c41_smt_t * smt_p,
      c41_smt_cond_t * cond_p
    );
  uint_t (C41_CALL * cond_finish)
    (
      c41_smt_t * smt_p,
      c41_smt_cond_t * cond_p
    );
  uint_t (C41_CALL * cond_signal)
    (
      c41_smt_t * smt_p,
      c41_smt_cond_t * cond_p
    );
  uint_t (C41_CALL * cond_wait)
    (
      c41_smt_t * smt_p,
      c41_smt_cond_t * cond_p,
      c41_smt_mutex_t * mutex_p
    );

  size_t mutex_size;
  size_t cond_size;

  void * thread_context;
  void * mutex_context;
  void * cond_context;
};

/* c41_smt_thread_create ****************************************************/
C41_INLINE uint_t C41_CALL c41_smt_thread_create
  (
    c41_smt_t * smt_p,
    c41_smt_tid_t * tid_p,
    c41_smt_thread_f func,
    void * arg
  )
{
  return smt_p->thread_create(smt_p, tid_p, func, arg);
}

/* c41_smt_thread_join ******************************************************/
C41_INLINE int C41_CALL c41_smt_thread_join
  (
    c41_smt_t * smt_p,
    c41_smt_tid_t tid_p
  )
{
  return smt_p->thread_join(smt_p, tid_p);
}

/* c41_smt_mutex_init *******************************************************/
C41_INLINE uint_t C41_CALL c41_smt_mutex_init
  (
    c41_smt_t * smt_p,
    c41_smt_mutex_t * mutex_p
  )
{
  return smt_p->mutex_init(smt_p, mutex_p);
}

/* c41_smt_mutex_finish *****************************************************/
C41_INLINE uint_t C41_CALL c41_smt_mutex_finish
  (
    c41_smt_t * smt_p,
    c41_smt_mutex_t * mutex_p
  )
{
  return smt_p->mutex_finish(smt_p, mutex_p);
}

/* c41_smt_mutex_lock *******************************************************/
C41_INLINE uint_t C41_CALL c41_smt_mutex_lock
  (
    c41_smt_t * smt_p,
    c41_smt_mutex_t * mutex_p
  )
{
  return smt_p->mutex_lock(smt_p, mutex_p);
}

/* c41_smt_mutex_trylock ****************************************************/
C41_INLINE uint_t C41_CALL c41_smt_mutex_trylock
  (
    c41_smt_t * smt_p,
    c41_smt_mutex_t * mutex_p
  )
{
  return smt_p->mutex_trylock(smt_p, mutex_p);
}

/* c41_smt_mutex_unlock *****************************************************/
C41_INLINE uint_t C41_CALL c41_smt_mutex_unlock
  (
    c41_smt_t * smt_p,
    c41_smt_mutex_t * mutex_p
  )
{
  return smt_p->mutex_unlock(smt_p, mutex_p);
}

/* c41_smt_cond_init ********************************************************/
C41_INLINE uint_t C41_CALL c41_smt_cond_init
  (
    c41_smt_t * smt_p,
    c41_smt_cond_t * cond_p
  )
{
  return smt_p->cond_init(smt_p, cond_p);
}

/* c41_smt_cond_finish ******************************************************/
C41_INLINE uint_t C41_CALL c41_smt_cond_finish
  (
    c41_smt_t * smt_p,
    c41_smt_cond_t * cond_p
  )
{
  return smt_p->cond_finish(smt_p, cond_p);
}

/* c41_smt_cond_signal ******************************************************/
C41_INLINE uint_t C41_CALL c41_smt_cond_signal
  (
    c41_smt_t * smt_p,
    c41_smt_cond_t * cond_p
  )
{
  return smt_p->cond_signal(smt_p, cond_p);
}

/* c41_smt_cond_wait ********************************************************/
C41_INLINE uint_t C41_CALL c41_smt_cond_wait
  (
    c41_smt_t * smt_p,
    c41_smt_cond_t * cond_p,
    c41_smt_mutex_t * mutex_p
  )
{
  return smt_p->cond_wait(smt_p, cond_p, mutex_p);
}

/* c41_smt_mutex_create *****************************************************/
C41_API uint_t C41_CALL c41_smt_mutex_create
  (
    c41_smt_mutex_t * * mutex_pp,
    c41_smt_t * smt_p,
    c41_ma_t * ma_p
  );

/* c41_smt_mutex_destroy ****************************************************/
C41_API uint_t C41_CALL c41_smt_mutex_destroy
  (
    c41_smt_mutex_t * mutex_p,
    c41_smt_t * smt_p,
    c41_ma_t * ma_p
  );

#endif /* _C41_SIMPLE_MULTITHREADING_H_ */

