/*----------------------------------------------------------------------
  File    : escape.c
  Contents: escape character handling
  Author  : Christian Borgelt
  History : 2010.08.22 file created from tabread.c
            2010.10.07 function esc_decode() changed to strtod() style
            2010.10.08 function esc_tabchs() added (table output chars)
            2013.03.07 type of size-related types changed to size_t
----------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "escape.h"

/*----------------------------------------------------------------------
  Constants
----------------------------------------------------------------------*/
static const int esc_len[256] = {
/* 00  NUL     SOH     STX     ETX     EOT     ENQ     ACK     BEL */
       4,      4,      4,      4,      4,      4,      4,      2,
/* 08  BS      HT      LF      VT      FF      CR      SO      SI  */
       2,      2,      2,      2,      2,      2,      4,      4,
/* 10  DLE     DC1     DC2     DC3     DC4     NAK     SYN     ETB */
       4,      4,      4,      4,      4,      4,      4,      4,
/* 18  CAN     EM      SUB     ESC     FS      GS      RS      US  */
       4,      4,      4,      4,      4,      4,      4,      4,
/* 20  ' '     '!'     '"'     '#'     '$'     '%'     '&'     ''' */
       1,      1,      1,      1,      1,      1,      1,      1,
/* 28  '('     ')'     '*'     '+'     ','     '-'     '.'     '/' */
       1,      1,      1,      1,      1,      1,      1,      1,
/* 30  '0'     '1'     '2'     '3'     '4'     '5'     '6'     '7' */
       1,      1,      1,      1,      1,      1,      1,      1,
/* 38  '8'     '9'     ':'     ';'     '<'     '='     '>'     '?' */
       1,      1,      1,      1,      1,      1,      1,      1,
/* 40  '@'     'A'     'B'     'C'     'D'     'E'     'F'     'G' */
       1,      1,      1,      1,      1,      1,      1,      1,
/* 48  'H'     'I'     'J'     'K'     'L'     'M'     'N'     'O' */
       1,      1,      1,      1,      1,      1,      1,      1,
/* 50  'P'     'Q'     'R'     'S'     'T'     'U'     'V'     'W' */
       1,      1,      1,      1,      1,      1,      1,      1,
/* 58  'X'     'Y'     'Z'     '['     '\'     ']'     '^'     '_' */
       1,      1,      1,      1,      1,      1,      1,      1,
/* 60  '`'     'a'     'b'     'c'     'd'     'e'     'f'     'g' */
       1,      1,      1,      1,      1,      1,      1,      1,
/* 68  'h'     'i'     'j'     'k'     'l'     'm'     'n'     'o' */
       1,      1,      1,      1,      1,      1,      1,      1,
/* 70  'p'     'q'     'r'     's'     't'     'u'     'v'     'w' */
       1,      1,      1,      1,      1,      1,      1,      1,
/* 78  'x'     'y'     'z'     '{'     '|'     '}'     '~'     DEL */
       1,      1,      1,      1,      1,      1,      1,      4,
       1,      1,      1,      1,      1,      1,      1,      1,
       1,      1,      1,      1,      1,      1,      1,      1,
       1,      1,      1,      1,      1,      1,      1,      1,
       1,      1,      1,      1,      1,      1,      1,      1,
       1,      1,      1,      1,      1,      1,      1,      1,
       1,      1,      1,      1,      1,      1,      1,      1,
       1,      1,      1,      1,      1,      1,      1,      1,
       1,      1,      1,      1,      1,      1,      1,      1,
       1,      1,      1,      1,      1,      1,      1,      1,
       1,      1,      1,      1,      1,      1,      1,      1,
       1,      1,      1,      1,      1,      1,      1,      1,
       1,      1,      1,      1,      1,      1,      1,      1,
       1,      1,      1,      1,      1,      1,      1,      1,
       1,      1,      1,      1,      1,      1,      1,      1,
       1,      1,      1,      1,      1,      1,      1,      1,
       1,      1,      1,      1,      1,      1,      1,      1 };

const char* esc_map[256] = {
/* 00  NUL     SOH     STX     ETX     EOT     ENQ     ACK     BEL */
       "\\x00","\\x01","\\x02","\\x03","\\x04","\\x05","\\x06","\\a",
/* 08  BS      HT      LF      VT      FF      CR      SO      SI  */
       "\\b",  "\\t",  "\\n",  "\\v",  "\\f",  "\\r",  "\\x0e","\\x0f",
/* 10  DLE     DC1     DC2     DC3     DC4     NAK     SYN     ETB */
       "\\x10","\\x11","\\x12","\\x13","\\x14","\\x15","\\x16","\\x17",
/* 18  CAN     EM      SUB     ESC     FS      GS      RS      US  */
       "\\x18","\\x19","\\x1a","\\x1b","\\x1c","\\x1d","\\x1e","\\x1f",
/* 20  ' '     '!'     '"'     '#'     '$'     '%'     '&'     ''' */
       " ",    "!",    "\"",   "#",    "$",    "%",    "&",    "'",
/* 28  '('     ')'     '*'     '+'     ','     '-'     '.'     '/' */
       "(",    ")",    "*",    "+",    ",",    "-",    ".",    "/",
/* 30  '0'     '1'     '2'     '3'     '4'     '5'     '6'     '7' */
       "0",    "1",    "2",    "3",    "4",    "5",    "6",    "7",
/* 38  '8'     '9'     ':'     ';'     '<'     '='     '>'     '?' */
       "8",    "9",    ":",    ";",    "<",    "=",    ">",    "?",
/* 40  '@'     'A'     'B'     'C'     'D'     'E'     'F'     'G' */
       "@",    "A",    "B",    "C",    "D",    "E",    "F",    "G",
/* 48  'H'     'I'     'J'     'K'     'L'     'M'     'N'     'O' */
       "H",    "I",    "J",    "K",    "L",    "M",    "N",    "O",
/* 50  'P'     'Q'     'R'     'S'     'T'     'U'     'V'     'W' */
       "P",    "Q",    "R",    "S",    "T",    "U",    "V",    "W",
/* 58  'X'     'Y'     'Z'     '['     '\'     ']'     '^'     '_' */
       "X",    "Y",    "Z",    "[",    "\\",   "]",    "^",    "_",
/* 60  '`'     'a'     'b'     'c'     'd'     'e'     'f'     'g' */
       "`",    "a",    "b",    "c",    "d",    "e",    "f",    "g",
/* 68  'h'     'i'     'j'     'k'     'l'     'm'     'n'     'o' */
       "h",    "i",    "j",    "k",    "l",    "m",    "n",    "o",
/* 70  'p'     'q'     'r'     's'     't'     'u'     'v'     'w' */
       "p",    "q",    "r",    "s",    "t",    "u",    "v",    "w",
/* 78  'x'     'y'     'z'     '{'     '|'     '}'     '~'     DEL */
       "x",    "y",    "z",    "{",    "|",    "}",    "~",    "\\x7f",
       "\x80", "\x81", "\x82", "\x83", "\x84", "\x85", "\x86", "\x87",
       "\x88", "\x89", "\x8a", "\x8b", "\x8c", "\x8d", "\x8e", "\x8f",
       "\x90", "\x91", "\x92", "\x93", "\x94", "\x95", "\x96", "\x97",
       "\x98", "\x99", "\x9a", "\x9b", "\x9c", "\x9d", "\x9e", "\x9f",
       "\xa0", "\xa1", "\xa2", "\xa3", "\xa4", "\xa5", "\xa6", "\xa7",
       "\xa8", "\xa9", "\xaa", "\xab", "\xac", "\xad", "\xae", "\xaf",
       "\xb0", "\xb1", "\xb2", "\xb3", "\xb4", "\xb5", "\xb6", "\xb7",
       "\xb8", "\xb9", "\xba", "\xbb", "\xbc", "\xbd", "\xbe", "\xbf",
       "\xc0", "\xc1", "\xc2", "\xc3", "\xc4", "\xc5", "\xc6", "\xc7",
       "\xc8", "\xc9", "\xca", "\xcb", "\xcc", "\xcd", "\xce", "\xcf",
       "\xd0", "\xd1", "\xd2", "\xd3", "\xd4", "\xd5", "\xd6", "\xd7",
       "\xd8", "\xd9", "\xda", "\xdb", "\xdc", "\xdd", "\xde", "\xdf",
       "\xe0", "\xe1", "\xe2", "\xe3", "\xe4", "\xe5", "\xe6", "\xe7",
       "\xe8", "\xe9", "\xea", "\xeb", "\xec", "\xed", "\xee", "\xef",
       "\xf0", "\xf1", "\xf2", "\xf3", "\xf4", "\xf5", "\xf6", "\xf7",
       "\xf8", "\xf9", "\xfa", "\xfb", "\xfc", "\xfd", "\xfe", "\xff" };

/*----------------------------------------------------------------------
  Functions
----------------------------------------------------------------------*/

int esc_decode (const char *s, char **end)
{                               /* --- decode one character */
  char c, *p;                   /* character and dummy end pointer */
  int  x;                       /* character code */

  assert(s);                    /* check the function arguments */
  if (!end) end = &p;           /* ensure a valid end pointer */
  if (!*s) {                    /* if at the end of the string, */
    *end = (char*)s; return -1; }         /* abort with failure */
  c = *s++;                     /* get the next character */
  if (c != '\\') {              /* if character is not quoted, */
    *end = (char*)s; return  c; }          /* simply return it */
  c = *s++;                     /* get the next character */
  *end = (char*)s;              /* and store the new end */
  switch (c) {                  /* evaluate the character */
    case 'a': return '\a';      /* 0x07 (BEL) */
    case 'b': return '\b';      /* 0x08 (BS)  */
    case 'f': return '\f';      /* 0x0c (FF)  */
    case 'n': return '\n';      /* 0x0a (NL)  */
    case 'r': return '\r';      /* 0x0d (CR)  */
    case 't': return '\t';      /* 0x09 (HT)  */
    case 'v': return '\v';      /* 0x0b (VT)  */
    case '0': case '1': case '2': case '3':
    case '4': case '5': case '6': case '7':
      x = c -'0';               /* --- octal character code */
      c = *s;                   /* get the next character */
      if ((c >= '0') && (c <= '7')) x = (x << 3) +c -'0';
      else return x;            /* decode second digit */
      c = *++s; *end = (char*)s;/* get the next character */
      if ((c >= '0') && (c <= '7')) x = (x << 3) +c -'0';
      else return x;            /* decode third digit */
      *end = (char*)(s+1);      /* consume the decoded character */
      return x & 0xff;          /* and return the character code */
    case 'x':                   /* --- hexadecimal character code */
      c = *s;                   /* get the next character */
      if      ((c >= '0') && (c <= '9')) x = c -'0';
      else if ((c >= 'a') && (c <= 'f')) x = c -'a' +10;
      else if ((c >= 'A') && (c <= 'F')) x = c -'A' +10;
      else return 'x';          /* decode first digit */
      c = *++s; *end = (char*)s;/* get the next character */
      if      ((c >= '0') && (c <= '9')) x = (x << 4) +c -'0';
      else if ((c >= 'a') && (c <= 'f')) x = (x << 4) +c -'a' +10;
      else if ((c >= 'A') && (c <= 'F')) x = (x << 4) +c -'A' +10;
      else return x;            /* decode second digit */
      *end = (char*)(s+1);      /* consume the decoded character */
      return x;                 /* and return the character code */
    default:                    /* non-function characters */
      if (!*s) return '\\';     /* return backslash at string end */
      *end = (char*)(s+1);      /* consume the quoted character */
      return (unsigned char)*s; /* and return it */
  }
}  /* esc_decode() */

/*--------------------------------------------------------------------*/

size_t esc_enclen (const char *s)
{                               /* --- get length of encoded sequence */
  size_t n = 0;                 /* total string length */

  assert(s);                    /* check the function arguments */
  while (*s)                    /* sum the string lengths */
    n += (size_t)esc_len[(unsigned int)*s++];
  return n;                     /* return the total length */
}  /* esc_enclen() */

/*--------------------------------------------------------------------*/

size_t esc_encstr (char *dst, const char *src)
{                               /* --- encode a string */
  char *d = dst;                /* to traverse the destination */

  assert(dst && src);           /* check the function arguments */
  while (*src) {                /* traverse the source string */
    strcpy(dst, esc_map[(unsigned char)*src++]);
    while (*++d);               /* append encoding of a character */
  }                             /* and skip this encoding */
  return (size_t)(d -dst);      /* return the destination length */
}  /* esc_encstr() */

/*--------------------------------------------------------------------*/

size_t esc_decstr (char *dst, const char *src)
{                               /* --- decode a string */
  char *s = (char*)src;         /* to traverse the source */
  char *d = dst;                /* to traverse the destination */

  assert(dst && src);           /* check the function arguments */
  while (*s)                    /* decode the characters in src */
    *d++ = (char)esc_decode(s, &s);
  *d = '\0';                    /* terminate the destination */
  return (size_t)(d -dst);      /* return the destination length */
}  /* esc_decstr() */
