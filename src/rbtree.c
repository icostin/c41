/* [c41] Red/Black Trees - functions
 * Changelog:
 *  - 2013/01/04 Costin Ionescu: initial commit
 */

#include <c41.h>

#define OTHER_SIDE(_s) ((_s) ^ 1)
#define IS_RED(_n) ((_n) && (_n)->red)
#define IS_BLACK(_n) (!(IS_RED(_n)))

/* c41_rbtree_init **********************************************************/
C41_API void c41_rbtree_init
(
  c41_rbtree_t * tree_p,
  c41_rbtree_cmp_f cmp,
  void * ctx
)
{
  tree_p->root = NULL;
  tree_p->cmp = cmp;
  tree_p->cmp_ctx = ctx;
}

/* c41_rbtree_find **********************************************************/
C41_API int C41_CALL c41_rbtree_find
(
  c41_rbtree_path_t * path_p,
  c41_rbtree_t * tree_p,
  void * key
)
{
  c41_rbtree_cmp_f cmp = tree_p->cmp;
  void * ctx = tree_p->cmp_ctx;
  c41_rbtree_node_t * n;
  uint_t i;
  int cr;

  path_p->nodes[0] = &tree_p->as_node;
  path_p->sides[0] = 0;
  for (i = 0, n = tree_p->root; n;)
  {
    path_p->nodes[++i] = n;
    cr = cmp(key, n + 1, ctx);
    path_p->sides[i] = cr;
    if (cr < 0) { path_p->last = i; return -1; }
    if (cr == C41_RBTREE_EQUAL) { path_p->last = i; return 0; }
    n = n->links[cr];
  }
  path_p->last = i;
  return 1;
}

/* c41_rbtree_insert ********************************************************/
C41_API void C41_CALL c41_rbtree_insert
(
  c41_rbtree_path_t * path_p,
  c41_rbtree_node_t * node_p
)
{
  uint_t i, gs, ps, ts;
  c41_rbtree_node_t * parent_p;
  c41_rbtree_node_t * grandpa_p;
  c41_rbtree_node_t * uncle_p;
  c41_rbtree_node_t * tmp_p;

  i = path_p->last;
  node_p->left = NULL;
  node_p->right = NULL;
  node_p->red = 1;
  ts = path_p->sides[i];
  path_p->nodes[i]->links[ts] = node_p;

  while (i)
  {
    parent_p = path_p->nodes[i];
    if (!parent_p->red) return; // 0;
    // parent is red (thus it is not the root)
    grandpa_p = path_p->nodes[--i];
    uncle_p = grandpa_p->links[OTHER_SIDE(path_p->sides[i])];
    if (uncle_p && uncle_p->red)
    {
      grandpa_p->red = 1;
      parent_p->red = 0;
      uncle_p->red = 0;
      node_p = grandpa_p;
      --i;
      continue;
    }
    // parent is red, uncle is black or NULL
    if ((gs = path_p->sides[i]) != (ps = path_p->sides[i + 1]))
    {
      // node and parent are on different sides
      grandpa_p->links[gs] = node_p;
      parent_p->links[ps] = tmp_p = node_p->links[gs];
      node_p->links[gs] = parent_p;

      tmp_p = node_p;
      node_p = parent_p;
      parent_p = tmp_p;
    }
    else ps = OTHER_SIDE(gs);
    // node and parent are on same side: gs; ps is set to the 'other' side
    tmp_p = parent_p->links[ps];
    parent_p->links[ps] = grandpa_p;
    parent_p->red = 0;
    grandpa_p->links[gs] = tmp_p;
    grandpa_p->red = 1;
    tmp_p = path_p->nodes[i - 1];
    ts = path_p->sides[i - 1];
    tmp_p->links[ts] = parent_p;
    return; // i == 1;
  }
  // if we processed 'til the top of the path then root is changed
  node_p->red = 0;
  return; // 1;
}

/* c41_rbtree_np ************************************************************/
C41_API c41_rbtree_node_t * C41_CALL c41_rbtree_np
(
  c41_rbtree_path_t * path_p,
  uint8_t side
)
{
  c41_rbtree_node_t * n;
  c41_rbtree_node_t * m;
  uint_t d;

  d = path_p->last;
  n = path_p->nodes[d];
  m = n->links[side];
  if (!m) return NULL;
  path_p->sides[d] = side;
  side = OTHER_SIDE(side);
  for (; m; m = m->links[side])
  {
    ++d;
    path_p->nodes[d] = m;
    path_p->sides[d] = side;
  }
  path_p->sides[d] = C41_RBTREE_EQUAL;
  path_p->last = d;
  return path_p->nodes[d];
}

#if 1
/* c41_rbtree_delete ********************************************************/
C41_API void C41_CALL c41_rbtree_delete
(
  c41_rbtree_path_t * path_p
)
{
  c41_rbtree_node_t * o;
  c41_rbtree_node_t * d;
  c41_rbtree_node_t * p;
  c41_rbtree_node_t * c;
  c41_rbtree_node_t * n;
  c41_rbtree_node_t * s;
  c41_rbtree_node_t * sl;
  c41_rbtree_node_t * sr;
  c41_rbtree_node_t * t;
  c41_rbtree_node_t tmp;
  uint_t cs, ds, od, dd, ns, pd, ps, ss, os;

  od = path_p->last;
  o = path_p->nodes[od]; // the node we want to delete

  if (o->left && o->right)
  {
    ds = path_p->sides[od - 1];
    d = c41_rbtree_np(path_p, ds);
    // now must delete d which has at most 1 non-null child
    dd = path_p->last;
    tmp = *o;
    *o = *d;
    *d = tmp;
    path_p->nodes[od] = d;
    t = path_p->nodes[od - 1];
    os = path_p->sides[od - 1];
    t->links[os] = d;
  }
  else { dd = od; }

  d = o;

  cs = d->left ? C41_RBTREE_LEFT : C41_RBTREE_RIGHT;
  c = d->links[cs]; // c is the only child of d or NULL

  ds = path_p->sides[dd - 1];
  p = path_p->nodes[dd - 1];
  if (d->red)
  {
    // d has no children since it has at most 1 non-null child and
    // both paths must have same ammount of black nodes
    p->links[ds] = NULL;
    return;
  }
  // d is black; it has either no children, or a single red child with no
  // children of its own

  if (c) // same as: (c && c->red)
  {
    p->links[ds] = c;
    c->red = 0;
    return;
  }

  // d is black and has no children (if c is black and its sibling is NULL then
  // it must be NULL itself)

  p->links[ds] = n = NULL;

  for (pd = dd - 1; pd; p = path_p->nodes[--pd])
  {
    // n is always black (in first iteration it is NULL)
    ns = path_p->sides[pd];
    ss = OTHER_SIDE(ns);
    s = p->links[ss]; // this is a non-NULL node
    if (s->red) // implies p is black
    {
      /* del case 2 */
      t = path_p->nodes[pd - 1];
      ps = path_p->sides[pd - 1];
      t->links[ps] = s;
      p->links[ss] = sl = s->links[ns];
      p->red = 1;
      s->links[ns] = p;
      s->red = 0;
      path_p->nodes[pd] = s;
      ++pd;
      s = sl; /* new s is black */
    }
    else
    {
      // s is black
      if (!p->red && IS_BLACK(s->left) && IS_BLACK(s->right))
      {
        /* del case 3 */
        s->red = 1;
        continue;
      }
    }
    // s must be black
    sl = s->links[ns];
    sr = s->links[ss];
    if (p->red && IS_BLACK(sl) && IS_BLACK(sr))
    {
      /* del case 4 */
      p->red = 0;
      s->red = 1;
      break;
    }
    if (IS_RED(sl) && IS_BLACK(sr))
    {
      p->links[ss] = sl;
      s->links[ns] = sl->links[ss];
      s->red = 1;
      sl->links[ss] = s;
      sl->red = 0;
      s = sl; // again new s is black
      sl = s->links[ns];
      sr = s->links[ss];
    }
    if (IS_RED(sr))
    {
      t = path_p->nodes[pd - 1];
      ps = path_p->sides[pd - 1];
      t->links[ps] = s;
      p->links[ss] = sl;
      s->red = p->red;
      p->red = 0;
      sr->red = 0;
      s->links[ns] = p;
    }
    break;
  }

  return;
}
#endif
