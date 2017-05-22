/*----------------------------------------------------------------------
  File    : clomax.c
  Contents: prefix tree management for closed and maximal item sets
  Author  : Christian Borgelt
  History : 2009.10.08 file created
            2009.10.09 item order direction added
            2009.10.31 merge functions improved (for empty arguments)
            2009.11.12 root node made fixed element in tree structure
            2010.02.10 function cmt_project() added (project a c/m tree)
            2010.03.11 function cmt_add() improved (create new nodes)
            2010.03.12 function cmt_xproj() added (project a c/m tree)
            2010.06.21 generalized to support type RSUPP (int/double)
            2010.07.21 early pruning of projection items added
            2010.07.22 closed/maximal item set filter functions added
            2010.07.29 functions prune_pos/_neg() simplified
            2010.12.07 added some explicit type casts (for C++)
            2012.04.10 function cm_addnc() added (no perfect ext. check)
            2012.04.11 separate memory management system for each tree
            2014.07.16 insertion loop in cmt_add() simplified
            2014.07.23 check for a valid tree based on associated item
            2016.09.26 bug in cmt_show() fixed (no name for item = -1)
            2016.11.20 bug in cmt_create() fixed (init. of keep[])
----------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include "clomax.h"
#ifdef STORAGE
#include "storage.h"
#endif

/* This version uses a top-down structure for the repository trees  */
/* and their processing. A frequent pattern tree structure was also */
/* tried, but turned out to be slower while needing more memory.    */

/*----------------------------------------------------------------------
  Preprocessor Definitions
----------------------------------------------------------------------*/
#define pos(x,y)   ((x) < (y))  /* macros for item comparison */
#define neg(x,y)   ((x) > (y))  /* (ascending and descending) */

#define COPYERR   ((CMNODE*)-1) /* error indicator for xcopy */

/*----------------------------------------------------------------------
  Prefix Tree Functions
----------------------------------------------------------------------*/

CMTREE* cmt_create (MEMSYS *mem, int dir, ITEM size)
{                               /* --- create a c/m prefix tree */
  CMTREE *cmt;                  /* created c/m prefix tree */

  assert(size > 0);             /* check the function arguments */
  cmt = (CMTREE*)malloc(sizeof(CMTREE) +(size_t)(size-1) *sizeof(ITEM));
  if (!cmt) return NULL;        /* create a c/m prefix tree */
  cmt->mem  = (mem) ? mem : ms_create(sizeof(CMNODE), 4095);
  if (!cmt->mem) { free(cmt); return NULL; }
  cmt->size = size;             /* note item count and order direction*/
  cmt->dir  = (dir < 0) ? -1 : +1;
  cmt->item = -2;               /* mark the tree as empty/invalid */
  cmt->max  = -2;               /* (special item/support) */
  memset(cmt->keep, 0, (size_t)size *sizeof(ITEM));
  cmt->root.sibling = cmt->root.children = NULL;
  cmt->root.item    = -1;       /* initialize the root node */
  cmt->root.supp    =  0;       /* (no item, no other nodes) */
  return cmt;                   /* return the created prefix tree */
}  /* cmt_create() */

/*--------------------------------------------------------------------*/

void cmt_clear (CMTREE *cmt)
{                               /* --- clear a c/m prefix tree */
  assert(cmt);                  /* check the function argument */
  ms_clear(cmt->mem, 0);        /* clear memory management system */
  cmt->max  = -2;               /* prefix tree is now empty/invalid */
  cmt->item = -2;               /* (special support/item) */
  cmt->root.sibling = cmt->root.children = NULL;
  cmt->root.supp    =  0;       /* reinitialize the root node */
}  /* cmt_clear() */

/*--------------------------------------------------------------------*/

void cmt_delete (CMTREE *cmt, int delms)
{                               /* --- delete a c/m prefix tree */
  assert(cmt);                  /* check the function arguments */
  if      (delms >  0) ms_delete(cmt->mem);
  else if (delms >= 0) ms_clear (cmt->mem, 1);
  free(cmt);                    /* delete memory system or nodes */
}  /* cmt_delete() */           /* and delete the base structure */

/*--------------------------------------------------------------------*/

int cmt_add (CMTREE *cmt, const ITEM *items, ITEM n, RSUPP supp)
{                               /* --- add an item set to a c/m tree */
  ITEM   i;                     /* buffer for an item */
  CMNODE **p;                   /* pointer to insertion position */
  CMNODE *node;                 /* to insert new nodes */

  assert(cmt                    /* check the function arguments */
  &&    (items || (n <= 0)) && (supp >= 0));
  if (supp > cmt->max)          /* update maximum item set support */
    cmt->max = supp;            /* (prefix tree is now valid) */
  /* In principle, it has to be checked whether the given item set   */
  /* contains at least one item and whether the first item coincides */
  /* with the item associated with the tree (if there is such item). */
  /* Otherwise cmt->max may not be updated with the support.         */
  node = &cmt->root;            /* start at the root node */
  do {                          /* traverse the items of the set */
    if (supp > node->supp)      /* adapt the node support */
      node->supp = supp;        /* (root represents empty set) */
    if (--n < 0) return 0;      /* if all items are processed, abort */
    i = *items++;               /* get the next item in the set and */
    p = &node->children;        /* traverse the list of children */
    if (cmt->dir < 0) while (*p && ((*p)->item > i)) p = &(*p)->sibling;
    else              while (*p && ((*p)->item < i)) p = &(*p)->sibling;
    node = *p;                  /* find the item/insertion position */
  } while (node && (node->item == i));
  node = (CMNODE*)ms_alloc(cmt->mem);
  if (!node) return -1;         /* create a new prefix tree node */
  node->supp    = supp;         /* store support of the item set */
  node->item    = i;            /* and the current/last item */
  node->sibling = *p;           /* insert the created node */
  *p = node;                    /* into the sibling list */
  while (--n >= 0) {            /* traverse the rest of the items */
    node = node->children = (CMNODE*)ms_alloc(cmt->mem);
    if (!node) return -1;       /* create a new prefix tree node */
    node->supp    = supp;       /* store support of the item set */
    node->item    = *items++;   /* and the current/last item */
    node->sibling = NULL;       /* there are no siblings yet */
  }
  node->children = NULL;        /* last created node is a leaf */
  return 0;                     /* return 'ok' */
}  /* cmt_add() */

/*--------------------------------------------------------------------*/

RSUPP cmt_get (CMTREE *cmt, const ITEM *items, ITEM n)
{                               /* --- get support of an item set */
  ITEM   i;                     /* buffer for an item */
  CMNODE *p;                    /* to traverse the nodes */

  assert(cmt && (items || (n <= 0))); /* check function arguments */
  p = &cmt->root;               /* start search at the root node */
  while (--n >= 0) {            /* traverse the items of the set */
    i = *items++;               /* try to find a child node */
    p = p->children;            /* with the next item in the set */
    if (cmt->dir < 0) while (p && (p->item > i)) p = p->sibling;
    else              while (p && (p->item < i)) p = p->sibling;
    if (!p || (p->item != i))   /* if a node with the next item */
      return -1;                /* does not exist in the tree, */
  }                             /* abort the search with failure */
  return p->supp;               /* return support of the item set */
}  /* cmt_get() */

/*--------------------------------------------------------------------*/

#define MERGE(dir) \
static CMNODE* merge_##dir (CMNODE *s1, CMNODE *s2, MEMSYS *mem)       \
{                               /* --- merge two node list */          \
  CMNODE *out, **end, *p;       /* output node list and end pointer */ \
                                                                       \
  assert(mem);                  /* check the function arguments */     \
  if (!s1) return s2;           /* if there is only one node list, */  \
  if (!s2) return s1;           /* simply return the other list */     \
  end = &out;                   /* start the output list */            \
  while (1) {                   /* node list merge loop */             \
    if      (dir(s1->item, s2->item)) {                                \
      *end = s1; end = &s1->sibling; s1 = *end; if (!s1) break; }      \
    else if (dir(s2->item, s1->item)) {                                \
      *end = s2; end = &s2->sibling; s2 = *end; if (!s2) break; }      \
    else {                      /* copy nodes with singular items */   \
      s1->children = merge_##dir(s1->children, s2->children, mem);     \
      if (s1->supp < s2->supp)  /* merge the children recursively */   \
        s1->supp = s2->supp;    /* and update the node support */      \
      p    = s2; s2  =  s2->sibling; ms_free(mem, p);                  \
      *end = s1; end = &s1->sibling; s1 = *end;                        \
      if (!s1 || !s2) break;    /* move node from the first source */  \
    }                           /* to the output and delete the one */ \
  }                             /* from the second source */           \
  *end = (s1) ? s1 : s2;        /* append the remaining nodes */       \
  return out;                   /* return the merged prefix tree */    \
}  /* merge() */

/*--------------------------------------------------------------------*/

MERGE(pos)                      /* function for ascending  item order */
MERGE(neg)                      /* function for descending item order */

/*--------------------------------------------------------------------*/

#define PRUNE(dir) \
static CMNODE* prune_##dir (CMNODE *node, ITEM item, MEMSYS *mem)      \
{                               /* --- prune a c/m prefix tree */      \
  CMNODE *p, *b = NULL;         /* buffer for merged subtrees */       \
                                                                       \
  assert(mem);                  /* check the function arguments */     \
  while (node && dir(node->item, item)) {                              \
    node->children =            /* prune children of current node */   \
    p = prune_##dir(node->children, item, mem);                        \
    if (p) b = (!b) ? p : merge_##dir(b, p, mem);                      \
    p    = node;                /* merge remaining children with */    \
    node = node->sibling;       /* the already collected subtrees */   \
    ms_free(mem, p);            /* and delete the processed node */    \
  }                             /* fimally merge with rem. nodes */    \
  return (!node) ? b : (!b) ? node : merge_##dir(b, node, mem);        \
}  /* prune() */

/*--------------------------------------------------------------------*/

PRUNE(pos)                      /* function for ascending  item order */
PRUNE(neg)                      /* function for descending item order */

/*--------------------------------------------------------------------*/

void cmt_prune (CMTREE *cmt, ITEM item)
{                               /* --- prune a c/m prefix tree */
  CMNODE *p;                    /* to access the tree nodes */

  assert(cmt && (item >= 0));   /* check the function arguments */
  cmt->item = item;             /* note the item pruned with and */
  p = &cmt->root;               /* prune up to, but not item itself */
  p = p->children = (cmt->dir < 0)
    ? prune_neg(p->children, item, cmt->mem)
    : prune_pos(p->children, item, cmt->mem);
  cmt->max = (p && (p->item == item)) ? p->supp : -1;
}  /* cmt_prune() */            /* set the maximal item set support */

/*--------------------------------------------------------------------*/

static CMNODE* copy (const CMNODE *src, MEMSYS *mem)
{                               /* --- copy a c/m subtree */
  CMNODE *dst, *node;           /* created copy of the node list */
  CMNODE **end = &dst;          /* end of the created copy */
  CMNODE *c;                    /* buffer for copied children */

  assert(src && mem);           /* check the function arguments */
  do {                          /* sibling copying loop */
    *end = node = (CMNODE*)ms_alloc(mem);
    if (!node) return NULL;     /* create a copy for each node */
    node->item = src->item;     /* copy the item  */
    node->supp = src->supp;     /* and its support */
    c = src->children;          /* if there are children, copy them */
    if (c && !(c = copy(c, mem))) return NULL;
    node->children = c;         /* store the (copied) children */
    end = &node->sibling;       /* get the new list end */
    src = src->sibling;         /* (to append the next node) */
  } while (src);                /* check for another sibling */
  *end = NULL;                  /* terminate the copied list */
  return dst;                   /* return the created copy */
}  /* copy() */

/*--------------------------------------------------------------------*/

static void* delclr (CMTREE *cmt, int del)
{                               /* --- delete or clear a prefix tree */
  if (del) cmt_delete(cmt, 1);  /* if deletion flag is set, delete, */
  else     cmt_clear (cmt);     /* otherwise only clear the tree */
  return NULL;                  /* return an error indicator */
}  /* delclr() */

/*--------------------------------------------------------------------*/

CMTREE* cmt_project (CMTREE *dst, CMTREE *src, ITEM item)
{                               /* --- project a c/m prefix tree */
  CMTREE *arg = dst;            /* buffer for destination argument */
  CMNODE *p;                    /* to traverse the tree nodes */

  assert(src && (item >= 0)     /* check the function arguments */
  &&    (!dst || (dst->dir == src->dir)));
  if (!dst) dst = cmt_create(NULL, src->dir, src->size-1);
  if (!dst) return NULL;        /* create/reinit. destination tree */
  src->item = item;             /* note the projection item */
  dst->item = -1; dst->max = src->max = -1;
  dst->root.supp = 0;           /* clear maximum item set support */
  assert(!dst->root.children);  /* check for an empty destination */
  p = &src->root;               /* if the prefix tree is empty, */
  if (!p->children) return dst; /* directly return the destination */
  p = p->children = (src->dir < 0) /* prune top level up to the item */
    ? prune_neg(p->children, item, src->mem)
    : prune_pos(p->children, item, src->mem);
  if (!p || (p->item != item))  /* if projection item is missing, */
    return dst;                 /* directly return the destination */
  dst->root.supp = src->max = p->supp;   /* set projection support */
  if (p->children) {            /* if projection item has children */
    dst->root.children = p = copy(p->children, dst->mem);
    if (!p) return delclr(dst, !arg);
  }                             /* copy the child nodes to the dest. */
  p = &src->root;               /* prune the projection item */
  p->children = (src->dir < 0)  /* from the source prefix tree */
              ? prune_neg(p->children, item-1, src->mem)
              : prune_pos(p->children, item+1, src->mem);
  return dst;                   /* return the destination tree */
}  /* cmt_project() */

/*--------------------------------------------------------------------*/

#define XCOPY(dir) \
static CMNODE* xcopy_##dir (const CMNODE *src, MEMSYS *mem,            \
                            const int *keep)                           \
{                               /* --- copy a c/m subtree */           \
  CMNODE *dst, *node;           /* created copy of the node list */    \
  CMNODE **end = &dst;          /* end of the created copy */          \
  CMNODE *c, *b = NULL;         /* buffer for copied children */       \
                                                                       \
  assert(src && mem);           /* check the function arguments */     \
  do {                          /* sibling copying loop */             \
    c = src->children;          /* if there children */                \
    if (c && ((c = xcopy_##dir(c, mem, keep)) == COPYERR))             \
      return COPYERR;           /* recursively copy the children */    \
    if (keep[src->item]) {      /* if to copy the node itself */       \
      *end = node = (CMNODE*)ms_alloc(mem);                            \
      if (!node) return COPYERR;/* create a copy of the node */        \
      node->item = src->item;   /* copy the item and the support */    \
      node->supp = src->supp;   /* into the created copy */            \
      node->children = c;       /* set the (copied) children */        \
      end = &node->sibling; }   /* get the new list end */             \
    else if (c)                 /* if there are copied children */     \
      b = (b) ? merge_##dir(b, c, mem) : c;                            \
    src = src->sibling;         /* merge them into a buffer */         \
  } while (src);                /* check for another sibling */        \
  *end = NULL;                  /* terminate the copied list */        \
  return (!b) ? dst : (!dst) ? b : merge_##dir(dst, b, mem);           \
}  /* xcopy() */               /* return the created copy */

/*--------------------------------------------------------------------*/

XCOPY(pos)                      /* function for ascending  item order */
XCOPY(neg)                      /* function for descending item order */

/*--------------------------------------------------------------------*/

CMTREE* cmt_xproj (CMTREE *dst, CMTREE *src, ITEM item,
                   const ITEM *keep, ITEM n)
{                               /* --- project a c/m prefix tree */
  ITEM   i;                     /* loop variable */
  CMTREE *arg = dst;            /* buffer for destination argument */
  CMNODE *p;                    /* to traverse the tree nodes */

  assert(src && (item >= 0)     /* check the function arguments */
  &&    (!dst || (dst->dir == src->dir)) && keep);
  if (!dst) dst = cmt_create(NULL, src->dir, src->size-1);
  if (!dst) return NULL;        /* create the destination tree */
  src->item = item;             /* note the projection item */
  dst->item = -1; dst->max = src->max = -1;
  dst->root.supp = 0;           /* clear maximum item set support */
  assert(!dst->root.children);  /* check for an empty destination */
  p = &src->root;               /* prune top level up to the item */
  p = p->children = (src->dir < 0)
    ? prune_neg(p->children, item, src->mem)
    : prune_pos(p->children, item, src->mem);
  if (!p || (p->item != item))  /* if projection item is missing, */
    return dst;                 /* directly return the destination */
  dst->root.supp = src->max = p->supp;   /* set projection support */
  if (p->children) {            /* if projection item has children */
    for (i = n; --i >= 0; )     /* traverse the items to keep and */
      dst->keep[keep[i]] = 1;   /* set the corresponding flags */
    p = (dst->dir < 0)          /* copy branch of projection item */
      ? xcopy_neg(p->children, dst->mem, dst->keep)
      : xcopy_pos(p->children, dst->mem, dst->keep);
    for (i = n; --i >= 0; )     /* traverse the items to keep and */
      dst->keep[keep[i]] = 0;   /* clear the corresponding flags */
    if (p == COPYERR) return delclr(dst, !arg);
    dst->root.children = p;     /* store the copied branch */
  }
  p = &src->root;               /* prune the projection item */
  p->children = (src->dir < 0)  /* from the source prefix tree */
              ? prune_neg(p->children, item-1, src->mem)
              : prune_pos(p->children, item+1, src->mem);
  return dst;                   /* return the destination tree */
}  /* cmt_xproj() */

/*--------------------------------------------------------------------*/
#ifndef NDEBUG

static void indent (int k)
{ while (--k >= 0) printf("   "); }

/*--------------------------------------------------------------------*/

static void show (CMNODE *node, ITEMBASE *base, int ind)
{                               /* --- recursively show nodes */
  assert(ind >= 0);             /* check the function arguments */
  while (node) {                /* traverse the node list */
    indent(ind);                /* indent the output line */
    if (base) printf("%s/", ib_xname(base, node->item));
    printf("%"ITEM_FMT":",   node->item); /* print node information */
    printf("%"RSUPP_FMT"\n", node->supp); /* (item and support) */
    show(node->children, base, ind+1);
    node = node->sibling;       /* recursively show the child nodes, */
  }                             /* then go to the next node */
}  /* show() */

/*--------------------------------------------------------------------*/

void cmt_show (CMTREE *cmt, ITEMBASE *base, int ind)
{                               /* --- print a c/m prefix tree */
  indent(ind);                  /* indent the output line */
  if (!cmt) {                   /* check whether tree exists */
    printf("(null)\n"); return; }
  if (cmt->item < -1) {         /* check whether tree is valid */
    printf("invalid\n"); return; }
  printf("item: ");             /* print the associated item */
  if (base && (cmt->item >= 0)) /* print item name if known */
    printf("%s/", ib_name(base, cmt->item));
  printf("%"ITEM_FMT, cmt->item);
  printf(" (%"RSUPP_FMT")\n", cmt->max);
  indent(ind);                  /* print root node information */
  printf("*:%"RSUPP_FMT"\n",  cmt->root.supp);
  show(cmt->root.children, base, ind+1);
}  /* cmt_show() */             /* recursively show the nodes */

#endif
/*----------------------------------------------------------------------
  Closed/Maximal Filter Functions
----------------------------------------------------------------------*/

CLOMAX* cm_create (int dir, ITEM size)
{                               /* --- create a c/m filter object */
  CLOMAX *cm;                   /* created closed/maximal filter */
  CMTREE *t;                    /* created root prefix tree */

  assert(size > 0);             /* check the function arguments */
  cm = (CLOMAX*)calloc(1, sizeof(CLOMAX) +(size_t)size*sizeof(CMTREE*));
  if (!cm) return NULL;         /* create a closed/maximal filter */
  cm->size = size;              /* and initialize its fields */
  cm->dir  = (dir < 0) ? -1 : +1;
  cm->cnt  = 0;                 /* there is no prefix yet */
  cm->trees[0] = t = cmt_create(NULL, dir, size);
  if (!t) { cm_delete(cm); return NULL; }
  cmt_add(t, NULL, 0, 0);       /* create and init. a root tree */
  t->item = -1;                 /* mark the root tree as valid */
  return cm;                    /* return the created c/m filter */
}  /* cm_create() */

/*--------------------------------------------------------------------*/

void cm_delete (CLOMAX *cm)
{                               /* --- delete a c/m filter object */
  ITEM i;                       /* loop variable */

  assert(cm);                   /* check the function argument */
  for (i = 0; cm->trees[i]; i++)/* traverse the c/m prefix trees */
    cmt_delete(cm->trees[i],1); /* and delete them */
  free(cm);                     /* delete the base structure */
}  /* cm_delete() */

/*--------------------------------------------------------------------*/

RSUPP cm_supp (CLOMAX *cm)
{                               /* --- get support of current prefix */
  assert(cm);                   /* check the function argument */
  return (cm->cnt > 0) ? cmt_max (cm->trees[cm->cnt-1])
                       : cmt_supp(cm->trees[0]);
}  /* cm_supp() */

/*--------------------------------------------------------------------*/

int cm_add (CLOMAX *cm, ITEM item, RSUPP supp)
{                               /* --- add an item to the prefix */
  CMTREE *t, **p;               /* to access the c/m prefix trees */

  assert(cm                     /* check the function arguments */
  &&    (item >= 0) && (item < cm->size));
  p = cm->trees +cm->cnt;       /* get the current prefix tree */
  if (!*p || !cmt_valid(*p)) {  /* if there is no (valid) tree, */
    t = p[-1];                  /* get the parent tree (source), */
    t = cmt_project(*p, t, t->item);
    if (!t) return -1;          /* project it to the corresponding */
    *p = t;                     /* extension item (end of prefix), */
  }                             /* and store the created projection */
  cmt_prune(t = *p, item);      /* prune the current prefix tree */
  if (t->max >= supp) return 0; /* check the support of the prefix */
  ++cm->cnt; return 1;          /* count the added item*/
}  /* cm_add() */

/*--------------------------------------------------------------------*/

int cm_addnc (CLOMAX *cm, ITEM item, RSUPP supp)
{                               /* --- add an item to the prefix */
  CMTREE *t, **p;               /* to access the c/m prefix trees */

  assert(cm                     /* check the function arguments */
  &&    (item >= 0) && (item < cm->size));
  p = cm->trees +cm->cnt;       /* get the current prefix tree */
  if (!*p || !cmt_valid(*p)) {  /* if there is no (valid) tree, */
    t = p[-1];                  /* get the parent tree (source), */
    t = cmt_project(*p, t, t->item);
    if (!t) return -1;          /* project it to the corresponding */
    *p = t;                     /* extension item (end of prefix), */
  }                             /* and store the created projection */
  cmt_prune(*p, item);          /* prune the current prefix tree */
  ++cm->cnt; return 1;          /* count the added item */
}  /* cm_addnc() */

/* In contrast to cm_add(), the function cm_addnc() does not check */
/* whether the extended prefix possesses a perfect extension.      */

/*--------------------------------------------------------------------*/

void cm_remove (CLOMAX *cm, ITEM n)
{                               /* --- remove items from the prefix */
  assert(cm && (n >= 0));       /* check the function arguments */
  for (n = (n < cm->cnt) ? cm->cnt -n : 0; cm->cnt > n; cm->cnt--)
    if (cm->trees[cm->cnt]) cmt_clear(cm->trees[cm->cnt]);
}  /* cm_remove() */             /* traverse and clear the trees */

/*--------------------------------------------------------------------*/

RSUPP cm_tail (CLOMAX *cm, const ITEM *items, ITEM n)
{                               /* --- prune with the tail items */
  RSUPP  s;                     /* support of the tail item set */
  CMTREE *t, **p;               /* to access the c/m prefix trees */

  assert(cm && (items || (n <= 0))); /* check the function arguments */
  if (n == 0) return 1;         /* empty tails can be ignored */
  if (cm->cnt <= 0) return 0;   /* check for a non-empty prefix */
  p = cm->trees +cm->cnt;       /* get the current prefix tree(s)  */
  t = p[-1];                    /* (source and destination) */
  t = cmt_xproj(*p, t, t->item, items, n);
  if (!t) return -1;            /* project parent with the tail items */
  *p = t;                       /* set the created projection */
  if (n < 0) return 0;          /* if not to check support, abort */
  s = cmt_get(t, items, n);     /* get support of given tail item set */
  return (s > 0) ? s : 0;       /* and check whether it is positive */
}  /* cm_tail() */

/*--------------------------------------------------------------------*/

int cm_update (CLOMAX *cm, const ITEM *items, ITEM n, RSUPP supp)
{                               /* --- update filter with found set */
  ITEM   i;                     /* loop variable */
  CMTREE *t;                    /* to traverse the prefix trees */

  assert(cm                     /* check function arguments */
  &&    (items || (n <= 0)) && (supp >= 0));
  for (i = 0; i < cm->cnt; i++) {
    t = cm->trees[i];           /* traverse the c/m prefix trees */
    while (*items != t->item) { ++items; --n; }
    if (cmt_add(t, ++items, --n, supp) < 0)
      return -1;                /* add the proper suffix of the set */
  }                             /* to report to the c/m prefix trees */
  return 0;                     /* return 'ok' */
}  /* cm_update() */

/*--------------------------------------------------------------------*/
#ifndef NDEBUG

void cm_show (CLOMAX *cm, ITEMBASE *base, int ind)
{                               /* --- print a closed/maximal filter */
  ITEM i;                       /* loop variable */

  assert(cm);                   /* check the function arguments */
  for (i = 0; i <= cm->cnt; i++)/* traverse and print the c/m trees */
    cmt_show(cm->trees[i], base, ind);
}  /* cm_show() */

#endif
