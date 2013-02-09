/* [c41] Double linked lists - header file
 * Changelog:
 * - 2013/02/03 Costin Ionescu:
 *   - initial structs & macros
 */
#ifndef _C41_DLIST_H_
#define _C41_DLIST_H_

typedef union c41_np_u c41_np_t;

#define C41_NEXT 0
#define C41_PREV 1

union c41_np_u
{
  struct
  {
    c41_np_t * next;
    c41_np_t * prev;
  };
  c41_np_t * (links[2]);
};

static __inline c41_np_t * c41_dlist_init (c41_np_t * list_p)
{
  list_p->next = list_p->prev = list_p;
  return list_p;
}

static __inline void c41_dlist_ins (c41_np_t * anchor_p, c41_np_t * to_ins_p, int dir)
{
  c41_np_t * n;
  to_ins_p->links[dir] = n = anchor_p->links[dir];
  to_ins_p->links[dir ^ 1] = anchor_p;
  n->links[dir ^ 1] = to_ins_p;
  anchor_p->links[dir] = to_ins_p;
}

#define C41_DLIST_APPEND(_list, _obj_p, _np_field) \
  (c41_dlist_ins(&(_list), &(_obj_p)->_np_field, C41_PREV))

#define C41_DLIST_PREPEND(_list, _obj_p, _np_field) \
  (c41_dlist_ins(&(_list), &(_obj_p)->_np_field, C41_NEXT))

#endif /* _C41_DLIST_H_ */

