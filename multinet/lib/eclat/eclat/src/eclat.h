/*----------------------------------------------------------------------
  File    : eclat.h
  Contents: eclat algorithm for finding frequent item sets
  Author  : Christian Borgelt
  History : 2011.08.22 file created
            2011.08.31 occurrence deliver variant ECL_OCCDLV added
            2012.04.17 interface for external call added (e.g. python)
            2014.08.19 adapted to modified item set reporter interface
            2014.08.21 parameter 'body' added to function eclat()
            2014.08.28 functions ecl_data() and ecl_report() added
            2016.11.10 eclat miner object and interface introduced
----------------------------------------------------------------------*/
#ifndef __ECLAT__
#define __ECLAT__
#include "report.h"
#include "ruleval.h"
#include "istree.h"

/*----------------------------------------------------------------------
  Preprocessor Definitions
----------------------------------------------------------------------*/
/* --- target pattern types --- */
#define ECL_FREQ      ISR_FREQUENT  /* frequent item sets */
#define ECL_FREQUENT  ISR_FREQUENT  /* frequent item sets */
#define ECL_CLOSED    ISR_CLOSED    /* closed  frequent item sets */
#define ECL_MAXIMAL   ISR_MAXIMAL   /* maximal frequent item sets */
#define ECL_GENERAS   ISR_GENERAS   /* generators */
#define ECL_RULES     ISR_RULES     /* association rules */

/* --- data preparation modes --- */
#define ECL_NORECODE  0x0001    /* do not sort and recode items */
#define ECL_NOFILTER  0x0002    /* do not filter transactions by size */
#define ECL_NOSORT    0x0004    /* do not sort items and transactions */
#define ECL_NOREDUCE  0x0008    /* do not reduce transactions */
#define ECL_NOPACK    0x0010    /* do not pack most frequent items */

/* --- evaluation measures --- */
/* most definitions in ruleval.h */
#define ECL_LDRATIO   RE_FNCNT  /* binary log. of support quotient */
#define ECL_INVBXS    IST_INVBXS/* inval. eval. below exp. supp. */

/* --- aggregation modes --- */
#define ECL_NONE      IST_NONE  /* no aggregation (use first value) */
#define ECL_FIRST     IST_FIRST /* no aggregation (use first value) */
#define ECL_MIN       IST_MIN   /* minimum of measure values */
#define ECL_MAX       IST_MAX   /* maximum of measure values */
#define ECL_AVG       IST_AVG   /* average of measure values */

/* --- algorithm variants --- */
#define ECL_BASIC     0         /* tid lists intersection (basic) */
#define ECL_LISTS     1         /* tid lists intersection (improved) */
#define ECL_BITS      2         /* bit vectors over transactions */
#define ECL_TABLE     3         /* item occurrence table (standard) */
#define ECL_SIMPLE    4         /* item occurrence table (simplified) */
#define ECL_RANGES    5         /* tid range lists intersection */
#define ECL_OCCDLV    6         /* occurrence deliver (LCM-style) */
#define ECL_DIFFS     7         /* tid difference sets (diffsets) */
#define ECL_AUTO      8         /* automatic choice based on data */

/* --- operation modes --- */
#define ECL_FIM16     0x001f    /* use 16 items machine (bit rep.) */
#define ECL_PERFECT   0x0020    /* perfect extension pruning */
#define ECL_REORDER   0x0040    /* reorder items in cond. databases */
#define ECL_ORIGSUPP  0x0080    /* use original support definition */
#define ECL_TAIL      0x0100    /* use head union tail pruning */
#define ECL_HORZ      0x0200    /* horizontal extensions tests */
#define ECL_VERT      0x0400    /* vertical   extensions tests */
#define ECL_TIDS      0x0800    /* flag for trans. identifier output */
#define ECL_PREFMT    0x4000    /* pre-format integer numbers */
#define ECL_EXTCHK    (ECL_HORZ|ECL_VERT)
#define ECL_DEFAULT   (ECL_PERFECT|ECL_REORDER|ECL_TAIL|ECL_FIM16)
#ifdef NDEBUG
#define ECL_NOCLEAN   0x8000    /* do not clean up memory */
#else                           /* in function eclat() */
#define ECL_NOCLEAN   0         /* in debug version */
#endif                          /* always clean up memory */
#define ECL_VERBOSE   INT_MIN   /* verbose message output */

/*----------------------------------------------------------------------
  Type Definitions
----------------------------------------------------------------------*/
typedef struct _eclat           /* eclat miner */
ECLAT;                          /* (opaque structure) */

/*----------------------------------------------------------------------
  Functions
----------------------------------------------------------------------*/

extern ECLAT* eclat_create (int target, double smin, double smax,
                            double conf, ITEM zmin, ITEM zmax,
                            int eval, int agg, double thresh,
                            int algo, int mode);
extern void   eclat_delete (ECLAT *eclat, int deldar);
extern int    eclat_data   (ECLAT *eclat, TABAG *tabag,
                            int mode, int sort);
extern int    eclat_report (ECLAT *eclat, ISREPORT *report);
extern int    eclat_mine   (ECLAT *eclat, ITEM prune, int order);
#endif
