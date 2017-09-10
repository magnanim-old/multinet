/*----------------------------------------------------------------------
  File    : tract.h
  Contents: item and transaction management
  Author  : Christian Borgelt
  History : 2001.11.18 file created from file apriori.c
            2001.12.28 first version completed
            2001.01.02 function t_sort() mapped to function int_qsort()
            2002.02.19 transaction tree functions added (tat_...)
            2003.07.17 function tbg_filter() added (remove unused items)
            2003.08.21 parameter 'heap' added to function tbg_sort()
            2003.09.12 function tbg_wgt() added (total trans. weight)
            2003.09.20 empty transactions in input made possible
            2004.12.11 access functions for extended frequency added
            2004.12.15 function ib_trunc added (remove irrelevant items)
            2006.11.26 item set formatter and evaluator added
            2007.02.13 adapted to modified module tabread
            2008.08.12 considerable redesign, transaction weight added
            2008.08.14 function tbg_filter() extended (minimal size)
            2008.10.13 functions t_reverse and tbg_reverse() added
            2008.11.19 transaction tree and tree node separated
            2009.05.28 bug in function tbg_filter() fixed (minimal size)
            2009.08.27 fixed prototypes of trans. tree node functions
            2010.03.16 handling of extended transactions added
            2010.06.23 function tbg_extent() added (total item insts.)
            2010.07.02 transaction size comparison functions added
            2010.08.05 function tbg_itemcnt() added for convenience
            2010.08.10 function tbg_trim() added (for sequences)
            2010.08.11 parameter of ib_read() changed to general mode
            2010.08.13 function tbg_addib() added (add from item base)
            2010.08.19 function ib_readsel() added (item selectors)
            2010.08.22 adapted to modified module tabread
            2010.09.13 functions tbg_reverse() and tbg_mirror() added
            2010.10.25 parameter max added to function ib_recode()
            2010.12.15 functions tbg_read() added (read database)
            2010.12.20 functions tbg_icnts() and tbg_ifrqs() added
            2011.05.06 parameter wgt added to tbg_filter(), tbg_trim()
            2011.07.09 interface for transaction bag recoding modified
            2011.07.12 adapted to modified symbol table/idmap interface
            2011.07.18 alternative transaction tree implementation added
            2012.03.22 function ta_cmplim() added (limited comparison)
            2012.05.25 function taa_reduce() added (for occ. deliver)
            2012.06.15 function tbg_packcnt() added (for packed items)
            2012.07.21 function tbg_ipwgt() added (idempotent weights)
            2012.07.23 functions ib_write() and tbg_write() added
            2012.07.30 parameter keep0 added to function tbg_reduce()
            2013.02.04 ib_getapp()/ib_setapp() made proper functions
            2013.02.11 arbitrary objects/pointers allowed as item names
            2013.03.07 direction parameter added to sorting functions
            2013.03.25 definitions for data types TID and SUPP added
            2013.03.28 definitions for data type ITEM added
            2013.11.07 transaction reading capability made optional
            2014.05.02 surrogate data generation functions added
            2014.09.08 transaction marker functions added (ta_..mark())
            2014.09.09 function ib_frqcnt() added (num. of freq. items)
            2014.10.17 function ib_clear() made a proper function
----------------------------------------------------------------------*/

#define TA_READ // Added by MM
#define SIZE_FMT  "zu"  // Added by MM
#define ITEM_FMT  "d"  // Added by MM

#ifndef __TRACT__
#define __TRACT__
#include <math.h>
#include "arrays.h"
#ifdef TA_SURR
#include "random.h"
#endif
#ifndef IDMAPFN
#define IDMAPFN
#endif
#ifdef OCTAVE
#  ifdef _WIN32
#  include "..\..\util\src\symtab.h"
#  else
#  include "../../util/src/symtab.h"
#  endif
/* Annoyingly, the Octave include directory also contains a file */
/* symtab.h, which clashes with the one needed for this module.  */
#else
#include "symtab.h"
#endif
#ifdef TA_READ
#include "tabread.h"
#endif
#ifdef TA_WRITE
#include "tabwrite.h"
#endif

#ifndef INFINITY                /* if C99 is not (fully) supported */
#include <float.h>
#define INFINITY    (DBL_MAX+DBL_MAX)
#endif                          /* define missing INFINITY constant */

#ifdef _MSC_VER
#ifndef strtoll                 /* if C99 is not (fully) supported */
#define strtoll     _strtoi64   /* convert string to long long */
#endif                          /* define missing conversion function */
#endif

/*----------------------------------------------------------------------
  Preprocessor Definitions
----------------------------------------------------------------------*/
#ifndef ITEM
#define ITEM        IDENT       /* item identifier type */
#endif

#ifndef TID
#define TID         int         /* transaction identifier type */
#endif

#ifndef SUPP
#define SUPP        TID         /* support type */
#endif

/*--------------------------------------------------------------------*/

#define int         1           /* to check definitions */
#define long        2           /* for certain types */
#define ptrdiff_t   3
#define double      4

/*--------------------------------------------------------------------*/

#if   ITEM==int
#ifndef UITEM
#define UITEM       unsigned int
#endif
#ifndef ITEM_MIN
#define ITEM_MIN    INT_MIN     /* minimum item identifier */
#endif
#ifndef ITEM_MAX
#define ITEM_MAX    INT_MAX     /* maximum item identifier */
#endif
#ifndef ITEM_FMT
#define ITEM_FMT    "d"         /* printf format code for int */
#endif
#ifndef ia_qsort
#define ia_qsort    int_qsort
#endif
#ifndef ia_heapsort
#define ia_heapsort int_heapsort
#endif
#ifndef ia_reverse
#define ia_reverse  int_reverse
#endif
#ifndef ia_unique
#define ia_unique   (ITEM)int_unique
#endif
#ifndef ia_bsearch
#define ia_bsearch  (ITEM)int_bsearch
#endif
#ifndef ia_bisect
#define ia_bisect   (ITEM)int_bisect
#endif

#elif ITEM==long
#ifndef UITEM
#define UITEM       unsigned long
#endif
#ifndef ITEM_MIN
#define ITEM_MIN    LONG_MIN    /* minimum item identifier */
#endif
#ifndef ITEM_MAX
#define ITEM_MAX    LONG_MAX    /* maximum item identifier */
#endif
#ifndef ITEM_FMT
#define ITEM_FMT    "ld"        /* printf format code for long */
#endif
#ifndef ia_qsort
#define ia_qsort    lng_qsort
#endif
#ifndef ia_heapsort
#define ia_heapsort lng_heapsort
#endif
#ifndef ia_reverse
#define ia_reverse  lng_reverse
#endif
#ifndef ia_unique
#define ia_unique   (ITEM)lng_unique
#endif
#ifndef ia_bsearch
#define ia_bsearch  (ITEM)lng_bsearch
#endif
#ifndef ia_bisect
#define ia_bisect   (ITEM)lng_bisect
#endif

#elif ITEM==ptrdiff_t
#ifndef UITEM
#define UITEM       size_t
#endif
#ifndef ITEM_MIN
#define ITEM_MIN    PTRDIFF_MIN /* minimum item identifier */
#endif
#ifndef ITEM_MAX
#define ITEM_MAX    PTRDIFF_MAX /* maximum item identifier */
#endif
#  ifdef _MSC_VER
#  define ITEM_FMT  "Id"        /* printf format code for ptrdiff_t */
#  else
#  define ITEM_FMT  "zd"        /* printf format code for ptrdiff_t */
#  endif                        /* MSC still does not support C99 */
#ifndef ia_qsort
#define ia_qsort    dif_qsort
#endif
#ifndef ia_heapsort
#define ia_heapsort dif_heapsort
#endif
#ifndef ia_reverse
#define ia_reverse  dif_reverse
#endif
#ifndef ia_unique
#define ia_unique   (ITEM)dif_unique
#endif
#ifndef ia_bsearch
#define ia_bsearch  (ITEM)dif_bsearch
#endif
#ifndef ia_bisect
#define ia_bisect   (ITEM)dif_bisect
#endif

#else
#error "ITEM must be either 'int', 'long' or 'ptrdiff_t'"
#endif

/*--------------------------------------------------------------------*/

#if   TID==int
#ifndef TID_MAX
#define TID_MAX     INT_MAX     /* maximum transaction identifier */
#endif
#ifndef TID_FMT
#define TID_FMT     "d"         /* printf format code for int */
#endif

#elif TID==long
#ifndef TID_MAX
#define TID_MAX     LONG_MAX    /* maximum transaction identifier */
#endif
#ifndef TID_FMT
#define TID_FMT     "ld"        /* printf format code for long */
#endif

#elif TID==ptrdiff_t
#ifndef TID_MAX
#define TID_MAX     PTRDIFF_MAX /* maximum transaction identifier */
#endif
#  ifdef _MSC_VER
#  define TID_FMT   "Id"        /* printf format code for ptrdiff_t */
#  else
#  define TID_FMT   "zd"        /* printf format code for ptrdiff_t */
#  endif                        /* MSC still does not support C99 */

#else
#error "TID must be either 'int', 'long' or 'ptrdiff_t'"
#endif

/*--------------------------------------------------------------------*/

#if   SUPP==int
#ifndef SUPP_MIN
#define SUPP_MIN    INT_MIN     /* minimum support value */
#endif
#ifndef SUPP_MAX
#define SUPP_MAX    INT_MAX     /* maximum support value */
#endif
#ifndef SUPP_EPS
#define SUPP_EPS    1           /* minimum support step */
#endif
#ifndef SUPP_FMT
#define SUPP_FMT    "d"         /* printf format code for int */
#endif
#ifndef strtosupp
#define strtosupp(s,p)  (int)strtol(s,p,0)
#endif
#ifndef ceilsupp
#define ceilsupp(s)     ceil(s)
#endif
#ifndef floorsupp
#define floorsupp(s)    floor(s)
#endif
#ifndef i2s_sort
#  if   ITEM==int
#  define i2s_sort      i2i_qsort
#  elif ITEM==long
#  define i2s_sort      l2i_qsort
#  elif ITEM==ptrdiff_t
#  define i2s_sort      x2i_qsort
#  else
#  error "ITEM must be either 'int', 'long' or 'ptrdiff_t'"
#  endif
#endif

#elif SUPP==long
#ifndef SUPP_MIN
#define SUPP_MIN    LONG_MIN    /* minimum support value */
#endif
#ifndef SUPP_MAX
#define SUPP_MAX    LONG_MAX    /* maximum support value */
#endif
#ifndef SUPP_EPS
#define SUPP_EPS    1           /* minimum support step */
#endif
#ifndef SUPP_FMT
#define SUPP_FMT    "ld"        /* printf format code for long */
#endif
#ifndef strtosupp
#define strtosupp(s,p)  strtol(s,p,0)
#endif
#ifndef ceilsupp
#define ceilsupp(s)     ceil(s)
#endif
#ifndef floorsupp
#define floorsupp(s)    floor(s)
#endif
#ifndef i2s_sort
#  if   ITEM==int
#  define i2s_sort      i2l_qsort
#  elif ITEM==long
#  define i2s_sort      l2l_qsort
#  elif ITEM==ptrdiff_t
#  define i2s_sort      x2l_qsort
#  else
#  error "ITEM must be either 'int', 'long' or 'ptrdiff_t'"
#  endif
#endif

#elif SUPP==ptrdiff_t
#ifndef SUPP_MIN
#define SUPP_MIN    PTRDIFF_MIN /* minimum support value */
#endif
#ifndef SUPP_MAX
#define SUPP_MAX    PTRDIFF_MAX /* maximum support value */
#endif
#ifndef SUPP_EPS
#define SUPP_EPS    1           /* minimum support step */
#endif
#ifndef SUPP_FMT
#  ifdef _MSC_VER
#  define SUPP_FMT  "Id"        /* printf format code for ptrdiff_t */
#  else
#  define SUPP_FMT  "td"        /* printf format code for ptrdiff_t */
#  endif                        /* MSC still does not support C99 */
#endif
#ifndef strtosupp
#define strtosupp(s,p)  (ptrdiff_t)strtoll(s,p,0)
#endif
#ifndef ceilsupp
#define ceilsupp(s)     ceil(s)
#endif
#ifndef floorsupp
#define floorsupp(s)    floor(s)
#endif
#ifndef i2s_sort
#  if   ITEM==int
#  define i2s_sort      i2x_qsort
#  elif ITEM==long
#  define i2s_sort      l2x_qsort
#  elif ITEM==ptrdiff_t
#  define i2s_sort      x2x_qsort
#  else
#  error "ITEM must be either 'int', 'long' or 'ptrdiff_t'"
#  endif
#endif

#elif SUPP==double
#ifndef SUPP_MIN
#define SUPP_MIN    (-INFINITY) /* minimum support value */
#endif
#ifndef SUPP_MAX
#define SUPP_MAX    INFINITY    /* maximum support value */
#endif
#ifndef SUPP_EPS
#define SUPP_EPS    0.0         /* minimum support step */
#endif
#ifndef SUPP_FMT
#define SUPP_FMT    "g"         /* printf format code for double */
#endif
#ifndef strtosupp
#define strtosupp(s,p)  strtod(s,p)
#endif
#ifndef ceilsupp
#define ceilsupp(s)     (s)
#endif
#ifndef floorsupp
#define floorsupp(s)    (s)
#endif
#  ifndef i2s_sort
#  if   ITEM==int
#  define i2s_sort      i2d_qsort
#  elif ITEM==long
#  define i2s_sort      l2d_qsort
#  elif ITEM==ptrdiff_t
#  define i2s_sort      x2d_qsort
#  else
#  error "ITEM must be either 'int', 'long' or 'ptrdiff_t'"
#  endif
#endif

#else
#error "SUPP must be either 'int', 'long', 'ptrdiff_t' or 'double'"
#endif

/*--------------------------------------------------------------------*/

#undef int                      /* remove preprocessor definitions */
#undef long                     /* needed for the type checking */
#undef ptrdiff_t
#undef double

/*--------------------------------------------------------------------*/

#ifndef SIZE_FMT
#  ifdef _MSC_VER
#  define SIZE_FMT  "Iu"        /* printf format code for size_t */
#  else
#  define SIZE_FMT  "zu"        /* printf format code for size_t */
#  endif                        /* MSC still does not support C99 */
#endif

/*--------------------------------------------------------------------*/

/* --- item appearance flags --- */
#define APP_NONE    0x00        /* item should be ignored */
#define APP_BODY    0x01        /* item may appear in rule body */
#define APP_HEAD    0x02        /* item may appear in rule head */
#define APP_BOTH    (APP_HEAD|APP_BODY)  /* item may apper in both */

/* --- item base/transaction bag modes --- */
#define IB_WEIGHTS  0x20        /* items have t.a.-specific weights */
#define IB_OBJNAMES 0x40        /* item names are arbitrary objects */

/* --- transaction sentinel --- */
#define TA_END      ITEM_MIN    /* sentinel for item instance arrays */

/* --- transaction modes --- */
#define TA_PACKED   0x1f        /* transactions have been packed */
#define TA_EQPACK   0x20        /* treat packed items all the same */
#define TA_HEAP     0x40        /* prefer heap sort to quicksort */

/* --- transaction read/write modes --- */
#define TA_WEIGHT   0x01        /* integer weight in last field */
#define TA_DUPLICS  0x02        /* allow duplicates of items */
#define TA_DUPERR   0x04        /* consider duplicates as errors */
#define TA_TERM     0x10        /* terminate all trans. with item 0 */
#define TA_WGTSEP   TRD_OTHER   /* item weight separator */
#define TA_PAREN    0x02        /* print parentheses around weight */

/* --- idempotent weight modes --- */
#define TA_NOGAPS   0x40        /* do not allow gaps in matching */
#define TA_ALLOCC   0x80        /* consider all occurrences */

/* --- error codes --- */
#define E_NONE         0        /* no error */
#define E_NOMEM      (-1)       /* not enough memory */
#define E_FOPEN      (-2)       /* cannot open file */
#define E_FREAD      (-3)       /* read error on file */
#define E_FWRITE     (-4)       /* write error on file */

#define E_NOITEMS   (-15)       /* no frequent items found */
#define E_ITEMEXP   (-16)       /* item expected */
#define E_ITEMWGT   (-17)       /* invalid item weight */
#define E_DUPITEM   (-18)       /* duplicate item */
#define E_INVITEM   (-19)       /* invalid item (not integer) */
#define E_WGTEXP    (-20)       /* transaction weight expected */
#define E_TAWGT     (-21)       /* invalid transaction weight */
#define E_FLDCNT    (-22)       /* too many fields */
#define E_APPEXP    (-23)       /* appearance indicator expected */
#define E_UNKAPP    (-24)       /* unknown appearance indicator */
#define E_PENEXP    (-25)       /* insertion penalty expected */
#define E_PENALTY   (-26)       /* invalid insertion penalty */

/* --- special macros --- */
#define ispacked(i) (((i) ^ TA_END) > 0)

/*----------------------------------------------------------------------
  Type Definitions
----------------------------------------------------------------------*/
typedef struct {                /* --- item data --- */
  ITEM     id;                  /* item identifier */
  int      app;                 /* appearance indicator */
  double   pen;                 /* insertion penalty */
  SUPP     frq;                 /* standard frequency (trans. weight) */
  SUPP     xfq;                 /* extended frequency (trans. sizes) */
  TID      idx;                 /* index of counted last transaction */
} ITEMDATA;                     /* (item data) */

typedef struct {                /* --- item base --- */
  IDMAP    *idmap;              /* key/name to identifier map */
  SUPP     wgt;                 /* total weight of transactions */
  SUPP     max;                 /* maximum support of an item */
  int      mode;                /* mode (IB_WEIGHTS, IB_OBJNAMES) */
  int      app;                 /* default appearance indicator */
  double   pen;                 /* default insertion penalty */
  TID      idx;                 /* index of current transaction */
  ITEM     size;                /* size of the transaction buffer */
  void     *tract;              /* buffer for a transaction */
  int      err;                 /* error code (file reading) */
  #ifdef TA_READ                /* if transaction reading capability */
  TABREAD  *trd;                /* table/transaction reader */
  #else                         /* if no transaction reading */
  void     *trd;                /* placeholder (for fixed size) */
  #endif
} ITEMBASE;                     /* (item base) */

typedef struct {                /* --- transaction --- */
  SUPP     wgt;                 /* weight (number of occurrences) */
  ITEM     size;                /* size   (number of items) */
  TID      mark;                /* mark   (e.g. bit coded items) */
  ITEM     items[1];            /* items in the transaction */
} TRACT;                        /* (transaction) */

typedef struct {                /* --- weighted item instance --- */
  ITEM     item;                /* item identifier */
  float    wgt;                 /* item weight (transaction-specific) */
} WITEM;                        /* (weighted item instance) */

typedef struct {                /* --- trans. with weighted items --- */
  SUPP     wgt;                 /* weight (number of occurrences) */
  ITEM     size;                /* size   (number of items) */
  int      mark;                /* mark   (e.g. bit coded items) */
  WITEM    items[1];            /* items in the transaction */
} WTRACT;                       /* (transaction with weighted items) */

typedef struct {                /* --- transaction bag/multiset --- */
  ITEMBASE *base;               /* underlying item base */
  int      mode;                /* mode (IB_OBJNAMES, IB_WEIGHT) */
  ITEM     max;                 /* number of items in largest trans. */
  SUPP     wgt;                 /* total weight of transactions */
  size_t   extent;              /* total number of item instances */
  TID      size;                /* size of the transaction array */
  TID      cnt;                 /* number of transactions */
  void     **tracts;            /* array  of transactions */
  TID      *icnts;              /* number of transactions per item */
  SUPP     *ifrqs;              /* frequency of the items (weight) */
  void     *buf;                /* buffer for surrogate generation */
} TABAG;                        /* (transaction bag/multiset) */

#ifdef TATREEFN
#ifdef TATCOMPACT

typedef struct {                /* --- transaction tree node --- */
  ITEM     item;                /* next item in transaction */
  ITEM     max;                 /* number of items in largest trans. */
  SUPP     wgt;                 /* weight of trans. with this prefix */
  void     *data;               /* data depending on node type */
} TANODE;                       /* (transaction tree node) */

typedef struct {                /* --- transaction tree --- */
  TABAG    *bag;                /* underlying transaction bag */
  TANODE   root;                /* root of the transaction tree */
  ITEM     suffix[1];           /* empty transaction suffix */
} TATREE;                       /* (transaction tree) */

#else

typedef struct {                /* --- transaction tree node --- */
  SUPP     wgt;                 /* weight (number of transactions) */
  ITEM     max;                 /* number of items in largest trans. */
  ITEM     size;                /* node size (number of children) */
  ITEM     items[1];            /* next items in rep. transactions */
} TANODE;                       /* (transaction tree node) */

typedef struct {                /* --- transaction tree --- */
  TABAG    *bag;                /* underlying transaction bag */
  TANODE   *root;               /* root of the transaction tree */
  TANODE   empty;               /* empty transaction node */
} TATREE;                       /* (transaction tree) */

#endif
#endif

#ifdef TA_SURR
typedef TABAG* TBGSURRFN (TABAG *src, RNG *rng, TABAG *dst);
#endif
/*----------------------------------------------------------------------
  Item Base Functions
----------------------------------------------------------------------*/
extern ITEMBASE*    ib_create   (int mode, ITEM size, ...);
extern void         ib_delete   (ITEMBASE *base);

extern int          ib_mode     (ITEMBASE *base);
extern ITEM         ib_cnt      (ITEMBASE *base);
extern ITEM         ib_add      (ITEMBASE *base, const void *name);
extern ITEM         ib_item     (ITEMBASE *base, const void *name);
extern const char*  ib_name     (ITEMBASE *base, ITEM item);
extern const void*  ib_key      (ITEMBASE *base, ITEM item);
extern const void*  ib_obj      (ITEMBASE *base, ITEM item);
extern const char*  ib_xname    (ITEMBASE *base, ITEM item);
extern void         ib_clear    (ITEMBASE *base);
extern ITEM         ib_add2ta   (ITEMBASE *base, const void *name);
extern void         ib_finta    (ITEMBASE *base, SUPP wgt);

extern SUPP         ib_getwgt   (ITEMBASE *base);
extern SUPP         ib_setwgt   (ITEMBASE *base, SUPP wgt);
extern SUPP         ib_incwgt   (ITEMBASE *base, SUPP wgt);

extern int          ib_getapp   (ITEMBASE *base, ITEM item);
extern int          ib_setapp   (ITEMBASE *base, ITEM item, int app);
extern SUPP         ib_getfrq   (ITEMBASE *base, ITEM item);
extern SUPP         ib_setfrq   (ITEMBASE *base, ITEM item, SUPP frq);
extern SUPP         ib_incfrq   (ITEMBASE *base, ITEM item, SUPP frq);
extern SUPP         ib_getxfq   (ITEMBASE *base, ITEM item);
extern SUPP         ib_setxfq   (ITEMBASE *base, ITEM item, SUPP xfq);
extern SUPP         ib_incxfq   (ITEMBASE *base, ITEM item, SUPP xfq);
extern double       ib_getpen   (ITEMBASE *base, ITEM item);
extern double       ib_setpen   (ITEMBASE *base, ITEM item, double pen);
extern SUPP         ib_maxfrq   (ITEMBASE *base);
extern ITEM         ib_frqcnt   (ITEMBASE *base, SUPP smin);

#ifdef TA_READ
extern int          ib_readsel  (ITEMBASE *base, TABREAD *trd);
extern int          ib_readapp  (ITEMBASE *base, TABREAD *trd);
extern int          ib_readpen  (ITEMBASE *base, TABREAD *trd);
extern int          ib_read     (ITEMBASE *base, TABREAD *trd,int mode);
extern const char*  ib_errmsg   (ITEMBASE *base, char *buf,size_t size);
#endif
#ifdef TA_WRITE
extern int          ib_write    (ITEMBASE *base, TABWRITE *twr,
                                 const char *wgtfmt, ...);
#endif

extern ITEM         ib_recode   (ITEMBASE *base, SUPP min, SUPP max,
                                 ITEM cnt, int dir, ITEM *map);
extern void         ib_trunc    (ITEMBASE *base, ITEM n);

extern TRACT*       ib_tract    (ITEMBASE *base);
extern WTRACT*      ib_wtract   (ITEMBASE *base);
#ifndef NDEBUG
extern void         ib_show     (ITEMBASE *base);
#endif

/*----------------------------------------------------------------------
  Transaction Functions
----------------------------------------------------------------------*/
extern TRACT*       ta_create   (const ITEM *items, ITEM n, SUPP wgt);
extern void         ta_delete   (TRACT *t);
extern TRACT*       ta_clone    (const TRACT *t);
extern TRACT*       ta_copy     (TRACT *dst, const TRACT *src);

extern const ITEM*  ta_items    (const TRACT *t);
extern ITEM         ta_size     (const TRACT *t);
extern SUPP         ta_wgt      (const TRACT *t);
extern int          ta_setmark  (TRACT *t, int mark);
extern int          ta_getmark  (const TRACT *t);
extern int          ta_bitmark  (TRACT *t);

extern void         ta_sort     (TRACT *t, int dir);
extern void         ta_reverse  (TRACT *t);
extern ITEM         ta_unique   (TRACT *t);
extern ITEM         ta_pack     (TRACT *t, int n);
extern ITEM         ta_unpack   (TRACT *t, int dir);

extern int          ta_equal    (const TRACT *t1, const TRACT *t2);
extern int          ta_cmp      (const void *p1,
                                 const void *p2, void *data);
extern int          ta_cmpep    (const void *p1,
                                 const void *p2, void *data);
extern int          ta_cmpoff   (const void *p1,
                                 const void *p2, void *data);
extern int          ta_cmplim   (const void *p1,
                                 const void *p2, void *data);
extern int          ta_cmpsfx   (const void *p1,
                                 const void *p2, void *data);
extern int          ta_cmpx     (const TRACT *t,
                                 const ITEM *items, ITEM n);
extern int          ta_cmpsz    (const void *p1,
                                 const void *p2, void *data);
extern ITEM         ta_subset   (const TRACT *t1,
                                 const TRACT *t2, ITEM off);
extern ITEM         ta_subwog   (const TRACT *t1,
                                 const TRACT *t2, ITEM off);
#ifdef TA_WRITE
extern int          ta_write    (const TRACT *t, const ITEMBASE *base,
                                 TABWRITE *twr, const char *wgtfmt);
#endif
#ifndef NDEBUG
extern void         ta_show     (TRACT *t, ITEMBASE *base);
#endif

/*----------------------------------------------------------------------
  Weighted Item Instance Functions
----------------------------------------------------------------------*/
extern int          wi_cmp      (const WITEM *a, const WITEM *b);
extern int          wi_cmpw     (const WITEM *a, const WITEM *b);
extern void         wi_sort     (WITEM *wia, ITEM n, int dir);
extern void         wi_reverse  (WITEM *wia, ITEM n);
extern ITEM         wi_unique   (WITEM *wia, ITEM n);

/*----------------------------------------------------------------------
  Extended Transaction Functions
----------------------------------------------------------------------*/
extern WTRACT*      wta_create  (ITEM size, SUPP wgt);
extern void         wta_delete  (WTRACT *t);
extern WTRACT*      wta_clone   (const WTRACT *t);
extern WTRACT*      wta_copy    (WTRACT *dst, const WTRACT *src);

extern void         wta_add     (WTRACT *t, ITEM item, float wgt);
extern const WITEM* wta_items   (const WTRACT *t);
extern ITEM         wta_size    (const WTRACT *t);
extern SUPP         wta_wgt     (const WTRACT *t);

extern void         wta_sort    (WTRACT *t, int dir);
extern void         wta_reverse (WTRACT *t);
extern ITEM         wta_unique  (WTRACT *t);

extern int          wta_cmp     (const void *p1,
                                 const void *p2, void *data);
extern int          wta_cmpsz   (const void *p1,
                                 const void *p2, void *data);
extern ITEM         wta_subset  (const WTRACT *t1,
                                 const WTRACT *t2, ITEM off);
extern ITEM         wta_subwog  (const WTRACT *t1,
                                 const WTRACT *t2, ITEM off);
#ifdef TA_WRITE
extern int          wta_write   (const WTRACT *t, const ITEMBASE *base,
                                 TABWRITE *twr, const char *wgtfmt,
                                 const char *iwfmt);
#endif
#ifndef NDEBUG
extern void         wta_show    (WTRACT *t, ITEMBASE *base);
#endif

/*----------------------------------------------------------------------
  Transaction Bag/Multiset Functions
----------------------------------------------------------------------*/
extern TABAG*       tbg_create  (ITEMBASE *base);
extern void         tbg_delete  (TABAG *bag, int delib);
extern ITEMBASE*    tbg_base    (TABAG *bag);
extern TABAG*       tbg_clone   (TABAG *bag);
extern TABAG*       tbg_copy    (TABAG *dst, TABAG *src);

extern int          tbg_mode    (const TABAG *bag);
extern ITEM         tbg_itemcnt (const TABAG *bag);
extern TID          tbg_cnt     (const TABAG *bag);
extern SUPP         tbg_wgt     (const TABAG *bag);
extern ITEM         tbg_max     (const TABAG *bag);
extern size_t       tbg_extent  (const TABAG *bag);
extern const TID*   tbg_icnts   (TABAG *bag, int recnt);
extern const SUPP*  tbg_ifrqs   (TABAG *bag, int recnt);

extern int          tbg_add     (TABAG *bag,  TRACT *t);
extern int          tbg_addw    (TABAG *bag, WTRACT *t);
extern int          tbg_addib   (TABAG *bag);
extern TRACT*       tbg_tract   (TABAG *bag, TID index);
extern WTRACT*      tbg_wtract  (TABAG *bag, TID index);
#ifdef TA_READ
extern int          tbg_read    (TABAG *bag, TABREAD *trd, int mode);
#endif
extern const char*  tbg_errmsg  (TABAG *bag, char *buf, size_t size);
#ifdef TA_WRITE
extern int          tbg_write   (TABAG *bag, TABWRITE *twr,
                                 const char *wgtfmt, ...);
#endif

extern int          tbg_istab   (TABAG *bag);
extern ITEM         tbg_recode  (TABAG *bag, SUPP min, SUPP max,
                                 ITEM cnt, int dir);
extern void         tbg_filter  (TABAG *bag, ITEM min,
                                 const int *marks, double wgt);
extern void         tbg_trim    (TABAG *bag, ITEM min,
                                 const int *marks, double wgt);
extern void         tbg_itsort  (TABAG *bag, int dir, int heap);
extern void         tbg_mirror  (TABAG *bag);
extern void         tbg_sort    (TABAG *bag, int dir, int heap);
extern void         tbg_sortsz  (TABAG *bag, int dir, int heap);
extern void         tbg_reverse (TABAG *bag);
extern TID          tbg_reduce  (TABAG *bag, int keep0);
extern void         tbg_setmark (TABAG *bag, int mark);
extern void         tbg_bitmark (TABAG *bag);
extern void         tbg_pack    (TABAG *bag, int n);
extern void         tbg_unpack  (TABAG *bag, int dir);
extern int          tbg_packcnt (TABAG *bag);
extern SUPP         tbg_occur   (TABAG *bag, const ITEM *items, ITEM n);
extern int          tbg_ipwgt   (TABAG *bag, int mode);

#ifndef NDEBUG
extern void         tbg_show    (TABAG *bag);
#endif

/*----------------------------------------------------------------------
  Surrogate Generation Functions
----------------------------------------------------------------------*/
#ifdef TA_SURR
extern TABAG*       tbg_ident  (TABAG *src, RNG *rng, TABAG *dst);
extern TABAG*       tbg_random (TABAG *src, RNG *rng, TABAG *dst);
extern TABAG*       tbg_swap   (TABAG *src, RNG *rng, TABAG *dst);
extern TABAG*       tbg_shuffle(TABAG *src, RNG *rng, TABAG *dst);
#endif
/*----------------------------------------------------------------------
  Transaction Array Functions
----------------------------------------------------------------------*/
extern void         taa_collate (TRACT **taa, TID n, ITEM k);
extern void         taa_uncoll  (TRACT **taa, TID n);
extern TID          taa_tabsize (TID n);
extern size_t       taa_dstsize (TID n, size_t x);
extern TID          taa_reduce  (TRACT **taa, TID n, ITEM end,
                                 const ITEM *map, void *buf,void **dst);

#ifndef NDEBUG
extern void         taa_show    (TRACT **taa, TID n, ITEMBASE *base);
#endif

/*----------------------------------------------------------------------
  Transaction Node Functions
----------------------------------------------------------------------*/
#ifdef TATREEFN
#ifdef TATCOMPACT
extern ITEM         tan_item    (const TANODE *node);
extern SUPP         tan_wgt     (const TANODE *node);
extern ITEM         tan_max     (const TANODE *node);
extern TANODE*      tan_sibling (const TANODE *node);
extern TANODE*      tan_children(const TANODE *node);
extern const ITEM*  tan_suffix  (const TANODE *node);
#else
extern SUPP         tan_wgt     (const TANODE *node);
extern ITEM         tan_max     (const TANODE *node);
extern ITEM         tan_size    (const TANODE *node);
extern ITEM*        tan_items   (TANODE *node);
extern ITEM         tan_item    (const TANODE *node, ITEM index);
extern TANODE*      tan_child   (const TANODE *node, ITEM index);
#endif
#endif
/*----------------------------------------------------------------------
  Transaction Tree Functions
----------------------------------------------------------------------*/
#ifdef TATREEFN
#ifdef TATCOMPACT
extern TATREE*      tat_create  (TABAG *bag);
extern void         tat_delete  (TATREE *tree, int del);
extern TABAG*       tat_tabag   (const TATREE *tree);
extern TANODE*      tat_root    (const TATREE *tree);
extern size_t       tat_size    (const TATREE *tree);
#else
extern TATREE*      tat_create  (TABAG *bag);
extern void         tat_delete  (TATREE *tree, int del);
extern TABAG*       tat_tabag   (const TATREE *tree);
extern TANODE*      tat_root    (const TATREE *tree);
extern size_t       tat_size    (const TATREE *tree);
#endif
extern int          tat_filter  (TATREE *tree, ITEM min,
                                 const int *marks, int heap);
#ifndef NDEBUG
extern void         tat_show    (TATREE *tree);
#endif
#endif

/*----------------------------------------------------------------------
  Preprocessor Definitions
----------------------------------------------------------------------*/
#define ib_mode(s)        ((s)->mode)
#define ib_cnt(s)         ((ITEM)idm_cnt((s)->idmap))
#define ib_item(s,n)      idm_getid((s)->idmap, n)
#define ib_name(s,i)      idm_name(idm_byid((s)->idmap, i))
#define ib_key(s,i)       idm_key (idm_byid((s)->idmap, i))
#define ib_obj(s,i)       (*(void**)idm_key(idm_byid((s)->idmap, i)))

#define ib_getwgt(s)      ((s)->wgt)
#define ib_setwgt(s,n)    ((s)->wgt  = (n))
#define ib_incwgt(s,n)    ((s)->wgt += (n))

#define ib_itemdata(s,i)  ((ITEMDATA*)idm_byid((s)->idmap, i))
#define ib_getfrq(s,i)    (ib_itemdata(s,i)->frq)
#define ib_setfrq(s,i,n)  (ib_itemdata(s,i)->frq  = (n))
#define ib_incfrq(s,i,n)  (ib_itemdata(s,i)->frq += (n))
#define ib_getxfq(s,i)    (ib_itemdata(s,i)->xfq)
#define ib_setxfq(s,i,n)  (ib_itemdata(s,i)->xfq  = (n))
#define ib_incxfq(s,i,n)  (ib_itemdata(s,i)->xfq += (n))
#define ib_getpen(s,i)    (ib_itemdata(s,i)->pen)
#define ib_setpen(s,i,p)  (ib_itemdata(s,i)->pen  = (p))
#define ib_maxfrq(s)      ((s)->max)

#define ib_tract(s)       ((TRACT*) (s)->tract)
#define ib_wtract(s)      ((WTRACT*)(s)->tract)

/*--------------------------------------------------------------------*/
#define ta_delete(t)      free(t)
#define ta_items(t)       ((const ITEM*)(t)->items)
#define ta_size(t)        ((t)->size)
#define ta_wgt(t)         ((t)->wgt)
#define ta_setmark(t,m)   ((t)->mark = (m))
#define ta_getmark(t)     ((t)->mark)

/*--------------------------------------------------------------------*/
#define wta_delete(t)     free(t)
#define wta_items(t)      ((t)->items)
#define wta_size(t)       ((t)->size)
#define wta_wgt(t)        ((t)->wgt)

/*--------------------------------------------------------------------*/
#define tbg_base(b)       ((b)->base)

#define tbg_mode(b)       ((b)->mode)
#define tbg_itemcnt(b)    ib_cnt((b)->base)
#define tbg_cnt(b)        ((b)->cnt)
#define tbg_wgt(b)        ((b)->wgt)
#define tbg_max(b)        ((b)->max)
#define tbg_extent(b)     ((b)->extent)

#define tbg_tract(b,i)    ((TRACT*) (b)->tracts[i])
#define tbg_wtract(b,i)   ((WTRACT*)(b)->tracts[i])
#define tbg_errmsg(b,s,n) ib_errmsg((b)->base, s, n)
#define tbg_reverse(b)    ptr_reverse((b)->tracts, (b)->cnt)
#define tbg_packcnt(b)    ((b)->mode & TA_PACKED)

/*--------------------------------------------------------------------*/

#define taa_dstsize(n,x)  ((size_t)(n)*sizeof(TRACT) +(x)*sizeof(ITEM))

/*--------------------------------------------------------------------*/
#ifdef TATREEFN
#ifdef TATCOMPACT

#define tan_item(n)       ((n)->item)
#define tan_wgt(n)        ((n)->wgt)
#define tan_max(n)        ((n)->max)
#define tan_sibling(n)    (((n)[1].item >= 0) ? (n)+1 : NULL)
#define tan_children(n)   ((TANODE*)(n)->data)
#define tan_suffix(n)     ((const ITEM*)(n)->data)

#define tat_tabag(t)      ((t)->bag)
#define tat_root(t)       (&(t)->root)

#else

#define tan_wgt(n)        ((n)->wgt)
#define tan_max(n)        ((n)->max)
#define tan_size(n)       ((n)->size)
#define tan_item(n,i)     ((n)->items[i])
#define tan_items(n)      ((n)->items)

#define tat_tabag(t)      ((t)->bag)
#define tat_root(t)       ((t)->root)

#endif
#endif

#endif
