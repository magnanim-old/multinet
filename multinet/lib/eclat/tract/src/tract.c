/*----------------------------------------------------------------------
  File    : tract.c
  Contents: item and transaction management
  Author  : Christian Borgelt
  History : 1996.02.14 file created as apriori.c
            1996.06.24 item reading optimized (function readitem())
            1996.07.01 adapted to modified module symtab
            1998.01.04 scan functions moved to module tabread
            1998.06.09 array enlargement modified
            1998.06.20 adapted to changed st_create function
            1998.08.07 bug in function ib_read() fixed
            1998.08.08 item appearances added
            1998.08.17 item sorting and recoding added
            1999.10.22 bug in item appearances reading fixed
            1999.11.11 adapted to name/identifier maps
            1999.12.01 check of item appearance added to sort function
            2000.03.15 removal of infrequent items added
            2001.07.14 adapted to modified module tabread
            2001.11.18 transaction functions added to this module
            2001.12.28 first version of this module completed
            2002.01.12 empty field at end of record reported as error
            2002.02.06 item sorting made flexible (different orders)
            2002.02.19 transaction tree functions added
            2003.07.17 function tbg_filter() added (remove unused items)
            2003.08.15 bug in function tat_delete() fixed
            2003.08.21 parameter 'heap' added to function tbg_sort()
            2003.09.20 empty transactions in input made possible
            2003.12.18 padding for align8 architecture added
            2004.02.26 item frequency counting moved to ib_read()
            2005.06.20 function nocmp() for neutral sorting added
            2007.02.13 adapted to modified module tabread
            2008.08.11 item appearance indicator decoding improved
            2008.08.12 considerable redesign, some new functions
            2008.08.14 sentinels added to item arrays in transactions
            2008.11.13 item and transaction sorting generalized
            2008.11.19 transaction tree and tree node separated
            2009.05.28 bug in function tbg_filter() fixed (minimal size)
            2009.08.27 fixed definitions of trans. tree node functions
            2010.03.16 handling of extended transactions added
            2010.07.02 transaction size comparison functions added
            2010.07.13 function tbg_reduce() optimized (size comparison)
            2010.08.10 function tbg_trim() added (for sequences)
            2010.08.11 parameter of ib_read() changed to general mode
            2010.08.13 function tbg_addib() added (add from item base)
            2010.08.19 function ib_readsel() added (item selectors)
            2010.08.22 adapted to modified module tabread
            2010.08.30 handling of empty transactions corrected
            2010.09.13 function tbg_mirror() added (mirror transactions)
            2010.10.08 adapted to modfied module tabread
            2010.12.07 added several explicit type casts (for C++)
            2010.12.15 functions tbg_read() and tbg_recount() added
            2010.12.20 functions tbg_icnts() and tbg_ifrqs() added
            2011.03.20 item reading moved from readitem() to ib_read()
            2011.05.05 tbg_count() adapted to extended transactions
            2011.05.09 bug in reading weighted items fixed (duplicates)
            2011.07.09 interface for transaction bag recoding modified
            2011.07.12 adapted to modified symbol table/idmap interface
            2011.07.13 read functions adapted to optional integer names
            2011.07.18 alternative transaction tree implementation added
            2011.08.11 bug in function ib_read() fixed (APP_NONE)
            2011.08.23 negative transaction weights no longer prohibited
            2011.08.25 prefix and suffix packing functions added
            2011.09.02 bin sort for transactions added (faster)
            2011.09.17 handling of filling end markers in transactions
            2011.09.28 bin sort for trans. extended to packed items
            2012.03.22 function ta_cmplim() added (limited comparison)
            2012.05.25 function taa_reduce() added (for occ. deliver)
            2012.05.30 version of taa_reduce() with hash table added
            2012.06.15 number of packed items stored in transaction bag
            2012.07.03 twin prime table for hash table sizes added
            2012.07.19 optional parentheses around transaction weight
            2012.07.21 function tbg_ipwgt() added (idempotent weights)
            2012.07.23 functions ib_write() and tbg_write() added
            2012.07.26 function tbg_ipwgt() improved with item arrays
            2012.07.27 duplicate transactions in tbg_ipgwt() eliminated
            2012.07.30 item weight update added to function tbg_ipwgt()
            2013.02.04 ib_getapp()/ib_setapp() made proper functions
            2013.02.11 arbitrary objects/pointers allowed as item names
            2013.03.07 direction parameter added to sorting functions
            2013.03.18 adapted to modified bsearch/bisect interface
            2013.03.25 adapted to definitions for types TID and SUPP
            2013.03.28 adapted to definition  for type ITEM
            2013.06.19 bug in function wi_cmp() fixed (weight comp.)
            2013.07.09 static constants emsgs and errmsgs merged
            2013.11.07 transaction reading capability made optional
            2014.05.07 surrogate data generation functions added
            2014.05.23 correction of over-/under-rep. in tbg_random()
            2014.09.08 transaction marker functions added (ta_..mark())
            2014.09.09 function ib_frqcnt() added (num. of freq. items)
            2014.10.17 function ib_clear() made a proper function
            2014.10.24 changed from LGPL license to MIT license
            2015.02.27 more item appearance indicator strings added
----------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <limits.h>
#include <float.h>
#include <math.h>
#include <time.h>
#include <assert.h>
#include "tract.h"
#ifdef TA_MAIN
#include "error.h"
#endif
#ifdef STORAGE
#include "storage.h"
#endif

#ifdef _MSC_VER
#ifndef snprintf
#define snprintf    _snprintf
#endif
#endif                          /* MSC still does not support C99 */

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
  Preprocessor Definitions
----------------------------------------------------------------------*/
#define PRGNAME     "tract"
#define DESCRIPTION "benchmarking transaction sorting methods"
#define VERSION     "version 2.4 (2016.03.01)         " \
                    "(c) 2011-2016   Christian Borgelt"

/* --- error codes --- */
/* error codes   0 to  -4 defined in tract.h */
#define E_STDIN      (-5)       /* double assignment of stdin */
#define E_OPTION     (-6)       /* unknown option */
#define E_OPTARG     (-7)       /* missing option argument */
#define E_ARGCNT     (-8)       /* too few/many arguments */
#define E_ITEMCNT    (-9)       /* invalid number of items */
/* error codes -15 to -25 defined in tract.h */

#define BLKSIZE      1024       /* block size for enlarging arrays */
#define TH_INSERT       8       /* threshold for insertion sort */
#define TS_PRIMES    (sizeof(primes)/sizeof(*primes))

#ifndef QUIET                   /* if not quiet version, */
#define MSG         fprintf     /* print messages */
#define CLOCK(t)    ((t) = clock())
#else                           /* if quiet version, */
#define MSG(...)    ((void)0)   /* suppress messages */
#define CLOCK(t)    ((void)0)
#endif

#define SEC_SINCE(t)  ((double)(clock()-(t)) /(double)CLOCKS_PER_SEC)

#ifndef CCHAR
#define CCHAR const char        /* abbreviation */
#endif

/*----------------------------------------------------------------------
  Type Definitions
----------------------------------------------------------------------*/
typedef struct {                /* --- item frequency --- */
  ITEM item;                    /* item identifier */
  SUPP frq;                     /* number of occurrences */
  SUPP dif;                     /* difference to original */
} ITEMFRQ;                      /* (item frequency) */

typedef ITEM SUBFN  (const TRACT  *t1, const TRACT  *t2, ITEM off);
typedef ITEM SUBWFN (const WTRACT *t1, const WTRACT *t2, ITEM off);

/*----------------------------------------------------------------------
  Constants
----------------------------------------------------------------------*/
static WITEM  WTA_END  = {-1,0};/* sentinel for weighted items */
#ifdef TA_READ
static WITEM  WTA_TERM = { 0,0};/* termination item */
static int    sorted   = 0;     /* flag for sorted indicators */
static CCHAR* appmap[] = {      /* item appearance indicators */
  "0:-",  "0:n",    "0:none", "0:neither", "0:ignore",
  "1:i",  "1:in",   "1:inp",  "1:input",
  "1:a",  "1:ante", "1:antecedent", "1:b", "1:body",
  "2:o",  "2:out",  "2:output",
  "2:c",  "2:cons", "2:consequent", "2:h", "2:head",
  "3:io", "3:i&o",  "3:inout", "3:in&out",
  "3:ac", "3:a&c",  "3:c&a",   "3:canda",
  "3:bh", "3:b&h",  "3:h&b",   "3:both",
};                              /* (code:name) */
#endif

static size_t primes[] = {      /* table of twin prime numbers */
  19, 31, 61, 109, 241, 463, 1021, 2029, 4093, 8089, 16363, 32719,
  65521, 131011, 262111, 524221, 1048573, 2097133, 4193803, 8388451,
  16777141, 33554011, 67108669, 134217439, 268435009, 536870839,
  1073741719, 2147482951,
#if TID_MAX > 0xefffffff        /* if TID is 64 bit type */
  4294965841, 8589934291, 17179868809, 34359737299, 68719476391,
  137438953273, 274877906629, 549755813359, 1099511626399,
  2199023255479, 4398046509739, 8796093021409, 17592186044299,
  35184372088321, 70368744176779, 140737488353701, 281474976710131,
  562949953419319, 1125899906841973, 2251799813684461,
  4503599627369863, 9007199254738543, 18014398509481729,
  36028797018963799, 72057594037925809, 144115188075854269,
  288230376151709779, 576460752303422431, 1152921504606845473,
  2305843009213692031, 4611686018427386203, 9223372036854774511
#endif                          /* add more twin prime numbers */
};                              /* (table contains larger twins) */

#ifdef TA_READ
static CCHAR *errmsgs[] = {     /* error messages */
  /* E_NONE      0 */  "no error",
  /* E_NOMEM    -1 */  "not enough memory",
  /* E_FOPEN    -2 */  "cannot open file %s",
  /* E_FREAD    -3 */  "read error on file %s",
  /* E_FWRITE   -4 */  "write error on file %s",
  /* E_STDIN    -5 */  "double assignment of standard input",
  /* E_OPTION   -6 */  "unknown option -%c",
  /* E_OPTARG   -7 */  "missing option argument",
  /* E_ARGCNT   -8 */  "wrong number of arguments",
  /* E_ITEMCNT  -9 */  "invalid number of items (must be <= 16)",
  /*    -10 to -14 */  NULL, NULL, NULL, NULL, NULL,
  /* E_NOITEMS -15 */  "no (frequent) items found",
  /* E_ITEMEXP -16 */  "#item expected",
  /* E_ITEMWGT -17 */  "#invalid item weight %s",
  /* E_DUPITEM -18 */  "#duplicate item '%s'",
  /* E_INVITEM -19 */  "#invalid item '%s' (no integer)",
  /* E_WGTEXP  -20 */  "#transaction weight expected",
  /* E_TAWGT   -21 */  "#invalid transaction weight %s",
  /* E_FLDCNT  -22 */  "#too many fields/columns",
  /* E_APPEXP  -23 */  "#appearance indicator expected",
  /* E_UNKAPP  -24 */  "#unknown appearance indicator '%s'",
  /* E_PENEXP  -25 */  "#insertion penalty expected",
  /* E_PENALTY -26 */  "#invalid insertion penalty %s",
  /*           -27 */  "unknown error"
};
#endif

/*----------------------------------------------------------------------
  Global Variables
----------------------------------------------------------------------*/
#ifdef TA_MAIN
#ifndef QUIET
static CCHAR    *prgname;       /* program name for error messages */
#endif
static TABREAD  *tread = NULL;  /* table/transaction reader */
static ITEMBASE *ibase = NULL;  /* item base */
static TABAG    *tabag = NULL;  /* transaction bag/multiset */
#endif

#ifdef TA_READ                  /* if transaction reading capability */
static char msgbuf[2*TRD_MAXLEN+64];
#endif                          /* buffer for error messages */

/*----------------------------------------------------------------------
  Auxiliary Functions
----------------------------------------------------------------------*/
#ifdef TA_READ                  /* if transaction reading capability */

static int appcmp (const void *p1, const void *p2, void *data)
{                               /* --- compare appearance indicators */
  CCHAR *s1 = (CCHAR*)p1 +2;    /* type the two pointers */
  CCHAR *s2 = (CCHAR*)p2 +2;    /* to strings (skip code) */
  int   d;                      /* difference of character values */

  for ( ; 1; s1++, s2++) {      /* traverse the strings */
    d = (unsigned char)*s1 -(unsigned char)*s2;
    if (d   != 0) return d;     /* if strings differ, return result */
    if (*s1 == 0) return 0;     /* if at the end of the string, */
  }                             /* return 'strings are equal' */
}  /* appcmp() */

/*--------------------------------------------------------------------*/

static int appcode (CCHAR *s)
{                               /* --- get appearance indicator code */
  int   i, n;                   /* index, number of app. indicators */
  CCHAR *t;                     /* to traverse the indicator name */

  assert(s);                    /* check the function argument */
  n = (int)(sizeof(appmap)/sizeof(*appmap));
  if (!sorted) {                /* if app. indicators not sorted yet */
    ptr_qsort((void*)appmap, (size_t)n, +1, appcmp, NULL);
    sorted = -1;                /* sort the appearance indicators */
  }                             /* and set the sorted flag */
  i = (int)ptr_bisect(s-2, (void*)appmap, (size_t)n, appcmp, NULL);
  if (i >= n) return -1;        /* try to find appearance indicator */
  t = appmap[i] +2;             /* check for a (prefix) match */
  while (*s && (*s == *t)) s++, t++;
  if (*s) return -1;            /* if no match, abort the function */
  return appmap[i][0] -'0';     /* return the app. indicator code */
}  /* appcode() */

#endif
/*--------------------------------------------------------------------*/

static int nocmp (const void *p1, const void *p2, void *data)
{                               /* --- compare item identifiers */
  const ITEMDATA *a = (const ITEMDATA*)p1; /* type the pointers */
  const ITEMDATA *b = (const ITEMDATA*)p2;

  if (a->app == APP_NONE) return (b->app == APP_NONE) ? 0 : 1;
  if (b->app == APP_NONE) return -1;
  if (a->id  >  b->id)    return +1;
  if (a->id  <  b->id)    return -1;
  return 0;                     /* return sign of identifier diff. */
}  /* nocmp() */

/*--------------------------------------------------------------------*/

static int asccmp (const void *p1, const void *p2, void *data)
{                               /* --- compare item frequencies */
  const ITEMDATA *a = (const ITEMDATA*)p1; /* type the pointers */
  const ITEMDATA *b = (const ITEMDATA*)p2;

  if (a->app == APP_NONE) return (b->app == APP_NONE) ? 0 : 1;
  if (b->app == APP_NONE) return -1;
  if (a->frq >  b->frq)   return +1;
  if (a->frq <  b->frq)   return -1;
  return 0;                     /* return sign of frequency diff. */
}  /* asccmp() */

/*--------------------------------------------------------------------*/

static int descmp (const void *p1, const void *p2, void *data)
{                               /* --- compare item frequencies */
  const ITEMDATA *a = (const ITEMDATA*)p1; /* type the pointers */
  const ITEMDATA *b = (const ITEMDATA*)p2;

  if (a->app == APP_NONE) return (b->app == APP_NONE) ? 0 : 1;
  if (b->app == APP_NONE) return -1;
  if (a->frq <  b->frq)   return +1;
  if (a->frq >  b->frq)   return -1;
  return 0;                     /* return sign of frequency diff. */
}  /* descmp() */

/*--------------------------------------------------------------------*/

static int asccmpx (const void *p1, const void *p2, void *data)
{                               /* --- compare item frequencies */
  const ITEMDATA *a = (const ITEMDATA*)p1; /* type the pointers */
  const ITEMDATA *b = (const ITEMDATA*)p2;

  if (a->app == APP_NONE) return (b->app == APP_NONE) ? 0 : 1;
  if (b->app == APP_NONE) return -1;
  if (a->xfq >  b->xfq)   return +1;
  if (a->xfq <  b->xfq)   return -1;
  return 0;                     /* return sign of frequency diff. */
}  /* asccmpx() */

/*--------------------------------------------------------------------*/

static int descmpx (const void *p1, const void *p2, void *data)
{                               /* --- compare item frequencies */
  const ITEMDATA *a = (const ITEMDATA*)p1; /* type the pointers */
  const ITEMDATA *b = (const ITEMDATA*)p2;

  if (a->app == APP_NONE) return (b->app == APP_NONE) ? 0 : 1;
  if (b->app == APP_NONE) return -1;
  if (a->xfq <  b->xfq)   return +1;
  if (a->xfq >  b->xfq)   return -1;
  return 0;                     /* return sign of frequency diff. */
}  /* descmpx() */

/*----------------------------------------------------------------------
  Item Base Functions
----------------------------------------------------------------------*/

ITEMBASE* ib_create (int mode, ITEM size, ...)
{                               /* --- create an item base */
  ITEMBASE *base;               /* created item base */
  TRACT    *t;                  /* a transaction */
  WTRACT   *x;                  /* a transaction with weighted items */
  va_list  args;                /* list of variable arguments */
  HASHFN   *hashfn;             /* hash function for general objects */
  CMPFN    *cmpfn;              /* ditto, comparison function */
  void     *data;               /* data for comparison function */
  OBJFN    *delfn;              /* ditto, deletion function */

  if (size <= 0) size = BLKSIZE;/* check and adapt number of items */
  base = (ITEMBASE*)malloc(sizeof(ITEMBASE));
  if (!base) return NULL;       /* create item base and components */
  if (mode & IB_OBJNAMES) {     /* if general objects as names, */
    va_start(args, size);       /* get the necessary arguments */
    hashfn = va_arg(args, HASHFN*);
    cmpfn  = va_arg(args, CMPFN*);
    data   = va_arg(args, void*);
    delfn  = va_arg(args, OBJFN*);
    va_end(args);               /* create an item identifier map */
    base->idmap = idm_create(8191, 0, hashfn, cmpfn, data, delfn); }
  else                          /* if item names are strings */
    base->idmap = idm_create(8191, 0, ST_STRFN, (OBJFN*)0);
  if (!base->idmap) { free(base); return NULL; }
  base->mode = mode;            /* initialize the fields */
  base->wgt  = base->max = 0;   /* there are no transactions yet */
  base->app  = APP_BOTH;        /* default: appearance in body & head */
  base->pen  = 0.0;             /* default: no item insertion allowed */
  base->idx  = 1;               /* index of current transaction */
  base->size = size;            /* size of transaction buffer */
  if (mode & IB_WEIGHTS) {      /* if items carry weights */
    base->tract = x = (WTRACT*)malloc(                 sizeof(WTRACT)
                                    +(size_t)(size+1) *sizeof(WITEM));
    if (x) { x->wgt = 0; x->size = 0; x->mark = 0;
             x->items[size+1] = x->items[0] = WTA_END; } }
  else {                        /* if items do not carry weights */
    base->tract = t = (TRACT*) malloc(                 sizeof(TRACT)
                                    +(size_t)(size+1) *sizeof(ITEM));
    if (t) { t->wgt = 0; t->size = 0; t->mark = 0;
             t->items[size+1] = t->items[0] = TA_END; }
  }                             /* clear the transaction buffer */
  if (!base->tract) { ib_delete(base); return NULL; }
  #ifdef TA_READ                /* if transaction reading capability */
  base->err = 0;                /* clear the error code and */
  base->trd = NULL;             /* the table/transaction reader */
  #endif
  return base;                  /* return the created item set */
}  /* ib_create() */

/*--------------------------------------------------------------------*/

void ib_delete (ITEMBASE *base)
{                               /* --- delete an item set */
  assert(base);                 /* check the function argument */
  if (base->tract) free(base->tract);
  if (base->idmap) idm_delete(base->idmap);
  free(base);                   /* delete the components */
}  /* ib_delete() */            /* and the item base body */

/*--------------------------------------------------------------------*/

ITEM ib_add (ITEMBASE *base, const void *name)
{                               /* --- add an item to the set */
  size_t   size;                /* size of the item name */
  ITEMDATA *itd;                /* to access the item data */

  assert(base && name);         /* check the function arguments */
  size = (base->mode & IB_OBJNAMES) ? sizeof(const void*)
       : strlen((const char*)name)+1;
  itd = (ITEMDATA*)idm_add(base->idmap, name, size, sizeof(ITEMDATA));
  if (itd == NULL)   return -1; /* add the new item */
  if (itd == EXISTS) return -2; /* to the identifier map */
  itd->app = base->app;         /* init. the appearance indicator */
  itd->xfq = itd->frq = 0;      /* clear the support counters */
  itd->idx = 0;                 /* and the transaction index */
  itd->pen = base->pen;         /* init. the insertion penalty */
  return itd->id;               /* return the item identifier */
}  /* ib_add() */

/*--------------------------------------------------------------------*/

const char* ib_xname (ITEMBASE *base, ITEM item)
{                               /* --- get an item name */
  static char buf[32];          /* buffer for integer formatting */

  assert(base && (item >= 0));  /* check the function arguments */
  if (!(base->mode & IB_OBJNAMES))
    return ib_name(base, item); /* if possible, return name directly */
  snprintf(buf, sizeof(buf), "%p", ib_name(base, item));
  return buf;                   /* format the object pointer and */
}  /* ib_xname() */             /* return the formatting buffer */

/*--------------------------------------------------------------------*/

void ib_clear (ITEMBASE *base)
{                               /* --- clear buffered transaction */
  ((TRACT*)base->tract)->size = 0;
  base->idx += 1;               /* reset the number of items and */
}  /* ib_clear() */             /* update the transaction index */

/*--------------------------------------------------------------------*/

ITEM ib_add2ta (ITEMBASE *base, const void *name)
{                               /* --- add an item to transaction */
  size_t   size;                /* size of item name/buffer */
  ITEMDATA *itd;                /* to access the item data */
  TRACT    *t;                  /* to access the transaction */
  ITEM     n;                   /* size of transaction buffer */

  assert(base && name);         /* check the function arguments */
  itd = (ITEMDATA*)idm_bykey(base->idmap, name);
  if (!itd) {                   /* get the item by its key/name */
    size = (base->mode & IB_OBJNAMES) ? sizeof(const void*)
         : strlen((const char*)name)+1;
    itd = (ITEMDATA*)idm_add(base->idmap, name, size, sizeof(ITEMDATA));
    if (itd == NULL) return -1; /* add a new item to the base */
    itd->app = base->app;       /* init. the appearance indicator */
    itd->xfq = itd->frq = 0;    /* clear the support counters */
    itd->idx = 0;               /* and the transaction index */
    itd->pen = base->pen;       /* init. the insertion penalty */
  }
  t = (TRACT*)base->tract;      /* get the transaction buffer */
  if (itd->idx >= base->idx)    /* if the item is already contained, */
    return t->size;             /* simply abort the function */
  itd->idx = base->idx;         /* update the transaction index */
  n = base->size;               /* get the current buffer size */
  if (t->size >= n) {           /* if the transaction buffer is full */
    n += (n > BLKSIZE) ? (n >> 1) : BLKSIZE;
    t  = (TRACT*)realloc(t, sizeof(TRACT) +(size_t)n *sizeof(ITEM));
    if (!t) return -1;          /* enlarge the transaction buffer */
    t->items[base->size = n] = TA_END; base->tract = t;
  }                             /* set the new buffer and its size */
  t->items[t->size] = itd->id;  /* store the new item */
  return ++t->size;             /* return the new transaction size */
}  /* ib_add2ta() */

/*--------------------------------------------------------------------*/

void ib_finta (ITEMBASE *base, SUPP wgt)
{                               /* --- finalize transaction buffer */
  ITEM     i;                   /* loop variable */
  TRACT    *t;                  /* to access the transaction buffer */
  ITEMDATA *itd;                /* to access the item data */

  assert(base);                 /* check the function arguments */
  t = (TRACT*)base->tract;      /* get the transaction buffer and */
  t->items[t->size] = TA_END;   /* store a sentinel at the end */
  base->wgt += t->wgt = wgt;    /* sum the transaction weight and */
  wgt *= (SUPP)t->size;         /* compute extended frequency weight */
  for (i = 0; i < t->size; i++) {
    itd = (ITEMDATA*)idm_byid(base->idmap, t->items[i]);
    itd->xfq += wgt;            /* traverse the items and */
    itd->frq += t->wgt;         /* sum the transaction weights */
    if (itd->frq > base->max) base->max = itd->frq;
  }                             /* update maximum item support */
}  /* ib_finta() */

/*--------------------------------------------------------------------*/

int ib_getapp (ITEMBASE *base, ITEM item)
{                               /* --- get item appearance */
  assert(base);                 /* check the function arguments */
  if (item < 0) return base->app;
  return ((ITEMDATA*)idm_byid(base->idmap, item))->app;
}  /* ib_getapp() */

/*--------------------------------------------------------------------*/

int ib_setapp (ITEMBASE *base, ITEM item, int app)
{                               /* --- get item appearance */
  assert(base);                 /* check the function arguments */
  if (item < 0) return base->app = app;
  return ((ITEMDATA*)idm_byid(base->idmap, item))->app = app;
}  /* ib_setapp() */

/*--------------------------------------------------------------------*/

ITEM ib_frqcnt (ITEMBASE *base, SUPP smin)
{                               /* --- get number of frequent items */
  ITEM i, n = 0;                /* loop variable, counter */

  assert(base);                 /* check the function arguments */
  for (i = idm_cnt(base->idmap); --i >= 0; )
    if (((ITEMDATA*)idm_byid(base->idmap, i))->frq >= smin)
      n += 1;                   /* count the frequent items */
  return n;                     /* and return their number */
}  /* ib_frqcnt() */

/*--------------------------------------------------------------------*/
#ifdef TA_READ                  /* if transaction reading capability */

int ib_readsel (ITEMBASE *base, TABREAD *trd)
{                               /* --- read item selection */
  int      d;                   /* delimiter type */
  char     *b;                  /* buffer for a field */
  ITEMDATA *itd;                /* to access the item data */

  assert(base && trd);          /* check the function arguments */
  base->trd = trd;              /* note the table reader and set */
  base->app = APP_NONE;         /* the default appearance indicator */
  while (1) {                   /* read a simple list of items */
    d = trd_read(trd);          /* read the next item */
    if (d <= TRD_ERR)  return base->err = E_FREAD;
    if (d <= TRD_EOF)  return base->err = 0;
    b = trd_field(trd);         /* check whether an item was read */
    if (!*b)           return base->err = E_ITEMEXP;
    itd = (ITEMDATA*)idm_add(base->idmap, b,
                             trd_len(trd)+1, sizeof(ITEMDATA));
    if (itd == NULL)   return base->err = E_NOMEM;
    if (itd == EXISTS) continue;/* add new item, skip known */
    itd->app = APP_BOTH;        /* init. the appearance indicator */
    itd->xfq = itd->frq = 0;    /* clear the support counters */
    itd->idx = 0;               /* and the transaction index */
    itd->pen = base->pen;       /* init. the insertion penalty */
  }
}  /* ib_readsel() */

/*--------------------------------------------------------------------*/

int ib_readapp (ITEMBASE *base, TABREAD *trd)
{                               /* --- read appearance indicators */
  int      d, a;                /* delimiter type, app. indicator */
  char     *b;                  /* buffer for a field */
  ITEMDATA *itd;                /* to access the item data */

  assert(base && trd);          /* check the function arguments */
  base->trd = trd;              /* note the table reader */
  d = trd_read(trd);            /* read the first record */
  if (d <= TRD_ERR)    return base->err = E_FREAD;
  if (d != TRD_REC)    return base->err = E_FLDCNT;
  a = appcode(trd_field(trd));  /* get and check appearance code */
  if (a < 0)           return base->err = E_UNKAPP;
  base->app = a;                /* store default appearance indicator */
  while (1) {                   /* read item/indicator pairs */
    d = trd_read(trd);          /* read the next item */
    if (d <= TRD_ERR)  return base->err = E_FREAD;
    if (d <= TRD_EOF)  return base->err = 0;
    b = trd_field(trd);         /* check whether an item was read */
    if (!*b)           return base->err = E_ITEMEXP;
    itd = (ITEMDATA*)idm_add(base->idmap, b,
                             trd_len(trd)+1, sizeof(ITEMDATA));
    if (itd == NULL)   return base->err = E_NOMEM;
    if (itd == EXISTS) return base->err = E_DUPITEM;
    itd->app = base->app;       /* init. the appearance indicator */
    itd->xfq = itd->frq = 0;    /* clear the support counters */
    itd->idx = 0;               /* and the transaction index */
    itd->pen = base->pen;       /* init. the insertion penalty */
    if (d != TRD_FLD)  return base->err = E_APPEXP;
    d = trd_read(trd);          /* read the item appearance code */
    if (d <= TRD_ERR)  return base->err = E_FREAD;
    if (d == TRD_FLD)  return base->err = E_FLDCNT;
    a = appcode(trd_field(trd));
    if (a < 0)         return base->err = E_UNKAPP;
    itd->app = a;               /* get, check and store */
  }                             /* the appearance indicator */
}  /* ib_readapp() */

/*--------------------------------------------------------------------*/

int ib_readpen (ITEMBASE *base, TABREAD *trd)
{                               /* --- read insertion penalties */
  int      d;                   /* delimiter type */
  double   p;                   /* item insertion penalty */
  char     *b, *e;              /* buffer for a field, end pointer */
  ITEMDATA *itd;                /* to access the item data */

  assert(base && trd);          /* check the function arguments */
  base->trd = trd;              /* note the table reader */
  d = trd_read(trd);            /* read the first record */
  if (d <= TRD_ERR)    return base->err = E_FREAD;
  if (d != TRD_REC)    return base->err = E_FLDCNT;
  b = trd_field(trd);           /* get default insertion penalty */
  p = strtod(b, &e);            /* and convert and check it */
  if (*e || (e == b) || (p > 1)) return base->err = E_PENALTY;
  if (p < 0) { base->app = APP_NONE; p = 0; }
  else       { base->app = APP_BOTH; }
  base->pen = p;                /* store default insertion penalty */
  while (1) {                   /* read item/indicator pairs */
    d = trd_read(trd);          /* read the next item */
    if (d <= TRD_ERR)  return base->err = E_FREAD;
    if (d <= TRD_EOF)  return base->err = 0;
    b = trd_field(trd);         /* check whether an item was read */
    if (!*b)           return base->err = E_ITEMEXP;
    itd = (ITEMDATA*)idm_add(base->idmap, b,
                             trd_len(trd)+1, sizeof(ITEMDATA));
    if (itd == NULL)   return base->err = E_NOMEM;
    if (itd == EXISTS) return base->err = E_DUPITEM;
    itd->app = base->app;       /* init. the appearance indicator */
    itd->xfq = itd->frq = 0;    /* clear the support counters */
    itd->idx = 0;               /* and the transaction index */
    itd->pen = base->pen;       /* init. the insertion penalty */
    if (d != TRD_FLD)  return base->err = E_PENEXP;
    d = trd_read(trd);          /* read the insertion penalty */
    if (d <= TRD_ERR)  return base->err = E_FREAD;
    if (d == TRD_FLD)  return base->err = E_FLDCNT;
    b = trd_field(trd);         /* get the insertion penalty */
    p = strtod(b, &e);          /* and convert and check it */
    if (*e || (e == b) || (p > 1)) return base->err = E_PENALTY;
    if (p < 0) { itd->app = APP_NONE; p = 0; }
    else       { itd->app = APP_BOTH; }
    itd->pen = p;               /* store item appearence indicator */
  }                             /* and the insertion penalty */
}  /* ib_readpen() */

/*--------------------------------------------------------------------*/

int ib_read (ITEMBASE *base, TABREAD *trd, int mode)
{                               /* --- read a transaction */
  ITEM     i, n;                /* item identifier, buffer size */
  SUPP     w;                   /* transaction/item weight */
  int      d, p;                /* delimiter type, parenthesis */
  char     *b, *e;              /* read buffer and end pointer */
  ITEMDATA *itd;                /* to access the item data */
  WITEM    *wit;                /* to access the weighted items */
  TRACT    *t;                  /* to access the transaction */
  WTRACT   *x;                  /* to access the transaction */
  double   z;                   /* buffer for an item weight */

  assert(base && trd);          /* check the function arguments */
  base->trd = trd;              /* note the table reader */
  ++base->idx;                  /* update the transaction index and */
  x = (WTRACT*)base->tract;     /* get the transaction buffers */
  t = (TRACT*) base->tract;     /* set the default transaction weight */
  t->wgt = 1; t->size = 0;      /* and init. the transaction size */
  do {                          /* read the transaction items */
    d = trd_read(trd);          /* read the next field (item name) */
    if (d <= TRD_ERR) return base->err = E_FREAD;
    if (d <= TRD_EOF) return base->err = 1;
    b = trd_field(trd);         /* check whether field is empty */
    if ((d == TRD_REC)          /* if at the last field of a record */
    &&  (mode & TA_WEIGHT)) {   /* and to read transaction weights */
      if (!*b) return base->err = E_WGTEXP;
      p = ((*b == '(') || (*b == '[') || (*b == '{'))
        ? *b++ : 0;             /* skip an opening parenthesis */
      t->wgt = strtosupp(b,&e); /* get the weight (multiplicity) */
      if ((e == b) || (p && !*e))
        return base->err = E_TAWGT;
      if (((p == '(') && (*e == ')'))
      ||  ((p == '[') && (*e == ']'))
      ||  ((p == '{') && (*e == '}')))
        e++;                    /* skip a closing parenthesis */
      if (*e) return base->err = E_TAWGT;
      break;                    /* check for following garbage and */
    }                           /* abort the item set read loop */
    if (!*b) {                  /* if the field is empty, */
      if (d == TRD_REC) break;  /* the transaction must be empty */
      return base->err = E_ITEMEXP;
    }                           /* otherwise there must be an item */
    itd = (ITEMDATA*)idm_bykey(base->idmap, b);
    if (!itd) {                 /* if the item is not known yet */
      if (base->app == APP_NONE) { /* if new items are to be ignored */
        if ((base->mode & IB_WEIGHTS)
        &&  trd_istype(trd, trd_last(trd), TA_WGTSEP))
          d = trd_read(trd);    /* consume a possible item weight */
        if (d <= TRD_ERR) return base->err = E_FREAD;
        if (d == TRD_REC) break;/* if at the end of a record, */
        continue;               /* abort the item read loop, */
      }                         /* otherwise read the next field */
      itd = (ITEMDATA*)idm_add(base->idmap, b,
                               trd_len(trd)+1, sizeof(ITEMDATA));
      if (!itd) return base->err = E_NOMEM;
      itd->app = base->app;     /* add the new item to the map */
      itd->xfq = itd->frq = 0;  /* clear the support counters */
      itd->idx = 0;             /* and the transaction index */
      itd->pen = base->pen;     /* init. the insertion penalty */
    }
    if (itd->idx >= base->idx){ /* if the item is already contained, */
      if   (mode & TA_DUPERR)   /* check what to do with duplicates */
        return base->err = E_DUPITEM;
      if (!(mode & TA_DUPLICS)){/* if to ignore duplicates */
        if ((base->mode & IB_WEIGHTS)
        &&  trd_istype(trd, trd_last(trd), TA_WGTSEP)) {
          d = trd_read(trd);    /* read a given item weight */
          if (d <= TRD_ERR)   return base->err = E_FREAD;
          z = strtod(b = trd_field(trd), &e);
          (void)z;              /* prevent 'set but not used' warning */
          if ((e == b) || *e) return base->err = E_ITEMWGT;
        } continue;             /* check the read item weight */
      }                         /* (even though it will be ignored) */
    }                           /* and then read the next item */
    itd->idx = base->idx;       /* update the transaction index */
    n = base->size;             /* get the current buffer size */
    if (base->mode & IB_WEIGHTS) { /* if the items carry weights */
      if (x->size >= n) {       /* if the transaction buffer is full */
        n += (n > BLKSIZE) ? (n >> 1) : BLKSIZE;
        x  = (WTRACT*)realloc(x, sizeof(WTRACT)
                 +(size_t)(n+1) *sizeof(WITEM));
        if (!x) return base->err = E_NOMEM;
        base->size = n; x->items[n+1] = WTA_END; base->tract = x;
      }                         /* enlarge the transaction buffer */
      wit = x->items +x->size++;/* get the next transaction item */
      wit->item = itd->id;      /* and store the item identifier */
      if (!trd_istype(trd, trd_last(trd), TA_WGTSEP))
        wit->wgt = 1;           /* if no weight separator follows, */
      else {                    /* set default weight, otherwise */
        d = trd_read(trd);      /* read the given item weight */
        if (d <= TRD_ERR)   return base->err = E_FREAD;
        wit->wgt = (float)strtod(b = trd_field(trd), &e);
        if ((e == b) || *e) return base->err = E_ITEMWGT;
      } }                       /* decode and set the item weight */
    else {                      /* if the items do not carry weights */
      if (t->size >= n) {       /* if the transaction buffer is full */
        n += (n > BLKSIZE) ? (n >> 1) : BLKSIZE;
        t  = (TRACT*)realloc(t, sizeof(TRACT)
                +(size_t)(n+1) *sizeof(ITEM));
        if (!t) return base->err = E_NOMEM;
        base->size = n; t->items[n+1] = TA_END; base->tract = t;
      }                         /* enlarge the transaction buffer */
      t->items[t->size++] = itd->id;
    }                           /* add the item to the transaction */
  } while (d == TRD_FLD);       /* while not at end of record */
  if (base->mode & IB_WEIGHTS){ /* if the items carry weights */
    x = (WTRACT*)base->tract;   /* get the transaction buffer and */
    if (mode & TA_TERM)         /* if requested, store term. item */
      x->items[x->size++] = WTA_TERM;
    x->items[x->size] = WTA_END;/* store sentinel after the items */
    base->wgt += x->wgt;        /* sum the transaction weight */
    w = (SUPP)x->size *x->wgt;  /* compute extended frequency weight */
    for (i = 0; i < x->size; i++) {
      itd = (ITEMDATA*)idm_byid(base->idmap, x->items[i].item);
      itd->xfq += w;            /* traverse the items and */
      itd->frq += x->wgt;       /* sum the transaction weights */
      if (itd->frq > base->max) base->max = itd->frq;
    } }                         /* update maximum item support */
  else {                        /* if the items do not carry weights */
    t = (TRACT*)base->tract;    /* get the transaction buffer and */
    if (mode & TA_TERM)         /* if requested, store term. item */
      t->items[t->size++] = 0;  /* (item 0 to indicate sequence end) */
    t->items[t->size] = TA_END; /* store a sentinel after the items */
    base->wgt += t->wgt;        /* sum the transaction weight and */
    w = (SUPP)t->size *t->wgt;  /* compute extended frequency weight */
    for (i = 0; i < t->size; i++) {
      itd = (ITEMDATA*)idm_byid(base->idmap, t->items[i]);
      itd->xfq += w;            /* traverse the items and */
      itd->frq += t->wgt;       /* sum the transaction weights */
      if (itd->frq > base->max) base->max = itd->frq;
    }                           /* update maximum item support */
  }
  return base->err = 0;         /* return 'ok' */
}  /* ib_read() */

/*--------------------------------------------------------------------*/

const char* ib_errmsg (ITEMBASE *base, char *buf, size_t size)
{                               /* --- get last (read) error message */
  int        i;                 /* index of error message */
  size_t     k = 0;             /* buffer for header length */
  const char *msg;              /* error message (format) */

  assert(base                   /* check the function arguments */
  &&    (!buf || (size > 0)));  /* if none given, get internal buffer */
  if (!buf) { buf = msgbuf; size = sizeof(msgbuf); }
  i = (base->err < 0) ? -base->err : 0;
  assert(i < (int)(sizeof(errmsgs)/sizeof(char*)));
  msg = errmsgs[i];             /* get the error message (format) */
  assert(msg);                  /* check for a proper message */
  if (*msg == '#') { msg++;     /* if message needs a header */
    k = (size_t)snprintf(buf, size, "%s:%"SIZE_FMT"(%"SIZE_FMT"): ",
                         TRD_FPOS(base->trd));
    if (k >= size) k = size-1;  /* print the input file name and */
  }                             /* the record and field number */
  snprintf(buf+k, size-k, msg, trd_field(base->trd));
  return buf;                   /* format the error message */
}  /* ib_errmsg() */

#endif
/*--------------------------------------------------------------------*/
#ifdef TA_WRITE

int ib_write (ITEMBASE *base, TABWRITE *twr, const char *wgtfmt, ...)
{                               /* --- write a transaction */
  va_list    args;              /* list of variable arguments */
  const char *iwfmt;            /* item weight format */

  assert(base && twr);          /* check the function arguments */
  if (!(base->mode & IB_WEIGHTS))
    return ta_write((TRACT*)base->tract, base, twr, wgtfmt);
  va_start(args, wgtfmt);       /* start variable arguments */
  iwfmt = va_arg(args, const char*);
  wta_write((WTRACT*)base->tract, base, twr, wgtfmt, iwfmt);
  va_end(args);                 /* write the transactions and */
  return (twr_error(twr)) ? E_FWRITE : 0;
}  /* ib_write() */             /* return a write error indicator */

#endif
/*--------------------------------------------------------------------*/

ITEM ib_recode (ITEMBASE *base, SUPP min, SUPP max,
                ITEM cnt, int dir, ITEM *map)
{                               /* --- recode items w.r.t. frequency */
  ITEM     k, n;                /* loop variables */
  ITEM     i;                   /* item buffer */
  ITEMDATA *itd;                /* to traverse the items */
  TRACT    *t;                  /* to access the standard transaction */
  ITEM     *s, *d;              /* to traverse the items */
  WTRACT   *x;                  /* to access the extended transaction */
  WITEM    *a, *b;              /* to traverse the items */
  CMPFN    *cmp;                /* comparison function */

  assert(base);                 /* check the function arguments */
  if (max < 0) max = SUPP_MAX;  /* adapt the maximum frequency */
  if (cnt < 0) cnt = ITEM_MAX;  /* adapt the maximum number of items */
  for (n = idm_cnt(base->idmap); --n >= 0; ) {
    itd = (ITEMDATA*)idm_byid(base->idmap, n);
    if ((itd->frq < min) || (itd->frq > max))
      itd->app = APP_NONE;      /* set all items to 'ignore' */
  }                             /* that have an invalid frequency */
  if      (dir >  1) cmp = asccmpx;  /* get the appropriate */
  else if (dir >  0) cmp = asccmp;   /* comparison function */
  else if (dir >= 0) cmp = nocmp;    /* (ascending/descending) */
  else if (dir > -2) cmp = descmp;   /* and sort the items */
  else               cmp = descmpx;  /* w.r.t. their frequency */
  idm_sort(base->idmap, cmp, NULL, map, 1);
  for (k = n = idm_cnt(base->idmap); n > 0; n--)
    if (((ITEMDATA*)idm_byid(base->idmap, n-1))->app != APP_NONE)
      break;                    /* find non-ignorable items */
  if (n > cnt) n = cnt;         /* limit the number of items */
  idm_trunc(base->idmap, (size_t)n); /* remove all items to ignore */
  if (!map) return n;           /* if no map is provided, abort */
  while (k > 0)                 /* mark all removed items */
    if (map[--k] >= n) map[k] = -1;
  if (base->mode & IB_WEIGHTS){ /* if the items carry weights */
    x = (WTRACT*)base->tract;   /* traverse the buffered transaction */
    for (a = b = x->items; a->item >= 0; a++) {
      i = map[a->item];         /* recode all items and remove */
      if (i >= 0) (b++)->item = i;      /* all items to ignore */
    }                           /* from the buffered transaction */
    x->size = (ITEM)(b -x->items); /* compute the new number of items */
    x->items[x->size] = WTA_END; } /* store sentinel after the items */
  else {                        /* if the items do not carry weights */
    t = (TRACT*)base->tract;    /* traverse the buffered transaction */
    for (s = d = t->items; *s > TA_END; s++) {
      i = map[*s];              /* recode all items and */
      if (i >= 0) *d++ = i;     /* remove all items to ignore */
    }                           /* from the buffered transaction */
    t->size = (ITEM)(d -t->items);
    t->items[t->size] = TA_END; /* compute the new number of items */
  }                             /* store a sentinel after the items */
  return n;                     /* return number of frequent items */
}  /* ib_recode() */

/*--------------------------------------------------------------------*/

void ib_trunc (ITEMBASE *base, ITEM cnt)
{                               /* --- truncate an item base */
  TRACT  *t;                    /* to access the standard transaction */
  ITEM   *s, *d;                /* to traverse the items */
  WTRACT *x;                    /* to access the extended transaction */
  WITEM  *a, *b;                /* to traverse the items */

  assert(base && (cnt >= 0));   /* check the function arguments */
  idm_trunc(base->idmap, (size_t)cnt); /* truncate the item base */
  if (base->mode & IB_WEIGHTS){ /* if the items carry weights */
    x = (WTRACT*)base->tract;   /* traverse the buffered transaction */
    for (a = b = x->items; a->item >= 0; a++)
      if (a->item < cnt) *b++ = *a;/* remove all deleted items */
    x->size = (ITEM)(b -x->items); /* compute the new number of items */
    x->items[x->size] = WTA_END; } /* store sentinel after the items */
  else {                        /* if the items do not carry weights */
    t = (TRACT*)base->tract;    /* traverse the buffered transaction */
    for (s = d = t->items; *s > TA_END; s++)
      if (*s < cnt) *d++ = *s;  /* remove all deleted items */
    t->size = (ITEM)(d -t->items); /* compute the new number of items */
    t->items[t->size] = TA_END;    /* store sentinel after the items */
  }                             /* (adapt the buffered transaction) */
}  /* ib_trunc() */

/*--------------------------------------------------------------------*/
#ifndef NDEBUG

void ib_show (ITEMBASE *base)
{                               /* --- show an item base */
  ITEM     i, n;                /* loop variables */
  ITEMDATA *itd;                /* to traverse the items */

  assert(base);                 /* check the function argument */
  for (n = ib_cnt(base), i = 0; i < n; i++) {
    printf("%-16s: ", ib_xname(base, i));
    itd = (ITEMDATA*)idm_byid(base->idmap, i);
    printf("id %6"ITEM_FMT", app ", itd->id);
    fputc((itd->app & APP_HEAD) ? 'h' : '-', stdout);
    fputc((itd->app & APP_BODY) ? 'b' : '-', stdout);
    printf(" pen %8.6g, ",       itd->pen);
    printf("frq %7"SUPP_FMT", ", itd->frq);
    printf("xfq %7"SUPP_FMT"\n", itd->xfq);
  }                             /* traverse items and print info */
  printf("%"ITEM_FMT" item(s)\n", idm_cnt(base->idmap));
}  /* ib_show() */

#endif
/*----------------------------------------------------------------------
  Transaction Functions
----------------------------------------------------------------------*/

TRACT* ta_create (const ITEM *items, ITEM n, SUPP wgt)
{                               /* --- create a transaction */
  TRACT *t;                     /* created transaction */

  assert(items || (n <= 0));    /* check the function arguments */
  t = (TRACT*)malloc(sizeof(TRACT) +(size_t)n *sizeof(ITEM));
  if (!t) return NULL;          /* allocate a new transaction */
  t->wgt  = wgt;                /* set weight, size and marker */
  t->size = n; t->mark = 0;     /* and copy the items */
  memcpy(t->items, items, (size_t)n *sizeof(ITEM));
  t->items[n] = TA_END;         /* store a sentinel after the items */
  return t;                     /* and return the created transaction */
}  /* ta_create() */

/*--------------------------------------------------------------------*/

TRACT* ta_clone (const TRACT *t)
{ return ta_create(t->items, t->size, t->wgt); }

/*--------------------------------------------------------------------*/

TRACT* ta_copy (TRACT *dst, const TRACT *src)
{                               /* --- copy a transaction */
  assert(dst                    /* check the function arguments */
  &&     src && (dst->size == src->size));
  dst->wgt = src->wgt;          /* copy weight and items */
  memcpy(dst->items, src->items, (size_t)(src->size+1) *sizeof(ITEM));
  return dst;                   /* return the destination */
}  /* ta_copy() */

/*--------------------------------------------------------------------*/

int ta_bitmark (TRACT *t)
{                               /* --- set mark to item bits */
  int  mark;                    /* bits representing items 0 to 31 */
  ITEM *s;                      /* to traverse the items */

  assert(t);                    /* check the function argument */
  for (mark = 0, s = t->items; *s > TA_END; s++) {
    if      (*s <  0) mark |= *s & ~TA_END;
    else if (*s < 32) mark |= 1 << *s;
  }                             /* set bits for items 0 to 31 */
  return t->mark = mark;        /* store and return the item bits */
}  /* ta_bitmark() */

/*--------------------------------------------------------------------*/

void ta_sort (TRACT *t, int dir)
{                               /* --- sort items in transaction */
  ITEM n;                       /* number of items */

  assert(t);                    /* check the function argument */
  if ((n = t->size) < 2) return;/* do not sort less than two items */
  while ((n > 0) && (t->items[n-1] <= TA_END))
    --n;                        /* skip additional end markers and */
  ia_qsort(t->items, (size_t)n, dir);  /* sort the remaining items */
}  /* ta_sort() */

/*--------------------------------------------------------------------*/

void ta_reverse (TRACT *t)
{                               /* --- reverse items in transaction */
  ITEM n;                       /* number of items */

  assert(t);                    /* check the function argument */
  if ((n = t->size) < 2) return;/* do not reverse less than two items */
  while ((n > 0) && (t->items[n-1] <= TA_END))
    --n;                        /* skip additional end markers and */
  ia_reverse(t->items, (size_t)n);  /* reverse the remaining items */
}  /* ta_reverse() */

/*--------------------------------------------------------------------*/

ITEM ta_unique (TRACT *t)
{                               /* --- make items unique */
  ITEM k, n;                    /* number of items */

  assert(t);                    /* check the function argument */
  if ((n = t->size) < 2)        /* do not process less than two items */
    return n;                   /* (cannot contain duplicates) */
  while ((n > 0) && (t->items[n-1] <= TA_END))
    --n;                        /* skip additional end markers and */
  k = (ITEM)ia_unique(t->items, (size_t)n);   /* remove duplicates */
  t->size -= n-k;               /* adapt the transaction size */
  while (k < t->size)           /* if necessary, fill again */
    t->items[k++] = TA_END;     /* with additional end markers */
  return t->size;               /* return the new number of items */
}  /* ta_unique() */

/*--------------------------------------------------------------------*/

ITEM ta_pack (TRACT *t, int n)
{                               /* --- pack items with codes 0--(n-1) */
  ITEM b;                       /* packed items (bit array) */
  ITEM *s, *d, *p;              /* to traverse the items */

  assert(t);                    /* check the function arguments */
  if (n <= 0) return 0;         /* if no items to pack, abort */
  if (n > 31) n = 31;           /* pack at most 31 items */
  for (s = t->items; *s > TA_END; s++)
    if (*s < n) break;          /* find item with code < n */
  if (*s <= TA_END) return 0;   /* if there is none, abort */
  p = d = s;                    /* note the destination location */
  for (b = 0; *s > TA_END; s++) {
    if      (*s < 0) b |= *s;   /* traverse the remaining items */
    else if (*s < n) b |= 1 << *s;
    else             *++d = *s; /* set bits for items with codes < n */
  }                             /* and copy the other items */
  *p = b | TA_END;              /* store the packed items (bit rep.) */
  while (++d < s) *d = TA_END;  /* clear rest of the transaction */
  return b & ~TA_END;           /* return bit rep. of packed items */
}  /* ta_pack() */

/*--------------------------------------------------------------------*/

ITEM ta_unpack (TRACT *t, int dir)
{                               /* --- unpack items (undo ta_pack()) */
  ITEM p, q, i, k;              /* packed items, loop variables */
  ITEM *s, *d;                  /* to traverse the items */

  assert(t);                    /* check the function arguments */
  for (d = t->items; *d >= 0; d++);  /* search for packed items */
  if (*d <= TA_END) return 0;   /* if there are none, abort */
  for (i = k = 0, q = p = *d & ~TA_END; q; q >>= 1) {
    i += q & 1; k++; }          /* get and count the packed items */
  for (s = d+1; *s > TA_END; s++);    /* find end of transaction */
  memmove(d+i, d+1, (size_t)(s-d) *sizeof(ITEM));   /* move rest */
  if (dir < 0) {                /* if negative direction requested, */
    for (i = k; --i >= 0; )     /* store items in descending order */
      if (p & (1 << i)) *d++ = i; }
  else {                        /* if positive direction requested, */
    for (i = 0; i < k; i++)     /* store items in ascending order */
      if (p & (1 << i)) *d++ = i; }
  return p & ~TA_END;           /* return bit rep. of packed items */
}  /* ta_unpack() */

/*--------------------------------------------------------------------*/

int ta_equal (const TRACT *t1, const TRACT *t2)
{                               /* --- compare transactions */
  const ITEM *a, *b;            /* to traverse the items */

  assert(t1 && t2);             /* check the function arguments */
  if (t1->size != t2->size)     /* if the sizes differ, */
    return -1;                  /* transactions cannot be equal */
  for (a = t1->items, b = t2->items; *a > TA_END; a++, b++)
    if (*a != *b) return -1;    /* abort if corresp. items differ */
  return 0;                     /* return that trans. are equal */
}  /* ta_equal() */

/*--------------------------------------------------------------------*/

int ta_cmp (const void *p1, const void *p2, void *data)
{                               /* --- compare transactions */
  const ITEM *a, *b;            /* to traverse the items */

  assert(p1 && p2);             /* check the function arguments */
  a = ((const TRACT*)p1)->items;
  b = ((const TRACT*)p2)->items;
  for ( ; 1; a++, b++) {        /* traverse the item arrays */
    if (*a < *b) return -1;     /* compare corresponding items */
    if (*a > *b) return +1;     /* and if one is greater, abort */
    if (*a <= TA_END) return 0; /* otherwise check for the sentinel */
  }                             /* and abort if it is reached */
}  /* ta_cmp() */

/* Note that this comparison function also works correctly if there */
/* are packed items, since packed item entries are always > TA_END. */

/*--------------------------------------------------------------------*/

int ta_cmpep (const void *p1, const void *p2, void *data)
{                               /* --- compare trans. (equal packed) */
  ITEM       i, k;              /* item buffers */
  const ITEM *a, *b;            /* to traverse the items */

  assert(p1 && p2);             /* check the function arguments */
  a = ((const TRACT*)p1)->items;
  b = ((const TRACT*)p2)->items;
  for ( ; 1; a++, b++) {        /* traverse the item arrays */
    i = (*a >= 0) ? *a : 0;     /* get the next items, but */
    k = (*b >= 0) ? *b : 0;     /* use 0 if items are packed */
    if (i < k) return -1;       /* compare corresponding items */
    if (i > k) return +1;       /* and if one is greater, abort */
    if (*a <= TA_END) return 0; /* otherwise check for the sentinel */
  }                             /* and abort if it is reached */
}  /* ta_cmpep() */

/*--------------------------------------------------------------------*/

int ta_cmpoff (const void *p1, const void *p2, void *data)
{                               /* --- compare transactions */
  ITEM i, k;                    /* item buffers */

  assert(p1 && p2);             /* check the function arguments */
  i = ((const TRACT*)p1)->items[*(ITEM*)data];
  k = ((const TRACT*)p2)->items[*(ITEM*)data];
  if (i < k) return -1;         /* compare items at given offset */
  if (i > k) return +1;         /* and if one is greater, abort */
  return 0;                     /* otherwise return 'equal' */
}  /* ta_cmpoff() */

/*--------------------------------------------------------------------*/

int ta_cmplim (const void *p1, const void *p2, void *data)
{                               /* --- compare transactions limited */
  const ITEM *a, *b;            /* to traverse the items */

  assert(p1 && p2);             /* check the function arguments */
  a = ((const TRACT*)p1)->items;
  b = ((const TRACT*)p2)->items;
  for ( ; 1; a++, b++) {        /* traverse the item arrays */
    if (*a < *b) return -1;     /* compare corresponding items */
    if (*a > *b) return +1;     /* and if one is greater, abort */
    if (*a == *(ITEM*)data) return 0;
  }                             /* abort if the limit is reached */
}  /* ta_cmplim() */

/*--------------------------------------------------------------------*/

int ta_cmpsfx (const void *p1, const void *p2, void *data)
{                               /* --- compare transaction suffixes */
  const ITEM *a, *b;            /* to traverse the items */

  assert(p1 && p2);             /* check the function arguments */
  a = ((const TRACT*)p1)->items +*(ITEM*)data;
  b = ((const TRACT*)p2)->items +*(ITEM*)data;
  for ( ; 1; a++, b++) {        /* traverse the item arrays */
    if (*a < *b) return -1;     /* compare corresponding items */
    if (*a > *b) return +1;     /* and if one is greater, abort */
    if (*a <= TA_END) return 0; /* otherwise check for the sentinel */
  }                             /* and abort if it is reached */
}  /* ta_cmpsfx() */

/*--------------------------------------------------------------------*/

int ta_cmpsep (const void *p1, const void *p2, void *data)
{                               /* --- compare transaction suffixes */
  ITEM       i, k;              /* item buffers */
  const ITEM *a, *b;            /* to traverse the items */

  assert(p1 && p2);             /* check the function arguments */
  a = ((const TRACT*)p1)->items +*(ITEM*)data;
  b = ((const TRACT*)p2)->items +*(ITEM*)data;
  for ( ; 1; a++, b++) {        /* traverse the item arrays */
    i = ((*a >= 0) || (*a <= TA_END)) ? *a : 0; /* use 0 for */
    k = ((*b >= 0) || (*b <= TA_END)) ? *b : 0; /* packed items */
    /* Note that the item identifier 0 cannot occur if items have */
    /* been packed, because this identifier must then be packed.  */
    if (i < k) return -1;       /* compare corresponding items */
    if (i > k) return +1;       /* and if one is greater, abort */
    if (*a <= TA_END) return 0; /* otherwise check for the sentinel */
  }                             /* and abort if it is reached */
}  /* ta_cmpsep() */

/*--------------------------------------------------------------------*/

int ta_cmpx (const TRACT *t, const ITEM *items, ITEM n)
{                               /* --- compare transactions */
  const ITEM *i, *end;          /* to traverse the items */

  assert(t && items);           /* check the function arguments */
  end = items +((n < t->size) ? n : t->size);
  for (i = t->items; items < end; i++, items++) {
    if (*i < *items) return -1; /* compare corresponding items */
    if (*i > *items) return +1; /* and abort the comparison */
  }                             /* if one of them is greater */
  return (t->size < n) ? -1 : (t->size > n) ? +1 : 0;
}  /* ta_cmpx() */              /* return sign of size difference */

/*--------------------------------------------------------------------*/

int ta_cmpsz (const void *p1, const void *p2, void *data)
{                               /* --- compare transactions */
  ITEM a, b;                    /* transaction sizes */

  assert(p1 && p2);             /* check the function arguments */
  a = ((const TRACT*)p1)->size; /* return the sign */
  b = ((const TRACT*)p2)->size; /* of the size difference */
  return (a > b) ? 1 : (a < b) ? -1 : ta_cmp(p1, p2, data);
}  /* ta_cmpsz() */

/*--------------------------------------------------------------------*/

ITEM ta_subset (const TRACT *t1, const TRACT *t2, ITEM off)
{                               /* --- test for subset/subsequence */
  const ITEM *s, *d, *x, *y;    /* to traverse the items */

  assert(t1 && t2 && (off >= 0));  /* check the function arguments */
  if ((off > t2->size) || (t1->size > t2->size -off))
    return -1;                  /* check for (enough) items in dest. */
  s = t1->items;                /* check for empty source sequence, */
  if (*s <= TA_END) return 0;   /* then traverse the destination */
  for (d = t2->items +off; *d > TA_END; d++) {
    if (*d != *s) continue;     /* try to find source start in dest. */
    for (x = s+1, y = d+1; 1; y++) {  /* compare the remaining items */
      if (*x <= TA_END) return (ITEM)(d -t2->items);
      if (*y <= TA_END) break;  /* all in source matched, subset, */
      if (*x == *y) x++;        /* end of destination, no subset */
    }                           /* skip matched item in source and */
  }                             /* always skip item in destination */
  return -1;                    /* return 'not subsequence w/o gaps' */
}  /* ta_subset() */

/*--------------------------------------------------------------------*/

ITEM ta_subwog (const TRACT *t1, const TRACT *t2, ITEM off)
{                               /* --- test for subsequence w/o gaps */
  const ITEM *s, *d, *x, *y;    /* to traverse the segments */

  assert(t1 && t2 && (off >= 0));  /* check the function arguments */
  if ((off > t2->size) || (t1->size > t2->size -off))
    return -1;                  /* check for (enough) items in dest. */
  s = t1->items;                /* check for empty source sequence, */
  if (*s <= TA_END) return 0;   /* then traverse the destination */
  for (d = t2->items +off; *d > TA_END; d++) {
    if (*d != *s) continue;     /* try to find source start in dest. */
    x = s; y = d;               /* compare the remaining items */
    do { if (*++x <= TA_END) return (ITEM)(d -t2->items); }
    while (*x == *++y);         /* if all items have been found, */
  }                             /* abort with success, otherwise */
  return -1;                    /* return 'not subsequence w/o gaps' */
}  /* ta_subwog() */

/*--------------------------------------------------------------------*/
#ifdef TA_WRITE

int ta_write (const TRACT *t, const ITEMBASE *base,
              TABWRITE *twr,  const char *wgtfmt)
{                               /* --- write a transaction */
  ITEM       i, p;              /* loop variable, packed items */
  const ITEM *s;                /* to traverse the items */

  assert(base && twr);          /* check the function arguments */
  if (!t) t = (const TRACT*)base->tract;
  for (s = t->items; *s > TA_END; s++) {
    if (*s >= 0) {              /* if unpacked item */
      if (s > t->items)         /* if this is not the first item, */
        twr_fldsep(twr);        /* print a field separator */
      twr_puts(twr, ib_name(base, *s)); }
    else {                      /* if packed items */
      p = *s & ~TA_END;         /* get the packed item bits */
      for (i = 0; (1 << i) <= p; i++) {
        if (!(p & (1 << i))) continue;
        if (s > t->items)       /* if this is not the first item, */
          twr_fldsep(twr);      /* print a field separator */
        twr_puts(twr, ib_name(base, i));
      }                         /* traverse the bits of the items */
    }                           /* and print the items */
  }                             /* for which the bits are set */
  if (wgtfmt)                   /* print the transaction weight */
    twr_printf(twr, wgtfmt, t->wgt);
  twr_recsep(twr);              /* print a record separator and */
  return (twr_error(twr)) ? E_FWRITE : 0;
}  /* ta_write() */             /* return a write error indicator */

#endif
/*--------------------------------------------------------------------*/
#ifndef NDEBUG

void ta_show (TRACT *t, ITEMBASE *base)
{                               /* --- show a transaction */
  ITEM *i;                      /* to traverse the items */

  for (i = t->items; *i > TA_END; i++) {
    if (*i < 0) { printf("%08x ", (int)*i); continue; }
    if (base) printf("%s/", ib_xname(base, *i));
    printf("%"ITEM_FMT" ",*i); /* traverse the items and */
  }                            /* print item name and identifier */
  printf("[%"SUPP_FMT"]\n", t->wgt);
}  /* ta_show() */             /* finally print the trans. weight */

#endif
/*----------------------------------------------------------------------
  Weighted Item Functions
----------------------------------------------------------------------*/

int wi_cmp (const WITEM *a, const WITEM *b)
{                               /* --- compare two transactions */
  ITEM i;                       /* loop variable */

  assert(a && b);               /* check the function arguments */
  for (i = 0; 1; i++) {         /* compare the items */
    if (a[i].item > b[i].item) return +1;
    if (a[i].item < b[i].item) return -1;
    if (a[i].item < 0) break;   /* check for the sentinel and */
  }                             /* abort if it is reached */
  for (i = 0; 1; i++) {         /* compare the item weights */
    if (a[i].wgt  > b[i].wgt) return +1;
    if (a[i].wgt  < b[i].wgt) return -1;
    if (a[i].item < 0) return 0;/* check for the sentinel and */
  }                             /* abort if it is reached */
}  /* wi_cmp() */

/*--------------------------------------------------------------------*/

static void wi_rec (WITEM *wia, ITEM n)
{                               /* --- recursive part of item sorting */
  WITEM *l, *r, t;              /* exchange positions and buffer */
  ITEM  x, m;                   /* pivot element, number of elements */

  do {                          /* sections sort loop */
    l = wia; r = l +n -1;       /* start at left and right boundary */
    if (l->item > r->item) {    /* bring the first and last element */
      t = *l; *l = *r; *r = t;} /* into the proper order */
    x = wia[n >> 1].item;       /* get the middle element as pivot */
    if      (x < l->item) x = l->item;  /* try to find a */
    else if (x > r->item) x = r->item;  /* better pivot element */
    while (1) {                 /* split and exchange loop */
      while ((++l)->item < x);  /* skip smaller elements on the left */
      while ((--r)->item > x);  /* skip greater elements on the right */
      if (l >= r) {             /* if less than two elements left, */
        if (l <= r) { l++; r--; } break; }       /* abort the loop */
      t = *l; *l = *r; *r = t;  /* otherwise exchange elements */
    }
    m = (ITEM)(wia +n -l);      /* compute the number of elements */
    n = (ITEM)(r -wia +1);      /* right and left of the split */
    if (n > m) {                /* if right section is smaller, */
      if (m >= TH_INSERT)       /* but larger than the threshold, */
        wi_rec(l, m); }         /* sort it by a recursive call, */
    else {                      /* if the left section is smaller, */
      if (n >= TH_INSERT)       /* but larger than the threshold, */
        wi_rec(wia, n);         /* sort it by a recursive call, */
      wia = l; n = m;           /* then switch to the right section */
    }                           /* keeping its size m in variable n */
  } while (n >= TH_INSERT);     /* while greater than threshold */
}  /* wi_rec() */

/*--------------------------------------------------------------------*/

void wi_sort (WITEM *wia, ITEM n, int dir)
{                               /* --- sort an transaction item array */
  ITEM  i, k;                   /* loop variable, first section */
  WITEM *l, *r;                 /* to traverse the array */
  WITEM t;                      /* exchange buffer */

  assert(wia && (n >= 0));      /* check the function arguments */
  if (n <= 1) return;           /* do not sort less than two elements */
  if (n < TH_INSERT)            /* if fewer elements than threshold */
    k = n;                      /* for insertion sort, note the */
  else {                        /* number of elements, otherwise */
    wi_rec(wia, n);             /* call the recursive function */
    k = TH_INSERT -1;           /* and get the number of elements */
  }                             /* in the first array section */
  for (l = r = wia; --k > 0; )  /* find the smallest element */
    if ((++r)->item < l->item)  /* among the first k elements */
      l = r;                    /* to use as a front sentinel */
  r = wia;                      /* swap the smallest element */
  t = *l; *l = *r; *r = t;      /* to the front as a sentinel */
  for (i = n; --i > 0; ) {      /* insertion sort loop */
    t = *++r;                   /* note the element to insert */
    for (l = r; (--l)->item > t.item; )  /* shift right elements */
      l[1] = *l;                /* that are greater than the one */
    l[1] = t;                   /* to insert and store this element */
  }                             /* in the place thus found */
  if (dir < 0)                  /* if descending order requested, */
    wi_reverse(wia, n);         /* reverse the element order */
}  /* wi_sort() */

/*--------------------------------------------------------------------*/

void wi_reverse (WITEM *wia, ITEM n)
{                               /* --- reverse an item array */
  WITEM t, *end = wia +n;       /* exchange buffer, end pointer */

  assert(wia && (n >= 0));      /* check the function arguments */
  while (--end > wia) {         /* reverse the order of the items */
    t = *end; *end = *wia; *wia++ = t; }
}  /* wi_reverse() */

/*--------------------------------------------------------------------*/

ITEM wi_unique (WITEM *wia, ITEM n)
{                               /* --- make item array unique */
  WITEM *s, *d;                 /* to traverse the item array */

  assert(wia && (n >= 0));      /* check the function arguments */
  if (n <= 1) return n;         /* check for 0 or 1 element */
  for (d = s = wia; --n > 0; ) {
    if      ((++s)->item != d->item) *++d = *s;
    else if (   s->wgt > d->wgt) d->wgt = s->wgt;
  }                             /* collect the unique elements */
  *++d = WTA_END;               /* store a sentinel at the end */
  return (ITEM)(d -wia);        /* return the new number of elements */
}  /* wi_unique() */

/*----------------------------------------------------------------------
  Transaction Functions (weighted items)
----------------------------------------------------------------------*/

WTRACT* wta_create (ITEM size, SUPP wgt)
{                               /* --- create a transaction */
  WTRACT *t;                    /* created transaction */

  assert(size >= 0);            /* check the function arguments */
  t = (WTRACT*)malloc(sizeof(WTRACT) +(size_t)size *sizeof(WITEM));
  if (!t) return NULL;          /* allocate a new transaction */
  t->wgt  = wgt;                /* store the transaction weight */
  t->size = 0; t->mark = 0;     /* and init. the transaction size */
  t->items[size] = WTA_END;     /* store a sentinel after the items */
  return t;                     /* return the created transaction */
}  /* wta_create() */

/*--------------------------------------------------------------------*/

WTRACT* wta_clone (const WTRACT *t)
{                               /* --- clone a transaction */
  WTRACT *c;                    /* created transaction clone */

  assert(t);                    /* check the function argument */
  c = (WTRACT*)malloc(sizeof(WTRACT) +(size_t)t->size *sizeof(WITEM));
  if (!c) return NULL;          /* allocate a new transaction and */
  c->wgt  = t->wgt;             /* copy old transaction into it */
  c->size = t->size; c->mark = t->mark;
  memcpy(c->items, t->items, (size_t)t->size *sizeof(WITEM));
  c->items[t->size] = WTA_END;  /* store a sentinel at the end */
  return c;                     /* return the created copy */
}  /* wta_clone() */

/*--------------------------------------------------------------------*/

WTRACT* wta_copy (WTRACT *dst, const WTRACT *src)
{                               /* --- copy a transaction */
  assert(dst                    /* check the function arguments */
  &&     src && (dst->size == src->size));
  dst->wgt = src->wgt;          /* copy weight and items */
  memcpy(dst->items, src->items, (size_t)(src->size+1) *sizeof(WITEM));
  return dst;                   /* return the destination */
}  /* ta_copy() */

/*--------------------------------------------------------------------*/

void wta_add (WTRACT *t, ITEM item, float wgt)
{                               /* --- add an item to a transaction */
  WITEM *d;                     /* destination for the item */

  assert(t && (item >= 0));     /* check the function arguments */
  d = t->items +t->size++;      /* get the next extended item and */
  d->item = item; d->wgt = wgt; /* store item ientifier and weight */
}  /* wta_add() */

/*--------------------------------------------------------------------*/

void wta_sort (WTRACT *t, int dir)
{ wi_sort(t->items, t->size, dir); }

/*--------------------------------------------------------------------*/

void wta_reverse (WTRACT *t)
{ wi_reverse(t->items, t->size); }

/*--------------------------------------------------------------------*/

ITEM wta_unique (WTRACT *t)
{ return t->size = wi_unique(t->items, t->size); }

/*--------------------------------------------------------------------*/

int wta_cmp (const void *p1, const void *p2, void *data)
{                               /* --- compare transactions */
  return wi_cmp(((const WTRACT*)p1)->items,
                ((const WTRACT*)p2)->items);
}  /* wta_cmp() */

/*--------------------------------------------------------------------*/

int wta_cmpsz (const void *p1, const void *p2, void *data)
{                               /* --- compare transactions */
  ITEM a, b;                    /* transaction sizes */

  assert(p1 && p2);             /* check the function arguments */
  a = ((const WTRACT*)p1)->size;/* return the sign */
  b = ((const WTRACT*)p2)->size;/* of the size difference */
  return (a > b) ? 1 : (a < b) ? -1 : wta_cmp(p1, p2, data);
}  /* wta_cmpsz() */

/*--------------------------------------------------------------------*/

ITEM wta_subset (const WTRACT *t1, const WTRACT *t2, ITEM off)
{                               /* --- test for subset/subsequence */
  const WITEM *s, *d, *x, *y;   /* to traverse the items */

  assert(t1 && t2 && (off >= 0));  /* check the function arguments */
  if ((off > t2->size) || (t1->size > t2->size -off))
    return -1;                  /* check for (enough) items in dest. */
  s = t1->items;                /* check for empty source sequence, */
  if (s->item < 0) return 0;    /* then traverse the destination */
  for (d = t2->items +off; d->item >= 0; d++) {
    if (d->item != s->item)     /* try to find source start in dest., */
      continue;                 /* then compare the remaining items */
    for (x = s+1, y = d+1; 1; y++) {
      if (x->item < 0) return (ITEM)(d -t2->items);
      if (y->item < 0) break;      /* all in source matched, subset, */
      if (x->item == y->item) x++; /* end of destination, no subset */
    }                           /* skip matched item in source */
  }                             /* always skip item in destination */
  return -1;                    /* return 'not subsequence w/o gaps' */
}  /* wta_subset() */

/*--------------------------------------------------------------------*/

ITEM wta_subwog (const WTRACT *t1, const WTRACT *t2, ITEM off)
{                               /* --- test for subsequence w/o gaps */
  const WITEM *s, *d, *x, *y;   /* to traverse the segments */

  assert(t1 && t2 && (off >= 0));  /* check the function arguments */
  if ((off > t2->size) || (t1->size > t2->size -off))
    return -1;                  /* check for (enough) items in dest. */
  s = t1->items;                /* check for empty source sequence, */
  if (s->item < 0) return 0;    /* then traverse the destination */
  for (d = t2->items +off; d->item >= 0; d++) {
    if (d->item != s->item)     /* try to find the source start */
      continue;                 /* in the destination sequence */
    x = s; y = d;               /* compare the remaining items */
    do { if ((++x)->item < 0) return (ITEM)(d -t2->items); }
    while (x->item == (++y)->item); /* if all items have been found, */
  }                             /* abort with success, otherwise */
  return -1;                    /* return 'not subsequence w/o gaps' */
}  /* wta_subwog() */

/*--------------------------------------------------------------------*/
#ifdef TA_WRITE

int wta_write (const WTRACT *t, const ITEMBASE *base,
               TABWRITE *twr, const char *wgtfmt, const char *iwfmt)
{                               /* --- write a weighted transaction */
  const WITEM *s;               /* to traverse the items */

  assert(base && twr && iwfmt); /* check the function arguments */
  if (!t) t  = (WTRACT*)base->tract;
  for (s = t->items; s->item >= 0; s++) {
    if (s > t->items)           /* if this is not the first item, */
      twr_fldsep(twr);          /* print a field separator */
    twr_puts(twr, ib_name(base, s->item));
    twr_printf(twr, iwfmt, s->wgt);
  }                             /* print item and item weight */
  if (wgtfmt)                   /* print the transaction weight */
    twr_printf(twr, wgtfmt, t->wgt);
  twr_recsep(twr);              /* print a record separator */
  return (twr_error(twr)) ? E_FWRITE : 0;
}  /* wta_write() */            /* return a write error indicator */

#endif
/*--------------------------------------------------------------------*/
#ifndef NDEBUG

void wta_show (WTRACT *t, ITEMBASE *base)
{                               /* --- show an extended transaction */
  ITEM k;                       /* loop variable */

  for (k = 0; k < t->size; k++){/* traverse the items */
    if (k > 0)                  /* if this is not the first item, */
      fputc(' ', stdout);       /* print an item separator */
    if (base) printf("%s/", ib_xname(base, t->items[k].item));
    printf("%"ITEM_FMT":%f", t->items[k].item, t->items[k].wgt);
  }                             /* print item name, id, and weight */
  printf(" [%"SUPP_FMT"]\n", t->wgt);
}  /* wta_show() */             /* finally print the trans. weight */

#endif
/*----------------------------------------------------------------------
  Transaction Bag/Multiset Functions
----------------------------------------------------------------------*/

TABAG* tbg_create (ITEMBASE *base)
{                               /* --- create a transaction bag */
  TABAG *bag;                   /* created transaction bag */

  assert(base);                 /* check the function argument */
  bag = (TABAG*)malloc(sizeof(TABAG));
  if (!bag) return NULL;        /* create a transaction bag/multiset */
  if (!base && !(base = ib_create(0, 0))) {
    free(bag); return NULL; }   /* create an item base if needed */
  bag->base   = base;           /* store the underlying item base */
  bag->mode   = base->mode;     /* and initialize the other fields */
  bag->extent = 0; bag->wgt = 0; bag->max = 0;
  bag->cnt    = bag->size = 0;
  bag->tracts = NULL;           /* there are no transactions yet */
  bag->icnts  = NULL;
  bag->ifrqs  = NULL;
  bag->buf    = NULL;
  return bag;                   /* return the created t.a. bag */
}  /* tbg_create() */

/*--------------------------------------------------------------------*/

void tbg_delete (TABAG *bag, int delib)
{                               /* --- delete a transaction bag */
  assert(bag);                  /* check the function argument */
  if (bag->buf) free(bag->buf); /* delete buffer for surrogates */
  if (bag->tracts) {            /* if there are transactions */
    while (bag->cnt > 0)        /* traverse the transaction array */
      free(bag->tracts[--bag->cnt]);
    free(bag->tracts);          /* delete all transactions */
  }                             /* and the transaction array */
  if (bag->icnts) free(bag->icnts);
  if (delib) ib_delete(bag->base);
  free(bag);                    /* delete the item base and */
}  /* tbg_delete() */           /* the transaction bag body */

/*--------------------------------------------------------------------*/

static TABAG* clone (TABAG *bag)
{                               /* --- clone memory structure */
  TID    i;                     /* loop variable */
  ITEM   n;                     /* number of items */
  TABAG  *dst;                  /* created clone of the trans. bag */
  TRACT  *t;                    /* to traverse the transactions */
  WTRACT *x;                    /* to traverse the transactions */

  assert(bag);                  /* check the function argument */
  dst = tbg_create(bag->base);  /* create an empty transaction bag */
  dst->tracts = (void**)malloc((size_t)bag->cnt *sizeof(TRACT*));
  if (!dst->tracts) { return NULL; }
  dst->max    = bag->max;       /* create a transaction array */
  dst->wgt    = bag->wgt;       /* copy maximum transaction size, */
  dst->extent = bag->extent;    /* total transaction weight and */
  dst->size   = bag->cnt;       /* transaction array size and extent */
  if (bag->mode & IB_WEIGHTS) { /* if trans. with weighted items */
    for (i = 0; i < bag->cnt; i++) { /* traverse the transactions */
      n = ((WTRACT*)bag->tracts[i])->size;
      x = (WTRACT*)malloc(sizeof(TRACT) +(size_t)(n+1) *sizeof(ITEM));
      if (!x) { tbg_delete(dst, 0); return NULL; }
      x->wgt  = 1;              /* create and init. the transaction */
      x->size = n; x->mark = 0; x->items[n] = WTA_END;
      dst->tracts[dst->cnt++] = x;
    } }                         /* store the created transaction */
  else {                        /* if simple transactions */
    for (i = 0; i < bag->cnt; i++) { /* traverse the transactions */
      n = ((TRACT*)bag->tracts[i])->size;
      t = (TRACT*)malloc(sizeof(TRACT) +(size_t)(n+1) *sizeof(ITEM));
      if (!t) { tbg_delete(dst, 0); return NULL; }
      t->wgt  = 1;              /* create and init. the transaction */
      t->size = n; t->mark = 0; t->items[n] = TA_END;
      dst->tracts[dst->cnt++] = t;
    }                           /* note the transaction size and */
  }                             /* store the created transaction */
  return dst;                   /* return the created clone */
}  /* clone() */

/*--------------------------------------------------------------------*/

TABAG* tbg_clone (TABAG *bag)
{                               /* --- clone a transaction bag */
  TABAG *dst = clone(bag);      /* clone the memory structure */
  if (dst) tbg_copy(dst, bag);  /* copy the transactions into it */
  return dst;                   /* return the created clone */
}  /* tbg_clone() */

/*--------------------------------------------------------------------*/

TABAG* tbg_copy (TABAG *dst, TABAG *src)
{                               /* --- copy a transaction bag */
  TID i;                        /* loop variable */

  assert(dst && src             /* check the function arguments */
  &&    (dst->size >= src->cnt));
  if (src->mode & IB_WEIGHTS) { /* if trans. with weighted items */
    for (i = 0; i < src->cnt; i++)  /* copy the transactions */
      wta_copy(dst->tracts[i], src->tracts[i]); }
  else {                        /* if simple transactions */
    for (i = 0; i < src->cnt; i++)  /* copy the transactions */
      ta_copy(dst->tracts[i], src->tracts[i]);
  }
  dst->mode = src->mode;        /* copy the transaction mode */
  return dst;                   /* return the destination bag */
}  /* tbg_copy() */

/*--------------------------------------------------------------------*/

static int tbg_count (TABAG *bag)
{                               /* --- count item occurrences */
  ITEM   i;                     /* item buffer, number of items */
  TID    n;                     /* loop variable for transactions */
  TRACT  *t;                    /* to traverse the transactions */
  WTRACT *x;                    /* to traverse the transactions */
  ITEM   *s;                    /* to traverse the transaction items */
  WITEM  *p;                    /* to traverse the transaction items */
  TID    *z;                    /* to reallocate counter arrays */

  i = ib_cnt(bag->base);        /* get the number of items */
  z = (TID*)realloc(bag->icnts, (size_t)i *(sizeof(TID)+sizeof(SUPP)));
  if (!z) return -1;            /* allocate the counter arrays */
  bag->icnts = (TID*) memset(z,            0, (size_t)i *sizeof(TID));
  bag->ifrqs = (SUPP*)memset(bag->icnts+i, 0, (size_t)i *sizeof(SUPP));
  if (bag->mode & IB_WEIGHTS) { /* if the items carry weights */
    for (n = 0; n < bag->cnt; n++) {
      x = (WTRACT*)bag->tracts[n]; /* traverse the transactions */
      for (p = x->items; p->item >= 0; p++) {
        bag->icnts[p->item] += 1;  /* traverse the transaction items */
        bag->ifrqs[p->item] += x->wgt;
      }                         /* count the occurrences and */
    } }                         /* sum the transaction weights */
  else {                        /* if the items do not carry weights */
    for (n = 0; n < bag->cnt; n++) {
      t = (TRACT*)bag->tracts[n];  /* traverse the transactions */
      for (s = t->items; *s > TA_END; s++) {
        if ((i = *s) < 0) i = 0;   /* traverse the transaction items */
        bag->icnts[i] += 1;     /* count packed items in 1st element */
        bag->ifrqs[i] += t->wgt;
      }                         /* count the occurrences and */
    }                           /* sum the transaction weights */
  }
  return 0;                     /* return 'ok' */
}  /* tbg_count() */

/*--------------------------------------------------------------------*/

const TID* tbg_icnts (TABAG *bag, int recnt)
{                               /* --- number of trans. per item */
  if ((recnt || !bag->icnts) && (tbg_count(bag) < 0)) return NULL;
  return bag->icnts;            /* count the item occurrences */
}  /* tbg_icnts() */

/*--------------------------------------------------------------------*/

const SUPP* tbg_ifrqs (TABAG *bag, int recnt)
{                               /* --- item frequencies (weight) */
  if ((recnt || !bag->ifrqs) && (tbg_count(bag) < 0)) return NULL;
  return bag->ifrqs;            /* determine the item frequencies */
}  /* tbg_ifrqs() */

/*--------------------------------------------------------------------*/

int tbg_add (TABAG *bag, TRACT *t)
{                               /* --- add a standard transaction */
  void **p;                     /* new transaction array */
  TID  n;                       /* new transaction array size */

  assert(bag                    /* check the function arguments */
  &&   !(bag->mode & IB_WEIGHTS));
  n = bag->size;                /* get the transaction array size */
  if (bag->cnt >= n) {          /* if the transaction array is full */
    n += (n > BLKSIZE) ? (n >> 1) : BLKSIZE;
    p  = (void**)realloc(bag->tracts, (size_t)n *sizeof(TRACT*));
    if (!p) return E_NOMEM;     /* enlarge the transaction array */
    bag->tracts = p; bag->size = n;
  }                             /* set the new array and its size */
  if (!t && !(t = ta_clone(ib_tract(bag->base))))
    return E_NOMEM;             /* get trans. from item base if nec. */
  if (bag->icnts) {             /* delete the item-specific counters */
    free(bag->icnts); bag->icnts = NULL; bag->ifrqs = NULL; }
  bag->tracts[bag->cnt++] = t;  /* store the transaction and */
  bag->wgt += t->wgt;           /* sum the transaction weight */
  if (t->size > bag->max)       /* update maximal transaction size */
    bag->max = t->size;         /* and count the item instances */
  bag->extent += (size_t)t->size;
  return 0;                     /* return 'ok' */
}  /* tbg_add() */

/*--------------------------------------------------------------------*/

int tbg_addw (TABAG *bag, WTRACT *t)
{                               /* --- add an extended transaction */
  void **p;                     /* new transaction array */
  TID  n;                       /* new transaction array size */

  assert(bag                    /* check the function arguments */
  &&    (bag->mode & IB_WEIGHTS));
  n = bag->size;                /* get the transaction array size */
  if (bag->cnt >= n) {          /* if the transaction array is full */
    n += (n > BLKSIZE) ? (n >> 1) : BLKSIZE;
    p  = (void**)realloc(bag->tracts, (size_t)n *sizeof(WTRACT*));
    if (!p) return E_NOMEM;     /* enlarge the transaction array */
    bag->tracts = p; bag->size = n;
  }                             /* set the new array and its size */
  if (!t && !(t = wta_clone(ib_wtract(bag->base))))
    return E_NOMEM;             /* get trans. from item base if nec. */
  if (bag->icnts) {             /* delete the item-specific counters */
    free(bag->icnts); bag->icnts = NULL; bag->ifrqs = NULL; }
  bag->tracts[bag->cnt++] = t;  /* store the transaction and */
  bag->wgt += t->wgt;           /* sum the transaction weight */
  if (t->size > bag->max)       /* update maximal transaction size */
    bag->max = t->size;         /* and count the item instances */
  bag->extent += (size_t)t->size;
  return 0;                     /* return 'ok' */
}  /* tbg_addw() */

/*--------------------------------------------------------------------*/

int tbg_addib (TABAG *bag)
{                               /* --- add transaction from item base */
  assert(bag);                  /* check the function argument */
  return (bag->mode & IB_WEIGHTS) ? tbg_addw(bag, NULL)
                                  : tbg_add (bag, NULL);
}  /* tbg_addib() */

/*--------------------------------------------------------------------*/
#ifdef TA_READ                  /* if transaction reading capability */

int tbg_read (TABAG *bag, TABREAD *tread, int mode)
{                               /* --- read transactions from a file */
  int r;                        /* result of ib_read()/tbg_add() */

  assert(bag && tread);         /* check the function arguments */
  if (bag->icnts) {             /* delete the item-specific counters */
    free(bag->icnts); bag->icnts = NULL; bag->ifrqs = NULL; }
  while (1) {                   /* transaction read loop */
    r = ib_read(bag->base, tread, mode);
    if (r < 0) return r;        /* read the next transaction and */
    if (r > 0) return 0;        /* check for error and end of file */
    r = (bag->mode & IB_WEIGHTS) ? tbg_addw(bag, NULL)
                                 : tbg_add (bag, NULL);
    if (r) return bag->base->err = E_NOMEM;
  }                             /* add transaction to bag/multiset */
}  /* tbg_read() */

#endif
/*--------------------------------------------------------------------*/
#ifdef TA_WRITE

int tbg_write (TABAG *bag, TABWRITE *twr, const char *wgtfmt, ...)
{                               /* --- write transactions to a file */
  TID        i;                 /* loop variable for transactions */
  va_list    args;              /* list of variable arguments */
  const char *iwfmt;            /* item weight format */

  assert(bag && twr);           /* check the function arguments */
  if (bag->mode & IB_WEIGHTS) { /* if the items carry weights */
    va_start(args, wgtfmt);     /* start variable arguments */
    iwfmt = va_arg(args, const char*);
    for (i = 0; i < bag->cnt; i++)
      wta_write((WTRACT*)bag->tracts[i], bag->base, twr, wgtfmt, iwfmt);
    va_end(args); }             /* traverse and print transactions */
  else {                        /* if the items do not carry weights */
    for (i = 0; i < bag->cnt; i++)
      ta_write ( (TRACT*)bag->tracts[i], bag->base, twr, wgtfmt);
  }                             /* traverse and print transactions */
  return (twr_error(twr)) ? E_FWRITE : 0;
}  /* tbg_write() */            /* return a write error indicator */

#endif
/*--------------------------------------------------------------------*/

int tbg_istab (TABAG *bag)
{                               /* --- check for table-derived data */
  int      r = -1;              /* result of check for table */
  ITEM     i, n, z;             /* loop variable for items */
  TID      k;                   /* loop variable for transactions */
  TRACT    *t;                  /* to traverse the transactions */
  ITEMDATA *itd;                /* to traverse the item data */
  IDMAP    *idmap;              /* item identifier map */

  assert(bag                    /* check the function arguments */
  &&   ((bag->mode & TA_PACKED) == 0));
  if (bag->cnt <= 1) return 0;  /* check for at most one transaction */
  idmap = bag->base->idmap;     /* get the item identifier map */
  n     = idm_cnt(idmap);       /* get the number of items and */
  for (i = n; --i >= 0; )       /* clear the occurrence columns */
    ((ITEMDATA*)idm_byid(idmap, i))->idx = -1;
  z = ((TRACT*)bag->tracts[0])->size;   /* get transaction size */
  for (k = bag->cnt; --k >= 0; ) {
    t = bag->tracts[k];         /* traverse the transactions */
    if (t->size != z) {         /* check the size of the transactions */
      r = 0; break; }           /* (must all have the same size) */
    for (i = t->size; --i >= 0; ) { /* traverse the items */
      itd = (ITEMDATA*)idm_byid(idmap, t->items[i]);
      if      (itd->idx <  0)      itd->idx = (TID)i;
      else if (itd->idx != (TID)i) { r = 0; break; }
    }                           /* check whether all items always */
  }                             /* occur in the same column */
  bag->base->idx = 1;           /* reset the global marker/index */
  for (i = n; --i >= 0; )       /* and the item markers/indices */
    ((ITEMDATA*)idm_byid(idmap, i))->idx = 0;
  return r;                     /* return the test result */
}  /* tbg_istab() */

/*--------------------------------------------------------------------*/

static void recode (TABAG *bag, ITEM *map)
{                               /* --- recode items in transactions */
  ITEM   i;                     /* item buffer */
  TID    n;                     /* loop variable for transactions */
  TRACT  *t;                    /* to traverse the transactions */
  WTRACT *x;                    /* to traverse the transactions */
  ITEM   *s, *d;                /* to traverse the items */
  WITEM  *a, *b;                /* to traverse the items */

  assert(bag && map);           /* check the function arguments */
  if (bag->icnts) {             /* delete the item-specific counters */
    free(bag->icnts); bag->icnts = NULL; bag->ifrqs = NULL; }
  bag->extent = 0; bag->max = 0;/* clear maximal transaction size */
  if (bag->mode & IB_WEIGHTS) { /* if the items carry weights */
    for (n = 0; n < bag->cnt; n++) {
      x = (WTRACT*)bag->tracts[n];  /* traverse the transactions */
      for (a = b = x->items; a->item >= 0; a++) {
        i = map[a->item];       /* traverse and recode the items */
        if (i >= 0) (b++)->item = i;    /* remove all items that are */
      }                             /* not mapped (mapped to id < 0) */
      x->size = (ITEM)(b-x->items); /* compute new number of items */
      x->items[x->size] = WTA_END;  /* store a sentinel at the end */
      if (x->size > bag->max)   /* update the maximal trans. size */
        bag->max = x->size;     /* (may differ from the old size) */
      bag->extent += (size_t)x->size;
    } }                         /* sum the item instances */
  else {                        /* if the items do not carry weights */
    for (n = 0; n < bag->cnt; n++) {
      t = (TRACT*)bag->tracts[n];   /* traverse the transactions */
      for (s = d = t->items; *s > TA_END; s++) {
        i = map[*s];            /* traverse and recode the items */
        if (i >= 0) *d++ = i;   /* remove all items that are */
      }                         /* not mapped (mapped to id < 0) */
      t->size = (ITEM)(d-t->items); /* compute new number of items */
      t->items[t->size] = TA_END;   /* store a sentinel at the end */
      if (t->size > bag->max)   /* update the maximal trans. size */
        bag->max = t->size;     /* (may differ from the old size) */
      bag->extent += (size_t)t->size;
    }                           /* sum the item instances */
  }
}  /* recode() */

/*--------------------------------------------------------------------*/

ITEM tbg_recode (TABAG *bag, SUPP min, SUPP max, ITEM cnt, int dir)
{                               /* --- recode items in transactions */
  ITEM *map;                    /* identifier map for recoding */

  assert(bag);                  /* check the function arguments */
  map = (ITEM*)malloc((size_t)ib_cnt(bag->base) *sizeof(ITEM));
  if (!map) return -1;          /* create an item identifier map */
  cnt = ib_recode(bag->base, min, max, cnt, dir, map);
  recode(bag, map);             /* recode items and transactions */
  free(map);                    /* delete the item identifier map */
  return cnt;                   /* return the new number of items */
}  /* tbg_recode() */

/*--------------------------------------------------------------------*/

void tbg_filter (TABAG *bag, ITEM min, const int *marks, double wgt)
{                               /* --- filter (items in) transactions */
  TID    n;                     /* loop variable for transactions */
  TRACT  *t;                    /* to traverse the transactions */
  WTRACT *x;                    /* to traverse the transactions */
  ITEM   *s, *d;                /* to traverse the items */
  WITEM  *a, *b;                /* to traverse the items */

  assert(bag);                  /* check the function arguments */
  if (!marks && (min <= 1)) return;
  if (bag->icnts) {             /* delete the item-specific counters */
    free(bag->icnts); bag->icnts = NULL; bag->ifrqs = NULL; }
  bag->extent = 0;              /* clear the item instance counter */
  bag->max    = 0;              /* and the maximal transaction size */
  if (bag->mode & IB_WEIGHTS) { /* if the items carry weights */
    for (n = 0; n < bag->cnt; n++) {
      x = (WTRACT*)bag->tracts[n]; /* traverse the transactions */
      if (marks) {              /* if item markers are given */
        for (a = b = x->items; a->item >= 0; a++)
          if (marks[a->item] && (a->wgt >= wgt))
            *b++ = *a;          /* remove unmarked items */
        x->size = (ITEM)(b -x->items);
      }                         /* store the new number of items */
      if (x->size < min)        /* if the transaction is too short, */
        x->size = 0;            /* delete all items (clear size) */
      x->items[x->size] = WTA_END; /* store a sentinel at the end */
      if (x->size > bag->max)   /* update the maximal trans. size */
        bag->max = x->size;     /* (may differ from the old size) */
      bag->extent += (size_t)x->size;
    } }                         /* sum the item instances */
  else {                        /* if the items do not carry weights */
    for (n = 0; n < bag->cnt; n++) {
      t = (TRACT*)bag->tracts[n];  /* traverse the transactions */
      if (marks) {              /* if item markers are given */
        for (s = d = t->items; *s > TA_END; s++)
          if (marks[*s]) *d++ = *s; /* remove unmarked items */
        t->size = (ITEM)(d -t->items);
      }                         /* store the new number of items */
      if (t->size < min)        /* if the transaction is too short, */
        t->size = 0;            /* delete all items (clear size) */
      t->items[t->size] = TA_END;  /* store a sentinel at the end */
      if (t->size > bag->max)   /* update the maximal trans. size */
        bag->max = t->size;     /* (may differ from the old size) */
      bag->extent += (size_t)t->size;
    }                           /* sum the item instances */
  }
}  /* tbg_filter() */

/*--------------------------------------------------------------------*/

void tbg_trim (TABAG *bag, ITEM min, const int *marks, double wgt)
{                               /* --- trim transactions (sequences) */
  ITEM   k;                     /* loop variable for items */
  TID    n;                     /* loop variable for transactions */
  TRACT  *t;                    /* to traverse the transactions */
  WTRACT *x;                    /* to traverse the transactions */
  ITEM   *s, *d;                /* to traverse the items */
  WITEM  *a, *b;                /* to traverse the items */

  assert(bag);                  /* check the function arguments */
  bag->extent = 0;              /* clear the item instance counter */
  bag->max    = 0;              /* and the maximal transaction size */
  if (bag->icnts) {             /* delete the item-specific counters */
    free(bag->icnts); bag->icnts = NULL; bag->ifrqs = NULL; }
  if (bag->mode & IB_WEIGHTS) { /* if the items carry weights */
    for (n = 0; n < bag->cnt; n++) {
      x = (WTRACT*)bag->tracts[n]; /* traverse the transactions */
      if (marks) {              /* if item markers are given */
        for (a = x->items, k = x->size; --k >= 0; )
          if (marks[a[k].item] && (a[k].wgt >= wgt))
            break;              /* trim items at the end */
        x->size     = ++k;      /* set new sequence length */
        x->items[k] = WTA_END;  /* and a (new) sentinel */
        for (a = b = x->items; a->item >= 0; a++)
          if (marks[a->item] && (a->wgt >= wgt))
            break;              /* trim items at the front */
        if (a > b) {            /* if front items were trimmed */
          while (a->item >= 0) *b++ = *a++;
          x->size = (ITEM)(b -x->items);
        }                       /* move remaining items to the front */
      }                         /* and store the new number of items */
      if (x->size < min)        /* if the transaction is too short, */
        x->size = 0;            /* delete all items (clear size) */
      x->items[x->size] = WTA_END; /* store a sentinel at the end */
      if (x->size > bag->max)   /* update the maximal trans. size */
        bag->max = x->size;     /* (may differ from the old size) */
      bag->extent += (size_t)x->size;
    } }                         /* sum the item instances */
  else {                        /* if the items do not carry weights */
    for (n = 0; n < bag->cnt; n++) {
      t = (TRACT*)bag->tracts[n];  /* traverse the transactions */
      if (marks) {              /* if item markers are given */
        for (s = t->items, k = t->size; --k >= 0; )
          if (marks[s[k]]) break;
        t->size     = ++k;      /* trim infrequent items at the end */
        t->items[k] = TA_END;   /* and set a (new) sentinel */
        for (s = d = t->items; *s >= 0; s++)
          if (marks[*s]) break; /* trim infrequent items at the front */
        if (s > d) {            /* if front items were trimmed */
          while (*s >= 0) *d++ = *s++;
          t->size = (ITEM)(d -t->items);
        }                       /* move remaining items to the front */
      }                         /* and store the new number of items */
      if (t->size < min)        /* if the transaction is too short, */
        t->size = 0;            /* delete all items (clear size) */
      t->items[t->size] = TA_END;  /* store a sentinel at the end */
      if (t->size > bag->max)   /* update the maximal trans. size */
        bag->max = t->size;     /* (may differ from the old size) */
      bag->extent += (size_t)t->size;
    }                           /* sum the item instances */
  }
}  /* tbg_trim() */

/*--------------------------------------------------------------------*/

void tbg_itsort (TABAG *bag, int dir, int heap)
{                               /* --- sort items in transactions */
  ITEM   k;                     /* number of items */
  TID    n;                     /* loop variable */
  TRACT  *t;                    /* to traverse the transactions */
  WTRACT *x;                    /* to traverse the transactions */
  void   (*sortfn)(ITEM*, size_t, int);  /* sort function */

  assert(bag);                  /* check the function arguments */
  if (bag->mode & IB_WEIGHTS) { /* if the items carry weights */
    for (n = 0; n < bag->cnt; n++) {
      x = (WTRACT*)bag->tracts[n]; /* traverse the transactions */
      wi_sort(x->items, x->size, dir);
    } }                         /* sort the items in each transaction */
  else {                        /* if the items do not carry weights */
    sortfn = (heap) ? ia_heapsort : ia_qsort;
    for (n = 0; n < bag->cnt; n++) {
      t = (TRACT*)bag->tracts[n];  /* traverse the transactions */
      k = t->size;              /* get transaction and its size */
      if (k < 2) continue;      /* do not sort less than two items */
      while ((k > 0) && (t->items[k-1] <= TA_END))
        --k;                    /* skip additional end markers */
      sortfn(t->items, (size_t)k, dir);
    }                           /* sort the items in the transaction */
  }                             /* if the given direction is negative */
}  /* tbg_itsort() */

/*--------------------------------------------------------------------*/

void tbg_mirror (TABAG *bag)
{                               /* --- mirror all transactions */
  TID n;                        /* loop variable for transactions */

  assert(bag);                  /* check the function argument */
  if (bag->mode & IB_WEIGHTS)   /* if the items carry weights */
    for (n = 0; n < bag->cnt; n++)
      wta_reverse((WTRACT*)bag->tracts[n]);
  else                          /* if the items do not carry weights */
    for (n = 0; n < bag->cnt; n++)
      ta_reverse ((TRACT*) bag->tracts[n]);
}  /* tbg_mirror() */

/*--------------------------------------------------------------------*/

static void pksort (TRACT **tracts, TRACT **buf, TID n, ITEM o)
{                               /* --- sort packed items with binsort */
  ITEM  i, k, x;                /* loop variables, bit pattern */
  ITEM  mask;                   /* overall item bit mask */
  TRACT **dst, **src, **t;      /* to traverse the transactions */
  TID   cnts[64];               /* counters for bit pattern occs. */

  assert(tracts && buf);        /* check the function arguments */
  if (n <= 1) return;           /* sort only 2 and more transactions */
  if (n <= 32) {                /* sort few transactions plainly */
    ptr_mrgsort(tracts, (size_t)n, +1, ta_cmpoff, &o, buf); return; }
  memset(cnts, 0, sizeof(cnts));/* clear the pattern counters */
  for (mask = 0, t = tracts+n; --t >= tracts; ) {
    mask |= x = (*t)->items[o]; /* traverse the transactions, */
    cnts[x & 0x3f]++;           /* combine all bit patterns and */
  }                             /* count patterns with 6 bits */
  src = tracts; dst = buf;      /* get trans. source and destination */
  if (cnts[mask & 0x3f] < n) {  /* if more than one bit pattern */
    for (i = 0; ++i < 64; )     /* traverse the patterns and compute */
      cnts[i] += cnts[i-1];     /* offsets for storing transactions */
    for (t = src+n; --t >= src;)/* sort transactions with bin sort */
      dst[--cnts[(*t)->items[o] & 0x3f]] = *t;
    t = src; src = dst; dst = t;/* exchange source and destination, */
  }                             /* because trans. have been moved */
  for (k = 6; k < 31; k += 5) { /* traverse the remaining sections */
    x = (mask >> k) & 0x1f;     /* if all transactions are zero */
    if (!x) continue;           /* in a section, skip the section */
    memset(cnts, 0, 32*sizeof(TID)); /* clear pattern counters */
    for (t = src+n; --t >= src;)/* count the pattern occurrences */
      cnts[((*t)->items[o] >> k) & 0x1f]++;
    if (cnts[x] >= n) continue; /* check for only one pattern */
    for (i = 0; ++i < 32; )     /* traverse the patterns and compute */
      cnts[i] += cnts[i-1];     /* offsets for storing transactions */
    for (t = src+n; --t >= src;)/* sort transactions with bin sort */
      dst[--cnts[((*t)->items[o] >> k) & 0x1f]] = *t;
    t = src; src = dst; dst = t;/* exchange source and destination, */
  }                             /* because trans. have been moved */
  if (src != tracts)            /* ensure that result is in tracts */
    memcpy(tracts, src, (size_t)n *sizeof(TRACT*));
}  /* pksort() */

/*--------------------------------------------------------------------*/

static void sort (TRACT **tracts, TID n, ITEM o,
                  TRACT **buf, TID *cnts, ITEM k, ITEM mask)
{                               /* --- sort trans. with bucket sort */
  TID   m;                      /* loop variable for transactions */
  ITEM  i, x, y;                /* loop variable, item buffers */
  TRACT **t;                    /* to traverse the transactions */

  assert(tracts && buf && cnts);/* check the function arguments */
  if (n <= 16) {                /* if only few transactions, */
    ptr_mrgsort(tracts, (size_t)n, +1,
                (mask > ITEM_MIN) ? ta_cmpsfx : ta_cmpsep, &o, buf);
    return;                     /* sort the transactions plainly, */
  }                             /* then abort the function */
  memset(cnts-1, 0, (size_t)(k+1) *sizeof(TID));
  x = 0;                        /* clear the transaction counters */
  for (t = tracts+n; --t >= tracts; ) {
    x = (*t)->items[o];         /* traverse the transactions */
    if (x < 0) x = (x <= TA_END) ? -1 : 0;
    cnts[x]++;                  /* count the transactions per item */
  }                             /* (0 for packed items, -1 for none) */
  if (cnts[x] >= n) {           /* check for only one or no item */
    if (x < 0) return;          /* if all transactions end, abort */
    x = (*tracts)->items[o];    /* check for packed items */
    if ((x < 0) && (mask <= ITEM_MIN)) pksort(tracts, buf, n, o);
    sort(tracts, n, o+1, buf, cnts, k, mask);
    if ((x < 0) && (mask >  ITEM_MIN)) pksort(tracts, buf, n, o);
    return;                     /* sort the whole array recursively */
  }                             /* and then abort the function */
  memcpy(buf, tracts, (size_t)n *sizeof(TRACT*));
  for (i = 0; i < k; i++)       /* traverse the items and compute */
    cnts[i] += cnts[i-1];       /* offsets for storing transactions */
  for (t = buf+n; --t >= buf; ) {
    x = (*t)->items[o];         /* traverse the transactions again */
    if (x < 0) x = (x <= TA_END) ? -1 : 0;
    tracts[--cnts[x]] = *t;     /* sort w.r.t. the item at offset o */
  }                             /* (0 for packed items, -1 for none) */
  tracts += m = cnts[0];        /* remove trans. that are too short */
  if ((n -= m) <= 0) return;    /* and if no others are left, abort */
  if ((*tracts)->items[o] < 0){ /* if there are packed items, sort */
    pksort(tracts, buf, m = cnts[1] -m, o);
    if (mask <= ITEM_MIN) {     /* if to treat packed items equally */
      sort(tracts, m, o+1, buf, cnts, k, mask);
      tracts += m;              /* sort suffixes of packed trans. */
      if ((n -= m) <= 0) return;/* and if no other transactions */
    }                           /* are left, abort the function */
  }                             /* traverse the formed sections */
  if ((x = (*tracts)->items[o]) < 0) x &= mask;
  for (t = tracts; --n > 0; ) { /* traverse the transactions */
    if ((y = (*++t)->items[o]) < 0) y &= mask;
    if (y == x) continue;       /* if same start item, continue */
    x = y;                      /* note the new start item */
    if ((m = (TID)(t-tracts)) > 1)
      sort(tracts, m, o+1, buf, cnts, k, mask);
    tracts = t;                 /* sort the section recursively */
  }                             /* and skip the transactions in it */
  if ((m = (TID)((t+1)-tracts)) > 1)
    sort(tracts, m, o+1, buf, cnts, k, mask);
}  /* sort() */                 /* finally sort the last section */

/*--------------------------------------------------------------------*/

void tbg_sort (TABAG *bag, int dir, int mode)
{                               /* --- sort a transaction bag */
  ITEM  k;                      /* number of items */
  TID   n;                      /* number of transactions */
  ITEM  mask;                   /* mask for packed item treatment */
  TRACT **buf;                  /* trans. buffer for bucket sort */
  TID   *cnts;                  /* counter array for bin sort */
  CMPFN *cmp;                   /* comparison function */

  assert(bag);                  /* check the function arguments */
  if (bag->cnt < 2) return;     /* check for at least two trans. */
  n = bag->cnt;                 /* get the number of transactions */
  k = ib_cnt(bag->base);        /* and the number of items */
  if (k < 2) k = 2;             /* need at least 2 counters */
  if (bag->mode & IB_WEIGHTS) { /* if the items carry weights, */
    if (mode & TA_HEAP)         /* sort with weighted item functions */
         ptr_heapsort(bag->tracts, (size_t)n, dir, wta_cmp, NULL);
    else ptr_qsort   (bag->tracts, (size_t)n, dir, wta_cmp, NULL); }
  else if ((buf = (TRACT**)malloc((size_t) n   *sizeof(TRACT*)
                                 +(size_t)(k+1)*sizeof(TID)))) {
    if ((size_t)k < (size_t)n){ /* if bin sort is possible/favorable, */
      cnts = (TID*)(buf+n)+1;   /* use bin sort to sort transactions */
      mask = (mode & TA_EQPACK) ? ITEM_MIN : -1;
      sort((TRACT**)bag->tracts, n, 0, buf, cnts, k, mask);
      if (dir < 0)              /* if necessary, reverse the order */
        ptr_reverse(bag->tracts, (size_t)n); }
    else {                      /* if more items than transactions */
      cmp = (mode & TA_EQPACK) ? ta_cmpep : ta_cmp;
      ptr_mrgsort(bag->tracts, (size_t)n, dir, cmp, NULL, buf);
    }                           /* sort transactions with merge sort */
    free(buf); }                /* delete the allocated buffer */
  else {                        /* if to use failsafe functions */
    cmp = (mode & TA_EQPACK) ? ta_cmpep : ta_cmp;
    if (mode & TA_HEAP)         /* sort the transactions */
         ptr_heapsort(bag->tracts, (size_t)n, dir, cmp, NULL);
    else ptr_qsort   (bag->tracts, (size_t)n, dir, cmp, NULL);
  }                             /* use heapsort or quicksort */
}  /* tbg_sort() */

/*--------------------------------------------------------------------*/

void tbg_sortsz (TABAG *bag, int dir, int mode)
{                               /* --- sort a transaction bag */
  TID n;                        /* number of transactions */

  assert(bag);                  /* check the function arguments */
  n = bag->cnt;                 /* get the number of transactions */
  if (bag->mode & IB_WEIGHTS) { /* if the items carry weights, */
    if (mode & TA_HEAP)         /* sort with weighted item functions */
         ptr_heapsort(bag->tracts, (size_t)n, dir, wta_cmpsz, NULL);
    else ptr_qsort   (bag->tracts, (size_t)n, dir, wta_cmpsz, NULL);}
  else {                        /* if the items do not carry weights */
    if (mode & TA_HEAP)         /* sort the transactions */
         ptr_heapsort(bag->tracts, (size_t)n, dir,  ta_cmpsz, NULL);
    else ptr_qsort   (bag->tracts, (size_t)n, dir,  ta_cmpsz, NULL);
  }                             /* use heapsort or quicksort */
}  /* tbg_sortsz() */

/*--------------------------------------------------------------------*/

TID tbg_reduce (TABAG *bag, int keep0)
{                               /* --- reduce a transaction bag */
  /* This function presupposes that the transaction bag has been */
  /* sorted with one of the above sorting functions beforehand.  */
  TID   i;                      /* loop variable */
  int   c;                      /* comparison result */
  TRACT **s, **d;               /* to traverse the transactions */

  assert(bag);                  /* check the function argument */
  if (bag->cnt <= 1) return 1;  /* deal only with two or more trans. */
  if (bag->icnts) {             /* delete the item-specific counters */
    free(bag->icnts); bag->icnts = NULL; bag->ifrqs = NULL; }
  bag->extent = 0;              /* reinit. number of item occurrences */
  s = d = (TRACT**)bag->tracts; /* traverse the sorted transactions */
  for (i = bag->cnt; --i > 0; ) {
    if (((*++s)->size != (*d)->size))
      c = ((*s)->size < (*d)->size) ? -1 : +1;
    else                        /* compare items for same size */
      c = (bag->mode & IB_WEIGHTS) ? wta_cmp(*s, *d, NULL)
                                   :  ta_cmp(*s, *d, NULL);
    if (c == 0) {               /* if the transactions are equal */
      (*d)->wgt += (*s)->wgt;   /* combine the transactions */
      free(*s); }               /* by summing their weights */
    else {                      /* if transactions are not equal */
      if (keep0 || ((*d)->wgt != 0))
        bag->extent += (size_t)(*d++)->size;
      else free(*d);            /* check weight of old transaction */
      *d = *s;                  /* copy the new transaction */
    }                           /* to close a possible gap */
  }                             /* (collect unique transactions) */
  if (keep0 || ((*d)->wgt != 0))
    bag->extent += (size_t)(*d++)->size;
  else free(*d);                /* check weight of last transaction */
  return bag->cnt = (TID)(d -(TRACT**)bag->tracts);
}  /* tbg_reduce() */           /* return new number of transactions */

/*--------------------------------------------------------------------*/

void tbg_setmark (TABAG *bag, int mark)
{                               /* --- set marks of all transactions */
  TID i;                        /* loop variable */
  assert(bag);                  /* check the function arguments */
  for (i = 0; i < bag->cnt; i++)/* set marks of all transactions */
    ((TRACT**)bag->tracts)[i]->mark = mark;
}  /* tbg_setmark() */

/*--------------------------------------------------------------------*/

void tbg_bitmark (TABAG *bag)
{                               /* --- set item bits of all trans. */
  TID i;                        /* loop variable */
  assert(bag);                  /* check the function arguments */
  for (i = 0; i < bag->cnt; i++)/* set all marks to bit encoded items */
    ta_bitmark(((TRACT**)bag->tracts)[i]);
}  /* tbg_bitmark() */

/*--------------------------------------------------------------------*/

void tbg_pack (TABAG *bag, int n)
{                               /* --- pack all transactions */
  TID i;                        /* loop variable */

  assert(bag                    /* check the function arguments */
  &&   !(bag->mode & IB_WEIGHTS));
  if (n <= 0) return;           /* if no items to pack, abort */
  for (i = 0; i < bag->cnt; i++)/* pack items in all transactions */
    ta_pack((TRACT*)bag->tracts[i], n);
  bag->mode |= n & TA_PACKED;   /* set flag for packed transactions */
}  /* tbg_pack() */

/*--------------------------------------------------------------------*/

void tbg_unpack (TABAG *bag, int dir)
{                               /* --- unpack all transactions */
  TID i;                        /* loop variable */

  assert(bag                    /* check the function arguments */
  &&   !(bag->mode & IB_WEIGHTS));
  for (i = 0; i < bag->cnt; i++)/* pack items in all transactions */
    ta_unpack((TRACT*)bag->tracts[i], dir);
  bag->mode &= ~TA_PACKED;      /* clear flag for packed transactions */
}  /* tbg_unpack() */

/*--------------------------------------------------------------------*/

SUPP tbg_occur (TABAG *bag, const ITEM *items, ITEM n)
{                               /* --- count transaction occurrences */
  TID  l, r, m, k;              /* index and loop variables */
  SUPP s;                       /* number of occurrences (support) */

  assert(bag && items           /* check the function arguments */
  &&   !(bag->mode & IB_WEIGHTS));
  k = bag->cnt;                 /* get the number of transactions */
  for (r = m = 0; r < k; ) {    /* find right boundary */
    m = (r+k) >> 1;             /* by a binary search */
    if (ta_cmpx((TRACT*)bag->tracts[m], items, n) > 0) k = m;
    else                                               r = m+1;
  }
  for (l = m = 0; l < k; ) {    /* find left boundary */
    m = (l+k) >> 1;             /* by a binary search */
    if (ta_cmpx((TRACT*)bag->tracts[m], items, n) < 0) l = m+1;
    else                                               k = m;
  }
  for (s = 0; l < r; l++)       /* traverse the found section */
    s += tbg_tract(bag,l)->wgt; /* sum the transaction weights */
  return s;                     /* return the number of occurrences */
}  /* tbg_occur() */

/*--------------------------------------------------------------------*/

int tbg_ipwgt (TABAG *bag, int mode)
{                               /* --- compute idempotent weights */
  /* Requires the transactions to be sorted ascendingly by size and */
  /* reduced to unique occurrences (tbg_sortsz() and tbg_reduce()). */
  ITEM   i, k;                  /* loop variables, buffers */
  TID    n, m;                  /* loop variable for transactions */
  size_t x;                     /* extent of the transaction bag */
  ITEM   r;                     /* result of subset test */
  double w, sum;                /* (sum of) transaction weights */
  void   ***occs, **o;          /* item occurrence arrays */
  TRACT  *s, *d;                /* to traverse the transactions */
  const ITEM *p;                /* to traverse the items */
  const TID  *c;                /* item occurrence counters */
  SUBFN  *sub;                  /* subset/subsequence function */
  WTRACT *a, *b;                /* to traverse the transactions */
  WITEM  *z;                    /* to traverse the items */
  SUBWFN *subw;                 /* subset/subsequence function */
  double *wgts;                 /* buffer for item weight adaptation */

  assert(bag);                  /* check the function argument */
  wgts = NULL;                  /* if to adapt item weights */
  if ((bag->mode & mode & IB_WEIGHTS)
  &&  (mode & TA_NOGAPS) && (mode & TA_ALLOCC)
  && !(wgts = (double*)malloc((size_t)bag->max *sizeof(double))))
    return -1;                  /* allocate a weight array */
  c = tbg_icnts(bag, 0);        /* get the occurrences per item */
  k = tbg_itemcnt(bag);         /* get the number of items and */
  x = tbg_extent(bag);          /* the total item occurrences */
  occs = (!c) ? NULL            /* allocate the transaction arrays */
       : (void***)malloc(   (size_t)k *sizeof(TRACT**)
                        +(x+(size_t)k)*sizeof(TRACT*));
  if (occs) {                   /* if allocation succeeded */
    o = (void**)(occs +k);      /* organize the memory and */
    for (i = 0; i < k; i++) {   /* store a sentinel at the end */
      occs[i] = o += c[i]; *o++ = NULL; }
    if (bag->mode & IB_WEIGHTS){/* if the items carry weights */
      for (n = 0; n < bag->cnt; n++) {
        a = tbg_wtract(bag, n); /* traverse the transactions */
        for (z = a->items; z->item >= 0; z++) {
          if ((WTRACT*)*occs[z->item] != a)
            *--occs[z->item] = a;
        }                       /* collect containing transactions */
      }                         /* per item, but avoid duplicates */
      subw = (mode & TA_NOGAPS) ? wta_subwog : wta_subset;
      for (n = bag->cnt; --n >= 0; ) {
        a = tbg_wtract(bag, n); /* traverse the transactions */
        if ((k = a->items[0].item) < 0) {
          a->wgt -= bag->wgt -a->wgt;
          continue;             /* if the transaction is empty, */
        }                       /* it can be handled directly */
        if (wgts) memset(wgts, 0, (size_t)a->size *sizeof(double));
        sum = 0;                /* init. the item weight array */
        for (o = occs[k]; (b = (WTRACT*)*o) != a; o++) {
          if (b->size <= a->size)
            continue;           /* traverse larger transactions */
          for (r = -1; (r = subw(a, b, r+1)) >= 0; ) {
            a->wgt -= b->wgt;   /* update the transaction weight */
            if (!wgts) {        /* skip item weights if not needed */
              if (!(mode & TA_ALLOCC)) break; else continue; }
            sum += w = (b->wgt != 0) ? (double)b->wgt : 1.0;
            for (z = b->items +r, i = 0; i < a->size; i++)
              wgts[i] += w *(double)z[i].wgt;
          }                     /* sum the item weight contributions */
        }                       /* from supersets or supersequences */
        if (!wgts) continue;    /* skip item weights if not needed */
        sum += w = (a->wgt != 0) ? (double)a->wgt : 1.0;
        for (z = a->items, i = 0; i < a->size; i++)
          z[i].wgt = (float)((sum *(double)z[i].wgt -wgts[i]) /w);
      } }                       /* rescale the item weights */
    else {                      /* if the items do not carry weights */
      for (n = 0; n < bag->cnt; n++) {
        s = tbg_tract(bag, n);  /* traverse the transactions */
        for (p = s->items; *p > TA_END; p++)
          if ((TRACT*)*occs[*p] != s)
            *--occs[*p] = s;    /* collect containing transactions */
      }                         /* per item, but avoid duplicates */
      sub = (mode & TA_NOGAPS) ? ta_subwog : ta_subset;
      for (n = bag->cnt; --n >= 0; ) {
        s = tbg_tract(bag, n);  /* traverse the transactions */
        if ((k = s->items[0]) <= TA_END) {
          s->wgt -= bag->wgt -s->wgt;
          continue;             /* if the transaction is empty, */
        }                       /* it can be handled directly */
        for (o = occs[k]; (d = (TRACT*)*o) != s; o++) {
          if (d->size <= s->size) continue;
          for (r = -1; (r = sub(s, d, r+1)) >= 0; ) {
            s->wgt -= d->wgt;   /* traverse transaction pairs and */
            if (!(mode & TA_ALLOCC)) break;
          }                     /* subtract transaction weights */
        }                       /* of supersets or supersequences */
      }                         /* stop after first occurrence */
    }                           /* unless all occurrences requested */
    free(occs); }               /* deallocate working memory */
  else {                        /* failsafe, but slower variant */
    if (bag->mode & IB_WEIGHTS){/* if the items carry weights */
      subw = (mode & TA_NOGAPS) ? wta_subwog : wta_subset;
      for (n = bag->cnt; --n >= 0; ) {
        a = tbg_wtract(bag, n); /* traverse the transactions */
        if (a->items[0].item < 0) {
          a->wgt -= bag->wgt -a->wgt;
          continue;             /* if the transaction is empty, */
        }                       /* it can be handled directly */
        if (wgts) memset(wgts, 0, (size_t)a->size *sizeof(double));
        sum = 0;                /* init. the item weight array */
        for (m = bag->cnt; --m > n; ) {
          b = tbg_wtract(bag,m);/* traverse larger transactions */
          if (b->size <= a->size) continue;
          for (r = -1; (r = subw(a, b, r+1)) >= 0; ) {
            a->wgt -= b->wgt;   /* update the transaction weight */
            if (!wgts) {        /* skip item weights if not needed */
              if (!(mode & TA_ALLOCC)) break; else continue; }
            sum += w = (b->wgt != 0) ? (double)b->wgt : 1.0;
            for (z = b->items +r, i = 0; i < a->size; i++)
              wgts[i] += w *(double)z[i].wgt;
          }                     /* sum the item weight contributions */
        }                       /* from supersets or supersequences */
        if (!wgts) continue;    /* skip item weights if not needed */
        sum += w = (a->wgt != 0) ? (double)a->wgt : 1.0;
        for (z = a->items, i = 0; i < a->size; i++)
          z[i].wgt = (float)((sum *(double)z[i].wgt -wgts[i]) /w);
      } }                       /* rescale the item weights */
    else {                      /* if the items do not carry weights */
      sub = (mode & TA_NOGAPS) ? ta_subwog : ta_subset;
      for (n = bag->cnt-1; n > 0; ) {
        s = tbg_tract(bag,--n); /* traverse the transactions */
        if (s->items[0] <= TA_END) {
          s->wgt -= bag->wgt -s->wgt;
          continue;             /* if the transaction is empty, */
        }                       /* it can be handled directly */
        for (m = bag->cnt; --m > n; ) {
          d = tbg_tract(bag,m); /* traverse larger transactions */
          if (d->size <= s->size) continue;
          for (r = 1; (r = sub(s, d, r+1)) >= 0; ) {
            s->wgt -= d->wgt;   /* traverse transaction pairs and */
            if (!(mode & TA_ALLOCC)) break;
          }                     /* if a transactions is a subset or */
        }                       /* a subsequence of another trans., */
      }                         /* subtract the transaction weight */
    }                           /* stop after first occurrence */
  }                             /* unless all occurrences requested */
  if (wgts) free(wgts);         /* deallocate working memory */
  return 0;                     /* return 'ok' */
}  /* tbg_ipwgt() */

/*--------------------------------------------------------------------*/
#ifndef NDEBUG

void tbg_show (TABAG *bag)
{                               /* --- show a transaction bag */
  TID i;                        /* loop variable */

  assert(bag);                  /* check the function argument */
  for (i = 0; i < bag->cnt; i++) {
    printf("%5"TID_FMT": ", i); /* traverse the transactions */
    if (bag->mode & IB_WEIGHTS) /* and print a transaction id */
         wta_show((WTRACT*)bag->tracts[i], bag->base);
    else  ta_show( (TRACT*)bag->tracts[i], bag->base);
  }                             /* print the transactions */
  printf("%"TID_FMT"/%"SUPP_FMT" transaction(s)\n",
         bag->cnt, bag->wgt);   /* finally print transaction counter */
}  /* tbg_show() */             /* and total transaction weight */

#endif
/*----------------------------------------------------------------------
  Surrogate Generation Functions
----------------------------------------------------------------------*/
#ifdef TA_SURR

static void iswap (TRACT *t1, TRACT *t2,
                   ITEM *flags, ITEM *offs, RNG *rng)
{                               /* --- swap items between trans. */
  ITEM  i, j, k;                /* item index in transaction */
  ITEM  *s;                     /* to traverse the transactions */
  TRACT *t;                     /* exchange buffer for transactions */

  assert(t1 && t2 && flags && offs);  /* check function arguments */
  if (t1->size < t2->size) {    /* if first transaction is smaller, */
    t = t1; t1 = t2; t2 = t; }  /* exchange the transactions */
  for (s = t2->items; *s > TA_END; s++)
    flags[*s] = 1;              /* mark items in second transaction */
  for (k = 0, s = t1->items; *s > TA_END; s++) {
    if (flags[*s] == 0) offs[k++] = (ITEM)(s -t1->items);
    flags[*s] = 0;              /* collect offsets in first trans. */
  }                             /* and clear the item flags */
  assert(t1->size == (ITEM)(s -t1->items));
  if (k <= 0) return;           /* check for a swapable item */
  /* After the initial swap, t1 has at least as many items as t2. */
  /* Therefore k <= 0 can only happen if the two transactions are */
  /* identical (that is, contain exactly the same set of items).  */
  /* However, in this case all flags have already been cleared.   */
  i = (ITEM)(rng_dbl(rng) *(double)k);
  i = offs[(i < 0) ? 0 : i % k];
  assert(i < t1->size);         /* choose a random offset/item */
  for (k = 0, s = t2->items; *s > TA_END; s++) {
    if (flags[*s] != 0) offs[k++] = (ITEM)(s -t2->items);
    flags[*s] = 0;              /* collect offsets in second trans. */
  }                             /* and clear the item flags */
  assert(t2->size == (ITEM)(s -t2->items));
  if (k <= 0) return;           /* check for a swapable item */
  j = (ITEM)(rng_dbl(rng) *(double)k);
  j = offs[(j < 0) ? 0 : j % k];
  assert(j < t2->size);         /* choose a random offset/item */
  k            = t1->items[i];  /* swap the items */
  t1->items[i] = t2->items[j];  /* at the randomly chosen offsets */
  t2->items[j] = k;             /* between the two transactions */
}  /* iswap() */

/*--------------------------------------------------------------------*/

TABAG* tbg_ident (TABAG *src, RNG *rng, TABAG *dst)
{                               /* --- identity: original data */
  assert(src && rng             /* check the function arguments */
  &&   !(src->mode & (TA_PACKED|IB_WEIGHTS)));
  if (!dst) return tbg_clone(src);
  return dst;                   /* return the created surrogate */
}  /* tbg_indent() */

/*--------------------------------------------------------------------*/

TABAG* tbg_random (TABAG *src, RNG *rng, TABAG *dst)
{                               /* --- random item selection */
  size_t  z, x;                 /* item frequency sum, index limit */
  TID     k;                    /* loop variable for transactions */
  ITEM    i, j, n, m;           /* loop variables for items */
  TRACT   *t;                   /* to traverse transactions */
  ITEM    *s, *d;               /* to traverse transaction items */
  ITEMFRQ *ifrq, f;             /* item frequencies, swap buffer */

  assert(src && rng             /* check the function arguments */
  &&   !(src->mode & (TA_PACKED|IB_WEIGHTS)));
  if (!dst) { if (!(dst = clone(src))) return NULL; }
  n = ib_cnt(dst->base);        /* clone the memory structure */
  if (!dst->buf) {              /* and get the number of items */
    ifrq = dst->buf = malloc((size_t)n*sizeof(ITEMFRQ)+sizeof(size_t));
    if (!ifrq) return NULL;     /* create an item frequency buffer */
    for (i = 0; i < n; i++) {   /* initialize the item frequencies */
      ifrq[i].item = i; ifrq[i].frq = 0; }
    for (k = src->cnt; --k >= 0; ) {
      for (s = ((TRACT*)src->tracts[k])->items; *s > TA_END; s++)
        ifrq[*s].frq += 1;      /* traverse the transactions and */
    }                           /* count the item occurrences */
    for (z = 0, i = 0; i < n; i++)
      z += (size_t)ifrq[i].frq; /* sum the item frequencies */
    assert(z == src->extent);   /* and check against extent */
    *(size_t*)(ifrq+n) = z;     /* note the frequency sum */
  }                             /* as a random number limit */
  ifrq = dst->buf;              /* get the item frequency buffer */
  for (k = dst->cnt; --k >= 0; ) {
    t = dst->tracts[k];         /* traverse the transactions */
    z = *(size_t*)(ifrq+n);     /* get number of item instances */
    m = n;                      /* traverse items in transaction */
    for (i = 0; i < t->size; i++) {
      x = (size_t)(rng_dbl(rng) *(double)z);
      for (j = 0; j < m; j++) { /* choose a random number */
        if  (x <  (size_t)ifrq[j].frq) break;
        else x -= (size_t)ifrq[j].frq;
      }                         /* find corresponding array index */
      if (j >= m) j = m-1;      /* ensure a proper array index */
      t->items[i] = ifrq[j].item;
      z -= (size_t)ifrq[j].frq; /* get and store the chosen item */
      f  = ifrq[j]; ifrq[j] = ifrq[--m]; ifrq[m] = f;
    }                           /* swap the chosen item entry */
  }                             /* to the end of the array */
  for (i = 0; i < n; i++)       /* init. frequencies differences */
    ifrq[i].dif = 0;            /* between surrogate and original */
  for (k = dst->cnt; --k >= 0; ) {
    for (s = ((TRACT*)dst->tracts[k])->items; *s > TA_END; s++)
      ifrq[*s].dif += 1;        /* traverse the transactions and */
  }                             /* count the item occurrences */
  for (k = src->cnt; --k >= 0; ) {
    for (s = ((TRACT*)src->tracts[k])->items; *s > TA_END; s++)
      ifrq[*s].dif -= 1;        /* traverse the transactions and */
  }                             /* count the item occurrences */
  for (z = 0, i = 0; i < n; i++)/* det. amount of over-representation */
    if (ifrq[i].dif > 0) z += (size_t)ifrq[i].dif;
  for (k = dst->cnt; (--k >= 0) && (z > 0); ) {
    t = dst->tracts[k];         /* traverse the transactions */
    d = NULL;                   /* traverse items of transactions */
    for (s = ((TRACT*)dst->tracts[k])->items; *s > TA_END; s++) {
      ifrq[*s].item |= ITEM_MIN;/* mark items of the transaction */
      if (ifrq[*s].dif > 0) d = s;
    }                           /* find an over-represented item */
    if (d) {                    /* if over-represented item found */
      i = (ITEM)(rng_dbl(rng) *(double)n) % n;
      j = (i+n-1) % n;          /* choose a random starting point */
      while ((i != j)           /* and search cyclically from there */
      &&     ((ifrq[i].item & ITEM_MIN) || (ifrq[i].dif >= 0)))
        i = (i+1) % n;          /* find an under-represented item */
      if (i != j) {             /* if under-represented item found */
        ifrq[*d].item &= ~ITEM_MIN;
        ifrq[*d    ].dif -= 1;  /* replace the over-represented item */
        ifrq[*d = i].dif += 1;  /* with the under-represented item */
        z -= 1;                 /* update frequency differences */
      }                         /* and reduce amount of over-rep. */
    }
    for (s = ((TRACT*)dst->tracts[k])->items; *s > TA_END; s++)
      ifrq[*s].item &= ~ITEM_MIN;
  }                             /* unmark items of transaction */
  return dst;                   /* return the created surrogate */
}  /* tbg_random() */

/*--------------------------------------------------------------------*/

TABAG* tbg_swap (TABAG *src, RNG *rng, TABAG *dst)
{                               /* --- pair swaps to permute items */
  int    first = 0;             /* flag for first run */
  size_t z;                     /* size of buffer, loop variable */
  ITEM   n;                     /* number of items */
  TID    i, j;                  /* random transaction indices */
  ITEM   *flags;                /* item flag array */

  assert(src && rng             /* check the function arguments */
  &&   !(src->mode & (TA_PACKED|IB_WEIGHTS)));
  if (!dst) { if (!(dst = tbg_clone(src))) return NULL; }
  if (dst->mode | TA_PACKED)    /* clone the memory structure and */
    tbg_unpack(dst, +1);        /* unpack destination if necessary */
  if (src->cnt < 2) return dst; /* check for at most one transaction */
  n = ib_cnt(dst->base);        /* get the number of items */
  if (!dst->buf) {              /* clone the memory structure */
    dst->buf = malloc((size_t)n *2 *sizeof(ITEM));
    if (!dst->buf) return NULL; /* create an item flag array */
    memset(dst->buf, 0, (size_t)n *sizeof(ITEM));
    first = -1;                 /* clear the item flags and */
  }                             /* set the first run flag */
  flags = dst->buf;             /* get the item flag array */
  for (z = (first) ? dst->extent << 1 : dst->extent >> 1; z > 0; z--) {
    i = (int)(rng_dbl(rng) *(double)dst->cnt);
    if (i >= dst->cnt) i = dst->cnt-1;
    if (i <  0)        i = 0;   /* generate first transaction index */
    j = (int)(rng_dbl(rng) *(double)dst->cnt);
    if (j >= dst->cnt) j = dst->cnt-1;
    if (j <  0)        j = 0;   /* generate second transaction index */
    if (i == j) continue;       /* transactions must be different */
    iswap(dst->tracts[i], dst->tracts[j], flags, flags+n, rng);
  }                             /* swap items between transactions */
  return dst;                   /* return the created surrogate */
}  /* tbg_swap() */

/*--------------------------------------------------------------------*/

TABAG* tbg_shuffle (TABAG *src, RNG *rng, TABAG *dst)
{                               /* --- column shuffles for table data */
  ITEM  i, n, x;                /* loop variables for items, buffer */
  TID   j, k;                   /* loop variables for transactions */
  TRACT **t;                    /* to access the transactions */

  assert(src && rng             /* check the function arguments */
  &&   !(src->mode & (TA_PACKED|IB_WEIGHTS)) && tbg_istab(src));
  if (!dst) { if (!(dst = clone(src))) return NULL; }
  if (dst != src)               /* clone the memory structure and */
    tbg_copy(dst, src);         /* copy the source transactions */
  if (src->cnt < 2) return dst; /* check for at most one transaction */
  t = (TRACT**)dst->tracts;     /* get the transaction array */
  n = ((TRACT*)t[0])->size;     /* and the transaction size */
  for (k = dst->cnt; --k > 0;){ /* traverse the transactions */
    for (i = 0; i < n; i++) {   /* traverse the items/columns */
      j = (TID)(rng_dbl(rng) *(double)(k+1));
      if      (j > k) j = k;    /* compute a random transaction index */
      else if (j < 0) j = 0;    /* and clamp it to the proper range */
      x              = t[j]->items[i];
      t[j]->items[i] = t[k]->items[i];
      t[k]->items[i] = x;       /* exchange the items of */
    }                           /* the selected transaction */
  }                             /* and the last transaction */
  return dst;                   /* return the created surrogate */
}  /* tbg_shuffle() */

#endif
/*----------------------------------------------------------------------
  Transaction Array Functions
----------------------------------------------------------------------*/

void taa_collate (TRACT **taa, TID n, ITEM k)
{                               /* --- collate transactions */
  TID   i;                      /* loop variable */
  TRACT *s, *d;                 /* to traverse the transactions */
  ITEM  *a, *b;                 /* to traverse the items */
  ITEM  x, y;                   /* item buffers */

  assert(taa);                  /* check the function arguments */
  for (d = *taa, i = 0; ++i < n; ) {
    s = taa[i];                 /* traverse the transactions */
    a = d->items; b = s->items; /* compare packed items */
    x = (ispacked(*a)) ? *a++ : 0;
    y = (ispacked(*b)) ? *b++ : 0;
    if (x != y) { d = s; continue; }
    for ( ; (UITEM)*a < (UITEM)k; a++, b++)
      if (*a != *b) break;      /* compare transaction prefixes */
    if (*a != k) d = s;         /* if not equal, keep transaction */
    else d->wgt -= s->wgt = -s->wgt;
  }                             /* otherwise combine trans. weights */
}  /* taa_collate() */

/*--------------------------------------------------------------------*/

void taa_uncoll (TRACT **taa, TID n)
{                               /* --- uncollate transactions */
  TID   i;                      /* loop variable */
  TRACT *s, *d;                 /* to traverse the transactions */

  assert(taa);                  /* check the function arguments */
  for (d = *taa, i = 0; ++i < n; ) {
    s = taa[i];                 /* traverse the transactions */
    if (s->wgt >= 0) d = s;     /* get uncollated transactions */
    else d->wgt -= s->wgt = -s->wgt;
  }                             /* uncombine the trans. weights */
}  /* taa_uncoll() */

/*--------------------------------------------------------------------*/

TID taa_tabsize (TID n)
{                               /* --- compute hash table size */
  size_t i;                     /* prime table index */
  #if 1                         /* smaller table, but more collisions */
  n = (n < 2*(TID_MAX/3)) ? n+(n >> 1) : n;
  #else                         /* larger  table, but less collisions */
  n = (n <   (TID_MAX/2)) ? n+n        : n;
  #endif                        /* find next larger size */
  i = siz_bisect((size_t)n, primes, TS_PRIMES);
  return (TID)primes[(i >= TS_PRIMES) ? TS_PRIMES-1 : i];
}  /* taa_tabsize() */

/*--------------------------------------------------------------------*/
#if 0                           /* reduction with sorting */

TID taa_reduce (TRACT **taa, TID n, ITEM end,
                const ITEM *map, void *buf, void **dst)
{                               /* --- reduce a transaction array */
  TID   i;                      /* loop variable */
  ITEM  x;                      /* item buffer */
  TRACT *t, **p;                /* to traverse the transactions */
  ITEM  *s, *d;                 /* to traverse the items */

  assert(taa                    /* check the function arguments */
  &&    (n > 0) && (end > 0) && map && buf && dst && *dst);
  t = *(TRACT**)dst; p = taa;   /* get the transaction memory */
  for (i = 0; i < n; i++) {     /* traverse the transactions */
    s = taa[i]->items; d = t->items;
    if (ispacked(*s)) {         /* if there are packed items, */
      x = *s++ & map[0];        /* remove those not present in mask */
      if (x) *d++ = x | TA_END; /* if there are packed items left, */
    }                           /* store them in the destination */
    for ( ; (UITEM)*s < (UITEM)end; s++)
      if ((x = map[*s]) >= 0)   /* map/remove the transaction items */
        *d++ = x;               /* and store them in the destination */
    t->size = (ITEM)(d-t->items);  /* compute the size of the trans. */
    if (t->size <= 0) continue; /* delete empty transactions */
    ia_qsort(t->items, (size_t)t->size, 1);  /* sort the items */
    t->wgt = taa[i]->wgt;       /* copy the transaction weight */
    *d++ = TA_END;              /* store a sentinel at the end */
    *p++ = t; t = (TRACT*)d;    /* store the filtered transaction */
  }                             /* and get the next destination */
  n = (TID)(p -taa);            /* get new number of transactions */
  if (n >= 32) {                /* if there are enough trans. left */
    if (end >= n) ptr_mrgsort(taa, (size_t)n, +1, ta_cmp, NULL, buf);
    else sort(taa, n, 0, buf, (TID*)((TRACT*)buf+n)+1, end, -1);
  }                             /* sort the reduced transactions */
  for (p = taa, i = 0; ++i < n; ) {
    t = taa[i];                 /* traverse the sorted transactions */
    if (t->size != (*p)->size){ /* compare the trans. size first */
      *++p = t; continue; }     /* and items only for same size */
    for (s = t->items, d = (*p)->items; 1; s++, d++) {
      if (*s != *d)     { *++p = t;            break; }
      if (*s <= TA_END) { (*p)->wgt += t->wgt; break; }
    }                           /* copy unequal transactions and */
  }                             /* combine equal transactions */
  return (TID)(p+1 -taa);       /* return the new number of trans. */
}  /* taa_reduce() */

/*--------------------------------------------------------------------*/
#else                           /* reduction with hash table */

TID taa_reduce (TRACT **taa, TID n, ITEM end,
                const ITEM *map, void *buf, void **dst)
{                               /* --- reduce a transaction array */
  TID    i;                     /* loop variable */
  size_t h, k, x, z;            /* hash value, bin index, table size */
  ITEM   m;                     /* item buffer (for mapped item) */
  TRACT  *t, *u, **p;           /* to traverse the transactions */
  ITEM   *s, *d;                /* to traverse the items */
  TRACT  **htab = buf;          /* hash table for reduction */

  assert(taa                    /* check the function arguments */
  &&    (n > 0) && (end > 0) && map && buf && dst && *dst);
  z = (size_t)taa_tabsize(n);   /* get the hash table size */
  t = *(TRACT**)dst;            /* and the transaction memory */
  for (i = 0; i < n; i++) {     /* traverse the transactions */
    s = taa[i]->items; d = t->items;
    if (ispacked(*s)) {         /* if there are packed items, */
      m = *s++ & map[0];        /* remove those not present in mask */
      if (m) *d++ = m | TA_END; /* if there are packed items left, */
    }                           /* store them in the destination */
    for ( ; (UITEM)*s < (UITEM)end; s++)
      if ((m = map[*s]) >= 0)   /* map/remove the transaction items */
        *d++ = m;               /* and store them in the destination */
    t->size = (ITEM)(d-t->items);  /* compute the size of the trans. */
    if (t->size <= 0) continue;    /* ignore empty transactions */
    ia_qsort(t->items, (size_t)t->size, 1); /* sort collected items */
    for (h = (size_t)*(s = t->items); ++s < d; )
      h = h *16777619 +(size_t)*s;
    *d++ = (ITEM)h;             /* compute and store the hash value */
    k =  h %  z;                /* compute hash bin index */
    x = (h % (z-2)) +1;         /* and probing step width */
    for ( ; htab[k]; k = (k+x) % z) {
      u = htab[k];              /* search transaction in hash table */
      if ((u->size != t->size)  /* if collision with diff. trans. */
      ||  (u->items[u->size] != (ITEM)h))
        continue;               /* skip the hash bin */
      for (m = t->size; --m >= 0; )
        if (t->items[m] != u->items[m]) break;
      if (m < 0) break;         /* if collision with same trans., */
    }                           /* abort probing (can combine) */
    if (htab[k]) htab[k]->wgt += taa[i]->wgt;
    else { htab[k] = t; t->wgt = taa[i]->wgt; t = (TRACT*)d; }
  }                             /* store transaction in hash table */
  for (p = taa, k = 0; k < z; k++) {
    if (!(t = htab[k])) continue;
    htab[k] = NULL;             /* traverse the hash table, */
    t->items[t->size] = TA_END; /* store a sentinel after the items */
    *p++ = t;                   /* and collect the transactions */
  }                             /* (unique occurrences) */
  n = (TID)(p -taa);            /* get the new number of tuples */
  return n;                     /* return the new number of trans. */
}  /* taa_reduce() */

#endif
/*--------------------------------------------------------------------*/
#ifndef NDEBUG

void taa_show (TRACT **taa, TID n, ITEMBASE *base)
{                               /* --- show a transaction array */
  assert(taa);                  /* check the function arguments */
  while (n-- > 0)               /* traverse the array and */
    ta_show(*taa++, base);      /* show each transaction */
}  /* taa_show() */

#endif
/*----------------------------------------------------------------------
  Transaction Tree Functions
----------------------------------------------------------------------*/
#ifdef TATREEFN
#ifdef TATCOMPACT

static int create (TANODE *node, TRACT **tracts, TID cnt, ITEM index)
{                               /* --- recursive part of tat_create() */
  TID    i;                     /* loop variable */
  ITEM   item, k, n;            /* item identifier and counter */
  TANODE *child;                /* created sibling node array */

  assert(tracts                 /* check the function arguments */
  &&    (cnt > 0) && (index >= 0));
  if (cnt <= 1) {               /* if only one transaction left */
    node->wgt  = (*tracts)->wgt;/* store weight and suffix length */
    node->max  = ((*tracts)->size -index) | ITEM_MIN;
    node->data = (*tracts)->items +index;
    return 0;                   /* store the transaction suffix and */
  }                             /* abort the function with success */
  node->max = 0; node->wgt = 0; /* init. weight and suffix length */
  while ((--cnt >= 0) && ((*tracts)->size <= index))
    node->wgt += (*tracts++)->wgt; /* skip trans. that are too short */
  for (n = 0, item = TA_END, i = cnt; i >= 0; i--) {
    node->wgt += tracts[i]->wgt;   /* traverse the transactions */
    k = tracts[i]->items[index];   /* and sum their weights */
    if (k != item) { item = k; n++; }
  }                             /* count the different items */
  child = (TANODE*)malloc((size_t)n *sizeof(TANODE) +sizeof(ITEM));
  if (!child) { node->data = NULL; return -1; }
  child[n].item = TA_END;       /* create a child node array and */
  node->data = child;           /* store a sentinel at the end, */
  node->max  = 1;               /* then store it in the parent node */
  for ( ; cnt >= 0; cnt = i) {  /* while there are transactions left */
    child->item = tracts[cnt]->items[index];
    for (i = cnt; i >= 0; i--)  /* find range of the next item */
      if (tracts[i]->items[index] != child->item) break;
    if (create(child, tracts+i+1, cnt-i, index+1) != 0) {
      free(node->data); node->data = NULL;
      node->max = 0; return -1; /* recursively fill the child nodes */
    }                           /* and on error clean up and abort */
    if ((k = (child->max & ~ITEM_MIN) +1) > node->max)
      node->max = k;            /* update the maximal suffix length */
    child++;                    /* and go to the next child node */
  }
  return 0;                     /* return 'ok' */
}  /* create() */

/*--------------------------------------------------------------------*/

TATREE* tat_create (TABAG *bag)
{                               /* --- create a transactions tree */
  TATREE *tree;                 /* created transaction tree */

  assert(bag);                  /* check the function argument */
  tree = (TATREE*)malloc(sizeof(TATREE));
  if (!tree) return NULL;       /* create the transaction tree body */
  tree->bag = bag;              /* note the underlying item set */
  if (bag->cnt <= 0) {          /* if the transaction bag is empty */
    tree->root.max  = 0; tree->root.wgt = 0;
    tree->root.data = tree->suffix; }  /* store empty trans. suffix */
  else {                        /* if the bag contains transactions */
    if (create(&tree->root, (TRACT**)bag->tracts, bag->cnt, 0) != 0) {
      tat_delete(tree, 0); return NULL; }
  }                             /* recursively build the tree */
  tree->root.item = (ITEM)-1;   /* root node represents no item */
  tree->suffix[0] = TA_END;     /* init. the empty trans. suffix */
  return tree;                  /* return the created tree */
}  /* tat_create() */

/*--------------------------------------------------------------------*/

void delete (TANODE *node)
{                               /* --- delete a transaction (sub)tree */
  TANODE *child;                /* to traverse the child nodes */

  assert(node && (node->max > 0)); /* check the function argument */
  for (child = (TANODE*)node->data; child->item >= 0; child++)
    if (child->max > 0) delete(child);
  free(node->data);             /* recursively delete the subtree */
}  /* delete() */

/*--------------------------------------------------------------------*/

void tat_delete (TATREE *tree, int del)
{                               /* --- delete a transaction tree */
  assert(tree);                 /* check the function argument */
  if (tree->root.max > 0)       /* if the root has children, */
    delete(&tree->root);        /* delete the nodes of the tree */
  if (tree->bag && del)         /* delete the transaction bag */
    tbg_delete(tree->bag, (del > 1));
  free(tree);                   /* delete the transaction tree body */
}  /* tat_delete() */

/*--------------------------------------------------------------------*/

static size_t nodecnt (const TANODE *node)
{                               /* --- count the nodes */
  size_t n = 1;                 /* node counter */
  TANODE *child;                /* to traverse the child nodes */

  assert(node && (node->max > 0)); /* check the function argument */
  for (child = (TANODE*)node->data; child->item >= 0; child++)
    if (child->max > 0) n += nodecnt(child);
  return n;                     /* recursively count the nodes */
}  /* nodecnt() */              /* return number of nodes in tree */

/*--------------------------------------------------------------------*/

size_t tat_size (const TATREE *tree)
{ return (tree->root.max > 0) ? nodecnt(&tree->root) : 1; }

/*--------------------------------------------------------------------*/

int tat_filter (TATREE *tree, ITEM min, const int *marks, int heap)
{                               /* --- filter a transaction tree */
  TABAG *bag;                   /* underlying transaction bag */

  assert(tree);                 /* check the function argument */
  delete(&tree->root);          /* delete the nodes of the tree */
  tbg_filter(bag = tree->bag, min, marks, 0);
  tbg_sort  (bag, 0, heap);     /* remove unnec. items and trans. */
  tbg_reduce(bag, 0);           /* and reduce trans. to unique ones */
  if (create(&tree->root, (TRACT**)bag->tracts, bag->cnt, 0) == 0)
    return 0;                   /* recreate the transaction tree */
  delete(&tree->root);          /* on failure delete the nodes */
  tree->root.max  = tree->root.wgt = 0;
  tree->root.data = tree->suffix;
  return -1;                    /* return an error indicator */
}  /* tat_filter() */

/*--------------------------------------------------------------------*/
#ifndef NDEBUG

static void show (TANODE *node, ITEMBASE *base, int ind)
{                               /* --- rekursive part of tat_show() */
  ITEM   i, k;                  /* loop variables */
  ITEM   *items;                /* transaction suffix */
  TANODE *child;                /* to traverse the child nodes */

  assert(node && (ind >= 0));   /* check the function arguments */
  if (node->max <= 0) {         /* if this is a leaf node */
    items = (ITEM*)node->data;  /* get the transaction suffix */
    for (k = node->max & ~ITEM_MIN, i = 0; i < k; i++)
      printf("%s ", ib_xname(base, items[i]));
    printf("[%"SUPP_FMT"]\n", node->wgt);
    return;                     /* print the items */
  }                             /* in the transaction suffix */
  child = (TANODE*)node->data;  /* get the child node array */
  for (i = 0; child->item != TA_END; i++, child++) {
    if (i > 0) for (k = 0; k < ind; k++) printf("  ");
    printf("%s ", ib_xname(base, child->item));
    show(child, base, ind+1);   /* traverse the items, print them, */
  }                             /* and show the children recursively */
}  /* show() */

/*--------------------------------------------------------------------*/

void tat_show (TATREE *tree)
{                               /* --- show a transaction tree */
  assert(tree);                 /* check the function argument */
  show(&tree->root, tbg_base(tree->bag), 0);
}  /* tat_show() */             /* call the recursive function */

#endif
/*--------------------------------------------------------------------*/
#else  /* #ifdef TATCOMPACT */

TANODE* tan_child (const TANODE *node, ITEM index)
{                               /* --- get child of a tree node */
  TANODE **chn;                 /* array of child nodes */

  assert(node);                 /* check the function argument */
  chn = (TANODE**)(node->items +node->size);
  ALIGN(chn);                   /* get the child pointer array */
  return chn[index];            /* return the requested child */
}  /* tan_child() */

/*--------------------------------------------------------------------*/

void delete (TANODE *root)
{                               /* --- delete a transaction (sub)tree */
  ITEM   i;                     /* loop variable */
  TANODE **chn;                 /* array of child nodes */

  assert(root);                 /* check the function argument */
  chn = (TANODE**)(root->items +root->size);
  ALIGN(chn);                   /* get the child pointer array */
  for (i = 0; i < root->size; i++)
    delete(chn[i]);             /* recursively delete the subtrees */
  free(root);                   /* and the tree node itself */
}  /* delete() */

/*--------------------------------------------------------------------*/

TANODE* create (TRACT **tracts, TID cnt, ITEM index)
{                               /* --- recursive part of tat_create() */
  TID    i;                     /* loop variable */
  ITEM   item, k, n;            /* item identifier and counter */
  SUPP   w;                     /* item weight */
  size_t z;                     /* size of the node with item array */
  TANODE *node;                 /* node of created transaction tree */
  TANODE **chn;                 /* array of child nodes */

  assert(tracts                 /* check the function arguments */
  &&    (cnt > 0) && (index >= 0));
  if (cnt <= 1) {               /* if only one transaction left */
    n    = (*tracts)->size -index;
    node = (TANODE*)malloc(sizeof(TANODE) +(size_t)(n-1) *sizeof(ITEM));
    if (!node) return NULL;     /* create a transaction tree node */
    node->wgt  = (*tracts)->wgt;/* and initialize the fields */
    node->size = -(node->max = n);
    if (n > 0)                  /* copy the transaction suffix */
      memcpy(node->items, (*tracts)->items +index,
                          (size_t)n *sizeof(ITEM));
    return node;                /* copy the remaining items and */
  }                             /* return the created leaf node */

  for (w = 0; (cnt > 0) && ((*tracts)->size <= index); cnt--)
    w += (*tracts++)->wgt;      /* skip trans. that are too short */
  for (n = 0, item = TA_END, i = cnt; --i >= 0; ) {
    w += tracts[i]->wgt;        /* traverse the transactions */
    k  = tracts[i]->items[index];
    if (k != item) { item = k; n++; }
  }                             /* count the different items */
  z = sizeof(TANODE) +(size_t)(n-1) *sizeof(ITEM);
  node = (TANODE*)malloc(z +PAD(z) +(size_t)n *sizeof(TANODE*));
  if (!node) return NULL;       /* create a transaction tree node */
  node->wgt  = w;               /* and initialize its fields */
  node->max  = 0;
  node->size = n;               /* if all transactions are captured, */
  if (n <= 0) return node;      /* return the created tree */
  chn = (TANODE**)(node->items +n);
  ALIGN(chn);                   /* get the child pointer array */
  for (--cnt; --n >= 0; cnt = i) { /* traverse the different items */
    node->items[n] = item = tracts[cnt]->items[index];
    for (i = cnt; --i >= 0; )   /* find trans. with the current item */
      if (tracts[i]->items[index] != item) break;
    chn[n] = create(tracts+i+1, cnt-i, index+1);
    if (!chn[n]) break;         /* recursively create a subtree */
    if ((k = chn[n]->max +1) > node->max) node->max = k;
  }                             /* adapt the maximal remaining size */
  if (n < 0) return node;       /* if successful, return created tree */

  while (++n < node->size) delete(chn[n]);
  free(node);                   /* on error delete created subtree */
  return NULL;                  /* return 'failure' */
}  /* create() */

/*--------------------------------------------------------------------*/

TATREE* tat_create (TABAG *bag)
{                               /* --- create a transactions tree */
  TATREE *tree;                 /* created transaction tree */

  assert(bag);                  /* check the function argument */
  tree = (TATREE*)malloc(sizeof(TATREE));
  if (!tree) return NULL;       /* create the transaction tree body */
  tree->bag  = bag;             /* note the underlying trans. bag */
  if (bag->cnt <= 0) {          /* if the transaction bag is empty, */
    tree->root = &tree->empty;  /* set an empty root node */
    tree->root->wgt = 0; tree->root->size = tree->root->max = 0; }
  else {                        /* if the bag contains transactions */
    tree->root = create((TRACT**)bag->tracts, bag->cnt, 0);
    if (!tree->root) { free(tree); return NULL; }
  }                             /* recursively build the tree */
  return tree;                  /* return the created trans. tree */
}  /* tat_create() */

/*--------------------------------------------------------------------*/

void tat_delete (TATREE *tree, int del)
{                               /* --- delete a transaction tree */
  assert(tree);                 /* check the function argument */
  delete(tree->root);           /* delete the nodes of the tree */
  if (tree->bag && del) tbg_delete(tree->bag, (del > 1));
  free(tree);                   /* delete the item base and */
}  /* tat_delete() */           /* the transaction tree body */

/*--------------------------------------------------------------------*/

static size_t nodecnt (const TANODE *node)
{                               /* --- count the nodes */
  ITEM   i;                     /* loop variable */
  size_t n;                     /* number of nodes */
  TANODE **chn;                 /* array of child nodes */

  assert(node);                 /* check the function argument */
  if (node->size <= 0)          /* if this is a leaf node, */
    return 1;                   /* there is only one node */
  chn = (TANODE**)(node->items +node->size);
  ALIGN(chn);                   /* get the child pointer array */
  for (n = 1, i = 0; i < node->size; i++)
    n += nodecnt(chn[i]);       /* recursively count the nodes */
  return n;                     /* return number of nodes in tree */
}  /* nodecnt() */

/*--------------------------------------------------------------------*/

size_t tat_size (const TATREE *tree)
{ return nodecnt(tree->root); }

/*--------------------------------------------------------------------*/

int tat_filter (TATREE *tree, ITEM min, const int *marks, int heap)
{                               /* --- filter a transaction tree */
  TABAG *bag;                   /* underlying transaction bag */

  assert(tree);                 /* check the function argument */
  delete(tree->root);           /* delete the nodes of the tree */
  tbg_filter(bag = tree->bag, min, marks, 0);
  tbg_sort  (bag, 0, heap);     /* remove unnec. items and trans. */
  tbg_reduce(bag, 0);           /* and reduce trans. to unique ones */
  tree->root = create((TRACT**)bag->tracts, bag->cnt, 0);
  if (tree->root) return 0;     /* recreate the transaction tree */
  delete(tree->root);           /* on failure delete the nodes */
  tree->root = NULL;            /* and clear the root pointer */
  return -1;                    /* return an error indicator */
}  /* tat_filter() */

/*--------------------------------------------------------------------*/
#ifndef NDEBUG

static void show (TANODE *node, ITEMBASE *base, int ind)
{                               /* --- rekursive part of tat_show() */
  ITEM   i, k;                  /* loop variables */
  TANODE **chn;                 /* array of child nodes */

  assert(node && (ind >= 0));   /* check the function arguments */
  if (node->size <= 0) {        /* if this is a leaf node */
    for (i = 0; i < node->max; i++)
      printf("%s ", ib_xname(base, node->items[i]));
    printf("[%"SUPP_FMT"]\n", node->wgt);
    return;                     /* print the items in the */
  }                             /* (rest of) the transaction */
  chn = (TANODE**)(node->items +node->size);
  ALIGN(chn);                   /* get the child pointer array */
  for (i = 0; i < node->size; i++) {
    if (i > 0) for (k = 0; k < ind; k++) printf("  ");
    printf("%s ", ib_xname(base, node->items[i]));
    show(chn[i], base, ind+1);  /* traverse the items, print them, */
  }                             /* and show the children recursively */
}  /* show() */

/*--------------------------------------------------------------------*/

void tat_show (TATREE *tree)
{                               /* --- show a transaction tree */
  assert(tree);                 /* check the function argument */
  show(tree->root, tbg_base(tree->bag), 0);
}  /* tat_show() */             /* call the recursive function */

#endif
#endif
#endif
/*----------------------------------------------------------------------
  Main Function
----------------------------------------------------------------------*/
#ifdef TA_MAIN

#ifndef NDEBUG                  /* if debug version */
  #undef  CLEANUP               /* clean up memory and close files */
  #define CLEANUP \
  if (tabag) tbg_delete(tabag, 0); \
  if (tread) trd_delete(tread, 1); \
  if (ibase) ib_delete (ibase);
#endif

GENERROR(error, exit)           /* generic error reporting function */

/*--------------------------------------------------------------------*/

int main (int argc, char *argv[])
{                               /* --- main function */
  int     i, k = 0;             /* loop variables */
  char    *s;                   /* to traverse the options */
  CCHAR   **optarg = NULL;      /* option argument */
  CCHAR   *fn_inp  = NULL;      /* name of input  file */
  CCHAR   *recseps = NULL;      /* record  separators */
  CCHAR   *fldseps = NULL;      /* field   separators */
  CCHAR   *blanks  = NULL;      /* blank   characters */
  CCHAR   *comment = NULL;      /* comment characters */
  double  supp     = -1;        /* minimum support */
  int     sort     = -2;        /* flag for item sorting and recoding */
  int     pack     =  0;        /* flag for packing 16 items */
  long    repeat   =  1;        /* number of repetitions */
  int     mtar     =  0;        /* mode for transaction reading */
  TRACT   **tracts = NULL;      /* array of transactions */
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
    printf("usage: %s [options] infile\n", argv[0]);
    printf("%s\n", DESCRIPTION);
    printf("%s\n", VERSION);
    printf("-s#      minimum support of an item set           "
                    "(default: %g)\n", supp);
    printf("         (positive: percentage, "
                     "negative: absolute number)\n");
    printf("-q#      sort items w.r.t. their frequency        "
                    "(default: %d)\n", sort);
    printf("         (1: ascending, -1: descending, 0: do not sort,\n"
           "          2: ascending, -2: descending w.r.t. "
                    "transaction size sum)\n");
    printf("-p       pack the 16 items with the lowest codes\n");
    printf("-x#      number of repetitions (for benchmarking) "
                    "(default: 1)\n");
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
    printf("infile   file to read transactions from           "
                    "[required]\n");
    return 0;                   /* print a usage message */
  }                             /* and abort the program */
  #endif  /* #ifndef QUIET */

  /* --- evaluate arguments --- */
  for (i = 1; i < argc; i++) {  /* traverse arguments */
    s = argv[i];                /* get option argument */
    if (optarg) { *optarg = s; optarg = NULL; continue; }
    if ((*s == '-') && *++s) {  /* -- if argument is an option */
      while (*s) {              /* traverse options */
        switch (*s++) {         /* evaluate switches */
          case 's': supp   =      strtod(s, &s);    break;
          case 'q': sort   = (int)strtol(s, &s, 0); break;
          case 'p': pack   = -1;                    break;
          case 'x': repeat =      strtol(s, &s, 0); break;
          case 'w': mtar  |= TA_WEIGHT;             break;
          case 'r': optarg = &recseps;              break;
          case 'f': optarg = &fldseps;              break;
          case 'b': optarg = &blanks;               break;
          case 'C': optarg = &comment;              break;
          default : error(E_OPTION, *--s);          break;
        }                       /* set option variables */
        if (optarg && *s) { *optarg = s; optarg = NULL; break; }
      } }                       /* get option argument */
    else {                      /* -- if argument is no option */
      switch (k++) {            /* evaluate non-options */
        case  0: fn_inp = s;      break;
        default: error(E_ARGCNT); break;
      }                         /* note filenames */
    }
  }
  if (optarg) error(E_OPTARG);  /* check (option) arguments */
  if (k < 1)  error(E_ARGCNT);  /* and number of arguments */
  MSG(stderr, "\n");            /* terminate the startup message */

  /* --- read transaction database --- */
  ibase = ib_create(0, 0);      /* create an item base */
  if (!ibase) error(E_NOMEM);   /* to manage the items */
  tread = trd_create();         /* create a transaction reader */
  if (!tread) error(E_NOMEM);   /* and configure the characters */
  trd_allchs(tread, recseps, fldseps, blanks, "", comment);
  tabag = tbg_create(ibase);    /* create a transaction bag */
  if (!tabag) error(E_NOMEM);   /* to store the transactions */
  CLOCK(t);                     /* start timer, open input file */
  if (trd_open(tread, NULL, fn_inp) != 0)
    error(E_FOPEN, trd_name(tread));
  MSG(stderr, "reading %s ... ", trd_name(tread));
  k = tbg_read(tabag, tread, mtar);
  if (k < 0)                    /* read the transaction database */
    error(-k, tbg_errmsg(tabag, NULL, 0));
  trd_delete(tread, 1);         /* close the input file and */
  tread = NULL;                 /* delete the table reader */
  m = ib_cnt(ibase);            /* get the number of items, */
  n = tbg_cnt(tabag);           /* the number of transactions, */
  w = tbg_wgt(tabag);           /* the total transaction weight */
  MSG(stderr, "[%"ITEM_FMT" item(s), %"TID_FMT, m, n);
  if (w != (SUPP)n) { MSG(stderr, "/%"SUPP_FMT, w); }
  MSG(stderr, " transaction(s)] done [%.2fs].", SEC_SINCE(t));
  if ((m <= 0) || (n <= 0))     /* check for at least one item */
    error(E_NOITEMS);           /* and at least one transaction */
  MSG(stderr, "\n");            /* compute absolute support value */
  supp = ceilsupp((supp >= 0) ? supp/100.0 *(double)w : -supp);

  /* --- sort and recode items --- */
  CLOCK(t);                     /* start timer, print log message */
  MSG(stderr, "filtering, sorting and recoding items ... ");
  m = tbg_recode(tabag, (SUPP)supp, -1, -1, sort);
  if (m <  0) error(E_NOMEM);   /* recode items and transactions */
  if (m <= 0) error(E_NOITEMS); /* and check the number of items */
  MSG(stderr, "[%"ITEM_FMT" item(s)]", m);
  MSG(stderr, " done [%.2fs].\n", SEC_SINCE(t));

  /* --- sort and reduce transactions --- */
  CLOCK(t);                     /* start timer, print log message */
  MSG(stderr, "sorting and reducing transactions ... ");
  tbg_filter(tabag, 0,NULL,0);  /* remove items of short transactions */
  tbg_itsort(tabag, +1, 0);     /* sort items in transactions */
  tracts = (TRACT**)malloc((size_t)n *sizeof(TRACT*));
  if (!tracts) error(E_NOMEM);  /* copy transactions to a buffer */
  memcpy(tracts, tabag->tracts, (size_t)n *sizeof(TRACT*));
  if (pack) tbg_pack(tabag,16); /* pack 16 items with lowest codes */
  for (i = 0; i < repeat; i++){ /* repeated sorting loop */
    memcpy(tabag->tracts, tracts, (size_t)n *sizeof(TRACT*));
    tbg_sort(tabag, +1, 0);     /* copy back the transactions */
  }                             /* and sort the transactions */
  n = tbg_reduce(tabag, 0);     /* reduce transactions to unique ones */
  free(tracts);                 /* delete the transaction buffer */
  MSG(stderr, "[%"TID_FMT, n);  /* print number of transactions */
  if (w != (SUPP)n) { MSG(stderr, "/%"SUPP_FMT, w); }
  MSG(stderr, " transaction(s)] done [%.2fs].\n", SEC_SINCE(t));

  /* --- clean up --- */
  CLEANUP;                      /* clean up memory and close files */
  SHOWMEM;                      /* show (final) memory usage */
  return 0;                     /* return 'ok' */
}  /* main() */

#endif
