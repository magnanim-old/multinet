/*----------------------------------------------------------------------
  File    : symtab.h
  Contents: symbol table and identifier map management
  Author  : Christian Borgelt
  History : 1995.10.22 file created
            1995.10.30 functions made independent of symbol data
            1995.11.26 symbol types and visibility levels added
            1996.01.04 function st_clear() added (remove all symbols)
            1996.02.27 st_insert() modified, st_name(), st_type() added
            1996.03.26 insertion into hash bin simplified
            1996.06.28 dynamic hash bin array enlargement added
            1997.04.01 functions st_clear() and st_remove() combined
            1998.05.31 list of all symbols removed (inefficient)
            1998.06.20 deletion function moved to st_create()
            1998.09.28 function st_stats() added (for debugging)
            1999.02.04 long int changed to int (assume 32 bit system)
            1999.11.10 special identifier map management added
            2004.12.15 function idm_trunc() added (remove names)
            2008.08.11 function idm_getid() added, changed to CMPFN
            2011.07.12 generalized to arbitrary keys (not just names)
            2011.08.17 size function removed, key size made parameter
            2013.02.03 argument of idm_getid() changed to const void*
            2013.02.11 general pointers added as possible keys
            2013.03.07 size-related data types changed to size_t
----------------------------------------------------------------------*/
#ifndef __SYMTAB__
#define __SYMTAB__
#include <stdio.h>
#include "arrays.h"

/*----------------------------------------------------------------------
  Preprocessor Definitions
----------------------------------------------------------------------*/

#define IDMAPFN //ADDED

#ifndef IDENT
#define IDENT       int         /* identifier type for map */
#endif

#define int         1           /* to check definition of IDENT */
#define long        2           /* for certain types */
#define ptrdiff_t   3

#if   IDENT==int
#ifndef IDENT_MIN
#define IDENT_MIN   INT_MIN     /* minimum identifier */
#endif
#ifndef IDENT_MAX
#define IDENT_MAX   INT_MAX     /* maximum identifier */
#endif
#ifndef IDENT_FMT
#define IDENT_FMT   "d"         /* printf format code for int */
#endif

#elif IDENT==long
#ifndef IDENT_MIN
#define IDENT_MIN   LONG_MIN    /* minimum identifier */
#endif
#ifndef IDENT_MAX
#define IDENT_MAX   LONG_MAX    /* maximum identifier */
#endif
#ifndef IDENT_FMT
#define IDENT_FMT   "ld"        /* printf format code for long */
#endif

#elif IDENT==ptrdiff_t
#ifndef IDENT_MIN
#define IDENT_MIN   PTRDIFF_MIN /* minimum identifier */
#endif
#ifndef IDENT_MAX
#define IDENT_MAX   PTRDIFF_MAX /* maximum identifier */
#endif
#  ifdef _MSC_VER
#  define IDENT_FMT "Id"        /* printf format code for ptrdiff_t */
#  else
#  define IDENT_FMT "td"        /* printf format code for ptrdiff_t */
#  endif                        /* MSC still does not support C99 */

#else
#error "IDENT must be either 'int', 'long' or 'ptrdiff_t'"
#endif

#undef int                      /* remove preprocessor definitions */
#undef long                     /* needed for the type checking */
#undef ptrdiff_t

/*--------------------------------------------------------------------*/

#ifndef SIZE_FMT
#  ifdef _MSC_VER
#  define SIZE_FMT  "Iu"        /* printf format code for size_t */
#  else
#  define SIZE_FMT  "zu"        /* printf format code for size_t */
#  endif                        /* MSC still does not support C99 */
#endif

/*--------------------------------------------------------------------*/

#define EXISTS    ((void*)-1)   /* symbol exists already */
#define IDMAP     SYMTAB        /* id maps are special symbol tables */

/* --- abbreviations for standard function sets --- */
#define ST_STRFN  st_strhash, st_strcmp, NULL
#define ST_INTFN  st_inthash, st_intcmp, NULL
#define ST_LNGFN  st_lnghash, st_lngcmp, NULL
#define ST_SIZFN  st_sizhash, st_sizcmp, NULL
#define ST_DIFFN  st_difhash, st_difcmp, NULL
#define ST_PTRFN  st_ptrhash, st_ptrcmp, NULL

/*----------------------------------------------------------------------
  Type Definitions
----------------------------------------------------------------------*/
typedef size_t HASHFN (const void *key, int type);

typedef struct ste {            /* --- symbol table element --- */
  struct ste *succ;             /* successor in hash bin */
  void       *key;              /* symbol name/key */
  int        type;              /* symbol type */
  size_t     level;             /* visibility level */
} STE;                          /* (symbol table element) */

typedef struct {                /* --- symbol table --- */
  size_t     cnt;               /* current number of symbols */
  size_t     level;             /* current visibility level */
  size_t     size;              /* current hash table size */
  size_t     max;               /* maximal hash table size */
  HASHFN     *hashfn;           /* hash function */
  CMPFN      *cmpfn;            /* comparison function */
  void       *data;             /* comparison data */
  OBJFN      *delfn;            /* symbol deletion function */
  STE        **bins;            /* array of hash bins */
  size_t     idsize;            /* size of identifier array */
  IDENT      **ids;             /* identifier array */
} SYMTAB;                       /* (symbol table) */

/*----------------------------------------------------------------------
  Name/Key Functions
----------------------------------------------------------------------*/
extern int         st_strcmp  (const void *a, const void *b, void *d);
extern size_t      st_strhash (const void *s, int type);
extern size_t      st_strsize (const void *s);

extern int         st_intcmp  (const void *a, const void *b, void *d);
extern size_t      st_inthash (const void *i, int type);
extern size_t      st_intsize (const void *i);

extern int         st_lngcmp  (const void *a, const void *b, void *d);
extern size_t      st_lnghash (const void *i, int type);
extern size_t      st_lngsize (const void *i);

extern int         st_difcmp  (const void *a, const void *b, void *d);
extern size_t      st_difhash (const void *i, int type);
extern size_t      st_difsize (const void *i);

extern int         st_ptrcmp  (const void *a, const void *b, void *d);
extern size_t      st_ptrhash (const void *p, int type);
extern size_t      st_ptrsize (const void *p);

/*----------------------------------------------------------------------
  Symbol Table Functions
----------------------------------------------------------------------*/
extern SYMTAB*     st_create  (size_t init, size_t max, HASHFN hashfn,
                               CMPFN cmpfn, void *data, OBJFN delfn);
extern void        st_delete  (SYMTAB *tab);
extern void*       st_insert  (SYMTAB *tab, const void *key, int type,
                               size_t keysize, size_t datasize);
extern int         st_remove  (SYMTAB *tab, const void *key, int type);
extern void*       st_lookup  (SYMTAB *tab, const void *key, int type);
extern void        st_begblk  (SYMTAB *tab);
extern void        st_endblk  (SYMTAB *tab);
extern size_t      st_symcnt  (const SYMTAB *tab);
extern const char* st_name    (const void *data);
extern const void* st_key     (const void *data);
extern int         st_type    (const void *data);
#ifndef NDEBUG
extern void        st_stats   (const SYMTAB *tab);
#endif

/*----------------------------------------------------------------------
  Identifier Map Functions
----------------------------------------------------------------------*/
#ifdef IDMAPFN
extern IDMAP*      idm_create (size_t init, size_t max, HASHFN hashfn,
                               CMPFN cmpfn, void *data, OBJFN delfn);
extern void        idm_delete (IDMAP* idm);
extern void*       idm_add    (IDMAP* idm, const void *key,
                               size_t keysize, size_t datasize);
extern void*       idm_byname (IDMAP* idm, const char *name);
extern void*       idm_bykey  (IDMAP* idm, const void *key);
extern void*       idm_byid   (IDMAP* idm, IDENT id);
extern IDENT       idm_getid  (IDMAP* idm, const void *name);
extern const char* idm_name   (const void *data);
extern const void* idm_key    (const void *data);
extern IDENT       idm_cnt    (const IDMAP *idm);
extern void        idm_sort   (IDMAP *idm, CMPFN cmpfn, void *data,
                               IDENT *map, int dir);
extern void        idm_trunc  (IDMAP *idm, size_t n);
#ifndef NDEBUG
extern void        idm_stats  (const IDMAP *idm);
#endif
#endif
/*----------------------------------------------------------------------
  Preprocessor Definitions
----------------------------------------------------------------------*/
#define st_begblk(t)      ((t)->level++)
#define st_symcnt(t)      ((t)->cnt)
#define st_name(d)        ((const char*)((STE*)(d)-1)->key)
#define st_key(d)         ((const void*)((STE*)(d)-1)->key)
#define st_type(d)        (((STE*)(d)-1)->type)

/*--------------------------------------------------------------------*/
#ifdef IDMAPFN
#define idm_delete(m)     st_delete(m)
#define idm_add(m,n,k,s)  st_insert(m,n,0,k,s)
#define idm_byname(m,n)   st_lookup(m,n,0)
#define idm_bykey(m,k)    st_lookup(m,k,0)
#define idm_byid(m,i)     ((void*)(m)->ids[i])
#define idm_name(d)       st_name(d)
#define idm_key(d)        st_key(d)
#define idm_cnt(m)        ((IDENT)st_symcnt(m))
#ifndef NDEBUG
#define idm_stats(m)      st_stats(m)
#endif
#endif
#endif
