/*----------------------------------------------------------------------
  File    : memsys.c
  Contents: memory management system for equally sized (small) objects
  Author  : Christian Borgelt
  History : 2004.12.10 file created from fpgrowth.c
            2008.01.23 counting of used objects added
            2008.11.18 recording of maximum number of used blocks added
            2010.07.30 arrays of unassigned objects added, ms_clear()
            2010.08.03 state stack, functions ms_push(), ms_pop() added
            2010.09.07 persistent allocation error indicator added
            2010.12.07 several explicit type casts added (for C++)
            2013.03.20 stack variables (cap, top) changed to size_t
----------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "memsys.h"
#ifdef STORAGE
#include "storage.h"
#endif

/*----------------------------------------------------------------------
  Main Functions
----------------------------------------------------------------------*/

MEMSYS* ms_create (size_t size, size_t cnt)
{                               /* --- create a memory system */
  MEMSYS *ms;                   /* created memory system */

  assert((cnt > 0) && (size > 0));   /* check the function arguments */
  ms = (MEMSYS*)malloc(sizeof(MEMSYS));
  if (!ms) return NULL;         /* create a memory management system */
  ms->size = (size+sizeof(void*)-1) /sizeof(void*);
  /* ms->size: the size of an object in void* units (rounded up) */
  ms->mbsz = cnt *ms->size +2;  /* 2: successor and predecessor */
  /* ms->mbsz: the size of a memory block in void* units */
  ms->free = ms->next = ms->curr = ms->list = NULL;
  ms->used = ms->umax = 0;      /* initialize the variables */
  ms->cap  = ms->top  = 0; ms->stack = NULL;
  ms->err  = 0;
  return ms;                    /* return the created memory system */
}  /* ms_create() */

/*--------------------------------------------------------------------*/

void ms_delete (MEMSYS *ms)
{                               /* --- delete a memory system */
  void **b;                     /* buffers for deallocation */

  assert(ms);                   /* check the function argument */
  for (b = ms->list; b; b = ms->list) {
    ms->list = (void**)*b; free(b); }
  while (ms->list) {            /* delete the list of blocks */
    b = ms->list; ms->list = (void**)*b; free(b); }
  if (ms->stack) free(ms->stack);  /* delete a possible stack */
  free(ms);                        /* and the base structure */
}  /* ms_delete() */

/*--------------------------------------------------------------------*/

void ms_clear (MEMSYS *ms, int shrink)
{                               /* --- clear a memory system */
  void **b;                     /* buffer for a memory block */

  if (!(b = ms->list))          /* if there are no memory blocks, */
    ms->next = ms->curr = NULL; /* clear the unassigned objects */
  else {                        /* if there are memory blocks, */
    if (shrink) {               /* if to shrink the memory block list */
      while (*b) { ms->list = (void**)*b; free(b); b = ms->list; }
      b[1] = NULL;              /* delete all but the last block */
    }                           /* and clear its successor pointer */
    ms->curr = b;               /* set the only memory block that */
    ms->next = b+2;             /* is left as the current block */
  }                             /* and set next unassigned object */
  ms->free = NULL;              /* clear the list of free objects */
  ms->used = ms->umax = 0;      /* there are no used objects and */
  ms->cap  = ms->top  = 0;      /* the memory state stack is empty */
  ms->err  = 0;                 /* clear the error indicator */
}  /* ms_clear() */

/*--------------------------------------------------------------------*/

void* ms_alloc (MEMSYS *ms)
{                               /* --- allocate an object */
  void **obj;                   /* allocated object */
  void **b;                     /* next/new memory block */

  assert(ms);                   /* check the function argument */
  if ((obj = ms->free)) {       /* if there is a free object, */
    ++ms->used;                 /* count the object as used, */
    ms->free = (void**)*(void**)obj;
    return obj;                 /* remove it from the free list */
  }                             /* and return it */
  /* The new value of ms->used cannot exceed ms->umax, because all  */
  /* objects on the free list have been in use before and thus have */
  /* already been counted for the maximum number of used objects.   */
  if      (ms->next)            /* if there are new objects, */
    obj = ms->next;             /* get the next new object */
  else if (((b = ms->curr)) && b[1]) {
    ms->curr = b = (void**)b[1];/* if there is a successor block */
    ms->next = obj = b+2; }     /* and get its first object */
  else {                        /* if there is no new object left */
    b = (void**)malloc(ms->mbsz *sizeof(void*));
    if (!b) { ms->err = -1; return NULL; }
    b[1] = NULL;                /* allocate a new memory block and */
    b[0] = ms->list;            /* add it at the end of the list */
    if (ms->list) ms->list[1] = b;
    ms->list = ms->curr = b;    /* make the block the current one */
    ms->next = obj      = b+2;  /* and get the first object */
  }                             /* in this memory block */
  ms->next += ms->size;         /* advance the object array position */
  if (ms->next >= ms->curr +ms->mbsz)
    ms->next = NULL;            /* check for end of object array */
  if (++ms->used > ms->umax)    /* count the allocated object */
    ms->umax = ms->used;        /* and update the maximum */
  return obj;                   /* return the retrieved object */
}  /* ms_alloc() */

/*--------------------------------------------------------------------*/

void ms_free (MEMSYS *ms, void *obj)
{                               /* --- deallocate an f.p. tree node */
  assert(ms && obj);            /* check the function arguments */
  assert(ms->used > 0);         /* check the number of used objects */
  *(void**)obj = ms->free;      /* insert the freed object */
  ms->free     = (void**)obj;   /* at the head of the free list */
  ms->used--;                   /* count the deallocated object */
}  /* ms_free() */

/*--------------------------------------------------------------------*/

ptrdiff_t ms_push (MEMSYS *ms)
{                               /* --- store the current state */
  size_t  n;                    /* new stack size */
  MSSTATE *s;                   /* buffer for reallocation */

  assert(ms && !ms->free);      /* check the function argument */
  if (ms->top >= ms->cap) {     /* if the state stack is full */
    n = ms->cap +((ms->cap > 32) ? ms->cap >> 1 : 32);
    s = (MSSTATE*)realloc(ms->stack, n *sizeof(MSSTATE));
    if (!s) return -1;          /* enlarge the state stack */
    ms->stack = s; ms->cap = n; /* and set the enlarged stack */
  }                             /* and its new size */
  s = ms->stack +ms->top;       /* get the next free stack element */
  s->next = ms->next;           /* not the current state */
  s->curr = ms->curr;           /* of the memory system */
  s->used = ms->used;
  return (ptrdiff_t)++ms->top;  /* return the new stack size */
}  /* ms_push() */

/*--------------------------------------------------------------------*/

ptrdiff_t ms_pop (MEMSYS *ms)
{                               /* --- retrieve the last state */
  MSSTATE *s;                   /* to access the top stack element */

  assert(ms && !ms->free);      /* check the function argument */
  if (ms->top <= 0) return -1;  /* check for an empty stack */
  s = ms->stack +(--ms->top);   /* get the top stack element */
  ms->next = s->next;           /* restore the last state */
  ms->curr = s->curr;           /* of the memory system */
  ms->used = s->used;
  return (ptrdiff_t)ms->top;    /* return the new stack size */
}  /* ms_pop() */
