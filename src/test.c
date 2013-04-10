/* [c41] test program
 * Changelog:
 *  - 2013/01/04 Costin Ionescu: initial commit
 */

#include <string.h>
#include <stdio.h>
#include <c41.h>

#define C(_cond) if ((_cond)) ; else do { fprintf(stderr, "Error: (%s) failed (line %u)\n", #_cond, __LINE__); return 1; } while (0)

int sbw_test ()
{
  c41_sbw_t sbw;
  char buf[0x40];
  size_t z;

  buf[0x0F] = 0xFF;
  buf[0x10] = 0xFE;
  C(c41_sbw_init(&sbw, buf, sizeof(buf), 0x10) == &sbw);
  C(sbw.buf_limit == 0x0F);
  C(c41_sbw_write(&sbw, (uint8_t *) "abc ", 4, NULL) == 4);
  z = c41_sbw_write(&sbw, (uint8_t *) "def ghi jkl\xC2\x80", 13, NULL);
  C(z == 12);
  C(sbw.len == 0x0F);
  C(C41_MEM_EQUAL(buf, "abc def ghi jkl\0\xFE", 0x11));
  return 0;
}

int pmvops_test ()
{
  char buf[0x40];
  uint32_t cp;

  C41_VAR_MINUS_ONE(cp);
  C(cp == 0xFFFFFFFF);
  C41_VAR_ZERO(buf);
  C41_MEM_FILL(buf, 0x20, 'a');
  C41_MEM_FILL(buf, 8, 'b');
  C41_MEM_COPY(&buf[0x10], &buf[0], 8);
  buf[9] = 'c';
  C41_MEM_MOVE(&buf[4], &buf[6], 4);
  // printf("- c41 mem ops:    %s\n", buf);
  C(C41_MEM_EQUAL(&buf[0], "bbbbbbacacaaaaaabbbbbbbbaaaaaaaa", 0x21));
  return 0;
}

int hex_test ()
{
  char buf[0x40];
  size_t l;
  uint16_t u16a[] = { 0x1234, 0xABCD };

  C(!strcmp(c41_hexz(buf, "\xAB\xCD", 2), "ABCD"));
  C(!strcmp(c41_hex16sz(buf, u16a, 2), "1234 ABCD"));

  memset(buf, 0, sizeof(buf));
  l = c41_unhex(buf, "4E4f70715-", 5);
  C(l == 4);
  C(!memcmp(buf, "NOpq", 4));

  return 0;
}

int fmt_test ()
{
  c41_sbw_t sbw;
  c41_int_fmt_t ifmt;
  char buf[0x40];
  size_t ulen;
  ssize_t slen;

  ifmt.sign_mode = 0;
  ifmt.radix = 0x10;
  ifmt.radix_mark = 0;
  ifmt.group_sep = '_';
  ifmt.group_len = 2;
  ifmt.min_digit_count = 9;
  ulen = c41_int64_fmt(buf, -0x12345, &ifmt);
  C(ulen == 14);
  C(C41_MEM_EQUAL(buf, "-0_00_01_23_45", 14));

  c41_sbw_init(&sbw, buf, sizeof(buf) - 1, -1);
  C(c41_write_fmt(&sbw, c41_sbw_write, NULL,
                  &c41_str_width_blind, NULL, &ulen,
                  "A: $.*s world!", (size_t) 4, "Hello") == 0);
  C(C41_MEM_EQUAL(buf, "A: Hell world!", 15));
  slen = c41_sfmt(buf, sizeof(buf), "$.7G2d", -0x123456);
  // printf("l: %ld. b: %s\n", (long) slen, buf);
  C(slen == 11);
  C(C41_MEM_EQUAL(buf, "-0_12_34_56", 11));
  return 0;
}

int utf8_test ()
{
  uint32_t cp;
  C(c41_utf8_char_len(0x7F) == 1);
  C(c41_utf8_char_len(0x7FF) == 2);
  C(c41_utf8_char_decode_raw("\x82Xcaca", 5, &cp) == -2);

  return 0;
}

typedef struct rbt0_s rbt0_t;
struct rbt0_s
{
  c41_rbtree_node_t node;
  int data;
};

uint_t C41_CALL rbt0_cmp (void * key, void * payload, void * ctx)
{
  int * ik = key;
  int * ip = payload;
  (void) ctx;
  return (*ik == *ip) ? C41_RBTREE_EQUAL : (*ik > *ip);
}

void rbt0_print (rbt0_t * n)
{
  if (n)
  {
    if (n->node.left)
    {
      printf("(");
      rbt0_print((rbt0_t *) n->node.left);
      printf(") ");
    }
    printf("%d:%c", n->data, n->node.red ? 'R' : 'B');
    if (n->node.right)
    {
      printf(" (");
      rbt0_print((rbt0_t *) n->node.right);
      printf(")");
    }
  }
}

void rbt0_path_print (c41_rbtree_path_t * p)
{
  rbt0_t * r;
  uint_t i;
  for (i = 1; i <= p->last; ++i)
  {
    r = (rbt0_t *) p->nodes[i];
    printf(" %d:%c", r->data, p->sides[i] == 2 ? 'c' : (p->sides[i] ? 'r' : 'l'));
  }

}

int rbtree_test ()
{
  rbt0_t a[0x100];
  c41_rbtree_t tree;
  c41_rbtree_path_t p;
  rbt0_t * r;
  int i, j, c;

  r = NULL;
  c41_rbtree_init(&tree, rbt0_cmp, NULL);

  printf("rbtree test:\n");
  for (i = 1, j = 0; i; i = (i + 11) % 32, ++j)
  {
    printf("tree: {");
    r = (rbt0_t *) tree.root;
    rbt0_print(r);
    printf("}.  path:");
    c = c41_rbtree_find(&p, &tree, &i);
    if (c <= 0)
    {
      printf("- find returned %d for key %d\n", c, i);
      return 1;
    }
    rbt0_path_print(&p); printf(" for adding %d\n", i);
    a[j].data = i;
// if (c41_rbtree_insert(&p, &a[j].node)) r = (rbt0_t *) p.hyper_root.left;
    c41_rbtree_insert(&p, &a[j].node);
  }

  for (i = 1, j = 0; i; i = (i + 11) % 32, ++j)
  {
    printf("tree: {");
    r = (rbt0_t *) tree.root;
    rbt0_print(r);
    printf("}.  path:");
    c = c41_rbtree_find(&p, &tree, &i);
    if (c != 0)
    {
      printf("- find returned %d for key %d\n", c, i);
      return 1;
    }
    rbt0_path_print(&p); printf(" for deleting %d\n", i);
// if (c41_rbtree_insert(&p, &a[j].node)) r = (rbt0_t *) p.hyper_root.left;
    c41_rbtree_delete(&p);
  }

  return 0;
}

int fsp_test ()
{
  c41_fspi_t fspi;
  uint8_t buf[0x100];
  ssize_t z;

  c41_fsp_mswin(&fspi);
  z = fspi.fsp_from_utf8(buf, 8, (uint8_t *) "caca", 4);
  C(z == 10);
  z = fspi.fsp_from_utf8(buf, 10, (uint8_t *) "caca", 4);
  C(z == 10 && !memcmp(buf, "c\0a\0c\0a\0\0\0", 10));

  z = fspi.fsp_from_utf8(buf, 2, (uint8_t *) "\xEF\xBB\xBF", 2);
  C(z == -C41_FSI_BAD_PATH);

  z = fspi.fsp_from_utf8(buf, 4, (uint8_t *) "\xEF\xBB\xBF", 3);
  C(z == 4);

  z = fspi.fsp_from_utf8(buf, 6, (uint8_t *) "\xF4\x8F\xBF\xBF", 3);
  C(z == -C41_FSI_BAD_PATH);

  z = fspi.fsp_from_utf8(buf, 8, (uint8_t *) "\xF4\x8F\xBF\xBF", 4);
  C(z == 6);

  c41_fsp_unix(&fspi);
  z = fspi.fsp_from_utf8(buf, 4, (uint8_t *) "caca", 4);
  C(z == 5);
  z = fspi.fsp_from_utf8(buf, 5, (uint8_t *) "caca", 4);
  C(z == 5);

  return 0;
}

int main (int argc, char const * const * argv)
{
  C(C41_LIB_IS_COMPATIBLE());
  C(argc);
  C(argv[0][0]);
  printf("c41 test\n");

  // printf("- argc: %u\n", argc);
  // printf("- argv[0]: %s\n", argv[0]);
  printf("- c41 lib name:   %s\n", c41_lib_name());
  printf("- c41 lib minor:  %u\n", c41_lib_minor());

  C(!pmvops_test()); printf("- pmvops test passed\n");
  C(!hex_test()); printf("- hex test passed\n");
  C(!utf8_test()); printf("- utf8 test passed\n");
  C(!sbw_test()); printf("- sbw test passed\n");
  C(!fmt_test()); printf("- fmt test passed\n");
  C(c41_term_char_width(1) == -1);
  C(c41_term_char_width(0xAD) == -1);
  C(c41_term_char_width(0x2329) == 2);
  C(c41_term_char_width(0x1F773) == 1);
  C(!rbtree_test()); printf("- rbtree test passed\n");
  C(!fsp_test()); printf("- fsp test passed\n");

  return 0;
}

