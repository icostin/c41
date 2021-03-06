#include <c41.h>

/* c41_smt_mutex_create *****************************************************/
C41_API uint_t C41_CALL c41_smt_mutex_create
(
  c41_smt_mutex_t * * mutex_pp,
  c41_smt_t * smt_p,
  c41_ma_t * ma_p
)
{
  uint_t ma_rc;
  uint_t smt_rc;

  ma_rc = c41_ma_alloc(ma_p, (void * *) mutex_pp, smt_p->mutex_size);
  if (ma_rc) return C41_SMT_ALLOC_ERROR;

  smt_rc = c41_smt_mutex_init(smt_p, *mutex_pp);
  if (smt_rc)
  {
    ma_rc = c41_ma_free(ma_p, *mutex_pp, smt_p->mutex_size);
    return C41_SMT_INIT_AND_FREE_ERROR;
  }
  return smt_rc;
}

/* c41_smt_mutex_destroy ****************************************************/
C41_API uint_t C41_CALL c41_smt_mutex_destroy
(
  c41_smt_mutex_t * mutex_p,
  c41_smt_t * smt_p,
  c41_ma_t * ma_p
)
{
  uint_t ma_rc;
  uint_t smt_rc;

  smt_rc = c41_smt_mutex_finish(smt_p, mutex_p);
  if (smt_rc) return smt_rc;
  ma_rc = c41_ma_free(ma_p, mutex_p, smt_p->mutex_size);
  if (ma_rc) return C41_SMT_FREE_ERROR;
  return 0;
}

/* c41_smt_cond_create ******************************************************/
C41_API uint_t C41_CALL c41_smt_cond_create
(
  c41_smt_cond_t * * cond_pp,
  c41_smt_t * smt_p,
  c41_ma_t * ma_p
)
{
  uint_t ma_rc;
  uint_t smt_rc;

  ma_rc = c41_ma_alloc(ma_p, (void * *) cond_pp, smt_p->cond_size);
  if (ma_rc) return C41_SMT_ALLOC_ERROR;

  smt_rc = c41_smt_cond_init(smt_p, *cond_pp);
  if (smt_rc)
  {
    ma_rc = c41_ma_free(ma_p, *cond_pp, smt_p->cond_size);
    if (ma_rc) return C41_SMT_INIT_AND_FREE_ERROR;
  }
  return smt_rc;
}

/* c41_smt_cond_destroy *****************************************************/
C41_API uint_t C41_CALL c41_smt_cond_destroy
(
 c41_smt_cond_t * cond_p,
  c41_smt_t * smt_p,
  c41_ma_t * ma_p
)
{
  uint_t ma_rc;
  uint_t smt_rc;

  smt_rc = c41_smt_cond_finish(smt_p, cond_p);
  if (smt_rc) return smt_rc;
  ma_rc = c41_ma_free(ma_p, cond_p, smt_p->cond_size);
  if (ma_rc) return C41_SMT_FREE_ERROR;
  return 0;
}

