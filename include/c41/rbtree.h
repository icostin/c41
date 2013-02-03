/* [c41] Red/Black Trees - header file
 * Changelog:
 *  - 2013/01/04 Costin Ionescu: initial commit
 */

#ifndef _C41_RED_BLACK_TREE_H_
#define _C41_RED_BLACK_TREE_H_

#define C41_RBTREE_MAX_DEPTH    0x40

#define C41_RBTREE_LEFT 0
#define C41_RBTREE_SMALLER 0
#define C41_RBTREE_RIGHT 1
#define C41_RBTREE_GREATER 1
#define C41_RBTREE_EQUAL 2
#define C41_RBTREE_ERROR -1


typedef uint_t (C41_CALL * c41_rbtree_cmp_f) (void * key, void * node_payload, 
                                              void * context);

typedef struct c41_rbtree_node_s c41_rbtree_node_t;
struct c41_rbtree_node_s
{
  union
  {
    struct
    {
      c41_rbtree_node_t * left;
      c41_rbtree_node_t * right;
    };
    c41_rbtree_node_t * links[2];
  };

  uint8_t red;
};

typedef union c41_rbtree_u c41_rbtree_t;
union c41_rbtree_u
{
  c41_rbtree_node_t as_node;
  struct
  {
    c41_rbtree_node_t * root;
    c41_rbtree_cmp_f cmp;
    void * cmp_ctx;
  };
};

typedef struct c41_rbtree_path_s c41_rbtree_path_t;
struct c41_rbtree_path_s
{
  c41_rbtree_node_t * nodes[C41_RBTREE_MAX_DEPTH];
  uint8_t sides[C41_RBTREE_MAX_DEPTH];
  uint_t last;
};

/* c41_rbtree_init **********************************************************
 * initialises a red/black tree.
 * it is shaped as a fake node with the root as the left child.
 */
C41_API void c41_rbtree_init
(
  c41_rbtree_t * tree_p,
  c41_rbtree_cmp_f cmp,
  void * ctx
);

/* c41_rbtree_find **********************************************************
 * generates the path while searching for a key
 * returns:
 *  0 - key found
 *  1 - key not found
 * -1 - error while comparing items
 */
C41_API int C41_CALL c41_rbtree_find
(
  c41_rbtree_path_t * path_p,
  c41_rbtree_t * tree_p,
  void * key
);

/* c41_rbtree_insert ********************************************************
 * inserts a new given node by using a search path.
 */
C41_API void C41_CALL c41_rbtree_insert
(
  c41_rbtree_path_t * path_p,
  c41_rbtree_node_t * node_p
);

/* c41_rbtree_delete ********************************************************
 * deletes the node at the end of the given path.
 */
C41_API void C41_CALL c41_rbtree_delete
(
  c41_rbtree_path_t * path_p
);

/* c41_rbtree_np ************************************************************
 * move to next/prev value, updating the path
 * path must not be empty
 */
C41_API c41_rbtree_node_t * C41_CALL c41_rbtree_np
(
  c41_rbtree_path_t * path_p,
  uint8_t side
);

#endif /* _C41_RED_BLACK_TREE_H_ */

