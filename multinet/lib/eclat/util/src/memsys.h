/*----------------------------------------------------------------------
  File    : memsys.h
  Contents: memory management system for equally sized (small) objects
  Author  : Christian Borgelt
  History : 2004.12.10 file created from fpgrowth.c
            2008.01.23 counting of used blocks added
            2008.11.18 recording of maximum number of used blocks added
            2010.07.30 arrays of unassigned objects added, ms_clear()
            2010.08.03 state stack, functions ms_push(), ms_pop() added
            2010.09.07 functions ms_error() and ms_reset() added
            2013.03.20 stack variables (cap, top) changed to size_t
----------------------------------------------------------------------*/
#ifndef __MEMSYS__
#define __MEMSYS__
#include <stddef.h>

/*----------------------------------------------------------------------
  Type Definitions
----------------------------------------------------------------------*/
typedef struct {                /* --- memory system state --- */
  void    **next;               /* next unassigned object */
  void    **curr;               /* current memory block */
  size_t  used;                 /* number of used objects */
} MSSTATE;                      /* (memory system state) */

typedef struct {                /* --- memory management system --- */
  size_t  size;                 /* size of an object (in void* units) */
  size_t  mbsz;                 /* size of a memory block */
  size_t  used;                 /* number of used objects */
  size_t  umax;                 /* maximum number of used objects */
  void    **free;               /* list of free objects */
  void    **next;               /* next unassigned object */
  void    **curr;               /* current memory block */
  void    **list;               /* list of allocated memory blocks */
  int     err;                  /* allocation error indicator */
  size_t  cap;                  /* stack capacity (maximum size) */
  size_t  top;                  /* top of stack (next free element) */
  MSSTATE *stack;               /* stack of memory system states */
} MEMSYS;                       /* (memory management system) */

/*----------------------------------------------------------------------
  Functions
----------------------------------------------------------------------*/
extern MEMSYS*   ms_create (size_t size, size_t cnt);
extern void      ms_delete (MEMSYS *ms);
extern void      ms_clear  (MEMSYS *ms, int shrink);
extern void*     ms_alloc  (MEMSYS *ms);
extern void      ms_free   (MEMSYS *ms, void *obj);
extern ptrdiff_t ms_push   (MEMSYS *ms);
extern ptrdiff_t ms_pop    (MEMSYS *ms);
extern int       ms_error  (MEMSYS *ms);
extern int       ms_reset  (MEMSYS *ms);
extern size_t    ms_used   (MEMSYS *ms);
extern size_t    ms_umax   (MEMSYS *ms);

/*----------------------------------------------------------------------
  Preprocessor Definitions
----------------------------------------------------------------------*/
#define ms_error(m)      ((m)->err)
#define ms_reset(m)      ((m)->err = 0)
#define ms_used(m)       ((m)->used)
#define ms_umax(m)       ((m)->umax)

#endif
