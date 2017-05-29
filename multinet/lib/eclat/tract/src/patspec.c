/*----------------------------------------------------------------------
  File    : patspec.c
  Contents: pattern spectrum management
  Author  : Christian Borgelt
  History : 2012.10.12 file created
            2013.09.28 adapted to definitions of ITEM and RSUPP
            2013.09.30 sums per size and total sum of frequencies added
            2013.10.15 functions psp_error() and psp_clear() added
            2013.10.16 made compatible with double support type
            2014.01.08 bug in function resize() fixed (size > n)
            2014.03.03 pattern spectrum estimation from trains added
            2014.06.17 bugs in functions taslots() and trnslots() fixed
            2014.07.25 spectrum estimation for item sequences added
            2014.10.24 treatment of non-integer support type corrected
            2016.10.05 slot counting with and without duplicate check
----------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <float.h>
#include <math.h>
#include <time.h>
#include <assert.h>
#ifdef PSP_MAIN
#ifndef PSP_REPORT
#define PSP_REPORT
#endif
#endif
#include "patspec.h"
#ifdef PSP_ESTIM
#include "gamma.h"
#include "random.h"
#endif
#ifdef PSP_MAIN
#include "error.h"
#endif
#ifdef STORAGE
#include "storage.h"
#endif

#ifdef _MSC_VER
#ifndef snprintf
#define snprintf _snprintf
#endif
#endif                          /* MSC still does not support C99 */

/*----------------------------------------------------------------------
  Preprocessor Definitions
----------------------------------------------------------------------*/
#define int         1           /* to check definition */
#define long        2           /* of support type */
#define ptrdiff_t   3
#define double      4

#if RSUPP==int || RSUPP==long || RSUPP==ptrdiff_t
#define INTSUPP     1           /* set   integer support flag */
#elif SUPP==double
#define INTSUPP     0           /* clear integer support flag */
#else
#error "RSUPP must be either 'int', 'long', 'ptrdiff_t' or 'double'"
#endif

#undef int                      /* remove preprocessor definitions */
#undef long                     /* needed for the type checking */
#undef ptrdiff_t
#undef double

/*--------------------------------------------------------------------*/
#define BLKSIZE      32         /* block size for enlarging arrays */

#ifdef PSP_MAIN
/* --- error codes --- */
/* error codes   0 to  -4 defined in tract.h */
#define E_STDIN      (-5)       /* double assignment of stdin */
#define E_OPTION     (-6)       /* unknown option */
#define E_OPTARG     (-7)       /* missing option argument */
#define E_ARGCNT     (-8)       /* too few/many arguments */

#ifndef QUIET                   /* if not quiet version, */
#define MSG         fprintf     /* print messages */
#define XMSG        if (mode & CCN_VERBOSE) fprintf
#else                           /* if quiet version, */
#define MSG(...)    ((void)0)   /* suppress messages */
#define XMSG(...)   ((void)0)
#endif

#define SEC_SINCE(t)  ((double)(clock()-(t)) /(double)CLOCKS_PER_SEC)
#endif

/*----------------------------------------------------------------------
  Type Definitions
----------------------------------------------------------------------*/
#if defined PSP_TRAIN && defined PSP_ESTIM

typedef struct {                /* --- heap element for slot counting */
  const double *next;           /* next point in train */
  ITEM         item;            /* associated item/neuron */
} HEAPELEM;                     /* (heap element) */

typedef struct {                /* --- point process element */
  double pt;                    /* point/spike time */
  ITEM   item;                  /* associated item/neuron */
} PPELEM;                       /* (point process element) */

#endif  /* #if defined PSP_TRAIN && defined PSP_ESTIM */
/*----------------------------------------------------------------------
  Global Variables
----------------------------------------------------------------------*/
static PSPROW empty = { RSUPP_MAX, RSUPP_MIN, RSUPP_MIN, 0, NULL };
/* an empty row entry for initializing new rows */

#ifdef PSP_MAIN
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
  /*            -9 */  "unknown error"
};
#endif

/*----------------------------------------------------------------------
  Global Variables
----------------------------------------------------------------------*/
#ifdef PSP_MAIN
static CCHAR    *prgname;       /* program name for error messages */
static PATSPEC  *psp    = NULL; /* pattern spectrum */
static TABWRITE *twrite = NULL; /* table writer */
#endif

/*----------------------------------------------------------------------
  Functions
----------------------------------------------------------------------*/

PATSPEC* psp_create (ITEM  minsize, ITEM  maxsize,
                     RSUPP minsupp, RSUPP maxsupp)
{                               /* --- create pattern spectrum */
  PATSPEC *psp;                 /* created pattern spectrum */

  assert((minsize >= 0) && (maxsize >= minsize)   /* check the */
  &&     (minsupp >= 0) && (maxsupp >= minsupp)); /* arguments */
  psp = (PATSPEC*)malloc(sizeof(PATSPEC));
  if (!psp) return NULL;        /* create the base structure */
  psp->minsize = minsize;       /* and initialize the fields */
  psp->maxsize = ((maxsize < 0) || (maxsize >= ITEM_MAX))
               ? ITEM_MAX -1 : maxsize;
  psp->minsupp = minsupp;
  psp->maxsupp = ((maxsupp < 0) || (maxsupp >= RSUPP_MAX))
               ? RSUPP_MAX-1 : maxsupp;
  psp->total   = psp->sigcnt = 0;
  psp->cur     = psp->max    = minsize -1;
  psp->err     = 0;
  psp->rows    = NULL;
  return psp;                   /* return created pattern spectrum */
}  /* psp_create() */

/*--------------------------------------------------------------------*/

void psp_delete (PATSPEC *psp)
{                               /* --- delete pattern spectrum */
  assert(psp);                  /* check the function argument */
  if (psp->rows) {              /* if there are pattern spectrum rows */
    #if INTSUPP                 /* if integer support type */
    ITEM size;                  /* loop variable */
    for (size = psp->minsize; size < psp->max; size++)
      if (psp->rows[size].frqs) free(psp->rows[size].frqs);
    #endif                      /* delete the counter arrays */
    free(psp->rows);            /* delete the row array */
  }
  free(psp);                    /* delete the base structure */
}  /* psp_delete() */

/*--------------------------------------------------------------------*/

void psp_clear (PATSPEC *psp)
{                               /* --- clear pattern spectrum */
  ITEM size;                    /* loop variable */

  assert(psp);                  /* check the function argument */
  if (psp->rows) {              /* if there are pattern spectrum rows */
    for (size = psp->minsize; size < psp->max; size++) {
      #if INTSUPP               /* if integer support type */
      if (psp->rows[size].frqs) free(psp->rows[size].frqs);
      #endif                    /* delete the counter arrays */
      psp->rows[size] = empty;  /* reinitialize the rows */
    }                           /* (also needed for RSUPP==double) */
  }
  psp->total = psp->sigcnt = 0; /* clear total and signature counter */
  psp->cur   = psp->max    = psp->minsize-1;
  psp->err   = 0;               /* clear the maximum size */
}  /* psp_clear() */            /* and the error status */

/*--------------------------------------------------------------------*/

size_t psp_getfrq (PATSPEC *psp, ITEM size, RSUPP supp)
{                               /* --- get a counter value */
  PSPROW *row;                  /* to access the table row */

  assert(psp);                  /* check the function arguments */
  if ((size < psp->minsize) || (size > psp->cur))
    return 0;                   /* if row does not exist, abort */
  row = psp->rows +size;        /* get the indexed row */
  if ((supp < row->min)     || (supp > row->cur))
    return 0;                   /* if counter does not exist, abort */
  #if INTSUPP                   /* if integer support type */
  return row->frqs[supp -row->min];     /* return the counter value */
  #else                         /* if double support type */
  return 1;                     /* return whether in supprt range */
  #endif
}  /* psp_getfrq() */

/*--------------------------------------------------------------------*/

static int resize (PATSPEC *psp, ITEM size, RSUPP supp)
{                               /* --- resize the row array (sizes) */
  size_t n, i;                  /* new maximum size, loop variable */
  PSPROW *row;                  /* to access/reallocate the rows */
  #if INTSUPP                   /* if integer support type */
  RSUPP  min, max;              /* new minimum and maximum support */
  size_t *p;                    /* to reallocate the counter array */
  #endif

  assert(psp                    /* check the function arguments */
  &&    (size >= psp->minsize) && (size <= psp->maxsize)
  &&    (supp >= psp->minsupp) && (supp <= psp->maxsupp));
  if (size > psp->max) {        /* if outside of size range */
    n = (psp->max > 0) ? (size_t)psp->max : 0;
    n += (n > BLKSIZE) ? n >> 1 : BLKSIZE;
    if (n < (size_t)size)         n = (size_t)size;
    if (n > (size_t)psp->maxsize) n = (size_t)psp->maxsize;
    row = (PSPROW*)realloc(psp->rows, (n+1) *sizeof(PSPROW));
    if (!row) return psp->err = -1;   /* enlarge the row array */
    for (i = (size_t)psp->max; ++i <= n; )
      row[i] = empty;           /* initialize the new elements */
    psp->rows = row;            /* set the new array */
    psp->max  = (ITEM)n;        /* and its size */
  }
  row = psp->rows +size;        /* get the indexed row */
  #if INTSUPP                   /* if integer support type */
  if ((supp >= row->min) && (supp <= row->max))
    return 0;                   /* if support is in range, abort */
  if      (!row->frqs)               min = supp     -BLKSIZE;
  else if (supp > row->min)          min = row->min;
  else if (supp > row->min -BLKSIZE) min = row->min -BLKSIZE;
  else                               min = supp;
  if (min < psp->minsupp +BLKSIZE)   min = psp->minsupp;
  if      (!row->frqs)               max = supp     +BLKSIZE;
  else if (supp < row->max)          max = row->max;
  else if (supp < row->max +BLKSIZE) max = row->max +BLKSIZE;
  else                               max = supp;
  if (max > psp->maxsupp)            max = psp->maxsupp;
  if (size <= 0) min = max = supp; /* only one counter for size = 0 */
  n = (size_t)max -(size_t)min +1; /* compute the new array size */
  p = (size_t*)realloc(row->frqs, n *sizeof(size_t));
  if (!p) return psp->err = -1; /* enlarge the counter array */
  if      (!row->frqs)          /* if new array created */
    memset(p, 0, n *sizeof(size_t));
  else if (supp > row->max) {   /* if enlarged at the end */
    n = (size_t)row->max -(size_t)row->min +1;
    memset(p +n, 0, (size_t)(max -row->max) *sizeof(size_t)); }
  else if (supp < row->min) {   /* if enlarged at the front */
    n = (size_t)(row->max -row->min +1);
    memmove(p +(row->min -min), p, n *sizeof(size_t));
    memset (p, 0, (size_t)(row->min -min) *sizeof(size_t));
  }                             /* move the existing counters, */
  row->frqs = p;                /* initialize the new elements */
  row->min  = min;              /* and set the new array */
  row->max  = max;              /* and its range */
  #else                         /* if double support type */
  if (supp < row->min) row->min = supp;
  if (supp > row->max) row->max = supp;
  #endif                        /* adapt the support range */
  return 0;                     /* return 'ok' */
}  /* resize() */

/*--------------------------------------------------------------------*/

int psp_setfrq (PATSPEC *psp, ITEM size, RSUPP supp, size_t frq)
{                               /* --- set a counter value */
  PSPROW *row;                  /* to access the table row */

  assert(psp);                  /* check the function arguments */
  if ((size < psp->minsize) || (size > psp->maxsize)
  ||  (supp < psp->minsupp) || (supp > psp->maxsupp))
    return 0;                   /* ignore values outside range */
  if (resize(psp, size, supp) < 0)
    return psp->err = -1;       /* enlarge table if necessary */
  if (size > psp->cur)          /* update the maximum size */
    psp->cur = size;            /* that is currently represented */
  row   = psp->rows +size;      /* get the indexed row and */
  #if INTSUPP                   /* if integer support type */
  if (supp > row->cur)          /* update the maximum support */
    row->cur = supp;            /* that is currently represented */
  supp -= row->min;             /* remove the support offset */
  if (frq > 0) { if (row->frqs[supp] <= 0) psp->sigcnt++; }
  else         { if (row->frqs[supp] >  0) psp->sigcnt--; }
  frq -= row->frqs[supp];       /* compute the frequency change */
  row->frqs[supp] += frq;       /* update the signature frequency */
  #endif
  row->sum        += frq;       /* and the sum for the size */
  psp->total      += frq;       /* as well as the total frequency */
  return 0;                     /* return 'ok' */
}  /* psp_setfrq() */

/*--------------------------------------------------------------------*/

int psp_incfrq (PATSPEC *psp, ITEM size, RSUPP supp, size_t frq)
{                               /* --- increase a counter value */
  PSPROW *row;                  /* to access the table row */

  assert(psp);                  /* check the function arguments */
  if ((size < psp->minsize) || (size > psp->maxsize)
  ||  (supp < psp->minsupp) || (supp > psp->maxsupp))
    return 0;                   /* ignore values outside range */
  if (resize(psp, size, supp) < 0)
    return psp->err = -1;       /* enlarge table if necessary */
  if (size > psp->cur)          /* update the maximum size */
    psp->cur = size;            /* that is currently represented */
  row   = psp->rows +size;      /* get the indexed row and */
  #if INTSUPP                   /* if integer support type */
  if (supp > row->cur)          /* update the maximum support */
    row->cur = supp;            /* that is currently represented */
  supp -= row->min;             /* remove the support offset */
  if ((row->frqs[supp] <= 0) && (row->frqs[supp] +frq > 0))
    psp->sigcnt++;              /* count a new signature */
  row->frqs[supp] += frq;       /* update the signature frequency */
  #else                         /* if double support type */
  psp->sigcnt++;                /* count a new signature */
  #endif
  row->sum        += frq;       /* update the sum for the size */
  psp->total      += frq;       /* as well as the total frequency */
  return 0;                     /* return 'ok' */
}  /* psp_incfrq() */

/*--------------------------------------------------------------------*/

int psp_addpsp (PATSPEC *dst, PATSPEC *src)
{                               /* --- add a spectrum to another */
  PSPROW *row;                  /* to traverse the rows (sizes) */
  ITEM   size;                  /* loop variable for sizes */
  #if INTSUPP
  SUPP   supp;                  /* loop variable for supports */
  size_t frq;                   /* (size,supp) signature frequency */
  #endif

  assert(dst && src);           /* check the function arguments */
  for (size = src->minsize; size <= src->max; size++) {
    row = src->rows +size;      /* traverse the rows (sizes) */
    #if INTSUPP                 /* if integer support type */
    if (!row->frqs) continue;   /* if no counters exist, skip row */
    for (supp = row->min; supp <= row->max; supp++)
      if ((frq = row->frqs[supp-row->min]) > 0)
        psp_incfrq(dst, size, supp, frq);
    #else                       /* if double support type */
    if (row->max < row->min) continue;
    if (resize(dst, size, row->min) < 0)
      return dst->err = -1;     /* enlarge table if necessary */
    psp_incfrq(dst, size, row->max, row->sum);
    #endif                      /* update the pattern spectrum */
  }                             /* with the source values */
  return dst->err;              /* return the error status */
}  /* psp_addpsp() */

/*--------------------------------------------------------------------*/
#ifdef PSP_ESTIM                /* if estimation from a train set */

static size_t choose (ITEM n, ITEM k)
{                               /* --- compute n choose k (size_t) */
  ITEM   i;                     /* loop variable */
  size_t r = 1;                 /* result */

  assert(k <= n);               /* check the function arguments */
  if (n-k < k) k = n-k;         /* minimize the loop executions */
  for (i = 1; i <= k; i++) {    /* calculation loop */
    if (SIZE_MAX/(size_t)n < r) /* if result of multiplication */
      return SIZE_MAX;          /* is out of range, abort */
    r = (r *(size_t)n--) /(size_t)i;
  }                             /* calculate \prod_{i=1}^k (n-i+1)/i */
  return r;                     /* return result */
}  /* choose() */

/*--------------------------------------------------------------------*/

static double dchoose (ITEM n, ITEM k)
{                               /* --- compute n choose k (double) */
  ITEM   i;                     /* loop variable */
  double r = 1.0;               /* result */

  assert((n >= 0) && (k >= 0) && (k <= n));
  if (n-k < k) k = n-k;         /* minimize the loop executions */
  for (i = 1; i <= k; i++)      /* calculate \prod_{i=1}^k (n-i+1)/i */
    r = (r *(double)n--) /(double)i;
  return r;                     /* return result */
}  /* dchoose() */

/*--------------------------------------------------------------------*/

static double samplelp (double *probs, ITEM n, ITEM k)
{                               /* --- get log. prob. of item sample */
  int    i;                     /* loop variable */
  ITEM   l;                     /* loop variable */
  double p1, p2, p3, p4, p;     /* item and sample probabilities */
  double t, r;                  /* buffers for probabilities */

  assert(probs);                /* check the function arguments */
  if (k <= 0) return 0.0;       /* check for an empty sample */
  if (k >  n) k = n;            /* ensure that sampling is possible */
  dbl_select(probs, (size_t)n, (size_t)k, drand);
  if (k <= 1) return log(probs[0]);
  if (k <= 2) {                 /* handle trivial cases directly */
    p1 = probs[0]; p2 = probs[1]; p = p1*p2;
    p  = p/(1.0-p1) +p/(1.0-p2);
    return log(p);              /* compute sample probability and */
  }                             /* return logarithm of probability */
  if (k <= 3) {                 /* if three items have been selected */
    p1 = probs[0]; p2 = probs[1]; p3 = probs[2]; p = p1*p2*p3;
    p  = (p/(1.0-p2) +p/(1.0-p3)) /(1.0-p2-p3)
       + (p/(1.0-p3) +p/(1.0-p1)) /(1.0-p1-p3)
       + (p/(1.0-p1) +p/(1.0-p2)) /(1.0-p1-p2);
    return log(p);              /* compute sample probability and */
  }                             /* return logarithm of probability */
  if (k <= 4) {                 /* if four items have been selected */
    p1 = probs[0]; p2 = probs[1];
    p3 = probs[2]; p4 = probs[3]; p = p1*p2*p3*p4;
    p  = ((p/(1.0-p3) +p/(1.0-p4)) /(1.0-p3-p4)
       +  (p/(1.0-p4) +p/(1.0-p2)) /(1.0-p2-p4)
       +  (p/(1.0-p2) +p/(1.0-p3)) /(1.0-p2-p3)) /(1.0-p2-p3-p4)
       + ((p/(1.0-p3) +p/(1.0-p4)) /(1.0-p3-p4)
       +  (p/(1.0-p4) +p/(1.0-p1)) /(1.0-p1-p4)
       +  (p/(1.0-p1) +p/(1.0-p3)) /(1.0-p1-p3)) /(1.0-p1-p3-p4)
       + ((p/(1.0-p2) +p/(1.0-p4)) /(1.0-p2-p4)
       +  (p/(1.0-p4) +p/(1.0-p1)) /(1.0-p1-p4)
       +  (p/(1.0-p1) +p/(1.0-p2)) /(1.0-p1-p2)) /(1.0-p1-p2-p4)
       + ((p/(1.0-p2) +p/(1.0-p3)) /(1.0-p2-p3)
       +  (p/(1.0-p3) +p/(1.0-p1)) /(1.0-p1-p3)
       +  (p/(1.0-p1) +p/(1.0-p2)) /(1.0-p1-p2)) /(1.0-p1-p2-p3);
    return log(p);              /* compute sample probability and */
  }                             /* return logarithm of probability */
  p = 0.0;                      /* initialize average probability */
  for (i = 0; i < 8; i++) {     /* draw 8/16 samples (permutations) */
    if (i <= 0) dbl_qsort  (probs, (size_t)k, +1);
    else        dbl_shuffle(probs, (size_t)k, drand);
    t = r = 1.0;                /* init. the probabilities */
    for (l = 0; l < k; l++) { t *= n *probs[l] /r; r -= probs[l]; }
    p += t;                     /* sum the permutation probability */
    dbl_reverse(probs, (size_t)k); /* reverse the permutation */
    t = r = 1.0;                /* init. the probabilities */
    for (l = 0; l < k; l++) { t *= n *probs[l] /r; r -= probs[l]; }
    p += t;                     /* sum the permutation probability */
    /* The factor n in each step, which is removed again with the */
    /* term -k*log(n), serves the purpose to prevent underflow.   */
  }                             /* return log. of sample probability */
  return log(p/16) -k*log(n) +logGamma(k+1);
}  /* samplelp() */

/*--------------------------------------------------------------------*/

static size_t* taslots (TABAG *tabag)
{                               /* --- count slots per item set size */
  ITEM   i, k, n;               /* loop variables, number of trains */
  TID    r;                     /* loop variable for transactions */
  size_t *cnts;                 /* number of slots per item set size */

  assert(tabag);                /* check the function arguments */
  n = tbg_itemcnt(tabag);       /* get the number of items */
  cnts = (size_t*)calloc((size_t)n+1, sizeof(size_t));
  if (!cnts) return NULL;       /* create a slot counter array */
  for (r = tbg_cnt(tabag); --r >= 0; )
    cnts[ta_size(tbg_tract(tabag, r))] += 1;
  while (--n > 0)               /* find the last non-zero entry */
    if (cnts[n] > 0) break;     /* (size of largest transaction) */
  for (i = 0; i <= n; i++)      /* traverse the item set sizes */
    for (k = 1; k < i; k++)     /* considering all possible subsets */
      cnts[k] += cnts[i] *choose(i,k);
  return cnts;                  /* return the slot counter array */
}  /* taslots() */

/*--------------------------------------------------------------------*/
#ifdef PSP_TRAIN

static void siftx (HEAPELEM *heap, ITEM lft, ITEM rgt)
{                               /* --- let element sift down in heap */
  ITEM     i;                   /* index of first successor in heap */
  HEAPELEM t;                   /* buffer for element to sift down */

  assert(heap && (lft >= 0) && (rgt > lft));
  t = heap[lft];                /* note the sift element */
  i = lft +lft +1;              /* compute index of first successor */
  do {                          /* sift loop */
    if ((i < rgt)               /* if second successor exists and */
    &&  (*heap[i+1].next < *heap[i].next)) /* smaller than first, */
      i++;                      /* go to the second successor */
    if (*heap[i].next >= *t.next)  /* if the successor is no less */
      break;                       /* than the sift element, abort */
    heap[lft] = heap[i];        /* let the successor ascend in heap */
    lft = i;                    /* note index of current element */
    i  += i+1;                  /* compute index of first successor */
  } while (i <= rgt);           /* while still within heap */
  heap[lft] = t;                /* store the sift element */
}  /* siftx() */

/*--------------------------------------------------------------------*/

static size_t* slots_perm (TRAINSET *tns, double width)
{                               /* --- count slots per item set size */
  ITEM     i, k, n;             /* loop variables, number of trains */
  size_t   l, r;                /* left and right point in window */
  size_t   *cnts;               /* number of slots per item set size */
  size_t   *occs;               /* number of occurrences in window */
  PPELEM   *pp;                 /* point process (all points sorted) */
  HEAPELEM *heap;               /* minimum heap for points */

  assert(tns && (width > 0));   /* check the function arguments */
  n = tns_cnt(tns);             /* get the number of trains */
  cnts = (size_t*)calloc(2*(size_t)(n+1), sizeof(size_t));
  if (!cnts) return NULL;       /* create a slot counter array */
  occs = cnts +n+1;             /* and an item occurrence array */
  pp = (PPELEM*)malloc((tns_extent(tns)+1) *sizeof(PPELEM));
  if (!pp) { free(cnts); return NULL; }
  heap = (HEAPELEM*)malloc((size_t)n *sizeof(HEAPELEM));
  if (!heap) { free(pp); free(cnts); return NULL; }
  for (i = 0; i < n; i++) {     /* traverse the trains in the set */
    heap[i].next = trn_points(tns_train(tns, i));
    heap[i].item = i;           /* initialize the heap elements */
  }                             /* with first points and item */
  for (i = n >> 1; --i >= 0; )  /* traverse left half of heap */
    siftx(heap, i, n-1);        /* and build the point heap */
  for (l = 0; *heap[0].next < TRN_END; l++) {
    pp[l].pt   = *heap[0].next++;
    pp[l].item =  heap[0].item; /* store the next point/spike time, */
    siftx(heap, 0, n-1);        /* remove the point from the heap, */
  }                             /* and let the next point sift down */
  pp[l].pt   = TRN_END;         /* add a sentinel at the end */
  pp[l].item = -1;              /* of the point process */
  free(heap);                   /* delete the point heap */
  k = 0;                        /* init. the number of items */
  for (l = r = 0; pp[l].item >= 0; l++) { /* slot counting loop */
    while (pp[r].pt -pp[l].pt <= width) {
      if (occs[pp[r].item]++ <= 0) k += 1;
      r++;                      /* update the occurrence counters, */
    }                           /* and go to the next point */
    cnts[k] += 1;               /* count window for number of items */
    if (--occs[pp[l].item] <= 0) k -= 1;
  }                             /* update the occurrence counters */
  free(pp);                     /* delete the point process */
  cnts = (size_t*)realloc(cnts, (size_t)n *sizeof(size_t));
  while (--n > 0)               /* shrink to the counter array and */
    if (cnts[n] > 0) break;     /* find the last non-zero entry */
  for (i = 0; i <= n; i++)      /* traverse the item set sizes */
    for (k = 1; k < i; k++)     /* considering all possible subsets */
      cnts[k] += cnts[i] *choose(i-1,k-1);
  return cnts;                  /* return the slot counter array */
}  /* slots_perm() */

/*--------------------------------------------------------------------*/
/* This version of counting the slots per size ensures that events    */
/* with the same item do not increase the slot size. This yields a    */
/* better estimate of the slots distribution than the next version.   */
/*--------------------------------------------------------------------*/

static void sift (const double *heap[], ITEM lft, ITEM rgt)
{                               /* --- let element sift down in heap */
  ITEM  i;                      /* index of first successor in heap */
  const double *t;              /* buffer for element to sift down */

  assert(heap && (lft >= 0) && (rgt > lft));
  t = heap[lft];                /* note the sift element */
  i = lft +lft +1;              /* compute index of first successor */
  do {                          /* sift loop */
    if ((i          < rgt)      /* if second successor exists and */
    &&  (*heap[i+1] < *heap[i]))/* it is smaller than the first, */
      i++;                      /* go to the second successor */
    if (*heap[i] >= *t)         /* if the successor is no less */
      break;                    /* than the sift element, abort */
    heap[lft] = heap[i];        /* let the successor ascend in heap */
    lft = i;                    /* note index of current element */
    i  += i+1;                  /* compute index of first successor */
  } while (i <= rgt);           /* while still within heap */
  heap[lft] = t;                /* store the sift element */
}  /* sift() */

/*--------------------------------------------------------------------*/

static size_t* slots_seqs (TRAINSET *tns, double width)
{                               /* --- count slots per item set size */
  ITEM   i, k, n;               /* loop variables, number of trains */
  size_t l, r, c;               /* left and right point in window */
  size_t *cnts;                 /* number of slots per item set size */
  double *pp;                   /* point process (all points sorted) */
  const double **heap;          /* minimum heap for points */

  assert(tns && (width > 0));   /* check the function arguments */
  n = tns_cnt(tns);             /* get the number of trains */
  cnts = (size_t*)calloc((size_t)n+1, sizeof(size_t));
  if (!cnts) return NULL;       /* create a slot counter array */
  pp = (double*)malloc((tns_extent(tns)+1) *sizeof(double));
  if (!pp) { free(cnts); return NULL; } /* point process and heap */
  heap = (const double**)malloc((size_t)n *sizeof(double*));
  if (!heap) { free(pp); free(cnts); return NULL; }
  for (i = 0; i < n; i++)       /* collect the trains in the set */
    heap[i] = trn_points(tns_train(tns, i));
  for (i = n >> 1; --i >= 0; )  /* traverse left half of heap */
    sift(heap, i, n-1);         /* and build the point heap */
  for (l = 0; *heap[0] < TRN_END; l++) {
    pp[l] = *heap[0]++;         /* store the next point/spike time */
    sift(heap, 0, n-1);         /* and let the next point sift down */
  }                             /* in the point heap */
  pp[l] = TRN_END;              /* store a sentinel at the end */
  free((void*)heap);            /* delete the point heap */
  k = 0;                        /* init. the number of items */
  for (l = r = 0; pp[l] < TRN_END; l++) { /* slot counting loop */
    while (pp[r] -pp[l] <= width)
      r++;                      /* add next point while inside window */
    c = r-l; if (c > (size_t)n) c = (size_t)n;
    cnts[c] += 1;               /* check the window size and */
  }                             /* count window for number of items */
  free(pp);                     /* delete the point process */
  while (--n > 0)               /* shrink to the counter array and */
    if (cnts[n] > 0) break;     /* find the last non-zero entry */
  for (i = 0; i <= n; i++)      /* traverse the item set sizes */
    for (k = 1; k < i; k++)     /* considering all possible subsets */
      cnts[k] += cnts[i] *choose(i-1,k-1);
  return cnts;                  /* return the slot counter array */
}  /* slots_seqs() */

/*--------------------------------------------------------------------*/
/* This version of counting the slots is faster than using two heaps  */
/* (one for the left and one for the right window border) or using    */
/* the system qsort() function to sort the points.                    */
/*--------------------------------------------------------------------*/
#endif

static int simpest (PATSPEC *psp, size_t *cnts, ITEM n,
                    RSUPP smax, size_t equiv, int seq)
{                               /* --- simple estimation (eq. probs.) */
  ITEM   z;                     /* loop variable, number of items */
  RSUPP  c;                     /* loop variable (support/coin.) */
  size_t frq;                   /* frequency of a signature */
  double l, x, y;               /* buffers for various purposes */

  assert(psp && cnts            /* check the function arguments */
  &&    (n > 0) && (smax > 0) && (equiv > 0));
  for (z = psp->minsize; z < n; z++) {
    if (cnts[z] <= 0) continue; /* if there are no slots, skip size */
    y = logGamma(n+1) -logGamma(n-z+1);
    if (!seq) y -= logGamma(z+1);
    l = log((double)cnts[z])-y; /* compute distribution parameter */
    x = y -exp(l);              /* and start of Poisson distribution */
    for (c = 1; c <= smax; c++) {
      x += y = l -log(c);       /* compute next distribution value */
      frq = (size_t)(exp(x) *(double)equiv +0.5);
      if (frq <= 0) {           /* check value against threshold */
        if (y > 0) continue; else break; }
      if (psp_incfrq(psp, z, c, frq) != 0) {
        free(cnts); return -1; }
    }                           /* add computed signature frequency */
  }                             /* to the pattern spectrum */
  return 0;                     /* return 'ok' */
}  /* simpest() */

/*--------------------------------------------------------------------*/

static int cplxest (PATSPEC *psp, size_t *cnts, double *probs, ITEM n,
                    RSUPP smax, size_t equiv, size_t smpls, int seq)
{                               /* --- complex est. (diff. probs.) */
  ITEM   z;                     /* loop variable, number of items */
  RSUPP  c, s;                  /* loop variables (coin./support) */
  size_t i;                     /* loop variable (samples) */
  size_t frq;                   /* frequency of a signature */
  double *dist;                 /* probability distribution */
  double t, l, x, y, q;         /* buffers for various purposes */

  assert(psp && cnts && probs   /* check the function arguments */
  &&    (n > 0) && (smax > 0) && (equiv > 0) && (smpls > 0));
  dist = probs +n;              /* get probability distribution */
  s    = smax+1;                /* clear fully on first loop */
  for (z = psp->minsize; z < n; z++) { /* traverse pattern sizes */
    if (cnts[z] <= 0) continue; /* if there are no slots, skip size */
    memset(dist, 0, (size_t)s *sizeof(double));
    s = 0;                      /* clear distrib. and get threshold */
    q = (seq) ? logGamma(z+1) : 0;
    y = logGamma(n+1) -q -logGamma(n-z+1);
    t = -log((double)equiv) -y -9;   /* compute threshold for dist. */
    for (i = 0; i < smpls; i++){/* draw a certain number of samples */
      l = log((double)cnts[z]) +samplelp(probs, n, z) -q;
      x = -exp(l);              /* compute distribution parameter */
      dist[0] += exp(x);        /* store for distribution value */
      for (c = 1; c <= smax; c++) {
        x += y = l -log((double)c);  /* traverse the coincidences */
        if      (x >= t) dist[(size_t)c] += exp(x);
        else if (y <= 0) break; /* compute next distribution value */
      }                         /* update probability distribution */
      if (c > s) s = c;         /* update maximum coincidence count */
    }                           /* (for clearing the distribution) */
    x = dchoose(n,z) /(double)smpls;
    y = 0.0;                    /* compute the scaling factor */
    for (c = 0; c < s; c++) {   /* traverse the distribution */
      dist[(size_t)c] *= x;     /* scale probability distribution */
      y += (double)c *dist[(size_t)c]; /* sum the covered slots */
    }                                  /* (for normalization) */
    x = (y > 0) ? (double)cnts[z] /y : 1.0;
    for (c = psp->minsupp; c < s; c++) {
      dist[(size_t)c] *= x;     /* normalize the number of slots */
      frq = (size_t)(dist[(size_t)c] *(double)equiv +0.5);
      if (frq <= 0) continue;   /* compute equivalent frequency */
      if (psp_incfrq(psp, z, c, frq) != 0) return -1;
    }                           /* add computed signature frequency */
  }                             /* to the pattern spectrum */
  return 0;                     /* return 'ok' */
}  /* cplxest() */

/*--------------------------------------------------------------------*/

int psp_tbgest (TABAG *tabag, PATSPEC *psp, size_t equiv,
                double alpha, size_t smpls)
{                               /* --- estimate a pattern spectrum */
  int      r;                   /* result of function call */
  ITEM     z, n;                /* loop variable, number of items */
  RSUPP    smax;                /* loop variables (coin./support) */
  size_t   *cnts;               /* slot counters per item set size */
  double   *probs;              /* item occurrence probabilities */
  ITEMBASE *base;               /* underlying item base */
  double   x;                   /* buffers for computations */

  assert(tabag && psp           /* check the function arguments */
  &&    (equiv > 0) && (smpls > 0));
  cnts = taslots(tabag);        /* count slots per item set size */
  if (!cnts) return -1;         /* (evaluate transaction sizes) */
  n    = tbg_itemcnt(tabag);    /* get the number of items */
  smax = (RSUPP)tbg_cnt(tabag); /* and the number of transactions */
  if (alpha <= 0) {             /* check for equal probabilities */
    r = simpest(psp, cnts, n, smax, equiv, 0);
    free(cnts); return r;       /* execute simple estimation */
  }                             /* and delete the counter array */
  probs = (double*)malloc(((size_t)n +(size_t)smax +1) *sizeof(double));
  if (!probs) { free(cnts); return -1; } /* allocate work memory */
  x    = 1.0 /(double)tbg_extent(tabag);
  base = tbg_base(tabag);       /* get the underlying item base, */
  for (z = 0; z < n; z++)       /* traverse items and compute probs. */
    probs[z] = x *(double)ib_getfrq(base, z);
  if (alpha != 1.0) {           /* if to contract rate distribution */
    x = 1.0/(double)n;          /* compute average item probability */
    for (z = 0; z < n; z++)     /* and multiply deviation with alpha */
      probs[z] = (probs[z]-x) *alpha +x;
  }                             /* estimate the pattern spectrum */
  r = cplxest(psp, cnts, probs, n, smax, equiv, smpls, 0);
  free(probs); free(cnts);      /* delete the working memory */
  return r;                     /* return the error status */
}  /* psp_tbgest() */

/*--------------------------------------------------------------------*/
#ifdef PSP_TRAIN

int psp_tnsest (TRAINSET *tns, PATSPEC *psp, size_t equiv,
                double width, double alpha, size_t smpls, int target)
{                               /* --- estimate a pattern spectrum */
  int    r;                     /* result of function call */
  ITEM   z, n;                  /* loop variable, number of items */
  RSUPP  smax;                  /* maximum length of a train */
  size_t *cnts;                 /* slot counters per item set size */
  double *probs;                /* item occurrence probabilities */
  double x;                     /* buffers for computations */

  assert(tns && psp             /* check the function arguments */
  &&    (equiv > 0) && (width > 0) && (smpls > 0));
  cnts = (target < PSP_SEQUENCE)/* count the slots per item set size */
       ? slots_perm(tns, width) : slots_seqs(tns, width);
  if (!cnts) return -1;         /* (evaluate points in trains) */
  n    = tns_cnt(tns);          /* get the number of trains */
  smax = tns_maxlen(tns);       /* and the maximum train length */
  if (alpha <= 0) {             /* check for equal probabilities */
    r = simpest(psp, cnts, n, smax, equiv, (target > PSP_ITEMSET));
    free(cnts); return r;       /* execute simple estimation */
  }                             /* and delete the counter array */
  probs = (double*)malloc(((size_t)n +(size_t)smax +1) *sizeof(double));
  if (!probs) { free(cnts); return -1; } /* allocate work memory */
  x = 1.0 /(double)tns_extent(tns);
  for (z = 0; z < n; z++)       /* traverse items and compute probs. */
    probs[z] = x *(double)trn_len(tns_train(tns, z));
  if (alpha != 1.0) {           /* if to contract rate distribution */
    x = 1.0/(double)n;          /* compute average item probability */
    for (z = 0; z < n; z++)     /* and multiply deviation with alpha */
      probs[z] = (probs[z]-x) *alpha +x;
  }                             /* estimate the pattern spectrum */
  r = cplxest(psp, cnts, probs, n, smax, equiv, smpls,
              (target > PSP_ITEMSET));
  free(probs); free(cnts);      /* delete the working memory */
  return r;                     /* return the error status */
}  /* psp_tnsest() */

#endif  /* #ifdef PSP_TRAIN */
#endif  /* #ifdef PSP_ESTIM */
/*--------------------------------------------------------------------*/
#ifdef PSP_REPORT

int psp_report (PATSPEC *psp, TABWRITE *twr, double scale)
{                               /* --- report pattern spectrum */
  PSPROW *row;                  /* to traverse the rows (sizes) */
  ITEM   size;                  /* loop variable for sizes */
  #if INTSUPP                   /* if integer support type */
  RSUPP  supp;                  /* loop variable for supports */
  size_t frq;                   /* (size,supp) signature frequency */
  char   buf[64];               /* output buffer for pattern size */
  #endif

  assert(psp && twr);           /* check the function arguments */
  for (size = psp->minsize; size <= psp->max; size++) {
    row = psp->rows +size;      /* traverse the rows (sizes) */
    #if INTSUPP                 /* if integer support type */
    if (!row->frqs) continue;   /* if no counters exist, skip row */
    snprintf(buf, sizeof(buf), "%"ITEM_FMT, size);
    for (supp = row->min; supp <= row->max; supp++) {
      if ((frq = row->frqs[supp-row->min]) <= 0)
        continue;               /* traverse the columns (support) */
      twr_puts(twr, buf);                       twr_fldsep(twr);
      twr_printf(twr, "%"RSUPP_FMT, supp);      twr_fldsep(twr);
      twr_printf(twr, "%g", scale*(double)frq); twr_recsep(twr);
    }                           /* print (size,support,counter) */
    #else                       /* if double support type */
    if (row->max < row->min) continue;
    twr_printf(twr, "%"ITEM_FMT, size);      twr_fldsep(twr);
    twr_printf(twr, "%"RSUPP_FMT, row->min); twr_fldsep(twr);
    twr_printf(twr, "%"RSUPP_FMT, row->max); twr_recsep(twr);
    #endif                      /* print (size,minsupp,maxsupp) */
  }
  return twr_error(twr);        /* return a write error indicator */
}  /* psp_report() */

#endif  /* #ifdef PSP_REPORT */
/*--------------------------------------------------------------------*/
#ifndef NDEBUG

void psp_show (PATSPEC *psp)
{                               /* --- show a pattern spectrum */
  ITEM   size;                  /* loop variable for sizes */
  PSPROW *row;                  /* to traverse the rows (sizes) */
  #if INTSUPP                   /* if integer support type */
  RSUPP  supp;                  /* loop variable for support values */
  size_t frq;                   /* (size,supp) signature frequency */
  #endif

  assert(psp);                  /* check the function argument */
  printf("sigcnt: %"SIZE_FMT"\n", psp->sigcnt);
  if (!psp->rows) return;       /* if there is no row array, abort */
  for (size = psp->minsize; size <= psp->maxsize; size++) {
    row = psp->rows +size;      /* traverse the rows (sizes) */
    #if INTSUPP                 /* if integer support type */
    if (!row->frqs) continue;   /* if no counters exist, skip row */
    printf("%3"ITEM_FMT":", size);
    for (supp = row->min; supp <= row->max; supp++)
      if ((frq = row->frqs[supp-row->min]) > 0)
        printf(" %"RSUPP_FMT":%"SIZE_FMT, supp, frq);
    printf("\n");               /* print support:frequency pairs */
    #else                       /* if double support type */
    printf("%3"ITEM_FMT":", size);
    printf("%"RSUPP_FMT":%"RSUPP_FMT"\n", row->min, row->max);
    #endif                      /* print (size,minsupp,maxsupp) */
  }
  printf("sigcnt: %"SIZE_FMT"\n", psp->sigcnt);
  printf("total : %"SIZE_FMT"\n", psp->total);
}  /* psp_show() */

#endif
/*----------------------------------------------------------------------
  Main Function
----------------------------------------------------------------------*/
#ifdef PSP_MAIN

#ifndef NDEBUG                  /* if debug version */
  #undef  CLEANUP               /* clean up memory and close files */
  #define CLEANUP \
  if (twrite) twr_delete(twrite, 1); \
  if (psp)    psp_delete(psp);
#endif

GENERROR(error, exit)           /* generic error reporting function */

/*--------------------------------------------------------------------*/

int main (int argc, char *argv[])
{                               /* --- main function for testing */
  int    i;                     /* loop variable */
  size_t equiv = 1;             /* equivalent number of surrogates */
  ITEM   size;                  /* size    of a signature */
  RSUPP  supp;                  /* support of a signature */

  psp = psp_create(2, 12, 2, 12);
  if (!psp) error(E_NOMEM);     /* create a pattern spectrum */
  for (i = 0; i < 10000; i++) { /* create some random signatures */
    size = (ITEM) (16 *(double)rand()/((double)RAND_MAX +1));
    supp = (RSUPP)(16 *(double)rand()/((double)RAND_MAX +1));
    #if 0
    printf("%d: (%"ITEM_FMT",%"RSUPP_FMT")\n", i, size, supp);
    #endif
    psp_incfrq(psp, size, supp, 1);
  }                             /* register each signature */
  twrite = twr_create();        /* create a table writer and */
  if (!twrite) error(E_NOMEM);  /* configure the characters */
  twr_xchars(twrite, "\n", " ", " ", "?");
  if (twr_open(twrite, NULL, "") != 0)
    error(E_FOPEN, twr_name(twrite));
  psp_report(psp, twrite, 1.0/(double)equiv);
  twr_delete(twrite, 1);        /* report the pattern spectrum */
  twrite = NULL;                /* and delete the table writer */
  printf("sigcnt: %"SIZE_FMT"\n", psp_sigcnt(psp));
  printf("total:  %"SIZE_FMT"\n", psp_total(psp));
  return 0;                     /* return 'ok' */
}  /* main() */

#endif
