/*----------------------------------------------------------------------
  File    : eclat.c
  Contents: eclat algorithm for finding frequent item sets
  Author  : Christian Borgelt
  History : 2002.06.09 file created from apriori.c
            2002.12.10 option -l (list supporting transactions added)
            2002.08.16 transaction reading improved
            2003.08.18 memory benchmarking functionality added
            2003.08.20 option -t (target type) added
            2003.08.22 based on transaction module from apriori
            2003.08.23 option -q (item sort control) added
            2003.09.12 option -u (sparse representation) added
            2004.08.16 bug concerning option -q0 fixed (min. support)
            2004.11.23 absolute/relative support output changed
            2004.12.09 filter added (binary logarithm of supp. quotient)
            2005.06.20 use of flag for "no item sorting" corrected
            2006.11.26 adapted to new structures ISFMTR and ISEVAL
            2006.11.28 closed and maximal item sets without repository
            2007.02.13 adapted to modified module tabread
            2008.05.02 default limit for maximal number of items removed
            2008.10.26 complete redesign to simplify the structure
            2008.10.28 basic functionality of redesign completed
            2008.10.30 output of transaction ids per item set added
            2008.10.31 closed and maximal item set mining added
            2008.11.13 adapted to changes in transaction management
            2008.12.05 item set reporting order changed (post-order)
            2009.05.28 adapted to modified function tbg_filter()
            2009.10.09 closed/maximal item set check with repository
            2009.10.15 adapted to item set counter in reporter
            2010.03.09 version using transaction ranges added
            2010.03.17 head union tail pruning for maximal sets added
            2010.04.29 bug in memory organization fixed (if n > m)
            2010.06.23 code for transaction id reporting simplified
            2010.07.01 search based on diffsets added (dEclat)
            2010.07.04 bug in tid list setup fixed (after tbg_filter)
            2010.07.09 variant with horizontal processing added
            2010.07.11 filter version of intersection variant added
            2010.07.14 output file made optional (for benchmarking)
            2010.07.15 variant based on an item occurrence table added
            2010.08.05 closedness check based on extensions improved
            2010.08.19 item selection file added as optional input
            2010.08.22 adapted to modified modules tabread and tract
            2010.10.15 adapted to modified interface of module report
            2010.11.24 adapted to modified error reporting (tract)
            2010.11.26 memory handling simplified (list base sizes etc.)
            2010.12.11 adapted to a generic error reporting function
            2010.12.20 adapted to function tbg_icnts() (filter problem)
            2011.03.14 bug in memory allocation in eclat() fixed
            2011.03.19 two range checks for malloc() calls added
            2011.03.20 optional integer transaction weights added
            2011.07.08 adapted to modified function tbg_recode()
            2011.07.27 bug in function eclat_diff() fixed (list length)
            2011.07.29 re-sorting switched off for closed/maximal
            2011.08.15 bit vector version (with masking/reduction) added
            2011.08.16 adapted algorithm variants to finding generators
            2011.08.17 bit vector version modified to use bit map table
            2011.08.28 output of item set counters per size added
            2011.08.31 occurrence deliver version eclat_ocd() added
            2011.09.02 closed/maximal filtering without repo. improved
            2011.09.16 using 16-items machine for trans. ranges added
            2011.09.20 bug in closed/maximal filtering fixed (no repo.)
            2011.09.27 bug in algorithm and mode checking fixed (hut)
            2011.10.01 packing and sorting order for transaction ranges
            2012.04.10 bug in function rec_odfx() fixed (isr_xable())
            2012.05.25 occurrence deliver with item reordering added
            2012.06.13 bug in function rec_odro() fixed (single trans.)
            2012.06.19 function rec_odro() redesigned (delayed 16-items)
            2012.06.22 use of 16-items machine in rec_odro() improved
            2013.01.24 closed/maximal filtering with vertical database
            2013.02.04 bug in transaction sorting for eclat_trg() fixed
            2013.02.07 check of elim. item support in closed() added
            2013.03.07 direction parameter added to sorting functions
            2013.03.22 adapted to type changes in module tract (SUPP)
            2013.03.26 adapted to type changes in module tract (TID)
            2013.03.28 adapted to type changes in module tract (ITEM)
            2013.10.15 checks of return code of isr_report() added
            2013.10.18 optional pattern spectrum collection added
            2013.11.12 item selection file changed to option -R#
            2013.11.22 bug in function rec_odro() fixed (option -l0)
            2014.05.12 option -F# added (support border for filtering)
            2014.08.05 association rule generation/evaluation added
            2014.08.22 adapted to modified item set reporter interface
            2014.08.28 functions ecl_data() and ecl_report() added
            2014.09.04 functions rec_odcm(), odclo() and odmax() added
            2014.09.08 item bit filtering added to closed() and odclo()
            2014.10.24 changed from LGPL license to MIT license
            2016.02.18 bug concerning ECL_TIDS fixed (exclude ECL_FIM16)
            2016.02.19 added pre-formatting for some integer numbers
            2016.09.26 bug in head union tail (HUT) pruning fixed
            2016.09.26 head union tail (HUT) pruning for fim16 added
            2016.10.06 bug in function rec_tid() fixed (m++ -> ++m)
            2016.11.10 eclat miner object and interface introduced
------------------------------------------------------------------------
  Reference for the Eclat algorithm:
  * M.J. Zaki, S. Parthasarathy, M. Ogihara, and W. Li.
    New Algorithms for Fast Discovery of Association Rules.
    Proc. 3rd Int. Conf. on Knowledge Discovery and Data Mining
    (KDD 1997, Newport Beach, CA), 283-296.
    AAAI Press, Menlo Park, CA, USA 1997
  Reference for the dEclat algorithm (diffsets, option -Ad):
  * M.J. Zaki and K. Gouda.
    Fast Vertical Mining Using Diffsets.
    Proc. 9th ACM SIGKDD Int. Conf. on Knowledge Discovery
    and Data Mining (KDD 2003, Washington, DC), 326-335.
    ACM Press, New York, NY, USA 2003
  References for the LCM algorithm (occurrence deliver, option -Ao):
  * T. Uno, T. Asai, Y. Uchida, and H. Arimura.
    LCM: An Efficient Algorithm for Enumerating
    Frequent Closed Item Sets.
    Proc. Workshop on Frequent Item Set Mining Implementations
    (FIMI 2003, Melbourne, FL).
    CEUR Workshop Proceedings 90, TU Aachen, Germany 2003
    http://www.ceur-ws.org/Vol-90/
  * T. Uno, M. Kiyomi and H. Arimura.
    LCM ver.2: Efficient Mining Algorithms
    for Frequent/Closed/Maximal Itemsets.
    Proc. Workshop Frequent Item Set Mining Implementations
    (FIMI 2004, Brighton, UK).
    CEUR Workshop Proceedings 126, Aachen, Germany 2004
    http://www.ceur-ws.org/Vol-126/
  * T. Uno, M. Kiyomi, and H. Arimura.
    LCM ver.3: Collaboration of Array, Bitmap and Prefix Tree
    for Frequent Itemset Mining
    Proc. 1st Int. Workshop Open Source Data Mining (OSDM 2005)
    ACM Press, New York, NY, USA 2005
----------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <float.h>
#include <math.h>
#include <time.h>
#include <assert.h>
#ifndef ISR_PATSPEC
#define ISR_PATSPEC
#endif
#ifndef ISR_CLOMAX
#define ISR_CLOMAX
#endif
#ifdef  ECL_MAIN
#ifndef PSP_REPORT
#define PSP_REPORT
#endif
#ifndef TA_READ
#define TA_READ
#endif
#endif
#ifdef ECL_ABORT
#include "sigint.h"
#endif
#include "eclat.h"
#include "fim16.h"
#ifdef ECL_MAIN
#include "error.h"
#endif
#ifdef STORAGE
#include "storage.h"
#endif

#define BITMAP_TABLE            /* use a table instead of shifting */

/*----------------------------------------------------------------------
  Preprocessor Definitions
----------------------------------------------------------------------*/
#define PRGNAME     "eclat"
#define DESCRIPTION "find frequent item sets with the eclat algorithm"
#define VERSION     "version 5.19 (2017.03.24)        " \
                    "(c) 2002-2017   Christian Borgelt"

/* --- error codes --- */
/* error codes   0 to  -4 defined in tract.h */
#define E_STDIN      (-5)       /* double assignment of stdin */
#define E_OPTION     (-6)       /* unknown option */
#define E_OPTARG     (-7)       /* missing option argument */
#define E_ARGCNT     (-8)       /* too few/many arguments */
#define E_TARGET     (-9)       /* invalid target type */
#define E_SIZE      (-10)       /* invalid item set size */
#define E_SUPPORT   (-11)       /* invalid item set support */
#define E_CONF      (-12)       /* invalid confidence */
#define E_MEASURE   (-13)       /* invalid evaluation measure */
#define E_AGGMODE   (-14)       /* invalid aggregation mode */
#define E_VARIANT   (-16)       /* invalid algorithm variant */
/* error codes -15 to -25 defined in tract.h */

#define DIFFSIZE(p,q) ((size_t)((int*)(p)-(int*)(q)) *sizeof(int))

#ifndef QUIET                   /* if not quiet version, */
#define MSG         fprintf     /* print messages */
#define XMSG        if (eclat->mode & ECL_VERBOSE) fprintf
#define CLOCK(t)    ((t) = clock())
#else                           /* if quiet version, */
#define MSG(...)    ((void)0)   /* suppress messages */
#define XMSG(...)   ((void)0)
#define CLOCK(t)    ((void)0)
#endif

#define SEC_SINCE(t)  ((double)(clock()-(t)) /(double)CLOCKS_PER_SEC)

/*----------------------------------------------------------------------
  Type Definitions
----------------------------------------------------------------------*/
typedef struct {                /* --- trans. identifier list --- */
  ITEM     item;                /* item identifier (last item in set) */
  SUPP     supp;                /* support of the item (or item set) */
  TID      tids[1];             /* array of transaction identifiers */
} TIDLIST;                      /* (transaction identifier list) */

typedef unsigned int BITBLK;    /* --- bit vector block --- */

typedef struct {                /* --- bit vector --- */
  ITEM     item;                /* item identifier (last item in set) */
  SUPP     supp;                /* support of the item (or item set) */
  BITBLK   bits[1];             /* bit vector over transactions */
} BITVEC;                       /* (bit vector) */

typedef struct {                /* --- transaction id range --- */
  TID      min;                 /* minimum transaction identifier */
  TID      max;                 /* maximum transaction identifier */
  SUPP     wgt;                 /* weight of transactions in range */
} TIDRANGE;                     /* (transaction id range) */

typedef struct {                /* --- transaction range list --- */
  ITEM     item;                /* item identifier (last item in set) */
  SUPP     supp;                /* support of the item (or item set) */
  TIDRANGE trgs[1];             /* array of transaction id ranges */
} TRGLIST;                      /* (transaction id range list) */

typedef struct {                /* --- transaction list --- */
  ITEM     item;                /* item identifier (last item in set) */
  SUPP     supp;                /* support of the item (or item set) */
  TID      cnt;                 /* number of transactions */
  TRACT    *tracts[1];          /* array  of transactions */
} TALIST;                       /* (transaction list) */

struct _eclat {                 /* --- eclat miner --- */
  int      target;              /* target type (e.g. closed/maximal) */
  double   smin;                /* minimum support of an item set */
  double   smax;                /* maximum support of an item set */
  SUPP     supp;                /* minimum support of an item set */
  SUPP     body;                /* minimum support of a rule body */
  double   conf;                /* minimum confidence of a rule */
  ITEM     zmin;                /* minimum size of a rule/item set */
  ITEM     zmax;                /* maximum size of a rule/item set */
  int      eval;                /* additional evaluation measure */
  int      agg;                 /* aggregation mode for eval. measure */
  double   thresh;              /* threshold for evaluation measure */
  int      algo;                /* variant of eclat algorithm */
  int      mode;                /* search mode (e.g. pruning) */
  TABAG    *tabag;              /* transaction bag/multiset */
  ISREPORT *report;             /* item set reporter */
  ITEM     first;               /* start value for item loops */
  int      dir;                 /* direction   for item loops */
  SUPP     *muls;               /* multiplicity of transactions */
  SUPP     *marks;              /* markers (for item occurrences) */
  ITEM     *cand;               /* to collect candidates (closed()) */
  SUPP     *miss;               /* support still missing (maximal()) */
  BITTA    *btas;               /* array of bit-rep. transactions */
  SUPP     **tab;               /* item occurrence table */
  TRACT    **hash;              /* buffer for hash table */
  TIDLIST  **elim;              /* tra. id lists of eliminated items */
  FIM16    *fim16;              /* 16-items machine */
  ISTREE   *istree;             /* item set tree for eclat_tree() */
};                              /* (eclat miner) */

typedef TID COMBFN  (TIDLIST *d, TIDLIST *s1, TIDLIST *s2, SUPP *w);
typedef int ECLATFN (ECLAT *eclat);

/*----------------------------------------------------------------------
  Constants
----------------------------------------------------------------------*/
#if !defined QUIET && defined ECL_MAIN
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
  /* E_SIZE    -10 */  "invalid item set size %"ITEM_FMT,
  /* E_SUPPORT -11 */  "invalid minimum support %g",
  /* E_CONF    -12 */  "invalid minimum confidence %g",
  /* E_MEASURE -13 */  "invalid evaluation measure '%c'",
  /* E_AGGMODE -14 */  "invalid aggregation mode '%c'",
  /* E_NOITEMS -15 */  "no (frequent) items found",
  /* E_VARIANT -16 */  "invalid eclat variant '%c'",
  /*           -17 */  "unknown error"
};
#endif

/*----------------------------------------------------------------------
  Global Variables
----------------------------------------------------------------------*/
#ifdef ECL_MAIN
#ifndef QUIET
static CCHAR    *prgname;       /* program name for error messages */
#endif
static TABREAD  *tread  = NULL; /* table/transaction reader */
static ITEMBASE *ibase  = NULL; /* item base */
static TABAG    *tabag  = NULL; /* transaction bag/multiset */
static ISREPORT *report = NULL; /* item set reporter */
static TABWRITE *twrite = NULL; /* table writer for pattern spectrum */
static double   *border = NULL; /* support border for filtering */
static ECLAT    *eclat  = NULL; /* eclat miner object */
#endif

#ifdef BITMAP_TABLE
static int    bitcnt[256];      /* bit count table */
static BITBLK bitmap[256][256]; /* bit map   table */
#endif

/*----------------------------------------------------------------------
  Auxiliary Functions for Debugging
----------------------------------------------------------------------*/
#ifndef NDEBUG

static void indent (int k)
{ while (--k >= 0) printf("   "); }

/*--------------------------------------------------------------------*/

static void show_tid (const char *text, ITEMBASE *base,
                      TIDLIST **lists, ITEM k, int ind)
{                               /* --- show a cond. trans. database */
  ITEM i, j;                    /* item, loop variable */
  TID  *s;                      /* to traverse transaction ids */

  if (text && *text) {          /* print the given text */
    indent(ind); printf("%s\n", text); }
  for (j = 0; j < k; j++) {     /* traverse the items / tid lists */
    indent(ind);                /* indent the output line */
    i = lists[j]->item;         /* print the item name and id */
    if (i < 0) printf("0x%04x  :", i & 0xff);
    else       printf("%4s[%2"ITEM_FMT"]:", ib_name(base, i), i);
    for (s = lists[j]->tids; *s >= 0; s++)
      printf(" %"TID_FMT, *s);  /* print the transaction ids */
    printf(" (%"SUPP_FMT")\n", lists[j]->supp);
  }                             /* print the item support */
}  /* show_tid() */

/*--------------------------------------------------------------------*/

static void show_tab (const char *text, ITEMBASE *base,
                      SUPP **tab, TID n, ITEM k)
{                               /* --- show item counter table */
  ITEM i;                       /* loop variable for items */
  TID  r;                       /* loop variable for rows */

  if (text && *text)            /* if it is not empty, */
    printf("%s\n", text);       /* print the given text */
  printf("    ");               /* skip row id/tid column */
  for (r = 0; r < n; r++)       /* print the transaction header */
    printf(" %3"TID_FMT, r);    /* print the row number / tid */
  printf("\n");                 /* terminate the header line */
  for (i = 0; i < k; i++) {     /* traverse the table columns */
    printf("%4s[%2"ITEM_FMT"]:", ib_name(base, i), i);
    for (r = 0; r < n; r++) printf(" %3"SUPP_FMT, tab[i][r]);
    printf("\n");               /* print the item counters */
  }                             /* and terminate the line */
}  /* show_tab() */

/*--------------------------------------------------------------------*/

static void show_trg (const char *text, ITEMBASE *base,
                      TRGLIST **lists, ITEM k, int ind)
{                               /* --- show a cond. trans. database */
  ITEM     i, j;                /* item, loop variable */
  TIDRANGE *r;                  /* to traverse transaction id ranges */

  if (text && *text) {          /* print the given text */
    indent(ind); printf("%s\n", text); }
  for (j = 0; j < k; j++) {     /* traverse the items / range lists */
    indent(ind);                /* indent the output line */
    i = lists[j]->item;         /* get the item identifier */
    r = lists[j]->trgs;         /* and the transaction ranges */
    if (i < 0) {                /* if list for packed items */
      printf("packed:");        /* print special indicator */
      for ( ; r->min >= 0; r++){/* and the transaction ids */
        printf(" %"TID_FMT":%04x", r->min, (unsigned int)r->max);
        printf(":%"SUPP_FMT, r->wgt);
      } }
    else {                      /* if list for a normal item */
      printf("%s[%"ITEM_FMT"]:", ib_name(base, i), i);
      for ( ; r->min >= 0; r++){/* print item name and id */
        printf(" %"TID_FMT"-%"TID_FMT, r->min, r->max);
        printf(":%"SUPP_FMT, r->wgt);
      }                         /* print the transaction ranges */
    }
    printf(" (%"SUPP_FMT")\n", lists[j]->supp);
  }                             /* print the item support */
}  /* show_trg() */

#endif  /* #ifndef NDEBUG */
/*----------------------------------------------------------------------
  Eclat with Transaction Id List Intersection (basic version)
----------------------------------------------------------------------*/

static TID isect (TIDLIST *dst, TIDLIST *src1, TIDLIST *src2,SUPP *muls)
{                               /* --- intersect two trans. id lists */
  TID *s1, *s2, *d;             /* to traverse sources and dest. */

  assert(dst && src1 && src2    /* check the function arguments */
  &&    (src1->tids[0] >= 0) && (src2->tids[0] >= 0) && muls);
  dst->item = src1->item;       /* copy the first item and */
  dst->supp = 0;                /* initialize the support */
  if (src1->supp > src2->supp) { s2 = src1->tids; s1 = src2->tids; }
  else                         { s1 = src1->tids; s2 = src2->tids; }
  d = dst->tids;                /* get sources and destination */
  while (1) {                   /* trans. id list intersection loop */
    if      (*s1 < *s2) s2++;   /* if one transaction id is larger, */
    else if (*s1 > *s2) s1++;   /* simply skip this transaction id */
    else if (*s1 <   0) break;  /* check for the sentinel */
    else { dst->supp += muls[*d++ = *s1++]; s2++; }
  }                             /* copy equal elements to destination */
  *d++ = (TID)-1;               /* store a sentinel at the list end */
  return (TID)(d -dst->tids);   /* return the size of the new list */
}  /* isect() */

/*--------------------------------------------------------------------*/

static int rec_base (ECLAT *eclat, TIDLIST **lists, ITEM k, size_t x)
{                               /* --- eclat recursion with tid lists */
  int     r;                    /* error status */
  ITEM    i, m, z;              /* loop variables */
  SUPP    pex;                  /* minimum support for perfect exts. */
  TIDLIST *l, *d;               /* to traverse transaction id lists */
  TIDLIST **proj = NULL;        /* trans. id lists of proj. database */
  TID     *p;                   /* to organize the trans. id lists */

  assert(eclat && lists && (k > 0));  /* check the function arguments */
  #ifdef ECL_ABORT              /* if to check for interrupt */
  if (sig_aborted()) return -1; /* if execution was aborted, */
  #endif                        /* abort the recursion */
  if ((k > 1)                   /* if there is more than one item and */
  &&  isr_xable(eclat->report, 2)) {     /* another item can be added */
    proj = (TIDLIST**)malloc((size_t)k *sizeof(TIDLIST*) +x);
    if (!proj) return -1;       /* allocate list and element arrays */
  }                             /* (memory for conditional databases) */
  if (eclat->dir > 0) { z =  k; k  = 0; }
  else                { z = -1; k -= 1; }
  for (r = 0; k != z; k += eclat->dir) {
    l = lists[k];               /* traverse the items / tid lists */
    r = isr_add(eclat->report, l->item, l->supp);
    if (r <  0) break;          /* add current item to the reporter */
    if (r <= 0) continue;       /* check if item needs processing */
    if (proj && (k > 0)) {      /* if another item can be added */
      pex = (eclat->mode & ECL_PERFECT) ? l->supp : SUPP_MAX;
      proj[m = 0] = d = (TIDLIST*)(p = (TID*)(proj +k+1));
      for (i = 0; i < k; i++) { /* intersect with preceding lists */
        x = (size_t)isect(d, lists[i], l, eclat->muls);
        if      (d->supp >= pex)         /* collect perfect exts. */
          isr_addpex(eclat->report, d->item);
        else if (d->supp >= eclat->supp) /* collect frequent exts. */
          proj[++m] = d = (TIDLIST*)(p = d->tids +x);
      }                         /* switch to the next output list */
      if (m > 0) {              /* if the projection is not empty */
        r = rec_base(eclat, proj, m, DIFFSIZE(p, proj[0]));
        if (r < 0) break;       /* recursively find freq. item sets */
      }                         /* in the created projection */
    }
    r = isr_report(eclat->report);
    if (r < 0) break;           /* report the current item set */
    isr_remove(eclat->report,1);/* remove the current item */
  }                             /* from the item set reporter */
  if (proj) free(proj);         /* delete the list and element arrays */
  return r;                     /* return the error status */
}  /* rec_base() */

/*--------------------------------------------------------------------*/

int eclat_base (ECLAT *eclat)
{                               /* --- eclat with trans. id lists */
  int        r = 0;             /* result of recursion/error status */
  ITEM       i, k, m;           /* loop variable, number of items */
  TID        n;                 /* number of transactions */
  size_t     x;                 /* number of item instances */
  SUPP       w;                 /* weight/support buffer */
  SUPP       pex;               /* minimum support for perfect exts. */
  TRACT      *t;                /* to traverse transactions */
  TIDLIST    **lists, *l;       /* to traverse transaction id lists */
  TID        *tids, *p, **next; /* to traverse transaction ids */
  const ITEM *s;                /* to traverse transaction items */
  const TID  *c;                /* item occurrence counters */

  assert(eclat);                /* check the function arguments */
  eclat->dir = (eclat->target & (ISR_CLOSED|ISR_MAXIMAL)) ? -1 : +1;
  pex = tbg_wgt(eclat->tabag);  /* get item processing direction */
  if (eclat->supp > pex)        /* and support for perfect exts. and */
    return 0;                   /* check the total transaction weight */
  if (!(eclat->mode & ECL_PERFECT)) pex = SUPP_MAX;
  n = tbg_cnt(eclat->tabag);    /* get the number of transactions */
  k = tbg_itemcnt(eclat->tabag);/* and check the number of items */
  if (k <= 0) return isr_report(eclat->report);
  c = tbg_icnts(eclat->tabag,0);/* get the number of containing */
  if (!c) return -1;            /* transactions per item */
  lists = (TIDLIST**)malloc((size_t)k *sizeof(TIDLIST*)
                           +(size_t)k *sizeof(TID*)
                           +(size_t)n *sizeof(SUPP));
  if (!lists) return -1;        /* create initial tid list array */
  next = (TID**)(lists+k);      /* and split off next position array */
  eclat->muls = (SUPP*)(next+k);/* and transaction multiplicity array */
  x = tbg_extent(eclat->tabag); /* allocate the tid list elements */
  p = tids = (TID*)malloc((size_t)k *sizeof(TIDLIST) +x *sizeof(TID));
  if (!p) { free(lists); return -1; } /* allocate tid list elements */
  for (i = 0; i < k; i++) {     /* traverse the items / tid lists */
    lists[i] = l = (TIDLIST*)p; /* get next transaction id list */
    l->item  = i;               /* initialize the list item */
    l->supp  = 0;               /* and the support counter */
    next[i]  = p = l->tids;     /* note position of next trans. id */
    p += c[i]; *p++ = (TID)-1;  /* skip space for transaction ids */
  }                             /* and store a sentinel at the end */
  while (n > 0) {               /* traverse the transactions */
    t = tbg_tract(eclat->tabag, --n);  /* get the next transaction */
    eclat->muls[n] = w = ta_wgt(t);    /* and store its weight */
    for (s = ta_items(t); *s > TA_END; s++) {
      lists[*s]->supp += w;     /* traverse the transaction's items */
      *next[*s]++      = n;     /* sum the transaction weight and */
    }                           /* collect the transaction ids */
  }
  for (i = m = 0; i < k; i++) { /* traverse the items / tid lists */
    l = lists[i];               /* eliminate all infrequent items and */
    if (l->supp <  eclat->supp) continue;  /* collect perfect exts. */
    if (l->supp >= pex) { isr_addpex(eclat->report, i); continue; }
    lists[m++] = l;             /* collect lists for frequent items */
  }                             /* (eliminate infrequent items) */
  if (m > 0)                    /* find freq. items sets recursively */
    r = rec_base(eclat, lists, m, DIFFSIZE(p,tids));
  if (r >= 0)                   /* finally report the empty item set */
    r = isr_report(eclat->report);
  free(tids); free(lists);      /* delete the allocated arrays */
  return r;                     /* return the error status */
}  /* eclat_base() */

/*----------------------------------------------------------------------
  Eclat with Transaction Id List Intersection (optimized version)
----------------------------------------------------------------------*/

static int tid_cmp (const void *a, const void *b, void *data)
{                               /* --- compare support of tid lists */
  if (((TIDLIST*)b)->supp > ((TIDLIST*)a)->supp) return  1;
  if (((TIDLIST*)b)->supp < ((TIDLIST*)a)->supp) return -1;
  return 0;                     /* return sign of support difference */
}  /* tid_cmp() */

/*--------------------------------------------------------------------*/

static int tid_cmpx (const void *a, const void *b, void *data)
{                               /* --- compare support of tid lists */
  if (((TIDLIST*)a)->item < 0) return -1;
  if (((TIDLIST*)b)->item < 0) return +1;
  if (((TIDLIST*)b)->supp > ((TIDLIST*)a)->supp) return  1;
  if (((TIDLIST*)b)->supp < ((TIDLIST*)a)->supp) return -1;
  return 0;                     /* return sign of support difference */
}  /* tid_cmpx() */

/*--------------------------------------------------------------------*/

static TID filter (TIDLIST *dst, TIDLIST *src, SUPP *muls)
{                               /* --- filter a transaction id list */
  SUPP m;                       /* multiplicity of transaction */
  TID  *s, *d;                  /* to traverse source and dest. */

  assert(dst && src && muls);   /* check the function arguments */
  dst->item = src->item;        /* copy first item and init. support */
  dst->supp = 0;                /* traverse the source trans. id list */
  for (d = dst->tids, s = src->tids; *s >= 0; s++)
    if ((m = muls[*s]) > 0) {   /* collect the marked trans. ids and */
      dst->supp += m; *d++ = *s; }    /* sum the transaction weights */
  *d++ = (TID)-1;               /* store a sentinel at the list end */
  return (TID)(d -dst->tids);   /* return the size of the new list */
}  /* filter() */

/*--------------------------------------------------------------------*/

static int closed (ECLAT *eclat, TIDLIST *list, ITEM n)
{                               /* --- check for a closed item set */
  TIDLIST    *elim;             /* to traverse eliminated items */
  const ITEM *p;                /* to traverse transaction items */
  TID        *s, *d;            /* to traverse transaction ids */
  ITEM       *t, *r;            /* to traverse items */
  ITEM       item;              /* item of the list to test */
  int        i, m;              /* loop variable, bit mask for items */

  assert(eclat && list          /* check the function arguments */
  &&    (eclat->mode & ECL_EXTCHK));
  if (eclat->mode & ECL_VERT) { /* if to use vertical representation */
    while (--n >= 0) {          /* traverse the eliminated items */
      elim = eclat->elim[n];    /* skip items with lower support */
      if (elim->supp < list->supp) continue;
      s = list->tids; d = elim->tids;
      while (1) {               /* test for a perfect extension */
        if      (*s < *d) d++;  /* skip missing destination id */
        else if (*s > *d) break;/* if source id is missing, abort */
        else if (*s <  0) return 0;
        else { s++; d++; }      /* check for the sentinel and */
      }                         /* skip matching transaction ids */
    }                           /* (all tids found: perfect ext.) */
    return -1; }                /* return 'item set is closed' */
  else {                        /* if to use horiz. representation */
    item = list->item;          /* get the item of the list */
    if (item < 31) {            /* if item bits can be used */
      for (m = 0, i = item; ++i < 32; )
        if (!isr_uses(eclat->report, (ITEM)i))
          m |= 1 << i;          /* collect bits of unused items */
      for (s = list->tids; m && (*s >= 0); s++)
        m &= ta_getmark(tbg_tract(eclat->tabag, *s));
      if (m) return 0;          /* if perf. ext found, 'not closed' */
      item = 31;                /* otherwise check remaining items */
    }                           /* (check only items 32 and above) */
    p = ta_items(tbg_tract(eclat->tabag, list->tids[0]));
    for (r = eclat->cand; *p > item; p++)
      if (!isr_uses(eclat->report, *p))
        *r++ = *p;              /* collect items from a transaction */
    if (r <= eclat->cand) return -1;
    *r = TA_END;                /* store a sentinel at the end */
    for (s = list->tids+1; *s >= 0; s++) {
      t = r = eclat->cand;      /* traverse the transaction ids */
      p = ta_items(tbg_tract(eclat->tabag, *s));
      while (1) {               /* item list intersection loop */
        if      (*t <  0) break;/* check for the list sentinel */
        else if (*t < *p) p++;  /* if one item id is larger, */
        else if (*t > *p) t++;  /* simply skip this item id, */
        else { *r++ = *t++; p++; }
      }                         /* (collect perfect ext. candidates) */
      if (r <= eclat->cand) return -1;
      *r = TA_END;              /* if intersection is empty, abort, */
    }                           /* otherwise store a sentinel */
    return 0;                   /* return 'item set is not closed' */
  }
}  /* closed() */

/*--------------------------------------------------------------------*/

static int maximal (ECLAT *eclat, TIDLIST *list, ITEM n)
{                               /* --- check for a maximal item set */
  ITEM       i;                 /* loop variable for items */
  SUPP       w;                 /* weight/support buffer */
  const ITEM *p;                /* to traverse transaction items */
  TID        *s, *d;            /* to traverse sources and dest. */

  assert(eclat && list          /* check the function arguments */
  &&    (eclat->mode & ECL_EXTCHK));
  if (eclat->mode & ECL_VERT) { /* if to use vertical representation */
    while (--n >= 0) {          /* traverse the eliminated items */
      s = list->tids; d = eclat->elim[n]->tids;
      for (w = 0; 1; ) {        /* test for a perfect extension */
        if      (*s < *d) d++;  /* if one transaction id is larger, */
        else if (*s > *d) s++;  /* skip missing destination id */
        else if (*s <  0) break;/* check for the sentinel and */
        else { w += eclat->muls[*s++]; d++; }
      }                         /* sum weights of matching trans. ids */
      if (w >= eclat->supp) return 0;
    } }                         /* check for a frequent extension */
  else {                        /* if to use horiz. representation */
    for (i = tbg_itemcnt(eclat->tabag); --i > list->item; )
      eclat->miss[i] = (isr_uses(eclat->report, i))
                     ? list->supp+1 : eclat->supp;
    for (s = list->tids; *s >= 0; s++) {
      w = eclat->muls[*s];      /* traverse the transactions */
      p = ta_items(tbg_tract(eclat->tabag, *s));
      for ( ; *p > list->item; p++)
        if ((eclat->miss[*p] -= w) <= 0) return 0;
    }                           /* count support of candidate exts.; */
  }                             /* if frequent cand. found, abort */
  return -1;                    /* return 'set is maximal' */
}  /* maximal() */

/*--------------------------------------------------------------------*/

static int rec_tcm (ECLAT *eclat, TIDLIST **lists,
                    ITEM k, size_t x, ITEM e)
{                               /* --- eclat recursion with tid lists */
  int     r;                    /* error status */
  ITEM    i, m, z;              /* loop variables */
  SUPP    max;                  /* maximum support of an ext. item */
  SUPP    pex;                  /* minimum support for perfect exts. */
  TIDLIST **proj = NULL;        /* trans. id lists of proj. database */
  TIDLIST *l, *d;               /* to traverse transaction id lists */
  TID     *p;                   /* to traverse transaction ids */

  assert(eclat && lists && (k > 0)); /* check the function arguments */
  #ifdef ECL_ABORT              /* if to check for interrupt */
  if (sig_aborted()) return -1; /* if execution was aborted, */
  #endif                        /* abort the recursion */
  if ((k > 1)                   /* if there is more than one item and */
  &&  isr_xable(eclat->report, 2)) {     /* another item can be added */
    proj = (TIDLIST**)malloc((size_t)k *sizeof(TIDLIST*) +x);
    if (!proj) return -1;       /* allocate list and element arrays */
  }                             /* (memory for conditional databases) */
  if ((k > 4)                   /* if there are enough items left, */
  &&  (eclat->mode & ECL_REORDER)) /* re-sort items w.r.t. support */
    ptr_qsort(lists, (size_t)k, 1,
              (eclat->fim16) ? tid_cmpx : tid_cmp, NULL);
  if (eclat->dir > 0) { z =  k; k  = 0; }
  else                { z = -1; k -= 1; }
  for (r = 0; k != z; k += eclat->dir) {
    l = lists[k];               /* traverse the items / tid lists */
    if (!closed(eclat, l, e))   /* if the current set is not closed, */
      continue;                 /* the item need not be processed */
    r = isr_addnc(eclat->report, l->item, l->supp);
    if (r < 0) break;           /* add current item to the reporter */
    max = 0;                    /* init. maximal extension support */
    if (proj && (k > 0)) {      /* if another item can be added */
      pex = (eclat->mode & ECL_PERFECT) ? l->supp : SUPP_MAX;
      proj[m = 0] = d = (TIDLIST*)(proj +k+1);
      if (k < 2) {              /* if there are only few items left */
        /* Benchmark tests showed that this version is faster only */
        /* if there is only one other tid list to intersect with.  */
        if (lists[i = 0]->item < 0) { /* if there are packed items */
          x = (size_t)isect(d, lists[i++], l, eclat->muls);
          if (d->supp >= eclat->supp) { /* if they are frequent */
            proj[++m] = d = (TIDLIST*)(d->tids +x); }
        }                       /* add a tid list for packed items */
        for ( ; i < k; i++) {   /* traverse the preceding lists */
          x = (size_t)isect(d, lists[i], l, eclat->muls);
          if (d->supp < eclat->supp)  /* intersect trans. id lists */
            continue;           /* eliminate infrequent items */
          if (d->supp >= pex) { /* collect perfect extensions */
            isr_addpex(eclat->report, d->item); continue; }
          if (d->supp > max)    /* find maximal extension support */
            max = d->supp;      /* (for later closed/maximal check) */
          proj[++m] = d = (TIDLIST*)(d->tids +x);
        } }                     /* collect tid lists of freq. items */
      else {                    /* if there are many items left */
        for (p = l->tids; *p >= 0; p++)      /* mark transaction ids */
          eclat->marks[*p] = eclat->muls[*p]; /* in the current list */
        if (lists[i = 0]->item < 0) {   /* if there are packed items */
          x = (size_t)filter(d, lists[i++], eclat->marks);
          if (d->supp >= eclat->supp) { /* if they are frequent */
            proj[++m] = d = (TIDLIST*)(d->tids +x); }
        }                       /* add a tid list for packed items */
        for ( ; i < k; i++) {   /* traverse the preceding lists */
          x = (size_t)filter(d, lists[i], eclat->marks);
          if (d->supp < eclat->supp)    /* intersect trans. id lists */
            continue;           /* eliminate infrequent items */
          if (d->supp >= pex) { /* collect perfect extensions */
            isr_addpex(eclat->report, d->item); continue; }
          if (d->supp > max)    /* find maximal extension support */
            max = d->supp;      /* (for later closed/maximal check) */
          proj[++m] = d = (TIDLIST*)(d->tids +x);
        }                       /* collect tid lists of freq. items */
        for (p = l->tids; *p >= 0; p++)
          eclat->marks[*p] = 0; /* unmark transaction ids */
      }                         /* in the current list */
      if (m > 0) {              /* if the projection is not empty */
        r = rec_tcm(eclat, proj, m, DIFFSIZE(d,proj[0]), e);
        if (r < 0) break;       /* recursively find freq. item sets */
      }                         /* in the created projection */
    }                           /* (or rather their trans. id lists) */
    if ((eclat->target & ISR_CLOSED) ? (max < l->supp)
    :   ((max < eclat->supp) && maximal(eclat, l, e))) {
      r = isr_reportx(eclat->report, l->tids, (TID)-l->supp);
      if (r < 0) break;         /* report the current item set */
    }                           /* and check for an error */
    isr_remove(eclat->report,1);/* remove the current item and */
    if (eclat->mode & ECL_VERT) /* collect the eliminated items */
      eclat->elim[e++] = l;     /* (for closed/maximal check) */
  }
  if (proj) free(proj);         /* delete the list and element arrays */
  return r;                     /* return the error status */
}  /* rec_tcm() */

/*--------------------------------------------------------------------*/

static int rec_tid (ECLAT *eclat, TIDLIST **lists, ITEM k, size_t x)
{                               /* --- eclat recursion with tid lists */
  int     r;                    /* error status */
  ITEM    i, m, z;              /* loop variables, error status */
  SUPP    max;                  /* maximum support of an ext. item */
  SUPP    pex;                  /* minimum support for perfect exts. */
  TIDLIST **proj = NULL;        /* trans. id lists of proj. database */
  TIDLIST *l, *d;               /* to traverse transaction id lists */
  TID     *p;                   /* to traverse transaction ids */
  ITEM    *t;                   /* to collect the tail items */

  assert(eclat && lists && (k > 0)); /* check the function arguments */
  #ifdef ECL_ABORT              /* if to check for interrupt */
  if (sig_aborted()) return -1; /* if execution was aborted, */
  #endif                        /* abort the recursion */
  if (eclat->mode & ECL_TAIL) { /* if to use tail to prune w/ repo. */
    t = isr_buf(eclat->report); /* get the item set buffer */
    for (m = 0, i = k; --i > 0; )
      t[m++] = lists[i]->item;  /* collect the tail items */
    if (lists[0]->item >= 0)    /* if there are no packed items, */
      t[m++] = lists[0]->item;  /* simply copy the last item */
    else {                      /* if there are packed items */
      for (z = 16; --z >= 0; )  /* traverse the bits */
        if (lists[0]->item & (1 << z))
          t[m++] = z;           /* collect the packed tail items */
    }                           /* after the other tail items */
    r = isr_tail(eclat->report, t, m);
    if (r) return r;            /* if tail need not be processed, */
  }                             /* abort the recursion */
  if ((k > 1)                   /* if there is more than one item and */
  &&  isr_xable(eclat->report, 2)) {     /* another item can be added */
    proj = (TIDLIST**)malloc((size_t)k *sizeof(TIDLIST*) +x);
    if (!proj) return -1;       /* allocate list and element arrays */
  }                             /* (memory for conditional databases) */
  if ((k > 4)                   /* if there are enough items left, */
  &&  (eclat->mode & ECL_REORDER)) /* re-sort items w.r.t. support */
    ptr_qsort(lists, (size_t)k, 1,
              (eclat->fim16) ? tid_cmpx : tid_cmp, NULL);
  if (eclat->dir > 0) { z =  k; k  = 0; }
  else                { z = -1; k -= 1; }
  for (r = 0; k != z; k += eclat->dir) {
    l = lists[k];               /* traverse the items / tid lists */
    if (l->item < 0) {          /* if this list is for packed items */
      for (p = l->tids; *p >= 0; p++)
        m16_add(eclat->fim16, eclat->btas[*p], eclat->muls[*p]);
      r = m16_mine(eclat->fim16);    /* add bit-rep. trans. prefixes */
      if (r >= 0) continue;     /* to the 16-items machine and mine, */
      if (proj) free(proj);     /* then go to the next trans. id list */
      return r;                 /* otherwise free allocated memory */
    }                           /* and abort the function */
    r = isr_add(eclat->report, l->item, l->supp);
    if (r <  0) break;          /* add current item to the reporter */
    if (r <= 0) continue;       /* check if item needs processing */
    max = 0;                    /* init. maximal extension support */
    if (proj && (k > 0)) {      /* if another item can be added */
      pex = (eclat->mode & ECL_PERFECT) ? l->supp : SUPP_MAX;
      proj[m = 0] = d = (TIDLIST*)(proj +k+1);
      if (k < 2) {              /* if there are only few items left */
        /* Benchmark tests showed that this version is faster only */
        /* if there is only one other tid list to intersect with.  */
        if (lists[i = 0]->item < 0) { /* if there are packed items */
          x = (size_t)isect(d, lists[i++], l, eclat->muls);
          if (d->supp >= eclat->supp) { /* if they are frequent */
            for (d->item = TA_END, p = d->tids; *p >= 0; p++)
              d->item |= eclat->btas[*p];
            proj[++m] = d = (TIDLIST*)(d->tids +x);
          }                     /* add a tid list for packed items */
        }                       /* with occ. bit mask as the item */
        for ( ; i < k; i++) {   /* traverse the preceding lists */
          x = (size_t)isect(d, lists[i], l, eclat->muls);
          if (d->supp < eclat->supp) /* intersect trans. id lists */
            continue;           /* eliminate infrequent items */
          if (d->supp >= pex) { /* collect perfect extensions */
            isr_addpex(eclat->report, d->item); continue; }
          if (d->supp > max)    /* find maximal extension support */
            max = d->supp;      /* (for later closed/maximal check) */
          proj[++m] = d = (TIDLIST*)(d->tids +x);
        } }                     /* collect tid lists of freq. items */
      else {                    /* if there are many items left */
        for (p = l->tids; *p >= 0; p++)      /* mark transaction ids */
          eclat->marks[*p] = eclat->muls[*p]; /* in the current list */
        if (lists[i = 0]->item < 0) {   /* if there are packed items */
          x = (size_t)filter(d, lists[i++], eclat->marks);
          if (d->supp >= eclat->supp) { /* if they are frequent */
            for (d->item = TA_END, p = d->tids; *p >= 0; p++)
              d->item |= eclat->btas[*p];
            proj[++m] = d = (TIDLIST*)(d->tids +x);
          }                     /* add a tid list for packed items */
        }                       /* with occ. bit mask as the item */
        for ( ; i < k; i++) {   /* traverse the preceding lists */
          x = (size_t)filter(d, lists[i], eclat->marks);
          if (d->supp < eclat->supp) /* intersect trans. id lists */
            continue;           /* eliminate infrequent items */
          if (d->supp >= pex) { /* collect perfect extensions */
            isr_addpex(eclat->report, d->item); continue; }
          if (d->supp > max)    /* find maximal extension support */
            max = d->supp;      /* (for later closed/maximal check) */
          proj[++m] = d = (TIDLIST*)(d->tids +x);
        }                       /* collect tid lists of freq. items */
        for (p = l->tids; *p >= 0; p++)
          eclat->marks[*p] = 0; /* unmark transaction ids */
      }                         /* in the current list */
      if (m > 0) {              /* if the projection is not empty */
        r = rec_tid(eclat, proj, m, DIFFSIZE(d,proj[0]));
        if (r < 0) break;       /* recursively find freq. item sets */
      }                         /* in the created projection */
    }
    r = isr_reportx(eclat->report, l->tids, (TID)-l->supp);
    if (r < 0) break;           /* report the current item set */
    isr_remove(eclat->report,1);/* remove the current item */
  }                             /* from the item set reporter */
  if (proj) free(proj);         /* delete the list and element arrays */
  return r;                     /* return the error status */
}  /* rec_tid() */

/*--------------------------------------------------------------------*/

int eclat_tid (ECLAT *eclat)
{                               /* --- eclat with trans. id lists */
  int        r = 0;             /* result of recursion/error status */
  ITEM       i, k, m, e;        /* loop variable, number of items */
  TID        n;                 /* number of transactions */
  size_t     x, z;              /* number of item instances */
  SUPP       w;                 /* weight/support buffer */
  SUPP       max;               /* maximum support of an item */
  SUPP       pex;               /* minimum support for perfect exts. */
  TRACT      *t;                /* to traverse transactions */
  TIDLIST    **lists, *l;       /* to traverse transaction id lists */
  TID        *tids, *p, **next; /* to traverse transaction ids */
  const ITEM *s;                /* to traverse transaction items */
  const TID  *c;                /* item occurrence counters */

  assert(eclat);                /* check the function arguments */
  eclat->dir = (eclat->target & (ISR_CLOSED|ISR_MAXIMAL)) ? -1 : +1;
  pex = tbg_wgt(eclat->tabag);  /* get the item processing direction */
  if (eclat->supp > pex)        /* and support for perfect exts. and */
    return 0;                   /* check the total transaction weight */
  if (!(eclat->mode & ECL_PERFECT)) pex = SUPP_MAX;
  k = tbg_itemcnt(eclat->tabag);/* get and check the number of items */
  if (k <= 0) return isr_report(eclat->report);
  n = tbg_cnt(eclat->tabag);    /* get the number of transactions */
  c = tbg_icnts(eclat->tabag,0);/* and the number of containing */
  if (!c) return -1;            /* transactions per item */
  e = (eclat->mode & ECL_VERT) ? k   : 0;
  m = (eclat->mode & ECL_HORZ) ? k+1 : 0;
  x = (eclat->mode & ECL_FIM16) ? (size_t)n *sizeof(BITTA) : 0;
  z = (sizeof(ITEM) > sizeof(SUPP)) ? sizeof(ITEM) : sizeof(SUPP);
  lists = (TIDLIST**)malloc((size_t)(k+e) *sizeof(TIDLIST*)
                           +(size_t) k    *sizeof(TID*)
                           +(size_t)(n+n) *sizeof(SUPP)
                           +(size_t) m    *z +x);
  if (!lists) return -1;        /* create initial tid list array and */
  eclat->elim  = lists +k;      /* split off the additional arrays */
  next         = (TID**) (eclat->elim +e);
  eclat->muls  = (SUPP*) (next    +k);
  eclat->miss  = (SUPP*) (eclat->muls +n); /* buffer for maximal() */
  eclat->cand  = (ITEM*)  eclat->miss;     /* buffer for closed() */
  eclat->marks = (sizeof(ITEM) > sizeof(SUPP))
               ? (SUPP*)(eclat->cand+m) : eclat->miss+m;
  eclat->btas  = (BITTA*)(eclat->marks+n);
  memset(eclat->marks, 0, (size_t)n *sizeof(TID));
  for (x = 0, i = 0; i < k; i++)/* traverse the items and sum */
    x += (size_t)c[i];          /* the number of item occurrences */
  /* Do not use tbg_extent(), because it does not take packed items */
  /* properly into account and thus may yield too big a value.      */
  if (x < (size_t)n) x = (size_t)n; /* ensure enough transaction ids */
  p = tids = (TID*)malloc((size_t)k *sizeof(TIDLIST) +x *sizeof(TID));
  if (!p) { free(lists); return -1; } /* allocate tid list elements */
  for (i = 0; i < k; i++) {     /* traverse the items / tid lists */
    lists[i] = l = (TIDLIST*)p; /* get next transaction id list */
    l->item  = i;               /* initialize the list item */
    l->supp  = 0;               /* and the support counter */
    next[i]  = p = l->tids;     /* note position of next trans. id */
    p += c[i]; *p++ = (TID)-1;  /* skip space for transaction ids */
  }                             /* and store a sentinel at the end */
  z = DIFFSIZE(p, tids);        /* get total size of tid lists */
  while (n > 0) {               /* traverse the transactions */
    t = tbg_tract(eclat->tabag, --n);  /* get the next transaction */
    eclat->muls[n] = w = ta_wgt(t);    /* and store its weight */
    for (s = ta_items(t); *s > TA_END; s++) {
      if ((i = *s) < 0) {       /* traverse the transaction's items */
        eclat->btas[n] = (BITTA)i; i = 0; }
      lists[i]->supp += w;      /* traverse the transaction's items */
      *next[i]++      = n;      /* sum the transaction weight and */
    }                           /* collect the transaction ids */
  }
  eclat->fim16 = NULL;          /* default: no 16-items machine */
  l = lists[i = 0];             /* get the list for packed items */
  if ((eclat->mode & ECL_FIM16) /* if to use a 16-items machine */
  &&  (l->supp >= eclat->supp)){/* and there are packed items */
    eclat->fim16 = m16_create(eclat->dir, eclat->supp, eclat->report);
    if (!eclat->fim16) { free(tids); free(lists); return -1; }
    l->item = TA_END; i = 1;    /* mark list for the packed items */
    for (p = l->tids; *p >= 0; p++)
      l->item |= eclat->btas[*p];     /* collect the occurring items */
  }                             /* and add list to the reduced array */
  max = 0;                      /* init. the maximal item support */
  for (m = i; i < k; i++) {     /* traverse the items / tid lists */
    l = lists[i];               /* eliminate all infrequent items and */
    if (l->supp <  eclat->supp) continue;    /* collect perfect exts. */
    if (l->supp >= pex) { isr_addpex(eclat->report, i); continue; }
    if (l->supp >  max)         /* find the maximal item support */
      max = l->supp;            /* (for later closed/maximal check) */
    lists[m++] = l;             /* collect lists for frequent items */
  }                             /* (eliminate infrequent items) */
  if (m > 0) {                  /* if there are frequent items, dep. */
    r = (eclat->mode & ECL_EXTCHK)   /* on how to filter closed/max. */
      ? rec_tcm(eclat, lists, m, z, 0)
      : rec_tid(eclat, lists, m, z);
  }                             /* find freq. item sets recursively */
  if (r >= 0) {                 /* if no error occurred */
    i = eclat->target & (ISR_CLOSED|ISR_MAXIMAL);
    w = (i & ISR_MAXIMAL) ? eclat->supp : tbg_wgt(eclat->tabag);
    if (!i || (max < w)) {      /* if to report the empty set */
      if (!isr_tidfile(eclat->report)) /* if not to report tr. ids, */
        r = isr_report(eclat->report); /* report the empty item set */
      else {                    /* if to report transaction ids */
        for (n = tbg_cnt(eclat->tabag); n > 0; n--) tids[n] = n;
        r = isr_reportx(eclat->report, tids, (TID)n);
      }                         /* report the empty item set */
    }                           /* with all transaction ids */
  }
  if (eclat->fim16)             /* if a 16-items machine was used, */
    m16_delete(eclat->fim16);   /* delete the 16-items machine */
  free(tids); free(lists);      /* delete the allocated arrays */
  return r;                     /* return the error status */
}  /* eclat_tid() */

/*----------------------------------------------------------------------
  Eclat with Bit Vectors
----------------------------------------------------------------------*/

static int bit_cmp (const void *a, const void *b, void *data)
{                               /* --- compare support of tid lists */
  if (((BITVEC*)b)->supp > ((BITVEC*)a)->supp) return  1;
  if (((BITVEC*)b)->supp < ((BITVEC*)a)->supp) return -1;
  return 0;                     /* return sign of support difference */
}  /* bit_cmp() */

/*--------------------------------------------------------------------*/
#ifdef BITMAP_TABLE

static void bit_init (void)
{                               /* --- init. bit count/map tables */
  int i, k, b;                  /* loop variables, bit index */

  if (bitcnt[1] != 0) return;   /* check for an initialized table */
  for (i = 0; ++i < 256; )      /* traverse all byte values */
    for (k = i; k; k >>= 1)     /* traverse the bits in the value */
      bitcnt[i] += k & 1;       /* store their number in the table */
  memset(bitmap[0], 0, sizeof(bitmap[0]));
  for (k = 0; k < 256; ) { bitmap[1][k++] = 0; bitmap[1][k++] = 1; }
  for (i = 1; ++i < 255; ) {    /* traverse the matrix rows (masks) */
    for (b = 8; --b >= 0; ) {   /* traverse set bits of the mask */
      if (((i >> b) & 1) == 0) continue;
      for (k = 0; k < 256; ) {  /* traverse the matrix columns */
        bitmap[i][k] = (bitmap[i][k] << 1) | ((k >> b) & 1); k++;
        bitmap[i][k] = (bitmap[i][k] << 1) | ((k >> b) & 1); k++;
        bitmap[i][k] = (bitmap[i][k] << 1) | ((k >> b) & 1); k++;
        bitmap[i][k] = (bitmap[i][k] << 1) | ((k >> b) & 1); k++;
        bitmap[i][k] = (bitmap[i][k] << 1) | ((k >> b) & 1); k++;
        bitmap[i][k] = (bitmap[i][k] << 1) | ((k >> b) & 1); k++;
        bitmap[i][k] = (bitmap[i][k] << 1) | ((k >> b) & 1); k++;
        bitmap[i][k] = (bitmap[i][k] << 1) | ((k >> b) & 1); k++;
      }                         /* collect the bits of the source */
    }                           /* that are under the mask bits */
  }                             /* for faster bit vector reduction */
  for (k = 0; k < 256; k++) bitmap[255][k] = (BITBLK)k;
}  /* bit_init() */

/*--------------------------------------------------------------------*/

static void bit_isect (BITVEC *dst, BITVEC *src1, BITVEC *src2, TID n)
{                               /* --- intersect two bit vectors */
  BITBLK *s1, *s2, *d;          /* to traverse sources and dest. */
  BITBLK s, m, o, x;            /* source, mask, and output blocks */
  int    b, c;                  /* number of bits in output */

  assert(dst && src1 && src2);  /* check the function arguments */
  dst->item = src1->item;       /* copy the first item and */
  dst->supp = 0;                /* initialize the support */
  d = dst->bits; s1 = src1->bits; s2 = src2->bits;
  for (o = 0, b = 0; n > 0; n--) { /* traverse the bit vector blocks */
    s = *s1++; m = *s2++;       /* traverse the bytes of each block */
    for ( ; m != 0; s >>= 8, m >>= 8) {
      dst->supp += (SUPP)bitcnt[x = bitmap[m & 0xff][s & 0xff]];
      o |= x << b;         b += c = bitcnt[m & 0xff];
      if (b < 32) continue;     /* add output bits for current byte */
      b -= 32; *d++ = o;        /* if a bit block is full, store it */
      o = x >> (c-b-1) >> 1;    /* store remaining bits in buffer, */
    }                           /* but note that x >> 32 == x >> 0, */
  }                             /* so simply o = x >> (c-b) fails */
  if (b > 0) *d = o;            /* store the last bit vector block */
}  /* bit_isect() */

/*--------------------------------------------------------------------*/
#else

#define bit_init()              /* no initialization needed */

static void bit_isect (BITVEC *dst, BITVEC *src1, BITVEC *src2, TID n)
{                               /* --- intersect two bit vectors */
  BITBLK *s1, *s2, *d;          /* to traverse sources and dest. */
  BITBLK s, m, o;               /* source, mask, and output block */
  int    b;                     /* number of bits in output */

  assert(dst && src1 && src2);  /* check the function arguments */
  dst->item = src1->item;       /* copy the first item and */
  dst->supp = 0;                /* initialize the support */
  d = dst->bits; s1 = src1->bits; s2 = src2->bits;
  for (o = 0, b = 0; n > 0; n--) { /* traverse the bit vector blocks */
    for (s = *s1++, m = *s2++; m != 0; ) {
      if (m & 1) {              /* if first of four mask bits is set */
        dst->supp += (SUPP)(s & 1); o |= (s & 1) << b;
        if (++b >= 32) { *d++ = o; o = 0; b = 0; }
      }                         /* copy the source bit to the output */
      s >>= 1; m >>= 1;         /* get the next source and mask bit */
      if (m & 1) {              /* if first of four mask bits is set */
        dst->supp += (SUPP)(s & 1); o |= (s & 1) << b;
        if (++b >= 32) { *d++ = o; o = 0; b = 0; }
      }                         /* copy the source bit to the output */
      s >>= 1; m >>= 1;         /* get the next source and mask bit */
      if (m & 1) {              /* if first of four mask bits is set */
        dst->supp += (SUPP)(s & 1); o |= (s & 1) << b;
        if (++b >= 32) { *d++ = o; o = 0; b = 0; }
      }                         /* copy the source bit to the output */
      s >>= 1; m >>= 1;         /* get the next source and mask bit */
      if (m & 1) {              /* if first of four mask bits is set */
        dst->supp += (SUPP)(s & 1); o |= (s & 1) << b;
        if (++b >= 32) { *d++ = o; o = 0; b = 0; }
      }                         /* copy the source bit to the output */
      s >>= 1; m >>= 1;         /* get the next source and mask bit */
    }                           /* collect the source bits */
  }                             /* for which a mask bit is set */
  if (b > 0) *d = o;            /* store the last bit block */
}  /* bit_isect() */

#endif
/*--------------------------------------------------------------------*/

static int rec_bit (ECLAT *eclat, BITVEC **vecs, ITEM k, TID n)
{                               /* --- eclat recursion with bit vecs. */
  int    r;                     /* error status */
  ITEM   i, m, z;               /* loop variables */
  SUPP   pex;                   /* minimum support for perf. exts. */
  TID    len;                   /* length of (reduced) bit vectors */
  BITVEC **proj = NULL;         /* bit vectors of projected database */
  BITVEC *v, *d;                /* to traverse bit vectors */
  BITBLK *p;                    /* to traverse bit vector blocks */
  ITEM   *t;                    /* to collect the tail items */

  assert(eclat && vecs && (k > 0)); /* check the function arguments */
  #ifdef ECL_ABORT              /* if to check for interrupt */
  if (sig_aborted()) return -1; /* if execution was aborted, */
  #endif                        /* abort the recursion */
  if (eclat->mode & ECL_TAIL) { /* if to use tail to prune w/ repo. */
    t = isr_buf(eclat->report); /* collect the tail items in buffer */
    for (m = 0, i = k; --i >= 0; ) t[m++] = vecs[i]->item;
    r = isr_tail(eclat->report, t, m);
    if (r) return r;            /* if tail need not be processed, */
  }                             /* abort the recursion */
  if ((k > 1)                   /* if there is more than one item and */
  &&  isr_xable(eclat->report, 2)) {     /* another item can be added */
    proj = (BITVEC**)malloc((size_t)k                *sizeof(BITVEC*)
                          + (size_t)k                *sizeof(BITVEC)
                          +((size_t)k*(size_t)(n-1)) *sizeof(BITBLK));
    if (!proj) return -1;       /* allocate bit vectors and array */
  }                             /* (memory for conditional databases) */
  if ((k > 4)                   /* if there are enough items left, */
  &&  (eclat->mode & ECL_REORDER)) /* re-sort items w.r.t. support */
    ptr_qsort(vecs, (size_t)k, +1, bit_cmp, NULL);
  if (eclat->dir > 0) { z =  k; k  = 0; }
  else                { z = -1; k -= 1; }
  for (r = 0; k != z; k += eclat->dir) {
    v = vecs[k];                /* traverse the remaining items */
    r = isr_add(eclat->report, v->item, v->supp);
    if (r <  0) break;          /* add current item to the reporter */
    if (r <= 0) continue;       /* check if item needs processing */
    if (proj && (k > 0)) {      /* if another item can be added */
      len = (TID)(v->supp+31) >> 5;    /* get new vector length */
      pex = (eclat->mode & ECL_PERFECT) ? v->supp : SUPP_MAX;
      proj[m = 0] = d = (BITVEC*)(p = (BITBLK*)(proj +k+1));
      for (i = 0; i < k; i++) { /* traverse preceding vectors */
        bit_isect(d, vecs[i], v, n);
        if (d->supp < eclat->supp) /* intersect trans. bit vectors */
          continue;             /* eliminate infrequent items */
        if (d->supp >= pex) {   /* collect perfect extensions */
          isr_addpex(eclat->report, d->item); continue; }
        proj[++m] = d = (BITVEC*)(p = d->bits +len);
      }                         /* collect the remaining bit vectors */
      if (m > 0) {              /* if the projection is not empty */
        r = rec_bit(eclat, proj, m, len);
        if (r < 0) break;       /* recursively find freq. item sets */
      }                         /* in the created projection */
    }
    r = isr_report(eclat->report);
    if (r < 0) break;           /* report the current item set */
    isr_remove(eclat->report,1);/* remove the current item */
  }                             /* from the item set reporter */
  if (proj) free(proj);         /* delete bit vectors and array */
  return r;                     /* return the error status */
}  /* rec_bit() */

/*--------------------------------------------------------------------*/

int eclat_bit (ECLAT *eclat)
{                               /* --- eclat with bit vectors */
  int        r = 0;             /* result of recursion/error status */
  ITEM       i, k, m;           /* loop variable, number of items */
  TID        n;                 /* number of transactions */
  TID        x;                 /* number of item instances */
  SUPP       pex;               /* minimum support for perfect exts. */
  TRACT      *t;                /* to traverse transactions */
  BITVEC     **vecs, *v;        /* to traverse bit vectors */
  BITBLK     *p;                /* to traverse bit vector blocks */
  const ITEM *s;                /* to traverse transaction items */

  assert(eclat);                /* check the function arguments */
  eclat->dir = (eclat->target & (ISR_CLOSED|ISR_MAXIMAL)) ? -1 : +1;
  pex = tbg_wgt(eclat->tabag);  /* get the item processing direction */
  if (eclat->supp > pex)        /* and support for perfect exts. and */
    return 0;                   /* check the total transaction weight */
  if (!(eclat->mode & ECL_PERFECT)) pex = SUPP_MAX;
  n = tbg_cnt(eclat->tabag);    /* get the number of transactions */
  k = tbg_itemcnt(eclat->tabag);/* and check the number of items */
  if (k <= 0) return isr_report(eclat->report);
  bit_init();                   /* initialize the bit count table */
  x = (n + 31) >> 5;            /* and compute the bit vector size */
  vecs = (BITVEC**)malloc((size_t)k                *sizeof(BITVEC*)
                        + (size_t)k                *sizeof(BITVEC)
                        +((size_t)k*(size_t)(x-1)) *sizeof(BITBLK));
  if (!vecs) return -1;         /* create initial bit vector array */
  p = (BITBLK*)(vecs+k);        /* and get the bit vector memory */
  for (i = 0; i < k; i++) {     /* traverse the items / bit vectors */
    vecs[i] = v = (BITVEC*)p;   /* get/create the next bit vector */
    v->item = i;                /* initialize the bit vector item */
    v->supp = 0;                /* and the support counter */
    memset(v->bits, 0, (size_t)x *sizeof(BITBLK));
    p = v->bits +x;             /* clear all transaction bits and */
  }                             /* skip them to get the next vector */
  while (n > 0) {               /* traverse the transactions */
    t = tbg_tract(eclat->tabag, --n);  /* retrieve next transaction */
    assert(ta_wgt(t) == 1);     /* transaction weight must be 1 */
    for (s = ta_items(t); *s > TA_END; s++) {
      v = vecs[*s];             /* traverse the transaction's items */
      v->supp += 1;             /* sum/count the transaction weight */
      v->bits[n >> 5] |= (BITBLK)(1 << (n & 0x1f));
    }                           /* set the bit for the current trans. */
  }                             /* to indicate that item is contained */
  for (i = m = 0; i < k; i++) { /* traverse the items / bit vectors */
    v = vecs[i];                /* eliminate all infrequent items and */
    if (v->supp <  eclat->supp) continue;    /* collect perfect exts. */
    if (v->supp >= pex) { isr_addpex(eclat->report, i); continue; }
    vecs[m++] = v;              /* collect vectors for frequent items */
  }                             /* (eliminate infrequent items) */
  if (m > 0)                    /* find freq. items sets recursively */
    r = rec_bit(eclat, vecs, m, x);
  if (r >= 0)                   /* finally report the empty item set */
    r = isr_report(eclat->report);
  free(vecs);                   /* delete the allocated bit vectors */
  return r;                     /* return the error status */
}  /* eclat_bit() */

/*----------------------------------------------------------------------
  Eclat with an Occurrence Indicator Table
----------------------------------------------------------------------*/

static int rec_tab (ECLAT *eclat, TIDLIST **lists, ITEM k, size_t x)
{                               /* --- eclat recursion with table */
  int     r;                    /* error status */
  ITEM    i, m, z;              /* loop variables */
  SUPP    pex;                  /* minimum support for perfect exts. */
  TIDLIST *l, *d;               /* to traverse transaction id lists */
  TIDLIST **proj = NULL;        /* trans. id lists of proj. database */
  ITEM    *t;                   /* to collect the tail items */

  assert(eclat && lists && (k > 0)); /* check the function arguments */
  #ifdef ECL_ABORT              /* if to check for interrupt */
  if (sig_aborted()) return -1; /* if execution was aborted, */
  #endif                        /* abort the recursion */
  if (eclat->mode & ECL_TAIL) { /* if to use tail to prune w/ repo. */
    t = isr_buf(eclat->report); /* collect the tail items in buffer */
    for (m = 0, i = k; --i >= 0; ) t[m++] = lists[i]->item;
    r = isr_tail(eclat->report, t, m);
    if (r) return r;            /* if tail need not be processed, */
  }                             /* abort the recursion */
  if ((k > 1)                   /* if there is more than one item and */
  &&  isr_xable(eclat->report, 2)) {     /* another item can be added */
    proj = (TIDLIST**)malloc((size_t)k *sizeof(TIDLIST*) +x);
    if (!proj) return -1;       /* allocate list and element arrays */
  }                             /* (memory for projected database) */
  if ((k > 4)                   /* if there are enough items left, */
  &&  (eclat->mode & ECL_REORDER)) /* re-sort items w.r.t. support */
    ptr_qsort(lists, (size_t)k, +1, tid_cmp, NULL);
  if (eclat->dir > 0) { z =  k; k  = 0; }
  else                { z = -1; k -= 1; }
  for (r = 0; k != z; k += eclat->dir) {
    l = lists[k];               /* traverse the items / tid lists */
    r = isr_add(eclat->report, l->item, l->supp);
    if (r <  0) break;          /* add current item to the reporter */
    if (r <= 0) continue;       /* check if item needs processing */
    if (proj && (k > 0)) {      /* if another item can be added */
      pex = (eclat->mode & ECL_PERFECT) ? l->supp : SUPP_MAX;
      proj[m = 0] = d = (TIDLIST*)(proj +k+1);
      for (i = 0; i < k; i++) { /* traverse the preceding lists */
        x = (size_t)filter(d, lists[i], eclat->tab[l->item]);
        if (d->supp < eclat->supp)  /* filter transaction id list */
          continue;             /* and eliminate infrequent items */
        if (d->supp >= pex) {   /* collect perfect extensions */
          isr_addpex(eclat->report, d->item); continue; }
        proj[++m] = d = (TIDLIST*)(d->tids +x);
      }                         /* collect tid lists of freq. items */
      if (m > 0) {              /* if the projection is not empty */
        r = rec_tab(eclat, proj, m, DIFFSIZE(d, proj[0]));
        if (r < 0) break;       /* recursively find freq. item sets */
      }                         /* in the created projection */
    }
    r = isr_reportx(eclat->report, l->tids, (TID)-l->supp);
    if (r < 0) break;           /* report the current item set */
    isr_remove(eclat->report,1);/* remove the current item */
  }                             /* from the item set reporter */
  if (proj) free(proj);         /* delete the list and element arrays */
  return r;                     /* return the error status */
}  /* rec_tab() */

/*--------------------------------------------------------------------*/

int eclat_tab (ECLAT *eclat)
{                               /* --- eclat with occurrence table */
  int        r = 0;             /* result of recursion/error status */
  ITEM       i, k, m;           /* loop variable, number of items */
  TID        n;                 /* number of transactions */
  size_t     x;                 /* number of item instances */
  SUPP       w;                 /* weight/support buffer */
  SUPP       max;               /* maximum support of an item */
  SUPP       pex;               /* minimum support for perfect exts. */
  SUPP       *d;                /* to traverse occurrence table rows */
  TRACT      *t;                /* to traverse transactions */
  TIDLIST    **lists, *l;       /* to traverse transaction id lists */
  TID        *tids, *p, **next; /* to traverse transaction ids */
  const ITEM *s;                /* to traverse transaction items */
  const TID  *c;                /* item occurrence counters */

  assert(eclat);                /* check the function arguments */
  eclat->dir = (eclat->target & (ISR_CLOSED|ISR_MAXIMAL)) ? -1 : +1;
  pex = tbg_wgt(eclat->tabag);  /* get the item processing direction */
  if (eclat->supp > pex)        /* and support for perfect exts. and */
    return 0;                   /* check the total transaction weight */
  if (!(eclat->mode & ECL_PERFECT)) pex = SUPP_MAX;
  n = tbg_cnt(eclat->tabag);    /* get the number of transactions */
  k = tbg_itemcnt(eclat->tabag);/* and check the number of items */
  if (k <= 0) return isr_report(eclat->report);
  x = tbg_extent(eclat->tabag); /* get the number of item instances */
  c = tbg_icnts(eclat->tabag,0);/* and the number of containing */
  if (!c) return -1;            /* transactions per item */
  if ((SIZE_MAX/sizeof(SUPP) -x) / (size_t)(n+4) < (size_t)k)
    return -1;                  /* check the table size */
  lists = (TIDLIST**)malloc((size_t) k              *sizeof(TIDLIST*)
                           +(size_t) k              *sizeof(TID*)
                           +(size_t) k              *sizeof(SUPP*)
                           +(size_t)(k+1)*(size_t)n *sizeof(SUPP));
  if (!lists) return -1;        /* create initial tid list array */
  next        = (TID**) (lists +k);    /* and split off arrays */
  eclat->tab  = (SUPP**)(next  +k);    /* get item occ. table header */
  eclat->muls = (SUPP*) (eclat->tab+k);/* split off the weight array */
  d = (SUPP*)memset(eclat->muls+n, 0, (size_t)k*(size_t)n*sizeof(SUPP));
  if (x < (size_t)n) x = (size_t)n; /* ensure enough transaction ids */
  p = tids = (TID*)malloc((size_t)k *sizeof(TIDLIST) +x *sizeof(TID));
  if (!p) { free(lists); return -1; }
  for (i = 0; i < k; i++) {     /* traverse the items / tid lists */
    eclat->tab[i] = d; d += n;  /* organize the table rows */
    lists[i] = l = (TIDLIST*)p; /* get/create the next trans. id list */
    l->item  = i;               /* initialize the list item */
    l->supp  = 0;               /* and the support counter */
    next[i]  = p = l->tids;     /* note position of next trans. id */
    p += c[i]; *p++ = (TID)-1;  /* skip space for transaction ids */
  }                             /* and store a sentinel at the end */
  while (n > 0) {               /* traverse the transactions */
    t = tbg_tract(eclat->tabag, --n);  /* get the next transaction */
    eclat->muls[n] = w = ta_wgt(t);    /* and store its weight */
    for (s = ta_items(t); *s > TA_END; s++) {
      eclat->tab[*s][n] = w;    /* traverse the transaction's items */
      lists[*s]->supp  += w;    /* and set the item occurrence flags */
      *next[*s]++      = n;     /* sum the transaction weight and */
    }                           /* collect the transaction ids */
  }
  max = 0;                      /* init. the maximal item support */
  for (i = m = 0; i < k; i++) { /* traverse the items / tid lists */
    l = lists[i];               /* eliminate all infrequent items and */
    if (l->supp <  eclat->supp) continue;    /* collect perfect exts. */
    if (l->supp >= pex) { isr_addpex(eclat->report, i); continue; }
    if (l->supp >  max)         /* find the maximal item support */
      max = l->supp;            /* (for later closed/maximal check) */
    lists[m++] = l;             /* collect lists for frequent items */
  }                             /* (eliminate infrequent items) */
  if (m > 0)                    /* find freq. item sets recursively */
    r = rec_tab(eclat, lists, m, DIFFSIZE(p, tids));
  if (r >= 0) {                 /* if no error occurred */
    i = eclat->target & (ISR_CLOSED|ISR_MAXIMAL);
    w = (i & ISR_MAXIMAL) ? eclat->supp : tbg_wgt(eclat->tabag);
    if (!i || (max < w)) {      /* if to report the empty set */
      if (!isr_tidfile(eclat->report)) /* if not to report tr. ids, */
        r = isr_report(eclat->report); /* report the empty item set */
      else {                    /* if to report transaction ids */
        for (n = tbg_cnt(eclat->tabag); n > 0; n--) tids[n] = n;
        r = isr_reportx(eclat->report, tids, (TID)n);
      }                         /* report the empty item set */
    }                           /* with all transaction ids */
  }
  free(tids); free(lists);      /* delete the allocated arrays */
  return r;                     /* return the error status */
}  /* eclat_tab() */

/*----------------------------------------------------------------------
  Eclat with an Occurrence Indicator Table (Simplified)
----------------------------------------------------------------------*/

static int rec_simp (ECLAT *eclat, TID *tids, SUPP n, ITEM k)
{                               /* --- eclat recursion (table based) */
  int  r;                       /* error status */
  ITEM z;                       /* loop variable */
  SUPP s, w;                    /* item set support, weight buffer */
  SUPP pex;                     /* trans. count for perfect exts. */
  TID  *dst, *d, *p;            /* to traverse transaction ids */
  SUPP *row;                    /* to traverse occurrence table rows */

  assert(eclat                  /* check the function arguments */
  &&     tids && (n > 0) && (k > 0));
  #ifdef ECL_ABORT              /* if to check for interrupt */
  if (sig_aborted()) return -1; /* if execution was aborted, */
  #endif                        /* abort the recursion */
  pex = (eclat->mode & ECL_PERFECT) ? n : SUPP_MAX;
  dst = tids +(TID)n+1;         /* get destination for intersections */
  if (eclat->dir > 0) { z =  k; k  = 0; }
  else                { z = -1; k -= 1; }
  for (r = 0; k != z; k += eclat->dir) { /* traverse remaining items */
    row = eclat->tab[k]; s = 0; /* filter tids with item's table row */
    for (d = dst, p = tids; *p >= 0; p++)
      if ((w = row[*p]) > 0) {  /* compute the item set support */
        s += w; *d++ = *p; }    /* and the reduced trans. id list */
    if (s < eclat->supp) continue;    /* skip infrequent items and */
    if ((w = (SUPP)(d-dst)) >= pex) { /* collect perfect extensions */
      isr_addpex(eclat->report, k); continue; }
    *d = -1;                    /* store a sentinel at the list end */
    r  = isr_add(eclat->report, k, s);
    if (r <  0) break;          /* add current item to the reporter */
    if (r <= 0) continue;       /* check if item needs processing */
    if ((k > 0)                 /* if another item can be added and */
    &&  isr_xable(eclat->report, 1) /* upper size limit not reached */
    &&  ((r = rec_simp(eclat, dst, w, k)) < 0))
      break;                    /* recursively find freq. item sets */
    r = isr_reportx(eclat->report, tids, (TID)-s);
    if (r < 0) break;           /* report the current item set */
    isr_remove(eclat->report,1);/* remove the current item */
  }                             /* from the item set reporter */
  return r;                     /* return the error status */
}  /* rec_simp() */

/*----------------------------------------------------------------------
Note that no memory is allocated in the above function; all processing
is done in the single memory block that is allocated in the function
below. The size of this memory block is O(n*k), where n is the number
of items and k the number of transactions. Additional memory is only
allocated in the item set reporter if closed or maximal item sets are
to be found, since this requires setting up an item set repository.
----------------------------------------------------------------------*/

int eclat_simp (ECLAT *eclat)
{                               /* --- eclat with occurrence table */
  int        r = 0;             /* result of recursion/error status */
  ITEM       i, k;              /* loop variable, number of items */
  TID        n, m;              /* number of transactions */
  size_t     x;                 /* number of item instances */
  SUPP       w;                 /* weight/support buffer */
  TID        *tids;             /* transaction identifier array */
  SUPP       *p;                /* to traverse occurrence table rows */
  const ITEM *s;                /* to traverse transaction items */
  TRACT      *t;                /* to traverse the transactions */

  assert(eclat);                /* check the function arguments */
  eclat->dir = -1;              /* supports only downward currently */
  if (tbg_wgt(eclat->tabag) < eclat->supp)
    return 0;                   /* check the total transaction weight */
  n = tbg_cnt(eclat->tabag);    /* get the number of transactions */
  k = tbg_itemcnt(eclat->tabag);/* and check the number of items */
  if (k <= 0) return isr_report(eclat->report);
  x = tbg_extent(eclat->tabag); /* get the number of item instances */
  if ((SIZE_MAX/sizeof(TID) -x-(size_t)n-1) / (size_t)(n+2) < (size_t)k)
    return -1;                  /* check the database/table size */
  x += (size_t)n+1+(size_t)k;   /* compute the database/table size */
  eclat->tab = (SUPP**)malloc((size_t)k           *sizeof(SUPP*)
                            + (size_t)k*(size_t)n *sizeof(SUPP)
                            +         x           *sizeof(TID));
  if (!eclat->tab) return -1;   /* allocate working memory */
  p = (SUPP*)memset(eclat->tab+k, 0, (size_t)k*(size_t)n*sizeof(SUPP));
  for (i = 0; i < k; i++) {     /* init and organize the table rows */
    eclat->tab[i] = p; p += n; }/* (one table row per item) */
  tids = (TID*)p;               /* get the transaction id array */
  for (m = 0; m < n; m++) {     /* traverse the transactions */
    tids[m] = m;                /* set the initial (full) tid set */
    t = tbg_tract(eclat->tabag, m); /* get the next transaction */
    w = ta_wgt(t);                  /* and note its weight */
    for (s = ta_items(t); *s > TA_END; s++)
      eclat->tab[*s][m] = w;    /* set the item occurrence flags */
  }                             /* (item *s occurs in transaction i) */
  tids[n] = (TID)-1;            /* store a sentinel at the end */
  if (isr_xable(eclat->report,1)) /* recursively find freq. item sets */
    r = rec_simp(eclat, tids, (SUPP)n, k);
  if (r >= 0)                   /* finally report the empty item set */
    r = isr_report(eclat->report);
  free(eclat->tab);             /* delete the allocated table/arrays */
  return r;                     /* return the error status */
}  /* eclat_simp() */

/*----------------------------------------------------------------------
  Eclat with Transaction Ranges
----------------------------------------------------------------------*/

static void build_trg (TRGLIST **lists, TIDRANGE **next,
                       TABAG *tabag, TID min, TID max, ITEM off)
{                               /* --- build the trans. range lists */
  ITEM     i;                   /* loop variable */
  TID      k;                   /* loop variable */
  SUPP     w;                   /* weight buffer */
  ITEM     item;                /* to traverse items at offset */
  TRGLIST  *l;                  /* to access the trans. range lists */
  TIDRANGE *r;                  /* to access the transaction ranges */
  TRACT    *t;                  /* to traverse the transactions */

  assert(lists && tabag         /* check the function arguments */
  &&    (min >= 0) && (max < (TID)tbg_cnt(tabag)) && (off >= 0));

  /* --- skip short transactions --- */
  while ((min <= max)           /* traverse the transactions */
  &&     (ta_items(tbg_tract(tabag, min))[off] <= TA_END))
    ++min;                      /* skip trans. that are too short */
  if (min > max) return;        /* check for an empty trans. range */

  /* --- handle packed items --- */
  if (off <= 0) {               /* if at first item in transactions */
    l = lists[0];               /* get the list for packed items */
    for (k = min; min <= max; min++) {
      t = tbg_tract(tabag,min); /* traverse the transactions */
      i = ta_items(t)[off];     /* get the first item from them */
      if (i >= 0) break;        /* if it is not packed, abort loop */
      r = next[0]++;            /* get the current range in list */
      r->min = min;             /* store the transaction id and */
      r->max = (TID)(BITTA)i;   /* the bit repr. of the items */
      l->supp += r->wgt = ta_wgt(t);
    }                           /* store and sum transaction weight */
    if (min > k) {              /* if the trans. range is not empty */
      build_trg(lists, next, tabag, k, min-1, off+1);
      if (min > max) return;    /* recursively build trans. ranges, */
    }                           /* check whether an empty range */
  }                             /* is left to be processed */

  /* --- handle normal items --- */
  t = tbg_tract(tabag, min);    /* get the first transaction */
  i = item = ta_items(t)[off];  /* and from it the first item */
  do {                          /* traverse the longer transactions */
    w = ta_wgt(t);              /* init. the transaction weight */
    for (k = min; ++min <= max; ) {  /* while not at end of section */
      t = tbg_tract(tabag,min); /* get the next transaction and */
      i = ta_items(t)[off];     /* from it the item at the offset */
      if (i != item) break;     /* if the item differs, abort loop */
      w += ta_wgt(t);           /* otherwise sum the trans. weight */
    }                           /* (collect trans. with same item) */
    l = lists[item];            /* get list for the current item */
    r = next[item]++; item = i; /* and create a new trans. id range */
    l->supp += r->wgt = w;      /* store the transaction weights */
    build_trg(lists, next, tabag, r->min = k, r->max = min-1, off+1);
  } while (min <= max);         /* create the children recursively */
}  /* build_trg() */            /* while the range is not empty */

/*--------------------------------------------------------------------*/

static TID isect_trg (TRGLIST *dst, TRGLIST *src1, TRGLIST *src2)
{                               /* --- intersect two range lists */
  TIDRANGE *s1, *s2, *d, *p;    /* to traverse sources and dest. */

  assert(dst && src1 && src2);  /* check the function arguments */
  dst->item = src1->item;       /* copy the first item and */
  dst->supp = 0;                /* initialize the support */
  s1 = src1->trgs; s2 = src2->trgs; d = dst->trgs-1; p = NULL;
  while (1) {                   /* range list intersection loop */
    if      (s1->max < s2->min) {    /* skip transaction ranges */
      if ((++s1)->min < 0) break; }  /* that do not overlap */
    else if (s2->max < s1->min) {    /* check for the sentinel */
      if ((++s2)->min < 0) break; }  /* after advancing the range */
    else {                      /* if the transaction ranges overlap */
      if (s1 == p)              /* if there was a previous overlap, */
        d->wgt += s2->wgt;      /* only add the additional support */
      else {                    /* if this is a new overlap, */
        p = s1; ++d;            /* get/create a new trans. range */
        d->min = s1->min;       /* note the minimum and the */
        d->max = s1->max;       /* maximum transaction identifier */
        d->wgt = s2->wgt;       /* and the corresponding support */
      }
      dst->supp += s2->wgt;     /* sum the support for the item */
      if ((++s2)->min < 0) break;
    }                           /* skip the processed trans. range */
  }                             /* and check for the sentinel */
  (++d)->min = -1;              /* store a sentinel at the list end */
  return (TID)(++d -dst->trgs); /* return the size of the new list */
}  /* isect_trg() */

/*--------------------------------------------------------------------*/

static TID filter_trg (TRGLIST *dst, TRGLIST *src1, TRGLIST *src2)
{                               /* --- filter tids with a range list */
  TIDRANGE *s1, *s2, *d;        /* to traverse sources and dest. */

  assert(dst && src1 && src2);  /* check the function arguments */
  dst->item = src1->item;       /* copy the first item and */
  dst->supp = 0;                /* initialize the support */
  s1 = src1->trgs; s2 = src2->trgs; d = dst->trgs-1;
  while (1) {                   /* transaction id filtering loop */
    if      (s1->min < s2->min) {    /* skip transaction ids */
      if ((++s1)->min < 0) break; }  /* that are not in range */
    else if (s1->min > s2->max) {    /* check for the sentinel */
      if ((++s2)->min < 0) break; }  /* after advancing the range */
    else {                      /* if transaction id is in range */
      *++d = *s1;               /* copy the entry to the dest. */
      dst->supp += s1->wgt;     /* sum the support for the item */
      if ((++s1)->min < 0) break;
    }                           /* check for the sentinel */
  }
  (++d)->min = -1;              /* store a sentinel at the list end */
  return (TID)(++d -dst->trgs); /* return the size of the new list */
}  /* filter_trg() */

/*--------------------------------------------------------------------*/

static int rec_trg (ECLAT *eclat, TRGLIST **lists, ITEM k, size_t x)
{                               /* --- eclat recursion with ranges */
  int      r;                   /* error status */
  ITEM     i, m, z;             /* loop variables */
  SUPP     pex;                 /* minimum support for perfect exts. */
  TRGLIST  **proj = NULL;       /* range lists of projected database */
  TRGLIST  *l, *d;              /* to traverse trans. range lists */
  TIDRANGE *p;                  /* to traverse transaction ranges */
  ITEM     *t;                  /* to collect the tail items */

  assert(eclat && lists && (k > 0)); /* check the function arguments */
  #ifdef ECL_ABORT              /* if to check for interrupt */
  if (sig_aborted()) return -1; /* if execution was aborted, */
  #endif                        /* abort the recursion */
  if (eclat->mode & ECL_TAIL) { /* if to use tail to prune w/ repo. */
    t = isr_buf(eclat->report); /* collect the tail items in buffer */
    for (m = 0, i = k; --i > 0; )
      t[m++] = lists[i]->item;  /* collect the tail items */
    if (lists[0]->item >= 0)    /* if there are no packed items, */
      t[m++] = lists[0]->item;  /* simply copy the last item */
    else {                      /* if there are packed items */
      for (z = 16; --z >= 0; )  /* traverse the bits */
        if (lists[0]->item & (1 << z))
          t[m++] = z;           /* collect the packed tail items */
    }                           /* after the other tail items */
    r = isr_tail(eclat->report, t, m);
    if (r) return r;            /* if tail need not be processed, */
  }                             /* abort the recursion */
  if ((k > 1)                   /* if there is more than one item and */
  &&  isr_xable(eclat->report, 2)) {     /* another item can be added */
    proj = (TRGLIST**)malloc((size_t)k *sizeof(TRGLIST*) +x);
    if (!proj) return -1;       /* allocate list and element arrays */
  }                             /* (memory for projected database) */
  if (eclat->dir > 0) { z =  k; k  = 0; }
  else                { z = -1; k -= 1; }
  for (r = 0; k != z; k += eclat->dir) {
    l = lists[k];               /* traverse the items / range lists */
    if (l->item < 0) {          /* if this list is for packed items */
      for (p = l->trgs; p->min >= 0; p++)
        m16_add(eclat->fim16, (BITTA)p->max, p->wgt);
      r = m16_mine(eclat->fim16);    /* add bit-rep. trans. prefixes */
      if (r >= 0) continue;     /* to the 16-items machine and mine, */
      if (proj) free(proj);     /* then go to the next tid range list */
      return r;                 /* otherwise free allocated memory */
    }                           /* and abort the function */
    r = isr_add(eclat->report, l->item, l->supp);
    if (r <  0) break;          /* add current item to the reporter */
    if (r <= 0) continue;       /* check if item needs processing */
    if (proj && (k > 0)) {      /* if another item can be added */
      pex = (eclat->mode & ECL_PERFECT) ? l->supp : SUPP_MAX;
      proj[m = 0] = d = (TRGLIST*)(proj +k+1);
      if (lists[i = 0]->item < 0) { /* if there are packed items */
        x = (size_t)filter_trg(d, lists[i++], l);
        if (d->supp >= eclat->supp) /* if they are frequent */
          proj[++m] = d = (TRGLIST*)(d->trgs +x);
      }                         /* add a range list for packed items */
      for ( ; i < k; i++) {     /* traverse the preceding lists */
        x = (size_t)isect_trg(d, lists[i], l);
        if (d->supp < eclat->supp)   /* intersect tid range lists */
          continue;             /* and eliminate infrequent items */
        if (d->supp >= pex) {   /* collect perfect extensions */
          isr_addpex(eclat->report, d->item); continue; }
        proj[++m] = d = (TRGLIST*)(d->trgs +x);
      }                         /* collect the trans. range lists */
      if (m > 0) {              /* if the projection is not empty */
        r = rec_trg(eclat, proj, m, DIFFSIZE(d, proj[0]));
        if (r < 0) break;       /* recursively find freq. item sets */
      }                         /* in the created projection */
    }
    r = isr_report(eclat->report);
    if (r < 0) break;           /* report the current item set */
    isr_remove(eclat->report,1);/* remove the current item */
  }                             /* from the item set reporter */
  if (proj) free(proj);         /* delete the list and element arrays */
  return r;                     /* return the error status */
}  /* rec_trg() */

/*--------------------------------------------------------------------*/

int eclat_trg (ECLAT *eclat)
{                               /* --- eclat with transaction ranges */
  int       r = 0;              /* result of recursion/error status */
  ITEM      i, k, m;            /* loop variable, number of items */
  TID       n;                  /* number of transactions */
  size_t    x;                  /* number of item instances */
  SUPP      pex;                /* minimum support for perfect exts. */
  TRGLIST   **lists, *l;        /* to traverse trans. range lists */
  TIDRANGE  *trgs, *p, **next;  /* to traverse transaction ranges */
  const TID *c;                 /* item occurrence counters */

  assert(eclat);                /* check the function arguments */
  eclat->dir = (eclat->target & (ISR_CLOSED|ISR_MAXIMAL)) ? -1 : +1;
  pex = tbg_wgt(eclat->tabag);  /* get the item processing direction */
  if (eclat->supp > pex)        /* and support for perfect exts. and */
    return 0;                   /* check the total transaction weight */
  if (!(eclat->mode & ECL_PERFECT)) pex = SUPP_MAX;
  n = tbg_cnt(eclat->tabag);    /* get the number of transactions */
  k = tbg_itemcnt(eclat->tabag);/* and check the number of items */
  if (k <= 0) return isr_report(eclat->report);
  c = tbg_icnts(eclat->tabag,0);/* get the number of containing */
  if (!c) return -1;            /* transactions per item */
  lists = (TRGLIST**)malloc((size_t)k *sizeof(TRGLIST*)
                           +(size_t)k *sizeof(TIDRANGE*));
  if (!lists) return -1;        /* create initial range lists array */
  next = (TIDRANGE**)(lists+k); /* and split off next range array */
  for (x = 0, i = 0; i < k; i++)/* traverse the items and sum */
    x += (size_t)c[i];          /* the number of item occurrences */
  /* Do not use tbg_extent(), because it does not take packed items */
  /* properly into account and thus may yield too big a value.      */
  p = trgs = (TIDRANGE*)malloc((size_t)k *sizeof(TRGLIST)
                                      +x *sizeof(TIDRANGE));
  if (!p) { free(lists); return -1; }
  for (i = 0; i < k; i++) {     /* allocate range list elements */
    lists[i] = l = (TRGLIST*)p; /* and organize the range lists */
    l->item  = i;               /* initialize the list item */
    l->supp  = 0;               /* and the support counter */
    next[i]  = p = l->trgs;     /* note position of next trans. id */
    p += c[i];                  /* skip space for transaction ids */
    (p++)->min = (TID)-1;       /* and store a sentinel at the end */
  }
  build_trg(lists, next, eclat->tabag, 0, (TID)(n-1), 0);
  eclat->fim16 = NULL;          /* build the transaction ranges */
  l = lists[i = 0];             /* get the list for packed items */
  if ((l->supp >= eclat->supp)  /* if there are packed items and */
  &&  (eclat->mode & ECL_FIM16)) {  /* to use a 16-items machine */
    eclat->fim16 = m16_create(eclat->dir, eclat->supp, eclat->report);
    if (!eclat->fim16) { free(trgs); free(lists); return -1; }
    next[i++]->min = (TID)-1;   /* store a sentinel at the list end */
    l->item        = -1;        /* mark list for the packed items */
  }                             /* and store it in the reduced array */
  for (m = i; i < k; i++) {     /* traverse the trans. range lists */
    l = lists[i];               /* eliminate all infrequent items and */
    if (l->supp <  eclat->supp) continue;    /* collect perfect exts. */
    if (l->supp >= pex) { isr_addpex(eclat->report, i); continue; }
    next[i]->min = (TID)-1;     /* store a sentinel at the list end */
    lists[m++] = l;             /* collect lists for frequent items */
  }                             /* (eliminate infrequent items) */
  if (m > 0)                    /* find freq. items sets recursively */
    r = rec_trg(eclat, lists, m, DIFFSIZE(p, trgs));
  if (r >= 0)                   /* finally report the empty item set */
    r = isr_report(eclat->report);
  if (eclat->fim16)             /* if a 16-items machine was used, */
    m16_delete(eclat->fim16);   /* delete the 16-items machine */
  free(trgs); free(lists);      /* delete the allocated arrays */
  return r;                     /* return the error status */
}  /* eclat_trg() */

/*----------------------------------------------------------------------
  Eclat with Occurrence Deliver (LCM-style)
----------------------------------------------------------------------*/

static int odclo (ECLAT *eclat, TALIST *list, ITEM item)
{                               /* --- check for a closed item set */
  TID        k;                 /* loop variable */
  const ITEM *p, *q;            /* to traverse transaction items */
  ITEM       *s, *d;            /* to traverse collected items */
  int        i, m;              /* loop variable, bit mask for items */

  assert(eclat && list          /* check the function arguments */
  &&    (item >= 0) && (list->cnt >= 1));
  if (item < 31) {              /* if item bits can be used */
    for (m = 0, i = item; ++i < 32; )
      if (!isr_uses(eclat->report, (ITEM)i))
        m |= 1 << i;            /* collect bits of unused items */
    for (k = list->cnt; m && (--k >= 0); )
      m &= ta_getmark(list->tracts[k]);
    if (m) return 0;            /* if perf. ext found, 'not closed' */
    item = 31;                  /* otherwise check remaining items */
  }                             /* (check only items 32 and above) */
  p = ta_items(list->tracts[0]);/* get first and last transaction */
  q = ta_items(list->tracts[list->cnt-1]);
  while ((UITEM)*p <= (UITEM)item) p++;  /* find item added last */
  while ((UITEM)*q <= (UITEM)item) q++;  /* in both transactions */
  for (d = eclat->cand; (*p >= 0) && (*q >= 0); ) {
    if      ((UITEM)*p < (UITEM)*q) p++; /* intersection loop: */
    else if ((UITEM)*p > (UITEM)*q) q++; /* skip smaller items */
    else { if (!isr_uses(eclat->report, *p)) *d++ = *p; p++; q++; }
  }                             /* collect perfect ext. candidates */
  if (d <= eclat->cand)         /* if the intersection is empty, */
    return -1;                  /* abort with 'item set is closed' */
  *d = TA_END;                  /* terminate the candidate list */
  for (k = list->cnt-1; --k > 0; ) {
    s = d = eclat->cand;        /* traverse the transactions */
    p = ta_items(list->tracts[k]);
    while (*p <= item) p++;     /* find the item added last */
    while (1) {                 /* item list intersection loop */
      if      (*s < 0) break;   /* check for the list sentinel */
      else if ((UITEM)*s < (UITEM)*p) s++;
      else if ((UITEM)*s > (UITEM)*p) p++;
      else { *d++ = *s++; p++; }/* skip smaller item identifier and */
    }                           /* collect perfect ext. candidates */
    if (d <= eclat->cand) return -1;
    *d = TA_END;                /* if intersection is empty, abort, */
  }                             /* otherwise store a sentinel */
  return 0;                     /* return 'item set is not closed' */
}  /* odclo() */

/*--------------------------------------------------------------------*/

static int odmax (ECLAT *eclat, TALIST *list, ITEM item)
{                               /* --- check for a maximal item set */
  TID        k;                 /* loop variable */
  ITEM       n;                 /* number of considered items */
  SUPP       w;                 /* transaction weight */
  TRACT      *t;                /* to traverse the transactions */
  const ITEM *p;                /* to traverse the items */

  assert(eclat && list          /* check the function arguments */
  &&    (item >= 0) && (list->cnt >= 1));
  for (n = tbg_itemcnt(eclat->tabag); --n > item; )
    eclat->miss[n] = (isr_uses(eclat->report, n))
                   ? list->supp+1 : eclat->supp;
  for (k = list->cnt; --k >= 0; ) {
    t = list->tracts[k];        /* traverse the transactions */
    w = ta_wgt(t);              /* and the items and each trans. */
    for (p = ta_items(t); *p != TA_END; p++)
      if ((*p > item) && ((eclat->miss[*p] -= w) <= 0))
        return 0;               /* count support of candidate exts.; */
  }                             /* if frequent cand. found, abort */
  return -1;                    /* return 'item set is maximal' */
}  /* odmax() */

/*--------------------------------------------------------------------*/

static SUPP rec_odcm (ECLAT *eclat, TALIST **lists, ITEM k)
{                               /* --- occ. deliver closed/maximal */
  int        r;                 /* error status */
  ITEM       i, m;              /* loop variables */
  TID        n;                 /* loop variable for transactions */
  SUPP       w;                 /* weight/support buffer */
  SUPP       max;               /* maximum support of an ext. item */
  SUPP       pex;               /* minimum support for perfect exts. */
  TALIST     *l, *p;            /* to traverse transaction lists */
  TRACT      *t;                /* to traverse transactions */
  const ITEM *s;                /* to traverse items */

  assert(eclat && lists && (k > 0)); /* check the function arguments */
  #ifdef ECL_ABORT              /* if to check for interrupt */
  if (sig_aborted()) return -1; /* if execution was aborted, */
  #endif                        /* abort the recursion */
  l = lists[k]; m = 0;          /* get the trans. list to process */
  for (n = 0; n < l->cnt; n++){ /* traverse the transactions */
    t = l->tracts[n];           /* get the next transaction */
    w = ta_wgt(t);              /* and traverse its items */
    for (s = ta_items(t); (UITEM)*s < (UITEM)k; s++) {
      p = lists[*s]; p->supp += w; p->tracts[p->cnt++] = t; }
  }                             /* deliver the item occurrences */
  pex = (eclat->mode & ECL_PERFECT) ? l->supp : SUPP_MAX;
  for (i = 0; i < k; i++) {     /* traverse the items / trans. lists */
    p = lists[i];               /* get the next transaction list */
    if (p->supp <  eclat->supp){/* eliminate infrequent items */
      p->supp = 0; p->cnt = 0; continue; }
    if (p->supp >= pex) {       /* collect perfect extension items */
      p->supp = 0; p->cnt = 0; isr_addpex(eclat->report, i); continue; }
    m++;                        /* count the frequent items */
  }                             /* (to see whether loop is needed) */
  if (m <= 0) return 0;         /* if no frequent items found, abort */
  m = isr_xable(eclat->report, 2) ? 0 : ITEM_MAX;
  for (max = w = 0, i = 0; i < k; i++) {
    p = lists[i];               /* traverse the items / trans. lists, */
    if (p->supp <= 0) continue; /* but skip all eliminated items */
    if (p->supp > max)          /* find maximal extension support */
      max = p->supp;            /* (for later closed/maximal check) */
    if (odclo(eclat, p, i)) {   /* if the current set may be closed */
      r = isr_add(eclat->report, i, p->supp);
      if (r < 0) break;         /* add current item to the reporter */
      w = 0;                    /* default: no perfect extension */
      if (i > m) {              /* if to compute a projection */
        w = rec_odcm(eclat, lists, i);
        if (w < 0) { r = -1; break; }
      }                         /* recursively find freq. item sets */
      if ((eclat->target & ISR_CLOSED) ? (w < p->supp)
      :   ((w < eclat->supp) && odmax(eclat, p, i))) {
        r = isr_report(eclat->report);
        if (r < 0) break;       /* if current set is closed/maximal, */
      }                         /* report the current item set */
      isr_remove(eclat->report,1); /* remove the current item */
    }                           /* from the item set reporter */
    p->supp = 0; p->cnt = 0;    /* reinitialize the transaction list */
  }
  return (r < 0) ? (SUPP)r : max; /* return error status/max. support */
}  /* rec_odcm() */

/*--------------------------------------------------------------------*/

static int rec_odfx (ECLAT *eclat, TALIST **lists, ITEM k)
{                               /* --- occ. deliver w/o reordering */
  int        r;                 /* error status */
  ITEM       i, m;              /* loop variables */
  TID        n;                 /* loop variable for transactions */
  SUPP       w;                 /* weight/support buffer */
  SUPP       pex;               /* minimum support for perfect exts. */
  TALIST     *l, *p;            /* to traverse transaction lists */
  TRACT      *t;                /* to traverse transactions */
  const ITEM *s;                /* to traverse items */

  assert(eclat && lists && (k > 0)); /* check the function arguments */
  #ifdef ECL_ABORT              /* if to check for interrupt */
  if (sig_aborted()) return -1; /* if execution was aborted, */
  #endif                        /* abort the recursion */
  l = lists[k];                 /* collate equal transactions */
  taa_collate(l->tracts, l->cnt, k);
  for (n = 0; n < l->cnt; n++){ /* traverse the transactions, */
    t = l->tracts[n];           /* but skip collated transactions */
    if ((w = ta_wgt(t)) <= 0) continue;
    s = ta_items(t);            /* if there are packed items, */
    if (ispacked(*s))           /* add them to the 16-items machine */
      m16_add(eclat->fim16, (BITTA)*s++, w);
    for ( ; (UITEM)*s < (UITEM)k; s++) {
      p = lists[*s]; p->supp += w; p->tracts[p->cnt++] = t; }
  }                             /* deliver the item occurrences */
  pex = (eclat->mode & ECL_PERFECT) ? l->supp : SUPP_MAX;
  for (m = 0, i = eclat->first; i < k; i++) {
    p = lists[i];               /* traverse the items / trans. lists */
    if (p->supp <  eclat->supp){/* eliminate infrequent items */
      p->supp = 0; p->cnt = 0; continue; }
    if (p->supp >= pex) {       /* collect perfect extension items */
      p->supp = 0; p->cnt = 0; isr_addpex(eclat->report, i); continue; }
    m++;                        /* count the frequent items */
  }                             /* (to see whether loop is needed) */
  r = (eclat->fim16)            /* if there is a 16-items machine, */
    ? m16_mine(eclat->fim16) : 0;  /* execute the 16-items machine */
  if (m <= 0) {                 /* if no frequent items found, abort */
    taa_uncoll(l->tracts, l->cnt); return r; }
  m = isr_xable(eclat->report, 2) ? 0 : ITEM_MAX;
  for (i = eclat->first; i < k; i++) {
    p = lists[i];               /* traverse the items / trans. lists, */
    if (p->supp <= 0) continue; /* but skip all eliminated items */
    r = isr_add(eclat->report, i, p->supp);
    if (r < 0) break;           /* add current item to the reporter */
    if (r > 0) {                /* if the item needs processing */
      if (i > m) {              /* if to compute a projection */
        r = rec_odfx(eclat, lists, i);
        if (r < 0) break;       /* recursively find freq. item sets */
      }                         /* and check for a recursion error */
      r = isr_report(eclat->report);
      if (r < 0) break;         /* report the current item set */
      isr_remove(eclat->report, 1); /* remove the current item */
    }                           /* from the item set reporter */
    p->supp = 0; p->cnt = 0;    /* reinitialize the transaction list */
  }
  taa_uncoll(l->tracts,l->cnt); /* uncollate the transactions */
  return r;                     /* and return the error status */
}  /* rec_odfx() */

/*--------------------------------------------------------------------*/

static int rec_odro (ECLAT *eclat, TALIST **lists, ITEM k)
{                               /* --- occ. deliver with reordering */
  int        r;                 /* error status */
  ITEM       i, m, b;           /* loop variables */
  TID        n;                 /* number of transactions */
  size_t     x;                 /* number of item instances */
  SUPP       w;                 /* weight/support buffer */
  SUPP       pex;               /* minimum support for perfect exts. */
  TALIST     *l, *p;            /* to traverse transaction lists */
  TRACT      *t;                /* to traverse transactions */
  const ITEM *s;                /* to traverse items */
  SUPP       *supp;             /* item support array */
  ITEM       *map, *inv;        /* item identifier maps */
  TALIST     **dst;             /* destination for reduction */
  void       *mem = NULL;       /* memory for reduction */

  assert(eclat && lists && (k > 0)); /* check the function arguments */
  #ifdef ECL_ABORT              /* if to check for interrupt */
  if (sig_aborted()) return -1; /* if execution was aborted, */
  #endif                        /* abort the recursion */
  supp = (SUPP*)memset(eclat->muls, 0, (size_t)k *sizeof(SUPP));
  l    = lists[k];              /* initialize the item support array */
  for (x = 0, n = 0; n < l->cnt; n++) {
    t = l->tracts[n];           /* traverse the transactions */
    w = ta_wgt(t);              /* get the transaction weight */
    for (s = ta_items(t); (UITEM)*s < (UITEM)k; s++)
      supp[*s] += w;            /* determine the support of the items */
    x += (size_t)(s -ta_items(t));
  }                             /* compute the size of the database */
  pex = (eclat->mode & ECL_PERFECT) ? l->supp : SUPP_MAX;
  for (inv = eclat->cand, i = m = 0; i < k; i++) {
    if (supp[i] <  eclat->supp){/* traverse items and their support */
      supp[i] = -1; continue; } /* eliminate infrequent items an */
    if (supp[i] >= pex) {       /* collect perfect extension items */
      supp[i] = -1; isr_addpex(eclat->report,lists[i]->item); continue;}
    inv[m++] = i;               /* collect the remaining items */
  }                             /* (map from new to old identifiers) */
  if (m <= 0) return 0;         /* if no frequent items found, abort */
  i = inv[m-1];                 /* get the highest frequent item */
  if (++i < k) k = i;           /* and compute limit for item loop */
  i2s_sort(inv, (size_t)m, -1, supp);
  map = inv+m;                  /* sort items by their support */
  if ((m <= 16) && eclat->fim16) { /* if at most 16-items left */
    for (i = 0; i < k; i++) map[i] = -1;
    for (i = 0; i < m; i++) {   /* build a map from the old */
      map[inv[i]] = i;          /* to the new item identifiers */
      m16_setmap(eclat->fim16, i, lists[inv[i]]->item);
    }                           /* set the item identifier map */
    for (n = 0; n < l->cnt; n++) {
      t = l->tracts[n]; b = 0;  /* traverse the transactions */
      for (s = ta_items(t); (UITEM)*s < (UITEM)k; s++)
        if ((i = map[*s]) >= 0) b |= 1 << i;
      m16_add(eclat->fim16, (BITTA)b, ta_wgt(t));
    }                           /* add bit-represented transactions */
    return m16_mine(eclat->fim16);   /* to the 16-items machine and */
  }                                  /* mine frequent item sets */
  for (i = 0; i < k; i++) {     /* copy support to trans. lists */
    if (supp[i] > 0) lists[i]->supp = supp[i];
    else           { lists[i]->supp = 0; map[i] = -1; }
  }                             /* set map for eliminated items */
  dst = lists;                  /* get the trans. lists to process */
  if ((l->cnt >= 6)             /* if there are enough transactions */
  &&  (m >= ((eclat->fim16) ? 20 : 6))) {  /* and enough items left */
    dst = (TALIST**)malloc((size_t)m *sizeof(TALIST*));
    if (!dst) return -1;        /* allocate memory for projection */
    for (i = 0; i < m; i++) {   /* copy the transaction lists that */
      dst[i] = lists[inv[i]];   /* are needed for the projection */
      map[inv[i]] = i;          /* and build a map from the old */
    }                           /* to the new item identifiers */
    mem = malloc(taa_dstsize(l->cnt, x)); /* allocate memory for */
    if (!mem) { free(dst); return -1; }   /* destination trans. */
    l->cnt = taa_reduce(l->tracts, l->cnt, k, map, eclat->hash, &mem);
    k = m;                      /* reduce the transactions */
  }                             /* (remove items, collate trans.) */
  if (eclat->fim16              /* if to use a 16-items machine */
  && (eclat->dir > 0)) {        /* and forward processing direction */
    for (n = 0; n < l->cnt; n++) {
      t = l->tracts[n]; b = 0;  /* traverse the transactions */
      for (s = ta_items(t); (UITEM)*s < (UITEM)16; s++)
        b |= 1 << *s;           /* add trans. to 16-items machine */
      m16_add(eclat->fim16, (BITTA)b, ta_wgt(t));
      for ( ; (UITEM)*s < (UITEM)k; s++) {
        p = dst[*s]; p->tracts[p->cnt++] = t; }
    }                           /* deliver the item occurrences */
    for (i = 0; i < 16; i++) {  /* traverse the first 16 items */
      l = dst[i]; l->supp = 0; l->cnt = 0; /* and clear support */
      m16_setmap(eclat->fim16, i, l->item);
    }                           /* set the item identifier map */
    r = m16_mine(eclat->fim16); /* mine with 16-items machine */
    if (r < 0) return r; }      /* and check for an error */
  else {                        /* if not to use a 16-items machine */
    for (n = 0; n < l->cnt; n++) {
      t = l->tracts[n];         /* traverse the transactions */
      for (s = ta_items(t); (UITEM)*s < (UITEM)k; s++) {
        p = dst[*s]; p->tracts[p->cnt++] = t; }
    }                           /* deliver the item occurrences */
    i = 0;                      /* to the transaction lists and */
  }                             /* get first item index to process */
  m = isr_xable(eclat->report, 2) ? 0 : ITEM_MAX;
  for (r = 0; i < k; i++) {     /* traverse the items/trans. lists, */
    l = dst[i];                 /* but skip all eliminated items */
    if (l->supp <= 0) { l->cnt = 0; continue; }
    r = isr_add(eclat->report, l->item, l->supp);
    if (r < 0) break;           /* add current item to the reporter */
    if (r > 0) {                /* if the item needs processing */
      if (i > m) {              /* if to compute a projection */
        r = rec_odro(eclat, dst, i);
        if (r < 0) break;       /* recursively find freq. item sets */
      }                         /* and check for a recursion error */
      r = isr_report(eclat->report);
      if (r < 0) break;         /* report the current item set */
      isr_remove(eclat->report,1); /* remove the current item */
    }                           /* from the item set reporter */
    l->supp = 0; l->cnt = 0;    /* reinitialize the transaction list */
  }
  if (mem) {                    /* delete projection lists */
    free(mem); free(dst); }     /* and destination memory */
  return r;                     /* return the error status */
}  /* rec_odro() */

/*--------------------------------------------------------------------*/

int eclat_ocd (ECLAT *eclat)
{                               /* --- eclat with occurrence deliver */
  int       r = 0;              /* result of recursion/error status */
  ITEM      i, k;               /* loop variable, number of items */
  TID       n, m;               /* number of transactions */
  size_t    x, h;               /* extent, hash table size */
  SUPP      pex;                /* minimum support for perfect exts. */
  TALIST    **lists, *l;        /* to traverse transaction lists */
  TRACT     **tras, **p;        /* to traverse transactions */
  const TID *c;                 /* item occurrence counters */

  assert(eclat);                /* check the function arguments */
  eclat->dir = (eclat->target & (ISR_CLOSED|ISR_MAXIMAL)) ? -1 : +1;
  pex = tbg_wgt(eclat->tabag);  /* get the item processing direction */
  if (eclat->supp > pex)        /* and support for perfect exts. and */
    return 0;                   /* check the total transaction weight */
  if (!(eclat->mode & ECL_PERFECT)) pex = SUPP_MAX;
  n = tbg_cnt(eclat->tabag);    /* get the number of transactions */
  k = tbg_itemcnt(eclat->tabag);/* and check the number of items */
  if (k <= 0) return isr_report(eclat->report);
  c = tbg_icnts(eclat->tabag,0);/* get the number of containing */
  if (!c) return -1;            /* transactions per item */
  lists = (TALIST**)malloc((size_t)(k+1) *sizeof(TALIST*));
  if (!lists) return -1;        /* create the trans. id list array */
  for (x = 0, i = 0; i < k; i++)/* traverse the items and sum */
    x += (size_t)c[i];          /* the numbers of item occurrences */
  /* Do not use tbg_extent(), because it does not take packed items */
  /* properly into account and thus may yield too large a value.    */
  h = (size_t)taa_tabsize(n);   /* get the hash table size and */
  x = (size_t)n +x -(size_t)(k+1);        /* the database size */
  p = tras = (TRACT**)malloc((size_t)(k+1)   *sizeof(TALIST)
                           +         (x+h)   *sizeof(TRACT*)
                           + (size_t) k      *sizeof(SUPP)
                           + (size_t)(k+k+1) *sizeof(ITEM));
  if (!p) { free(lists); return -1; }
  for (i = 0; i < k; i++) {     /* allocate the list elements and */
    lists[i] = l = (TALIST*)p;  /* traverse the items / trans. lists */
    l->item  = i;               /* set the item identifier */
    l->supp  = 0;               /* clear the item support */
    l->cnt   = 0;               /* and the transaction counter */
    p = l->tracts +c[i];        /* skip space for transactions */
  }                             /* and a sentinel at the end */
  lists[k] = l = (TALIST*)p;    /* set last list (all transactions) */
  l->item  = k; l->cnt = n;     /* for a dummy item (> all items) */
  l->supp  = tbg_wgt(eclat->tabag); /* with full database support */
  for (m = 0; m < n; m++)       /* copy the transactions */
    l->tracts[m] = tbg_tract(eclat->tabag, m);
  eclat->hash  = (TRACT**)memset(l->tracts+n, 0, h *sizeof(TRACT*));
  eclat->muls  = (SUPP*)memset(eclat->hash+h, 0,(size_t)k*sizeof(SUPP));
  eclat->miss  = eclat->muls;            /* get the auxiliary arrays */
  eclat->cand  = (ITEM*)(eclat->muls+k); /* and initialize them */
  eclat->fim16 = NULL;          /* default: no 16-items machine */
  eclat->first = 0;             /* first item is normal item */
  if (eclat->mode & ECL_FIM16){ /* if to use a 16-items machine */
    eclat->fim16 = m16_create(eclat->dir, eclat->supp, eclat->report);
    if (!eclat->fim16) { free(tras); free(lists); return -1; }
    eclat->first = tbg_packcnt(eclat->tabag);
  }                             /* get the number of packed items */
  r = (eclat->mode & ECL_EXTCHK)/* exceute the eclat recursion with */
    ? (int)rec_odcm(eclat, lists, k)   /* explicit extension checks */
    : (eclat->mode & ECL_REORDER)  /* execute the eclat recursion */
    ? rec_odro(eclat, lists, k)    /* with    item reordering */
    : rec_odfx(eclat, lists, k);   /* without item reordering */
  if (r >= 0)                   /* finally report the empty item set */
    r = isr_report(eclat->report);
  if (eclat->fim16)             /* if a 16-items machine was used, */
    m16_delete(eclat->fim16);   /* delete the 16-items machine */
  free(tras); free(lists);      /* deallocate the transaction array */
  return r;                     /* return the error status */
}  /* eclat_ocd() */

/*----------------------------------------------------------------------
  Eclat with Diffsets
----------------------------------------------------------------------*/

static TID cmpl (TIDLIST *dst, TIDLIST *src1, TIDLIST *src2, SUPP *muls)
{                               /* --- complement two trans. id lists */
  TID *s1, *s2, *d;             /* to traverse sources and dest. */

  assert(dst && src1 && src2 && muls); /* check function arguments */
  dst->item = src1->item;       /* copy the first item and */
  dst->supp = src1->supp;       /* initialize the support */
  d = dst->tids; s1 = src1->tids; s2 = src2->tids;
  while (1) {                   /* trans. id list difference loop */
    if      (*s1 > *s2) dst->supp -= muls[*s1++];
    else if (*s1 < *s2) *d++ = *s2++;
    else if (*s1 < 0) break;    /* collect elements of second source */
    else { s1++; s2++; }        /* that are not in the first source */
  }                             /* (form complement of first source) */
  *d++ = -1;                    /* store a sentinel at the list end */
  return (TID)(d -dst->tids);   /* return the size of the new lists */
}  /* cmpl() */

/*--------------------------------------------------------------------*/

static TID diff (TIDLIST *dst, TIDLIST *src1, TIDLIST *src2, SUPP *muls)
{                               /* --- subtract two trans. id lists */
  TID *s1, *s2, *d;             /* to traverse sources and dest. */

  assert(dst && src1 && src2 && muls); /* check function arguments */
  dst->item = src1->item;       /* copy the first item and */
  dst->supp = src1->supp;       /* initialize the support */
  d = dst->tids; s1 = src1->tids; s2 = src2->tids;
  while (1) {                   /* trans. id list difference loop */
    if      (*s1 > *s2) *d++ = *s1++;
    else if (*s1 < *s2) dst->supp -= muls[*s2++];
    else if (*s1 < 0) break;    /* remove all elements of the second */
    else { s1++; s2++; }        /* source from the first source */
  }                             /* (form difference of tid lists) */
  *d++ = -1;                    /* store a sentinel at the list end */
  return (TID)(d -dst->tids);   /* return the size of the new lists */
}  /* diff() */

/*--------------------------------------------------------------------*/

static int rec_diff (ECLAT *eclat, TIDLIST **lists, ITEM k, TID x,
                     COMBFN comb)
{                               /* --- eclat recursion with diffsets */
  int     r;                    /* error status */
  ITEM    i, m, z;              /* loop variables */
  TID     c;                    /* size of combined lists */
  SUPP    pex;                  /* minimum support for perfect exts. */
  TIDLIST *l, *d;               /* to traverse transaction id lists */
  TIDLIST **proj = NULL;        /* trans. id lists of proj. database */
  ITEM    *t;                   /* to collect the tail items */

  assert(eclat && lists && (k > 0)); /* check the function arguments */
  #ifdef ECL_ABORT              /* if to check for interrupt */
  if (sig_aborted()) return -1; /* if execution was aborted, */
  #endif                        /* abort the recursion */
  if (eclat->mode & ECL_TAIL) { /* if to use tail to prune w/ repo. */
    t = isr_buf(eclat->report); /* collect the tail items in buffer */
    for (m = 0, i = k; --i >= 0; ) t[m++] = lists[i]->item;
    r = isr_tail(eclat->report, t, m);
    if (r) return r;            /* if tail need not be processed, */
  }                             /* abort the recursion */
  if ((k > 1)                   /* if there is more than one item and */
  &&  isr_xable(eclat->report, 2)) {     /* another item can be added */
    proj = (TIDLIST**)malloc((size_t)k           *sizeof(TIDLIST*)
                            +(size_t)k           *sizeof(TIDLIST)
                            +(size_t)k*(size_t)x *sizeof(TID));
    if (!proj) return -1;       /* allocate list and element arrays */
  }                             /* (memory for projected database) */
  if ((k > 4)                   /* if there are enough items left, */
  &&  (eclat->mode & ECL_REORDER)) /* re-sort items w.r.t. support */
    ptr_qsort(lists, (size_t)k, +1, tid_cmp, NULL);
  if (eclat->dir > 0) { z =  k; k  = 0; }
  else                { z = -1; k -= 1; }
  for (r = 0; k != z; k += eclat->dir) {
    l = lists[k];               /* traverse the items / tid lists */
    r = isr_add(eclat->report, l->item, l->supp);
    if (r <  0) break;          /* add current item to the reporter */
    if (r <= 0) continue;       /* check if item needs processing */
    if (proj && (k > 0)) {      /* if another item can be added */
      pex = (eclat->mode & ECL_PERFECT) ? l->supp : SUPP_MAX;
      proj[m = 0] = d = (TIDLIST*)(proj +k+1); x = 0;
      for (i = 0; i < k; i++) { /* traverse the preceding lists */
        c = comb(d, lists[i], l, eclat->muls);
        if (d->supp < eclat->supp)/* combine transaction id lists */
          continue;             /* and eliminate infrequent items */
        if (d->supp >= pex) {   /* collect perfect extensions */
          isr_addpex(eclat->report, d->item); continue; }
        proj[++m] = d = (TIDLIST*)(d->tids +c);
        if (c > x) x = c;       /* collect the trans. id lists and */
      }                         /* determine their maximum length */
      if (m > 0) {              /* if the projection is not empty */
        r = rec_diff(eclat, proj, m, x, diff);
        if (r < 0) break;       /* recursively find freq. item sets */
      }                         /* in the created projection */
    }
    r = isr_report(eclat->report);
    if (r < 0) break;           /* report the current item set */
    isr_remove(eclat->report,1);/* remove the current item */
  }                             /* from the item set reporter */
  if (proj) free(proj);         /* delete the list and element arrays */
  return r;                     /* return the error status */
}  /* rec_diff() */

/*--------------------------------------------------------------------*/

int eclat_diff (ECLAT *eclat)
{                               /* --- eclat with difference sets */
  int        r = 0;             /* result of recursion/error status */
  ITEM       i, k, m;           /* loop variable, number of items */
  TID        n, z;              /* (maximum) number of transactions */
  size_t     x;                 /* number of item instances */
  SUPP       w;                 /* weight/support buffer */
  SUPP       pex;               /* minimum support for perfect exts. */
  TRACT      *t;                /* to traverse transactions */
  TIDLIST    **lists, *l;       /* to traverse transaction id lists */
  TID        *tids, *p, **next; /* to traverse transaction ids */
  const ITEM *s;                /* to traverse transaction items */
  const TID  *c;                /* item occurrence counters */

  assert(eclat);                /* check the function arguments */
  eclat->dir = (eclat->target & (ISR_CLOSED|ISR_MAXIMAL)) ? -1 : +1;
  pex = tbg_wgt(eclat->tabag);  /* get the item processing direction */
  if (eclat->supp > pex)        /* and support for perfect exts. and */
    return 0;                   /* check the total transaction weight */
  if (!(eclat->mode & ECL_PERFECT)) pex = SUPP_MAX;
  n = tbg_cnt(eclat->tabag);    /* get the number of transactions */
  k = tbg_itemcnt(eclat->tabag);/* and check the number of items */
  if (k <= 0) return isr_report(eclat->report);
  c = tbg_icnts(eclat->tabag,0);/* get the number of containing */
  if (!c) return -1;            /* transactions per item */
  lists = (TIDLIST**)malloc((size_t)k *sizeof(TIDLIST*)
                           +(size_t)k *sizeof(TID*)
                           +(size_t)n *sizeof(SUPP));
  if (!lists) return -1;        /* create initial tid list array */
  next        = (TID**)(lists+k);  /* split off next position array, */
  eclat->muls = (SUPP*)(next +k);  /* transaction multiplicity array */
  x = tbg_extent(eclat->tabag); /* get the number of item occurrences */
  p = tids = (TID*)malloc((size_t)k *sizeof(TIDLIST) +x *sizeof(TID));
  if (!p) { free(lists); return -1; }  /* allocate tid list elements */
  for (i = 0; i < k; i++) {     /* traverse the items / tid lists */
    lists[i] = l = (TIDLIST*)p; /* get/create the next trans. id list */
    l->item  = i;               /* initialize the list item */
    l->supp  = 0;               /* and the support counter */
    next[i]  = p = l->tids;     /* note position of next trans. id */
    p += c[i]; *p++ = (TID)-1;  /* skip space for transaction ids */
  }                             /* and store a sentinel at the end */
  while (n > 0) {               /* traverse the transactions */
    t = tbg_tract(eclat->tabag, --n);  /* get the next transaction */
    eclat->muls[n] = w = ta_wgt(t);    /* and store its weight */
    for (s = ta_items(t); *s > TA_END; s++) {
      lists[*s]->supp += w;     /* traverse the transaction's items */
      *next[*s]++      = n;     /* sum the transaction weight and */
    }                           /* collect the transaction ids */
  }
  z = 0;                        /* init. the maximal list length */
  for (i = m = 0; i < k; i++) { /* traverse the items / tid lists */
    l = lists[i];               /* eliminate all infrequent items and */
    if (l->supp <  eclat->supp) continue;    /* collect perfect exts. */
    if (l->supp >= pex) { isr_addpex(eclat->report, i); continue; }
    n = (TID)(next[i] -l->tids);
    if (n > z) z = n;           /* find maximum trans. id list length */
    lists[m++] = l;             /* collect lists for frequent items */
  }                             /* (eliminate infrequent items) */
  if (m > 0)                    /* find freq. items sets recursively */
    r = rec_diff(eclat, lists, m, z, cmpl);
  if (r >= 0)                   /* finally report the empty item set */
    r = isr_report(eclat->report);
  free(tids); free(lists);      /* delete the allocated arrays */
  return r;                     /* return the error status */
}  /* eclat_diff() */

/*----------------------------------------------------------------------
  Eclat with Occurrence Deliver (for rules)
----------------------------------------------------------------------*/

static int rec_tree (ECLAT *eclat, TALIST **lists, ITEM k)
{                               /* --- occ. deliver w/o reordering */
  int        r = 0;             /* error status */
  ITEM       i, m;              /* loop variables */
  TID        n;                 /* loop variable for transactions */
  SUPP       w;                 /* weight/support buffer */
  TALIST     *l, *p;            /* to traverse transaction lists */
  TRACT      *t;                /* to traverse transactions */
  const ITEM *s;                /* to traverse items */

  assert(eclat && lists && (k > 0)); /* check the function arguments */
  #ifdef ECL_ABORT              /* if to check for interrupt */
  if (sig_aborted()) return -1; /* if execution was aborted, */
  #endif                        /* abort the recursion */
  l = lists[k];                 /* collate equal transactions */
  taa_collate(l->tracts, l->cnt, k);
  for (n = 0; n < l->cnt; n++){ /* traverse the transactions, */
    t = l->tracts[n];           /* but skip collated transactions */
    if ((w = ta_wgt(t)) <= 0) continue;
    for (s = ta_items(t); (UITEM)*s < (UITEM)k; s++) {
      p = lists[*s]; p->supp += w; p->tracts[p->cnt++] = t; }
  }                             /* deliver the item occurrences */
  for (i = m = 0; i < k; i++) { /* traverse the items */
    p = lists[i];               /* get corresp. transaction list */
    if (p->supp < eclat->supp) { p->supp = 0; p->cnt = 0; }
    else m += 1;                /* eliminate infrequent items */
  }                             /* and count the frequent items */
  if (m <= 0) {                 /* if no frequent items found, abort */
    taa_uncoll(l->tracts, l->cnt); return 0; }
  for (i = 0; i < k; i++) {     /* traverse the items and */
    p = lists[i];               /* get corresp. transaction list */
    if (p->supp >= eclat->supp) ist_setsupp(eclat->istree, i, p->supp);
  }                             /* set the item set support */
  m = ist_xable(eclat->istree, 1) ? 0 : ITEM_MAX;
  if ((m <= 0) && (ist_addchn(eclat->istree) != 0))
    return -1;                  /* add children to current node */
  for (i = 0; i < k; i++) {     /* traverse the items */
    p = lists[i];               /* get corresp. transaction list */
    if (p->supp <= 0) continue; /* skip all eliminated items */
    if ((i > m)                 /* go down in the item set tree */
    &&  (ist_down(eclat->istree, i) >= 0)) {
      r = rec_tree(eclat, lists, i);
      if (r < 0) break;         /* recursively find freq. item sets */
      ist_up(eclat->istree);    /* and check for a recursion error, */
    }                           /* then go back up in the tree */
    p->supp = 0; p->cnt = 0;    /* reinit. the transaction list */
  }
  taa_uncoll(l->tracts,l->cnt); /* uncollate the transactions */
  return r;                     /* and return the error status */
}  /* rec_tree() */

/*--------------------------------------------------------------------*/

int eclat_tree (ECLAT *eclat)
{                               /* --- search for frequent item sets */
  int       r = 0;              /* result of recursion/error status */
  ITEM      i, k;               /* loop variable, number of items */
  TID       n, m;               /* number of transactions */
  size_t    x, h;               /* extent, hash table size */
  TALIST    **lists, *l;        /* to traverse transaction lists */
  TRACT     **tras, **p;        /* to traverse transactions */
  const TID *c;                 /* item occurrence counters */

  assert(eclat);                /* check the function arguments */
  eclat->dir = +1;              /* only upward item loops possible */
  k = tbg_itemcnt(eclat->tabag);/* get the number of (frequent) items */
  if (k <= 0) return 0;         /* if there are none, abort */
  c = tbg_icnts(eclat->tabag,0);/* get the number of containing */
  if (!c) return -1;            /* transactions per item */
  lists = (TALIST**)malloc((size_t)(k+1) *sizeof(TALIST*));
  if (!lists) return -1;        /* create the trans. id list array */
  for (x = 0, i = 0; i < k; i++)/* traverse the items and sum */
    x += (size_t)c[i];          /* the numbers of item occurrences */
  /* Do not use tbg_extent(), because it does not take packed items */
  /* properly into account and thus may yield too big a value.      */
  n = tbg_cnt(eclat->tabag);    /* get the number of transactions */
  h = (size_t)taa_tabsize(n);   /* get the hash table size and */
  x = (size_t)n +x -(size_t)(k+1);        /* the database size */
  p = tras = (TRACT**)malloc((size_t)(k+1) *sizeof(TALIST)
                           +         (x+h) *sizeof(TRACT*)
                           + (size_t) k    *sizeof(SUPP)
                           + (size_t)(k+k) *sizeof(ITEM));
  if (!p) { free(lists); return -1; }
  for (i = 0; i < k; i++) {     /* allocate the list elements and */
    lists[i] = l = (TALIST*)p;  /* traverse the items / trans. lists */
    l->item  = i;               /* set the item identifier */
    l->supp  = 0;               /* clear the item support */
    l->cnt   = 0;               /* and the transaction counter */
    p = l->tracts +c[i];        /* skip space for transactions */
  }                             /* and a sentinel at the end */
  lists[k] = l = (TALIST*)p;    /* set last list (all transactions) */
  l->item  = k; l->cnt = n;     /* for a dummy item (> all items) */
  l->supp  = tbg_wgt(eclat->tabag); /* with full database support */
  for (m = 0; m < n; m++)       /* copy the transactions */
    l->tracts[m] = tbg_tract(eclat->tabag, m);
  eclat->hash = (TRACT**)memset(l->tracts+n, 0, h *sizeof(TRACT*));
  eclat->muls = (SUPP*)memset(eclat->hash+h, 0, (size_t)k*sizeof(SUPP));
  eclat->cand = (ITEM*)(eclat->muls+k);/* get auxiliary arrays */
  r = rec_tree(eclat, lists, k);/* execute the eclat recursion */
  free(tras); free(lists);      /* deallocate the transaction array */
  return r;                     /* return the error status */
}  /* eclat_tree() */

/*----------------------------------------------------------------------
  Eclat Algorithm (generic)
----------------------------------------------------------------------*/

static ECLATFN* eclatvars[] = { /* --- table of eclat variants */
  eclat_base,                   /* trans. id lists (basic) */
  eclat_tid,                    /* trans. id lists (improved) */
  eclat_bit,                    /* bit vector over transactions */
  eclat_tab,                    /* item occurrence table */
  eclat_simp,                   /* simplified version with table */
  eclat_trg,                    /* transaction identifier ranges */
  eclat_ocd,                    /* occurrence deliver (LCM-style) */
  eclat_diff,                   /* difference sets (diffsets) */
};

/*--------------------------------------------------------------------*/

ECLAT* eclat_create (int target, double smin, double smax,
                     double conf, ITEM zmin, ITEM zmax,
                     int eval, int agg, double thresh,
                     int algo, int mode)
{                               /* --- create an eclat miner */
  ECLAT *eclat;                 /* created eclat miner */
  int   e;                      /* evaluation without flags */

  /* --- make parameters consistent --- */
  if      (target & ECL_RULES)   target = ISR_RULES;
  else if (target & ECL_GENERAS) target = ISR_GENERAS;
  else if (target & ECL_MAXIMAL) target = ISR_MAXIMAL;
  else if (target & ECL_CLOSED)  target = ISR_CLOSED;
  else                           target = ISR_FREQUENT;
  if (!(target & ISR_RULES))    /* if not to find association rules, */
    conf = 100.0;               /* set a neutral confidence */
  if ((mode & ECL_TIDS) && (algo != ECL_LISTS) && (algo != ECL_TABLE))
    algo = ECL_LISTS;           /* trans. identifiers require lists */
  if ((target & ISR_GENERAS) && (algo == ECL_SIMPLE))
    algo = ECL_TABLE;           /* no generators with simple algo. */
  if (target & ISR_GENERAS)     /* if to filter for generators, */
    mode |= ECL_PERFECT;        /* need perfect extension pruning */
  else if (target & (ISR_CLOSED|ISR_MAXIMAL)) {
    mode &= ~ECL_REORDER;       /* cannot reorder for closed/maximal */
    if (algo == ECL_OCCDLV) {   /* if occurrence deliver method */
      mode |= ECL_EXTCHK; mode &= ~ECL_FIM16; }
  }                             /* special closed/maximal treatment */
  if ((algo == ECL_RANGES) || (algo == ECL_SIMPLE))
    mode &= ~ECL_REORDER;       /* not all variants permit reordering */
  if ((algo != ECL_LISTS) && (algo != ECL_OCCDLV))
    mode &= ~ECL_EXTCHK;        /* extension checks possible? */
  if ((algo != ECL_LISTS) && (algo != ECL_RANGES)
  &&  (algo != ECL_OCCDLV))     /* not all algorithm variants */
    mode &= ~ECL_FIM16;         /* support a 16-items machine */
  e = eval & ~ECL_INVBXS;       /* get eval. measure without flags */
  if ((target & ISR_RULES) || ((e > RE_NONE) && (e < RE_FNCNT)))
    mode &= ~ECL_FIM16;         /* no 16-items machine for evaluation */
  if (mode & (ECL_EXTCHK|ECL_TIDS))
    mode &= ~ECL_FIM16;         /* no 16-items machine for ext./tids. */
  if (!(target & ISR_MAXIMAL))  /* tail pruning only for mining */
    mode &= ~ECL_TAIL;          /* maximal frequent item sets */

  /* --- create an eclat miner --- */
  eclat = (ECLAT*)malloc(sizeof(ECLAT));
  if (!eclat) return NULL;      /* create an eclat miner */
  eclat->target = target;       /* and store all parameters */
  eclat->smin   = smin;
  eclat->smax   = smax;
  eclat->supp   = 1;
  eclat->body   = 1;
  eclat->conf   = conf/100.0;
  eclat->zmin   = zmin;
  eclat->zmax   = zmax;
  eclat->eval   = eval;
  eclat->agg    = agg;
  eclat->thresh = thresh/100.0;
  eclat->algo   = algo;
  eclat->mode   = mode;
  eclat->tabag  = NULL;
  eclat->report = NULL;
  eclat->first  = 0;
  eclat->dir    = 1;
  eclat->muls   = NULL;
  eclat->marks  = NULL;
  eclat->cand   = NULL;
  eclat->tab    = NULL;
  eclat->hash   = NULL;
  eclat->elim   = NULL;
  eclat->fim16  = NULL;
  eclat->istree = NULL;
  return eclat;                 /* return the created eclat miner */
}  /* eclat_create() */

/*--------------------------------------------------------------------*/

static int cleanup (ECLAT *eclat)
{                               /* --- clean up on error */
  if (eclat->mode & ECL_NOCLEAN)
    return E_NOMEM;             /* if not to clean up memory, abort */
  if (eclat->istree) {          /* free item set tree (for counting) */
    ist_delete(eclat->istree); eclat->istree = NULL; }
  return E_NOMEM;               /* return an error indicator */
}  /* cleanup() */

/*--------------------------------------------------------------------*/

void eclat_delete (ECLAT *eclat, int deldar)
{                               /* --- delete an eclat miner */
  cleanup(eclat);               /* clean up temporary data */
  if (deldar) {                 /* if to delete data and reporter */
    if (eclat->report) isr_delete(eclat->report, 0);
    if (eclat->tabag)  tbg_delete(eclat->tabag,  1);
  }                             /* delete if existing */
  free(eclat);                  /* delete the base structure */
}  /* eclat_delete() */

/*--------------------------------------------------------------------*/

int eclat_data (ECLAT *eclat, TABAG *tabag, int mode, int sort)
{                               /* --- prepare data for eclat */
  ITEM    m;                    /* number of items */
  ITEM    pack;                 /* number of items to pack */
  int     dir;                  /* order of items in transactions */
  double  smin;                 /* absolute minimum support */
  SUPP    w;                    /* total transaction weight */
  double  x;                    /* extent of the database */
  int     e;                    /* evaluation without flags */
  #ifndef QUIET                 /* if to print messages */
  TID     n;                    /* number of transactions */
  clock_t t;                    /* timer for measurements */
  #endif                        /* (only needed for messages) */

  assert(eclat && tabag);       /* check the function arguments */
  eclat->tabag = tabag;         /* note the transaction bag */

  /* --- compute data-specific parameters --- */
  w = tbg_wgt(tabag);           /* compute absolute minimum support */
  smin = ceilsupp((eclat->smin < 0) ? -eclat->smin
                : (eclat->smin/100.0) *(double)w *(1-DBL_EPSILON));
  eclat->body = (SUPP)smin;     /* compute body and body&head support */
  if ((eclat->target & ISR_RULES) && !(eclat->mode & ECL_ORIGSUPP))
    smin *= eclat->conf *(1-DBL_EPSILON);
  eclat->supp = (SUPP)ceilsupp(smin);
  if (eclat->algo == ECL_AUTO){ /* if automatic variant choice */
    m = ib_frqcnt(tbg_base(tabag), eclat->supp);
    w = tbg_wgt(tabag);         /* get database parameters */
    x = (double)tbg_extent(eclat->tabag);
    eclat->algo = ((eclat->target & (ISR_CLOSED|ISR_MAXIMAL))
                && (x/((double)m*(double)w) > 0.02))
                ? ECL_LISTS : ECL_OCCDLV;
    if ((eclat->target & (ISR_CLOSED|ISR_MAXIMAL))
    &&  (eclat->algo == ECL_OCCDLV)) {
      eclat->mode |= ECL_EXTCHK; eclat->mode &= ~ECL_FIM16; }
  }                             /* choose the eclat variant */

  /* --- make parameters consistent --- */
  pack = eclat->mode & ECL_FIM16;
  if (pack > 16) pack = 16;     /* pack at most 16 items */
  if ((eclat->algo == ECL_OCCDLV) && (eclat->mode & ECL_REORDER))
    pack = 0;                   /* delayed packing if reordering */
  if (mode & ECL_NOPACK)        /* if excluded by processing mode, */
    pack = 0;                   /* do not pack items */
  dir = ((eclat->algo == ECL_RANGES) || (eclat->algo == ECL_OCCDLV))
      ? +1 : -1;                /* get order of items in trans. */
  if ((eclat->target & ISR_RULES)
  || ((eclat->eval > RE_NONE) && (eclat->eval < RE_FNCNT)))
    dir = +1;                   /* ascending order if evaluation */

  /* --- sort and recode items --- */
  if (!(mode & ECL_NORECODE)) { /* if to sort and recode the items */
    CLOCK(t);                   /* start timer, print log message */
    XMSG(stderr, "filtering, sorting and recoding items ... ");
    if (eclat->mode & ECL_REORDER) /* simplified sorting for reorder */
      sort = (sort < 0) ? -1 : (sort > 0) ? +1 : 0;
    m = tbg_recode(tabag, eclat->supp, -1, -1, -sort);
    if (m < 0) return E_NOMEM;  /* recode items and transactions */
    if (m < 1) return E_NOITEMS;/* and check the number of items */
    XMSG(stderr, "[%"ITEM_FMT" item(s)]", m);
    XMSG(stderr, " done [%.2fs].\n", SEC_SINCE(t));
  }                             /* print a log message */

  /* --- sort and reduce transactions --- */
  CLOCK(t);                     /* start timer, print log message */
  XMSG(stderr, "sorting and reducing transactions ... ");
  e = eclat->eval & ~ECL_INVBXS;/* filter transactions if possible */
  if (!(mode & ECL_NOFILTER) && !(eclat->target & ISR_RULES)
  && ((e <= RE_NONE) || (e >= RE_FNCNT)))
    tbg_filter(tabag, eclat->zmin, NULL, 0);
  if (!(mode & ECL_NOSORT))     /* if item sorting allowed, */
    tbg_itsort(tabag, dir, 0);  /* sort items in transactions */
  if (eclat->mode & ECL_EXTCHK){/* if checking exts. for clo./max. */
    if (!(mode & ECL_NOSORT)) { /* if transaction sorting allowed, */
      tbg_sortsz(tabag, -1, 0); /* sort transactions by size */
      if (!(mode & ECL_NOREDUCE))
        tbg_reduce(tabag, 0);   /* reduce transactions to unique ones */
    }                           /* (need sorting for reduction) */
    tbg_bitmark(tabag); }       /* set item bits in markers */
  else if ((eclat->algo == ECL_RANGES)/* if to use transaction ranges */
  &&       (pack > 0)) {        /* together with a 16-items machine */
    tbg_pack(tabag, pack);      /* pack the most frequent items */
    if (!(mode & ECL_NOSORT)) { /* sort trans. lexicographically */
      tbg_sort(tabag, 1, TA_EQPACK);
      if (!(mode & ECL_NOREDUCE))
        tbg_reduce(tabag, 0);   /* reduce transactions to unique ones */
    } }                         /* (need sorting for reduction) */
  else if (!(eclat->mode & ECL_TIDS)   /* if not to report trans. ids */
  &&       (eclat->algo != ECL_BITS)){ /* and not to use bit vectors */
    if (!(mode & ECL_NOSORT)) { /* if transaction sorting allowed, */
      tbg_sort(tabag, dir, 0);  /* sort trans. lexicographically */
      if (!(mode & ECL_NOREDUCE))
        tbg_reduce(tabag, 0);   /* reduce transactions to unique ones */
    }                           /* (need sorting for reduction) */
    if (pack > 0)               /* if to use a 16-items machine, */
      tbg_pack(tabag, pack);    /* pack the most frequent items */
  }                             /* (bit-represented transactions) */
  #ifndef QUIET                 /* if to print messages */
  n = tbg_cnt(tabag);           /* get the number of transactions */
  w = tbg_wgt(tabag);           /* and the new transaction weight */
  XMSG(stderr, "[%"TID_FMT, n); /* print number of transactions */
  if (w != (SUPP)n) { XMSG(stderr, "/%"SUPP_FMT, w); }
  XMSG(stderr, " transaction(s)] done [%.2fs].\n", SEC_SINCE(t));
  #endif
  return 0;                     /* return 'ok' */
}  /* eclat_data() */

/*--------------------------------------------------------------------*/

int eclat_report (ECLAT *eclat, ISREPORT *report)
{                               /* --- prepare reporter for eclat */
  TID    n;                     /* number of transactions */
  SUPP   w;                     /* total transaction weight */
  double smax;                  /* absolute maximum support */
  int    mrep;                  /* mode for item set reporter */
  int    e;                     /* evaluation without flags */

  assert(eclat && report);      /* check the function arguments */
  eclat->report = report;       /* note the item set reporter */

  /* --- make parameters consistent --- */
  mrep = 0;                     /* init. the reporting mode */
  if ((eclat->target & ISR_GENERAS) && (eclat->mode & ECL_REORDER))
    mrep |= ISR_SORT;           /* reordering requires set sorting */
  e = eclat->eval & ~ECL_INVBXS;/* remove flags from measure code */
  if ((eclat->mode & ECL_EXTCHK)
  ||  (eclat->target & ISR_RULES) || ((e > RE_NONE) && (e < RE_FNCNT)))
    mrep |= ISR_NOFILTER;       /* no filtering if done in eclat */

  /* --- configure item set reporter --- */
  w = tbg_wgt(eclat->tabag);    /* set support and size range */
  smax = (eclat->smax < 0) ? -eclat->smax
       : (eclat->smax/100.0) *(double)w *(1-DBL_EPSILON);
  isr_setsupp(report, (RSUPP)eclat->supp, (RSUPP)floorsupp(smax));
  isr_setsize(report, eclat->zmin, eclat->zmax);
  if (e == ECL_LDRATIO)         /* set additional evaluation measure */
    isr_seteval(report, isr_logrto, NULL, +1, eclat->thresh);
  n = (eclat->mode & ECL_PREFMT)/* get range of number to preformat */
    ? (TID)ib_maxfrq(tbg_base(eclat->tabag)) : -1;
  if ((isr_prefmt(report, (TID)eclat->supp, n)      != 0)
  ||  (isr_settarg(report, eclat->target, mrep, -1) != 0))
    return E_NOMEM;             /* set pre-format and target type */
  return 0;                     /* return 'ok' */
}  /* eclat_report() */

/*--------------------------------------------------------------------*/

int eclat_mine (ECLAT *eclat, ITEM prune, int order)
{                               /* --- eclat algorithm */
  int     r;                    /* result of function call */
  ITEM    m, x;                 /* number of items */
  int     e;                    /* evaluation without flags */
  #ifndef QUIET                 /* if to print messages */
  clock_t t;                    /* timer for measurements */
  #endif                        /* (only needed for messages) */

  assert(eclat);                /* check the function arguments */

  /* --- make parameters consistent --- */
  e = eclat->eval & ~ECL_INVBXS;/* remove flags from measure code */
  if (e <= RE_NONE)             /* if there is no evaluation, */
    prune = ITEM_MIN;           /* do not prune with evaluation */

  /* --- find frequent item sets --- */
  if (!(eclat->target & ISR_RULES)  /* if to find plain item sets */
  &&  ((e <= RE_NONE) || (e >= RE_FNCNT))) {
    CLOCK(t);                   /* start timer, print log message */
    XMSG(stderr, "writing %s ... ", isr_name(eclat->report));
    r = eclatvars[eclat->algo](eclat);
    if (r < 0) return E_NOMEM;  /* search for frequent item sets */
    XMSG(stderr, "[%"SIZE_FMT" set(s)]", isr_repcnt(eclat->report));
    XMSG(stderr, " done [%.2fs].\n", SEC_SINCE(t)); }
  else {                        /* if rules or rule-based evaluation */
    CLOCK(t);                   /* start timer, print log message */
    XMSG(stderr, "finding frequent item sets ... ");
    eclat->istree = ist_create(tbg_base(eclat->tabag), IST_REVERSE,
                               eclat->supp, eclat->body, eclat->conf);
    if (!eclat->istree) return E_NOMEM;
    x = ((eclat->target & (ISR_CLOSED|ISR_MAXIMAL))
      && (eclat->zmax < ITEM_MAX)) ? eclat->zmax+1 : eclat->zmax;
    if (x > (m = tbg_max(eclat->tabag))) x = m;
    ist_setsize(eclat->istree, eclat->zmin, x);
    r = eclat_tree(eclat);      /* search for frequent item sets */
    if (r) return cleanup(eclat);
    XMSG(stderr, "done [%.2fs].\n", SEC_SINCE(t));
    if ((prune >  ITEM_MIN)     /* if to filter with evaluation */
    &&  (prune <= 0)) {         /* (backward and weak forward) */
      CLOCK(t);                 /* start the timer for filtering */
      XMSG(stderr, "filtering with evaluation ... ");
      ist_filter(eclat->istree, prune);
      XMSG(stderr, "done [%.2fs].\n", SEC_SINCE(t));
    }                           /* filter with evaluation */
    if (eclat->target & (ISR_CLOSED|ISR_MAXIMAL|ISR_GENERAS)) {
      CLOCK(t);                 /* start the timer for filtering */
      XMSG(stderr, "filtering for %s item sets ... ",
           (eclat->target & ISR_GENERAS) ? "generator" :
           (eclat->target & ISR_MAXIMAL) ? "maximal" : "closed");
      ist_clomax(eclat->istree, /* filter closed/maximal/generators */
                 eclat->target | ((prune > ITEM_MIN) ? IST_SAFE : 0));
      XMSG(stderr, "done [%.2fs].\n", SEC_SINCE(t));
    }
    CLOCK(t);                   /* start timer, print log message */
    XMSG(stderr, "writing %s ... ", isr_name(eclat->report));
    if (e != ECL_LDRATIO)       /* set additional evaluation measure */
      ist_seteval(eclat->istree, eclat->eval,
                  eclat->agg, eclat->thresh, prune);
    ist_init(eclat->istree, order); /* initialize the extraction */
    r = ist_report(eclat->istree, eclat->report, eclat->target);
    cleanup(eclat);             /* report item sets/rules, */
    if (r < 0) return E_NOMEM;  /* then clean up the work memory */
    XMSG(stderr, "[%"SIZE_FMT" %s(s)]", isr_repcnt(eclat->report),
                 (eclat->target == ISR_RULES) ? "rule" : "set");
    XMSG(stderr, " done [%.2fs].\n", SEC_SINCE(t));
  }                             /* print a log message */
  return 0;                     /* return 'ok' */
}  /* eclat_mine() */

/*----------------------------------------------------------------------
  Main Functions
----------------------------------------------------------------------*/
#ifdef ECL_MAIN

static void help (void)
{                               /* --- print add. option information */
  #ifndef QUIET
  fprintf(stderr, "\n");        /* terminate startup message */
  printf("additional evaluation measures (option -e#)\n");
  printf("frequent item sets:\n");
  printf("  x   no measure (default)\n");
  printf("  b   binary logarithm of support quotient            (+)\n");
  printf("association rules:\n");
  printf("  x   no measure (default)\n");
  printf("  o   rule support (original def.: body & head)       (+)\n");
  printf("  c   rule confidence                                 (+)\n");
  printf("  d   absolute confidence difference to prior         (+)\n");
  printf("  l   lift value (confidence divided by prior)        (+)\n");
  printf("  a   absolute difference of lift value to 1          (+)\n");
  printf("  q   difference of lift quotient to 1                (+)\n");
  printf("  v   conviction (inverse lift for negated head)      (+)\n");
  printf("  e   absolute difference of conviction to 1          (+)\n");
  printf("  r   difference of conviction quotient to 1          (+)\n");
  printf("  k   conditional probability ratio                   (+)\n");
  printf("  j   importance (binary log. of cond. prob. ratio)   (+)\n");
  printf("  z   certainty factor (relative confidence change)   (+)\n");
  printf("  n   normalized chi^2 measure                        (+)\n");
  printf("  p   p-value from (unnormalized) chi^2 measure       (-)\n");
  printf("  y   normalized chi^2 measure with Yates' correction (+)\n");
  printf("  t   p-value from Yates-corrected chi^2 measure      (-)\n");
  printf("  i   information difference to prior                 (+)\n");
  printf("  g   p-value from G statistic/information difference (-)\n");
  printf("  f   Fisher's exact test (table probability)         (-)\n");
  printf("  h   Fisher's exact test (chi^2 measure)             (-)\n");
  printf("  m   Fisher's exact test (information gain)          (-)\n");
  printf("  s   Fisher's exact test (support)                   (-)\n");
  printf("All measures for association rules are also applicable\n");
  printf("to item sets and are then aggregated over all possible\n");
  printf("association rules with a single item in the consequent.\n");
  printf("The aggregation mode can be set with the option -a#.\n");
  printf("Measures marked with (+) "
         "must meet or exceed the threshold,\n");
  printf("measures marked with (-) "
         "must not exceed the threshold\n");
  printf("in order for the rule or item set to be reported.\n");
  printf("\n");
  printf("evaluation measure aggregation modes (option -a#)\n");
  printf("  x   no aggregation (use first value)\n");
  printf("  m   minimum of individual measure values\n");
  printf("  n   maximum of individual measure values\n");
  printf("  a   average of individual measure values\n");
  printf("\n");
  printf("eclat algorithm variants (option -A#)\n");
  printf("  a   automatic choice based on data properties (default)\n");
  printf("  e   transaction id lists intersection (basic)\n");
  printf("  i   transaction id lists intersection (improved)\n");
  printf("  b   transaction id lists represented as bit vectors\n");
  printf("  t   filtering with item occurrence table (standard)\n");
  printf("  s   filtering with item occurrence table (simplified)\n");
  printf("  r   transaction id range lists intersection\n");
  printf("  o   occurrence deliver from transaction lists (default)\n");
  printf("  d   transaction id difference sets (diffsets/dEclat)\n");
  printf("With algorithm variant 'o' closed or maximal item sets\n");
  printf("can only be found with extensions checks. Hence option");
  printf("-y0 is\nautomatically added to options -tc or -tm ");
  printf("if -Ao is given.\n");
  printf("Rules as the target type or using a rule-based evaluation\n");
  printf("measure enforce algorithm variant 'o'.\n");
  printf("\n");
  printf("information output format characters (option -v#)\n");
  printf("  %%%%  a percent sign\n");
  printf("  %%i  number of items (item set size)\n");
  printf("  %%a  absolute item set  support\n");
  printf("  %%s  relative item set  support as a fraction\n");
  printf("  %%S  relative item set  support as a percentage\n");
  printf("  %%b  absolute body set  support\n");
  printf("  %%x  relative body set  support as a fraction\n");
  printf("  %%X  relative body set  support as a percentage\n");
  printf("  %%h  absolute head item support\n");
  printf("  %%y  relative head item support as a fraction\n");
  printf("  %%Y  relative head item support as a percentage\n");
  printf("  %%c  rule confidence as a fraction\n");
  printf("  %%C  rule confidence as a percentage\n");
  printf("  %%l  lift value of a rule (confidence/prior)\n");
  printf("  %%L  lift value of a rule as a percentage\n");
  printf("  %%e  additional evaluation measure\n");
  printf("  %%E  additional evaluation measure as a percentage\n");
  printf("  %%Q  total transaction weight (database size)\n");
  printf("All format characters can be preceded by the number\n");
  printf("of significant digits to be printed (at most 32 digits),\n");
  printf("even though this value is ignored for integer numbers.\n");
  #endif                        /* print help information */
  exit(0);                      /* abort the program */
}  /* help() */

/*--------------------------------------------------------------------*/

static ITEM getbdr (char *s, char **end, double **border)
{                               /* --- get the support border */
  ITEM   i, k;                  /* loop variables */
  double *b;                    /* support border */

  assert(s && end && border);   /* check the function arguments */
  for (i = k = 0; s[i]; i++)    /* traverse the string and */
    if (s[i] == ':') k++;       /* count the number separators */
  *border = b = (double*)malloc((size_t)++k *sizeof(double));
  if (!b) return -1;            /* allocate a support border */
  for (i = 0; i < k; i++) {     /* traverse the parameters */
    b[i] = strtod(s, end);      /* get the next parameter and */
    if (*end == s) break;       /* check for an empty parameter */
    s = *end; if (*s++ != ':') break;
  }                             /* check for a colon */
  if (++i < k)                  /* shrink support array if possible */
    *border = (double*)realloc(b, (size_t)i *sizeof(double));
  return i;                     /* return number of support values */
}  /* getbdr() */

/*--------------------------------------------------------------------*/

static int setbdr (ISREPORT *report, SUPP w, ITEM zmin,
                   double **border, ITEM n)
{                               /* --- set the support border */
  double s;                     /* to traverse the support values */

  assert(report                 /* check the function arguments */
  &&    (w > 0) && (zmin >= 0) && border && (*border || (n <= 0)));
  while (--n >= 0) {            /* traverse the support values */
    s = (*border)[n];           /* transform to absolute count */
    s = ceilsupp((s >= 0) ? s/100.0 *(double)w *(1-DBL_EPSILON) : -s);
    if (isr_setbdr(report, n+zmin, (RSUPP)s) < 0) return -1;
  }                             /* set support in item set reporter */
  if (*border) { free(*border); *border = NULL; }
  return 0;                     /* return 'ok' */
}  /* setbdr() */

/*--------------------------------------------------------------------*/

#ifndef NDEBUG                  /* if debug version */
  #undef  CLEANUP               /* clean up memory and close files */
  #define CLEANUP \
  if (eclat)  eclat_delete(eclat, 0); \
  if (twrite) twr_delete(twrite, 1);  \
  if (report) isr_delete(report, 0);  \
  if (tabag)  tbg_delete(tabag,  0);  \
  if (tread)  trd_delete(tread,  1);  \
  if (ibase)  ib_delete (ibase);      \
  if (border) free(border);
#endif

GENERROR(error, exit)           /* generic error reporting function */

/*--------------------------------------------------------------------*/

int main (int argc, char *argv[])
{                               /* --- main function */
  int     i, k = 0;             /* loop variables, counters */
  char    *s;                   /* to traverse the options */
  CCHAR   **optarg = NULL;      /* option argument */
  CCHAR   *fn_inp  = NULL;      /* name of the input  file */
  CCHAR   *fn_out  = NULL;      /* name of the output file */
  CCHAR   *fn_sel  = NULL;      /* name of item selection file */
  CCHAR   *fn_tid  = NULL;      /* name of transaction ids file */
  CCHAR   *fn_psp  = NULL;      /* name of pattern spectrum file */
  CCHAR   *recseps = NULL;      /* record  separators */
  CCHAR   *fldseps = NULL;      /* field   separators */
  CCHAR   *blanks  = NULL;      /* blank   characters */
  CCHAR   *comment = NULL;      /* comment characters */
  CCHAR   *hdr     = "";        /* record header  for output */
  CCHAR   *sep     = " ";       /* item separator for output */
  CCHAR   *imp     = " <- ";    /* implication sign for ass. rules */
  CCHAR   *dflt    = " (%S)";   /* default format for check */
  CCHAR   *info    = dflt;      /* format for information output */
  int     target   = 's';       /* target type (e.g. closed/maximal) */
  ITEM    zmin     = 1;         /* minimum rule/item set size */
  ITEM    zmax     = ITEM_MAX;  /* maximum rule/item set size */
  double  smin     = 10;        /* minimum support of an item set */
  double  smax     = 100;       /* maximum support of an item set */
  double  conf     = 80;        /* minimum confidence (in percent) */
  int     eval     = 'x';       /* additional evaluation measure */
  int     eflgs    = 0;         /* evaluation measure flags */
  int     agg      = 'x';       /* aggregation mode for eval. measure */
  double  thresh   = 10;        /* threshold for evaluation measure */
  ITEM    prune    = ITEM_MIN;  /* (min. size for) evaluation pruning */
  int     sort     = 2;         /* flag for item sorting and recoding */
  int     algo     = 'a';       /* variant of eclat algorithm */
  int     mode     = ECL_DEFAULT|ECL_PREFMT;   /* search mode */
  int     pack     = 16;        /* number of bit-packed items */
  int     cmfilt   = -1;        /* mode for closed/maximal filtering */
  int     mtar     = 0;         /* mode for transaction reading */
  int     scan     = 0;         /* flag for scanable item output */
  int     bdrcnt   = 0;         /* number of support values in border */
  int     stats    = 0;         /* flag for item set statistics */
  PATSPEC *psp;                 /* collected pattern spectrum */
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
  else {                        /* if no argument is given */
    printf("usage: %s [options] infile [outfile]\n", argv[0]);
    printf("%s\n", DESCRIPTION);
    printf("%s\n", VERSION);
    printf("-t#      target type                              "
                    "(default: %c)\n", target);
    printf("         (s: frequent, c: closed, m: maximal item sets,\n");
    printf("          g: generators, r: association rules)\n");
    printf("-m#      minimum number of items per set/rule     "
                    "(default: %"ITEM_FMT")\n", zmin);
    printf("-n#      maximum number of items per set/rule     "
                    "(default: no limit)\n");
    printf("-s#      minimum support of an item set/rule      "
                    "(default: %g%%)\n", smin);
    printf("-S#      maximum support of an item set/rule      "
                    "(default: %g%%)\n", smax);
    printf("         (positive: percentage, "
                     "negative: absolute number)\n");
    printf("-o       use original rule support definition     "
                    "(body & head)\n");
    printf("-c#      minimum confidence of an assoc. rule     "
                    "(default: %g%%)\n", conf);
    printf("-e#      additional evaluation measure            "
                    "(default: none)\n");
    printf("-a#      aggregation mode for evaluation measure  "
                    "(default: none)\n");
    printf("-d#      threshold for add. evaluation measure    "
                    "(default: %g%%)\n", thresh);
    printf("-z       invalidate eval. below expected support  "
                    "(default: evaluate all)\n");
    printf("-p#      (min. size for) pruning with evaluation  "
                    "(default: no pruning)\n");
    printf("         (< 0: weak forward, > 0 strong forward, "
                     "= 0: backward pruning)\n");
    printf("-q#      sort items w.r.t. their frequency        "
                    "(default: %d)\n", sort);
    printf("         (1: ascending, -1: descending, 0: do not sort,\n"
           "          2: ascending, -2: descending w.r.t. "
                    "transaction size sum)\n");
    printf("-A#      variant of the eclat algorithm to use    "
                    "(default: 'a')\n");
    printf("-x       do not prune with perfect extensions     "
                    "(default: prune)\n");
    printf("-l#      number of items for k-items machine      "
                    "(default: %d)\n", pack);
    printf("         (only for algorithm variants i,r,o,   "
                    "options -Ai/-Ar/-Ao)\n");
    printf("-i       do not sort items w.r.t. cond. support   "
                    "(default: sort)\n");
    printf("         (only for algorithm variants i,b,t,d, "
                    "options -Ai/-Ab/-At/-Ad)\n");
    printf("-y#      check extensions for closed/maximal sets "
                    "(default: repository)\n");
    printf("         (0: horizontal, > 0: vertical representation)\n");
    printf("         (only with improved tid lists variant, "
                    "option -Ai)\n");
    printf("-u       do not use head union tail (hut) pruning "
                    "(default: use hut)\n");
    printf("         (only for maximal item sets, option -tm, "
                    "not with option -Ab)\n");
    printf("-F#:#..  support border for filtering item sets   "
                    "(default: none)\n");
    printf("         (list of minimum support values, "
                    "one per item set size,\n");
    printf("         starting at the minimum size, "
                    "as given with option -m#)\n");
    printf("-R#      read item selection/appearance indicators\n");
    printf("-P#      write a pattern spectrum to a file\n");
    printf("-Z       print item set statistics "
                    "(number of item sets per size)\n");
    printf("-N       do not pre-format some integer numbers   "
                    "(default: do)\n");
    printf("-g       write output in scanable form "
                    "(quote certain characters)\n");
    printf("-h#      record header  for output                "
                    "(default: \"%s\")\n", hdr);
    printf("-k#      item separator for output                "
                    "(default: \"%s\")\n", sep);
    printf("-I#      implication sign for association rules   "
                    "(default: \"%s\")\n", imp);
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
    printf("-T#      file to write transaction identifiers to "
                    "(default: none)\n");
    printf("-!       print additional option information\n");
    printf("infile   file to read transactions from           "
                    "[required]\n");
    printf("outfile  file to write item sets/assoc.rules to   "
                    "[optional]\n");
    return 0;                   /* print a usage message */
  }                             /* and abort the program */
  #endif  /* #ifndef QUIET */
  /* free option characters: j [A-Z]\[ACFINPRSTZ] */

  /* --- evaluate arguments --- */
  for (i = 1; i < argc; i++) {  /* traverse the arguments */
    s = argv[i];                /* get an option argument */
    if (optarg) { *optarg = s; optarg = NULL; continue; }
    if ((*s == '-') && *++s) {  /* -- if argument is an option */
      while (*s) {              /* traverse the options */
        switch (*s++) {         /* evaluate the options */
          case '!': help();                          break;
          case 't': target = (*s) ? *s++ : 's';      break;
          case 'm': zmin   = (ITEM)strtol(s, &s, 0); break;
          case 'n': zmax   = (ITEM)strtol(s, &s, 0); break;
          case 's': smin   =       strtod(s, &s);    break;
          case 'S': smax   =       strtod(s, &s);    break;
          case 'o': mode  |= ECL_ORIGSUPP;           break;
          case 'c': conf   =       strtod(s, &s);    break;
          case 'e': eval   = (*s) ? *s++ : 0;        break;
          case 'a': agg    = (*s) ? *s++ : 0;        break;
          case 'd': thresh =       strtod(s, &s);    break;
          case 'z': eflgs |= ECL_INVBXS;             break;
          case 'p': prune  = (ITEM)strtol(s, &s, 0); break;
          case 'q': sort   = (int) strtol(s, &s, 0); break;
          case 'A': algo   = (*s) ? *s++ : 0;        break;
          case 'x': mode  &= ~ECL_PERFECT;           break;
          case 'l': pack   = (int) strtol(s, &s, 0); break;
          case 'i': mode  &= ~ECL_REORDER;           break;
          case 'y': cmfilt = (int) strtol(s, &s, 0); break;
          case 'u': mode  &= ~ECL_TAIL;              break;
          case 'F': bdrcnt = getbdr(s, &s, &border); break;
          case 'R': optarg = &fn_sel;                break;
          case 'P': optarg = &fn_psp;                break;
          case 'Z': stats  = 1;                      break;
          case 'N': mode  &= ~ECL_PREFMT;            break;
          case 'g': scan   = 1;                      break;
          case 'h': optarg = &hdr;                   break;
          case 'k': optarg = &sep;                   break;
          case 'I': optarg = &imp;                   break;
          case 'v': optarg = &info;                  break;
          case 'w': mtar  |= TA_WEIGHT;              break;
          case 'r': optarg = &recseps;               break;
          case 'f': optarg = &fldseps;               break;
          case 'b': optarg = &blanks;                break;
          case 'C': optarg = &comment;               break;
          case 'T': optarg = &fn_tid;                break;
          default : error(E_OPTION, *--s);           break;
        }                       /* set the option variables */
        if (optarg && *s) { *optarg = s; optarg = NULL; break; }
      } }                       /* get an option argument */
    else {                      /* -- if argument is no option */
      switch (k++) {            /* evaluate non-options */
        case  0: fn_inp = s;      break;
        case  1: fn_out = s;      break;
        default: error(E_ARGCNT); break;
      }                         /* note filenames */
    }
  }
  if (optarg)       error(E_OPTARG);     /* check option arguments */
  if (k      < 1)   error(E_ARGCNT);     /* and number of arguments */
  if (zmin   < 0)   error(E_SIZE, zmin); /* check the size limits */
  if (zmax   < 0)   error(E_SIZE, zmax); /* and the minimum support */
  if (smin   > 100) error(E_SUPPORT, smin);
  if (bdrcnt < 0)   error(E_NOMEM);
  if ((conf  < 0) || (conf > 100))
    error(E_CONF, conf);        /* check the minimum confidence */
  if ((!fn_inp || !*fn_inp) && (fn_sel && !*fn_sel))
    error(E_STDIN);             /* stdin must not be used twice */
  switch (target) {             /* check and translate target type */
    case 's': target = ISR_ALL;              break;
    case 'f': target = ISR_FREQUENT;         break;
    case 'c': target = ISR_CLOSED;           break;
    case 'm': target = ISR_MAXIMAL;          break;
    case 'g': target = ISR_GENERAS;          break;
    case 'r': target = ISR_RULES;            break;
    default : error(E_TARGET, (char)target); break;
  }                             /* (get target type code) */
  switch (eval) {               /* check and translate measure */
    case 'x': eval = RE_NONE;                break;
    case 'o': eval = RE_SUPP;                break;
    case 'c': eval = RE_CONF;                break;
    case 'd': eval = RE_CONFDIFF;            break;
    case 'l': eval = RE_LIFT;                break;
    case 'a': eval = RE_LIFTDIFF;            break;
    case 'q': eval = RE_LIFTQUOT;            break;
    case 'v': eval = RE_CVCT;                break;
    case 'e': eval = RE_CVCTDIFF;            break;
    case 'r': eval = RE_CVCTQUOT;            break;
    case 'k': eval = RE_CPROB;               break;
    case 'j': eval = RE_IMPORT;              break;
    case 'z': eval = RE_CERT;                break;
    case 'n': eval = RE_CHI2;                break;
    case 'p': eval = RE_CHI2PVAL;            break;
    case 'y': eval = RE_YATES;               break;
    case 't': eval = RE_YATESPVAL;           break;
    case 'i': eval = RE_INFO;                break;
    case 'g': eval = RE_INFOPVAL;            break;
    case 'f': eval = RE_FETPROB;             break;
    case 'h': eval = RE_FETCHI2;             break;
    case 'm': eval = RE_FETINFO;             break;
    case 's': eval = RE_FETSUPP;             break;
    case 'b': eval = ECL_LDRATIO;            break;
    default : error(E_MEASURE, (char)eval);  break;
  }  /* free: u w */            /* (get evaluation measure code) */
  eval |= eflgs;                /* add evaluation measure flags */
  switch (agg) {                /* check and translate agg. mode */
    case 'x': agg  = ECL_NONE;               break;
    case 'm': agg  = ECL_MIN;                break;
    case 'n': agg  = ECL_MAX;                break;
    case 'a': agg  = ECL_AVG;                break;
    default : error(E_AGGMODE, (char)agg);   break;
  }                             /* (get aggregation mode code) */
  switch (algo) {               /* check and translate alg. variant */
    case 'a': algo = ECL_AUTO;               break;
    case 'e': algo = ECL_BASIC;              break;
    case 'i': algo = ECL_LISTS;              break;
    case 't': algo = ECL_TABLE;              break;
    case 'r': algo = ECL_RANGES;             break;
    case 'b': algo = ECL_BITS;               break;
    case 's': algo = ECL_SIMPLE;             break;
    case 'o': algo = ECL_OCCDLV;             break;
    case 'd': algo = ECL_DIFFS;              break;
    default : error(E_VARIANT, (char)algo);  break;
  }                             /* (get eclat algorithm code) */
  if ((cmfilt >= 0) && (target & (ISR_CLOSED|ISR_MAXIMAL)))
    mode |= (cmfilt > 0) ? ECL_VERT : ECL_HORZ;
  if (fn_tid) {                 /* if to write transaction ids. */
    if (strcmp(fn_tid, "-") == 0) fn_tid = "";
    mode |= ECL_TIDS;           /* turn "-" into "" for consistency */
  }                             /* set transaction identifier flag */
  mode = (mode & ~ECL_FIM16)    /* add packed items to search mode */
       | ((pack <= 0) ? 0 : (pack < 16) ? pack : 16);
  if (target & ISR_RULES)       /* if to find association rules, */
    fn_psp = NULL;              /* no pattern spectrum possible */
  if (info == dflt) {           /* if default info. format is used, */
    if (target != ISR_RULES)    /* set default according to target */
         info = (smin < 0) ? " (%a)"     : " (%S)";
    else info = (smin < 0) ? " (%b, %C)" : " (%X, %C)";
  }                             /* select absolute/relative support */
  mode |= ECL_VERBOSE|ECL_NOCLEAN;
  MSG(stderr, "\n");            /* terminate the startup message */

  /* --- read item selection/appearance indicators --- */
  ibase = ib_create(0, 0);      /* create an item base */
  if (!ibase) error(E_NOMEM);   /* to manage the items */
  tread = trd_create();         /* create a transaction reader */
  if (!tread) error(E_NOMEM);   /* and configure the characters */
  trd_allchs(tread, recseps, fldseps, blanks, "", comment);
  if (fn_sel) {                 /* if an item selection is given */
    CLOCK(t);                   /* start timer, open input file */
    if (trd_open(tread, NULL, fn_sel) != 0)
      error(E_FOPEN, trd_name(tread));
    MSG(stderr, "reading %s ... ", trd_name(tread));
    m = (target == ISR_RULES)   /* depending on the target type */
      ? ib_readapp(ibase,tread) /* read the item appearances */
      : ib_readsel(ibase,tread);/* or a simple item selection */
    if (m < 0) error((int)-m, ib_errmsg(ibase, NULL, 0));
    trd_close(tread);           /* close the input file */
    MSG(stderr, "[%"ITEM_FMT" item(s)]", ib_cnt(ibase));
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

  /* --- find frequent item sets/association rules --- */
  eclat = eclat_create(target, smin, smax, conf, zmin, zmax,
                       eval, agg, thresh, algo, mode);
  if (!eclat) error(E_NOMEM);   /* create an eclat miner */
  k = eclat_data(eclat, tabag, 0, sort);
  if (k) error(k);              /* prepare data for eclat */
  report = isr_create(ibase);   /* create an item set reporter */
  if (!report) error(E_NOMEM);  /* and configure it */
  k = eclat_report(eclat, report);
  if (k) error(k);              /* prepare reporter for eclat */
  if (setbdr(report, w, zmin, &border, bdrcnt) != 0)
    error(E_NOMEM);             /* set the support border (if any) */
  if (fn_psp && (isr_addpsp(report, NULL) < 0))
    error(E_NOMEM);             /* set a pattern spectrum if req. */
  if (isr_setfmt(report, scan, hdr, sep, imp, info) != 0)
    error(E_NOMEM);             /* set the output format strings */
  k = isr_tidopen(report, NULL, fn_tid);  /* open the file for */
  if (k) error(k, isr_tidname(report));   /* transaction ids */
  k = isr_open(report, NULL, fn_out);
  if (k) error(k, isr_name(report));
  if (isr_setup(report) < 0)    /* open the item set file and */
    error(E_NOMEM);             /* set up the item set reporter */
  k = eclat_mine(eclat, prune, 0);
  if (k) error(k);              /* find frequent item sets */
  if (stats)                    /* print item set statistics */
    isr_prstats(report, stdout, 0);
  if (isr_close   (report) != 0)/* close item set output file */
    error(E_FWRITE, isr_name(report));
  if (isr_tidclose(report) != 0)/* close trans. id output file */
    error(E_FWRITE, isr_tidname(report));

  /* --- write pattern spectrum --- */
  if (fn_psp) {                 /* if to write a pattern spectrum */
    CLOCK(t);                   /* start timer, create table write */
    psp    = isr_getpsp(report);/* get the pattern spectrum */
    twrite = twr_create();      /* create a table writer and */
    if (!twrite) error(E_NOMEM);/* open the output file */
    if (twr_open(twrite, NULL, fn_psp) != 0)
      error(E_FOPEN,  twr_name(twrite));
    MSG(stderr, "writing %s ... ", twr_name(twrite));
    if (psp_report(psp, twrite, 1.0) != 0)
      error(E_FWRITE, twr_name(twrite));
    twr_delete(twrite, 1);      /* write the pattern spectrum */
    twrite = NULL;              /* and delete the table writer */
    MSG(stderr, "[%"SIZE_FMT" signature(s)]", psp_sigcnt(psp));
    MSG(stderr, " done [%.2fs].\n", SEC_SINCE(t));
  }                             /* write a log message */

  /* --- clean up --- */
  CLEANUP;                      /* clean up memory and close files */
  SHOWMEM;                      /* show (final) memory usage */
  return 0;                     /* return 'ok' */
}  /* main() */

#endif  /* #ifdef ECL_MAIN ... */
