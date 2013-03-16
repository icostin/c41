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
    size_t n, m; } _vec_type

C41_VECTOR_DECL(c41_u8v_t, uint8_t);

/* c41_u8v_init *************************************************************/
C41_INLINE c41_u8v_t * c41_u8v_init (c41_u8v_t * v, c41_ma_t * ma_p)
{
  v->a = NULL;
  v->ma_p = ma_p;
  v->n = v->m = 0;
  return v;
}

C41_API uint_t C41_CALL c41_u8v_extend (c41_u8v_t * v, size_t len); // ret. a ma error
C41_API uint_t C41_CALL c41_u8v_append (c41_u8v_t * v, size_t len);

#endif /* _C41_ARRAY_H_ */

