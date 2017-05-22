/*----------------------------------------------------------------------
  File    : scanner.c
  Contents: scanner management (lexical analysis of a character stream)
  Author  : Christian Borgelt
  History : 1996.01.16 file created
            1996.02.21 identifier recognition made more flexible
            1996.03.17 keyword tokens removed (generalized to T_ID)
            1996.04.15 duplicate state removed from scn_next()
            1997.07.29 '<' and '>' declared active (for decision trees)
            1997.09.08 escape sequences in strings made possible
            1997.09.11 single characters stored also in scn->value
            1998.02.08 recover and error message functions added
            1998.02.09 bug in state S_NUMPT concerning "-." removed
            1998.02.13 token T_RGT ('->') added (for directed graphs)
            1998.03.04 returned tokens changed for some states
            1998.04.17 token T_LFT ('<-') added (for association rules)
            1998.05.27 token T_CMP (two char comparison operator) added
            1998.05.31 immediate token conversion to number removed
            1999.02.08 reading from standard input made possible
            1999.04.29 quoted string parsing improved (more quotes)
            1999.11.13 token value length stored in scn->len
            2000.11.23 functions scn_fmtlen() and scn_format() added
            2001.07.15 scanner made an object, state definitions added
            2001.07.16 look ahead functionality added (scn_back())
            2006.02.02 token T_DASH (undirected edge '--') added
            2010.11.17 functions scn_open() and scn_close() added
            2011.01.04 (parse) error reporting functions (re)added
            2011.07.27 function scn_first() added (cond. scn_next())
            2013.03.20 sizes and lengths changed to type size_t
            2013.08.29 error code not set in scn_eof() if reperr == 0
----------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include "scanner.h"
#ifdef STORAGE
#include "storage.h"
#endif

#ifndef SIZE_FMT
#  ifdef _MSC_VER
#  define SIZE_FMT "Iu"
#  else
#  define SIZE_FMT "zu"
#  endif                        /* MSC still does not support C99 */
#endif

#ifdef _MSC_VER
#ifndef snprintf
#define snprintf _snprintf
#endif
#endif                          /* MSC still does not support C99 */

/*----------------------------------------------------------------------
  Preprocessor Definitions
----------------------------------------------------------------------*/
#ifdef SCN_SCAN
/* --- character classes --- */
#define C_INVALID    0          /* invalid character */
#define C_SPACE      1          /* white space, e.g. ' ' '\t' '\n' */
#define C_LETTER     2          /* letter or underscore '_' */
#define C_DIGIT      3          /* digit */
#define C_POINT      4          /* point, '.' */
#define C_SIGN       5          /* sign,  '+' or '-' */
#define C_SLASH      6          /* slash, '/' */
#define C_QUOTE      7          /* quote, e.g. '"' '`' */
#define C_CMPOP      8          /* comparison operator, e.g. '<' */
#define C_ACTIVE     9          /* active characters, e.g. ',' '(' */

/* --- scanner states --- */
#define S_SPACE      0          /* skipping white space */
#define S_ID         1          /* reading identifier */
#define S_NUMDIG     2          /* reading number, digit */
#define S_NUMPT      3          /* reading number, decimal point */
#define S_FRAC       4          /* reading number, digit and point */
#define S_EXPIND     5          /* reading exponent, indicator */
#define S_EXPSGN     6          /* reading exponent, sign */
#define S_EXPDIG     7          /* reading exponent, digit */
#define S_SIGN       8          /* sign read */
#define S_CMPOP      9          /* reading comparison operator */
#define S_STRING    10          /* reading quoted string */
#define S_ESC       11          /* reading escaped character */
#define S_OCT1      12          /* reading octal  number, 1 digit */
#define S_OCT2      13          /* reading octal  number, 2 digits */
#define S_HEX1      14          /* reading hexad. number, 1 digit */
#define S_HEX2      15          /* reading hexad. number, 2 digits */
#define S_SLASH     16          /* slash read */
#define S_CPPCOM    17          /* reading C++ comment */
#define S_CCOM1     18          /* reading C comment */
#define S_CCOM2     19          /* reading C comment, possible end */
#define S_CCOM3     20          /* reading C comment, possible start */

#endif
/*----------------------------------------------------------------------
  Constants
----------------------------------------------------------------------*/
static const char scftab[256] = {  /* scanable form classes */
        /* NUL  SOH  STX  ETX  EOT  ENQ  ACK  BEL */
/* 00 */    2,   2,   2,   2,   2,   2,   2,  'a',
        /*  BS   HT   LF   VT   FF   CR   SO   SI */
           'b', 't', 'n', 'v', 'f', 'r',  2,   2,
        /* DLE  DC1  DC2  DC3  DC4  NAK  SYN  ETB */
/* 10 */    2,   2,   2,   2,   2,   2,   2,   2,
        /* CAN   EM  SUB  ESC   FS   GS   RS   US */
            2,   2,   2,   2,   2,   2,   2,   2,
        /* ' '  '!'  '"'  '#'  '$'  '%'  '&'  ''' */
/* 20 */    1,   1,  '"',  1,   1,   1,   1,   1,
        /* '('  ')'  '*'  '+'  ','  '-'  '.'  '/' */
            1,   1,   1,   0,   1,   0,   0,   1,
        /* '0'  '1'  '2'  '3'  '4'  '5'  '6'  '7' */
/* 30 */    0,   0,   0,   0,   0,   0,   0,   0,
        /* '8'  '9'  ':'  ';'  '<'  '='  '>'  '?' */
            0,   0,   1,   1,   1,   1,   1,   1,
        /* '@'  'A'  'B'  'C'  'D'  'E'  'F'  'G' */
/* 40 */    1,   0,   0,   0,   0,   0,   0,   0,
        /* 'H'  'I'  'J'  'K'  'L'  'M'  'N'  'O' */
            0,   0,   0,   0,   0,   0,   0,   0,
        /* 'P'  'Q'  'R'  'S'  'T'  'U'  'V'  'W' */
/* 50 */    0,   0,   0,   0,   0,   0,   0,   0,
        /* 'X'  'Y'  'Z'  '['  '\'  ']'  '^'  '_' */
            0,   0,   0,   1, '\\',  1,   1,   0,
        /* '`'  'a'  'b'  'c'  'd'  'e'  'f'  'g' */
/* 60 */    1,   0,   0,   0,   0,   0,   0,   0,
        /* 'h'  'i'  'j'  'k'  'l'  'm'  'n'  'o' */
            0,   0,   0,   0,   0,   0,   0,   0,
        /* 'p'  'q'  'r'  's'  't'  'u'  'v'  'w' */
/* 70 */    0,   0,   0,   0,   0,   0,   0,   0,
        /* 'x'  'y'  'z'  '{'  '|'  '}'  '~'  DEL */
            0,   0,   0,   1,   1,   1,   1,   2,
/* 80 */    1,   1,   1,   1,   1,   1,   1,   1,
            1,   1,   1,   1,   1,   1,   1,   1,
/* 90 */    1,   1,   1,   1,   1,   1,   1,   1,
            1,   1,   1,   1,   1,   1,   1,   1,
/* a0 */    1,   1,   1,   1,   1,   1,   1,   1,
            1,   1,   1,   1,   1,   1,   1,   1,
/* b0 */    1,   1,   1,   1,   1,   1,   1,   1,
            1,   1,   1,   1,   1,   1,   1,   1,
/* c0 */    1,   1,   1,   1,   1,   1,   1,   1,
            1,   1,   1,   1,   1,   1,   1,   1,
/* d0 */    1,   1,   1,   1,   1,   1,   1,   1,
            1,   1,   1,   1,   1,   1,   1,   1,
/* e0 */    1,   1,   1,   1,   1,   1,   1,   1,
            1,   1,   1,   1,   1,   1,   1,   1,
/* f0 */    1,   1,   1,   1,   1,   1,   1,   1,
            1,   1,   1,   1,   1,   1,   1,   1 };

#ifdef SCN_SCAN
static const char ccltab[256] = {  /* character classes */
        /* NUL  SOH  STX  ETX  EOT  ENQ  ACK  BEL */
/* 00 */    0,   0,   0,   0,   0,   0,   0,   0,
        /*  BS   HT   LF   VT   FF   CR   SO   SI */
            0,   1,   1,   1,   1,   1,   0,   0,
        /* DLE  DC1  DC2  DC3  DC4  NAK  SYN  ETB */
/* 10 */    0,   0,   0,   0,   0,   0,   0,   0,
        /* CAN   EM  SUB  ESC   FS   GS   RS   US */
            0,   0,   0,   0,   0,   0,   0,   0,
        /* ' '  '!'  '"'  '#'  '$'  '%'  '&'  ''' */
/* 20 */    1,   8,   7,   9,   9,   9,   9,   7,
        /* '('  ')'  '*'  '+'  ','  '-'  '.'  '/' */
            9,   9,   9,   5,   9,   5,   4,   6,
        /* '0'  '1'  '2'  '3'  '4'  '5'  '6'  '7' */
/* 30 */    3,   3,   3,   3,   3,   3,   3,   3,
        /* '8'  '9'  ':'  ';'  '<'  '='  '>'  '?' */
            3,   3,   9,   9,   8,   8,   8,   9,
        /* '@'  'A'  'B'  'C'  'D'  'E'  'F'  'G' */
/* 40 */    0,   2,   2,   2,   2,   2,   2,   2,
        /* 'H'  'I'  'J'  'K'  'L'  'M'  'N'  'O' */
            2,   2,   2,   2,   2,   2,   2,   2,
        /* 'P'  'Q'  'R'  'S'  'T'  'U'  'V'  'W' */
/* 50 */    2,   2,   2,   2,   2,   2,   2,   2,
        /* 'X'  'Y'  'Z'  '['  '\'  ']'  '^'  '_' */
            2,   2,   2,   9,   9,   9,   9,   2,
        /* '`'  'a'  'b'  'c'  'd'  'e'  'f'  'g' */
/* 60 */    7,   2,   2,   2,   2,   2,   2,   2,
        /* 'h'  'i'  'j'  'k'  'l'  'm'  'n'  'o' */
            2,   2,   2,   2,   2,   2,   2,   2,
        /* 'p'  'q'  'r'  's'  't'  'u'  'v'  'w' */
/* 70 */    2,   2,   2,   2,   2,   2,   2,   2,
        /* 'x'  'y'  'z'  '{'  '|'  '}'  '~'  DEL */
            2,   2,   2,   9,   9,   9,   9,   0,
/* 80 */    0,   0,   0,   0,   0,   0,   0,   0,
            0,   0,   0,   0,   0,   0,   0,   0,
/* 90 */    0,   0,   0,   0,   0,   0,   0,   0,
            0,   0,   0,   0,   0,   0,   0,   0,
/* a0 */    0,   0,   0,   0,   0,   0,   0,   0,
            0,   0,   0,   0,   0,   0,   0,   0,
/* b0 */    0,   0,   0,   0,   0,   0,   0,   0,
            0,   0,   0,   0,   0,   0,   0,   0,
/* c0 */    0,   0,   0,   0,   0,   0,   0,   0,
            0,   0,   0,   0,   0,   0,   0,   0,
/* d0 */    0,   0,   0,   0,   0,   0,   0,   0,
            0,   0,   0,   0,   0,   0,   0,   0,
/* e0 */    0,   0,   0,   0,   0,   0,   0,   0,
            0,   0,   0,   0,   0,   0,   0,   0,
/* f0 */    0,   0,   0,   0,   0,   0,   0,   0,
            0,   0,   0,   0,   0,   0,   0,   0 };

static const char *msgs[] = {   /* error messages */
  /* E_NONE      0 */  "no error",
  /* E_NOMEM    -1 */  "not enough memory",
  /* E_FOPEN    -2 */  "cannot open file %s",
  /* E_FREAD    -3 */  "read error on file %s",
  /* E_FWRITE   -4 */  "write error on file %s",
  /* E_STDIN    -5 */  NULL,
  /* E_CHAR     -6 */  "#invalid character '%c' (0x%02x)",
  /* E_BUFOVF   -7 */  "#scan buffer overflow",
  /* E_UNTSTR   -8 */  "#unterminated string",
  /* E_UNTCOM   -9 */  "#unexpected end of file in comment "
                         "started on line %d",
  /* E_STATE   -10 */  "#invalid scanner state",
  /* E_GARBAGE -11 */  "#garbage at end of file",
  /* E_CHREXP  -12 */  "#'%c' expected instead of '%s'",
  /* E_STREXP  -13 */  "#'%s' expected instead of '%s'",
  /* E_NUMEXP  -14 */  "#number expected instead of '%s'",
  /* E_NUMBER  -15 */  "#invalid number '%s'",
};
#endif  /* #ifdef SCN_SCAN */

/*----------------------------------------------------------------------
  Global Variables
----------------------------------------------------------------------*/
#ifdef SCN_SCAN
static char msgbuf[2*SCN_MAXLEN+64];  /* buffer for error messages */
#endif

/*----------------------------------------------------------------------
  Main Functions
----------------------------------------------------------------------*/

size_t scn_fmtlen (const char *s, size_t *len)
{                               /* --- length of a formatted name */
  size_t n = 0, k = 0;          /* number of (additional) characters */
  size_t q = 0;                 /* quote flag (default: no quotes) */

  assert(s);                    /* check the function arguments */
  while (*s) {                  /* while not at end of name */
    n++;                        /* count character */
    switch (scftab[(unsigned char)*s++]) {
      case  0:                break;
      case  1:         q = 2; break;
      case  2: k += 3; q = 2; break;
      default: k += 1; q = 2; break;
    }                           /* sum additional characters and */
  }                             /* set quote flag (if necessary) */
  if (len) *len = n;            /* store normal length and */
  return n +k +q;               /* return length of scanable form */
}  /* scn_fmtlen() */

/*--------------------------------------------------------------------*/

size_t scn_format (char *dst, const char *src, int quotes)
{                               /* --- format name in scanable form */
  char *d; const char *s;       /* to traverse buffer and name */
  int  c, cls;                  /* character and character class */
  int  t;                       /* temporary buffer */

  assert(dst && src);           /* check the function arguments */
  if (!*src) quotes = 1;        /* an empty name needs quotes */
  if (!quotes) {                /* if quotes are not mandatory, */
    for (s = src; *s; )         /* traverse the string to convert */
      if (scftab[(unsigned char)*s++] != 0) {
        quotes = 1; break; }    /* if a character needs quotes, */
  }                             /* set the quotes flag and abort */
  d = dst;                      /* get the destination and */
  if (quotes) *d++ = '"';       /* store a quote if necessary */
  while (*src) {                /* traverse the characters */
    c   = (unsigned char)*src++;/* get the next character */
    cls = scftab[c];            /* and its character class */
    if      (cls < 2)           /* if it is a normal character, */
      *d++ = (char)c;           /* just store it */
    else if (cls > 2) {         /* if it is an ANSI escape character, */
      *d++ = '\\'; *d++ = (char)cls; }  /* store it as '\c' */
    else {                      /* if it is any other character */
      *d++ = '\\'; *d++ = 'x';
      t = c >> 4;  *d++ = (char)((t > 9) ? (t -10 +'a') : (t +'0'));
      t = c & 0xf; *d++ = (char)((t > 9) ? (t -10 +'a') : (t +'0'));
    }                           /* store the character code */
  }                             /* as a hexadecimal number */
  if (quotes) *d++ = '"';       /* store the closing quote */
  *d = '\0';                    /* and terminate the string */
  return (size_t)(d -dst);      /* return the length of the result */
}  /* scn_format() */

/*--------------------------------------------------------------------*/
#ifdef SCN_SCAN

SCANNER* scn_create (void)
{                               /* --- create a scanner */
  SCANNER *scan;                /* created scanner */

  scan = (SCANNER*)malloc(sizeof(SCANNER));
  if (!scan) return NULL;       /* allocate memory for a scanner */
  scan->file    = NULL;         /* and initialize the fields */
  scan->name    = NULL;
  scan->token   = scan->ptoken = T_INIT;
  scan->len     = scan->plen   = 0;
  scan->line    = scan->pline  = 1;
  scan->start   = 0;
  scan->back    = 0;
  scan->value   = scan->tvs[0];
  scan->next    = scan->end    = scan->buf;
  scan->msgs    = NULL;
  scan->msgcnt  = 0;
  scan->errfile = stderr;
  scan->errname = "<stderr>";
  scan->tvs[0][0] = scan->tvs[1][0] = '\0';
  return scan;                  /* return the created scanner */
}  /* scn_create() */

/*--------------------------------------------------------------------*/

int scn_delete (SCANNER *scan, int close)
{                               /* --- delete a scanner */
  int e, r;                     /* result of fclose() */

  assert(scan);                 /* check the function argument */
  e = (close) ? scn_errclose(scan) : 0;
  r = (close) ? scn_close(scan)    : 0;
  free(scan);                   /* close files and delete scanner */
  return (e) ? e : r;           /* return the result of fclose() */
}  /* scn_delete() */

/*--------------------------------------------------------------------*/

int scn_open (SCANNER *scan, FILE *file, const char *name)
{                               /* --- open a new file */
  assert(scan);                 /* check the function arguments */
  if (file) {                   /* if a file is given directly, */
    if      (name)          scan->name = name;    /* store name */
    else if (file == stdin) scan->name = "<stdin>";
    else                    scan->name = "<unknown>"; }
  else if (!name || !*name) {   /* if no file name is given */
    file = stdin;           scan->name = "<stdin>"; }
  else {                        /* if a proper name is given */
    file = fopen(scan->name = name, "r");
    if (!file) return -2;       /* open file with given name */
  }                             /* and check for an error */
  scan->file  = file;           /* store the new input file and */
  scan->token = scan->ptoken = T_INIT;   /* reset the variables */
  scan->len   = scan->plen   = 0;
  scan->line  = scan->pline  = 1;
  scan->start = 0;
  scan->back  = 0;
  scan->value = scan->tvs[0];
  scan->next  = scan->end    = scan->buf;
  scan->tvs[0][0] = scan->tvs[1][0] = 0;
  return 0;                     /* return 'ok' */
}  /* scn_open() */

/*--------------------------------------------------------------------*/

int scn_close (SCANNER *scan)
{                               /* --- close the current file */
  int r = 0;                    /* result of fclose() */

  assert(scan);                 /* check the function arguments */
  if (scan->file && (scan->file != stdin))
    r = fclose(scan->file);     /* close the current input file */
  scan->file = NULL;            /* clear the file (but keep the name) */
  return r;                     /* return the result of fclose() */
}  /* scn_close() */

/*--------------------------------------------------------------------*/

int scn_getc (SCANNER *scan)
{                               /* --- get the next character */
  int c;                        /* next character */

  assert(scan);                 /* check the function argument */
  if (scan->next >= scan->end){ /* if no more characters available */
    size_t n = fread(scan->buf, sizeof(char), SCN_BUFSIZE, scan->file);
    if (n <= 0) return ferror(scan->file) ? -2 : -1;
    scan->next = scan->buf;     /* read a new block from the file */
    scan->end  = scan->buf +n;  /* set pointer to next character */
  }                             /* and to the end of the buffer */
  c = (unsigned char)*scan->next++;      /* get the next character */
  if (c == '\n') scan->line++;  /* count the line if newline char. */
  return c;                     /* return the next character */
}  /* scn_getc() */

/*--------------------------------------------------------------------*/

int scn_ungetc (SCANNER *scan, int c)
{                               /* --- push back a character */
  assert(scan);                 /* check the function arguments */
  if (c == EOF) return 0;       /* check whether there is space */
  if (scan->next <= scan->buf) return EOF;
  if (c == '\n') scan->line--;  /* step back to previous line */
  return *--scan->next = (char)c;
}  /* scn_ungetc() */           /* return the pushed back character */

/*--------------------------------------------------------------------*/

static int swap (SCANNER *scan)
{                               /* --- swap token information */
  int    t;                     /* swap buffer */
  size_t z;                     /* ditto */

  if (scan->value == scan->tvs[0]) scan->value = scan->tvs[1];
  else                             scan->value = scan->tvs[0];
  t = scan->ptoken; scan->ptoken = scan->token; scan->token = t;
  t = scan->plen;   scan->plen   = scan->len;   scan->len   = t;
  z = scan->pline;  scan->pline  = scan->line;  scan->line  = z;
  return t;                     /* return the new token */
}  /* swap() */

/*--------------------------------------------------------------------*/

int scn_first (SCANNER *scan)
{                               /* --- get first token */
  return (scan->token == T_INIT) ? scn_next(scan) : scan->token;
}  /* scn_first() */

/*--------------------------------------------------------------------*/

int scn_next (SCANNER *scan)
{                               /* --- get next token */
  int  c, ccl;                  /* character and character class */
  int  quote = 0;               /* quote at the start of a string */
  int  ec    = 0;               /* escaped character */
  int  state = 0;               /* state of automaton */
  int  level = 0;               /* comment nesting level */
  char *p;                      /* to traverse the scan buffer */
  char *end;                    /* end of the scan buffer */

  if (scan->back) {             /* if a step backwards has been made, */
    scan->back = 0;             /* clear the corresponding flag, */
    return swap(scan);          /* swap back the token information, */
  }                             /* and return the current token */
  scan->ptoken = scan->token;   /* note the relevant information */
  scan->plen   = scan->len;     /* and swap the buffers for */
  scan->pline  = scan->line;    /* the current token value */
  if (scan->value == scan->tvs[0]) scan->value = p = scan->tvs[1];
  else                             scan->value = p = scan->tvs[0];
  end = p +SCN_MAXLEN -1;       /* get the end of the scan buffer */

  while (1) {                   /* read loop */
    c   = scn_getc(scan);       /* get character and character class */
    ccl = (c < 0) ? EOF : ccltab[c];

    switch (state) {            /* evaluate state of automaton */

      case S_SPACE:             /* --- skip white space */
        switch (ccl) {          /* evaluate character category */
          case C_SPACE : /* do nothing */                   break;
          case C_LETTER: *p++  = (char)c; state = S_ID;     break;
          case C_DIGIT : *p++  = (char)c; state = S_NUMDIG; break;
          case C_POINT : *p++  = (char)c; state = S_NUMPT;  break;
          case C_SIGN  : *p++  = (char)c; state = S_SIGN;   break;
          case C_CMPOP : *p++  = (char)c; state = S_CMPOP;  break;
          case C_QUOTE : quote = c; state = S_STRING;       break;
          case C_SLASH :            state = S_SLASH;        break;
          case C_ACTIVE: *p++  = (char)c; *p = '\0'; scan->len = 1;
                         return scan->token = c;
          case EOF     : strcpy(p, "<eof>");         scan->len = 5;
                         return scan->token = (c <-1) ? E_FREAD : T_EOF;
          default      : *p++  = (char)c; *p = '\0'; scan->len = 1;
                         return scan->token = E_CHAR;
        } break;

      case S_ID:                /* --- identifier (letter read) */
        if ((ccl == C_LETTER)   /* if another letter */
        ||  (ccl == C_DIGIT)    /* or a digit */
        ||  (ccl == C_POINT)    /* or a decimal point */
        ||  (ccl == C_SIGN)) {  /* or a sign follows */
          if (p >= end) return scan->token = E_BUFOVF;
          *p++ = (char)c; break;/* buffer character */
        }                       /* otherwise */
        scn_ungetc(scan, c);    /* put back last character, */
        *p = '\0';              /* terminate string in buffer */
        scan->len = (int)(p -scan->value); /* set string length */
        return scan->token = T_ID;   /* and return 'identifier' */

      case S_NUMDIG:            /* --- number (digit read) */
        if (p < end) *p++ = (char)c;  /* buffer character */
        else return scan->token = E_BUFOVF;
        if  (ccl == C_DIGIT)    /* if another digit follows, */
          break;                /* do nothing */
        if  (ccl == C_POINT) {  /* if a decimal point follows, */
          state = S_FRAC;   break; } /* go to 'fraction' state */
        if ((c == 'e')          /* if an exponent indicator follows */
        ||  (c == 'E')) {       /* (lower- or uppercase), */
          state = S_EXPIND; break; } /* go to 'exponent' state */
        if ((ccl == C_LETTER)   /* if a letter */
        ||  (ccl == C_SIGN)) {  /* or a sign follows, */
          state = S_ID; break;  /* go to 'identifier' state */
        }                       /* otherwise */
        scn_ungetc(scan, c);    /* put back last character, */
        *--p = '\0';            /* terminate string in buffer */
        scan->len = (int)(p -scan->value); /* set string length */
        return scan->token = T_NUM;      /* and return 'number' */

      case S_NUMPT:             /* --- number (point read) */
        if (p < end) *p++ = (char)c;  /* buffer character */
        else return scan->token = E_BUFOVF;
        if  (ccl == C_DIGIT) {       /* if a digit follows, */
          state = S_FRAC; break; }   /* go to 'fraction' state */
        if ((ccl == C_LETTER)   /* if a letter */
        ||  (ccl == C_POINT)    /* or a decimal point */
        ||  (ccl == C_SIGN)) {  /* or a sign follows */
          state = S_ID; break;  /* go to 'identifier' state */
        }                       /* otherwise */
        scn_ungetc(scan, c);    /* put back last character, */
        *--p = '\0';            /* terminate string in buffer */
        scan->len = (int)(p -scan->value); /* set string length */
        return scan->token = T_ID;   /* and return 'identifier' */

      case S_FRAC:              /* --- number (digit & point read) */
        if (p < end) *p++ = (char)c;  /* buffer character */
        else return scan->token = E_BUFOVF;
        if  (ccl == C_DIGIT)    /* if another digit follows, */
          break;                /* do nothing else */
        if ((c == 'e')          /* if an exponent indicator follows, */
        ||  (c == 'E')) {       /* (lower- or uppercase), */
          state = S_EXPIND; break; } /* go to exponent state */
        if ((ccl == C_LETTER)   /* if a letter */
        ||  (ccl == C_POINT)    /* or a decimal point */
        ||  (ccl == C_SIGN)) {  /* or a sign follows, */
          state = S_ID; break;  /* go to 'identifier' state */
        }                       /* otherwise */
        scn_ungetc(scan, c);    /* put back last character, */
        *--p = '\0';            /* terminate string in buffer */
        scan->len = (int)(p -scan->value); /* set string length */
        return scan->token = T_NUM;      /* and return 'number' */

      case S_EXPIND:            /* --- exponent (indicator read) */
        if (p < end) *p++ = (char)c;  /* buffer character */
        else return scan->token = E_BUFOVF;
        if  (ccl == C_SIGN) {        /* if a sign follows, */
          state = S_EXPSGN; break; } /* go to 2nd 'exponent' state */
        if  (ccl == C_DIGIT) {       /* if a digit follows, */
          state = S_EXPDIG; break; } /* go to 3rd 'exponent' state */
        if ((ccl == C_LETTER)   /* if a letter */
        ||  (ccl == C_POINT)) { /* or a decimal point follows */
          state = S_ID; break;  /* go to 'identifier' state */
        }                       /* otherwise */
        scn_ungetc(scan, c);    /* put back last character, */
        *--p = '\0';            /* terminate string in buffer */
        scan->len = (int)(p -scan->value); /* set string length */
        return scan->token = T_ID;   /* and return 'identifier' */

      case S_EXPSGN:            /* --- exponent (sign read) */
        if (p < end) *p++ = (char)c;  /* buffer character */
        else return scan->token = E_BUFOVF;
        if  (ccl == C_DIGIT) {      /* if a digit follows, */
          state = S_EXPDIG; break;} /* do nothing else */
        if ((ccl == C_LETTER)   /* if a letter */
        ||  (ccl == C_POINT)    /* or a decimal point */
        ||  (ccl == C_SIGN)) {  /* or a sign follows */
          state = S_ID; break;  /* go to 'identifier' state */
        }                       /* otherwise */
        scn_ungetc(scan, c);    /* put back last character, */
        *--p = '\0';            /* terminate string in buffer */
        scan->len = (int)(p -scan->value); /* set string length */
        return scan->token = T_ID;   /* and return 'identifier' */

      case S_EXPDIG:            /* --- exponent (digit read) */
        if (p < end) *p++ = (char)c;  /* buffer character */
        else return scan->token = E_BUFOVF;
        if  (ccl == C_DIGIT)    /* if another digit follows, */
          break;                /* do nothing else */
        if ((ccl == C_LETTER)   /* if a letter */
        ||  (ccl == C_POINT)    /* or a decimal point */
        ||  (ccl == C_SIGN)) {  /* or a sign follows, */
          state = S_ID; break;  /* go to 'identifier' state */
        }                       /* otherwise */
        scn_ungetc(scan, c);    /* put back last character, */
        *--p = '\0';            /* terminate string in buffer */
        scan->len = (int)(p -scan->value); /* set string length */
        return scan->token = T_NUM;      /* and return 'number' */

      case S_SIGN:              /* --- number (sign read) */
        *p++ = (char)c;         /* buffer character */
        if  (ccl == C_DIGIT) {       /* if a digit follows, */
          state = S_NUMDIG; break; } /* go to 'number' state */
        if  (ccl == C_POINT) {       /* if a decimal point follows, */
          state = S_NUMPT; break; }  /* go to fraction state */
        if ((c == '-')          /* if a '-' follows and previous */
        &&  (scan->value[0] == '-')) {  /* char was a minus sign */
          *p = '\0'; scan->len = 2; return scan->token = T_DASH; }
        if ((c == '>')          /* if a '>' follows and previous */
        &&  (scan->value[0] == '-')) {  /* char was a minus sign */
          *p = '\0'; scan->len = 2; return scan->token = T_RGT; }
        if ((ccl == C_LETTER)        /* if a letter */
        ||  (ccl == C_SIGN)) {       /* or a sign follows, */
          state = S_ID; break; }     /* go to 'identifier' state */
        scn_ungetc(scan, c);    /* otherwise put back last character, */
        *--p = '\0';            /* terminate string in buffer */
        scan->len = (int)(p -scan->value); /* set string length */
        return scan->token = T_ID;   /* and return 'identifier' */

      case S_CMPOP:             /* --- comparison operator read */
        if ((c == '-')          /* if a minus sign follows and */
        &&  (scan->value[0] == '<')) {  /* prev. char was a '<' */
          *p++ = '-';          scan->token = T_LFT; }
        else if (c == '=') {    /* if an equal sign follows */
          *p++ = '=';          scan->token = T_CMP; }
        else {                  /* if anything else follows */
          scn_ungetc(scan, c); scan->token = scan->value[0]; }
        *p = '\0';              /* terminate string in buffer */
        scan->len = (int)(p -scan->value); /* set string length */
        return scan->token;        /* and return the token read */

      case S_STRING:            /* --- quoted string */
        if ((c == '\n') || (c == EOF))  /* if end of line or file, */
          return scan->token = E_UNTSTR;   /* string is unterminated */
        if (c != quote) {       /* if not at end of string */
          if (p >= end) return scan->token = E_BUFOVF;
          if (c == '\\') {      /* if escaped character follows, */
            state = S_ESC; break; }  /* go to escaped char state */
          *p++ = (char)c; break;     /* otherwise buffer character */
        }                       /* if at end of string, */
        *p = '\0';              /* terminate string in buffer */
        scan->len = (int)(p -scan->value); /* set string length */
        return scan->token = T_ID;   /* and return 'identifier' */

      case S_ESC:               /* --- after '\' in quoted string */
        if ((c >= '0') && (c <= '7')) {        /* if octal digit, */
          ec = c -'0'; state = S_OCT1; break; }/* evaluate digit  */
        if (c == 'x') {         /* if hexadecimal character code, */
          state = S_HEX1; break;} /* go to hexadecimal evaluation */
        switch (c) {            /* evaluate character after '\' */
          case  'a': c = '\a'; break;
          case  'b': c = '\b'; break;
          case  'f': c = '\f'; break;
          case  'n': c = '\n'; break;
          case  'r': c = '\r'; break;
          case  't': c = '\t'; break;
          case  'v': c = '\v'; break;
          case '\n': c = -1;   break;
          default  :           break;
        }                       /* get escaped character and */
        if (c >= 0) *p++ = (char)c;        /* store it, then */
        state = S_STRING; break;/* return to quoted string state */

      case S_OCT1:              /* --- escaped octal number 1 */
        if ((c >= '0')          /* if an octal digit follows, */
        &&  (c <= '7')) {       /* evaluate it */
          ec = ec *8 +c -'0'; state = S_OCT2; break; }
        scn_ungetc(scan, c);    /* otherwise put back last character */
        *p++  = (char)ec;       /* store escaped character and */
        state = S_STRING; break;/* return to quoted string state */

      case S_OCT2:              /* --- escaped octal number 2 */
        if ((c >= '0') || (c <= '7'))
          ec = ec *8 +c -'0';   /* if octal digit, evaluate it */
        else scn_ungetc(scan,c);/* otherwise put back last character */
        *p++  = (char)ec;       /* store escaped character and */
        state = S_STRING; break;/* return to quoted string state */

      case S_HEX1:              /* --- escaped hexadecimal number 1 */
        if (ccl == C_DIGIT) {   /* if hexadecimal digit, evaluate it */
          ec = c -'0';     state = S_HEX2; break; }
        if ((c >= 'a') && (c <= 'f')) {
          ec = c -'a' +10; state = S_HEX2; break; }
        if ((c >= 'A') && (c <= 'F')) {
          ec = c -'A' +10; state = S_HEX2; break; }
        scn_ungetc(scan, c);    /* otherwise put back last character */
        *p++  = 'x';            /* store escaped character ('x') and */
        state = S_STRING; break;/* return to quoted string state */

      case S_HEX2:              /* --- escaped hexadecimal number 2 */
        if (ccl == C_DIGIT)     /* if hexadecimal digit, evaluate it */
          ec = ec*16 +c -'0';
        else if ((c >= 'a') && (c <= 'f'))
          ec = ec*16 +c -'a' +10;
        else if ((c >= 'A') && (c <= 'F'))
          ec = ec*16 +c -'A' +10;
        else scn_ungetc(scan,c);/* otherwise put back last character */
        *p++  = (char)ec;       /* store escaped character and */
        state = S_STRING; break;/* return to quoted string state */

      case S_SLASH:             /* --- slash '/' */
        if (c == '/') {         /* if C++ style comment, then */
          state = S_CPPCOM; break; }   /* skip to end of line */
        if (c == '*') {         /* if C style comment */
          scan->start = scan->line; level = 1;
          state = S_CCOM1; break;    /* note start line, init. level */
        }                       /* and go to first 'comment' state */
        scn_ungetc(scan, c);    /* otherwise put back last character */
        *p++ = '/'; *p = '\0';  /* store character in buffer */
        scan->len = 1;          /* set string length and */
        return scan->token = '/';  /* return 'character' */

      case S_CPPCOM:            /* --- C++ style comment */
        if ((c == '\n')         /* if at end of line */
        ||  (c == EOF))         /* or at end of file */
          state = S_SPACE;      /* return to white space skipping */
        break;                  /* (skip to end of line) */

      case S_CCOM1:             /* --- C style comment 1 */
        if      (c == EOF)      /* if end of file, abort */
          return scan->token = E_UNTCOM;
        if      (c == '*')      /* if possibly 'end of comment', */
          state = S_CCOM2;      /* go to 2nd 'comment' state */
        else if (c == '/')      /* if possibly 'start of comment', */
          state = S_CCOM3;      /* go to 3rd 'comment' state */
        break;

      case S_CCOM2:             /* --- C style comment 2 */
        if      (c == EOF)      /* if end of file, abort */
          return scan->token = E_UNTCOM;
        if      (c == '/') {    /* if end of comment found */
          if (--level <= 0) state = S_SPACE;
          else              state = S_CCOM1; }
        else if (c != '*')      /* if end of comment impossible */
          state = S_CCOM1;      /* return to comment skipping */
        break;                  /* (possible start of comment) */

      case S_CCOM3:             /* --- C style comment 3 */
        if      (c == EOF)      /* if end of file, abort */
          return scan->token = E_UNTCOM;
        if      (c == '*') {    /* if start of comment found */
          level++; state = S_CCOM1; }
        else if (c != '/')      /* if start of comment impossible */
          state = S_CCOM1;      /* return to comment skipping */
        break;                  /* (possible end of comment) */

      default:                  /* if state is invalid, abort */
        return scan->token = E_STATE;

    }  /* switch() */
  }  /* while(1) */
}  /* scn_next() */

/*--------------------------------------------------------------------*/

int scn_back (SCANNER *scan)
{                               /* --- go back one token */
  if (scan->back)               /* a second step backwards */
    return scan->token;         /* is impossible, so do nothing */
  scan->back = -1;              /* set the step backward flag */
  return swap(scan);            /* swap the token information */
}  /* scn_back() */             /* and return the previous token */

/*--------------------------------------------------------------------*/

int scn_eof (SCANNER *scan, int reperr)
{                               /* --- check for end of file */
  if (scan->token == T_EOF)     /* if at the end of the file, */
    return 1;                   /* return 'true' */
  if (reperr) scn_error(scan, scan->token = E_GARBAGE);
  return 0;                     /* report error and return 'false' */
}  /* scn_eof() */

/*--------------------------------------------------------------------*/

const char* scn_errmsg (SCANNER *scan, char *buf, size_t size)
{                               /* --- get message for last error */
  int        i, k = 0;          /* error message index, buffers */
  const char *msg;              /* error message (format) */

  assert(scan                   /* check the function arguments */
  &&    (!buf || (size > 0)));  /* if none given, get internal buffer */
  if (!buf) { buf = msgbuf; size = sizeof(msgbuf); }
  i = (scan->token < 0) ? -scan->token : 0;
  assert(i < (int)(sizeof(msgs)/sizeof(*msgs)));
  msg = msgs[i];                /* get the error message format */
  assert(msg);                  /* check for a proper message */
  if (*msg == '#') { msg++;     /* if message needs a header */
    k = snprintf(buf, size, "%s:%"SIZE_FMT": ", scan->name, scan->line);
    if (k >= (int)size) k = (int)(size-1);
  }                             /* print file name and line number */
  if (scan->token == E_UNTCOM)  /* if 'unterminated comment' */
    snprintf(buf+k, size-(size_t)k, msg, scan->start);
  else {                        /* if any other error */
    i = (int)scan->value[0];    /* get first token value character */
    snprintf(buf+k, size-(size_t)k, msg, (char)i, i);
  }                             /* format the error message */
  return buf;                   /* return the message buffer */
}  /* scn_errmsg() */

/*--------------------------------------------------------------------*/

void scn_setmsgs (SCANNER *scan, const char *msgs[], int n)
{                               /* --- set additional error messages */
  assert(scan && (msgs || (n <= 0)));/* check the function arguments */
  scan->msgs   = msgs;          /* store the error messages */
  scan->msgcnt = n;             /* and their number */
}  /* scn_setmsgs() */

/*--------------------------------------------------------------------*/

int scn_erropen (SCANNER *scan, FILE *file, const char *name)
{                               /* --- open/set the error output file */
  assert(scan);                 /* check the function arguments */
  if (file) {                   /* if a file is given directly, */
    if (name)                scan->errname = name;/* store name */
    else if (file == stderr) scan->errname = "<stderr>";
    else if (file == stdout) scan->errname = "<stdout>";
    else                     scan->errname = "<unknown>"; }
  else if (! name) {            /* if no name is given */
    file = NULL;             scan->errname = "<none>"; }
  else if (!*name) {            /* if an empty name is given */
    file = stderr;           scan->errname = "<stderr>"; }
  else {                        /* if a proper name is given */
    file = fopen(scan->errname = name, "w");
    if (!file) return -2;       /* open file with given name */
  }                             /* and check for an error */
  scan->errfile = file;         /* store the new error file */
  return 0;                     /* return 'ok' */
}  /* scn_erropen() */

/*--------------------------------------------------------------------*/

int scn_errclose (SCANNER *scan)
{                               /* --- close the error output file */
  int r;                        /* result of fclose()/fflush() */

  assert(scan);                 /* check the function arguments */
  if (!scan->errfile) return 0; /* check for an output file */
  r = ((scan->errfile == stderr) || (scan->errfile == stdout))
    ? fflush(scan->errfile) : fclose(scan->errfile);
  scan->errfile = NULL;         /* close the error output file */
  return r;                     /* return the result of fclose() */
}  /* scn_errclose() */

/*--------------------------------------------------------------------*/

int scn_error (SCANNER *scan, int code, ...)
{                               /* --- report a scan/parse error */
  int        i;                 /* error message index */
  const char *msg;              /* error message (format) */
  va_list    args;              /* variable arguments */

  assert(scan);                 /* check the function arguments */
  if (scan->errfile && code) {  /* if an error file is set */
    va_start(args, code);       /* start variable argument evaluation */
    if      (code >  0)         /* if a message is given, print it */
      fputs(va_arg(args, char*), scan->errfile);
    else if (code >= E_GARBAGE){/* if basic scanner error */
      msg = msgs[i = (code < 0) ? -code : 0];
      assert(msg);              /* get the error message (format) */
      fputc('\n',scan->errfile);/* start a new output line */
      if (*msg == '#') { msg++; /* print a message header if needed */
        fprintf(scan->errfile, "%s:%"SIZE_FMT": ",
                scan->name, scan->line); }
      if (code == E_UNTCOM)     /* if 'unterminated comment' */
        fprintf(scan->errfile, msg, scan->start);
      else {                    /* if any other error */
        i = (unsigned char)scan->value[0];
        fprintf(scan->errfile, msg, (char)i, i);
      } }                       /* print the error message */
    else if (code >= E_NUMBER){ /* if basic parser error */
      msg = msgs[i = -code];    /* get the error message (format) */
      fputc('\n',scan->errfile);/* start a new output line */
      if (*msg == '#') { msg++; /* print a message header if needed */
        fprintf(scan->errfile, "%s:%"SIZE_FMT": ",
                scan->name, scan->line); }
      if      (code == E_CHREXP)/* if specific character expected */
        fprintf(scan->errfile, msg, va_arg(args, int  ), scan->value);
      else if (code == E_STREXP)/* if specific string expected */
        fprintf(scan->errfile, msg, va_arg(args, char*), scan->value);
      else {                    /* if number (format) error */
        fprintf(scan->errfile, msg, scan->value);
      } }                       /* print the error message */
    else {                      /* if externally defined error */
      i   = -code;              /* get the error message (format) */
      msg = (i < scan->msgcnt) ? scan->msgs[i] : NULL;
      if (!msg) msg = msgs[(int)(sizeof(msgs)/sizeof(*msgs))-1];
      fputc('\n',scan->errfile);/* start a new output line */
      if (*msg == '#') { msg++; /* print a message header if needed */
        fprintf(scan->errfile, "%s:%"SIZE_FMT": ",
                scan->name, scan->line); }
      vfprintf(scan->errfile, msg, args);
    }                           /* print the error message */
    va_end(args);               /* end variable argument evaluation */
    fflush(scan->errfile);      /* flush the error output file */
  }                             /* to ensure a printed message */
  return -abs(code);            /* return the error code */
}  /* scn_error() */

/*--------------------------------------------------------------------*/

int scn_recover (SCANNER *scan, int stop, int beg, int end, int level)
{                               /* --- recover from an error */
  assert(scan);                 /* check the function arguments */
  while ((scan->token != stop)     /* while at stop token */
  &&     (scan->token != T_EOF)) { /* and not at end of file */
    if       (scan->token == beg)  /* if begin level token found, */
      level++;                     /* increment level counter */
    else if ((scan->token == end)  /* if end level token found */
    &&       (--level     <= 0))   /* and on level to return to, */
      break;                       /* abort loop */
    if (scn_next(scan) < 0) return scan->token;
  }                             /* consume token */
  if (scan->token != T_EOF)     /* if not at end of file, */
    scn_next(scan);             /* consume token (stop or end) */
  return scan->token;           /* return the next token */
}  /* scn_recover() */

#endif
