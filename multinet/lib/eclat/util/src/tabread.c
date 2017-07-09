/*----------------------------------------------------------------------
  File    : tabread.c
  Contents: table reader management
  Author  : Christian Borgelt
  History : 1998.01.04 file created
            1998.03.11 additional character flags enabled
            1998.08.12 function trd_copy() added (copy character flags)
            1998.09.27 function trd_read() improved
            1998.11.26 some function parameters changed to const
            1999.02.04 long int changed to int (32 bit systems)
            1999.11.16 number of characters cleared for an empty field
            2000.12.01 '\r' made a default blank character
            2001.07.14 functions trd_field() and trd_info() added
            2001.08.19 last delimiter stored in reader object
            2002.02.11 function trd_rec() added (current record)
            2006.10.06 result value policy of trd_read() improved
            2007.02.13 renamed to tabscan, redesigned, TRD_NULL added
            2007.05.17 function trd_allchs() added (set all chars.)
            2007.09.02 made '*' a null value character by default
            2008.07.08 bug in function trd_read() fixed (null at EOL)
            2010.03.15 function trd_chars() adapted (optional adding)
            2010.03.16 storing of last read character added
            2010.08.22 redesigned with internal file and buffer
            2010.10.07 functions trd_open() and trd_close() added
            2010.10.13 name of input file added, error info. simplified
            2010.10.15 bug in function trd_open() fixed (name assignm.)
            2011.03.20 order of arguments of trd_istype() changed
            2013.03.20 record and position type changed to size_t
            2013.10.15 check of ferror() added to trd_close()
----------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "tabread.h"
#include "escape.h"
#ifdef STORAGE
#include "storage.h"
#endif

/*----------------------------------------------------------------------
  Preprocessor Definitions
----------------------------------------------------------------------*/
#ifndef SIZE_FMT
#  ifdef _MSC_VER
#  define SIZE_FMT    "Iu"      /* printf format code for size_t */
#  else
#  define SIZE_FMT    "zu"      /* printf format code for size_t */
#  endif                        /* MSC still does not support C99 */
#endif

/* --- convenience functions --- */
#define isrecsep(c)   trd_istype(trd, c, TRD_RECSEP)
#define isfldsep(c)   trd_istype(trd, c, TRD_FLDSEP)
#define issep(c)      trd_istype(trd, c, TRD_FLDSEP|TRD_RECSEP)
#define isblank(c)    trd_istype(trd, c, TRD_BLANK)
#define isnull(c)     trd_istype(trd, c, TRD_NULL)
#define iscomment(c)  trd_istype(trd, c, TRD_COMMENT)

#define GETC(t,c,d) \
  if ((c = trd_getc(t)) < 0) { (t)->last = EOF; \
    return (t)->delim = (c <= TRD_ERR) ? TRD_ERR : (d); }

/*----------------------------------------------------------------------
  Functions
----------------------------------------------------------------------*/

TABREAD* trd_create (void)
{                               /* --- create a table reader */
  TABREAD *trd;                 /* created table reader */

  trd = (TABREAD*)malloc(sizeof(TABREAD));
  if (!trd) return NULL;        /* allocate memory for a reader */
  trd->file  = NULL;            /* clear the file */
  trd->name  = NULL;            /* and   its name */
  trd->delim = trd->last = TRD_EOF;
  trd->next  = trd->end  = trd->buf;
  trd->rec   = 1;               /* current record is the first */
  trd->pos   = 0;               /* position is before first field */
  trd->field[trd->len = 0] = 0; /* current field is empty */
  memset(trd->flags, 0, sizeof(trd->flags));
  trd->flags['\n'] = TRD_RECSEP;
  trd->flags['\t'] = trd->flags[' '] = TRD_BLANK|TRD_FLDSEP;
  trd->flags['\r'] = TRD_BLANK;
  trd->flags[',' ] = TRD_FLDSEP;
  trd->flags['?' ] = trd->flags['*'] = TRD_NULL;
  trd->flags['#' ] = TRD_COMMENT;
  return trd;                   /* set default character flags */
}  /* trd_create() */           /* return created table reader */

/*--------------------------------------------------------------------*/

int trd_delete (TABREAD *trd, int close)
{                               /* --- delete a table reader */
  int r;                        /* result of fclose() */

  assert(trd);                  /* check the function argument */
  r = (close) ? trd_close(trd) : 0; /* close the input file and */
  free(trd);                        /* delete the table reader */
  return r;                     /* return the result of fclose() */
}  /* trd_delete() */

/*--------------------------------------------------------------------*/

int trd_open (TABREAD *trd, FILE *file, const char *name)
{                               /* --- open a new file */
  assert(trd);                  /* check the function arguments */
  if (file) {                   /* if a file is given directly, */
    if      (name)          trd->name = name; /* store the name */
    else if (file == stdin) trd->name = "<stdin>";
    else                    trd->name = "<unknown>"; }
  else if (!name || !*name) {   /* if no file name is given */
    file = stdin;           trd->name = "<stdin>"; }
  else {                        /* if a proper file name is given */
    file = fopen(trd->name = name, "rb");
    if (!file) return -2;       /* open file with given name */
  }                             /* and check for an error */
  trd->file  = file;            /* store the new input file */
  trd->delim = trd->last = TRD_EOF;
  trd->next  = trd->end  = trd->buf;
  trd->rec   = 1;               /* current record is the first */
  trd->pos   = 0;               /* position is before first field */
  trd->field[trd->len = 0] = 0; /* current field is empty */
  return 0;                     /* return 'ok' */
}  /* trd_open() */

/*--------------------------------------------------------------------*/

int trd_close (TABREAD *trd)
{                               /* --- close the current file */
  int r;                        /* result of fclose() */

  assert(trd);                  /* check the function arguments */
  if (!trd->file) return 0;     /* check whether there is a file */
  r = ferror(trd->file);        /* check the error indicator */
  if (trd->file != stdin) r |= fclose(trd->file);
  trd->file = NULL;             /* close the current input file */
  return r;                     /* return the result of fclose() */
}  /* trd_close() */

/*--------------------------------------------------------------------*/

void trd_chars (TABREAD *trd, int type, const char *chars)
{                               /* --- set characters of a class */
  int  c;                       /* loop variable, character */
  char *s;                      /* to traverse the characters */

  assert(trd);                  /* check the function arguments */
  if (!chars) return;           /* if no characters given, abort */
  if (!(type & TRD_ADD))        /* if to replace characters */
    for (c = 0; c < 256; c++)   /* traverse the characters and */
      trd->flags[c] &= ~type;   /* clear character flags in type */
  type &= ~TRD_ADD;             /* remove the flag for adding */
  for (s = (char*)chars; *s; )  /* set the character flags */
    trd->flags[esc_decode(s, &s)] |= type;
}  /* trd_chars() */

/*--------------------------------------------------------------------*/

void trd_allchs (TABREAD *trd,        const char *recseps,
                 const char *fldseps, const char *blanks,
                 const char *nullchs, const char *comment)
{                               /* --- set characters of all classes */
  assert(trd);                  /* check the function arguments */
  if (recseps) trd_chars(trd, TRD_RECSEP,  recseps);
  if (fldseps) trd_chars(trd, TRD_FLDSEP,  fldseps);
  if (blanks)  trd_chars(trd, TRD_BLANK,   blanks);
  if (nullchs) trd_chars(trd, TRD_NULL,    nullchs);
  if (comment) trd_chars(trd, TRD_COMMENT, comment);
}  /* trd_allchs() */

/*--------------------------------------------------------------------*/

int trd_getc (TABREAD *trd)
{                               /* --- get the next character */
  assert(trd && trd->file);     /* check the function arguments */
  if (trd->next >= trd->end) {  /* if no more characters available */
    size_t n = fread(trd->buf, sizeof(char), TRD_BUFSIZE, trd->file);
    if (n <= 0) return ferror(trd->file) ? TRD_ERR : TRD_EOF;
    trd->next = trd->buf;       /* read a new block from the file */
    trd->end  = trd->buf +n;    /* set pointer to next character */
  }                             /* and to the end of the buffer */
  return (unsigned char)*trd->next++;
}  /* trd_getc() */             /* return the next character */

/*--------------------------------------------------------------------*/

int trd_ungetc (TABREAD *trd, int c)
{                               /* --- push back a character */
  assert(trd);                  /* check the function arguments */
  return (trd->next > trd->buf) ? *--trd->next = (char)c : EOF;
}  /* trd_ungetc() */

/*--------------------------------------------------------------------*/

int trd_read (TABREAD *trd)
{                               /* --- read the next table field */
  int  c, d;                    /* character read, delimiter type */
  char *p, *e;                  /* to traverse the field */

  /* --- initialize --- */
  assert(trd && trd->file);     /* check the function arguments */
  trd->pos = (trd->delim == TRD_FLD) ? trd->pos+1 : 1;
  trd->field[trd->len = 0] = 0; /* clear the current field */
  GETC(trd, c, TRD_EOF);        /* get the first character */

  /* --- skip comment records --- */
  if (trd->delim != TRD_FLD) {  /* if at the start of a record */
    while (iscomment(c)) {      /* while the record is a comment */
      while (!isrecsep(c))      /* while not at end of record, */
        GETC(trd, c, TRD_EOF);  /* get the next character */
      trd->rec++;               /* count the comment record */
      GETC(trd, c, TRD_EOF);    /* get the first character */
    }                           /* after the comment record */
  }                             /* (comment records are skipped) */

  /* --- skip leading blanks --- */
  while (isblank(c))            /* while the character is blank, */
    GETC(trd, c, TRD_REC);      /* get the next character */
  if (issep(c)) {               /* check for field/record separator */
    trd->last = c;              /* store the last character read */
    if (isfldsep(c)) return trd->delim = TRD_FLD;
    trd->rec++;      return trd->delim = TRD_REC;
  }                             /* if at end of record, count record */
  /* Note that after at least one valid character was read, even  */
  /* if it is a blank, the end of file/input is translated into a */
  /* record separator. EOF is returned only if no character could */
  /* be read before the end of file/input is encountered.         */

  /* --- read the field --- */
  p = trd->field; e = p +TRD_MAXLEN;
  while (1) {                   /* field read loop */
    if (p < e) *p++ = (char)c;  /* append the last character */
    c = trd_getc(trd);          /* and get the next character */
    if (c < 0)    { d = (c <= TRD_ERR) ? TRD_ERR : TRD_REC; break; }
    if (issep(c)) { d = (isfldsep(c))  ? TRD_FLD : TRD_REC; break; }
  }                             /* while character is no separator */
  trd->last = c;                /* store the last character read */

  /* --- remove trailing blanks --- */
  while (isblank(*--p));        /* skip blank characters at the end */
  *++p = '\0';                  /* and terminate the current field */
  trd->len = (size_t)(p -trd->field); /* store number of characters */

  /* --- check for a null value --- */
  while (--p >= trd->field)     /* check for only null value chars. */
    if (!isnull((unsigned char)*p)) break;
  if (p < trd->field)           /* clear field if null value */
    trd->field[trd->len = 0] = 0;

  /* --- check for end of line --- */
  if (d != TRD_FLD) {           /* if not at a field separator */
    if (d == TRD_REC) trd->rec++;
    return trd->delim = d;      /* if at end of record, count record, */
  }                             /* and then abort the function */

  /* --- skip trailing blanks --- */
  while (isblank(c)) {          /* while character is blank, */
    trd->last = c;              /* note the last character */
    GETC(trd, c, TRD_REC);      /* and get the next character */
  }
  if (isrecsep(c)) {            /* check for a record separator */
    trd->last = c; trd->rec++; return trd->delim = TRD_REC; }
  if (isfldsep(c))              /* note the field separator or */
    trd->last = c;              /* put back last character (may be */
  else trd_ungetc(trd, c);      /* necessary if blank = field sep.) */
  return trd->delim = TRD_FLD;  /* return the delimiter type */
}  /* trd_read() */

/*--------------------------------------------------------------------*/
#ifdef TRD_MAIN

int main (int argc, char* argv[])
{                               /* --- main function for testing */
  int     d;                    /* delimiter of current field */
  TABREAD *trd;                 /* table reader for testing */

  if (argc < 2) {               /* if no arguments given, abort */
    printf("usage: %s file [x]\n", argv[0]);   return  0; }
  trd = trd_create();           /* create a table reader */
  if (!trd) { printf("not enough memory\n");   return -1; }
  if (trd_open(trd, NULL, argv[1]) != 0) {
    printf("cannot open %s\n", trd_name(trd)); return -1; }
  do {                          /* file read loop */
    d = trd_read(trd);          /* print delimiter and field */
    if (argc > 2) printf("% d : >%s<\n", d, trd_field(trd));
  } while (d >= 0);             /* while not at end of file */
  if (d <= TRD_ERR) {           /* check for a read error */
    printf("file %s:%"SIZE_FMT"(%"SIZE_FMT"): read error at '%s'\n",
           TRD_INFO(trd)); return -1; }
  trd_delete(trd, 1);           /* delete the table reader */
  return 0;                     /* return 'ok' */
}  /* main() */

#endif
