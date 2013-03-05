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

#endif /* _C41_ARRAY_H_ */

