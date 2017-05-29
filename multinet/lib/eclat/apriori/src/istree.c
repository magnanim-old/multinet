/*----------------------------------------------------------------------
  File    : istree.c
  Contents: item set tree management for apriori algorithm
  Author  : Christian Borgelt
  History : 1996.01.22 file created
            1996.02.07 child(), count(), ist_addlvl(), and ist_count()
            1996.02.09 ist_rule() programmed and debugged
            1996.02.10 empty rule bodies made optional
            1996.03.28 support made relative to number of item sets
            1996.06.25 function count() optimized
            1996.11.23 rule extraction redesigned
            1996.11.24 rule selection criteria added
            1997.08.18 chi^2 added, mincnt added to function ist_init()
            1998.01.15 confidence comparison changed to >=
            1998.01.23 integer support computation changed (ceil)
            1998.01.26 condition added to set extension in child()
            1998.02.10 bug in computation of IST_INFO fixed
            1998.02.11 parameter thresh added to function ist_init()
            1998.05.14 item set tree navigation functions added
            1998.08.08 item appearances considered for rule selection
            1998.08.20 deferred child node array allocation added
            1998.09.05 bug concerning node id fixed
            1998.09.22 bug in rule extraction fixed (item appearances)
            1998.09.23 computation of chi^2 measure simplified
            1999.08.25 rule extraction simplified
            1999.11.05 rule evaluation measure IST_LIFT_DIFF added
            1999.11.08 parameter 'eval' added to function ist_rule()
            1999.11.11 rule consequents moved to first field
            1999.12.01 bug in node reallocation fixed
            2001.04.01 functions ist_iset() and ist_getcntx() added
            2001.12.28 sort function moved to module tract
            2002.02.07 tree clearing removed, counting improved
            2002.02.08 child creation improved (check of body support)
            2002.02.10 APP_NONE bugs fixed (ist_iset() and ist_hedge())
            2002.02.11 memory usage minimization option added
            2002.02.12 ist_first() and ist_last() replaced by ist_next()
            2002.02.19 transaction tree functions added
            2002.10.09 bug in function ist_hedge() fixed (conf. comp.)
            2003.07.17 check of item usage added (function ist_check())
            2003.07.18 maximally frequent item set filter added
            2003.08.11 item set filtering generalized (ist_mark())
            2003.08.15 renamed new to cur in ist_addlvl() (C++ compat.)
            2003.11.14 definition of HDONLY changed to ITEM_MIN
            2003.12.02 skipping unnecessary subtrees added (_checksub)
            2003.12.03 bug in ist_check() for rule mining fixed
            2003.12.12 padding for 64 bit architecture added
            2004.05.09 additional selection measure for sets added
            2004.12.09 bug in add. evaluation measure for sets fixed
            2006.11.26 support parameter changed to an absolute value
            2007.02.07 bug in function ist_addlvl() / child() fixed
            2008.01.25 bug in filtering closed/maximal item sets fixed
            2008.03.13 additional rule evaluation redesigned
            2008.03.24 creation based on ITEMBASE structure
            2008.08.12 adapted to redesign of tract.[hc]
            2008.08.19 memory saving node structure simplified
            2008.08.21 function ist_report() added (recursive reporting)
            2008.09.07 ist_prune() added, memory saving always used
            2008.09.10 item set extraction and evaluation redesigned
            2008.09.11 pruning with evaluation measure added
            2008.11.19 adapted to modified transaction tree interface
            2008.12.02 bug in ist_create() fixed (support adaptation)
            2008.12.06 perfect extension pruning added (optional)
            2009.09.03 bugs in functions countx() and report() fixed
            2009.10.15 adapted to item set counter in reporter
            2009.11.13 optional zeroing of evaluation below expectation
            2010.03.02 bug in forward pruning fixed (executed too late)
            2010.06.17 rule evaluation aggregation without functions
            2010.06.18 filtering for increase of evaluation added
            2010.08.30 Fisher's exact test added as evaluation measure
            2010.10.22 chi^2 measure with Yates correction added
            2010.12.09 bug in child node pointer array allocation fixed
            2011.06.22 handling of roundoff errors in fet_*() improved
            2011.07.18 alternative transaction tree implementation added
            2011.07.22 adapted to new module ruleval (rule evaluation)
            2011.07.25 threshold inverted for measures yielding p-values
            2011.08.04 bug in function ist_mark() fixed (min. support)
            2011.08.05 function ist_clomax() added (replaces ist_mark())
            2011.08.08 minimum improvement check added to ld_ratio()
            2011.08.16 filtering for generators added to ist_clomax()
            2012.02.15 bug in minimum improvement check fixed (ist->dir)
            2012.06.13 bug in ist_rule() fixed (ist->invbxs, ist->dir)
            2013.03.10 adapted to modified bsearch/bisect interface
            2013.03.30 adapted to type changes in module tract
            2013.06.03 real-valued support made possible (SUPP==double)
            2013.10.15 checks of return code of isr_report() added
            2014.08.01 minimum improvement of evaluation measure removed
            2014.08.14 function ist_addchn() and related functions added
            2014.08.21 parameter 'body' added to function ist_create()
            2014.08.22 bugs in functions ist_down(), ist_[]supp() fixed
            2014.08.28 function ist_clomax() adapted to IST_REVERSE
            2014.11.14 bug in function evaluate() fixed (negative index)
            2015.02.25 bug in function r4set() fixed (ITEMOF(node))
            2016.11.19 bug in function ist_filter() fixed (path length)
----------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <math.h>
#include <assert.h>
#include "istree.h"
#include "chi2.h"
#include "gamma.h"
#ifdef STORAGE
#include "storage.h"
#endif

#ifdef _MSC_VER
#ifndef copysign
#define copysign    _copysign   /* C99 copysign() function */
#endif
#ifndef signbit
#define signbit     _signbit    /* C99 signbit() function */
#endif
#endif                          /* MSC still does not support C99 */
#ifndef INFINITY
#define INFINITY    (DBL_MAX+DBL_MAX)
#endif                          /* MSC still does not support C99 */

/*----------------------------------------------------------------------
  Preprocessor Definitions
----------------------------------------------------------------------*/
#define LN_2        0.69314718055994530942  /* ln(2) */
#define HDONLY      ITEM_MIN    /* flag for head only item in path */
#define ITEMOF(n)   ((ITEM)((n)->item & ~HDONLY))
#define ISHDONLY(n) ((n)->item < 0)
#define int         1           /* to check definition of SUPP */
#define long        2           /* for double precision type */
#define double      3
#if SUPP==double
#define SKIP        (-0.0)      /* flag for subtree skipping */
#define SETSKIP(n)  ((n) = copysign((n),-1.0))
#define CLRSKIP(n)  ((n) = copysign((n),+1.0))
#define IS2SKIP(n)  signbit(n)
#define COUNT(n)    copysign((n),+1.0)
#define INC(n,w)    ((n) += copysign((w),(n)))
/* Double precision floating-point support requires the compiler   */
/* and the underlying system to comply with the IEEE standard for  */
/* floating-point arithmetic (IEEE 754), which allows for a number */
/* -0.0 (zero value with a set sign bit), the representation of    */
/* which differs from +0.0 (zero value with a cleared sign bit).   */
/* Even though both numbers have the same value (namely simply 0)  */
/* and thus -0.0 == +0.0, it is signbit(-0.0) != signbit(+0.0).    */
/* This is essential for the program to function correctly.        */
#else
#define SKIP        SUPP_MIN    /* flag for subtree skipping */
#define SETSKIP(n)  ((n) |=  SKIP)
#define CLRSKIP(n)  ((n) &= ~SKIP)
#define IS2SKIP(n)  ((n) < 0)
#define COUNT(n)    ((n) &  ~SKIP)
#define INC(n,w)    ((n) += (w))
#endif
#undef int                      /* remove preprocessor definitions */
#undef long                     /* needed for the type checking */
#undef double
#define CHILDCNT(n) ((n)->chcnt & ~ITEM_MIN)
#define ITEMAT(n,i) (((n)->offset >= 0) ? (n)->offset +(i) \
                      : ((ITEM*)((n)->cnts +(n)->size))[i])

#ifdef ALIGN8                   /* if aligned 64 bit architecture */
#define PAD(p)      ((((size_t)(p) -1) & 7) ^ 7)
#define ALIGN(p)    ((p) = (void*)((size_t)(p) +PAD(p)))
#else                           /* otherwise (32 bit) */
#define PAD(p)      0           /* no padding is needed */
#define ALIGN(p)
#endif
/* Note that not all 64 bit architectures need pointers to be aligned */
/* to addresses divisible by 8. Use ALIGN8 only if this is the case.  */

/*----------------------------------------------------------------------
  Auxiliary Functions
----------------------------------------------------------------------*/

static ITEM search (ITEM id, ISTNODE **chn, ITEM n)
{                               /* --- find a child node (index) */
  ITEM l, r, m;                 /* left, right, and middle index */
  ITEM x;                       /* item of middle child */

  assert(chn && (n > 0));       /* check the function arguments */
  for (l = 0, r = n; l < r; ) { /* while the range is not empty */
    m = (l+r) >> 1;             /* get index of the middle element */
    x = ITEMOF(chn[m]);         /* compare the item identifier */
    if      (id > x) l = m+1;   /* to the middle element and */
    else if (id < x) r = m;     /* adapt the range boundaries */
    else return m;              /* if there is an exact match, */
  }                             /* return the child node index */
  return (ITEM)-1;              /* return 'not found' */
}  /* search() */

/*--------------------------------------------------------------------*/
#ifdef IST_BSEARCH

static ITEM bisect (ITEM id, ISTNODE **chn, ITEM n)
{                               /* --- find a child node (index) */
  ITEM l, r, m;                 /* left, right, and middle index */
  ITEM x;                       /* item of middle child */

  assert(chn && (n > 0));       /* check the function arguments */
  for (l = 0, r = n; l < r; ) { /* while the range is not empty */
    m = (l+r) >> 1;             /* get index of the middle element */
    x = ITEMOF(chn[m]);         /* compare the item identifier */
    if      (id > x) l = m+1;   /* to the middle element and */
    else if (id < x) r = m;     /* adapt the range boundaries */
    else return m;              /* if there is an exact match, */
  }                             /* return the child node index */
  return l;                     /* return the insertion position */
}  /* bisect() */

#endif
/*--------------------------------------------------------------------*/

static SUPP getsupp (ISTNODE *node, ITEM *items, ITEM n)
{                               /* --- get support of an item set */
  ITEM    i, k;                 /* array indices, number of children */
  ISTNODE **chn;                /* child node array */

  assert(node                   /* check the function arguments */
  &&    (n >= 0) && (items || (n <= 0)));
  for ( ; --n > 0; items++) {   /* follow the set/path from the node */
    k = CHILDCNT(node);         /* if there are no children, */
    if (k <= 0) return SKIP;    /* the support is less than minsupp */
    if (node->offset >= 0) {    /* if a pure array is used */
      chn = (ISTNODE**)(node->cnts +node->size);
      ALIGN(chn);               /* get the child array index */
      i = *items -ITEMOF(chn[0]);  /* compute the child array index */
      if (i >= k) return SKIP; }   /* and check whether entry exists */
    else {                      /* if an identifier map is used */
      chn = (ISTNODE**)((ITEM*)(node->cnts +node->size) +node->size);
      ALIGN(chn);               /* get the child array index */
      i = search(*items, chn, k);
    }                           /* find the child array index */
    if (i < 0) return SKIP;     /* if child does not exist, abort */
    node = chn[i];              /* go to the corresponding child */
    if (!node) return SKIP;     /* if child does not exist, abort */
  }                             /* (support is less than minsupp) */
  k = node->size;               /* get the number of counters */
  if (node->offset >= 0) {      /* if a pure array is used, */
    i = *items -node->offset;   /* compute the counter index and */
    if (i >= k) return SKIP; }  /* check whether counter exists */
  else                          /* if an identifier map is used */
    i = ia_bsearch(*items, (ITEM*)(node->cnts +k), (size_t)k);
  if (i < 0) return SKIP;       /* if no counter exists, abort */
  return node->cnts[i];         /* return the item set support */
}  /* getsupp() */

/*--------------------------------------------------------------------*/

static void reclvls (ISTREE *ist, ISTNODE *node, ITEM lvl)
{                               /* --- set successor pointers */
  ITEM    i, n;                 /* loop variable, number of children */
  ISTNODE **chn;                /* child node array */

  assert(ist && node && (lvl >= 0));  /* check function arguments */
  node->succ = ist->lvls[lvl];  /* add the node at the head */
  ist->lvls[lvl] = node;        /* of the level list */
  n = CHILDCNT(node);           /* get the number of children */
  if (n <= 0) return;           /* if there are no children, abort */
  lvl += 1;                     /* go to the child level */
  if (node->offset >= 0)        /* if a pure array is used */
    chn = (ISTNODE**)(node->cnts +node->size);
  else                          /* if an item map is used */
    chn = (ISTNODE**)((ITEM*)(node->cnts +node->size) +node->size);
  ALIGN(chn);                   /* get the child node array */
  for (i = 0; i < n; i++)       /* recursively process the children */
    if (chn[i]) reclvls(ist, chn[i], lvl);
}  /* reclvls() */

/*--------------------------------------------------------------------*/

static void makelvls (ISTREE *ist)
{                               /* --- set successor pointers */
  ISTNODE *root;                /* root node of the tree */

  assert(ist);                  /* check the function argument */
  root = ist->lvls[0];          /* get the root node */
  memset(ist->lvls, 0, (size_t)ist->height *sizeof(ISTNODE*));
  reclvls(ist, root, 0);        /* recursively set successors */
  ist->valid = -1;              /* levels/successors are now valid */
}  /* makelvls() */

/*--------------------------------------------------------------------*/

static void delete (ISTNODE *node)
{                               /* --- recursively delete a subtree */
  ITEM    i, n;                 /* loop variable, number of children */
  ISTNODE **chn;                /* child node array */

  assert(node);                 /* check the function argument */
  n = CHILDCNT(node);           /* get the number of children */
  if (n > 0) {                  /* if there are children */
    if (node->offset >= 0)      /* if a pure array is used */
      chn = (ISTNODE**)(node->cnts +node->size);
    else                        /* if an item map is used */
      chn = (ISTNODE**)((ITEM*)(node->cnts +node->size) +node->size);
    ALIGN(chn);                 /* get the child node array */
    for (i = 0; i < n; i++)     /* recursively delete the children */
      if (chn[i]) delete(chn[i]);
  }
  free(node);                   /* delete the node */
}  /* delete() */

/*----------------------------------------------------------------------
  Counting Functions
----------------------------------------------------------------------*/

static void count (ISTNODE *node,
                   const ITEM *items, ITEM n, SUPP wgt, ITEM min)
{                               /* --- count transaction recursively */
  ITEM    i, k, o;              /* array index, offset, map size */
  ITEM    *map;                 /* item identifier map */
  ISTNODE **chn;                /* array of child nodes */

  assert(node                   /* check the function arguments */
  &&    (n >= 0) && (items || (n <= 0)));
  if (node->offset >= 0) {      /* if a pure array is used */
    if (node->chcnt == 0) {     /* if this is a new node (leaf) */
      o = node->offset;         /* get the index offset */
      while ((n > 0) && (*items < o)) {
        n--; items++; }         /* skip items before first counter */
      while (--n >= 0) {        /* traverse the transaction's items */
        i = *items++ -o;        /* compute the counter array index */
        if (i >= node->size) return;
        INC(node->cnts[i],wgt); /* if the corresp. counter exists, */
      } }                       /* add the transaction weight to it */
    else if (node->chcnt > 0) { /* if there are child nodes */
      chn = (ISTNODE**)(node->cnts +node->size);
      ALIGN(chn);               /* get the child node array and */
      o   = ITEMOF(chn[0]);     /* the item of the first child */
      while ((n >= min) && (*items < o)) {
        n--; items++; }         /* skip items before the first child */
      for (--min; --n >= min;){ /* traverse the transaction's items */
        i = *items++ -o;        /* compute the child array index */
        if (i >= node->chcnt) return;
        if (chn[i]) count(chn[i], items, n, wgt, min);
      }                         /* if the corresp. child node exists, */
    } }                         /* count the transaction recursively */
  else {                        /* if an identifer map is used */
    if (node->chcnt == 0) {     /* if this is a new node (leaf) */
      map = (ITEM*)(node->cnts +(k = node->size));
      o   = map[0];             /* get the identifier map */
      while ((n > 0) && (*items < o)) {
        n--; items++; }         /* skip items before first counter */
      o   = map[k-1];           /* get the last item with a counter */
      for (i = 0; --n >= 0; items++) {  /* traverse the items */
        if (*items > o) return; /* if beyond last item, abort */
        #ifdef IST_BSEARCH      /* if to use a binary search */
        i = ia_bsearch(*items, map, (size_t)k);
        if (i >= 0)           INC(node->cnts[i], wgt);
        #else                   /* if to use a linear search */
        while (map[i] < *items) i++;
        if (map[i] == *items) INC(node->cnts[i], wgt);
        #endif                  /* if the corresp. counter exists, */
      } }                       /* add the transaction weight to it */
    else if (node->chcnt > 0) { /* if there are child nodes */
      chn = (ISTNODE**)((ITEM*)(node->cnts +node->size) +node->size);
      ALIGN(chn);               /* get the child node array and */
      o   = ITEMOF(chn[0]);     /* the index of the first child */
      while ((n >= min) && (*items < o)) {
        n--; items++; }         /* skip items before first child */
      k   = node->chcnt;        /* get the number of children and */
      o   = ITEMOF(chn[k-1]);   /* the index of the last item */
      for (--min; --n >= min; ) {
        if (*items > o) return; /* traverse the transaction */
        #ifdef IST_BSEARCH      /* if to use a binary search */
        k   -= i = bisect(*items, chn, k);
        chn += i;               /* find the child node index */
        #else                   /* if to use a linear search */
        while (ITEMOF(*chn) < *items) chn++;
        #endif                  /* find the child node index */
        if (ITEMOF(*chn) == *items++) count(*chn, items, n, wgt, min);
      }                         /* if the corresp. child node exists, */
    }                           /* count the transaction recursively */
  }
}  /* count() */

/*--------------------------------------------------------------------*/
#ifdef TATREEFN
#ifdef TATCOMPACT

static void countx (ISTNODE *node, const TANODE *tan, ITEM min)
{                               /* --- count trans. tree recursively */
  ITEM    i, k, o, n;           /* array indices, loop variables */
  ITEM    item;                 /* buffer for an item */
  ITEM    *map;                 /* item identifier map */
  ISTNODE **chn;                /* child node array */
  TANODE  *cld;                 /* child node in transaction tree */

  assert(node && tan);          /* check the function arguments */
  n = tan_max(tan);             /* get the maximum tansaction length */
  k = n & ~ITEM_MIN;            /* if the transactions are too short, */
  if (k < min) return;          /* abort the recursion */
  if (n <= 0) {                 /* if this is a leaf node */
    if (n < 0) count(node, tan_suffix(tan), k, tan_wgt(tan), min);
    return;                     /* count the transaction suffix */
  }                             /* and abort the function */
  for (cld = tan_children(tan); cld; cld = tan_sibling(cld))
    countx(node, cld, min);     /* count the transactions recursively */
  if (node->offset >= 0) {      /* if a pure array is used */
    if (node->chcnt == 0) {     /* if this is a new node (leaf) */
      o = node->offset;         /* get the index offset */
      for (cld = tan_children(tan); cld; cld = tan_sibling(cld)) {
        i = tan_item(cld) -o;   /* traverse the child items */
        if (i < 0) return;      /* if before first item, abort */
        if (i < node->size) INC(node->cnts[i], tan_wgt(cld));
      } }                       /* otherwise add the trans. weight */
    else if (node->chcnt > 0) { /* if there are child nodes */
      chn = (ISTNODE**)(node->cnts +node->size);
      ALIGN(chn);               /* get the child node array and */
      o   = ITEMOF(chn[0]);     /* the item of the first child */
      --min;                    /* traverse the child nodes */
      for (cld = tan_children(tan); cld; cld = tan_sibling(cld)) {
        i = tan_item(cld) -o;   /* traverse the child items */
        if  (i < 0) return;     /* if before first item, abort */
        if ((i < node->chcnt) && chn[i]) countx(chn[i], cld, min);
      }                         /* if the corresp. child node exists, */
    } }                         /* count the trans. tree recursively */
  else {                        /* if an identifer map is used */
    if (node->chcnt == 0) {     /* if this is a new node (leaf) */
      map = (ITEM*)(node->cnts +(k = node->size));
      o   = map[0];             /* get the item identifier map */
      for (cld = tan_children(tan); cld; cld = tan_sibling(cld)) {
        item = tan_item(cld);   /* traverse the child items */
        if (item < o) return;   /* if before the first item, return */
        #ifdef IST_BSEARCH      /* if to use a binary search */
        i = ia_bsearch(item, map, (size_t)k);
        if (i >= 0) { k = i; INC(node->cnts[k], tan_wgt(cld)); }
        #else                   /* if to use a linear search */
        while (map[--k] > item);
        if (map[k] == item)  INC(node->cnts[k], tan_wgt(cld));
        else k++;               /* if the corresp. counter exists, */
        #endif                  /* add the transaction weight to it, */
      } }                       /* otherwise adapt the map index */
    else if (node->chcnt > 0) { /* if there are child nodes */
      chn = (ISTNODE**)((ITEM*)(node->cnts +node->size) +node->size);
      ALIGN(chn);               /* get the child node array, */
      k   = node->chcnt;        /* the number of children, and */
      o   = ITEMOF(chn[0]);     /* the last item with a child */
      --min;                    /* traverse the child nodes */
      for (cld = tan_children(tan); cld; cld = tan_sibling(cld)) {
        item = tan_item(cld);   /* traverse the child items */
        if (item < o) return;   /* if before the first item, abort */
        #ifdef IST_BSEARCH      /* if to use a binary search */
        i = bisect(item, chn, k);
        if (i < k)              countx(chn[k = i], cld, min);
        #else                   /* if to use a linear search */
        while (ITEMOF(chn[--k]) > item);
        if (ITEMOF(chn[k]) == item) countx(chn[k], cld, min);
        else k++;               /* if the corresp. counter exists, */
        #endif                  /* count the transaction recursively, */
      }                         /* otherwise adapt the child index */
    }                           /* into the child node array */
  }
}  /* countx() */

/*--------------------------------------------------------------------*/
#else  /* #ifdef TATCOMPACT */

static void countx (ISTNODE *node, const TANODE *tan, ITEM min)
{                               /* --- count trans. tree recursively */
  ITEM    i, k, o, n;           /* array indices, loop variables */
  ITEM    item;                 /* buffer for an item */
  ITEM    *map;                 /* item identifier map */
  ISTNODE **chn;                /* child node array */

  assert(node && tan);          /* check the function arguments */
  if (tan_max(tan) < min)       /* if the transactions are too short, */
    return;                     /* abort the recursion */
  n = tan_size(tan);            /* get the number of children */
  if (n <= 0) {                 /* if there are no children */
    if (n < 0) count(node, tan_items(tan), -n, tan_wgt(tan), min);
    return;                     /* count the normal transaction */
  }                             /* and abort the function */
  while (--n >= 0)              /* count the transactions recursively */
    countx(node, tan_child(tan, n), min);
  if (node->offset >= 0) {      /* if a pure array is used */
    if (node->chcnt == 0) {     /* if this is a new node (leaf) */
      o = node->offset;         /* get the index offset */
      for (n = tan_size(tan); --n >= 0; ) {
        i = tan_item(tan, n)-o; /* traverse the node's items */
        if (i < 0) return;      /* if before the first item, abort */
        if (i < node->size)     /* if the corresp. counter exists */
          INC(node->cnts[i], tan_wgt(tan_child(tan, n)));
      } }                       /* add the transaction weight to it */
    else if (node->chcnt > 0) { /* if there are child nodes */
      chn = (ISTNODE**)(node->cnts +node->size);
      ALIGN(chn);               /* get the child node array and */
      o   = ITEMOF(chn[0]);     /* the item of the first child */
      for (--min, n = tan_size(tan); --n >= 0; ) {
        i = tan_item(tan, n)-o; /* traverse the node's items */
        if (i < 0) return;      /* if before the first item, abort */
        if ((i < node->chcnt) && chn[i])
          countx(chn[i], tan_child(tan, n), min);
      }                         /* if the corresp. child node exists, */
    } }                         /* count the trans. tree recursively */
  else {                        /* if an identifer map is used */
    if (node->chcnt == 0) {     /* if this is a new node (leaf) */
      map = (ITEM*)(node->cnts +(k = node->size));
      o   = map[0];             /* get the item identifier map */
      for (n = tan_size(tan); --n >= 0; ) {
        item = tan_item(tan,n); /* traverse the node's items */
        if (item < o) return;   /* if before the first item, abort */
        #ifdef IST_BSEARCH      /* if to use a binary search */
        i = ia_bsearch(item, map, (size_t)k);
        if (i >= 0) { k = i;    /* if counter exists, add trans. wgt. */
          INC(node->cnts[k], tan_wgt(tan_child(tan, n))); }
        #else                   /* if to use a linear search */
        while (map[--k] > item);
        if (map[k] == item)     /* if the corresp. counter exists */
          INC(node->cnts[k], tan_wgt(tan_child(tan, n)));
        else k++;               /* add the transaction weight to it, */
        #endif                  /* otherwise adapt the map index */
      } }
    else if (node->chcnt > 0) { /* if there are child nodes */
      chn = (ISTNODE**)((ITEM*)(node->cnts +node->size) +node->size);
      ALIGN(chn);               /* get the child node array, */
      k   = node->chcnt;        /* the number of children, and */
      o   = ITEMOF(chn[0]);     /* the last item with a child */
      for (--min, n = tan_size(tan); --n >= 0; ) {
        item = tan_item(tan,n); /* traverse the node's items */
        if (item < o) return;   /* if before the first item, abort */
        #ifdef IST_BSEARCH      /* if to use a binary search */
        i = search(item, chn, k);
        if (i >= 0) countx(chn[k = i], tan_child(tan, n), min);
        #else                   /* if to use a linear search */
        while (ITEMOF(chn[--k]) > item);
        if (ITEMOF(chn[k]) == item)
          countx(chn[k], tan_child(tan, n), min);
        else k++;               /* if the corresp. counter exists, */
        #endif                  /* count the transaction recursively, */
      }                         /* otherwise adapt the child index */
    }                           /* into the child node array */
  }
}  /* countx() */

#endif  /* #ifdef TATCOMPACT .. #else .. */
#endif  /* #ifdef TATREEFN */
/*----------------------------------------------------------------------
  Evaluation Functions
----------------------------------------------------------------------*/

static double evaluate (ISTREE *ist, ISTNODE *node, ITEM index)
{                               /* --- aggregate rule evaluations */
  ITEM      n;                  /* loop variable, buffer */
  ITEM      item;               /* current (head) item */
  SUPP      base;               /* total transaction weight */
  SUPP      supp;               /* support of item set */
  SUPP      body, head;         /* support of rule body and head */
  ITEM      *path;              /* path to follow for body support */
  ISTNODE   *curr;              /* to traverse the nodes on the path */
  RULEVALFN *refn;              /* rule evaluation function */
  double    val, agg;           /* (aggregated) value of measure */

  assert(ist && node);          /* check the function arguments */
  if (ist->eval <= IST_NONE)    /* if no evaluation measure is given, */
    return 0;                   /* the evaluation is always 0 */
  if (index < 0)                /* if not at any item (root node), */
    return (ist->dir < 0) ? 1 : 0;             /* there is no item */
  curr = node->parent;          /* get the parent of the given node */
  if (!curr)                    /* if there is no parent (root node), */
    return (ist->dir < 0) ? 1 : 0;     /* there is only a single item */
  item = ITEMAT(node, index);   /* get the corresponding item */
  supp = COUNT(node->cnts[index]);
  base = COUNT(ist->wgt);       /* get item set and base support */
  head = COUNT(ist->lvls[0]->cnts[item]);
  if (curr->offset >= 0)        /* if a pure array is used */
    body = COUNT(curr->cnts[ITEMOF(node) -curr->offset]);
  else {                        /* if an identifier map is used */
    path = (ITEM*)(curr->cnts +(n = curr->size));
    body = COUNT(curr->cnts[ia_bsearch(ITEMOF(node), path, (size_t)n)]);
  }                             /* find index and get body support */
  refn = re_function(ist->eval);/* get the evaluation function */
  agg  = (!ist->invbxs          /* compute the first measure value */
      || ((double)supp *(double)base > (double)head *(double)body))
       ? refn(supp, body, head, base) : (ist->dir < 0) ? 1 : 0;
  if (ist->agg <= IST_FIRST)    /* if to return the first value, */
    return agg;                 /* return the computed value */
  path = ist->buf +ist->height; /* initialize the path/item array */
  *--path = item; n = 1;        /* for the support retrieval */
  item = ITEMOF(node);          /* get the next head item */
  for ( ; curr; curr = curr->parent) {
    head = COUNT(ist->lvls[0]->cnts[item]);
    body = COUNT(getsupp(curr, path, n));
    val  = (!ist->invbxs        /* compute next measure value */
        || ((double)supp *(double)base > (double)head *(double)body))
         ? refn(supp, body, head, base) : (ist->dir < 0) ? 1 : 0;
    if      (ist->agg == IST_MIN) { if (val < agg) agg = val; }
    else if (ist->agg == IST_MAX) { if (val > agg) agg = val; }
    else agg += val;            /* compute the rule evaluation */
    *--path = item; n += 1;     /* and aggregate it by min/max/sum */
    item = ITEMOF(curr);        /* then extend the path/item array */
  }                             /* (store the head item) */
  if (ist->agg == IST_AVG)      /* if to average the evaluations, */
    agg /= (double)n;           /* divide by the number of items */
  return agg;                   /* return the measure aggregate */
}  /* evaluate() */

/*----------------------------------------------------------------------
  Main Functions
----------------------------------------------------------------------*/

ISTREE* ist_create (ITEMBASE *base, int mode,
                    SUPP smin, SUPP body, double conf)
{                               /* --- create an item set tree */
  ITEM    n;                    /* loop variable, number of items */
  ISTREE  *ist;                 /* created item set tree */
  ISTNODE *root;                /* root node of the tree */

  assert(base                   /* check the function arguments */
  &&    (smin >= 0) && (body >= 0) && (conf >= 0) && (conf <= 1));

  /* --- allocate memory --- */
  n   = ib_cnt(base);           /* get the number of items */
  ist = (ISTREE*)malloc(sizeof(ISTREE));
  if (!ist) return NULL;        /* allocate the tree body */
  ist->lvls = (ISTNODE**)malloc((size_t)(n+1) *sizeof(ISTNODE*));
  if (!ist->lvls) {                  free(ist); return NULL; }
  ist->buf  = (ITEM*)    malloc((size_t)(n+1) *sizeof(ITEM));
  if (!ist->buf)  { free(ist->lvls); free(ist); return NULL; }
  ist->map  = (ITEM*)    malloc((size_t)(n+1) *sizeof(ITEM));
  if (!ist->map)  { free(ist->buf);
                    free(ist->lvls); free(ist); return NULL; }
  ist->lvls[0] = ist->curr =    /* allocate a root node */
  root = (ISTNODE*)calloc(1,            sizeof(ISTNODE)
                        +(size_t)(n-1) *sizeof(SUPP));
  if (!root)      { free(ist->map); free(ist->buf);
                    free(ist->lvls); free(ist); return NULL; }

  /* --- initialize structures --- */
  ist->base   = base;           /* copy parameters to the structure */
  ist->mode   = mode;
  ist->wgt    = ib_getwgt(base);
  ist->height =  1;
  ist->valid  = -1;
  ist->smin   = (smin > 0)         ? smin : 1;
  ist->body   = (body > ist->smin) ? body : ist->smin;
  ist->conf   = conf *(1.0-DBL_EPSILON);
  /* Multiplying the minimum confidence with (1.0-DBL_EPSILON) takes */
  /* care of rounding errors. For example, a minimum confidence of   */
  /* 0.8 (or 80%) cannot be coded accurately with a double precision */
  /* floating point number. It is rather stored as a slightly larger */
  /* number, which can lead to missing rules. To prevent this, the   */
  /* confidence is made smaller by the largest possible factor < 1.  */
  ist->depth  = 1;
  #ifdef BENCH                  /* if benchmark version */
  ist->ndcnt  = 1; ist->ndprn = ist->mapsz = 0;
  ist->sccnt  = ist->scnec = n; ist->scprn = 0;
  ist->cpcnt  = ist->cpnec =    ist->cpprn = 0;
  #endif                        /* initialize the benchmark variables */
  ist_setsize(ist, 1, ITEM_MAX);
  ist_seteval(ist, IST_NONE, IST_NONE, 1, ITEM_MAX);
  ist_init(ist, 0);             /* initialize the extraction vars. */
  root->parent = root->succ  = NULL;
  root->offset = root->chcnt = root->item = 0;
  root->size   = n;             /* initialize the root node */
  while (--n >= 0)              /* copy the item frequencies */
    root->cnts[n] = ib_getfrq(base, n);
  return ist;                   /* return created item set tree */
}  /* ist_create() */

/*--------------------------------------------------------------------*/

void ist_delete (ISTREE *ist)
{                               /* --- delete an item set tree */
  ITEM    h;                    /* loop variable */
  ISTNODE *node, *t;            /* to traverse the nodes */

  assert(ist);                  /* check the function argument */
  if (!ist->valid)              /* if levels are not valid */
    delete(ist->lvls[0]);       /* use recursive deletion */
  else {                        /* if levels are valid */
    for (h = ist->height; --h >= 0; ) {
      for (node = ist->lvls[h]; node; ) {
        t = node; node = node->succ; free(t); }
    }                           /* delete all nodes */
  }                             /* by traversing the levels */
  free(ist->lvls);              /* delete the level array, */
  free(ist->map);               /* the identifier map, */
  free(ist->buf);               /* the path buffer, */
  free(ist);                    /* and the tree body */
}  /* ist_delete() */

/*--------------------------------------------------------------------*/

void ist_count (ISTREE *ist, const ITEM *items, ITEM n, SUPP wgt)
{                               /* --- count a transaction */
  assert(ist                    /* check the function arguments */
  &&    (n >= 0) && (items || (n <= 0)));
  if (n >= ist->height)         /* recursively count the transaction */
    count(ist->lvls[0], items, n, wgt, ist->height);
}  /* ist_count() */

/*--------------------------------------------------------------------*/

void ist_countt (ISTREE *ist, const TRACT *t)
{                               /* --- count a transaction */
  ITEM k;                       /* number of items */

  assert(ist && t);             /* check the function arguments */
  k = ta_size(t);               /* get the transaction size and */
  if (k >= ist->height)         /* count the transaction recursively */
    count(ist->lvls[0], ta_items(t), k, ta_wgt(t), ist->height);
}  /* ist_countt() */

/*--------------------------------------------------------------------*/

void ist_countb (ISTREE *ist, const TABAG *bag)
{                               /* --- count a transaction bag */
  TID   i;                      /* loop variable */
  ITEM  k;                      /* number of items */
  TRACT *t;                     /* to traverse the transactions */

  assert(ist && bag);           /* check the function arguments */
  if (tbg_max(bag) < ist->height)
    return;                     /* check for suff. long transactions */
  for (i = tbg_cnt(bag); --i >= 0; ) {
    t = tbg_tract(bag, i);      /* traverse the transactions */
    k = ta_size(t);             /* get the transaction size and */
    if (k >= ist->height)       /* count the transaction recursively */
      count(ist->lvls[0], ta_items(t), k, ta_wgt(t), ist->height);
  }
}  /* ist_countb() */

/*--------------------------------------------------------------------*/
#ifdef TATREEFN

void ist_countx (ISTREE *ist, const TATREE *tree)
{                               /* --- count transaction in tree */
  assert(ist && tree);          /* check the function arguments */
  countx(ist->lvls[0], tat_root(tree), ist->height);
}  /* ist_countx() */           /* recursively count the trans. tree */

#endif
/*--------------------------------------------------------------------*/

void ist_commit (ISTREE *ist)
{                               /* --- commit transaction counting */
  ITEM    i;                    /* loop variable, counter index */
  ISTNODE *node;                /* to traverse the nodes */

  assert(ist);                  /* check the function argument */
  if ((ist->eval   <= IST_NONE) /* if not to prune with evaluation */
  ||  (ist->height <  ist->prune))
    return;                     /* abort the function */
  if (!ist->valid)              /* if the levels are not valid, */
    makelvls(ist);              /* set the successor pointers */
  for (node = ist->lvls[ist->height-1]; node; node = node->succ)
    for (i = node->size; --i >= 0; )
      if ((node->cnts[i] < ist->smin)
      ||  (ist->dir *evaluate(ist, node, i) < ist->thresh))
        SETSKIP(node->cnts[i]); /* mark sets that do not qualify */
}  /* ist_commit() */

/*--------------------------------------------------------------------*/

static int used (ISTNODE *node, int *marks, SUPP supp)
{                               /* --- recursively check item usage */
  int     r = 0;                /* result */
  ITEM    i, k;                 /* array index, map size */
  ITEM    *map;                 /* item identifier map */
  ISTNODE **chn;                /* child node array */

  assert(node && marks);        /* check the function arguments */
  if (node->offset >= 0) {      /* if a pure array is used */
    if (node->chcnt == 0) {     /* if this is a new node (leaf) */
      k = node->offset;         /* get the index offset */
      for (i = node->size; --i >= 0; ) {
        if (node->cnts[i] >= supp)
          marks[k+i] = r = 1;   /* mark items in set that satisfy */
      } }                       /* the minimum support criterion */
    else if (node->chcnt > 0) { /* if there are child nodes */
      chn = (ISTNODE**)(node->cnts +node->size);
      ALIGN(chn);               /* get the child node array */
      for (i = node->chcnt; --i >= 0; )
        if (chn[i]) r |= used(chn[i], marks, supp);
    } }                         /* recursively process all children */
  else {                        /* if an identifer map is used */
    if (node->chcnt == 0) {     /* if this is a new node */
      map = (ITEM*)(node->cnts +node->size);
      for (i = node->size; --i >= 0; ) {
        if (node->cnts[i] >= supp)
          marks[map[i]] = r = 1;/* mark items in set that satisfies */
      } }                       /* the minimum support criterion */
    else if (node->chcnt > 0) { /* if there are child nodes */
      chn = (ISTNODE**)((ITEM*)(node->cnts +node->size) +node->size);
      ALIGN(chn);               /* get the child node array */
      for (i = node->chcnt; --i >= 0; )
        r |= used(chn[i], marks, supp);
    }                           /* get the child node array and */
  }                             /* recursively process all children */
  if ((r != 0) && node->parent) /* if the check succeeded, mark */
    marks[ITEMOF(node)] = 1;    /* the item associated with the node */
  return r;                     /* return the check result */
}  /* used() */

/*--------------------------------------------------------------------*/

ITEM ist_check (ISTREE *ist, int *marks)
{                               /* --- check item usage */
  ITEM i, n;                    /* loop variable, number of items */

  assert(ist);                  /* check the function argument */
  for (i = ist->lvls[0]->size; --i >= 0; )
    marks[i] = 0;               /* clear the marker array */
  used(ist->lvls[0], marks, ist->smin);
  for (n = 0, i = ist->lvls[0]->size; --i >= 0; )
    if (marks[i]) n++;          /* count used items */
  return n;                     /* and return this number */
}  /* ist_check() */

/*--------------------------------------------------------------------*/

void ist_prune (ISTREE *ist)
{                               /* --- prune counters and pointers */
  ITEM    i, k, n;              /* loop variables */
  SUPP    *c;                   /* counter array */
  ITEM    *map;                 /* item identifier map */
  ISTNODE **np, *node;          /* to traverse the nodes */
  ISTNODE **chn;                /* child node array */

  assert(ist);                  /* check the function argument */
  if (ist->height <= 1)         /* if there is only the root node, */
    return;                     /* there is nothing to prune */
  if (!ist->valid)              /* if the levels are not valid, */
    makelvls(ist);              /* set the successor pointers */

  /* -- prune counters for infrequent items -- */
  for (node = ist->lvls[ist->height-1]; node; node = node->succ) {
    c = node->cnts;             /* traverse the deepest level */
    if (node->offset >= 0) {    /* if a pure array is used */
      for (n = node->size; --n >= 0; ) /* find the last */
        if (c[n] >= ist->smin) break;  /* frequent item */
      for (i = 0; i < n; i++)          /* find the first */
        if (c[i] >= ist->smin) break;  /* frequent item  */
      node->size = ++n-i;       /* set the new node size */
      #ifdef BENCH              /* if benchmark version */
      k = node->size -(n-i);    /* get the number of pruned counters */
      ist->sccnt -= k;          /* update the number of counters */
      ist->scprn += k;          /* and of pruned counters */
      #endif                    /* update the memory usage */
      if (i > 0) {              /* if there are leading infreq. items */
        node->offset += i;      /* set the new item offset */
        memmove(c, c+i, (size_t)n *sizeof(SUPP));
      } }                       /* trim infrequent item from front */
    else {                      /* if an identifier map is used */
      map = (ITEM*)(c +node->size); /* get the item identifier map */
      for (i = n = 0; i < node->size; i++) {
        if (c[i] >= ist->smin) {
          c[n] = c[i]; map[n++] = map[i]; }
      }                         /* remove infrequent items */
      k = node->size -n;        /* get the number of pruned counters */
      if (k <= 0) continue;     /* if no items were pruned, continue */
      #ifdef BENCH              /* if benchmark version, */
      ist->sccnt -= k;          /* update the number of counters */
      ist->scprn += k;          /* and of pruned counters */
      ist->mapsz -= k;          /* update the total item map size */
      #endif
      node->size = n;           /* set the new node size */
      memmove(c+n, map, (size_t)n *sizeof(ITEM));
    }                           /* move the item identifier map */
  }                             /* after the support counters */

  /* -- prune pointers to empty children -- */
  for (node = ist->lvls[ist->height-2]; node; node = node->succ) {
    n = CHILDCNT(node);         /* traverse the parent nodes */
    if (n <= 0) continue;       /* skip childless nodes */
    if (node->offset >= 0) {    /* if a pure array is used */
      chn = (ISTNODE**)(node->cnts +node->size);
      ALIGN(chn);               /* get the child node array */
      while (--n >= 0)          /* find the last  non-empty child */
        if (chn[n] && (chn[n]->size > 0)) break;
      for (i = 0; i < n; i++)   /* find the first non-empty child */
        if (chn[i] && (chn[i]->size > 0)) break;
      node->chcnt = ++n-i;      /* set the new number of children */
      #ifdef BENCH              /* if benchmark version, */
      k = node->chcnt -(n-i);   /* get the number of pruned pointers */
      ist->cpcnt -= k;          /* update the number of pointers */
      ist->cpprn += k;          /* and of pruned pointers */
      #endif
      for (k = 0; i < n; i++)   /* remove all empty children */
        chn[k++] = (chn[i] && (chn[i]->size > 0)) ? chn[i] : NULL; }
    else {                      /* if an item identifier map is used */
      chn = (ISTNODE**)((ITEM*)(node->cnts +node->size) +node->size);
      ALIGN(chn);               /* get the child node array */
      for (i = k = 0; i < n; i++)
        if (chn[i]->size > 0)   /* collect the child nodes */
          chn[k++] = chn[i];    /* that are not empty */
      node->chcnt = k;          /* set the new number of children */
      #ifdef BENCH              /* if benchmark version, */
      n -= k;                   /* get the number of pruned pointers */
      ist->cpcnt -= n;          /* update the number of pointers */
      ist->cpprn += n;          /* and of pruned pointers */
      #endif
    }
    if (node->chcnt <= 0)       /* if all children were removed, */
      node->chcnt |= ITEM_MIN;  /* set the skip flag, so that */
  }                             /* no recounting takes place */

  /* -- remove empty children -- */
  for (np = ist->lvls +ist->height-1; *np; ) {
    node = *np;                 /* traverse the deepest level again */
    if (node->size > 0) { np = &node->succ; continue; }
    *np = node->succ; free(node); /* remove empty nodes */
    #ifdef BENCH                /* if benchmark version */
    ist->ndcnt--; ist->ndprn++; /* update the number nodes */
    #endif                      /* and of pruned nodes */
  }
}  /* ist_prune() */

/*--------------------------------------------------------------------*/

static ISTNODE* child (ISTREE *ist, ISTNODE *node, ITEM index, SUPP pex)
{                               /* --- create child node (extend set) */
  ITEM    i, k, n, m, e;        /* loop variables, counters */
  ISTNODE *curr;                /* to traverse the path to the root */
  ITEM    item;                 /* item identifier */
  ITEM    *set;                 /* next (partial) item set to check */
  int     body;                 /* enough support for a rule body */
  int     hdonly;               /* whether head only item on path */
  int     app;                  /* appearance flags of an item */
  SUPP    supp;                 /* support of an item set */

  assert(ist && node            /* check the function arguments */
  &&    (index >= 0) && (index < node->size));

  /* --- initialize --- */
  supp = node->cnts[index];     /* get support of item set to extend */
  if ((supp <  ist->smin)       /* if the support is insufficient */
  ||  (supp >= pex))            /* or item is a perfect extension, */
    return NULL;                /* abort (do not create a child) */
  item = ITEMAT(node, index);   /* get the item for the index and */
  app  = ib_getapp(ist->base, item);    /* the corresp. app. flag */
  if ((app == APP_NONE)         /* do not extend an item to ignore */
  || ((app == APP_HEAD) && (ISHDONLY(node))))
    return NULL;                /* do not combine two head only items */
  hdonly = (app == APP_HEAD) || ISHDONLY(node);
  body   = (supp >= ist->body)  /* if the set has enough support for */
         ? 1 : 0;               /* a rule body, set the body flag */
  ist->buf[ist->height-1] = item;/* init. set for support checks */

  /* --- check candidates --- */
  if (ist->mode & IST_REVERSE) { i = -1;    e = index;      }
  else                         { i = index; e = node->size; }
  for (m = 0; ++i < e; ) {      /* traverse the relevant indices */
    k   = ITEMAT(node, i);      /* retrieve the associated item */
    app = ib_getapp(ist->base, k);
    if ((app == APP_NONE) || (hdonly && (app == APP_HEAD)))
      continue;                 /* skip sets with two head only items */
    supp = node->cnts[i];       /* traverse the candidate items */
    if ((supp <  ist->smin)     /* if set support is insufficient */
    ||  (supp >= pex))          /* or item is a perfect extension, */
      continue;                 /* ignore the corresponding candidate */
    body &= 1;                  /* restrict body flags to set support */
    if (supp >= ist->body)      /* if set support is sufficient for */
      body |= 2;                /* a rule body, set the body flag */
    curr = node;                /* start at the current node */
    if (ist->mode & IST_PARTIAL)/* if to check only some subsets */
      body |= 4;                /* assume sufficient body support */
    else {                      /* if to check all subsets */
      set    = ist->buf +ist->height+1 -(n = 2);
      set[1] = k;               /* add the candidate item to the set */
      for ( ; curr->parent; curr = curr->parent) {
        supp = getsupp(curr->parent, set, n);
        if (supp <  ist->smin)  /* get the subset support and */
          break;                /* if it is too low, abort loop */
        if (supp >= ist->body)  /* if some subset has enough support */
          body |= 4;            /* for a rule body, set the body flag */
        *--set = ITEMOF(curr);  /* add id of current node to the set */
        n += 1;                 /* and adapt the number of items */
      }
    }
    if (!curr->parent && body)  /* if subset support is high enough */
      ist->map[m++] = k;        /* for a full rule and a rule body, */
  }                             /* note the item identifier */
  if (m <= 0) return NULL;      /* if no child is needed, abort */
  #ifdef BENCH                  /* if benchmark version, */
  ist->scnec += m;              /* sum the necessary counters */
  #endif

  /* --- decide on node structure --- */
  n = ist->map[m-1] -ist->map[0] +1;
  k = (m+m < n) ? n = m : 0;    /* compute the range of items */
  #ifdef BENCH                  /* if benchmark version, */
  ist->sccnt += n;              /* sum the number of counters */
  ist->mapsz += k;              /* sum the size of the maps */
  ist->ndcnt += 1;              /* count the node to be created */
  #endif

  /* --- create child --- */
  curr = (ISTNODE*)malloc(sizeof(ISTNODE) +(size_t)(n-1) *sizeof(SUPP)
                                          +(size_t) k    *sizeof(ITEM));
  if (!curr) return (ISTNODE*)-1;      /* create a child node */
  if (hdonly) item |= HDONLY;   /* set the head only flag and */
  curr->item  = item;           /* initialize the item identifier */
  curr->chcnt = 0;              /* there are no children yet */
  curr->size  = n;              /* set size of counter array */
  if (k <= 0) {                 /* if to use a pure array, note */
    curr->offset = k = ist->map[0];  /* first item as an offset */
    for (i = 0; i < n; i++) curr->cnts[i] = SKIP;
    for (i = 0; i < m; i++) curr->cnts[ist->map[i]-k] = 0; }
  else {                        /* if to use an identifier map, */
    curr->offset = -1;          /* use negative offset as indicator */
    memset(curr->cnts,    0,        (size_t)n *sizeof(SUPP));
    memcpy(curr->cnts +n, ist->map, (size_t)n *sizeof(ITEM));
  }                             /* clear counters, copy item id. map */
  return curr;                  /* return pointer to created child */
}  /* child() */

/*----------------------------------------------------------------------
  In the above function the set S represented by the index-th array
element of the current node is extended only by combining it with
the sets represented by the items that follow it in the node array,
i.e. by the sets represented by vec[index+1] to vec[size-1]. The sets
that can be formed by combining the set S and the sets represented by
vec[0] to vec[index-1] are processed in the branches for these sets.
  In the 'check candidates' loop it is checked for each set represented
by vec[index+1] to vec[size-1] whether this set and all other subsets
of the same size, which can be formed from the union of this set and
the set S, have enough support, so that a counter is necessary.
  Note that for a pure array representation index +offset is the
identifier of the item that has to be added to set S to form the union
of the set S and the set T represented by vec[index], since S and T have
the same path with the exception of the index in the current node. Hence
we can speak of candidate items that are added to S.
  Checking the support of the other subsets of the union of S and T
that have the same size as S and T is done with the aid of a path
variable. The items in this variable combined with the items on the
path to the current node always represent the subset currently tested.
That is, the path variable holds the path to be followed from the
current node to arrive at the support counter for the subset. The path
variable is initialized to [0]: <item>, [1]: <offset+index>, since the
support counters for S and T can be inspected directly. Then this
path is followed from the parent node of the current node, which is
equivalent to checking the subset that can be obtained by removing
from the union of S and T the item that corresponds to the parent node
(in the path to S or T, resp.).
  Iteratively making the parent node the current node, adding its
corresponding item to the path and checking the support counter at the
end of the path variable when starting from its (the new current node's)
parent node tests all other subsets.
  Another criterion is that the extended set must not contain two items
which may appear only in the head of a rule. If two such items are
contained in a set, neither can a rule be formed from its items nor
can it be the antecedent of a rule. Whether a set contains two head
only items is determined from the nodes 'hdonly' flag and the
appearance flags of the items.
----------------------------------------------------------------------*/

static ISTNODE** children (ISTREE *ist, ISTNODE **np, ISTNODE **end)
{                               /* --- create children of a node */
  ITEM    i, n;                 /* loop variable, node counter */
  size_t  z;                    /* size of counter and map arrays */
  SUPP    pex;                  /* support for a perfect extension */
  ISTNODE *node;                /* node to get children */
  ISTNODE *par;                 /* parent of current node */
  ISTNODE *cur;                 /* current node in new level (child) */
  ISTNODE **frst;               /* first child of current node */
  ISTNODE *last;                /* last  child of current node */
  ISTNODE **chn;                /* child node array */

  assert(ist && np && end);     /* check the function arguments */
  node = *np;                   /* get the node to get children */
  frst = end; last = NULL;      /* note start of the child node list */
  if (!(ist->mode & IST_PERFECT)) pex = SUPP_MAX;
  else if (!node->parent)         pex = ist->wgt;
  else pex = getsupp(node->parent, &node->item, 1);
  pex = COUNT(pex);             /* get support for perfect extension */
  for (i = n = 0; i < node->size; i++) {
    cur = child(ist,node,i,pex);/* traverse the counter array */
    if (!cur) continue;         /* create a child node if necessary */
    if (cur == (void*)-1) { *end = NULL; return NULL; }
    *end = last = cur;          /* add node at the end of the list */
    end  = &cur->succ; n++;     /* that contains the new level */
  }                             /* and advance the end pointer */
  *end = NULL;                  /* terminate the child node list */
  if (n <= 0) {                 /* if no child node was created, */
    node->chcnt = ITEM_MIN; return end; }       /* skip the node */
  #ifdef BENCH                  /* if benchmark version, */
  ist->cpnec += n;              /* sum the number of */
  #endif                        /* necessary child pointers */
  chn = np; par = node->parent; /* get the parent node */
  if (par) {                    /* if there is a parent node */
    if (par->offset >= 0) {     /* if a pure array is used */
      chn = (ISTNODE**)(par->cnts +par->size);
      ALIGN(chn);               /* get the child node array */
      chn += ITEMOF(node) -ITEMOF(chn[0]); }
    else {                      /* if an identifier map is used */
      chn = (ISTNODE**)((ITEM*)(par->cnts +par->size) +par->size);
      ALIGN(chn);               /* get the child node array */
      chn += search(ITEMOF(node), chn, CHILDCNT(par));
    }                           /* find the child node pointer */
  }                             /* in the parent node */
  /* The location of the child pointer in the parent node must be  */
  /* retrieved here, because ITEMOF(chn[0]) (for a pure array) may */
  /* access already freed memory after the node has been resized,  */
  /* since it may be that node = chn[0] and then chn[0] may become */
  /* by the reallocation of the node to add the child pointers.    */
  if (node->offset >= 0) {      /* if a pure counter array is used */
    z = (size_t)(node->size-1) *sizeof(SUPP);
    n = ITEMOF(last) -ITEMOF(*frst) +1; } /* pure child array */
  else {                        /* if an identifier map is used */
    z = (size_t)(node->size-1) *sizeof(SUPP)
      + (size_t) node->size    *sizeof(ITEM);
  }                             /* add a compact child array */
  z += sizeof(ISTNODE);         /* add the node size */
  node = (ISTNODE*)realloc(node, z+PAD(z) +(size_t)n*sizeof(ISTNODE*));
  if (!node) return NULL;       /* add a child array to the node */
  *np = *chn = node;            /* update the node pointer and */
  node->chcnt = n;              /* note the number of children */
  #ifdef BENCH                  /* if benchmark version, */
  ist->cpcnt += n;              /* sum the number of child pointers */
  #endif                        /* (whether necessary or not) */
  if (node->offset >= 0) {      /* if a pure array is used */
    chn = (ISTNODE**)(node->cnts +node->size);
    ALIGN(chn);                 /* get the child node array */
    while (--n >= 0) chn[n] = NULL;
    i = ITEMOF(*frst);          /* get the child node array */
    for (cur = *frst; cur; cur = cur->succ) {
      chn[ITEMOF(cur)-i] = cur; /* set the child node pointer */
      cur->parent = node;       /* and the parent pointer */
    } }
  else {                        /* if an identifier map is used */
    chn = (ISTNODE**)((ITEM*)(node->cnts +node->size) +node->size);
    ALIGN(chn);                 /* get the child node array */
    for (i = 0, cur = *frst; cur; cur = cur->succ) {
      chn[i++]    = cur;        /* set the child node pointer */
      cur->parent = node;       /* and the parent pointer */
    }                           /* in the new node */
  }                             /* (store pointers to children) */
  return end;                   /* return new end of node list */
}  /* children() */

/*--------------------------------------------------------------------*/

static int needed (ISTNODE *node)
{                               /* --- recursively check nodes */
  int     r;                    /* check result */
  ITEM    i;                    /* array index */
  ISTNODE **chn;                /* child node array */

  assert(node);                 /* check the function argument */
  if (node->chcnt <= 0)         /* skip already marked subtrees, */
    return (node->chcnt == 0) ? -1 : 0;    /* but not new leaves */
  i   = (node->offset < 0) ? node->size : 0;
  chn = (ISTNODE**)((ITEM*)(node->cnts +node->size) +i);
  ALIGN(chn);                   /* get the child node array */
  for (r = 0, i = node->chcnt; --i >= 0; )
    if (chn[i]) r |= needed(chn[i]);
  if (r) return -1;             /* recursively check all children */
  node->chcnt |= ITEM_MIN;      /* set the skip flag if possible */
  return 0;                     /* return 'subtree can be skipped' */
}  /* needed() */

/*--------------------------------------------------------------------*/

static void cleanup (ISTREE *ist)
{                               /* --- clean up on error */
  ISTNODE *node, *t;            /* to traverse the nodes */

  assert(ist);                  /* check the function argument */
  for (node = ist->lvls[ist->height]; node; ) {
    t = node; node = node->succ; free(t); }
  ist->lvls[ist->height] = NULL;/* delete all created nodes */
  for (node = ist->lvls[ist->height-1]; node; node = node->succ)
    node->chcnt = 0;            /* clear the child node counters */
}  /* cleanup() */              /* of the deepest nodes in the tree */

/*--------------------------------------------------------------------*/

int ist_addlvl (ISTREE *ist)
{                               /* --- add a level to item set tree */
  ISTNODE **np;                 /* to traverse the nodes */
  ISTNODE **end;                /* end of node list of new level */

  assert(ist);                  /* check the function arguments */
  if (!ist->valid)              /* if the levels are not valid, */
    makelvls(ist);              /* set the successor pointers */
  end  = ist->lvls +ist->height;
  *end = NULL;                  /* start a new tree level */
  for (np = ist->lvls +ist->height -1; *np; np = &(*np)->succ) {
    end = children(ist,np,end); /* traverse the node level list */
    if (!end) { cleanup(ist); return -1; }
  }                             /* create children and update node */
  if (!ist->lvls[ist->height])  /* if no child has been added, */
    return 1;                   /* abort the function, otherwise */
  ist->height += 1;             /* increment the level counter */
  needed(ist->lvls[0]);         /* mark unnecessary subtrees */
  return 0;                     /* return 'ok' */
}  /* ist_addlvl() */

/*--------------------------------------------------------------------*/

void ist_root (ISTREE *ist)
{                               /* --- go to the root node */
  assert(ist);                  /* check the function argument */
  ist->curr  = ist->lvls[0];    /* go to the root node */
  ist->depth = 1;               /* and init. the node depth */
}  /* ist_root() */

/*--------------------------------------------------------------------*/

void ist_up (ISTREE *ist)
{                               /* --- go up in item set tree */
  assert(ist && ist->curr);     /* check the function argument */
  if (!ist->curr->parent) return;  /* check for a parent node */
  ist->curr   = ist->curr->parent; /* go to the parent node and */
  ist->depth -= 1;                 /* decrease the node depth */
}  /* ist_up() */

/*--------------------------------------------------------------------*/

int ist_down (ISTREE *ist, ITEM item)
{                               /* --- go down in item set tree */
  ISTNODE *node;                /* current node */
  ISTNODE **chn;                /* child node array */
  ITEM    cnt, i;               /* number of children, index */

  assert(ist && ist->curr);     /* check the function argument */
  node = ist->curr;             /* get the current node */
  cnt  = CHILDCNT(node);        /* if there are no child nodes, */
  if (cnt <= 0) return -1;      /* abort the function */
  if (node->offset >= 0) {      /* if a pure array is used */
    chn = (ISTNODE**)(node->cnts +node->size);
    ALIGN(chn);                 /* get the child node array */
    i   = item -ITEMOF(chn[0]); /* compute index in child node array */
    if ((i < 0) || (i >= cnt) || !chn[i]) return -1; }
  else {                        /* if an identifier map is used */
    chn = (ISTNODE**)((ITEM*)(node->cnts +node->size) +node->size);
    ALIGN(chn);                 /* get the child node array */
    i   = search(item, chn, cnt);
    if (i < 0) return -1;       /* search for the item in the map */
  }                             /* and check whether child exists */
  ist->curr   = chn[i];         /* go to the child node and */
  ist->depth += 1;              /* increase the node depth */
  return 0;                     /* return 'ok' */
}  /* ist_down() */

/*--------------------------------------------------------------------*/

ITEM ist_next (ISTREE *ist, ITEM item)
{                               /* --- get next item with a counter */
  ITEM    i, n;                 /* array index, map size */
  ITEM    *map;                 /* item identifier map */
  ISTNODE *node;                /* current node in tree */

  assert(ist && ist->curr);     /* check the function argument */
  node = ist->curr;             /* get the current node */
  if (node->offset >= 0) {      /* if a pure array is used, */
    i = item -node->offset;     /* compute the array index */
    if (i <  0) return node->offset;
    if (i >= node->size) return -1;
    return item +1; }           /* return the next item identifier */
  else {                        /* if an identifier map is used */
    map = (ITEM*)(node->cnts +(n = node->size));
    i = ia_bsearch(item, map, (size_t)n);
    if (i >= 0) i++;            /* try to find the item in the map */
    return (i < n) ? map[i] : -1;
  }                             /* return the following item */
}  /* ist_next() */

/*--------------------------------------------------------------------*/

SUPP ist_getsupp (ISTREE *ist, ITEM item)
{                               /* --- get support for an item */
  ITEM    i;                    /* array index */
  ITEM    *map;                 /* item identifier map */
  ISTNODE *node;                /* current node in tree */

  assert(ist && ist->curr);     /* check the function argument */
  node = ist->curr;             /* get the current node */
  if (node->offset >= 0) {      /* if pure arrays are used, */
    i = item -node->offset;     /* get index in counter array */
    if (i >= node->size) return 0; }
  else {                        /* if an identifier map is used */
    map = (ITEM*)(node->cnts +node->size);
    i   = ia_bsearch(item, map, (size_t)node->size);
  }                             /* search the item in the map */
  if (i < 0) return 0;          /* abort if item not found */
  return COUNT(node->cnts[i]);  /* return the item set support */
}  /* ist_getsupp() */

/*--------------------------------------------------------------------*/

SUPP ist_setsupp (ISTREE *ist, ITEM item, SUPP supp)
{                               /* --- get support for an item */
  ITEM    i;                    /* array index */
  ITEM    *map;                 /* item identifier map */
  ISTNODE *node;                /* current node in tree */

  assert(ist && ist->curr);     /* check the function argument */
  node = ist->curr;             /* get the current node */
  if (node->offset >= 0) {      /* if pure arrays are used, */
    i = item -node->offset;     /* get index in counter array */
    if (i >= node->size) return 0; }
  else {                        /* if an identifier map is used */
    map = (ITEM*)(node->cnts +node->size);
    i   = ia_bsearch(item, map, (size_t)node->size);
  }                             /* search the item in the map */
  if (i < 0) return 0;          /* abort if item not found */
  node->cnts[i] = supp;         /* set new item set support */
  return COUNT(supp);           /* return the item set support */
}  /* ist_setsupp() */

/*--------------------------------------------------------------------*/

SUPP ist_incsupp (ISTREE *ist, ITEM item, SUPP supp)
{                               /* --- get support for an item */
  ITEM    i;                    /* array index */
  ITEM    *map;                 /* item identifier map */
  ISTNODE *node;                /* current node in tree */

  assert(ist && ist->curr);     /* check the function argument */
  node = ist->curr;             /* get the current node */
  if (node->offset >= 0) {      /* if pure arrays are used, */
    i = item -node->offset;     /* get index in counter array */
    if (i >= node->size) return 0; }
  else {                        /* if an identifier map is used */
    map = (ITEM*)(node->cnts +node->size);
    i   = ia_bsearch(item, map, (size_t)node->size);
  }                             /* search the item in the map */
  if (i < 0) return 0;          /* abort if item not found */
  node->cnts[i] += supp;        /* increase the item set support */
  return COUNT(node->cnts[i]);  /* return the item set support */
}  /* ist_incsupp() */

/*--------------------------------------------------------------------*/

int ist_addchn (ISTREE *ist)
{                               /* --- add a child node */
  ISTNODE *list;                /* list of child nodes */
  ISTNODE **end = &list;        /* end of node list of new level */

  assert(ist && ist->curr);     /* check the function argument */
  if (CHILDCNT(ist->curr) > 0)  /* if there are children already, */
    return 1;                   /* abort the function */
  end = children(ist, &ist->curr, end);
  if (!end) return -1;          /* add children to the current node */
  if (ist->depth <= 1)          /* if currently at the root node, */
    ist->lvls[0] = ist->curr;   /* update the root node */
  if (ist->depth+1 > ist->height)
    ist->height = ist->depth+1; /* update the tree height */
  ist->valid = 0;               /* levels/successors are not valid */
  return 0;                     /* return that children were added */
}  /* ist_addchn() */

/*--------------------------------------------------------------------*/

SUPP ist_supp (ISTREE *ist, ITEM *items, ITEM n)
{                               /* --- get support of an item set */
  assert(ist                    /* check the function arguments */
  &&    (n >= 0) && (items || (n <= 0)));
  if (n <= 0)                   /* if the item set is empty, */
    return COUNT(ist->wgt);     /* return the total trans. weight */
  return COUNT(getsupp(ist->lvls[0], items, n));
}  /* ist_supp() */             /* return the item set support */

/*--------------------------------------------------------------------*/

void ist_clear (ISTREE *ist)
{                               /* --- clear all node markers */
  ITEM    i, h;                 /* loop variables, buffers */
  ISTNODE *node;                /* to traverse the nodes */

  assert(ist);                  /* check the function argument */
  if (!ist->valid)              /* if the levels are not valid, */
    makelvls(ist);              /* set the successor pointers */
  CLRSKIP(ist->wgt);            /* clear skip flag of empty set */
  for (h = ist->height; --h >= 0; )
    for (node = ist->lvls[h]; node; node = node->succ)
      for (i = node->size; --i >= 0; )
        CLRSKIP(node->cnts[i]); /* traverse the tree levels */
}  /* ist_clear() */

/*--------------------------------------------------------------------*/

void ist_filter (ISTREE *ist, ITEM size)
{                               /* --- filter frequent item sets */
  ITEM    i, k, n, h;           /* loop variables, buffers */
  ITEM    *path;                /* path to follow for subset support */
  ISTNODE *node, *curr;         /* to traverse the nodes */

  assert(ist);                  /* check the function argument */
  if (!ist->valid)              /* if the levels are not valid, */
    makelvls(ist);              /* set the successor pointers */
  for (h = ist->height; --h > 0; ) {
    for (node = ist->lvls[h]; node; node = node->succ) {
      for (i = node->size; --i >= 0; ) {
        if ((node->cnts[i] < ist->smin)
        ||  (ist->dir *evaluate(ist, node, i) < ist->thresh))
          SETSKIP(node->cnts[i]);
      }                         /* traverse all nodes of the tree */
    }                           /* and all counters in each node and */
  }                             /* mark sets that do not qualify */
  if      (size < 0) {          /* -- weak filtering with evaluation */
    if (size > -2) size = -2;   /* traverse the tree levels */
    for (h = -size; h < ist->height; h++) {
      for (node = ist->lvls[h]; node; node = node->succ) {
        curr = node->parent;    /* traverse the nodes on each level */
        k = ITEMOF(node);       /* and check the direkt parent */
        k = (curr->offset >= 0) ? k -curr->offset
          : ia_bsearch(k, (ITEM*)(curr->cnts +curr->size),
                          (size_t)curr->size);
        if (curr->cnts[k] >= ist->smin)
          continue;             /* abort search if parent qualifies */
        for (i = node->size; --i >= 0; ) {
          path = ist->buf +ist->height +1;
          *--path = ITEMAT(node, i);
          *--path = ITEMOF(node);   /* initialize the path */
          n = 1;                /* with the last two items */
          for (curr = node->parent; curr; curr = curr->parent) {
            if (getsupp(curr, path+1, n) >= ist->smin) break;
            *--path = ITEMOF(curr); n += 1;
          }                     /* try to find a qualifying subset */
          if (!curr) SETSKIP(node->cnts[i]);
        }                       /* if the whole path to the root */
      }                         /* was traversed, but no qualifying */
    } }                         /* subset was found, mark item set */
  else if (size > 0) {          /* -- strong filtering with evaluation*/
    if (size < 2) size = 2;     /* traverse the tree levels */
    for (h = size; h < ist->height; h++) {
      for (node = ist->lvls[h]; node; node = node->succ) {
        curr = node->parent;    /* traverse the nodes on each level */
        k = ITEMOF(node);       /* and check the direkt parent */
        k = (curr->offset >= 0) ? k -curr->offset
          : ia_bsearch(k, (ITEM*)(curr->cnts +curr->size),
                          (size_t)curr->size);
        if (curr->cnts[k] < ist->smin) {
          for (i = node->size; --i >= 0; )
            SETSKIP(node->cnts[i]);
          continue;             /* if the direct parent is invalid, */
        }                       /* all sets in the node can be marked */
        for (i = node->size; --i >= 0; ) {
          path = ist->buf +ist->height +1;
          *--path = ITEMAT(node, i);
          *--path = ITEMOF(node);   /* initialize the path */
          n = 1;                /* with the last two items */
          for (curr = node->parent; curr; curr = curr->parent) {
            if (getsupp(curr, path+1, n) < ist->smin) break;
            *--path = ITEMOF(curr); n += 1;
          }                     /* try to find a qualifying subset */
          if (curr) SETSKIP(node->cnts[i]);
        }                       /* if on the way to the root */
      }                         /* a subset was found that does */
    }                           /* not qualify, mark the item set */
  }
  if (((ist->dir < 0) ? -1 : 0) < ist->thresh) {
    SETSKIP(ist->wgt);          /* if the empty set and singletons */
    node = ist->lvls[0];        /* do not reach the eval. threshold */
    for (i = node->size; --i >= 0; ) SETSKIP(node->cnts[i]);
  }                             /* mark them all with a skip flag */
}  /* ist_filter() */

/*--------------------------------------------------------------------*/

static void clear (ISTNODE *node, ITEM *items, ITEM n, SUPP supp)
{                               /* --- clear an item set flag */
  ITEM    i, k;                 /* array index, map size */
  ITEM    *map;                 /* item identifier map */
  ISTNODE **chn;                /* child node array */

  assert(node                   /* check the function arguments */
  &&    (n >= 0) && (items || (n <= 0)));
  while (--n > 0) {             /* follow the set/path from the node */
    if (node->offset >= 0) {    /* if a pure array is used */
      chn = (ISTNODE**)(node->cnts +node->size);
      ALIGN(chn);               /* get the child node array */
      i   = *items++ -ITEMOF(chn[0]); }
    else {                      /* if an identifier map is used */
      chn = (ISTNODE**)((ITEM*)(node->cnts +node->size) +node->size);
      ALIGN(chn);               /* get the child node array */
      i   = search(*items++, chn, CHILDCNT(node));
    }                           /* get the proper child array index */
    node = chn[i];              /* go to the corresponding child */
  }
  if (node->offset >= 0)        /* if a pure array is used, */
    i   = *items -node->offset; /* compute the counter index */
  else {                        /* if an identifier map is used */
    map = (ITEM*)(node->cnts +(k = node->size));
    i   = ia_bsearch(*items, map, (size_t)k);
  }                             /* search for the proper index */
  if (node->cnts[i] <= supp)    /* if the support is low enough, */
    CLRSKIP(node->cnts[i]);     /* clear skip flag of the item set */
}  /* clear() */

/*--------------------------------------------------------------------*/

void ist_clomax (ISTREE *ist, int target)
{                               /* --- filter for closed/maximal sets */
  ITEM    i, k, n, h;           /* loop variables, buffers */
  ITEM    item;                 /* buffer for an item */
  SUPP    supp;                 /* minimum support for a superset */
  ITEM    *map;                 /* item identifier map */
  ITEM    *path;                /* path to access superset support */
  ISTNODE *node, *curr;         /* to traverse the nodes */
  ISTNODE **chn;                /* child node array */

  assert(ist);                  /* check the function argument */
  if (!ist->valid)              /* if the levels are not valid, */
    makelvls(ist);              /* set the successor pointers */

  /* --- safe filtering --- */
  if (target & IST_SAFE) {      /* if to filter in a safe way */
    supp = SUPP_MAX;            /* set default support filter (max.) */
    for (k = ist->height; --k > 0; ) { /* traverse the tree top down */
      for (node = ist->lvls[k]; node; node = node->succ) {
        for (i = node->size; --i >= 0; ) {  /* traverse all sets */
          if (node->cnts[i] < ist->smin) {  /* of all nodes */
            SETSKIP(node->cnts[i]); continue; }
          if (!(target & IST_MAXIMAL)) supp = node->cnts[i];
          curr = node->parent;  /* get parent of the current node */
          path = ist->buf +ist->height;
          *--path = ITEMAT(node, i); /* mark item corresp. to index */
          clear(curr, path, 1, supp);
          *--path = ITEMOF(node);    /* mark item corresp. to node */
          clear(curr, path, 1, supp);
          for (n = 1; curr->parent; curr = curr->parent) {
            clear(curr->parent, path, ++n, supp);
            *--path = ITEMOF(curr);
          }                     /* climb up the tree and clear */
        }                       /* skip flags for all n-1 subsets */
      }                         /* if their support does not */
    }                           /* exceed the value of supp */
  }                             /* (remove all possible holes) */
  /* If the evaluation of an item set was used to filter the found */
  /* frequent item sets, the set of found item sets may no longer  */
  /* be closed. The above procedure restores it to a closed set.   */

  /* --- filter generators --- */
  if (target & ISR_GENERAS) {   /* if to filter for generators */
    node = ist->lvls[0];        /* traverse the root node elements */
    for (i = node->size; --i >= 0; ) {
      if ((node->cnts[i] <  ist->smin)
      ||  (node->cnts[i] >= ist->wgt))
        SETSKIP(node->cnts[i]); /* mark all infrequent items */
    }                           /* and single item generators */
    for (h = 0; ++h < ist->height; ) {  /* traverse the tree levels */
      for (node = ist->lvls[h]; node; node = node->succ) {
        for (i = node->size; --i >= 0; ) {    /* traverse the nodes */
          supp = node->cnts[i]; /* traverse the nodes on each level */
          if (supp < ist->smin){/* check for minimum support */
            SETSKIP(node->cnts[i]); continue; }
          curr = node->parent;  /* check the direkt parent */
          k = ITEMOF(node);     /* for equal support */
          k = (curr->offset >= 0) ? k -curr->offset
            : ia_bsearch(k, (ITEM*)(curr->cnts +curr->size),
                            (size_t)curr->size);
          if (curr->cnts[k] <= supp) {
            SETSKIP(node->cnts[i]); continue; }
          path = ist->buf +ist->height +1;
          *--path = ITEMAT(node, i);
          *--path = ITEMOF(node);   /* initialize the path */
          n = 1;                /* with the last two items */
          for (curr = node->parent; curr; curr = curr->parent) {
            if (getsupp(curr, path+1, n) <= supp) break;
            *--path = ITEMOF(curr); n += 1;
          }                     /* try to find a qualifying subset */
          if (curr) SETSKIP(node->cnts[i]);
        }                       /* if on the path to the root */
      }                         /* a subset could be found */
    }                           /* that has the same support, */
    return;                     /* the set is not a generator */
  }                             /* abort when filtering finishes */

  /* --- check empty set --- */
  supp = (target & IST_MAXIMAL) ? ist->smin : ist->wgt;
  node = ist->lvls[0];          /* traverse the root node elements */
  for (i = node->size; --i >= 0; )  /* mark empty set if necessary */
    if (node->cnts[i] >= supp) { SETSKIP(ist->wgt); break; }

  /* --- process intermediate levels --- */
  supp = SUPP_MAX;              /* set default support filter (max.) */
  for (h = 0; h < ist->height-1; h++) {   /* traverse the tree levels */
    for (node = ist->lvls[h]; node; node = node->succ) {
      for (i = node->size; --i >= 0; ) {  /* traverse the nodes */
        if (node->cnts[i] < ist->smin) {  /* check for min. support */
          SETSKIP(node->cnts[i]); continue; }
        item = ITEMAT(node, i); /* get item and min. superset support */
        supp = (target & IST_MAXIMAL) ? ist->smin : node->cnts[i];

        /* -- check supersets in child -- */
        n = CHILDCNT(node);     /* get the number of children */
        if (n > 0) {            /* if there are child nodes */
          if (node->offset >= 0) { /* if pure array is used */
            chn  = (ISTNODE**)(node->cnts +node->size);
            ALIGN(chn);         /* get the child node array */
            k    = item -ITEMOF(chn[0]);
            curr = ((k < 0) || (k >= n)) ? NULL : chn[k]; }
          else {                /* if an identifier map is used */
            chn  = (ISTNODE**)((ITEM*)(node->cnts +node->size)
                                                  +node->size);
            ALIGN(chn);         /* get the child node array */
            k    = search(item, chn, n);
            curr = (k < 0)               ? NULL : chn[k];
          }                     /* get child node for current item */
          if (curr) {           /* if the child node exists */
            for (k = curr->size; --k >= 0; )
              if (curr->cnts[k] >= supp) break;
            if (k >= 0) { SETSKIP(node->cnts[i]); continue; }
          }                     /* if a superset in the tail has */
        }                       /* sufficient support, mark set */

        /* -- check other supersets -- */
        path = ist->buf +ist->height +1;
        *--path = item;         /* init. the path for lookups and */
        n = 1;                  /* traverse the path to the root */
        if (ist->mode & IST_REVERSE) {
          for (curr = node; curr; curr = curr->parent) {
            if (curr->offset >= 0) {   /* if a pure array is used */
              k = *path -curr->offset; /* get index of current item */
              while (++k < curr->size) {  /* traverse succ. items */
                path[-1] = curr->offset +k;
                if (getsupp(curr, path-1, n+1) >= supp) break;
              } }               /* if a superset qualifies, abort */
            else {              /* if an identifier map is used */
              map = (ITEM*)(curr->cnts +(k = curr->size));
              k   = ia_bisect(*path, map, (size_t)k);
              while (++k < curr->size) {  /* traverse succ. items */
                path[-1] = ((ITEM*)(curr->cnts +curr->size))[k];
                if (getsupp(curr, path-1, n+1) >= supp) break;
              }                 /* if a superset qualifies, abort */
            }
            if (k < curr->size) /* if a superset has been found, */
              break;            /* abort the search */
            if (path <= ist->buf) { curr = NULL; break; }
            *--path = ITEMOF(curr); n += 1;
          } }                   /* extend the item set suffix/path */
        else {
          for (curr = node; curr; curr = curr->parent) {
            if (curr->offset >= 0) {   /* if a pure array is used */
              k = *path -curr->offset; /* get index of current item */
              if (k > curr->size) k = curr->size;
              while (--k >= 0){ /* traverse the preceding items */
                path[-1] = curr->offset +k;
                if (getsupp(curr, path-1, n+1) >= supp) break;
              } }               /* if a superset qualifies, abort */
            else {              /* if an identifier map is used */
              map = (ITEM*)(curr->cnts +(k = curr->size));
              k   = ia_bisect(*path, map, (size_t)k);
              while (--k >= 0){ /* traverse the preceding items */
                path[-1] = ((ITEM*)(curr->cnts +curr->size))[k];
                if (getsupp(curr, path-1, n+1) >= supp) break;
              }                 /* if a superset qualifies, abort */
            }
            if (k >= 0)         /* if a superset has been found, */
              break;            /* abort the search */
            if (path <= ist->buf) { curr = NULL; break; }
            *--path = ITEMOF(curr); n += 1;
          }                     /* extend the item set suffix/path */
        }
        if (curr) SETSKIP(node->cnts[i]);
      }                         /* if frequent/equal support superset */
    }                           /* was found, the current item set */
  }                             /* is not closed/maximal, resp. */

  /* --- process deepest level --- */
  for (node = ist->lvls[h]; node; node = node->succ)
    for (i = node->size; --i >= 0; )
      if (node->cnts[i] < ist->smin)
        SETSKIP(node->cnts[i]); /* mark infrequent item sets only */
  /* All frequent item sets on the deepest level tree must be closed */
  /* and maximal, because they do not have supersets (in the tree).  */
}  /* ist_clomax() */

/*--------------------------------------------------------------------*/

void ist_setsize (ISTREE *ist, ITEM zmin, ITEM zmax)
{                               /* --- set the set/rule size range */
  assert(ist);                  /* check the function arguments */
  ist->zmin = zmin;             /* store the set/rule size range */
  ist->zmax = zmax;             /* (minimum and maximum size) */
}  /* ist_setsize() */

/*--------------------------------------------------------------------*/

void ist_seteval (ISTREE *ist, int eval, int agg,
                  double thresh, ITEM prune)
{                               /* --- set additional evaluation */
  assert(ist);                  /* check the function arguments */
  ist->invbxs = eval & IST_INVBXS; eval &= ~IST_INVBXS;
  ist->eval   = ((eval > RE_NONE) && (eval < RE_FNCNT))
              ? eval : RE_NONE; /* check and note the eval. measure */
  ist->agg    = ((agg  > IST_NONE) && (agg  <= IST_AVG))
              ? agg  : IST_NONE;/* check and note the agg. mode */
  ist->dir    = re_dir(ist->eval);
  ist->thresh = ist->dir*thresh;/* note the evaluation parameters */
  ist->prune  = (prune <= 0) ? ITEM_MAX : (prune > 1) ? prune : 2;
}  /* ist_seteval() */

/*--------------------------------------------------------------------*/

void ist_init (ISTREE *ist, int order)
{                               /* --- initialize (rule) extraction */
  assert(ist);                  /* check the function argument */
  if (!ist->valid)              /* if the levels are not valid, */
    makelvls(ist);              /* set the successor pointers */
  ist->order = order;           /* note the traversal order */
  ist->size  = (order >= 0)     /* set the size to start at */
             ? ((ist->zmin >  0)           ? ist->zmin : 0)
             : ((ist->zmax <  ist->height) ? ist->zmax : ist->height);
  ist->node  = ist->lvls[(ist->size > 0) ? ist->size-1 : 0];
  ist->item  = ist->index = -1;
  ist->head  = NULL;            /* init. the extraction variables */
}  /* ist_init() */

/*--------------------------------------------------------------------*/

static int emptyset (ISTREE *ist, SUPP *supp, double *eval)
{                               /* --- whether to report empty set */
  assert(ist);                  /* check the function argument */
  ist->size += ist->order;      /* immediately go the next level */
  if ((ist->wgt  >= ist->smin)  /* if the empty set qualifies */
  && ((ist->eval == IST_NONE) || (0 >= ist->thresh))) {
    if (supp) *supp = COUNT(ist->wgt);
    if (eval) *eval = (ist->dir < 0) ? 1 : 0;
    return -1;                  /* store support and add. evaluation */
  }                             /* return 'report empty set' */
  return 0;                     /* return 'do not report empty set' */
}  /* emptyset() */

/*--------------------------------------------------------------------*/

ITEM ist_iset (ISTREE *ist, ITEM *set, SUPP *supp, double *eval)
{                               /* --- extract next frequent item set */
  ITEM    i;                    /* loop variable, buffer */
  ITEM    item;                 /* item identifier */
  ISTNODE *node;                /* current item set node */
  SUPP    curr;                 /* support of the current set */
  double  val;                  /* value of evaluation measure */

  assert(ist && set);           /* check the function arguments */
  if ((ist->size < ist->zmin)   /* if below the minimal size */
  ||  (ist->size > ist->zmax))  /* or above the maximal size, */
    return -1;                  /* abort the function */
  if ((ist->size == 0)          /* if to report the empty item set */
  &&  emptyset(ist, supp, eval))
    return  0;                  /* check whether it qualifies */

  /* --- find frequent item set --- */
  node = ist->node;             /* get the current item set node */
  while (1) {                   /* search for a frequent item set */
    if (++ist->index >= node->size) { /* if all subsets have been */
      node = node->succ;        /* processed, go to the successor */
      while (!node) {           /* if at the end of a level, */
        ist->size += ist->order;/* go to the next level */
        if ((ist->size < ist->zmin) || (ist->size < 0)
        ||  (ist->size > ist->zmax) || (ist->size > ist->height))
          return -1;            /* if outside size range, abort */
        if ((ist->size == 0)    /* if to report the empty item set */
        &&  emptyset(ist, supp, eval))
          return  0;            /* check whether it qualifies */
        node = ist->lvls[ist->size-1];
      }                         /* get the 1st node of the new level */
      ist->node  = node;        /* note the new item set node */
      ist->index = 0;           /* start with the first item set */
    }                           /* of the new item set tree node */
    item = ITEMAT(node, ist->index);     /* get the current item */
    if (ib_getapp(ist->base, item) == APP_NONE)
      continue;                 /* skip items to ignore */
    curr = node->cnts[ist->index];
    if (curr < ist->smin)       /* if the item set is not frequent, */
      continue;                 /* go to the next item set */
    /* Note that this check automatically skips all item sets that */
    /* are marked with the flag SKIP, because curr is negative     */
    /* with this flag and thus necessarily smaller than ist->smin. */
    if (ist->eval <= IST_NONE){ /* if no add. eval. measure given */
      val = 0; break; }         /* abort the loop (select the set) */
    val = evaluate(ist, node, ist->index);
    if (ist->dir *val >= ist->thresh)
      break;                    /* if the evaluation is high enough, */
  }  /* while (1) */            /* abort the loop (select the set) */
  if (supp) *supp = curr;       /* store the item set support and */
  if (eval) *eval = val;        /* the value of the add. measure */

  /* --- build frequent item set --- */
  i        = ist->size;         /* get the current item set size */
  set[--i] = item;              /* and store the first item */
  while (node->parent) {        /* while not at the root node */
    set[--i] = ITEMOF(node);    /* add item to the item set */
    node = node->parent;        /* and go to the parent node */
  }
  return ist->size;             /* return the item set size */
}  /* ist_iset() */

/*--------------------------------------------------------------------*/

ITEM ist_rule (ISTREE *ist, ITEM *rule,
               SUPP *sset, SUPP *sbody, SUPP *shead, double *eval)
{                               /* --- extract next association rule */
  ITEM      i;                  /* loop variable */
  ITEM      item;               /* an item identifier */
  ISTNODE   *node;              /* current item set node */
  ISTNODE   *parent;            /* parent of the item set node */
  ITEM      *map, n;            /* identifier map and its size */
  SUPP      base;               /* base support (number of trans.) */
  SUPP      supp;               /* support of set  (body & head) */
  SUPP      body;               /* support of body (antecedent) */
  SUPP      head;               /* support of head (consequent) */
  double    val;                /* value of evaluation measure */
  int       app;                /* appearance flag of head item */
  RULEVALFN *refn;              /* rule evaluation function */

  assert(ist && rule);          /* check the function arguments */
  if (ist->size == 0)           /* if at the empty item set, */
    ist->size += ist->order;    /* go to the next item set size */
  if ((ist->size < ist->zmin)   /* if the item set is too small */
  ||  (ist->size > ist->zmax))  /* or too large (number of items), */
    return -1;                  /* abort the function */

  /* --- find rule --- */
  base = COUNT(ist->wgt);       /* get the base support and */
  node = ist->node;             /* the current item set node */
  refn = ((ist->eval > RE_NONE) && (ist->eval < RE_FNCNT))
       ? re_function(ist->eval) : (RULEVALFN*)0;
  while (1) {                   /* search for a rule */
    if (ist->item >= 0) {       /* --- select next item subset */
      *--ist->path = ist->item; /* add previous head to the path and */
      ist->item = ITEMOF(ist->head);       /* get the next head item */
      ist->head = ist->head->parent;
      if (!ist->head)           /* if all subsets have been processed */
        ist->item = -1;         /* clear the head item to trigger the */
    }                           /* selection of a new item set */
    if (ist->item < 0) {        /* --- select next item set */
      if (++ist->index >= node->size){/* if all subsets have been */
        node = node->succ;      /* processed, go to the successor */
        while (!node) {         /* if at the end of a level, */
          ist->size += ist->order;  /* go to the next level */
          if ((ist->size < ist->zmin) || (ist->size < 0)
          ||  (ist->size > ist->zmax) || (ist->size > ist->height))
            return -1;          /* if outside the size range, abort */
          node = ist->lvls[ist->size-1];
        }                       /* get the 1st node of the new level */
        ist->node  = node;      /* note the new item set node and */
        ist->index = 0;         /* start with the first item set */
      }                         /* of the new item set tree node */
      item = ITEMAT(node, ist->index);   /* get the current item */
      app  = ib_getapp(ist->base, item);
      if ((app == APP_NONE) || ((app == APP_HEAD) && ISHDONLY(node)))
        continue;               /* skip sets with two head only items */
      ist->item   = item;       /* set the head item identifier */
      ist->hdonly = (app == APP_HEAD) || ISHDONLY(node);
      ist->head   = node;       /* set the new head item node */
      ist->path   = ist->buf +ist->height;
    }                           /* clear the path (reinitialize it) */
    app = ib_getapp(ist->base, ist->item);
    if (!(app &  APP_HEAD)      /* get head item appearance indicator */
    ||  ((app != APP_HEAD) && ist->hdonly))
      continue;                 /* if rule is not allowed, skip it */
    supp = COUNT(node->cnts[ist->index]);
    if (supp < ist->smin) {     /* if the item set is not frequent, */
      ist->item = -1; continue; }        /* go to the next item set */
    parent = node->parent;      /* get the parent node */
    n = (ITEM)(ist->buf +ist->height -ist->path);
    if (n > 0)                  /* if there is a path, use it */
      body = COUNT(getsupp(ist->head, ist->path, n));
    else if (!parent)           /* if there is no parent (root node), */
      body = COUNT(ist->wgt);   /* get the total transation weight */
    else if (parent->offset >= 0)   /* if a pure array is used */
      body = COUNT(parent->cnts[ITEMOF(node) -parent->offset]);
    else {                      /* if an identifier map is used */
      map  = (ITEM*)(parent->cnts +(n = parent->size));
      i    = ia_bsearch(ITEMOF(node), map, (size_t)n);
      body = COUNT(parent->cnts[i]);
    }                           /* find array index and get support */
    if ((body < ist->body)      /* check the body support */
    ||  ((double)supp < (double)body *ist->conf))
      continue;                 /* check the rule confidence */
    head = COUNT(ist->lvls[0]->cnts[ist->item]);
    if (!refn) {                /* if no add. eval. measure given, */
      val = 0; break; }         /* abort the loop (select the rule) */
    val = (!ist->invbxs         /* compute add. evaluation measure */
       || ((double)supp *(double)base > (double)head *(double)body))
        ? refn(supp, body, head, base) : (ist->dir < 0) ? 1 : 0;
    if (ist->dir *val >= ist->thresh)
      break;                    /* if the evaluation is high enough, */
  }  /* while (1) */            /* abort the loop (select the rule) */
  if (sset)  *sset  = supp;     /* store the rule support values */
  if (sbody) *sbody = body;     /* (whole rule and only body) */
  if (shead) *shead = head;     /* store the head item support, */
  if (eval)  *eval  = val;      /* the value of the add. measure */

  /* --- build rule --- */
  item = ITEMAT(node, ist->index);
  i    = ist->size;             /* get the current item and */
  if (item != ist->item)        /* if this item is not the head, */
    rule[--i] = item;           /* add it to the rule body */
  while (node->parent) {        /* traverse the path to the root */
    if (ITEMOF(node) != ist->item)
      rule[--i] = ITEMOF(node); /* add all items on this path */
    node = node->parent;        /* to the rule body */
  }                             /* (except the head of the rule) */
  rule[0] = ist->item;          /* set the head of the rule */
  return ist->size;             /* return the rule size (# of items) */
}  /* ist_rule() */

/*--------------------------------------------------------------------*/

static int isets (ISTREE *ist, ISREPORT *rep, ISTNODE *node, SUPP supp)
{                               /* --- recursive item set reporting */
  ITEM    i, k, c;              /* loop variables, buffers */
  SUPP    pex;                  /* support for perfect extension */
  ITEM    off;                  /* item offset */
  ITEM    *map;                 /* item identifier map */
  ISTNODE **chn;                /* child node array */
  double  v;                    /* value of evaluation measure */

  assert(ist && rep);           /* check the function arguments */
  if (!(ist->mode & IST_PERFECT))  /* if no perfext extension pruning */
    pex = SUPP_MAX;             /* clear perfect extension support */
  else {                        /* if perfect extensions pruning */
    pex = supp;                 /* note the parent set support */
    for (k = 0; k < node->size; k++) {
      if (COUNT(node->cnts[k]) >= pex)
        isr_addpex(rep, ITEMAT(node, k));
    }                           /* collect the perfect extensions */
  }                             /* (note that they may be redisc.) */
  if (!IS2SKIP(supp)) {         /* evaluate item set if not marked */
    v = evaluate(ist, ist->node, ist->index);
    if ((v *ist->dir >= ist->thresh)
    &&  (isr_reportv(rep, v) < 0)) return -1;
  }                             /* if item set qualifies, report it */
  if (node->offset >= 0) {      /* if a pure array is used */
    chn = (ISTNODE**)(node->cnts +node->size);
    ALIGN(chn);                 /* get the child node array */
    c   = CHILDCNT(node);       /* and the number of children */
    off = (c > 0) ? ITEMOF(chn[0]) : 0;
    for (i = 0; i < node->size; i++) {
      supp = COUNT(node->cnts[i]);
      if ((supp <  ist->smin)   /* traverse the node's items and */
      ||  (supp >= pex))        /* check against minimum support */
        continue;               /* and the parent set support */
      ist->node  = node;        /* store the node and the index */
      ist->index = i;           /* note index for evaluation */
      k = node->offset +i;      /* compute the item identifier */
      isr_add(rep, k, supp);    /* add the item to the reporter */
      supp = node->cnts[i];     /* get the item support (with flag) */
      k -= off;                 /* compute the child node index */
      if ((k >= 0)              /* if the corresp. child node exists, */
      &&  (k <  c) && chn[k])   /* recursively report the subtree */
        isets(ist, rep, chn[k], supp);
      else if (!IS2SKIP(supp)){ /* report item set if not marked */
        v = evaluate(ist, node, i);
        if ((v *ist->dir >= ist->thresh)
        &&  (isr_reportv(rep, v) < 0)) return -1;
      }                         /* if item set qualifies, report it */
      isr_remove(rep, 1);       /* remove the last item */
    } }                         /* from the current item set */
  else {                        /* if an identifier map is used */
    map = (ITEM*)(node->cnts +(k = node->size));
    chn = (ISTNODE**)(map +k);  /* get the item id map */
    c   = CHILDCNT(node);       /* and the child node array  */
    c   = (c > 0) ? ITEMOF(chn[c-1]) : -1;
    for (i = 0; i < node->size; i++) {
      supp = COUNT(node->cnts[i]);
      if ((supp <  ist->smin)   /* traverse the node's items and */
      ||  (supp >= pex))        /* check against minimum support */
        continue;               /* and the parent set support */
      ist->node  = node;        /* store the node and the index */
      ist->index = i;           /* in the node for evaluation */
      k = map[i];               /* retrieve the item identifier */
      isr_add(rep, k, supp);    /* add the item to the reporter */
      supp = node->cnts[i];     /* get the item support (with flag) */
      if (k <= c)               /* if there may be a child node, */
        while (ITEMOF(*chn) < k) chn++;  /* skip preceding items */
      if ((k <= c)              /* if the corresp. child node exists, */
      &&  (k == ITEMOF(*chn)))  /* recursively report the subtree */
        isets(ist, rep, *chn, supp);
      else if (!IS2SKIP(supp)){ /* report item set if not marked */
        v = evaluate(ist, node, i);
        if ((v *ist->dir >= ist->thresh)
        &&  (isr_reportv(rep, v) < 0)) return -1;
      }                         /* if item set qualifies, report it */
      isr_remove(rep, 1);       /* remove the last item */
    }                           /* from the current item set */
  }
  return 0;                     /* return 'ok' */
}  /* isets() */

/*--------------------------------------------------------------------*/

static int r4set (ISTREE *ist, ISREPORT *rep, ISTNODE *node, ITEM index)
{                               /* --- report rules for an item set */
  ITEM       i;                 /* loop variable */
  ITEM       item;              /* head item of the current rule */
  int        app;               /* appearance flag of head item */
  ISTNODE    *parent;           /* parent of the item set node */
  ITEM       *map, n;           /* identifier map and its size */
  SUPP       base;              /* base support (number of trans.) */
  SUPP       supp;              /* support of set  (body & head) */
  SUPP       body;              /* support of body (antecedent) */
  SUPP       head;              /* support of head (consequent) */
  double     val;               /* value of evaluation measure */
  RULEVALFN  *refn;             /* rule evaluation function */

  assert(ist                    /* check the function arguments */
  &&     rep && node && (index >= 0));
  refn = ((ist->eval > RE_NONE) && (ist->eval < RE_FNCNT))
       ? re_function(ist->eval) : (RULEVALFN*)0;
  base = COUNT(ist->wgt);       /* get base and item set support */
  supp = COUNT(node->cnts[index]);
  item = (node->offset >= 0) ? node->offset +index
       : ((ITEM*)(node->cnts +node->size))[index];
  app  = ib_getapp(ist->base, item);
  if ((app == APP_NONE) || ((app == APP_HEAD) && ISHDONLY(node)))
    return 0;                   /* skip sets with two head only items */
  parent = node->parent;        /* get the parent node */
  if (!parent)                  /* if there is no parent (root node), */
    body = COUNT(ist->wgt);     /* get the total transaction weight */
  else if (parent->offset >= 0) /* if a pure array is used */
    body = COUNT(parent->cnts[ITEMOF(node) -parent->offset]);
  else {                        /* if an identifier map is used */
    map  = (ITEM*)(parent->cnts +(n = parent->size));
    i    = ia_bsearch(ITEMOF(node), map, (size_t)n);
    body = COUNT(parent->cnts[i]);
  }                             /* find array index and get support */
  do {                          /* check and report the first rule */
    if (!(app & APP_HEAD))      /* check whether the current item */
      break;                    /* can occur as a rule head */
    if ((body < ist->body)      /* check the body support */
    ||  ((double)supp < (double)body *ist->conf))
      break;                    /* check the rule confidence */
    head = COUNT(ist->lvls[0]->cnts[item]);
    if (!refn) val = 0;         /* if no add. eval. measure given, */
    else {                      /* clear the evaluation, otherwise */
      val = (!ist->invbxs       /* compute add. evaluation measure */
         || ((double)supp *(double)base > (double)head *(double)body))
          ? refn(supp, body, head, base) : (ist->dir < 0) ? 1 : 0;
      if (ist->dir *val < ist->thresh)
        break;                  /* check whether the evaluation */
    }                           /* reaches or exceed the threshold */
    if (isr_reprule(rep, item, body, head, val) != 0) return -1;
  } while (0);                  /* report the current rule */
  ist->path = ist->buf +ist->height;
  *--ist->path = item; n = 1;   /* store head item on the path */
  for ( ; parent; node = parent, parent = node->parent) {
    body = COUNT(getsupp(parent, ist->path, n));
    *--ist->path = item = ITEMOF(node);
    n += 1;                     /* traverse the path to the root */
    if (!(ib_getapp(ist->base, item) & APP_HEAD))
      continue;                 /* check whether item can be a head */
    if ((body < ist->body)      /* check the body support */
    ||  ((double)supp < (double)body *ist->conf))
      continue;                 /* check the rule confidence */
    head = COUNT(ist->lvls[0]->cnts[item]);
    if (!refn) val = 0;         /* if no add. eval. measure given, */
    else {                      /* clear the evaluation, otherwise */
      val = (!ist->invbxs       /* compute add. evaluation measure */
         || ((double)supp *(double)base > (double)head *(double)body))
          ? refn(supp, body, head, base) : (ist->dir < 0) ? 1 : 0;
      if (ist->dir *val < ist->thresh)
        continue;               /* check whether the evaluation */
    }                           /* reaches or exceed the threshold */
    if (isr_reprule(rep, item, body, head, val) != 0) return -1;
  }                             /* report the current rule */
  return 0;                     /* return 'ok' */
}  /* r4set() */

/*--------------------------------------------------------------------*/

static int rules (ISTREE *ist, ISREPORT *rep, ISTNODE *node)
{                               /* --- recursive rule reporting */
  ITEM    i, k, c;              /* loop variables, buffers */
  ITEM    off;                  /* item offset */
  ITEM    *map;                 /* item identifier map */
  SUPP    supp;                 /* support of current item set */
  ISTNODE **chn;                /* child node array */

  assert(ist && rep);           /* check the function arguments */
  if (node->offset >= 0) {      /* if a pure array is used */
    chn = (ISTNODE**)(node->cnts +node->size);
    ALIGN(chn);                 /* get the child node array */
    c   = CHILDCNT(node);       /* and the number of children */
    off = (c > 0) ? ITEMOF(chn[0]) : 0;
    for (i = 0; i < node->size; i++) {
      supp = COUNT(node->cnts[i]);
      if (supp < ist->smin)     /* traverse the node's items and */
        continue;               /* check against minimum support */
      k = node->offset +i;      /* compute the item identifier */
      isr_add(rep, k, supp);    /* add the item to the reporter */
      k -= off;                 /* compute the child node index */
      if ((k >= 0)              /* if the corresp. child node exists, */
      &&  (k <  c) && chn[k])   /* recursively report the subtree, */
        rules(ist, rep, chn[k]);/* then report rules for item set */
      if (r4set(ist, rep, node, i) < 0) return -1;
      isr_remove(rep, 1);       /* remove the last item */
    } }                         /* from the current item set */
  else {                        /* if an identifier map is used */
    map = (ITEM*)(node->cnts +(k = node->size));
    chn = (ISTNODE**)(map +k);  /* get the item id map */
    c   = CHILDCNT(node);       /* and the child node array  */
    c   = (c > 0) ? ITEMOF(chn[c-1]) : -1;
    for (i = 0; i < node->size; i++) {
      supp = COUNT(node->cnts[i]);
      if (supp < ist->smin)     /* traverse the node's items and */
        continue;               /* check against minimum support */
      k = map[i];               /* retrieve the item identifier */
      isr_add(rep, k, supp);    /* add the item to the reporter */
      supp = node->cnts[i];     /* get the item support (with flag) */
      if (k <= c) {             /* if there may be a child node, */
        while (ITEMOF(*chn) < k) chn++;  /* skip preceding items */
        if (k == ITEMOF(*chn))  /* if the corresp. child node exists, */
          rules(ist, rep, *chn);/* recursively report the subtree, */
      }                         /* then report rules for item set */
      if (r4set(ist, rep, node, i) < 0) return -1;
      isr_remove(rep, 1);       /* remove the last item */
    }                           /* from the current item set */
  }
  return 0;                     /* return 'ok' */
}  /* rules() */

/*--------------------------------------------------------------------*/

int ist_report (ISTREE *ist, ISREPORT *rep, int target)
{                               /* --- extended item set reporting */
  int    r = 0;                 /* result of function call */
  ITEM   k;                     /* number of items in set/rule */
  SUPP   supp, body, head;      /* support of an item set/rule */
  double val;                   /* value of evaluation measure */

  assert(ist && rep);           /* check the function arguments */
  if (target & ISR_RULES) {     /* if to report association rules */
    if (!ist->order)            /* if no size order is requested */
      r = rules(ist, rep, ist->lvls[0]);
    else {                      /* if a size order is requested */
      while (1) {               /* extract assoc. rules from tree */
        k = ist_rule(ist, ist->map, &supp, &body, &head, &val);
        if (k < 0) break;       /* get the next association rule */
        r = isr_rule(rep, ist->map, k, supp, body, head, val);
        if (r < 0) break;       /* report the extracted ass. rule */
      }                         /* (the rules are traversed in */
    } }                         /* an order of size/number of items) */
  else {                        /* if to report frequent item sets */
    if (!ist->order)            /* if no size order is requested */
      r = isets(ist, rep, ist->lvls[0], ist->wgt);
    else {                      /* if a size order is requested */
      while (1) {               /* extract item sets from the tree */
        k = ist_iset(ist, ist->map,   &supp, &val);
        if (k < 0) break;       /* get the next frequent item set */
        r = isr_iset(rep, ist->map, k, supp, val, val);
        if (r < 0) break;       /* report the extracted item set */
      }                         /* (the item sets are traversed in */
    }                           /* an order of size/number of items) */
  }
  return r;                     /* return 'ok' */
}  /* ist_report() */

/*--------------------------------------------------------------------*/

double ist_eval (ISTREE *ist)
{                               /* --- evaluate current item set */
  assert(ist);                  /* check the function argument */
  return evaluate(ist, ist->node, ist->index);
}  /* ist_eval() */

/*--------------------------------------------------------------------*/

double ist_evalx (ISREPORT *rep, void *data)
{                               /* --- evaluate current item set */
  ISTREE *ist;                  /* item set tree to work on */

  assert(rep && data);          /* check the function arguments */
  ist = (ISTREE*)data;          /* type the user data */
  return evaluate(ist, ist->node, ist->index);
}  /* ist_evalx() */

/*--------------------------------------------------------------------*/
#ifdef BENCH

void ist_stats (ISTREE *ist)
{                               /* --- show search statistics */
  assert(ist);                  /* check the function argument */
  printf("number of created nodes    : %"SIZE_FMT"\n", ist->ndcnt);
  printf("number of pruned  nodes    : %"SIZE_FMT"\n", ist->ndprn);
  printf("number of item map elements: %"SIZE_FMT"\n", ist->mapsz);
  printf("number of support counters : %"SIZE_FMT"\n", ist->sccnt);
  printf("necessary support counters : %"SIZE_FMT"\n", ist->scnec);
  printf("pruned    support counters : %"SIZE_FMT"\n", ist->scprn);
  printf("number of child pointers   : %"SIZE_FMT"\n", ist->cpcnt);
  printf("necessary child pointers   : %"SIZE_FMT"\n", ist->cpnec);
  printf("pruned    child pointers   : %"SIZE_FMT"\n", ist->cpprn);
}  /* ist_stats() */

#endif
/*--------------------------------------------------------------------*/
#ifndef NDEBUG

static void showtree (ISTNODE *node, ITEMBASE *base, ITEM level)
{                               /* --- show subtree */
  ITEM    i, k, cnt;            /* loop variables, number of children */
  ISTNODE **chn;                /* child node array */

  assert(node && (level >= 0)); /* check the function arguments */
  i   = (node->offset < 0) ? node->size : 0;
  chn = (ISTNODE**)((ITEM*)(node->cnts +node->size) +i);
  ALIGN(chn);                   /* get the child node array */
  cnt = CHILDCNT(node);         /* and the number of children */
  for (i = 0; i < node->size; i++) {
    for (k = level; --k >= 0; ) printf("   ");
    k = ITEMAT(node, i);        /* print item identifier and counter */
    printf("%s", ib_name(base, k));
    printf("/%"ITEM_FMT": %"SUPP_FMT, k, COUNT(node->cnts[i]));
    if (IS2SKIP(node->cnts[i])) printf("*");
    printf("\n");               /* print a skip flag indicator */
    if (cnt <= 0) continue;     /* check whether there are children */
    if (node->offset >= 0) k -= ITEMOF(chn[0]);
    else                   k  = (int)search(k, chn, cnt);
    if ((k >= 0) && (k < cnt) && chn[k])
      showtree(chn[k], base, level +1);
  }                             /* show subtree recursively */
}  /* showtree() */

/*--------------------------------------------------------------------*/

void ist_show (ISTREE *ist)
{                               /* --- show an item set tree */
  assert(ist);                  /* check the function argument */
  showtree(ist->lvls[0], ist->base, 0);
  printf("total: %"SUPP_FMT"\n", COUNT(ist->wgt));
}  /* ist_show() */             /* show the nodes recursively */

#endif
