/*----------------------------------------------------------------------
  File    : patspec.h
  Contents: pattern spectrum/statistics management
  Author  : Christian Borgelt
  History : 2012.10.12 file created (as patstat.h)
            2013.09.28 adapted to definitions of ITEM and RSUPP
            2013.09.30 sums per size and total sum of frequencies added
            2013.10.15 functions psp_error() and psp_clear() added
            2014.02.28 optional function psp_estim() added (PSP_ESTIM)
            2014.07.25 spectrum estimation for item sequences added
----------------------------------------------------------------------*/
#ifndef __PATSPEC__
#define __PATSPEC__
#ifdef PSP_TRAIN
#include "train.h"
#else
#include "tract.h"
#endif
#ifdef PSP_REPORT
#include "tabwrite.h"
#endif

/*--------------------------------------------------------------------*/

#ifndef RSUPP
#define RSUPP       SUPP        /* support type for reporting */
#define RSUPP_MIN   SUPP_MIN    /* minimum support value */
#define RSUPP_MAX   SUPP_MAX    /* maximum support value */
#define RSUPP_EPS   SUPP_EPS    /* minimum support step */
#define RSUPP_FMT   SUPP_FMT    /* printf format code for SUPP */

#else
#define int         1           /* to check definition of RSUPP */
#define long        2           /* for certain types */
#define ptrdiff_t   3
#define double      4

#if   RSUPP==int
#ifndef RSUPP_MIN
#define RSUPP_MIN   INT_MIN     /* minimum support value */
#endif
#ifndef RSUPP_MAX
#define RSUPP_MAX   INT_MAX     /* maximum support value */
#endif
#ifndef RSUPP_MAX
#define RSUPP_EPS   1           /* maximum support step */
#endif
#ifndef RSUPP_FMT
#define RSUPP_FMT   "d"         /* printf format code for int */
#endif

#elif RSUPP==long
#ifndef RSUPP_MIN
#define RSUPP_MIN   LONG_MIN    /* minimum support value */
#endif
#ifndef RSUPP_MAX
#define RSUPP_MAX   LONG_MAX    /* maximum support value */
#endif
#ifndef RSUPP_MAX
#define RSUPP_EPS   1           /* maximum support step */
#endif
#ifndef RSUPP_FMT
#define RSUPP_FMT   "ld"        /* printf format code for long */
#endif

#elif RSUPP==ptrdiff_t
#ifndef RSUPP_MIN
#define RSUPP_MIN   PTRDIFF_MIN /* minimum support value */
#endif
#ifndef RSUPP_MAX
#define RSUPP_MAX   PTRDIFF_MAX /* maximum support value */
#endif
#ifndef RSUPP_MAX
#define RSUPP_EPS   1           /* maximum support step */
#endif
#ifndef RSUPP_FMT
#  ifdef _MSC_VER
#  define RSUPP_FMT "Id"        /* printf format code for ptrdiff_t */
#  else
#  define RSUPP_FMT "td"        /* printf format code for ptrdiff_t */
#  endif                        /* MSC still does not support C99 */
#endif

#elif RSUPP==double
#ifndef RSUPP_MIN
#define RSUPP_MIN   (-INFINITY) /* minimum support value */
#endif
#ifndef RSUPP_MAX
#define RSUPP_MAX   INFINITY    /* maximum support value */
#endif
#ifndef RSUPP_MAX
#define RSUPP_EPS   0           /* maximum support step */
#endif
#ifndef RSUPP_FMT
#define RSUPP_FMT   "g"         /* printf format code for double */
#endif

#else
#error "RSUPP must be either 'int', 'long', 'ptrdiff_t' or 'double'"
#endif

#undef int                      /* remove preprocessor definitions */
#undef long                     /* needed for the type checking */
#undef ptrdiff_t
#undef double
#endif

/*----------------------------------------------------------------------
  Preprocessor Definitions
----------------------------------------------------------------------*/
/* --- estimation target indicators --- */
#define PSP_ITEMSET     0       /* item sets */
#define PSP_PERMUT      1       /* partial permutations (no reps.) */
#define PSP_SEQUENCE    2       /* sequences (with & without reps.) */

/*----------------------------------------------------------------------
  Type Definitions
----------------------------------------------------------------------*/
typedef struct {                /* --- pattern spectrum row -- */
  RSUPP  min, max;              /* minimum and maximum support */
  RSUPP  cur;                   /* current maximum support */
  size_t sum;                   /* sum of occurrences (for this size) */
  size_t *frqs;                 /* occurrence counters (by support) */
} PSPROW;                       /* (pattern spectrum row) */

typedef struct {                /* --- pattern spectrum --- */
  ITEM   minsize;               /* minimum pattern size (offset) */
  ITEM   maxsize;               /* maximum pattern size (limit) */
  RSUPP  minsupp;               /* minimum support (offset) */
  RSUPP  maxsupp;               /* maximum support (limit) */
  size_t sigcnt;                /* number of registered signatures */
  size_t total;                 /* total frequency of signatures */
  ITEM   max;                   /* number of pattern spectrum rows */
  ITEM   cur;                   /* current maximum used size/row */
  int    err;                   /* error status */
  PSPROW *rows;                 /* pattern spectrum rows (by size) */
} PATSPEC;                      /* (pattern spectrum) */

/*----------------------------------------------------------------------
  Functions
----------------------------------------------------------------------*/
extern PATSPEC* psp_create  (ITEM  minsize, ITEM  maxsize,
                             RSUPP minsupp, RSUPP maxsupp);
extern void     psp_delete  (PATSPEC *psp);
extern void     psp_clear   (PATSPEC *psp);
extern ITEM     psp_minsize (PATSPEC *psp);
extern ITEM     psp_maxsize (PATSPEC *psp);
extern ITEM     psp_min     (PATSPEC *psp);
extern ITEM     psp_max     (PATSPEC *psp);
extern RSUPP    psp_minsupp (PATSPEC *psp);
extern RSUPP    psp_maxsupp (PATSPEC *psp);
extern RSUPP    psp_min4sz  (PATSPEC *psp, ITEM size);
extern RSUPP    psp_max4sz  (PATSPEC *psp, ITEM size);
extern int      psp_error   (PATSPEC *psp);
extern size_t   psp_sigcnt  (PATSPEC *psp);
extern size_t   psp_total   (PATSPEC *psp);
extern size_t   psp_getfrq  (PATSPEC *psp, ITEM size, RSUPP supp);
extern int      psp_setfrq  (PATSPEC *psp, ITEM size, RSUPP supp,
                             size_t frq);
extern int      psp_incfrq  (PATSPEC *psp, ITEM size, RSUPP supp,
                             size_t frq);
extern int      psp_addpsp  (PATSPEC *dst, PATSPEC *src);
#ifdef PSP_ESTIM
extern int      psp_tbgest  (TABAG *tabag, PATSPEC *psp, size_t eqsur,
                             double alpha, size_t smpls);
#ifdef PSP_TRAIN
extern int      psp_tnsest  (TRAINSET *tns, PATSPEC *psp, size_t eqsur,
                             double width, double alpha, size_t smpls,
                             int seq);
#endif
#endif
#ifdef PSP_REPORT
extern int      psp_report  (PATSPEC *psp, TABWRITE *twr, double scale);
#endif
#ifndef NDEBUG
extern void     psp_show    (PATSPEC *psp);
#endif

/*----------------------------------------------------------------------
  Preprocessor Definitions
----------------------------------------------------------------------*/
#define psp_minsize(p)      ((p)->minsize)
#define psp_maxsize(p)      ((p)->maxsize)
#define psp_min(p)          ((p)->minsize)
#define psp_max(p)          ((p)->cur)
#define psp_minsupp(p)      ((p)->minsupp)
#define psp_maxsupp(p)      ((p)->maxsupp)
#define psp_min4sz(p,s)     ((p)->rows[s].min)
#define psp_max4sz(p,s)     ((p)->rows[s].cur)
#define psp_sum4sz(p,s)     ((p)->rows[s].sum)
#define psp_error(p)        ((p)->err)
#define psp_sigcnt(p)       ((p)->sigcnt)
#define psp_total(p)        ((p)->total)

#endif
