/*----------------------------------------------------------------------
  File    : tabwrite.c
  Contents: table writer management
  Author  : Christian Borgelt
  History : 2010.10.08 file created
            2010.10.12 function twr_pad() added
            2010.10.13 name of output file added, error code added
            2010.10.15 bug in function twr_open() fixed (name assignm.)
            2010.12.10 close file in twr_delete() mapped to twr_close()
            2012.07.23 functions twr_(x)ochr() and twr_other() added
            2013.03.20 size/length types changed to size_t
            2013.10.15 check of ferror() added to twr_close()
----------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "tabwrite.h"
#include "escape.h"
#ifdef STORAGE
#include "storage.h"
#endif

/*----------------------------------------------------------------------
  Functions
----------------------------------------------------------------------*/

TABWRITE* twr_create (void)
{                               /* --- create a table writer */
  TABWRITE *twr;                /* created table writer */

  twr = (TABWRITE*)malloc(sizeof(TABWRITE));
  if (!twr) return NULL;        /* allocate memory for a writer */
  twr->file      = NULL;        /* clear the file */
  twr->name      = NULL;        /* and   its name */
  twr->recsep    = '\n';        /* and initialize the characters */
  twr->fldsep    =      twr->blank = ' ';
  twr->nvname[0] = '?'; twr->null  = '?';
  twr->nvname[1] = '\0';
  return twr;                   /* return created table writer */
}  /* twr_create() */

/*--------------------------------------------------------------------*/

int twr_delete (TABWRITE *twr, int close)
{                               /* --- delete a table writer */
  int r = 0;                    /* result of twr_close() */

  assert(twr);                  /* check the function argument */
  if (close) r = twr_close(twr);/* close the current output file */
  free(twr);                    /* and delete the table writer */
  return r;                     /* return the result of fclose() */
}  /* twr_delete() */

/*--------------------------------------------------------------------*/

int twr_open (TABWRITE *twr, FILE *file, const char *name)
{                               /* --- open a new file */
  assert(twr);                  /* check the function arguments */
  if (file) {                   /* if a file is given directly, */
    if      (name)           twr->name = name;/* store the name */
    else if (file == stdout) twr->name = "<stdout>";
    else if (file == stderr) twr->name = "<stderr>";
    else                     twr->name = "<unknown>"; }
  else if (! name) {            /* if no name is given */
    file = NULL;             twr->name = "<null>"; }
  else if (!*name) {            /* if an empty name is given */
    file = stdout;           twr->name = "<stdout>"; }
  else {                        /* if a proper name is given */
    file = fopen(twr->name = name, "wb");
    if (!file) return -2;       /* open file with given name */
  }                             /* and check for an error */
  twr->file = file;             /* store the new output file */
  return 0;                     /* return 'ok' */
}  /* twr_open() */

/*--------------------------------------------------------------------*/

int twr_close (TABWRITE *twr)
{                               /* --- close the current file */
  int r;                        /* result of fflush()/fclose() */

  assert(twr);                  /* check the function argument */
  if (!twr->file) return 0;     /* check for an output file */
  r  = ferror(twr->file);       /* get the error indicator */
  r |= ((twr->file == stdout) || (twr->file == stderr))
     ? fflush(twr->file) : fclose(twr->file);
  twr->file = NULL;             /* close the current output file */
  return r;                     /* return the result of fclose() */
}  /* twr_close() */

/*--------------------------------------------------------------------*/

void twr_chars (TABWRITE *twr, int recsep, int fldsep,
                               int blank,  int null)
{                               /* --- set characters of all classes */
  assert(twr);                  /* check the function arguments */
  if (recsep > 0) twr->recsep = recsep;
  if (fldsep > 0) twr->fldsep = fldsep;
  if (blank  > 0) twr->blank  = blank;
  if (null   > 0) twr->null   = null;
}  /* twr_chars() */

/*--------------------------------------------------------------------*/

void twr_xchars (TABWRITE *twr, const char *recsep, const char *fldsep,
                                const char *blank,  const char *null)
{                               /* --- set characters of all classes */
  assert(twr);                  /* check the function arguments */
  if (recsep) twr->recsep = esc_decode(recsep, NULL);
  if (fldsep) twr->fldsep = esc_decode(fldsep, NULL);
  if (blank)  twr->blank  = esc_decode(blank,  NULL);
  if (null)   twr->null   = esc_decode(null,   NULL);
}  /* twr_xchars() */

/*--------------------------------------------------------------------*/

void twr_ochr (TABWRITE *twr, int id, int c)
{                               /* --- set other special character */
  assert(twr);                  /* check the function argument */
  twr->chars[id & 0x1f] = c;    /* store the character */
}  /* twr_ochr() */

/*--------------------------------------------------------------------*/

void twr_xochr (TABWRITE *twr, int id, const char *s)
{                               /* --- set other special character */
  assert(twr);                  /* check the function argument */
  twr->chars[id & 0x1f] = esc_decode(s, NULL);
}  /* twr_xochr() */

/*--------------------------------------------------------------------*/

void twr_pad (TABWRITE *twr, size_t n)
{                               /* --- pad with blanks */
  assert(twr);                  /* check the function arguments */
  if (!twr->file) return;       /* check for an output file */
  while (n-- > 0) fputc(twr->fldsep, twr->file);
}  /* twr_pad() */
