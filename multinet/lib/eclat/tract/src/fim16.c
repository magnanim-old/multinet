/*----------------------------------------------------------------------
  File    : fim16.c
  Contents: frequent item set mining with 16 items / 16 items machine
  Author  : Christian Borgelt
  History : 2011.08.24 file created
            2011.08.28 output of item set counters per size added
            2011.09.17 min. support and reporter moved to m16_create()
            2011.09.19 function m16_clear() added (remove all trans.)
            2011.09.20 perfect extension support removed from m16_mine()
            2011.09.21 function m16_show() added (for debugging)
            2011.10.04 m16_clear() extended to clear sub-machines
            2012.05.25 special treatment of single transaction added
            2012.06.13 special treatment of single transaction improved
            2012.06.19 item identifier map added (e.g. m16_setmap())
            2012.06.22 bug in function m16_clear() fixed (variable s)
            2012.06.25 explicit treatment of item 2 added to rec_pos()
            2012.06.27 explicit treatment of item 3 added to rec_pos()
            2013.11.12 item selection file changed to option -R#
            2014.08.27 adapted to modified item set reporter interface
            2014.10.24 changed from LGPL license to MIT license
------------------------------------------------------------------------
  Reference for the basic scheme of the 16 items machine:
    T. Uno, M. Kiyomi, and H. Arimura.
    LCM ver.3: Collaboration of Array, Bitmap and Prefix Tree
    for Frequent Itemset Mining.
    Proc. 1st Int. Workshop Open Source Data Mining (OSDM 2005).
    ACM Press, New York, NY, USA 2005
  However, LCM3 does not use a fixed number of bit-mapped items,
  but chooses their number in such a way that the memory needed to
  store the (reduced) transaction database and the k-items machine
  does not exceed the memory needed for the (unreduced) transactions.
----------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <float.h>
#include <math.h>
#include <time.h>
#include <assert.h>
#ifndef ISR_CLOMAX
#define ISR_CLOMAX
#endif
#ifdef M16_MAIN
#ifndef TA_READ
#define TA_READ
#endif
#endif
#include "fim16.h"
#ifdef M16_MAIN
#include "error.h"
#endif
#ifdef STORAGE
#include "storage.h"
#endif

/*----------------------------------------------------------------------
  Preprocessor Definitions
----------------------------------------------------------------------*/
#define PRGNAME     "fim16"
#define DESCRIPTION "find frequent item sets for at most 16 items"
#define VERSION     "version 2.6 (2016.03.01)         " \
                    "(c) 2011-2016   Christian Borgelt"

/* --- error codes --- */
/* error codes   0 to  -4 defined in tract.h */
#define E_STDIN      (-5)       /* double assignment of stdin */
#define E_OPTION     (-6)       /* unknown option */
#define E_OPTARG     (-7)       /* missing option argument */
#define E_ARGCNT     (-8)       /* too few/many arguments */
#define E_TARGET     (-9)       /* invalid target type */
#define E_SIZE      (-10)       /* invalid item set size */
#define E_SUPPORT   (-11)       /* invalid minimum item set support */
#define E_ITEMCNT   (-12)       /* invalid number of items */
#define E_MEASURE   (-13)       /* invalid evaluation measure */
#define E_REPEAT    (-14)       /* invalid number of repetitions */
/* error codes -15 to -25 defined in tract.h */

#ifndef QUIET                   /* if not quiet version, */
#define MSG         fprintf     /* print messages */
#define CLOCK(t)    ((t) = clock())
#else                           /* if quiet version, */
#define MSG(...)    ((void)0)   /* suppress messages */
#define CLOCK(t)    ((void)0)
#endif

#define SEC_SINCE(t)  ((double)(clock()-(t)) /(double)CLOCKS_PER_SEC)

/*----------------------------------------------------------------------
  Constants
----------------------------------------------------------------------*/
#if !defined QUIET && defined M16_MAIN
/* --- error messages --- */
static const char *errmsgs[] = {
  /* E_NONE      0 */  "no error",
  /* E_NOMEM    -1 */  "not enough memory",
  /* E_FOPEN    -2 */  "cannot open file %s",
  /* E_FREAD    -3 */  "read error on file %s",
  /* E_FWRITE   -4 */  "write error on file %s",
  /* E_STDIN    -5 */  "double assignment of standard input",
  /* E_OPTION   -6 */  "unknown option -%c",
  /* E_OPTARG   -7 */  "missing option argument",
  /* E_ARGCNT   -8 */  "wrong number of arguments",
  /* E_TARGET   -9 */  "invalid target type '%c'",
  /* E_SIZE    -10 */  "invalid item set size %d",
  /* E_SUPPORT -11 */  "invalid minimum support %g",
  /* E_ITEMCNT -12 */  "invalid number of items (must be <= 16)",
  /* E_MEASURE -13 */  "invalid evaluation measure '%c'",
  /* E_REPEAT  -14 */  "invalid number of repetitions %d",
  /* E_NOITEMS -15 */  "no (frequent) items found",
  /*           -16 */  "unknown error"
};
#endif

/*----------------------------------------------------------------------
  Global Variables
----------------------------------------------------------------------*/
#ifdef M16_MAIN
#ifndef QUIET
static CCHAR    *prgname;       /* program name for error messages */
#endif
static TABREAD  *tread  = NULL; /* table/transaction reader */
static ITEMBASE *ibase  = NULL; /* item base */
static TABAG    *tabag  = NULL; /* transaction bag/multiset */
static ISREPORT *report = NULL; /* item set reporter */
static FIM16    *fim16  = NULL; /* freq. item set mining object */
#endif

static unsigned char hibit[1 << 16] = { 1 };
/* map from bit representation of a transaction to highest set bit */

static const BITTA prjms[16] = {/* bit masks for trans. projection */
  0x0000, 0x0001, 0x0003, 0x0007,
  0x000f, 0x001f, 0x003f, 0x007f,
  0x00ff, 0x01ff, 0x03ff, 0x07ff,
  0x0fff, 0x1fff, 0x3fff, 0x7fff };

/*----------------------------------------------------------------------
  Auxiliary Functions (for debugging)
----------------------------------------------------------------------*/
#ifndef NDEBUG

static void indent (int k)
{ while (--k >= 0) printf("   "); }

/*--------------------------------------------------------------------*/

static void show (const char *text, FIM16 *fim, int ind)
{                               /* --- show a 16 items machine */
  ITEM  i;                      /* loop variable (item) */
  BITTA *t, *e;                 /* to traverse the transactions */

  assert(fim);                  /* check the function arguments */
  if (text && *text) {          /* print the given text */
    indent(ind); printf("%s\n", text); }
  for (i = 16; --i >= 0; )      /* find top item with support */
    if (fim->supps[i] > 0) break;
  for (++i; --i >= 0; ) {       /* traverse the items/trans. lists */
    t = fim->btas[i];           /* get the transaction list */
    if (!t) continue;           /* skip non-existing lists */
    indent(ind);                /* indent the output line */
    printf("%s/",  isr_itemname(fim->report, fim->map[i]));
    printf("%2"ITEM_FMT" ", fim->map[i]);   /* print item, list index */
    printf("%2"SUPP_FMT":", fim->supps[i]); /* and item support */
    for (e = fim->ends[i]; t < e; t++)
      printf(" %04x:%"SUPP_FMT, *t, fim->wgts[*t]);
    printf("\n");               /* print the transactions (as int) */
  }                             /* and terminate the output line */
}  /* show() */

#endif
/*----------------------------------------------------------------------
  16 Items Machine Functions
----------------------------------------------------------------------*/

FIM16* m16_create (int dir, SUPP supp, ISREPORT *report)
{                               /* --- create a 16 items machine */
  int   i, k, n, s;             /* loop variables, buffers */
  FIM16 *fim, *cur;             /* created 16 items machine */
  BITTA *p;                     /* to organize the memory */

  if (hibit[0] != 0)            /* init. highest bit map if necessary */
    for (k = i = 0; k < 16; k++)/* traverse all possible transactions */
      while (i < (1 << (k+1))) hibit[i++] = (unsigned char)k;
  n   = (dir > 0) ? 1 : 16;     /* get the number of sub-machines */
  fim = (FIM16*)calloc((size_t)n, sizeof(FIM16));
  if (!fim) return NULL;        /* allocate the base structure */
  fim->map = (ITEM*)malloc(16*sizeof(ITEM));
  if (!fim->map) { free(fim); return NULL; }
  m16_clrmap(fim);              /* create an item identifier map */
  for (cur = fim, s = 16; --n >= 0; cur++, s--) {
    cur->report = report;       /* traverse the 16 items sub-machines */
    cur->dir    = dir;          /* and initialize the basic fields */
    cur->smin   = supp;         /* (i.e., direction, minimum support */
    cur->ttw    = 0;            /* and the total transaction weight) */
    cur->wgts   = (SUPP*)calloc(((size_t)1) << s, sizeof(SUPP));
    if (!cur->wgts) { m16_delete(cur); return NULL; }
    k = (s > 10) ? 10 : s;      /* allocate the trans. weight array */
    p = (BITTA*)malloc((((size_t)1) << k) *sizeof(BITTA));
    if (!p) { m16_delete(fim); return NULL; }
    for (i = 0; i < k; i++) {   /* allocate one block for short lists */
      cur->ends[i] = cur->btas[i] = p; p += (i > 1) ? 1 << i : 2; }
    for ( ; i < s; i++) {       /* individual blocks for the rest */
      p = (BITTA*)malloc((((size_t)1) << i) *sizeof(BITTA));
      if (!p) { m16_delete(fim); return NULL; }
      cur->ends[i] = cur->btas[i] = p;
    } /* This allocation scheme looks certainly weird, but showed the */
  }   /* best performance in several experiments; reasons are unclear.*/
      /* The difference is bigger on a 32 bit system than on 64 bit.  */
  return fim;                   /* return created 16 items machine */
}  /* m16_create() */

/*--------------------------------------------------------------------*/

void m16_delete (FIM16 *fim)
{                               /* --- delete a 16 items machine */
  int   i, n, s;                /* loop variable, buffers */
  FIM16 *cur;                   /* to traverse the machines */

  assert(fim);                  /* check the function arguments */
  n = (fim->dir < 0) ? 16 : 1;  /* traverse the machines */
  for (cur = fim, s = 16; --n >= 0; cur++, s--) {
    for (i = s; --i >= 10; )    /* traverse the transaction arrays */
      if (cur->btas[i]) free(cur->btas[i]);
    if (cur->btas[0]) free(cur->btas[0]);
    free(cur->wgts);            /* delete the transaction arrays */
  }                             /* and the transaction weight array */
  free(fim->map);               /* delete the item identifier map */
  free(fim);                    /* delete the base structure */
}  /* m16_delete() */

/*--------------------------------------------------------------------*/

void m16_clrmap (FIM16 *fim)
{                               /* --- clear item identifier map */
  fim->map[ 0] =  0; fim->map[ 1] =  1; fim->map[ 2] =  2;
  fim->map[ 3] =  3; fim->map[ 4] =  4; fim->map[ 5] =  5;
  fim->map[ 6] =  6; fim->map[ 7] =  7; fim->map[ 8] =  8;
  fim->map[ 9] =  9; fim->map[10] = 10; fim->map[11] = 11;
  fim->map[12] = 12; fim->map[13] = 13; fim->map[14] = 14;
  fim->map[15] = 15;            /* set item map to identity */
}  /* m16_clrmap() */

/*--------------------------------------------------------------------*/

void m16_add (FIM16 *fim, BITTA tract, SUPP wgt)
{                               /* --- add a transaction in bit rep. */
  assert(fim);                  /* check the function arguments */
  fim->ttw += wgt;              /* sum the transaction weight */
  if (tract == 0) return;       /* ignore empty transactions */
  fim->tor |= tract;            /* combine for single trans. check */
  if ((fim->wgts[tract] += wgt) <= wgt)  /* update trans. weight and */
    *fim->ends[hibit[tract]]++ = tract;  /* add it to its hibit list */
}  /* m16_add() */

/*--------------------------------------------------------------------*/

void m16_addx (FIM16 *fim, const ITEM *items, ITEM n, SUPP wgt)
{                               /* --- add a transaction */
  BITTA tract;                  /* bit representation of transaction */

  assert(fim && (items || (n <= 0))); /* check function arguments */
  for (tract = 0; --n >= 0; items++) {
    if      (*items <  0) tract = (BITTA)(tract | (BITTA)*items);
    else if (*items < 16) tract = (BITTA)(tract | (BITTA)(1 << *items));
  }                             /* set the corresponding bits */
  m16_add(fim, tract, wgt);     /* add the constructed bit rep. */
}  /* m16_addx() */

/*--------------------------------------------------------------------*/

void m16_adds (FIM16 *fim, const ITEM *items, SUPP wgt)
{                               /* --- add a transaction */
  BITTA tract;                  /* bit representation of transaction */

  assert(fim && items);         /* check the function arguments */
  for (tract = 0; *items > TA_END; items++) {
    if      (*items <  0) tract = (BITTA)(tract | (BITTA)*items);
    else if (*items < 16) tract = (BITTA)(tract | (BITTA)(1 << *items));
  }                             /* set the corresponding bits */
  m16_add(fim, tract, wgt);     /* add the constructed bit rep. */
}  /* m16_adds() */

/*--------------------------------------------------------------------*/

void m16_addtbg (FIM16 *fim, TABAG *tabag)
{                               /* --- add a transaction bag */
  TID   i, n;                   /* loop variables */
  TRACT *t;                     /* to traverse the transactions */

  assert(fim && tabag);         /* check the function arguments */
  for (n = tbg_cnt(tabag), i = 0; i < n; i++) {
    t = tbg_tract(tabag, i);    /* traverse the transactions */
    m16_adds(fim, ta_items(t), ta_wgt(t));
  }                             /* add each trans. to FIM object */
}  /* m16_addtbg() */

/*--------------------------------------------------------------------*/

static void project (FIM16 *fim, ITEM item, BITTA mask, FIM16 *dst)
{                               /* --- project trans. to an item */
  BITTA *t, *e;                 /* to traverse the transactions */
  BITTA p;                      /* projected trans. (item removed) */
  SUPP  *w;                     /* (location of) transaction weight */

  assert(fim && dst && (item > 0)); /* check the function arguments */
  dst->map = fim->map;          /* copy the item identifier map */
  mask &= prjms[item];          /* remove the item from the mask */
  fim->supps[item] = 0;         /* clear the item support (reinit.) */
  e = fim->ends[item];          /* get and clear the trans. range */
  fim->ends[item] = t = fim->btas[item];
  for ( ; t < e; t++) {         /* traverse the item's transactions */
    w = fim->wgts +*t;          /* note the trans. weight location */
    p = *t & mask;              /* project transaction (remove item) */
    if (p) {                    /* if the projection is not empty */
      if ((dst->wgts[p] += *w) <= *w)     /* add the projection */
        *dst->ends[hibit[p]]++ = p;       /* to the transaction list */
    }                           /* of its highest bit (if necessary) */
    *w = 0;                     /* update the projection support and */
  }                             /* clear support of original trans. */
}  /* project() */

/* A separate function without a destination (projecting a machine   */
/* to itself, for rec_pos()) does not improve the processing speed.  */
/* Note that the check whether p is empty can be avoided if one sets */
/* dst->wgts[0] = 1, which prevents p = 0 being added for item 0.    */
/* However, this rather seems to slow down processing somewhat.      */

/*--------------------------------------------------------------------*/

static void count (FIM16 *fim, int n)
{                               /* --- count support of all items */
  BITTA *t, *e;                 /* to traverse the transactions */
  BITTA p;                      /* projected trans. (item removed) */
  SUPP  w, s;                   /* trans. weight and item support */

  assert(fim && (n > 0));       /* check the function arguments */
  while (--n > 1) {             /* traverse the (lower) items */
    e = fim->ends[n]; t = fim->btas[n];
    for (s = 0; t < e; t++) {   /* traverse the item's transactions */
      s += w = fim->wgts[*t];   /* note and sum the trans. weight */
      p = *t & prjms[n];        /* project transaction (remove item) */
      if (!p) continue;         /* if projection is empty, skip it */
      if ((fim->wgts[p] += w) <= w)  /* update projection support */
        *fim->ends[hibit[p]]++ = p;  /* and add the projection to */
    }                                /* the list of its highest bit */
    fim->supps[n] = s;          /* store the computed item support */
  }                             /* finally count for items 0 and 1 */
  fim->supps[1] = fim->wgts[2] +fim->wgts[3];
  fim->supps[0] = fim->wgts[1] +fim->wgts[3];
}  /* count() */

/*--------------------------------------------------------------------*/

static void clear (FIM16 *fim, int item)
{                               /* --- clear a transaction list */
  BITTA *t, *e;                 /* to traverse the transactions */

  assert(fim && (item >= 0));   /* check the function arguments */
  fim->supps[item] = 0;         /* clear the item support (reinit.) */
  e = fim->ends[item];          /* get and clear the trans. range */
  fim->ends[item] = t = fim->btas[item];
  for ( ; t < e; t++) fim->wgts[*t] = 0;
}  /* clear() */                /* clear all transaction weights */

/*--------------------------------------------------------------------*/

static BITTA filter (FIM16 *fim, int n, SUPP pex)
{                               /* --- filter items from projection */
  BITTA mask;                   /* item mask for needed items */

  assert(fim && (pex > 0));     /* check the function arguments */
  for (mask = 0; --n >= 0; ) {  /* traverse the items */
    if      (fim->supps[n] <  fim->smin)
      clear(fim, n);            /* eliminate infrequent items */
    else if (fim->supps[n] >= pex) {
      clear(fim, n);            /* collect perfect extensions */
      isr_addpex(fim->report, fim->map[n]); }
    else mask = (BITTA)(mask | (1 << n));
  }                             /* if item is frequent, set bit mask */
  return mask;                  /* return the item mask */
}  /* filter() */

/*--------------------------------------------------------------------*/

static int rec_pos (FIM16 *fim, int n, BITTA mask)
{                               /* --- find frequent item sets */
  int   i, r;                   /* loop variable, error status */
  SUPP  s;                      /* item support */
  BITTA m;                      /* filtering mask */

  assert(fim && (n >= 0));      /* check the function arguments */
  /* The item mask needs to be checked, because items 0,1,2 may have  */
  /* been identified as perfect extensions in higher recursion levels.*/
  if ((mask & 1)                /* if item 0 is frequent */
  &&  (fim->supps[0] >= fim->smin)) {
    r = isr_add(fim->report, fim->map[0], fim->supps[0]);
    if (r < 0) return -1;       /* add item 0 to the reporter */
    if (r > 0) {                /* if item 0 needs processing */
      if (isr_report(fim->report) < 0)
        return -1;              /* report the current item set */
      isr_remove(fim->report,1);/* remove the item 0 again */
    }                           /* from the item set reporter */
  }
  if ((mask & 2)                /* if item 1 is frequent */
  &&  (fim->supps[1] >= fim->smin)) {
    r = isr_add(fim->report, fim->map[1], fim->supps[1]);
    if (r < 0) return -1;       /* add item 1 to the reporter */
    if (r > 0) {                /* if item 1 needs processing, */
      if ((mask & 1)            /* if item 0 is frequent given 1 */
      &&  (fim->wgts[3] >= fim->smin)) {
        if (fim->wgts[3] >= fim->supps[1])
          isr_addpex(fim->report, fim->map[0]);
        else {                  /* check for a perfect extension */
          r = isr_add(fim->report, fim->map[0], fim->wgts[3]);
          if (r < 0) return -1; /* add item 0 to the reporter */
          if (r > 0) {          /* if item 0 needs processing */
            if (isr_report(fim->report) < 0)
              return -1;        /* report the current item set */
            isr_remove(fim->report, 1);
          }                     /* remove the item 0 again */
        }                       /* from the item set reporter */
      }
      if (isr_report(fim->report) < 0)
        return -1;              /* report the current item set */
      isr_remove(fim->report,1);/* remove the item 1 again */
    }                           /* from the item set reporter */
  }
  if (n <= 2) {                 /* if only two items to process */
    memset(fim->wgts+1, 0, 3*sizeof(SUPP));
    memset(fim->supps,  0, 2*sizeof(SUPP));
    memcpy(fim->ends, fim->btas, 2*sizeof(BITTA*));
    return 0;                   /* clear counters for items 0 and 1 */
  }                             /* and clear the transaction lists */
  if ((mask & 4)                /* if item 2 is frequent */
  &&  (fim->supps[2] >= fim->smin)) {
    r = isr_add(fim->report, fim->map[2], fim->supps[2]);
    if (r < 0) return -1;       /* add item 2 to the reporter */
    if (r > 0) {                /* if  item 2 needs processing, */
      m = 0;                    /* project/count for items 0 and 1 */
      fim->wgts[3]  = fim->wgts[7];
      fim->wgts[2]  = fim->wgts[6];
      fim->wgts[1]  = fim->wgts[5];
      fim->supps[0] = s = fim->wgts[3] +fim->wgts[1];
      if      (s >= fim->supps[2]) isr_addpex(fim->report, fim->map[0]);
      else if (s >= fim->smin)     m = (BITTA)(m | (mask & 1));
      fim->supps[1] = s = fim->wgts[3] +fim->wgts[2];
      if      (s >= fim->supps[2]) isr_addpex(fim->report, fim->map[1]);
      else if (s >= fim->smin)     m = (BITTA)(m | (mask & 2));
      r = rec_pos(fim, 2, m);   /* perfect extension items and then */
      if (r < 0) return -1;     /* find freq. item sets recursively */
      if (isr_report(fim->report) < 0)
        return -1;              /* report the current item set */
      isr_remove(fim->report,1);/* remove the item 2 again */
    }                           /* from the item set reporter */
  }
  if (n <= 3) {                 /* if only two items to process */
    memset(fim->wgts+1, 0, 7*sizeof(SUPP));  /* clear weights, */
    memset(fim->supps,  0, 3*sizeof(SUPP));  /* support and lists */
    memcpy(fim->ends, fim->btas, 3*sizeof(BITTA*));
    return 0;                   /* clear counters for items 0, 1, 2 */
  }                             /* and clear the transaction lists */
  if ((mask & 8)                /* if item 3 is frequent */
  &&  (fim->supps[3] >= fim->smin)) {
    r = isr_add(fim->report, fim->map[3], fim->supps[3]);
    if (r < 0) return -1;       /* add item 3 to the reporter */
    if (r > 0) {                /* if  item 3 needs processing, */
      m = 0;                    /* project/count for items 0 and 1 */
      fim->wgts[4]  = fim->wgts[12];
      fim->wgts[3]  = fim->wgts[11] +(fim->wgts[7] = fim->wgts[15]);
      fim->wgts[2]  = fim->wgts[10] +(fim->wgts[6] = fim->wgts[14]);
      fim->wgts[1]  = fim->wgts[ 9] +(fim->wgts[5] = fim->wgts[13]);
      fim->supps[0] = s = fim->wgts[3] +fim->wgts[1];
      if      (s >= fim->supps[3]) isr_addpex(fim->report, fim->map[0]);
      else if (s >= fim->smin)     m = (BITTA)(m | (mask & 1));
      fim->supps[1] = s = fim->wgts[3] +fim->wgts[2];
      if      (s >= fim->supps[3]) isr_addpex(fim->report, fim->map[1]);
      else if (s >= fim->smin)     m = (BITTA)(m | (mask & 2));
      fim->supps[2] = s = fim->wgts[7] +fim->wgts[6]
                        + fim->wgts[5] +fim->wgts[4];
      if      (s >= fim->supps[3]) isr_addpex(fim->report, fim->map[2]);
      else if (s >= fim->smin)     m = (BITTA)(m | (mask & 4));
      r = rec_pos(fim, 3, m);   /* perfect extension items and then */
      if (r < 0) return -1;     /* find freq. item sets recursively */
      if (isr_report(fim->report) < 0)
        return -1;              /* report the current item set */
      isr_remove(fim->report,1);/* remove the item 3 again */
    }                           /* from the item set reporter */
  }
  memset(fim->wgts+1, 0, 15*sizeof(SUPP));  /* clear weights, */
  memset(fim->supps,  0,  4*sizeof(SUPP));  /* support and lists */
  memcpy(fim->ends, fim->btas, 4*sizeof(BITTA*));

  for (i = 4; i < n; i++) {     /* traverse the (remaining) items */
    s = fim->supps[i];          /* get the support of the item and */
    if (s < fim->smin) continue;/* skip infrequent/eliminated items */
    r = isr_add(fim->report, fim->map[i], s);
    if (r <  0) return -1;      /* add item i to the reporter */
    if (r <= 0) {               /* check if item i needs processing, */
      clear(fim, i); continue;} /* otherwise delete its transactions */
    assert(mask & (1 << i));    /* item must not be excluded */
    if (fim->ends[i]-fim->btas[i] <= 1){ /* if only one transaction, */
      fim->ends[i] = fim->btas[i]; /* add all items as perfect exts. */
      fim->wgts[m = *fim->btas[i]] = 0;
      for (r = 0; (unsigned int)(1 << r) <= (unsigned int)m; r++)
        if (m & (1 << r)) isr_addpex(fim->report, fim->map[r]); }
    else {                      /* if more than one transaction */
      project(fim,i,mask, fim); /* project database to item i */
      count(fim, i);            /* count the support of all items */
      m = filter (fim, i, s);   /* remove infrequent items and */
      r = rec_pos(fim, i, m);   /* perfect extension items and then */
      if (r < 0) return -1;     /* find freq. item sets recursively */
    }                           /* and check for a recursion error */
    if (isr_report(fim->report) < 0)
      return -1;                /* report the current item set */
    isr_remove(fim->report, 1); /* remove the item i again */
  }                             /* from the item set reporter */
  return 0;                     /* return 'ok' */
}  /* rec_pos() */

/*--------------------------------------------------------------------*/

static int rec_neg (FIM16 *fim, int n, BITTA mask)
{                               /* --- find frequent item sets */
  int   i, r;                   /* loop variable, error status */
  SUPP  s;                      /* item support */
  BITTA m;                      /* filtering mask */

  assert(fim && (n >= 0));      /* check the function arguments */
  for (i = n; --i >= 2; ) {     /* traverse the remaining items */
    s = fim->supps[i];          /* get the support of the item and */
    if (s < fim->smin) continue;/* skip infrequent/eliminated items */
    r = isr_add(fim->report, fim->map[i], s);
    if (r <  0) return -1;      /* add item i to the reporter */
    if (r <= 0) {               /* check if item i needs processing, */
      clear(fim, i); continue;} /* otherwise delete its transactions */
    assert(mask & (1 << i));    /* item must not be excluded */
    if (fim->ends[i]-fim->btas[i] <= 1){ /* if only one transaction, */
      fim->ends[i] = fim->btas[i]; /* add all items as perfect exts. */
      fim->wgts[m = *fim->btas[i]] = 0;
      for (r = 0; (unsigned int)(1 << r) <= (unsigned int)m; r++)
        if (m & (1 << r)) isr_addpex(fim->report, fim->map[r]); }
    else {                      /* if more than one transaction */
      project(fim,i,mask,fim+1);/* project database to item i */
      count(fim+1, i);          /* count th support of all items */
      m = filter (fim+1, i, s); /* remove infrequent items and */
      r = rec_neg(fim+1, i, m); /* prefect extension items and then */
      if (r < 0) return -1;     /* find freq. item sets recursively */
    }                           /* and check for a recursion error */
    if (isr_report(fim->report) < 0)
      return -1;                /* report the current item set */
    isr_remove(fim->report, 1); /* remove the item i again */
  }                             /* from the item set reporter */

  if ((mask & 2)                /* if item 1 is frequent */
  &&  (fim->supps[1] >= fim->smin)) {
    r = isr_add(fim->report, fim->map[1], fim->supps[1]);
    if (r < 0) return -1;       /* add item 1 to the reporter */
    if (r > 0) {                /* if item 1 needs processing, */
      if ((mask & 1)            /* if item 0 is frequent given 1 */
      &&  (fim->wgts[3] >= fim->smin)) {
        if (fim->wgts[3] >= fim->supps[1])
          isr_addpex(fim->report, fim->map[0]);
        else {                  /* check for a perfect extension */
          r = isr_add(fim->report, fim->map[0], fim->wgts[3]);
          if (r < 0) return -1; /* add item 0 to the reporter */
          if (r > 0) {          /* if item 0 needs processing */
            if (isr_report(fim->report) < 0)
              return -1;        /* report the current item set */
            isr_remove(fim->report, 1);
          }                     /* remove the item 0 again */
        }                       /* from the item set reporter */
      }
      if (isr_report(fim->report) < 0)
        return -1;              /* report the current item set */
      isr_remove(fim->report,1);/* remove the item 1 again */
    }                           /* from the item set reporter */
  }
  if ((mask & 1)                /* if item 0 is frequent */
  &&  (fim->supps[0] >= fim->smin)) {
    r = isr_add(fim->report, fim->map[0], fim->supps[0]);
    if (r < 0) return -1;       /* add item 0 to the reporter */
    if (r > 0) {                /* if item 0 needs processing */
      if (isr_report(fim->report) < 0)
        return -1;              /* report the current item set */
      isr_remove(fim->report,1);/* remove the item 0 again */
    }                           /* from the item set reporter */
  }
  memset(fim->wgts+1, 0, 3*sizeof(SUPP));  /* clear weights, */
  memset(fim->supps,  0, 2*sizeof(SUPP));  /* support and lists */
  memcpy(fim->ends, fim->btas, 2*sizeof(BITTA*));
  return 0;                     /* return 'ok' */
}  /* rec_neg() */

/*--------------------------------------------------------------------*/

void m16_clear (FIM16 *fim)
{                               /* --- clear 16 items machine */
  int i, n, s;                  /* loop variables */

  assert(fim);                  /* check the function argument */
  n = (fim->dir > 0) ? 1 : 16;  /* get the number of sub-machines */
  for (s = 16; --n >= 0; fim++, s--) {
    if (fim->ttw <= 0) continue;/* skip empty sub-machines */
    fim->ttw = 0; fim->tor = 0; /* clear the total trans. weight */
    for (i = 0; i < s; i++)     /* traverse the 16 items and */
      clear(fim, i);            /* clear their transaction lists */
  }
}  /* m16_clear() */

/*--------------------------------------------------------------------*/

int m16_mine (FIM16 *fim)
{                               /* --- find frequent item sets */
  int   r, i;                   /* error status, buffers */
  SUPP  s;                      /* support of current prefix */
  BITTA m;                      /* mask after item filtering, bit */

  assert(fim);                  /* check the function argument */
  if (fim->ttw <= 0) return 0;  /* if the machine is empty, abort */
  s = isr_supp(fim->report);    /* get min. support for perfect exts. */
  m = fim->tor;                 /* and bitwise or of all transactions */
  if (fim->wgts[m] >= s) {      /* if there is only one transaction, */
    fim->wgts[m] = 0;           /* remove it from the 16 items mach. */
    for (i = 0; (unsigned int)(1 << i) <= (unsigned int)m; i++)
      if (m & (1 << i))         /* all items are perfect extensions */
        isr_addpex(fim->report, fim->map[i]);
    i = hibit[m]; fim->ends[i] = fim->btas[i];
    fim->ttw = 0; fim->tor = 0; /* clear 16 items machine */
    return 0;                   /* and abort the function */
  }
  count(fim, i = hibit[m]+1);   /* count the support of all items */
  m = filter(fim, i, s);        /* remove infreq. and perf. exts. */
  r = (fim->dir > 0)            /* find freq. item sets recursively */
    ? rec_pos(fim, i, m) : rec_neg(fim, i, m);
  fim->ttw = 0; fim->tor = 0;   /* clear the total transaction weight */
  return (r) ? r : m;           /* return error status or mask */
}  /* m16_mine() */

/*--------------------------------------------------------------------*/
#ifndef NDEBUG

void m16_show (FIM16 *fim)
{                               /* --- show 16 items machine */
  int   i;                      /* loop variable, item */
  BITTA *t, *e;                 /* to traverse the transactions */
  SUPP  w, s;                   /* trans. weight and item support */

  assert(fim);                  /* check the function arguments */
  for (i = 0; i < 16; i++) {    /* traverse the items */
    printf("%2"ITEM_FMT"/", fim->map[i]); /* print item id and name */
    printf("%s:", isr_itemname(fim->report, fim->map[i]));
    e = fim->ends[i]; t = fim->btas[i];
    for (s = 0; t < e; t++) {   /* traverse the item's transactions */
      s += w = fim->wgts[*t];   /* get and sum the transaction weight */
      printf(" %04x:%"SUPP_FMT, *t, w);
    }                           /* print transaction and weight */
    printf(" (%"SUPP_FMT")\n", s);
  }                             /* print the item support */
  printf("total: %"SUPP_FMT"\n", fim->ttw);
}  /* m16_show() */

#endif
/*----------------------------------------------------------------------
  Main Functions
----------------------------------------------------------------------*/
#ifdef M16_MAIN

static void help (void)
{                               /* --- print add. option information */
  #ifndef QUIET
  fprintf(stderr, "\n");        /* terminate startup message */
  printf("additional evaluation measures (option -e#)\n");
  printf("  x   no measure (default)\n");
  printf("  b   binary logarithm of support quotient\n");
  #endif                        /* print help information */
  exit(0);                      /* abort the program */
}  /* help() */

/*--------------------------------------------------------------------*/

#ifndef NDEBUG                  /* if debug version */
  #undef  CLEANUP               /* clean up memory and close files */
  #define CLEANUP \
  if (fim16)  m16_delete(fim16);     \
  if (report) isr_delete(report, 0); \
  if (tabag)  tbg_delete(tabag,  0); \
  if (tread)  trd_delete(tread,  1); \
  if (ibase)  ib_delete (ibase);
#endif

GENERROR(error, exit)           /* generic error reporting function */

/*--------------------------------------------------------------------*/

int main (int argc, char *argv[])
{                               /* --- main function */
  int     i, k = 0;             /* loop variables */
  char    *s;                   /* to traverse the options */
  CCHAR   **optarg = NULL;      /* option argument */
  CCHAR   *fn_inp  = NULL;      /* name of input  file */
  CCHAR   *fn_out  = NULL;      /* name of output file */
  CCHAR   *fn_sel  = NULL;      /* name of item selection file */
  CCHAR   *recseps = NULL;      /* record  separators */
  CCHAR   *fldseps = NULL;      /* field   separators */
  CCHAR   *blanks  = NULL;      /* blank   characters */
  CCHAR   *comment = NULL;      /* comment characters */
  CCHAR   *hdr     = "";        /* record header  for output */
  CCHAR   *sep     = " ";       /* item separator for output */
  CCHAR   *dflt    = " (%1S)";  /* default format for check */
  CCHAR   *info    = dflt;      /* format for information output */
  int     target   = 's';       /* target type (closed/maximal) */
  ITEM    zmin     =  1;        /* minimum size of an item set */
  ITEM    zmax     = 16;        /* maximum size of an item set */
  double  supp     = 10;        /* minimum support of an item set */
  SUPP    smin     = 1;         /* minimum support of an item set */
  int     eval     = 'x';       /* additional evaluation measure */
  double  thresh   = 10;        /* evaluation measure threshold */
  int     sort     =  2;        /* flag for item sorting and recoding */
  int     dir      = +1;        /* item processing order */
  long    repeat   =  1;        /* number of repetitions */
  int     mtar     =  0;        /* mode for transaction reading */
  int     scan     =  0;        /* flag for scanable item output */
  int     stats    =  0;        /* flag for item set statistics */
  ITEM    m;                    /* number of items */
  TID     n;                    /* number of transactions */
  SUPP    w;                    /* total transaction weight */
  #ifndef QUIET                 /* if not quiet version */
  clock_t t;                    /* timer for measurements */

  prgname = argv[0];            /* get program name for error msgs. */

  /* --- print usage message --- */
  if (argc > 1) {               /* if arguments are given */
    fprintf(stderr, "%s - %s\n", argv[0], DESCRIPTION);
    fprintf(stderr, VERSION); } /* print a startup message */
  else {                        /* if no arguments given */
    printf("usage: %s [options] infile [outfile]\n", argv[0]);
    printf("%s\n", DESCRIPTION);
    printf("%s\n", VERSION);
    printf("-t#      target type                              "
                    "(default: %c)\n", target);
    printf("         (s: frequent, c: closed, m: maximal item sets, "
                     "g: generators)\n");
    printf("-m#      minimum number of items per item set     "
                    "(default: %"ITEM_FMT")\n", zmin);
    printf("-n#      maximum number of items per item set     "
                    "(default: %"ITEM_FMT")\n", zmax);
    printf("-s#      minimum support of an item set           "
                    "(default: %g%%)\n", supp);
    printf("         (positive: percentage, "
                     "negative: absolute number)\n");
    printf("-e#      additional evaluation measure            "
                    "(default: none)\n");
    printf("-d#      threshold for add. evaluation measure    "
                    "(default: %g%%)\n", thresh);
    printf("-q#      sort items w.r.t. their frequency        "
                    "(default: %d)\n", sort);
    printf("         (1: ascending, -1: descending, 0: do not sort,\n"
           "          2: ascending, -2: descending w.r.t. "
                    "transaction size sum)\n");
    printf("-u#      item processing order/search direction   "
                    "(default: %d)\n", dir);
    printf("         (fixed to -1 for closed/maximal item sets\n"
           "          fixed to +1 for generators, free otherwise)\n");
    printf("-x#      number of repetitions (for benchmarking) "
                    "(default: 1)\n");
    printf("-R#      read an item selection from a file\n");
    printf("-Z       print item set statistics (counts per size)\n");
    printf("-g       write output in scanable form "
                    "(quote certain characters)\n");
    printf("-h#      record header  for output                "
                    "(default: \"%s\")\n", hdr);
    printf("-k#      item separator for output                "
                    "(default: \"%s\")\n", sep);
    printf("-v#      output format for item set information   "
                    "(default: \"%s\")\n", info);
    printf("-w       transaction weight in last field         "
                    "(default: only items)\n");
    printf("-r#      record/transaction separators            "
                    "(default: \"\\n\")\n");
    printf("-f#      field /item        separators            "
                    "(default: \" \\t,\")\n");
    printf("-b#      blank   characters                       "
                    "(default: \" \\t\\r\")\n");
    printf("-C#      comment characters                       "
                    "(default: \"#\")\n");
    printf("-!       print additional option information\n");
    printf("infile   file to read transactions from           "
                    "[required]\n");
    printf("outfile  file to write frequent item sets to      "
                    "[optional]\n");
    return 0;                   /* print a usage message */
  }                             /* and abort the program */
  #endif  /* #ifndef QUIET */
  /* free option characters: acilopy [A-Z]\[CT] */

  /* --- evaluate arguments --- */
  for (i = 1; i < argc; i++) {  /* traverse arguments */
    s = argv[i];                /* get option argument */
    if (optarg) { *optarg = s; optarg = NULL; continue; }
    if ((*s == '-') && *++s) {  /* -- if argument is an option */
      while (*s) {              /* traverse options */
        switch (*s++) {         /* evaluate switches */
          case '!': help();                          break;
          case 't': target = (*s) ? *s++ : 's';      break;
          case 'm': zmin   = (ITEM)strtol(s, &s, 0); break;
          case 'n': zmax   = (ITEM)strtol(s, &s, 0); break;
          case 's': supp   =       strtod(s, &s);    break;
          case 'e': eval   = (*s) ? *s++ : 0;        break;
          case 'd': thresh =       strtod(s, &s);    break;
          case 'q': sort   = (int) strtol(s, &s, 0); break;
          case 'u': dir    = (int) strtol(s, &s, 0); break;
          case 'x': repeat =       strtol(s, &s, 0); break;
          case 'R': optarg = &fn_sel;                break;
          case 'Z': stats  = 1;                      break;
          case 'g': scan   = 1;                      break;
          case 'h': optarg = &hdr;                   break;
          case 'k': optarg = &sep;                   break;
          case 'v': optarg = &info;                  break;
          case 'w': mtar  |= TA_WEIGHT;              break;
          case 'r': optarg = &recseps;               break;
          case 'f': optarg = &fldseps;               break;
          case 'b': optarg = &blanks;                break;
          case 'C': optarg = &comment;               break;
          default : error(E_OPTION, *--s);           break;
        }                       /* set option variables */
        if (optarg && *s) { *optarg = s; optarg = NULL; break; }
      } }                       /* get option argument */
    else {                      /* -- if argument is no option */
      switch (k++) {            /* evaluate non-options */
        case  0: fn_inp = s;      break;
        case  1: fn_out = s;      break;
        default: error(E_ARGCNT); break;
      }                         /* note filenames */
    }
  }
  if (optarg)     error(E_OPTARG);      /* check (option) arguments */
  if (k    < 1)   error(E_ARGCNT);      /* and number of arguments */
  if (zmin < 0)   error(E_SIZE, zmin);  /* check the size limits */
  if (zmax < 0)   error(E_SIZE, zmax);  /* and the minimum support */
  if (zmax > 16)  error(E_SIZE, zmax);  /* and the minimum support */
  if (supp > 100) error(E_SUPPORT, supp);
  if (repeat < 1) error(E_REPEAT, repeat);
  if ((!fn_inp || !*fn_inp) && (fn_sel && !*fn_sel))
    error(E_STDIN);             /* stdin must not be used twice */
  switch (target) {             /* check and translate target type */
    case 's': target = ISR_ALL;              break;
    case 'c': target = ISR_CLOSED;           break;
    case 'm': target = ISR_MAXIMAL;          break;
    case 'g': target = ISR_GENERAS;          break;
    default : error(E_TARGET, (char)target); break;
  }                             /* (get target type code) */
  if ((eval != 'x') && (eval != 'b'))
    error(E_MEASURE,(char)eval);/* check evaluation measure code */
  if (info == dflt)             /* adapt the default info. format */
    info = (supp < 0) ? " (%a)" : " (%S)";
  MSG(stderr, "\n");            /* terminate the startup message */

  /* --- read item selection --- */
  ibase = ib_create(0, 0);      /* create an item base */
  if (!ibase) error(E_NOMEM);   /* to manage the items */
  tread = trd_create();         /* create a transaction reader */
  if (!tread) error(E_NOMEM);   /* and configure the characters */
  trd_allchs(tread, recseps, fldseps, blanks, "", comment);
  if (fn_sel) {                 /* if item appearances are given */
    CLOCK(t);                   /* start timer, open input file */
    if (trd_open(tread, NULL, fn_sel) != 0)
      error(E_FOPEN, trd_name(tread));
    MSG(stderr, "reading %s ... ", trd_name(tread));
    m = ib_readsel(ibase,tread);/* read the given item selection */
    if (m < 0) error((int)-m, ib_errmsg(ibase, NULL, 0));
    trd_close(tread);           /* close the input file */
    MSG(stderr, "[%"ITEM_FMT" item(s)]", m);
    MSG(stderr, " done [%.2fs].\n", SEC_SINCE(t));
  }                             /* print a log message */

  /* --- read transaction database --- */
  tabag = tbg_create(ibase);    /* create a transaction bag */
  if (!tabag) error(E_NOMEM);   /* to store the transactions */
  CLOCK(t);                     /* start timer, open input file */
  if (trd_open(tread, NULL, fn_inp) != 0)
    error(E_FOPEN, trd_name(tread));
  MSG(stderr, "reading %s ... ", trd_name(tread));
  k = tbg_read(tabag, tread, mtar);
  if (k < 0) error(-k, tbg_errmsg(tabag, NULL, 0));
  trd_delete(tread, 1);         /* read the transaction database, */
  tread = NULL;                 /* then delete the table reader */
  m = ib_cnt(ibase);            /* get the number of items, */
  n = tbg_cnt(tabag);           /* the number of transactions, */
  w = tbg_wgt(tabag);           /* the total transaction weight */
  MSG(stderr, "[%"ITEM_FMT" item(s), %"TID_FMT, m, n);
  if (w != (SUPP)n) { MSG(stderr, "/%"SUPP_FMT, w); }
  MSG(stderr, " transaction(s)] done [%.2fs].", SEC_SINCE(t));
  if ((m <= 0) || (n <= 0))     /* check for at least one item */
    error(E_NOITEMS);           /* and at least one transaction */
  MSG(stderr, "\n");            /* terminate the log message */
  supp = (supp >= 0) ? supp/100.0 *(double)w *(1-DBL_EPSILON) : -supp;
  smin = (SUPP)ceilsupp(supp);  /* compute absolute support value */

  /* --- sort and recode items --- */
  CLOCK(t);                     /* start timer, print log message */
  MSG(stderr, "filtering, sorting and recoding items ... ");
  m = tbg_recode(tabag, smin, -1, 16, -sort);
  if (m <  0) error(E_NOMEM);   /* recode items and transactions */
  if (m <= 0) error(E_NOITEMS); /* and check the number of items */
  MSG(stderr, "[%"ITEM_FMT" item(s)]", m);
  MSG(stderr, " done [%.2fs].\n", SEC_SINCE(t));

  /* --- sort and reduce transactions --- */
  CLOCK(t);                     /* start timer, print log message */
  MSG(stderr, "sorting and reducing transactions ... ");
  tbg_filter(tabag, zmin, NULL, 0);  /* remove short transactions */
  tbg_itsort(tabag, +1, 1);     /* sort items in transactions and */
  tbg_sort  (tabag, +1, 1);     /* sort the trans. lexicographically */
  n = tbg_reduce(tabag, 0);     /* reduce transactions to unique ones */
  tbg_pack(tabag, 16);          /* pack items with codes < 16 */
  MSG(stderr, "[%"TID_FMT, n);  /* print number of transactions */
  if (w != (SUPP)n) { MSG(stderr, "/%"SUPP_FMT, w); }
  MSG(stderr, " transaction(s)] done [%.2fs].\n", SEC_SINCE(t));

  /* --- create an item set reporter --- */
  report = isr_create(ibase);   /* create an item set reporter */
  if (!report) error(E_NOMEM);  /* and configure it */
  if (isr_settarg(report, target, 0, -1) != 0)
    error(E_NOMEM);             /* set target type and operation mode */
  isr_setsize(report,        zmin, (zmax < 16) ? zmax : ITEM_MAX);
  isr_setsupp(report, (RSUPP)smin, RSUPP_MAX);
  if (eval == 'b')              /* set the evaluation function */
    isr_seteval(report, isr_logrto, NULL, +1, thresh/100.0);
  if (isr_setfmt(report, scan, hdr, sep, NULL, info) != 0)
    error(E_NOMEM);             /* set the output format strings */
  if (isr_setup(report) != 0)   /* set up the item set reporter */
    error(E_NOMEM);             /* (prepare reporter for output) */
  k = isr_open(report, NULL, fn_out);
  if (k) error(k, isr_name(report)); /* open the output file */

  /* --- find frequent item sets --- */
  CLOCK(t);                     /* start timer, print log message */
  MSG(stderr, "writing %s ... ", isr_name(report));
  if      (target == ISR_GENERAS)             dir = +1;
  else if (target & (ISR_CLOSED|ISR_MAXIMAL)) dir = -1;
  fim16 = m16_create(dir, smin, report);
  if (!fim16) error(E_NOMEM);   /* create a 16 items machine */
  for (i = 0; i < repeat; i++){ /* repeated mining loop */
    isr_reset(report);          /* (re)init. the output counters */
    m16_addtbg(fim16, tabag);   /* add trans. bag to 16 items machine */
    k = m16_mine(fim16);        /* find frequent item sets */
    if (k < 0) error(E_NOMEM);  /* with 16 items machine */
  }
  if (isr_report(report) < 0)   /* report the empty set (if needed) */
    error(E_NOMEM);
  MSG(stderr, "[%"SIZE_FMT" set(s)]", isr_repcnt(report));
  MSG(stderr, " done [%.2fs].\n", SEC_SINCE(t));
  if (isr_close(report) != 0)   /* close the output file */
    error(E_FWRITE, isr_name(report));
  if (stats)                    /* print item set statistics */
    isr_prstats(report, stdout, 0);

  /* --- clean up --- */
  CLEANUP;                      /* clean up memory and close files */
  SHOWMEM;                      /* show (final) memory usage */
  return 0;                     /* return 'ok' */
}  /* main() */

#endif
