/*----------------------------------------------------------------------
  File    : tabwrite.h
  Contents: table writer management
  Author  : Christian Borgelt
  History : 2010.10.08 file created
            2010.10.12 function twr_pad() added
            2010.10.13 name of output file added
            2012.07.23 functions twr_(x)ochr() and twr_other() added
            2013.03.20 size and length types changed to size_t
----------------------------------------------------------------------*/
#ifndef __TABWRITE__
#define __TABWRITE__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*----------------------------------------------------------------------
  Preprocessor Definitions
----------------------------------------------------------------------*/
#define CCHAR   const char      /* abbreviation */

/*----------------------------------------------------------------------
  Type Definitions
----------------------------------------------------------------------*/
typedef struct {                /* --- table writer --- */
  FILE  *file;                  /* file to write to */
  CCHAR *name;                  /* name of the output file */
  int   recsep;                 /* record separator */
  int   fldsep;                 /* field  separator */
  int   blank;                  /* blanks character */
  int   null;                   /* null   character */
  int   chars[32];              /* other  characters */
  char  nvname[2];              /* null   value name */
} TABWRITE;                     /* (table writer) */

/*----------------------------------------------------------------------
  Functions
----------------------------------------------------------------------*/
extern TABWRITE* twr_create (void);
extern int       twr_delete (TABWRITE *twr, int close);
extern int       twr_open   (TABWRITE *twr, FILE *file, CCHAR *name);
extern int       twr_close  (TABWRITE *twr);
extern FILE*     twr_file   (TABWRITE *twr);
extern CCHAR*    twr_name   (TABWRITE *twr);
extern int       twr_flush  (TABWRITE *twr);
extern int       twr_error  (TABWRITE *twr);

extern void      twr_chars  (TABWRITE *twr, int recsep, int fldsep,
                                            int blank,  int null);
extern void      twr_xchars (TABWRITE *twr,      const char *recsep,
                             const char *fldsep, const char *blank,
                             const char *null);
extern void      twr_ochr   (TABWRITE *twr, int id, int c);
extern void      twr_xochr  (TABWRITE *twr, int id, const char *s);

extern int       twr_printf (TABWRITE *twr, const char *fmt, ...);
extern int       twr_puts   (TABWRITE *twr, const char *s);
extern int       twr_putc   (TABWRITE *twr, int c);
extern int       twr_recsep (TABWRITE *twr);
extern int       twr_fldsep (TABWRITE *twr);
extern int       twr_blank  (TABWRITE *twr);
extern int       twr_other  (TABWRITE *twr, int id);
extern int       twr_null   (TABWRITE *twr);
extern void      twr_pad    (TABWRITE *twr, size_t n);
extern int       twr_nvname (TABWRITE *twr);

/*----------------------------------------------------------------------
  Preprocessor Definitions
----------------------------------------------------------------------*/
#define twr_file(t)         ((t)->file)
#define twr_name(t)         ((t)->name)
#define twr_flush(t)        (!(t)->file ? 0 : fflush((t)->file))
#define twr_error(t)        (!(t)->file ? 0 : ferror((t)->file))
#define twr_printf(t,s,...) (!(t)->file ? 0 : \
                             fprintf((t)->file, s, ##__VA_ARGS__))
#define twr_puts(t,s)       (!(t)->file ? 0 : fputs((s), (t)->file))
#define twr_putc(t,c)       (!(t)->file ? 0 : fputc((c), (t)->file))
#define twr_recsep(t)       (!(t)->file ? 0 : \
                             fputc((t)->recsep,   (t)->file))
#define twr_fldsep(t)       (!(t)->file ? 0 : \
                             fputc((t)->fldsep,   (t)->file))
#define twr_blank(t)        (!(t)->file ? 0 : \
                             fputc((t)->blank,    (t)->file))
#define twr_other(t,i)      (!(t)->file ? 0 : \
                             fputc((t)->chars[i], (t)->file))
#define twr_null(t)         (!(t)->file ? 0 : \
                             fputc((t)->null,     (t)->file))
#define twr_nvname(t)       ((t)->nvname)

#endif  /* #ifdef __TABWRITE__ */
