/*----------------------------------------------------------------------
  File    : report.h
  Contents: item set reporter management
  Author  : Christian Borgelt
  History : 2008.08.18 item set reporter created in tract.[ch]
            2008.08.30 handling of perfect extensions completed
            2008.09.01 handling of closed and maximal item sets added
            2008.10.15 function isr_xable() added (reporter extendable)
            2008.10.30 transaction identifier reporting added
            2008.10.31 item set reporter made a separate module
            2009.10.15 counter for reported item sets added
            2009.10.16 functions isr_wgt() and isr_wgtx() added
            2010.04.07 extended information reporting functions removed
            2010.07.02 function isr_all() added for easier access
            2010.07.22 adapted to closed/maximal item set filter
            2010.08.06 function isr_iset() for direct reporting added
            2010.08.11 function isr_isetx() for extended items added
            2010.08.14 item set header for output added to isr_create()
            2010.10.15 functions isr_open(), isr_close(), isr_rule()
            2011.05.06 generalized to support type RSUPP (int/double)
            2011.06.10 function isr_wgtsupp() added (weight/support)
            2011.07.23 parameter dir added to function isr_seteval()
            2011.08.12 definition of ISR_GENERA added (for generators)
            2011.08.17 structure ISREPORT reorganized (fields moved)
            2011.09.20 flag ISR_NOFILTER added (no internal filtering)
            2011.10.05 type of item set counters changed to long int
            2012.05.30 function isr_addpexpk() added (packed items)
            2012.10.15 minimum and maximum support added
            2013.03.18 function isr_check() added (check for a superset)
            2013.10.08 function isr_seqrule() added (head at end)
            2013.10.15 result of isr_iset[x]() and isr_[seq]rule()
            2014.05.12 filtering border for item signatures added
            2014.08.05 function isr_reportv() added (with evaluation)
            2014.08.18 creation and configuration functions cleaned up
            2014.08.27 functions isr_settarg(), isr_target() etc. added
            2014.09.02 return type of reporting functions changed to int
            2014.09.18 functions isr_reprule(), isr_setrule() added
            2015.02.20 functions isr_extrule() and isr_xinfo() added
            2015.06.04 function isr_createx() added (add. parameter)
            2015.02.18 function isr_repfmt() added (preformat integers)
            2016.03.31 function isr_ibname() added (base item names)
            2016.04.10 functions isr_extrule() and isr_xinfo() extended
            2016.08.27 bug in item statistics reporting fixed
            2016.09.29 function isr_sxrule() added (explicit head item)
            2016.10.14 function isr_size() added (item array size)
----------------------------------------------------------------------*/
#ifndef __REPORT__
#define __REPORT__
#include <limits.h>
#include <math.h>
#ifdef ISR_PATSPEC
#include "patspec.h"
#endif
#ifdef ISR_CLOMAX
#include "clomax.h"
#endif
#include "tract.h"

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
#ifndef RSUPP_EPS
#define RSUPP_EPS   1           /* minimum support step */
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
#ifndef RSUPP_EPS
#define RSUPP_EPS   1           /* minimum support step */
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
#ifndef RSUPP_EPS
#define RSUPP_EPS   1           /* minimum support step */
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
#ifndef RSUPP_EPS
#define RSUPP_EPS   0.0         /* minimum support step */
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

/*--------------------------------------------------------------------*/

#ifndef CCHAR
#define CCHAR const char        /* abbreviation */
#endif

/*----------------------------------------------------------------------
  Preprocessor Definitions
----------------------------------------------------------------------*/
/* --- targets (for isr_settarg()) --- */
#define ISR_SETS      0x0000    /* report all frequent item sets */
#define ISR_ALL       0x0000    /* report all frequent item sets */
#define ISR_FREQ      0x0000    /* report all frequent item sets */
#define ISR_FREQUENT  0x0000    /* report all frequent item sets */
#define ISR_CLOSED    0x0001    /* report only closed  item sets */
#define ISR_MAXIMAL   0x0002    /* report only maximal item sets */
#define ISR_GENERAS   0x0004    /* report only generators */
#define ISR_RULES     0x0008    /* report association rules */

/* --- modes (for isr_settarg()) --- */
#define ISR_NOFILTER  0x0010    /* do not use internal filtering */
#define ISR_NOEXPAND  0x0020    /* do not expand perfect extensions */
#define ISR_SORT      0x0040    /* generator filtering needs sorting */
#define ISR_SEQUENCE  0x0080    /* allow for sequences (repeat items) */

/*----------------------------------------------------------------------
  Type Definitions
----------------------------------------------------------------------*/
struct isreport;                /* --- an item set eval. function --- */
typedef double ISEVALFN (struct isreport *rep, void *data);
typedef void   ISREPOFN (struct isreport *rep, void *data);
typedef void   ISRULEFN (struct isreport *rep, void *data,
                         ITEM item, RSUPP body, RSUPP head);

typedef struct isreport {       /* --- an item set reporter --- */
  ITEMBASE   *base;             /* underlying item base */
  int        target;            /* target type (e.g. ISR_CLOSED) */
  int        mode;              /* reporting mode (e.g. ISR_SORT) */
  ITEM       zmin;              /* minimum number of items in set */
  ITEM       zmax;              /* maximum number of items in set */
  ITEM       xmax;              /* maximum number for isr_xable() */
  ITEM       size;              /* size of the item array "items" */
  RSUPP      smin;              /* minimum support of an item set */
  RSUPP      smax;              /* maximum support of an item set */
  RSUPP      *border;           /* item set filtering border */
  ITEM       bdrcnt;            /* number of used border entries */
  ITEM       bdrsize;           /* size of filtering border */
  ITEM       cnt;               /* current number of items in set */
  ITEM       pfx;               /* number of items in valid prefix */
  ITEM       *pxpp;             /* number of perfect exts. per prefix */
  ITEM       *pexs;             /* perfect extension items */
  ITEM       *items;            /* current item set (array of items) */
  RSUPP      *supps;            /* (prefix) item sets support values */
  double     *wgts;             /* (prefix) item sets weights */
  double     *ldps;             /* binary logarithms of item probs. */
  #ifdef ISR_CLOMAX             /* if closed/maximal sets filter */
  CLOMAX     *clomax;           /* closed/maximal item set filter */
  SYMTAB     *gentab;           /* generator      item set filter */
  #else                         /* if no closed/maximal sets filter */
  void       *clomax;           /* placeholder (for fixed offsets) */
  void       *gentab;           /* dito */
  #endif
  RSUPP      sto;               /* max. superset support for storing */
  int        dir;               /* direction of item order in clomax */
  ITEM       *iset;             /* additional buffer for an item set */
  ISEVALFN   *evalfn;           /* additional evaluation function */
  void       *evaldat;          /* additional evaluation data */
  int        evaldir;           /* direction of evaluation */
  double     evalthh;           /* threshold of evaluation */
  double     eval;              /* additional evaluation value */
  ISREPOFN   *repofn;           /* item set reporting function */
  void       *repodat;          /* item set reporting data */
  ISRULEFN   *rulefn;           /* assoc. rule reporting function */
  void       *ruledat;          /* assoc. rule reporting data */
  int        scan;              /* flag for scanable item output */
  const char *str;              /* buffer for format strings */
  const char *hdr;              /* record header for output */
  const char *sep;              /* item separator for output */
  const char *imp;              /* implication sign for rule output */
  const char *iwf;              /* format for item weight output */
  const char *info;             /* format for information output */
  const char **inames;          /* (formatted) item names */
  size_t     nmax;              /* maximum of the item name sizes */
  size_t     nsum;              /* sum of the item name sizes */
  size_t     repcnt;            /* number of reported item sets */
  size_t     *stats;            /* reported item sets per set size */
  #ifdef ISR_PATSPEC            /* if pattern spectrum support */
  PATSPEC    *psp;              /* an (optional) pattern spectrum */
  #else                         /* if no pattern spectrum support */
  void       *psp;              /* placeholder (for fixed offsets) */
  #endif
  char       **ints;            /* preformatted integer numbers */
  TID        imin;              /* smallest pre-formatted integer */
  TID        imax;              /* largest  pre-formatted integer */
  FILE       *file;             /* output file to write to */
  const char *name;             /* name of item set output file */
  char       *buf;              /* write buffer for output */
  char       *next;             /* next character position to write */
  char       *end;              /* end of the write buffer */
  FILE       *tidfile;          /* output file for transaction ids */
  const char *tidname;          /* name of tid output file */
  char       *tidbuf;           /* write buffer for output */
  char       *tidnxt;           /* next character position to write */
  char       *tidend;           /* end of the write buffer */
  ITEM       *occs;             /* array  of item occurrences */
  TID        *tids;             /* array  of transaction ids */
  TID        tidcnt;            /* number of transaction ids */
  TID        tracnt;            /* total number of transactions */
  ITEM       miscnt;            /* accepted number of missing items */
  int        fast;              /* whether fast output is possible */
  int        fosize;            /* size of set info. for fastout() */
  char       foinfo[64];        /* item set info.    for fastout() */
  char       *out;              /* output buffer for sets/rules */
  char       *pos[1];           /* append positions in output buffer */
} ISREPORT;                     /* (item set reporter) */

/*----------------------------------------------------------------------
  Functions
----------------------------------------------------------------------*/
extern ISREPORT* isr_create   (ITEMBASE *base);
extern ISREPORT* isr_createx  (ITEMBASE *base, ITEM max);
extern int       isr_delete   (ISREPORT *rep, int delis);
extern ITEMBASE* isr_base     (ISREPORT *rep);
extern ITEM      isr_size     (ISREPORT *rep);

extern int       isr_settarg  (ISREPORT *rep,
                               int target, int mode, int dir);
extern int       isr_target   (ISREPORT *rep);
extern int       isr_mode     (ISREPORT *rep);
extern void      isr_setsupp  (ISREPORT *rep, RSUPP smin, RSUPP smax);
extern ITEM      isr_smin     (ISREPORT *rep);
extern ITEM      isr_smax     (ISREPORT *rep);
extern void      isr_setsize  (ISREPORT *rep, ITEM  zmin, ITEM  zmax);
extern ITEM      isr_zmin     (ISREPORT *rep);
extern ITEM      isr_zmax     (ISREPORT *rep);
extern ITEM      isr_xmax     (ISREPORT *rep);

extern RSUPP     isr_setbdr   (ISREPORT *rep, ITEM size, RSUPP supp);
extern RSUPP     isr_getbdr   (ISREPORT *rep, ITEM size);
extern void      isr_clrbdr   (ISREPORT *rep);
extern ITEM      isr_bdrcnt   (ISREPORT *rep);

extern int       isr_setfmt   (ISREPORT *rep, int scan, CCHAR *hdr,
                               CCHAR *sep, CCHAR *imp, CCHAR *info);
extern int       isr_setfmtx  (ISREPORT *rep, int scan, CCHAR *hdr,
                               CCHAR *sep, CCHAR *imp, CCHAR *info,
                               CCHAR *iwf);
extern CCHAR*    isr_hdr      (ISREPORT *rep);
extern CCHAR*    isr_sep      (ISREPORT *rep);
extern CCHAR*    isr_imp      (ISREPORT *rep);
extern CCHAR*    isr_info     (ISREPORT *rep);
extern CCHAR*    isr_iwf      (ISREPORT *rep);

extern int       isr_setsmt   (ISREPORT *rep, RSUPP supp);
extern int       isr_setwgt   (ISREPORT *rep, double wgt);
extern void      isr_seteval  (ISREPORT *rep, ISEVALFN evalfn,
                               void *data, int dir, double thresh);
extern void      isr_setrepo  (ISREPORT *rep, ISREPOFN repofn,
                               void *data);
extern void      isr_setrule  (ISREPORT *rep, ISRULEFN rulefn,
                               void *data);
extern int       isr_prefmt   (ISREPORT *rep, TID min, TID max);

extern int       isr_open     (ISREPORT *rep, FILE *file, CCHAR *name);
extern int       isr_close    (ISREPORT *rep);
extern FILE*     isr_file     (ISREPORT *rep);
extern CCHAR*    isr_name     (ISREPORT *rep);

extern int       isr_tidopen  (ISREPORT *rep, FILE *file, CCHAR *name);
extern int       isr_tidclose (ISREPORT *rep);
extern void       isr_tidflush (ISREPORT *rep);
extern void      isr_tidcfg   (ISREPORT *rep, TID tracnt, ITEM miscnt);
extern FILE*     isr_tidfile  (ISREPORT *rep);
extern CCHAR*    isr_tidname  (ISREPORT *rep);

extern int       isr_setup    (ISREPORT *rep);

extern int       isr_add      (ISREPORT *rep, ITEM item, RSUPP supp);
extern int       isr_addnc    (ISREPORT *rep, ITEM item, RSUPP supp);
extern int       isr_addwgt   (ISREPORT *rep, ITEM item, RSUPP supp,
                               double wgt);
extern int       isr_addpex   (ISREPORT *rep, ITEM item);
extern void      isr_addpexpk (ISREPORT *rep, ITEM bits);
extern int       isr_uses     (ISREPORT *rep, ITEM item);
extern void      isr_remove   (ISREPORT *rep, ITEM n);
extern int       isr_xable    (ISREPORT *rep, ITEM n);

extern ITEM      isr_cnt      (ISREPORT *rep);
extern ITEM      isr_item     (ISREPORT *rep);
extern ITEM      isr_itemx    (ISREPORT *rep, ITEM index);
extern const ITEM* isr_items  (ISREPORT *rep);
extern RSUPP     isr_supp     (ISREPORT *rep);
extern RSUPP     isr_suppx    (ISREPORT *rep, ITEM index);
extern double    isr_wgt      (ISREPORT *rep);
extern double    isr_wgtx     (ISREPORT *rep, ITEM index);
extern double    isr_eval     (ISREPORT *rep);

extern ITEM      isr_pexcnt   (ISREPORT *rep);
extern ITEM      isr_pex      (ISREPORT *rep, ITEM index);
extern const int*isr_pexs     (ISREPORT *rep);
extern ITEM      isr_all      (ISREPORT *rep);
extern ITEM      isr_lack     (ISREPORT *rep);

extern double    isr_logrto   (ISREPORT *rep, void *data);
extern double    isr_lrsize   (ISREPORT *rep, void *data);
extern double    isr_sizewgt  (ISREPORT *rep, void *data);
extern double    isr_wgtsize  (ISREPORT *rep, void *data);
extern double    isr_wgtsupp  (ISREPORT *rep, void *data);

extern CCHAR*    isr_itemname (ISREPORT *rep, ITEM item);
extern void*     isr_itemobj  (ISREPORT *rep, ITEM item);
extern int       isr_report   (ISREPORT *rep);
extern int       isr_reportv  (ISREPORT *rep, double eval);
extern int       isr_reportx  (ISREPORT *rep, TID  *tids, TID n);
extern int       isr_reporto  (ISREPORT *rep, ITEM *occs, TID n);
extern int       isr_reprule  (ISREPORT *rep, ITEM item,
                               RSUPP body, RSUPP head, double eval);
extern int       isr_iset     (ISREPORT *rep, const ITEM *items,ITEM n,
                               RSUPP supp, double wgt, double eval);
extern int       isr_isetx    (ISREPORT *rep, const ITEM *items,ITEM n,
                               const double *iwgts,
                               RSUPP supp, double wgt, double eval);
extern int       isr_rule     (ISREPORT *rep, const ITEM *items,ITEM n,
                               RSUPP supp, RSUPP body, RSUPP head,
                               double eval);
extern int       isr_seqrule  (ISREPORT *rep, const ITEM *items,ITEM n,
                               RSUPP supp, RSUPP body, RSUPP head,
                               double eval);
extern int       isr_sxrule   (ISREPORT *rep, const ITEM *ante, ITEM n,
                               ITEM cons, RSUPP supp, RSUPP body,
                               RSUPP head, double eval);
extern int       isr_extrule  (ISREPORT *rep, const ITEM *items, ITEM n,
                               ITEM a, ITEM b, RSUPP body,
                               RSUPP supp, RSUPP head,
                               RSUPP salt, RSUPP halt, RSUPP join);

extern void      isr_reset    (ISREPORT *rep);
extern size_t    isr_repcnt   (ISREPORT *rep);
extern const size_t* isr_stats(ISREPORT *rep);
extern void      isr_prstats  (ISREPORT *rep, FILE *out, ITEM min);
#ifdef ISR_PATSPEC
extern int       isr_addpsp   (ISREPORT *rep, PATSPEC *psp);
extern PATSPEC*  isr_rempsp   (ISREPORT *rep, int delpsp);
extern PATSPEC*  isr_getpsp   (ISREPORT *rep);
#endif
extern int       isr_intout   (ISREPORT *rep, diff_t num);
extern int       isr_numout   (ISREPORT *rep, double num, int digits);
extern int       isr_wgtout   (ISREPORT *rep, RSUPP supp, double wgt);
extern int       isr_sinfo    (ISREPORT *rep, RSUPP supp, double wgt,
                               double eval);
extern int       isr_rinfo    (ISREPORT *rep, RSUPP supp,
                               RSUPP body, RSUPP head, double eval);
extern int       isr_xinfo    (ISREPORT *rep, RSUPP supp,
                               RSUPP body, RSUPP head,
                               RSUPP salt, RSUPP halt, RSUPP join);
extern void      isr_getinfo  (ISREPORT *rep, const char *sel,
                               double *vals);
#ifdef ISR_CLOMAX
extern ITEM*     isr_buf      (ISREPORT *rep);
extern int       isr_tail     (ISREPORT *rep, const ITEM *items,ITEM n);
#endif

/*----------------------------------------------------------------------
  Preprocessor Definitions
----------------------------------------------------------------------*/
#define isr_create(b)     isr_createx(b,0)
#define isr_base(r)       ((r)->base)
#define isr_size(r)       ((r)->size)
#define isr_target(r)     ((r)->target)
#define isr_mode(r)       ((r)->mode)
#define isr_smin(r)       ((r)->smin)
#define isr_smax(r)       ((r)->smax)
#define isr_zmin(r)       ((r)->zmin)
#define isr_zmax(r)       ((r)->zmax)
#define isr_xmax(r)       ((r)->xmax)
#define isr_getbdr(r,s)   ((r)->border[s])
#define isr_bdrcnt(r)     ((r)->bdrcnt)

#define isr_setsmt(r,s)   ((r)->supps[0] = (s))
#define isr_setwgt(r,w)   ((r)->wgts[0]  = (w))

#define isr_setfmt(r,g,h,s,i,n)  isr_setfmtx(r,g,h,s,i,n,NULL)
#define isr_hdr(r)        ((r)->hdr)
#define isr_sep(r)        ((r)->sep)
#define isr_imp(r)        ((r)->imp)
#define isr_info(r)       ((r)->format)
#define isr_iwf(r)        ((r)->iwf)

#define isr_file(r)       ((r)->file)
#define isr_name(r)       ((r)->name)
#define isr_tidfile(r)    ((r)->tidfile)
#define isr_tidname(r)    ((r)->tidname)

#define isr_uses(r,i)     ((r)->pxpp[i] < 0)
#define isr_xable(r,n)    ((r)->cnt+(n) <= (r)->xmax)

#define isr_cnt(r)        ((r)->cnt)
#define isr_item(r)       ((r)->items[(r)->cnt -1])
#define isr_itemx(r,i)    ((r)->items[i])
#define isr_items(r)      ((const ITEM*)(r)->items)
#define isr_supp(r)       ((r)->supps[(r)->cnt])
#define isr_suppx(r,i)    ((r)->supps[i])
#define isr_wgt(r)        ((r)->wgts [(r)->cnt])
#define isr_wgtx(r,i)     ((r)->wgts [i])
#define isr_eval(r)       ((r)->eval)

#define isr_pexcnt(r)     ((ITEM)((r)->items -(r)->pexs))
#define isr_pex(r,t)      ((r)->pexs [i])
#define isr_pexs(r)       ((const ITEM*)(r)->pexs)
#define isr_all(r)        ((r)->cnt +(ITEM)((r)->items -(r)->pexs))
#define isr_lack(r)       ((r)->zmin -isr_all(r))

#define isr_itemname(r,i) ((r)->inames[i])
#define isr_itemobj(r,i)  ib_obj((r)->base, i)
#define isr_basename(r,i) ib_name((r)->base, i)

#define isr_repcnt(r)     ((r)->repcnt)
#define isr_stats(r)      ((const size_t*)(r)->stats)
#ifdef ISR_PATSPEC
#define isr_getpsp(r)     ((r)->psp)
#endif
#ifdef ISR_CLOMAX
#define isr_buf(r)        ((r)->iset)
#define isr_tail(r,i,n)   (cm_tail((r)->clomax, i, n) > 0 ? 1 : 0)
#endif

#endif
