/* [c41] Array support - header file
 * Changelog:
 *  - 2013/01/04 Costin Ionescu: initial commit
 *  - 2013/03/05 Costin Ionescu: C41_AN_DECL()
 */

#ifndef _C41_ARRAY_H_
#define _C41_ARRAY_H_


#define C41_AN_DECL(_an_type, _item_type) \
  typedef struct _an_type##_s { _item_type * a; size_t n; } _an_type

C41_AN_DECL(c41_u8an_t, uint8_t);
C41_AN_DECL(c41_u16an_t, uint16_t);
C41_AN_DECL(c41_u32an_t, uint32_t);
C41_AN_DECL(c41_u64an_t, uint64_t);
C41_AN_DECL(c41_s8an_t, int8_t);
C41_AN_DECL(c41_s16an_t, int16_t);
C41_AN_DECL(c41_s32an_t, int32_t);
C41_AN_DECL(c41_s64an_t, int64_t);

#define C41_VECTOR_DECL(_vec_type, _item_type) \
  typedef struct _vec_type##_s { _item_type * a; c41_ma_t * ma_p; \
    size_t n, m; uint_t order; uint8_t ma_rc; } _vec_type

C41_VECTOR_DECL(c41_u8v_t, uint8_t);

/* c41_u8v_init *************************************************************/
C41_INLINE c41_u8v_t * c41_u8v_init (c41_u8v_t * v, c41_ma_t * ma_p, 
                                     uint8_t order)
{
  v->a = NULL;
  v->ma_p = ma_p;
  v->n = v->m = 0;
  v->order = order;
  return v;
}

/* c41_u8v_extend ***********************************************************
 * extends the data buffer to have at least len bytes available more than
 * the current used length.
 * returns a ma error, or 0 for success
 */
C41_API uint_t C41_CALL c41_u8v_extend (c41_u8v_t * v, size_t len); // ret. a ma error

/* c41_u8v_append ***********************************************************
 * increases used length with the given length, or returns NULL if failed;
 * in case of failure, the actual ma error is stored in v->ma_rc;
 * on success, returns the pointer to where appended data can be written
 */
C41_API uint8_t * C41_CALL c41_u8v_append (c41_u8v_t * v, size_t len);

/* c41_u8v_free *************************************************************
 * frees the buffer
 */
C41_API uint_t C41_CALL c41_u8v_free (c41_u8v_t * v);

/* c41_u8v_opt **************************************************************
 * if allocated length is bigger than used length, it reallocates to fit
 * exactly the used data; this is usually the last call after append operations
 * end;
 */
C41_API uint_t C41_CALL c41_u8v_opt (c41_u8v_t * v);

#endif /* _C41_ARRAY_H_ */

