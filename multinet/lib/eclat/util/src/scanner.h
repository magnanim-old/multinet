/*----------------------------------------------------------------------
  File    : scanner.h
  Contents: scanner management (lexical analysis of a character stream)
  Author  : Christian Borgelt
  History : 1996.01.16 file created
            1996.02.21 definition of BUFSIZE made global
            1996.03.17 special tokens for keywords removed
            1998.02.08 recover and error message functions added
            1998.02.13 token T_RGT (right arrow '->') added
            1998.03.04 definitions of T_ID and T_NUM exchanged
            1998.04.17 token T_LFT (left  arrow '<-') added
            1998.05.27 token T_CMP (two char comparison operator) added
            2000.11.23 functions scn_fmtlen() and scn_format() added
            2001.07.15 scanner made an object (storing token etc.)
            2006.02.02 token T_DASH (undirected edge '--') added
            2010.11.17 functions scn_open() and scn_close() added
            2011.01.04 (parse) error reporting functions (re)added
            2011.07.27 function scn_first() added (cond. scn_next())
            2011.07.28 macros SCN_NUMID() and SCN_ERRVAL() added
            2011.12.16 "do {  } while (0)" added to many macros
            2013.03.20 sizes and lengths changed to type size_t
----------------------------------------------------------------------*/
#ifndef __SCANNER__
#define __SCANNER__
#include <stdio.h>

/*----------------------------------------------------------------------
  Preprocessor Definitions
----------------------------------------------------------------------*/
#ifdef SCN_SCAN
#define CCHAR   const char      /* abbreviation */

/* --- sizes --- */
#define SCN_BUFSIZE  65536      /* size of internal read buffer */
#define SCN_MAXLEN    1024      /* maximum length of a token */

/* --- tokens --- */
#define T_EOF        256        /* end of file */
#define T_NUM        257        /* number (floating point) */
#define T_ID         258        /* identifier or string */
#define T_RGT        259        /* right arrow '->' */
#define T_LFT        260        /* left  arrow '<-' */
#define T_DASH       261        /* dash '--' */
#define T_CMP        262        /* two char. comparison, e.g. '<=' */
#define T_INIT       263        /* initial token (before reading) */

/* --- error codes --- */
#define E_NONE         0        /* no error */
#define E_NOMEM      (-1)       /* not enough memory */
#define E_FOPEN      (-2)       /* cannot open file */
#define E_FREAD      (-3)       /* read error on file */
#define E_FWRITE     (-4)       /* write error on file */
#define E_STDIN      (-5)       /* double assignment of stdin */
#define E_CHAR       (-6)       /* invalid character */
#define E_BUFOVF     (-7)       /* scan buffer overflow */
#define E_UNTSTR     (-8)       /* unterminated string */
#define E_UNTCOM     (-9)       /* unterminated comment */
#define E_STATE     (-10)       /* invalid scanner state */
#define E_GARBAGE   (-11)       /* garbage at end of file */
#define E_CHREXP    (-12)       /* character expected */
#define E_STREXP    (-13)       /* string expected */
#define E_NUMEXP    (-14)       /* number expected */
#define E_NUMBER    (-15)       /* invalid number */

#define SCN_FPOS(s)  scn_name(s), scn_line(s)
#define SCN_INFO(s)  scn_name(s), scn_line(s), scn_value(s)

#endif  /* #ifdef SCN_SCAN */
/*----------------------------------------------------------------------
  Type Definitions
----------------------------------------------------------------------*/
#ifdef SCN_SCAN
typedef struct {                /* --- scanner --- */
  FILE   *file;                 /* file to scan */
  CCHAR  *name;                 /* name of the file to scan */
  int    token;                 /* current token */
  int    len;                   /* current token value length */
  size_t line;                  /* current input line of file */
  size_t start;                 /* start line of comment */
  int    back;                  /* flag for backward step */
  int    ptoken, plen;          /* buffers for length, token */
  size_t pline;                 /* buffer  for line */
  char   *value;                /* token value (points to tvs[0/1]) */
  char   *next;                 /* next character to read */
  char   *end;                  /* current end of the buffer */
  CCHAR  **msgs;                /* array  of error messages */
  int    msgcnt;                /* number of error messages */
  FILE   *errfile;              /* output file for error messages */
  CCHAR  *errname;              /* name of the error output file */
  char   tvs[2][SCN_MAXLEN+4];  /* buffers for token values */
  char   buf[SCN_BUFSIZE];      /* read buffer */
} SCANNER;                      /* (scanner) */
#endif

/*----------------------------------------------------------------------
  Functions
----------------------------------------------------------------------*/
extern size_t   scn_fmtlen   (const char *s, size_t *len);
extern size_t   scn_format   (char *dst, const char *src, int quotes);

#ifdef SCN_SCAN
extern SCANNER* scn_create   (void);
extern int      scn_delete   (SCANNER *scan, int close);
extern int      scn_open     (SCANNER *scan, FILE *file, CCHAR *name);
extern int      scn_close    (SCANNER *scan);
extern FILE*    scn_file     (SCANNER *scan);
extern CCHAR*   scn_name     (SCANNER *scan);

extern int      scn_getc     (SCANNER *scan);
extern int      scn_ungetc   (SCANNER *scan, int c);

extern int      scn_first    (SCANNER *scan);
extern int      scn_next     (SCANNER *scan);
extern int      scn_back     (SCANNER *scan);

extern int      scn_token    (SCANNER *scan);
extern CCHAR*   scn_value    (SCANNER *scan);
extern int      scn_len      (SCANNER *scan);
extern int      scn_line     (SCANNER *scan);
extern int      scn_eof      (SCANNER *scan, int report);
extern CCHAR*   scn_errmsg   (SCANNER *scan, char *buf, size_t size);

extern void     scn_setmsgs  (SCANNER *scan, const char *msgs[], int n);
extern int      scn_erropen  (SCANNER *scan, FILE *file, CCHAR *name);
extern int      scn_errclose (SCANNER *scan);
extern FILE*    scn_errfile  (SCANNER *scan);
extern CCHAR*   scn_errname  (SCANNER *scan);
extern int      scn_error    (SCANNER *scan, int code, ...);

extern int      scn_recover  (SCANNER *scan,
                              int stop, int beg, int end, int level);

/*----------------------------------------------------------------------
  Preprocessor Definitions
----------------------------------------------------------------------*/
#define scn_file(s)     ((s)->file)
#define scn_name(s)     ((CCHAR*)(s)->name)

#define scn_token(s)    ((s)->token)
#define scn_value(s)    ((CCHAR*)(s)->value)
#define scn_len(s)      ((s)->len)
#define scn_line(s)     ((s)->line)

#define scn_errfile(s)  ((s)->errfile)
#define scn_errname(s)  ((s)->errname)

#define SCN_ERROR       return scn_error
#define SCN_ERRVAL(s,c) return scn_error(s, c, scn_value(s))
#define SCN_NEXT(s)     do { if (scn_next(s) < 0) \
                               SCN_ERROR(s, (s)->token); } while (0)
#define SCN_FIRST(s)    do { if (scn_first(s) < 0) \
                               SCN_ERROR(s, (s)->token); } while (0)
#define SCN_CHAR(s,c)   do { if ((s)->token != (c)) \
                               SCN_ERROR(s, E_CHREXP, c); \
                             if (scn_next(s) < 0) \
                               SCN_ERROR(s, (s)->token); } while (0)
#define SCN_NUM(s)      do { if ((s)->token != T_NUM)   \
                               SCN_ERROR(s, E_NUMEXP);   } while (0)
#define SCN_NUMID(s,e)  do { if (((s)->token != T_ID)   \
                             &&  ((s)->token != T_NUM)) \
                               SCN_ERROR(s, e); } while (0)
#define SCN_RECOVER(s,x,a,z,l)  \
                        do { if (scn_recover(s,x,a,z,l) == T_EOF) \
                               return 1; } while (0)

#endif  /* #ifdef SCN_SCAN */
#endif  /* #ifdef __SCANNER__ */
