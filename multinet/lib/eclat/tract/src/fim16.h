/*----------------------------------------------------------------------
  File    : fim16.h
  Contents: frequent item set mining with 16 items / 16 items machine
  Author  : Christian Borgelt
  History : 2011.08.24 file created
            2011.09.17 min. support and reporter moved to m16_create()
            2011.09.19 function m16_clear() added (remove all trans.)
            2011.09.20 perfect extension support removed from m16_mine()
            2011.09.21 function m16_show() added (for debugging)
            2012.06.19 item identifier map added (e.g. m16_setmap())
 ---------------------------------------------------------------------*/
#ifndef __FIM16__
#define __FIM16__
#include <stdlib.h>
#include <string.h>
#include "report.h"

/*----------------------------------------------------------------------
  Type Definitions
----------------------------------------------------------------------*/
typedef unsigned short BITTA;   /* --- bit rep. of a transaction --- */

typedef struct {                /* --- FIM 16 items machine --- */
  ISREPORT *report;             /* item set reporter */
  int      dir;                 /* processing direction */
  SUPP     smin;                /* minimum support */
  SUPP     ttw;                 /* total transaction weight */
  BITTA    tor;                 /* bitwise or of added trans. */
  SUPP     *wgts;               /* transaction weights */
  ITEM     *map;                /* item identifier map */
  SUPP     supps[16];           /* support values of items */
  BITTA    *btas[16];           /* array of bit rep. transactions */
  BITTA    *ends[16];           /* ends of transaction arrays */
} FIM16;                        /* (FIM 16 items machine) */

/*----------------------------------------------------------------------
  Functions
----------------------------------------------------------------------*/
extern FIM16* m16_create (int dir, SUPP supp, ISREPORT *report);
extern void   m16_delete (FIM16 *fim);
extern int    m16_dir    (FIM16 *fim);
extern void   m16_setmap (FIM16 *fim, int i, ITEM id);
extern ITEM   m16_getmap (FIM16 *fim, int i);
extern void   m16_clrmap (FIM16 *fim);
extern void   m16_add    (FIM16 *fim, BITTA tract, SUPP wgt);
extern void   m16_addx   (FIM16 *fim, const ITEM *items, ITEM n,
                          SUPP wgt);
extern void   m16_adds   (FIM16 *fim, const ITEM *items, SUPP wgt);
extern void   m16_addta  (FIM16 *fim, TRACT *tract);
extern void   m16_addtbg (FIM16 *fim, TABAG *tabag);
extern void   m16_clear  (FIM16 *fim);
extern int    m16_mine   (FIM16 *fim);
#ifndef NDEBUG
extern void   m16_show   (FIM16 *fim);
#endif

/*----------------------------------------------------------------------
  Preprocessor Definitions
----------------------------------------------------------------------*/
#define m16_dir(m)          ((m)->dir)
#define m16_addta(m,t)      m16_adds(m, ta_items(t), ta_wgt(t))
#define m16_setmap(m,i,d)   ((m)->map[i] = (d))
#define m16_getmap(m,i)     ((m)->map[i])

#endif
