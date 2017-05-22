/*----------------------------------------------------------------------
  File    : report.c
  Contents: item set reporter management
  Author  : Christian Borgelt
  History : 2008.08.18 item set reporter created in tract.[ch]
            2008.08.30 handling of perfect extensions completed
            2008.09.01 handling of closed and maximal item sets added
            2008.09.08 functions isr_intout() and isr_numout() added
            2008.10.30 transaction identifier reporting added
            2008.10.31 item set reporter made a separate module
            2008.11.01 optional double precision support added
            2008.12.05 bug handling real-valued support fixed (_report)
            2009.10.15 counting of reported item sets added
            2010.02.11 closed/maximal item set filtering added
            2010.02.12 bugs in prefix tree handling fixed (clomax)
            2010.03.09 bug in reporting the empty item set fixed
            2010.03.11 filtering of maximal item sets improved
            2010.03.17 head union tail pruning for maximal sets added
            2010.03.18 parallel item set support and weight reporting
            2010.04.07 extended information reporting functions removed
            2010.07.01 correct output of infinite float values added
            2010.07.02 order of closed/maximal and size filtering fixed
            2010.07.04 bug in isr_report() fixed (closed set filtering)
            2010.07.12 null output file made possible (for benchmarking)
            2010.07.19 bug in function isr_report() fixed (clomax)
            2010.07.21 early closed/maximal repository pruning added
            2010.07.22 adapted to closed/maximal item set filter
            2010.08.06 function isr_iset() for direct reporting added
            2010.08.11 function isr_isetx() for extended items added
            2010.08.14 item set header for output added to isr_create()
            2010.10.15 functions isr_open(), isr_close(), isr_rule()
            2010.10.27 handling of null names in isr_open() changed
            2011.05.06 generalized to support type RSUPP (int/double)
            2011.06.10 function isr_wgtsupp() added (weight/support)
            2011.07.12 adapted to optional integer item names
            2011.07.23 parameter dir added to function isr_seteval()
            2011.08.16 filtering for generators added (with hash table)
            2011.08.17 header/separator/implication sign copied
            2011.08.19 item sorting for generator filtering added
            2011.08.27 no explicit item set generation for no output
            2011.08.29 internal file write buffer added (faster output)
            2011.09.20 internal repository for filtering made optional
            2011.09.27 bug in function isr_report() fixed (item counter)
            2011.10.18 bug in function fastchk() fixed (hdr/sep check)
            2011.10.21 output of floating point numbers improved
            2012.04.10 function isr_addnc() added (no perf. ext. check)
            2012.04.17 weights and logarithms initialized to zero
            2012.05.30 function isr_addpexpk() added (packed items)
            2012.07.23 format character 'd' added (absolute support)
            2012.10.16 bug in function isr_rinfo() fixed ("L", lift)
            2012.10.26 bug in function fastout() fixed (empty set)
            2013.01.25 output order item set/perfect extensions changed
            2013.03.07 adapted to direction param. of sorting functions
            2013.03.10 adapted to modified bsearch/bisect interface
            2013.08.22 number of significant digits limited in getsd()
            2013.10.08 function isr_seqrule() added (head at end)
            2013.10.15 check of ferror() added to isr_[tid]close()
            2013.11.07 item name handling made optional (pyfim/pycoco)
            2014.05.12 filtering border for item signatures added
            2014.08.05 function isr_reportv() added (with evaluation)
            2014.08.18 creation and configuration functions redesigned
            2014.08.27 functions isr_settarg(), isr_target() etc. added
            2014.09.02 return type of reporting functions changed to int
            2014.09.18 functions isr_reprule(), isr_setrule() added
            2015.02.20 functions isr_extrule() and isr_xinfo() added
            2015.06.04 function isr_createx() added (add. parameter)
            2015.09.04 bug in function isr_delete() fixed (rep->iset)
            2016.02.13 bug in isr_xinfo() fixed (%j, %J, %k, %K))
            2016.02.18 write buffer, pre-formatting for transaction ids.
            2016.02.19 pre-formatted integers for absolute support
            2016.04.10 functions isr_extrule() and isr_xinfo() extended
            2016.08.27 bug in item statistics reporting fixed
            2016.09.29 function isr_sxrule() added (explicit head item)
            2016.10.14 function isr_size() added (item array size)
            2016.10.14 bugs in array/memory sizes for sequences fixed
----------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <assert.h>
#include <math.h>
#include "report.h"
#ifndef ISR_NONAMES
#include "scanner.h"
#endif
#ifdef STORAGE
#include "storage.h"
#endif

#ifdef _MSC_VER
#ifndef isnan
#define isnan(x)      _isnan(x)
#endif                          /* check for 'not a number' */
#ifndef isinf
#define isinf(x)    (!_isnan(x) && !_finite(x))
#endif                          /* check for an infinite value */
#endif                          /* MSC still does not support C99 */

/*----------------------------------------------------------------------
  Preprocessor Definitions
----------------------------------------------------------------------*/
#define BS_BORDER      32       /* block size for filtering border */
#define BS_WRITE    65536       /* size of internal write buffer */
#define BS_INT         48       /* buffer size for integer output */
#define BS_FLOAT       96       /* buffer size for float   output */
#define LN_2        0.69314718055994530942  /* ln(2) */

/*----------------------------------------------------------------------
  Constants
----------------------------------------------------------------------*/
static const double pows[] = {  /* several powers of ten */
  1e-02, 1e-01,                 /* for floating point number output */
  1e+00, 1e+01, 1e+02, 1e+03, 1e+04, 1e+05, 1e+06, 1e+07,
  1e+08, 1e+09, 1e+10, 1e+11, 1e+12, 1e+13, 1e+14, 1e+15,
  1e+16, 1e+17, 1e+18, 1e+19, 1e+20, 1e+21, 1e+22, 1e+23,
  1e+24, 1e+25, 1e+26, 1e+27, 1e+28, 1e+29, 1e+30, 1e+31,
  1e+32, 1e+33 };

/*----------------------------------------------------------------------
  Basic Output Functions
----------------------------------------------------------------------*/

static void fastchk (ISREPORT *rep)
{                               /* --- check for fast output mode */
  if (rep->border               /* if there is a filtering border */
  ||  rep->repofn               /* or a report function */
  ||  rep->evalfn               /* or an evaluation function */
  ||  rep->tidfile)             /* or trans ids. are to be written, */
    rep->fast =  0;             /* standard output has to be used */
  else if (!rep->file)          /* if no output (and no filtering), */
    rep->fast = -1;             /* only count the item sets */
  else {                        /* if only an output file is written */
    rep->fast = ((rep->zmin <= 1) && (rep->zmax >= ITEM_MAX)
              && ((strcmp(rep->info, " (%a)") == 0)
              ||  (strcmp(rep->info, " (%d)") == 0))
              &&  (strcmp(rep->hdr,  "")      == 0)
              &&  (strcmp(rep->sep,  " ")     == 0)) ? +1 : 0;
  }                             /* check standard reporting settings */
}  /* fastchk() */

/*--------------------------------------------------------------------*/

static int getsd (const char *s, const char **end)
{                               /* --- get number of signif. digits */
  int k = 6;                    /* number of significant digits */

  assert(s && end);             /* check the function arguments */
  if ((*s >= '0') && (*s <= '9')) {
    k = *s++ -'0';              /* get the first digit */
    if ((*s >= '0') && (*s <= '9'))
      k = 10 *k +*s++ -'0';     /* get a possible second digit and */
  }                             /* compute the number of digits */
  if (k > 32) k = 32;           /* limit   the number of digits */
  *end = s; return k;           /* return  the number of digits */
}  /* getsd() */

/*--------------------------------------------------------------------*/

static void isr_flush (ISREPORT *rep)
{                               /* --- flush the output buffer */
  assert(rep);                  /* check the function arguments */
  fwrite(rep->buf, sizeof(char),(size_t)(rep->next-rep->buf),rep->file);
  rep->next = rep->buf;         /* write the output buffer */
  #ifndef NDEBUG                /* in debug mode */
  fflush(rep->file);            /* flush the output buffer */
  #endif                        /* after every flush operation */
}  /* isr_flush() */

/*--------------------------------------------------------------------*/

static void isr_putc (ISREPORT *rep, int c)
{                               /* --- write a single character */
  assert(rep);                  /* check the function arguments */
  if (rep->next >= rep->end)    /* if the output buffer is full, */
    isr_flush(rep);             /* flush it (write it to the file) */
  *rep->next++ = (char)c;       /* store the given character */
}  /* isr_putc() */

/*--------------------------------------------------------------------*/

static int isr_puts (ISREPORT *rep, const char *s)
{                               /* --- write a character string */
  const char *t;                /* to traverse the characters string */

  assert(rep);                  /* check the function arguments */
  for (t = s; *t; ) {           /* while not at end of string */
    if (rep->next >= rep->end)  /* if the output buffer is full, */
      isr_flush(rep);           /* flush it (write it to the file) */
    *rep->next++ = *t++;        /* store the next string character */
  }
  return (int)(t-s);            /* return the number of characters */
}  /* isr_puts() */

/*--------------------------------------------------------------------*/

static void isr_putsn (ISREPORT *rep, const char *s, int n)
{                               /* --- write a character string */
  int k;                        /* number of chars in buffer */

  assert(rep);                  /* check the function arguments */
  while (n > 0) {               /* while there are characters left */
    k = (int)(rep->end -rep->next); /* get free space in write buffer */
    if (k >= n) {               /* if the string fits into buffer */
      memcpy(rep->next, s, (size_t)n *sizeof(char));
      rep->next += n; break;    /* simply copy the string into */
    }                           /* the write buffer and abort */
    memcpy(rep->next, s, (size_t)k *sizeof(char));
    s += k; n -= k; rep->next = rep->end;
    isr_flush(rep);             /* fill the buffer, then flush it, */
  }                             /* and reduce the remaining string */
}  /* isr_putsn() */

/*--------------------------------------------------------------------*/

int isr_intout (ISREPORT *rep, ptrdiff_t num)
{                               /* --- print an integer number */
  int  i = BS_INT, n;           /* loop variable, character counter */
  char buf[BS_INT];             /* output buffer */

  assert(rep);                  /* check the function arguments */
  if (rep->ints                 /* if pre-formatted ints. available */
  && (num >= (ptrdiff_t)rep->imin)    /* and the number is in range */
  && (num <= (ptrdiff_t)rep->imax))
    return isr_puts(rep, rep->ints[num -rep->imin]);
  if (num == 0) {               /* treat zero as a special case */
    isr_putc(rep, '0'); return 1; }
  if (num <= PTRDIFF_MIN) {     /* treat minimum as a special case */
    #if PTRDIFF_MIN < 0x80000000/* if 64 bit system */
    isr_putsn(rep, "-9223372036854775808", 20); return 20;
    #else                       /* if 32 bit system */
    isr_putsn(rep, "-2147483648", 11); return 11;
    #endif                      /* directly return string and size */
  }
  n = 0;                        /* default: no sign printed */
  if (num < 0) {                /* if the number is negative, */
    num = -num; isr_putc(rep, '-'); n = 1; }  /* print a sign */
  do {                          /* digit output loop */
    buf[--i] = (char)((num % 10) +'0');  /* store the next digit */
    num /= 10;                  /* and remove it from the number */
  } while (num > 0);            /* while there are more digits */
  isr_putsn(rep, buf+i, BS_INT-i);
  n += BS_INT-i;                /* print the generated digits and */
  return n;                     /* return the number of characters */
}  /* isr_intout() */

/*--------------------------------------------------------------------*/

int mantout (ISREPORT *rep, double num, int digits, int ints)
{                               /* --- format a non-negative mantissa */
  int    i, n;                  /* loop variables, sign flag */
  double x, y;                  /* integral and fractional part */
  char   *s, *e, *d;            /* pointers into the output buffer */
  char   buf[BS_FLOAT];         /* output buffer */

  assert(rep);                  /* check the function arguments */
  i = (int)dbl_bisect(num, pows, 36);
  if ((i >= 36) || (pows[i] == num)) i++;
  n = digits -(i-2);            /* compute the number of decimals */
  x = floor(num); y = num-x;    /* split into integer and fraction */
  e = d = buf +40;              /* get buffer for the decimals */
  if (n > 0) {                  /* if to print decimal digits, */
    *e++ = '.';                 /* store a decimal point */
    do { y *= 10;               /* compute the next decimal */
      *e++ = (char)((int)y+'0');/* and store it in the buffer */
      y   -= floor(y);          /* remove the printed decimal */
    } while (--n > 0);          /* while there are more decimals */
  }                             /* remove a decimal if necessary */
  if ((y > 0.5) || ((y == 0.5)  /* if number needs to be rounded */
  &&  ((e > d) ? *(e-1) & 1 : floor(x/2) >= x/2))) {
    for (s = e; --s > d; ) {    /* traverse the decimal digits */
      if (*s < '9') { (*s)++; break; }
      *s = '0';                 /* if digit can be incremented, */
    }                           /* abort, otherwise store a zero */
    if ((s <= d) && ((x += 1) >= pows[i]))
      if (--e <= d+1) e = d;    /* if all decimals have been adapted, */
  }                             /* increment the integer part and */
  if (e > d) {                  /* if there are decimal places, */
    while (*--e == '0');        /* remove all trailing zeros */
    if (e > d) e++;             /* if there are no decimals left, */
  }                             /* also remove the decimal point */
  s = d;                        /* adapt the decimals if necessary */
  do {                          /* integral part output loop */
    *--s = (char)(fmod(x, 10) +'0');
    x = floor(x/10);            /* compute and store next digit */
  } while (x > 0);              /* while there are more digits */
  if ((n = (int)(d-s)) > ints)  /* check size of integral part */
    return -n;                  /* and abort if it is too large */
  isr_putsn(rep, s, n = (int)(e-s)); /* print the formatted number */
  return n;                     /* return the number of characters */
}  /* mantout() */

/*--------------------------------------------------------------------*/

int isr_numout (ISREPORT *rep, double num, int digits)
{                               /* --- print a floating point number */
  int  k, n, e;                 /* character counters and exponent */
  char buf[BS_FLOAT];           /* output buffer */

  assert(rep);                  /* check the function arguments */
  if (isnan(num)) {             /* check for 'not a number' */
    isr_putsn(rep, "nan", 3); return 3; }
  n = 0;                        /* default: no character printed */
  if (num < 0) {                /* if the number is negative, */
    num = -num; isr_putc(rep, '-'); n = 1; }  /* print a sign */
  if (isinf(num)) {             /* check for an infinite value */
    isr_putsn(rep, "inf", 3); return n+3; }
  if (num < DBL_MIN) {          /* check for a zero value */
    isr_putc(rep, '0');   return n+1; }
  if (digits > 32) digits = 32; /* limit the number of sign. digits */
  if (digits > 11) {            /* if very high precision is needed */
    k = sprintf(buf, "%.*g", digits, num);
    isr_putsn(rep, buf, k);     /* format with standard printf, */
    return n+k;                 /* print the formatted number and */
  }                             /* return the number of characters */
  e = 0;                        /* default: no exponential represent. */
  if ((num >= pows[digits+2])   /* if an exponential representation */
  ||  (num <  0.001)) {         /* is of the number is preferable */
    while (num <  1e00) { num *= 1e32; e -= 32; }
    while (num >= 1e32) { num /= 1e32; e += 32; }
    k = (int)dbl_bisect(num, pows+2, 34);
    if ((k >= 34) || (pows[k+2] != num)) k--;
    e += k;                     /* find and extract decimal exponent */
    num /= pows[k+2];           /* compute the new mantissa */
  }                             /* (one digit before decimal point) */
  k = mantout(rep, num, digits, (e == 0) ? digits : 1);
  if (k < 0) {                  /* try to output the mantissa */
    num /= pows[1-k]; e += -1-k;/* on failure adapt the mantissa */
    k = mantout(rep, num, digits, 1);
  }                             /* output the adapted number */
  n += k;                       /* compute number of printed chars. */
  if (e == 0) return n;         /* if no exponent, abort the function */
  isr_putc(rep, 'e'); n += 2;   /* print an exponent indicator */
  isr_putc(rep, (e < 0) ? '-' : '+');
  if ((e = abs(e)) < 10) { isr_putc(rep, '0'); n++; }
  k = BS_INT;                   /* get the end of the buffer */
  do {                          /* exponent digit output loop */
    buf[--k] = (char)((e % 10) +'0');    /* store the next digit */
    e /= 10;                    /* and remove it from the number */
  } while (e > 0);              /* while there are more digits */
  isr_putsn(rep, buf+k, BS_INT-k);
  return n+BS_INT-k;            /* print the generated digits and */
}  /* isr_numout() */           /* return the number of characters */

/* It is (significantly) faster to output a floating point number  */
/* with the above routines than with sprintf. However, the above   */
/* code produces slightly less accurate output for more than about */
/* 14 significant digits. For those cases sprintf is used instead. */

/*--------------------------------------------------------------------*/

int isr_wgtout (ISREPORT *rep, RSUPP supp, double wgt)
{                               /* --- print an item weight */
  int        k, n = 0;          /* number of decimals, char. counter */
  const char *s, *t;            /* to traverse the format */

  assert(rep);                  /* check the function arguments */
  if (!rep->iwf || !rep->file)  /* check for an item weight format */
    return 0;                   /* and an output file */
  for (s = rep->iwf; *s; ) {    /* traverse the output format */
    if (*s != '%') {            /* copy everything except '%' */
      isr_putc(rep, *s++); n++; continue; }
    t = s++; k = getsd(s,&s);   /* get the number of signif. digits */
    switch (*s++) {             /* evaluate the indicator character */
      case '%': isr_putc(rep, '%'); n++;                   break;
      case 'g': n += isr_numout(rep, wgt,              k); break;
      case 'w': n += isr_numout(rep, wgt,              k); break;
      case 'm': n += isr_numout(rep, wgt/(double)supp, k); break;
      case  0 : --s;            /* print the requested quantity */
      default : isr_putsn(rep, t, k = (int)(s-t)); n += k; t = s; break;
    }                           /* otherwise copy characters */
  }
  return n;                     /* return the number of characters */
}  /* isr_wgtout() */

/*--------------------------------------------------------------------*/

static void isr_tidflush (ISREPORT *rep)
{                               /* --- flush the output buffer */
  assert(rep);                  /* check the function arguments */
  fwrite(rep->tidbuf, sizeof(char), (size_t)(rep->tidnxt-rep->tidbuf),
         rep->tidfile);         /* write the output buffer */
  rep->tidnxt = rep->tidbuf;    /* reinit. next character pointer */
  #ifndef NDEBUG                /* in debug mode */
  fflush(rep->tidfile);         /* flush the output buffer */
  #endif                        /* after every flush operation */
}  /* isr_tidflush() */

/*--------------------------------------------------------------------*/

static void isr_tidputc (ISREPORT *rep, int c)
{                               /* --- write a single character */
  assert(rep);                  /* check the function arguments */
  if (rep->tidnxt >= rep->tidend)     /* if output buffer is full, */
    isr_tidflush(rep);          /* flush it (write it to the file) */
  *rep->tidnxt++ = (char)c;     /* store the given character */
}  /* isr_tidputc() */

/*--------------------------------------------------------------------*/

static void isr_tidputs (ISREPORT *rep, const char *s)
{                               /* --- write a character string */
  assert(rep);                  /* check the function arguments */
  while (*s) {                  /* while not at end of string */
    if (rep->tidnxt >= rep->tidend)   /* if output buffer is full, */
      isr_tidflush(rep);        /* flush it (write it to the file) */
    *rep->tidnxt++ = *s++;      /* store the next string character */
  }
}  /* isr_tidputs() */

/*--------------------------------------------------------------------*/

static void isr_tidputsn (ISREPORT *rep, const char *s, int n)
{                               /* --- write a character string */
  int k;                        /* number of chars in buffer */

  assert(rep);                  /* check the function arguments */
  while (n > 0) {               /* while there are characters left */
    k = (int)(rep->tidend -rep->tidnxt); /* get free space in buffer */
    if (k >= n) {               /* if the string fits into buffer */
      memcpy(rep->tidnxt, s, (size_t)n *sizeof(char));
      rep->tidnxt += n; break;  /* simply copy the string into */
    }                           /* the write buffer and abort */
    memcpy(rep->tidnxt, s, (size_t)k *sizeof(char));
    s += k; n -= k; rep->tidnxt = rep->tidend;
    isr_tidflush(rep);          /* fill the buffer, then flush it, */
  }                             /* and reduce the remaining string */
}  /* isr_tidputsn() */

/*--------------------------------------------------------------------*/

static void isr_tidout (ISREPORT *rep, TID tid)
{                               /* --- print a transaction id */
  int  i;                       /* loop variable */
  char buf[BS_INT];             /* output buffer */

  assert(rep && (tid >= 0));    /* check the function arguments */
  if (rep->ints                 /* if pre-formatted ints. available */
  && (tid >= rep->imin)         /* and transaction id is in range */
  && (tid <= rep->imax)) {
    isr_tidputs(rep, rep->ints[tid -rep->imin]); return; }
  i = BS_INT;                   /* get end of output buffer */
  do {                          /* digit output loop (back to front) */
    buf[--i] = (char)((tid % 10) +'0');  /* store the next digit */
    tid /= 10;                  /* and remove it from the number */
  } while (tid > 0);            /* while there are more digits */
  isr_tidputsn(rep, buf+i, BS_INT-i);      /* print the digits */
}  /* isr_tidout() */

/*--------------------------------------------------------------------*/

static void isr_occout (ISREPORT *rep, ITEM occ)
{                               /* --- print an occurrence count */
  int  i;                       /* loop variable */
  char buf[BS_INT];             /* output buffer */

  assert(rep && (occ >= 0));    /* check the function arguments */
  if (rep->ints                 /* if pre-formatted ints. available */
  && ((TID)occ <= rep->imin)    /* and occurrence counter is in range */
  && ((TID)occ <= rep->imax)) {
    isr_tidputs(rep, rep->ints[occ -rep->imin]); return; }
  i = BS_INT;                   /* get end of output buffer */
  do {                          /* digit output loop (back to front) */
    buf[--i] = (char)((occ % 10) +'0');  /* store the next digit */
    occ /= 10;                  /* and remove it from the number */
  } while (occ > 0);            /* while there are more digits */
  isr_tidputsn(rep, buf+i, BS_INT-i);      /* print the digits */
}  /* isr_occout() */

/*----------------------------------------------------------------------
  Generator Filtering Functions
----------------------------------------------------------------------*/
#ifdef ISR_CLOMAX

static size_t is_hash (const void *set, int type)
{                               /* --- compute item set hash value */
  size_t     i;                 /* loop variable */
  size_t     h;                 /* computed hash value */
  const ITEM *p;                /* to access the items */

  assert(set);                  /* check the function argument */
  p = (const ITEM*)set;         /* type the item set pointer */
  h = (size_t)*p++;             /* get the number of items */
  i = (h >> 3) +1;              /* use Duff's device */
  switch (h & 7) {              /* to compute the hash value */
    do {    h = h *251 +(size_t)*p++;
    case 7: h = h *251 +(size_t)*p++;
    case 6: h = h *251 +(size_t)*p++;
    case 5: h = h *251 +(size_t)*p++;
    case 4: h = h *251 +(size_t)*p++;
    case 3: h = h *251 +(size_t)*p++;
    case 2: h = h *251 +(size_t)*p++;
    case 1: h = h *251 +(size_t)*p++;
    case 0: ; } while (--i > 0);
  }                             /* semicolon is necessary */
  return h;                     /* return the computed hash value */
  /* This hash function treats an item set like a string, that is, */
  /* the hash code depends on the order of the items. This is no   */
  /* drawback, though, because the comparison also requires that   */
  /* the items are in the same order in the item sets to compare.  */
  /* However, an order-independent hash code could be used to make */
  /* the function is_isgen() faster by avoiding recomputations.    */
}  /* is_hash() */

/*--------------------------------------------------------------------*/

static int is_cmp (const void *a, const void *b, void *d)
{                               /* --- compare two item sets */
  ITEM n;                       /* loop variable, number of items */
  ITEM *x, *y;                  /* to access the item sets */

  assert(a && b);               /* check the function arguments */
  x = (ITEM*)a; y = (ITEM*)b;   /* get/type the item set pointers */
  n = *x++;                     /* if the item set sizes differ, */
  if (n != *y++) return 1;      /* the item sets are not equal */
  while (--n >= 0)              /* traverse and compare the items */
    if (x[n] != y[n]) return 1; /* if an item differs, abort */
  return 0;                     /* otherwise the item sets are equal */
  /* Using memcmp() for the comparison is slower, because memcmp() */
  /* also checks the order relationship, not just equality, which, */
  /* however, is all that is needed inside the hash table.         */
}  /* is_cmp() */

/*--------------------------------------------------------------------*/

static int is_isgen (ISREPORT *rep, ITEM item, RSUPP supp)
{                               /* --- check for a generator */
  ITEM   i;                     /* loop variable */
  size_t z;                     /* key size */
  ITEM   *p;                    /* to access the hash table key */
  RSUPP  *s;                    /* to access the hash table data */
  ITEM   a, b;                  /* buffers for items (hold-out) */

  assert(rep && (item >= 0));   /* check the function arguments */
  rep->iset[rep->cnt+1] = item; /* store the new item at the end */
  if (rep->cnt > 0) {           /* if the current set is not empty */
    rep->iset[0] = rep->cnt;    /* copy the item set to the buffer */
    p = (ITEM*)memcpy(rep->iset+1, rep->items,
                      (size_t)rep->cnt *sizeof(ITEM));
    if (rep->mode & ISR_SORT)   /* sort the items according to code */
      ia_qsort(p, (size_t)rep->cnt+1, rep->dir);
    a = p[i = rep->cnt];        /* note the first hold-out item */
    for (++i; --i >= 0; ) {     /* traverse the items in the set */
      b = p[i]; p[i] = a; a = b;/* get next subset (next hold-out) */
      if (a == item) continue;  /* do not exclude the new item */
      s = (RSUPP*)st_lookup(rep->gentab, rep->iset, 0);
      if (!s || (*s == supp))   /* if a subset with one item less */
        break;                  /* is not in the generator repository */
    }                           /* or has the same support, abort */
    if (i >= 0) return 0;       /* if subset was found, no generator */
    memmove(p+1, p, (size_t)rep->cnt *sizeof(ITEM));
    p[0] = a;                   /* restore the full new item set */
  }                             /* (with the proper item order) */
  rep->iset[0] = rep->cnt+1;    /* store the new item set size */
  z = (size_t)(rep->cnt+2) *sizeof(ITEM);  /* compute key size */
  s = (RSUPP*)st_insert(rep->gentab, rep->iset, 0, z, sizeof(RSUPP));
  if (!s) return -1;            /* add the new set to the repository */
  *s = supp;                    /* and store its support as the data */
  return 1;                     /* return 'set is a generator' */
}  /* is_isgen() */

#endif
/*----------------------------------------------------------------------
  Main Functions
----------------------------------------------------------------------*/

ISREPORT* isr_createx (ITEMBASE *base, ITEM max)
{                               /* --- create an item set reporter */
  ISREPORT   *rep;              /* created item set reporter */
  ITEM       i, k, n;           /* loop variable, number of items */
  double     b, x;              /* logarithms of item probabilities */
  #ifndef ISR_NONAMES           /* if not to use item names */
  const char *name;             /* to traverse the item names */
  char       *buf;              /* buffer for formatted item name */
  size_t     len, m;            /* length of an item name */
  #endif

  assert(base);                 /* check the function argument */
  n   = ib_cnt(base);           /* get the number of items and */
  k   = (max > n) ? max : n;    /* the maximum pattern length */
  rep = (ISREPORT*)malloc(sizeof(ISREPORT)
                        +(size_t)(k+k+1) *sizeof(char*));
  rep->base    = base;          /* note the item base */
  rep->target  = ISR_ALL;       /* report all item sets by default */
  rep->mode    = 0;             /* no special mode set yet */
  rep->zmin    = 1;             /* set the default size range */
  rep->zmax    = ITEM_MAX;
  rep->xmax    = ITEM_MAX;
  rep->size    = k;
  rep->smin    = 1;             /* set the default support range */
  rep->smax    = RSUPP_MAX;
  rep->border  = NULL;          /* clear the filtering border */
  rep->bdrsize = rep->bdrcnt = 0;
  rep->pfx     = rep->cnt = 0;  /* init. the number of items */
  rep->clomax  = NULL;          /* clear the filtering tools */
  rep->gentab  = NULL;          /* (closed/maximal/generators) */
  rep->sto     = RSUPP_MAX;     /* clear the storage flag and */
  rep->dir     = 0;             /* the item sorting direction */
  rep->iset    = NULL;          /* cleat the item set buffer */
  rep->evalfn  = (ISEVALFN*)0;  /* clear add. evaluation function */
  rep->evaldat = NULL;          /* and the corresponding data */
  rep->evaldir = 1;             /* default: threshold is minimum */
  rep->evalthh = rep->eval = 0; /* clear evaluation and its minimum */
  rep->repofn  = (ISREPOFN*)0;  /* clear item set report function */
  rep->repodat = NULL;          /* and the corresponding data */
  rep->rulefn  = (ISRULEFN*)0;  /* clear rule report function */
  rep->ruledat = NULL;          /* and the corresponding data */
  rep->scan    = 0;             /* clear scanable items flag */
  rep->str     = NULL;          /* no user-specified format strings */
  rep->hdr     = "";            /* item set/rule header */
  rep->sep     = " ";           /* item separator */
  rep->imp     = " <- ";        /* implication sign */
  rep->iwf     = ":%w";         /* item weight format */
  rep->info    = " (%a)";       /* item set/rule information format */
  rep->inames  = (const char**)(rep->pos +k+1);
  rep->nmax    = rep->nsum = 0; /* clear maximum/sum of name lengths */
  rep->repcnt  = 0;             /* init. the item set counter */
  rep->psp     = NULL;          /* clear pattern spectrum variable */
  rep->ints    = NULL;          /* clear pre-formatted integers */
  rep->imax    = -1;
  rep->file    = NULL;          /* clear the output file and its name */
  rep->name    = NULL;          /* and the file write buffer */
  rep->buf     = rep->next   = rep->end    = NULL;
  rep->tidfile = NULL;          /* clear transaction id output file */
  rep->tidname = NULL;          /* and its name */
  rep->tidbuf  = rep->tidnxt = rep->tidend = NULL;
  rep->occs    = NULL;
  rep->tids    = NULL;
  rep->tidcnt  = 0;
  rep->tracnt  = 0;
  rep->miscnt  = 0;
  rep->fast    = -1;            /* default: only count the item sets */
  rep->fosize  = 0;
  rep->out     = NULL;          /* there is no output buffer yet */
  rep->pxpp    = (ITEM*)  malloc((size_t)(k+k+k+2) *sizeof(ITEM));
  rep->iset    = (ITEM*)  malloc((size_t)(k+1)     *sizeof(ITEM));
  rep->supps   = (RSUPP*) malloc((size_t)(k+1)     *sizeof(RSUPP));
  rep->wgts    = (double*)calloc((size_t)(k+n+1),   sizeof(double));
  rep->stats   = (size_t*)calloc((size_t)(k+1),     sizeof(size_t));
  if (!rep->pxpp || !rep->iset || !rep->supps || !rep->wgts
  ||  !rep->stats) { isr_delete(rep, 0); return NULL; }
  memset(rep->pxpp, 0, (size_t)(n+1) *sizeof(ITEM));
  rep->pexs    = rep->pxpp +n+1;/* allocate memory for the arrays */
  rep->items   = rep->pexs += k;/* and organize and init. the arrays */
  rep->ldps    = rep->wgts +k+1;
  *rep->wgts   = (double)(*rep->supps = ib_getwgt(base));
  b = (double)rep->supps[0];    /* get the empty set support */
  b = (b > 0) ? log(b) /LN_2 : 0;
  for (i = 0; i < n; i++) {     /* traverse the items */
    x = (double)ib_getfrq(rep->base, i);
    rep->ldps[i] = (x > 0) ? log(x) /LN_2 -b : 0;
  }                             /* compute bin. logs. of item probs. */
  memset((void*)rep->inames, 0, (size_t)(n+1) *sizeof(char*));
  #ifndef ISR_NONAMES           /* if to use item names */
  for (rep->nmax = rep->nsum = 0, i = 0; i < n; i++) {
    name = ib_xname(base, i);   /* traverse items and their names */
    if (!rep->scan)             /* if to use items names directly, */
      m = strlen(name);         /* simply get their string lengths */
    else {                      /* if name formatting may be needed */
      m = scn_fmtlen(name, &len);
      if (m > len) {            /* if name formatting is needed */
        buf = (char*)malloc((m+1) *sizeof(char));
        if (buf) scn_format(buf, name, 0);
        name = buf;             /* format the item name */
      }                         /* (quote certain characters) */
    }                           /* and replace the original name */
    rep->nsum += m;             /* sum name size and find maximum */
    if (m > rep->nmax) rep->nmax = m;
    rep->inames[i] = name;      /* store the (formatted) item name */
    if (!name) { isr_delete(rep, 0); return NULL; }
  }                             /* check for proper name copying */
  #endif
  return rep;                   /* return created item set reporter */
}  /* isr_createx() */

/*--------------------------------------------------------------------*/

int isr_delete (ISREPORT *rep, int delis)
{                               /* --- delete an item set reporter */
  int r, s;                     /* results of file close operations */

  assert(rep);                  /* check the function arguments */
  if (rep->out) free(rep->out); /* delete the item set output buffer */
  #ifdef ISR_CLOMAX             /* if closed/maximal filtering */
  if (rep->clomax) cm_delete(rep->clomax);
  if (rep->gentab) st_delete(rep->gentab);
  #endif                        /* delete the closed/maximal filter */
  #ifdef ISR_PATSPEC            /* if pattern spectrum functions */
  if (rep->psp)    psp_delete(rep->psp);
  #endif                        /* delete the pattern spectrum */
  if (rep->str)    free((void*)rep->str);
  if (rep->border) free(rep->border);
  #ifndef ISR_NONAMES
  { ITEM i;                     /* loop variable */
    for (i = 0; rep->inames[i]; i++) {
      if (rep->inames[i] != ib_name(rep->base, i))
        free((void*)rep->inames[i]);
    }                           /* delete existing item names */
  }                             /* delete all other arrays */
  #endif
  if (rep->ints)   free(rep->ints);
  if (rep->stats)  free(rep->stats);
  if (rep->wgts)   free(rep->wgts);
  if (rep->supps)  free(rep->supps);
  if (rep->iset)   free(rep->iset);
  if (rep->pxpp)   free(rep->pxpp);
  if (rep->base && delis) ib_delete(rep->base);
  r = isr_close(rep);           /* close the output files */
  s = isr_tidclose(rep);        /* (if output files are open) */
  if (rep->tidbuf) free(rep->tidbuf);
  if (rep->buf)    free(rep->buf); /* delete file write buffers */
  free(rep);                    /* delete the base structure */
  return (r) ? r : s;           /* return file closing result */
}  /* isr_delete() */

/*--------------------------------------------------------------------*/

int isr_settarg (ISREPORT *rep, int target, int mode, int dir)
{                               /* --- set target and operation mode */
  assert(rep);                  /* check the function arguments */
  if      (target & ISR_RULES)   target = ISR_RULES;
  else if (target & ISR_GENERAS) target = ISR_GENERAS;
  else if (target & ISR_MAXIMAL) target = ISR_MAXIMAL;
  else if (target & ISR_CLOSED)  target = ISR_CLOSED;
  else                           target = ISR_ALL;
  if (target & (ISR_CLOSED|ISR_MAXIMAL))
    mode |= ISR_NOEXPAND;       /* make reporting mode consistent */
  rep->target = target;         /* note target (closed/maximal etc) */
  rep->mode   = mode;           /* and reporting mode */
  #ifdef ISR_CLOMAX             /* if closed/maximal filtering */
  if (rep->clomax) { cm_delete(rep->clomax); rep->clomax = NULL; }
  if (rep->gentab) { st_delete(rep->gentab); rep->gentab = NULL; }
  if ((target & (ISR_MAXIMAL|ISR_CLOSED|ISR_GENERAS))
  &&  !(mode & ISR_NOFILTER)) { /* if to filter the item sets */
    if (target & ISR_GENERAS) { /* if to filter for generators, */
      size_t n = 1024*1024-1;   /* create an item set hash table */
      rep->gentab = st_create(n, 0, is_hash, is_cmp, NULL, (OBJFN*)0);
      if (!rep->gentab) return E_NOMEM; }
    else {                      /* if to filter for closed/maximal */
      rep->clomax = cm_create(dir, ib_cnt(rep->base));
      if (!rep->clomax) return E_NOMEM;
    }                           /* create a closed/maximal filter */
    rep->sto = (target & ISR_MAXIMAL) ? 0 : RSUPP_MAX;
    rep->dir = (dir < 0) ? -1 : 1;
  }                             /* note storage flag and direction */
  #endif
  fastchk(rep);                 /* check for fast output */
  return 0;                     /* return 'ok '*/
}  /* isr_settarg() */

/*--------------------------------------------------------------------*/

void isr_setsupp (ISREPORT *rep, RSUPP smin, RSUPP smax)
{                               /* --- set support range for item set */
  assert(rep                    /* check the function arguments */
  &&    (smin >= 0) && (smax >= smin));
  rep->smin = smin;             /* store the minimum and maximum */
  rep->smax = smax;             /* support of an item set to report */
}  /* isr_setsupp() */

/*--------------------------------------------------------------------*/

void isr_setsize (ISREPORT *rep, ITEM zmin, ITEM zmax)
{                               /* --- set size range for item set */
  assert(rep                    /* check the function arguments */
  &&    (zmin >= 0) && (zmax >= zmin));
  rep->zmin = zmin;             /* store the minimum and maximum */
  rep->zmax = zmax;             /* size of an item set to report */
  fastchk(rep);                 /* check for fast output */
}  /* isr_setsize() */

/*--------------------------------------------------------------------*/

RSUPP isr_setbdr (ISREPORT *rep, ITEM size, RSUPP supp)
{                               /* --- set the filtering border */
  ITEM  n;                      /* new size of the border array */
  RSUPP *p;                     /* buffer for reallocation */

  assert(rep                    /* check the function arguments */
  &&    (size >= 0) && (supp >= 0));
  n = rep->bdrsize;             /* get the number of border elements */
  if (size >= n) {              /* if to enlarge the border array */
    n += (n > BS_BORDER) ? n >> 1 : BS_BORDER;
    if (n <= size) n = size+1;  /* compute the new array size */
    p = (RSUPP*)realloc(rep->border, (size_t)n *sizeof(RSUPP));
    if (!p) return -1;          /* enlarge the border array */
    while (rep->bdrsize < n) p[rep->bdrsize++] = 0;
    rep->border = p;            /* clear the new array elements and */
  }                             /* set the new array and its size */
  if (size >= rep->bdrcnt) rep->bdrcnt = size+1;
  rep->border[size] = supp;     /* store the support threshold */
  fastchk(rep);                 /* check for fast output */
  return supp;                  /* return the new support threshold */
}  /* isr_setbdr() */

/*--------------------------------------------------------------------*/

void isr_clrbdr (ISREPORT *rep)
{                               /* --- clear the filtering border */
  assert(rep);                  /* check the function argument */
  if (rep->border) free(rep->border);
  rep->border = NULL;           /* deallocate the border array */
  rep->bdrcnt = rep->bdrsize = 0;    /* and clear the counters */
  fastchk(rep);                 /* check for fast output */
}  /* isr_clrbdr() */

/*--------------------------------------------------------------------*/

int isr_setfmtx (ISREPORT *rep, int scan, CCHAR *hdr, CCHAR *sep,
                 CCHAR *imp, CCHAR *info, CCHAR *iwf)
{                               /* --- set output format strings */
  size_t len = 0;               /* total length of format strings */
  CCHAR  *s;                    /* to traverse the format strings */
  char   *d;                    /* to traverse the destination */

  assert(rep);                  /* check the function argument */
  rep->scan = scan;             /* note the scanable item flag */
  if (hdr)  len += strlen(hdr);  else hdr  = "";
  if (sep)  len += strlen(sep);  else sep  = "";
  if (imp)  len += strlen(imp);  else imp  = "";
  if (info) len += strlen(info); else info = "";
  if (iwf)  len += strlen(iwf);  else iwf  = "";
  d = (char*)realloc((void*)rep->str, (len+5) *sizeof(char));
  if (!d) return -1;            /* create a buffer for the strings */
  for (rep->str  = d, s = hdr;  *s; ) *d++ = *s++;
  *d++ = 0;                     /* copy the record header */
  for (rep->sep  = d, s = sep;  *s; ) *d++ = *s++;
  *d++ = 0;                     /* copy the item separator */
  for (rep->imp  = d, s = imp;  *s; ) *d++ = *s++;
  *d++ = 0;                     /* copy the implication sign */
  for (rep->info = d, s = info; *s; ) *d++ = *s++;
  *d++ = 0;                     /* copy the information format */
  for (rep->iwf  = d, s = iwf;  *s; ) *d++ = *s++;
  *d++ = 0;                     /* copy the item weight format */
  fastchk(rep);                 /* check for fast output */
  return 0;                     /* return 'ok' */
}  /* isr_setfmtx() */

/*--------------------------------------------------------------------*/

void isr_seteval (ISREPORT *rep, ISEVALFN evalfn, void *data,
                  int dir, double thresh)
{                               /* --- set evaluation function */
  assert(rep);                  /* check the function argument */
  rep->evalfn  = evalfn;        /* store the evaluation function, */
  rep->evaldat = data;          /* the corresponding user data, */
  rep->evaldir = (dir >= 0) ? +1 : -1;  /* the evaluation direction */
  rep->evalthh = rep->evaldir *thresh;  /* and the threshold value  */
  fastchk(rep);                 /* check for fast output */
}  /* isr_seteval() */

/*--------------------------------------------------------------------*/

void isr_setrepo (ISREPORT *rep, ISREPOFN repofn, void *data)
{                               /* --- set set reporting function */
  assert(rep);                  /* check the function argument */
  rep->repofn  = repofn;        /* store the reporting function and */
  rep->repodat = data;          /* the corresponding user data */
  fastchk(rep);                 /* check for fast output */
}  /* isr_setrepo() */

/*--------------------------------------------------------------------*/

void isr_setrule (ISREPORT *rep, ISRULEFN rulefn, void *data)
{                               /* --- set rule reporting function */
  assert(rep);                  /* check the function argument */
  rep->rulefn  = rulefn;        /* store the reporting function and */
  rep->ruledat = data;          /* the corresponding user data */
}  /* isr_setrule() */

/*--------------------------------------------------------------------*/

int isr_prefmt (ISREPORT *rep, TID min, TID max)
{                               /* --- pre-format transaction ids */
  TID  t, z;                    /* to traverse the integers to format */
  char *s, *b, *c, *e;          /* next position, buffer positions */
  char buf[BS_INT+1];           /* to pre-format an integer number */

  if (rep->ints) {              /* clear pre-formatted integers */
    free(rep->ints); rep->ints = NULL; }
  if (max < 0) return 0;        /* if no preformatting, abort */
  if (min < 0) min = 0;         /* ensure a non-negative minimum */
  rep->imin = min;              /* note the range of integers */
  rep->imax = max;              /* that will be pre-formatted */
  max += 1; z = max+max;        /* one digit plus end-of string */
  for (t = 10; (max >= t) && (TID_MAX /10 > t); t *= 10)
    z += max-t;                 /* compute preformatted size */
  z -= min+min;                 /* one digit plus end-of string */
  for (t = 10; (min >= t) && (TID_MAX /10 > t); t *= 10)
    z -= min-t;                 /* compute preformatted size */
  t = max-min;                  /* get the number of integers */
  rep->ints = (char**)malloc((size_t)t*sizeof(char*)
                            +(size_t)z*sizeof(char));
  if (!rep->ints) return -1;    /* get buffer for preformatted nums. */
  s = (char*)(rep->ints +t);    /* and position of first integer */
  memset(buf, '0', sizeof(buf));/* fill preformat buffer with zeros */
  e = b = buf+BS_INT; *b = 0;   /* and get pointer to last digit */
  t = min;                      /* format the minimum */
  do {                          /* digit output loop */
    *--b = (char)((t % 10)+'0');/* store the next digit */
    t /= 10;                    /* and remove it from the number */
  } while (t > 0);              /* while there are more digits */
  for (t = min; t < max; t++) { /* traverse the transaction ids */
    rep->ints[t-min] = memcpy(s, b, (size_t)(e-b)+1);
    s += (e-b)+1;               /* copy the number representation */
    for (c = e; --c >= buf; ) { /* advance the number representation */
      if (*c >= '9') *c = '0'; else { (*c)++; break; } }
    if (c < b) b = c;           /* adapt the start of the number */
  }                             /* and advance the pointer */
  return 0;                     /* return 'ok' */
}  /* isr_prefmt() */

/*--------------------------------------------------------------------*/

int isr_open (ISREPORT *rep, FILE *file, const char *name)
{                               /* --- open an output file */
  assert(rep);                  /* check the function arguments */
  if (!rep->buf) {              /* if there is no output buffer */
    rep->buf  = (char*)malloc(BS_WRITE *sizeof(char));
    if (!rep->buf) return E_NOMEM;
    rep->end  = rep->buf +BS_WRITE;
    rep->next = rep->buf;       /* create an output buffer */
  }                             /* and set the pointers */
  if (file)                     /* if a file is given, */
    rep->name = name;           /* store the file name */
  else if (! name) {            /* if no name is given */
    file = NULL;   rep->name = "<null>"; }
  else if (!*name) {            /* if an empty name is given */
    file = stdout; rep->name = "<stdout>"; }
  else {                        /* if a proper name is given */
    file = fopen(rep->name = name, "w");
    if (!file) return E_FOPEN;  /* open file with given name */
  }                             /* and check for an error */
  rep->file = file;             /* store the new output file */
  fastchk(rep);                 /* check for fast output */
  return 0;                     /* return 'ok' */
}  /* isr_open() */

/*--------------------------------------------------------------------*/

int isr_close (ISREPORT *rep)
{                               /* --- close the output file */
  int r;                        /* result of fclose()/fflush() */

  assert(rep);                  /* check the function arguments */
  if (!rep->file) return 0;     /* check for an output file */
  isr_flush(rep);               /* flush the write buffer */
  r  = ferror(rep->file);       /* check the error indicator */
  r |= ((rep->file == stdout) || (rep->file == stderr))
     ? fflush(rep->file) : fclose(rep->file);
  rep->file = NULL;             /* close the current output file */
  fastchk(rep);                 /* check for fast output */
  return r;                     /* return the result of fclose() */
}  /* isr_close() */

/*--------------------------------------------------------------------*/

int isr_tidopen (ISREPORT *rep, FILE *file, const char *name)
{                               /* --- set/open trans. id output file */
  assert(rep);                  /* check the function arguments */
  if (!rep->tidbuf) {           /* if there is no output buffer */
    rep->tidbuf  = (char*)malloc(BS_WRITE *sizeof(char));
    if (!rep->tidbuf) return E_NOMEM;
    rep->tidend = rep->tidbuf +BS_WRITE;
    rep->tidnxt = rep->tidbuf;  /* create an output buffer */
  }                             /* and set the pointers */
  if (file) {                   /* if a file is given directly, */
    if      (name)           rep->tidname = name; /* store name */
    else if (file == stdout) rep->tidname = "<stdout>";
    else if (file == stderr) rep->tidname = "<stderr>";
    else                     rep->tidname = "<unknown>"; }
  else if (! name) {            /* if no name is given */
    file = NULL;             rep->tidname = "<null>"; }
  else if (!*name) {            /* if an empty name is given */
    file = stdout;           rep->tidname = "<stdout>"; }
  else {                        /* if a proper name is given */
    file = fopen(rep->tidname = name, "w");
    if (!file) return E_FOPEN;  /* open file with given name */
  }                             /* and check for an error */
  rep->tidfile = file;          /* store the new output file */
  fastchk(rep);                 /* check for fast output */
  return 0;                     /* return 'ok' */
}  /* isr_tidopen() */

/*--------------------------------------------------------------------*/

int isr_tidclose (ISREPORT *rep)
{                               /* --- close trans. id output file */
  int r;                        /* result of fclose() */

  assert(rep);                  /* check the function arguments */
  if (!rep->tidfile) return 0;  /* check for an output file */
  isr_tidflush(rep);            /* flush the write buffer */
  r  = ferror(rep->tidfile);    /* check the error indicator */
  r |= ((rep->tidfile == stdout) || (rep->tidfile == stderr))
     ? fflush(rep->tidfile) : fclose(rep->tidfile);
  rep->tidfile = NULL;          /* close the current output file */
  fastchk(rep);                 /* check for fast output */
  return r;                     /* return the result of fclose() */
}  /* isr_tidclose() */

/*--------------------------------------------------------------------*/

void isr_tidcfg (ISREPORT *rep, TID tracnt, ITEM miscnt)
{                               /* --- configure trans. id output */
  rep->tracnt = tracnt;         /* note number of transactions and */
  rep->miscnt = miscnt;         /* accepted number of missing items */
}  /* isr_tidcfg() */

/*--------------------------------------------------------------------*/

int isr_setup (ISREPORT *rep)
{                               /* --- set up the item set reporter */
  size_t h, s, z;               /* lengths, size of output buffer */

  assert(rep);                  /* check the function arguments */
  if (rep->out) free(rep->out); /* delete an existing output buffer */
  h = strlen(rep->hdr);         /* compute the output buffer size */
  s = strlen(rep->sep);         /* (items + header + separators) */
  z = (rep->mode & ISR_SEQUENCE)
    ? (size_t)rep->size *rep->nmax : rep->nsum;
  z += h +(size_t)(rep->size-1) *s +1;
  rep->out = (char*)malloc(z *sizeof(char));
  if (!rep->out) return E_NOMEM;/* create an output buffer and */
  strcpy(rep->out, rep->hdr);   /* copy the record header into it */
  rep->pos[0] = rep->out +h;    /* store the first item position */
  rep->pfx    = rep->cnt = 0;   /* initialize the item counters */
  rep->xmax   = ((rep->target & (ISR_CLOSED|ISR_MAXIMAL))
              && (rep->zmax < ITEM_MAX)) ? rep->zmax+1 : rep->zmax;
  fastchk(rep);                 /* check for fast output */
  return 0;                     /* return 'ok '*/
}  /* isr_setupx() */

/*--------------------------------------------------------------------*/

int isr_add (ISREPORT *rep, ITEM item, RSUPP supp)
{                               /* --- add an item (only support) */
  assert(rep && (item >= 0)     /* check the function arguments */
  &&    (item < ib_cnt(rep->base)));
  /* assert(!isr_uses(rep, item)); */
  /* if (supp < rep->smin) return 0; */
  #ifdef ISR_CLOMAX             /* if closed/maximal filtering */
  if      (rep->clomax) {       /* if a closed/maximal filter exists */
    int r = cm_add(rep->clomax, item, supp);
    if (r <= 0) return r; }     /* add the item to the c/m filter */
  else if (rep->gentab) {       /* if a generator filter exists */
    int r = is_isgen(rep, item, supp);
    if (r <= 0) return r;       /* add item set to the gen. filter */
  }                             /* check if item needs processing */
  #endif
  rep->pxpp [item] |= ITEM_MIN; /* mark the item as used */
  rep->items[  rep->cnt] = item;/* store the item and its support */
  rep->supps[++rep->cnt] = supp;/* clear the perfect ext. counter */
  rep->pxpp [  rep->cnt] &= ITEM_MIN;
  return 1;                     /* return 'ok' */
}  /* isr_add() */

/*--------------------------------------------------------------------*/

int isr_addnc (ISREPORT *rep, ITEM item, RSUPP supp)
{                               /* --- add an item (only support) */
  assert(rep && (item >= 0)     /* check the function arguments */
  &&    (item < ib_cnt(rep->base)));
  /* assert(!isr_uses(rep, item)); */
  /* if (supp < rep->smin) return 0; */
  #ifdef ISR_CLOMAX             /* if closed/maximal filtering */
  if (rep->clomax) {            /* if a closed/maximal filter exists */
    int r = cm_addnc(rep->clomax, item, supp);
    if (r <= 0) return r;       /* add the item to the c/m filter */
  }                             /* check only for a memory error */
  #endif
  rep->pxpp [item] |= ITEM_MIN; /* mark the item as used */
  rep->items[  rep->cnt] = item;/* store the item and its support */
  rep->supps[++rep->cnt] = supp;/* clear the perfect ext. counter */
  rep->pxpp [  rep->cnt] &= ITEM_MIN;
  return 1;                     /* return 'ok' */
}  /* isr_addnc() */

/* In contrast to isr_add(), the function isr_addnc() does not check */
/* whether the extended prefix possesses a perfect extension.        */

/*--------------------------------------------------------------------*/

int isr_addwgt (ISREPORT *rep, ITEM item, RSUPP supp, double wgt)
{                               /* --- add an item (support & weight) */
  assert(rep && (item >= 0)     /* check the function arguments */
  &&    (item < ib_cnt(rep->base)));
  /* assert(!isr_uses(rep, item)); */
  /* if (supp < rep->smin) return 0; */
  #ifdef ISR_CLOMAX             /* if closed/maximal filtering */
  if      (rep->clomax) {       /* if a closed/maximal filter exists */
    int r = cm_add(rep->clomax, item, supp);
    if (r <= 0) return r; }     /* add the item to the c/m filter */
  else if (rep->gentab) {       /* if a generator filter exists */
    int r = is_isgen(rep, item, supp);
    if (r <= 0) return r;       /* add item set to the gen. filter */
  }                             /* check if item needs processing */
  #endif
  rep->pxpp [item] |= ITEM_MIN; /* mark the item as used */
  rep->items[  rep->cnt] = item;/* store the item and its support */
  rep->supps[++rep->cnt] = supp;/* as well as its weight */
  rep->wgts [  rep->cnt] = wgt; /* clear the perfect ext. counter */
  rep->pxpp [  rep->cnt] &= ITEM_MIN;
  return 1;                     /* return 'ok' */
}  /* isr_addwgt() */

/*--------------------------------------------------------------------*/

int isr_addpex (ISREPORT *rep, ITEM item)
{                               /* --- add a perfect extension */
  assert(rep && (item >= 0)     /* check the function arguments */
  &&    (item < ib_cnt(rep->base)));
  if ((rep->pxpp[item] < 0)     /* if the item is already in use */
  ||  (rep->target & ISR_GENERAS))/* or to filter for generators, */
    return -1;                  /* perfect extensions are ignored */
  rep->pxpp[item] |= ITEM_MIN;  /* mark the item as used */
  *--rep->pexs = item;          /* store the added item and */
  rep->pxpp[rep->cnt]++;        /* count it for the current prefix */
  return 0;                     /* return 'ok' */
}  /* isr_addpex() */

/*--------------------------------------------------------------------*/

void isr_addpexpk (ISREPORT *rep, ITEM bits)
{                               /* --- add a perfect extension */
  ITEM i;                       /* loop variable/item */

  assert(rep);                  /* check the function arguments */
  bits &= ~ITEM_MIN;            /* traverse the set bits */
  for (i = 0; (UITEM)(1 << i) <= (UITEM)bits; i++) {
    if (((bits & (1 << i)) == 0)/* if the bit is not set */
    || (rep->pxpp[i] < 0)       /* or the item is already in use */
    || (rep->target & ISR_GENERAS)) /* or to filter for generators, */
      continue;                 /* perfect extensions are ignored */
    rep->pxpp[i] |= ITEM_MIN;   /* mark the item as used */
    *--rep->pexs = i;           /* store the added item and */
    rep->pxpp[rep->cnt] += 1;   /* count it for the current prefix */
  }
}  /* isr_addpexpk() */

/*--------------------------------------------------------------------*/

void isr_remove (ISREPORT *rep, ITEM n)
{                               /* --- remove one or more items */
  ITEM i;                       /* loop variable, buffer for an item */

  assert(rep                    /* check the function arguments */
  &&    (n >= 0) && (n <= rep->cnt));
  #ifdef ISR_CLOMAX             /* if closed/maximal filtering */
  if (rep->clomax)              /* if a closed/maximal filter exists, */
    cm_remove(rep->clomax, n);  /* remove the same number of items */
  #endif                        /* from this filter */
  while (--n >= 0) {            /* traverse the items to remove */
    for (i = rep->pxpp[rep->cnt] & ~ITEM_MIN; --i >= 0; )
      rep->pxpp[*rep->pexs++] &= ~ITEM_MIN;
    i = rep->items[--rep->cnt]; /* traverse the item to remove */
    rep->pxpp[i] &= ~ITEM_MIN;  /* (current item and perfect exts.) */
  }                             /* and remove their "in use" markers */
  if (rep->cnt < rep->pfx)      /* if too few items are left, */
    rep->pfx = rep->cnt;        /* reduce the valid prefix */
}  /* isr_remove() */

/*--------------------------------------------------------------------*/

double isr_logrto (ISREPORT *rep, void *data)
{                               /* --- logarithm of support ratio */
  ITEM   i;                     /* loop variable */
  double sum;                   /* sum of item logarithms */

  assert(rep);                  /* check the function arguments */
  if (rep->cnt <= 1) return 0;  /* if only one item, abort */
  sum = (double)rep->supps[rep->cnt];
  sum = (sum > 0) ? log(sum /(double)rep->supps[0]) /LN_2 : 0;
  for (i = 0; i < rep->cnt; i++)
    sum -= rep->ldps[rep->items[i]];
  return sum;                   /* compute log of support ratio */
}  /* isr_logrto() */

/* Evaluate an itemset by the logarithm of the quotient of the actual */
/* support of an item set and the support that is expected under full */
/* independence of the items (product of item probabilities times the */
/* total transaction weight). 'data' is needed for the interface.     */

/*--------------------------------------------------------------------*/

double isr_lrsize (ISREPORT *rep, void *data)
{                               /* --- logarithm of support quotient */
  assert(rep);                  /* check the function arguments */
  return isr_logrto(rep, data) /(double)rep->cnt;
}  /* isr_lrsize() */

/*--------------------------------------------------------------------*/

double isr_sizewgt (ISREPORT *rep, void *data)
{                               /* --- item set size times weight */
  assert(rep);                  /* check the function arguments */
  return rep->wgts[rep->cnt] *(double)rep->cnt;
}  /* isr_sizewgt() */

/* Evaluate an item set by the product of size and weight in order to */
/* favor large item sets and thus to compensate anti-monotone weights.*/

/*--------------------------------------------------------------------*/

double isr_wgtsize (ISREPORT *rep, void *data)
{                               /* --- item set weight / size */
  assert(rep);                  /* check the function arguments */
  return (rep->cnt > 0) ? rep->wgts[rep->cnt] /(double)rep->cnt : 0;
}  /* isr_wgtsize() */

/*--------------------------------------------------------------------*/

double isr_wgtsupp (ISREPORT *rep, void *data)
{                               /* --- item set weight / support */
  assert(rep);                  /* check the function arguments */
  return (rep->supps[rep->cnt] > 0)
       ? (double)rep->wgts[rep->cnt] /(double)rep->supps[rep->cnt] : 0;
}  /* isr_wgtsupp() */

/*--------------------------------------------------------------------*/

static void fastout (ISREPORT *rep, ITEM n)
{                               /* --- fast output of an item set */
  char       *s;                /* to traverse the output buffer */
  const char *name;             /* to traverse the item names */

  assert(rep);                  /* check the function argument */
  rep->stats[rep->cnt] += 1;    /* count the reported item set */
  rep->repcnt          += 1;    /* (for its size and overall) */
  #ifdef ISR_PATSPEC            /* if pattern spectrum functions */
  if (rep->psp)                 /* count item set in pattern spectrum */
    psp_incfrq(rep->psp, rep->cnt, rep->supps[rep->cnt], 1);
  #endif
  s = rep->pos[rep->pfx];       /* get the position for appending */
  while (rep->pfx < rep->cnt) { /* traverse the additional items */
    if (rep->pfx > 0)           /* if this is not the first item */
      for (name = rep->sep; *name; )
        *s++ = *name++;         /* copy the item separator */
    for (name = rep->inames[rep->items[rep->pfx]]; *name; )
      *s++ = *name++;           /* copy the item name to the buffer */
    rep->pos[++rep->pfx] = s;   /* compute and record new position */
  }                             /* for appending the next item */
  while (n > 0) {               /* traverse the perfect extensions */
    rep->items[rep->cnt++] = rep->pexs[--n];
    fastout(rep, n);            /* add the next perfect extension, */
    rep->pfx = --rep->cnt;      /* recursively report supersets, */
  }                             /* and remove the item again */
  isr_putsn(rep, rep->out, (int)(s-rep->out)); /* print item set */
  isr_putsn(rep, rep->foinfo, rep->fosize);    /* and its support */
}  /* fastout() */

/*--------------------------------------------------------------------*/

static void output (ISREPORT *rep)
{                               /* --- output an item set */
  TID        k;                 /* loop variable */
  ITEM       min;               /* minimum number of items */
  char       *s;                /* to traverse the output buffer */
  const char *name;             /* to traverse the item names */

  assert(rep                    /* check the function arguments */
  &&    (rep->cnt >= rep->zmin)
  &&    (rep->cnt <= rep->zmax));
  if (rep->border               /* if there is a filtering border */
  && (rep->cnt < rep->bdrcnt)   /* and the set size is in its range */
  && (rep->supps[rep->cnt] < rep->border[rep->cnt]))
    return;                     /* check the item set signature */
  if (rep->evalfn) {            /* if an evaluation function is given */
    rep->eval = rep->evalfn(rep, rep->evaldat);
    if (rep->evaldir *rep->eval < rep->evalthh)
      return;                   /* if the item set does not qualify, */
  }                             /* abort the output function */
  rep->stats[rep->cnt] += 1;    /* count the reported item set */
  rep->repcnt          += 1;    /* (for its size and overall) */
  #ifdef ISR_PATSPEC            /* if pattern spectrum functions */
  if (rep->psp)                 /* count item set in pattern spectrum */
    psp_incfrq(rep->psp, rep->cnt, rep->supps[rep->cnt], 1);
  #endif
  if (rep->repofn)              /* call reporting function if given */
    rep->repofn(rep, rep->repodat);
  if (!rep->file) return;       /* check for an output file */
  s = rep->pos[rep->pfx];       /* get the position for appending */
  while (rep->pfx < rep->cnt) { /* traverse the additional items */
    if (rep->pfx > 0)           /* if this is not the first item */
      for (name = rep->sep; *name; )
        *s++ = *name++;         /* copy the item separator */
    for (name = rep->inames[rep->items[rep->pfx]]; *name; )
      *s++ = *name++;           /* copy the item name to the buffer */
    rep->pos[++rep->pfx] = s;   /* compute and record new position */
  }                             /* for appending the next item */
  isr_putsn(rep, rep->out, (int)(s-rep->out));
  isr_sinfo(rep, rep->supps[rep->cnt], rep->wgts[rep->cnt], rep->eval);
  isr_putc (rep, '\n');         /* print the item set information */
  if (!rep->tidfile || !rep->tids) /* check whether to report */
    return;                        /* a list of transaction ids */
  if      (rep->tidcnt > 0) {   /* if tids are in ascending order */
    for (k = 0; k < rep->tidcnt; k++) {
      if (k > 0) isr_tidputs(rep, rep->sep);
      isr_tidout(rep, rep->tids[k]+1);
    } }                         /* report the transaction ids */
  else if (rep->tidcnt < 0) {   /* if tids are in descending order */
    for (k = -rep->tidcnt; k > 0; ) {
      isr_tidout(rep, rep->tids[--k]+1);
      if (k > 0) isr_tidputs(rep, rep->sep);
    } }                         /* report the transaction ids */
  else if (rep->tracnt > 0) {   /* if item occurrence counters */
    min = (ITEM)(rep->cnt-rep->miscnt); /* traverse all trans. ids */
    for (k = 0; k < rep->tracnt; k++) {
      if (rep->occs[k] < min)   /* skip all transactions that */
        continue;               /* do not contain enough items */
      if (k > 0) isr_tidputs(rep, rep->sep);
      isr_tidout(rep, k+1);     /* print the transaction identifier */
      if (rep->miscnt <= 0) continue;
      isr_tidputc(rep, ':');    /* print an item counter separator */
      isr_occout(rep, rep->occs[k]);
    }                           /* print number of contained items */
  }
  isr_tidputc(rep, '\n');       /* terminate the transaction id list */
}  /* output() */

/*--------------------------------------------------------------------*/

static void report (ISREPORT *rep, ITEM n)
{                               /* --- recursively report item sets */
  assert(rep && (n >= 0));      /* check the function arguments */
  while (n > 0) {               /* traverse the perfect extensions */
    rep->items[rep->cnt++] = rep->pexs[--n];
    if ((rep->cnt+n >= rep->zmin)  /* if a valid size can be reached */
    &&  (rep->cnt   <= rep->zmax)) /* (in the interval [min, max]), */
      report(rep, n);              /* recursively report supersets */
    if (--rep->cnt < rep->pfx)  /* remove the current item again */
      rep->pfx = rep->cnt;      /* and adapt the valid prefix */
  }
  if (rep->cnt >= rep->zmin)    /* if item set has minimum size, */
    output(rep);                /* report the current item set */
}  /* report() */

/*--------------------------------------------------------------------*/

int isr_report (ISREPORT *rep)
{                               /* --- report the current item set */
  ITEM   n, k;                  /* number of perfect extensions */
  ITEM   z;                     /* item set size */
  size_t m, c;                  /* buffers for item set counting */
  double w;                     /* buffer for an item set weight */
  RSUPP  s;                     /* support buffer */
  #ifdef ISR_CLOMAX             /* if closed/maximal filtering */
  RSUPP  r;                     /* support buffer */
  ITEM   *items;                /* item set for prefix tree update */
  #endif

  assert(rep);                  /* check the function argument */
  n = isr_pexcnt(rep);          /* get the number of perfect exts. */
  s = rep->supps[rep->cnt];     /* and the support of the item set */
  #ifdef ISR_CLOMAX             /* closed/maximal filtering support */
  if (rep->clomax) {            /* if a closed/maximal filter exists */
    r = cm_supp(rep->clomax);   /* get the maximal known support */
    if (r >= s)        return 0;/* check if item set is not closed */
    if (r >= rep->sto) return 0;/* check whether to store item set */
    k = rep->cnt +n;            /* compute the total number of items */
    if (n <= 0)                 /* if there are no perfect extensions */
      items = rep->items;       /* the items can be used directly */
    else {                      /* if there are perfect extensions */
      items = (ITEM*)memcpy(rep->iset,rep->pexs,(size_t)k*sizeof(ITEM));
      ia_qsort(items, (size_t)k, rep->dir);
    }                           /* copy and sort the items in the set */
    if (cm_update(rep->clomax, items, k, s) < 0)
      return -1;                /* add the item set to the filter */
    if ((rep->target & ISR_MAXIMAL) && (r >= 0))
      return  0;                /* check for a non-maximal item set */
  }                             /* (if the known support is > 0) */
  #endif
  if ((s < rep->smin) || (s > rep->smax))
    return 0;                   /* check the item set support */
  if ((rep->cnt   > rep->zmax)  /* if the item set is too large or */
  ||  (rep->cnt+n < rep->zmin)) /* the minimum size cannot be reached */
    return 0;                   /* with prefect extensions, abort */
  if (rep->fast < 0) {          /* if just to count the item sets */
    /* if no output is produced and no item sets can be filtered out, */
    /* compute the number of item sets in the perfect ext. hypercube. */
    s = rep->supps[rep->cnt];   /* get the support of the item set */
    if (rep->mode & ISR_NOEXPAND) {
      z = rep->cnt +n;          /* if not to expand perfect exts., */
      rep->stats[z] += 1;       /* count only one item set */
      rep->repcnt   += 1;       /* (for its size and overall) */
      #ifdef ISR_PATSPEC        /* if pattern spectrum functions */
      if (rep->psp && (psp_incfrq(rep->psp, z, s, 1) < 0))
        return -1;              /* if a pattern spectrum exists, */
      #endif                    /* count item set in pattern spectrum */
      return 0;                 /* return 'ok' */
    }
    m = 0; z = rep->cnt;        /* and init. the item set counter */
    if (z >= rep->zmin) {       /* if the item set is large enough */
      rep->stats[z] += 1;       /* count the current item set */
      m += 1;                   /* (for its size and overall) */
      #ifdef ISR_PATSPEC        /* if pattern spectrum functions */
      if (rep->psp && (psp_incfrq(rep->psp, z, s, 1) < 0))
        return -1;              /* if a pattern spectrum exists, */
      #endif                    /* count item set in pattern spectrum */
    }
    for (c = 1, k = 1; (k <= n) && (++z <= rep->zmax); k++) {
      c = (c *(size_t)(n-k+1))  /* compute n choose k */
        / (size_t)k;            /* for 1 <= k <= n */
      if (z >= rep->zmin) {     /* count the current item set */
        rep->stats[z] += c; m += c; /* (for its size and overall) */
        #ifdef ISR_PATSPEC      /* if pattern spectrum functions */
        if (rep->psp && (psp_incfrq(rep->psp, z, s, c) < 0))
          return -1;            /* if a pattern spectrum exists, */
        #endif                  /* count item set in pattern spectrum */
      }
    }                           /* (n choose k is the number of */
    rep->repcnt += m;           /* item sets of size rep->cnt +k) */
    return 0;                   /* return 'ok' */
  }
  /* It is debatable whether this way of handling perfect extensions  */
  /* in case no output is produced is acceptable for fair benchmarks, */
  /* because the sets in the hypercube are not explicitly generated.  */
  if (rep->fast)                /* format support for fast output */
    rep->fosize = sprintf(rep->foinfo, " (%"RSUPP_FMT")\n",
                          rep->supps[rep->cnt]);
  if (rep->mode & ISR_NOEXPAND){/* if not to expand perfect exts. */
    k = rep->cnt +n;            /* if all perfext extensions make */
    if (k > rep->zmax) return 0;/* the item set too large, abort */
    rep->supps[k] = rep->supps[rep->cnt];
    rep->wgts [k] = rep->wgts [rep->cnt];
    for (k = n; --k >= 0; )     /* add all perfect extensions */
      rep->items[rep->cnt++] = rep->pexs[k];
    if (rep->fast) fastout(rep, 0); /* report the expanded set */
    else           output (rep);    /* (fast or normal output) */
    rep->cnt -= n;              /* remove the perfect extensions */
    #ifndef NDEBUG              /* in debug mode */
    isr_flush(rep);             /* flush the output buffer */
    #endif                      /* after every item set */
    return 0;                   /* abort the function */
  }                             /* (all reporting has been done) */
  if (rep->fast)                /* if fast output is possible, */
    fastout(rep, n);            /* report item sets recursively */
  else {                        /* if fast output is not possible */
    s = rep->supps[rep->cnt];   /* set support and weights */
    w = rep->wgts [rep->cnt];   /* for perfect extension hypercube */
    for (k = 0; ++k <= n; ) { rep->supps[rep->cnt+k] = s;
                              rep->wgts [rep->cnt+k] = w; }
    report(rep, n);             /* recursively add perfect exts. and */
  }                             /* report the resulting item sets */
  #ifdef ISR_PATSPEC            /* if pattern spectrum functions */
  if (rep->psp && psp_error(rep->psp))
    return -1;                  /* check whether updating the */
  #endif                        /* pattern spectrum failed */
  #ifndef NDEBUG                /* in debug mode */
  isr_flush(rep);               /* flush the output buffer */
  #endif                        /* after every item set */
  return 0;                     /* return 'ok' */
}  /* isr_report() */

/*--------------------------------------------------------------------*/

int isr_reportv (ISREPORT *rep, double eval)
{                               /* --- report the current item set */
  assert(rep);                  /* check the function arguments */
  rep->eval = eval;             /* store the item set evaluation */
  return isr_report(rep);       /* report the current item set */
}  /* isr_reportv() */

/*--------------------------------------------------------------------*/

int isr_reportx (ISREPORT *rep, TID *tids, TID n)
{                               /* --- report the current item set */
  int r;                        /* result of function call */

  assert(rep);                  /* check the function arguments */
  rep->tids   = tids;           /* store the transaction id array */
  rep->tidcnt = n;              /* and the number of transaction ids */
  r = isr_report(rep);          /* report the current item set */
  rep->tids   = NULL;           /* clear the transaction id array */
  return r;                     /* return number of rep. item sets */
}  /* isr_reportx() */

/*--------------------------------------------------------------------*/

int isr_reporto (ISREPORT *rep, ITEM *occs, TID n)
{                               /* --- report the current item set */
  int r;                        /* result of function call */

  assert(rep);                  /* check the function arguments */
  rep->occs   = occs;           /* store the item occurrence array */
  rep->tids   = (TID*)-1;       /* and set the transaction id array */
  rep->tidcnt = n;              /* and the number of transaction ids */
  r = isr_report(rep);          /* report the current item set */
  rep->tids   = NULL;           /* clear the transaction id array */
  return r;                     /* return number of rep. item sets */
}  /* isr_reporto() */

/*--------------------------------------------------------------------*/

int isr_reprule (ISREPORT *rep, ITEM item,
                 RSUPP body, RSUPP head, double eval)
{                               /* --- report an association rule */
  ITEM  i, n, s;                /* loop variable, separator flag */
  RSUPP supp;                   /* support of current item set */

  assert(rep                    /* check the function arguments */
  &&    (item >= 0) && (body > 0) && (head >= 0));
  assert(isr_uses(rep, item));  /* head item must be in item set */
  supp = rep->supps[n = rep->cnt]; /* get the item set support */
  if ((supp < rep->smin) || (supp > rep->smax))
    return 0;                   /* check the item set support */
  if ((n    < rep->zmin) || (n    > rep->zmax))
    return 0;                   /* check the item set size */
  rep->stats[n] += 1;           /* count the reported rule */
  rep->repcnt   += 1;           /* (for its size and overall) */
  #ifdef ISR_PATSPEC            /* if pattern spectrum functions */
  if (rep->psp && (psp_incfrq(rep->psp, n, supp, 1) < 0))
    return -1;                  /* if a pattern spectrum exists, */
  #endif                        /* count item set in pattern spectrum */
  if (rep->rulefn) {            /* if a reporting function is given */
    rep->eval = eval;           /* note the evaluation */
    rep->rulefn(rep, rep->ruledat, item, body, head);
  }                             /* call the reporting function */
  if (!rep->file) return 0;     /* check for an output file */
  isr_puts(rep, rep->hdr);      /* print the record header */
  isr_puts(rep, rep->inames[item]);
  isr_puts(rep, rep->imp);      /* print rule head and impl. sign */
  for (i = s = 0; i < n; i++) {
    if (rep->items[i] == item) continue;
    if (s++ > 0) isr_puts(rep, rep->sep);
    isr_puts(rep, rep->inames[rep->items[i]]);
  }                             /* print items in rule body */
  isr_rinfo(rep, supp, body, head, eval);
  isr_putc (rep, '\n');         /* print the rule information */
  return 0;                     /* return 'ok' */
}  /* isr_reprule() */

/*--------------------------------------------------------------------*/

int isr_iset (ISREPORT *rep, const ITEM *items, ITEM n,
              RSUPP supp, double wgt, double eval)
{                               /* --- report an item set */
  ITEM i;                       /* buffer for the item counter */

  assert(rep                    /* check the function arguments */
  &&    (items || (n <= 0)) && (supp >= 0));
  if ((supp < rep->smin) || (supp > rep->smax))
    return 0;                   /* check the item set support */
  if ((n    < rep->zmin) || (n    > rep->zmax))
    return 0;                   /* check the item set size */
  if (rep->border               /* if there is a filtering border */
  && ((n >= rep->bdrcnt) || (supp < rep->border[n])))
    return 0;                   /* check the item set signature */
  rep->stats[n] += 1;           /* count the reported item set */
  rep->repcnt   += 1;           /* (for its size and overall) */
  #ifdef ISR_PATSPEC            /* if pattern spectrum functions */
  if (rep->psp && (psp_incfrq(rep->psp, n, supp, 1) < 0))
    return -1;                  /* if a pattern spectrum exists, */
  #endif                        /* count item set in pattern spectrum */
  if (rep->repofn) {            /* if there is a reporting function */
    isr_remove(rep, rep->cnt);  /* remove all items from the reporter */
    for (i = 0; i < n; i++)     /* and then add all given items */
      isr_addwgt(rep, items[i], supp, wgt);
    rep->repofn(rep, rep->repodat);
  }                             /* call the reporter function */
  if (!rep->file) return 0;     /* check for an output file */
  i = rep->cnt; rep->cnt = n;   /* note the number of items */
  isr_puts(rep, rep->hdr);      /* print the record header */
  if (n > 0)                    /* print the first item */
    isr_puts(rep, rep->inames[*items++]);
  while (--n > 0) {             /* traverse the remaining items */
    isr_puts(rep, rep->sep);    /* print an item separator */
    isr_puts(rep, rep->inames[*items++]);
  }                             /* print the next item */
  isr_sinfo(rep, supp, wgt, eval);
  isr_putc (rep, '\n');         /* print the item set information */
  rep->cnt = i;                 /* restore the number of items */
  return 0;                     /* return 'ok' */
}  /* isr_iset() */

/*--------------------------------------------------------------------*/

int isr_isetx (ISREPORT *rep, const ITEM *items, ITEM n,
               const double *iwgts,
               RSUPP supp, double wgt, double eval)
{                               /* --- report an item set */
  ITEM i;                       /* buffer for the item counter */

  assert(rep                    /* check the function arguments */
  &&    (items || (n <= 0)) && (supp >= 0));
  if ((supp < rep->smin) || (supp > rep->smax))
    return 0;                   /* check the item set support */
  if ((n    < rep->zmin) || (n    > rep->zmax))
    return 0;                   /* check the item set size */
  if (rep->border               /* if there is a filtering border */
  && ((n >= rep->bdrcnt) || (supp < rep->border[n])))
    return 0;                   /* check the item set signature */
  rep->stats[n] += 1;           /* count the reported item set */
  rep->repcnt   += 1;           /* (for its size and overall) */
  #ifdef ISR_PATSPEC            /* if pattern spectrum functions */
  if (rep->psp && (psp_incfrq(rep->psp, n, supp, 1) < 0))
    return -1;                  /* if a pattern spectrum exists, */
  #endif                        /* count item set in pattern spectrum */
  if (!rep->file) return 0;     /* check for an output file */
  i = rep->cnt; rep->cnt = n;   /* note the number of items */
  isr_puts(rep, rep->hdr);      /* print the record header */
  if (n > 0) {                  /* if at least one item */
    isr_puts(rep, rep->inames[*items]);
    isr_wgtout(rep, supp, *iwgts);
  }                             /* print first item and item weight */
  while (--n > 0) {             /* traverse the remaining items */
    isr_puts(rep, rep->sep);    /* print an item separator */
    isr_puts(rep, rep->inames[*++items]);
    isr_wgtout(rep, supp, *++iwgts);
  }                             /* print next item and item weight */
  isr_sinfo(rep, supp, wgt, eval);
  isr_putc (rep, '\n');         /* print the item set information */
  rep->cnt = i;                 /* restore the number of items */
  return 0;                     /* return 'ok' */
}  /* isr_isetx() */

/*--------------------------------------------------------------------*/

int isr_rule (ISREPORT *rep, const ITEM *items, ITEM n,
              RSUPP supp, RSUPP body, RSUPP head, double eval)
{                               /* --- report an association rule */
  ITEM i;                       /* buffer for the item counter */

  assert(rep                    /* check the function arguments */
  &&     items && (n > 0) && (supp > 0) && (body > 0) && (head > 0));
  if ((supp < rep->smin) || (supp > rep->smax))
    return 0;                   /* check the item set support */
  if ((n    < rep->zmin) || (n    > rep->zmax))
    return 0;                   /* check the item set size */
  rep->stats[n] += 1;           /* count the reported rule */
  rep->repcnt   += 1;           /* (for its size and overall) */
  if (rep->rulefn) {            /* if a reporting function is given */
    rep->eval = eval;           /* note the evaluation */
    rep->rulefn(rep, rep->ruledat, items[0], body, head);
  }                             /* call the reporting function */
  if (!rep->file) return 0;     /* check for an output file */
  i = rep->cnt; rep->cnt = n;   /* note the number of items */
  isr_puts(rep, rep->hdr);      /* print the record header */
  isr_puts(rep, rep->inames[*items++]);
  isr_puts(rep, rep->imp);      /* print the rule head and imp. sign */
  if (--n > 0)                  /* print the first item in body */
    isr_puts(rep, rep->inames[*items++]);
  while (--n > 0) {             /* traverse the remaining items */
    isr_puts(rep, rep->sep);    /* print an item separator */
    isr_puts(rep, rep->inames[*items++]);
  }                             /* print the next item */
  isr_rinfo(rep, supp, body, head, eval);
  isr_putc (rep, '\n');         /* print the rule information */
  rep->cnt = i;                 /* restore the number of items */
  return 0;                     /* return 'ok' */
}  /* isr_rule() */

/*--------------------------------------------------------------------*/

int isr_seqrule (ISREPORT *rep, const ITEM *items, ITEM n,
                 RSUPP supp, RSUPP body, RSUPP head, double eval)
{                               /* --- report a sequence rule */
  assert(rep                    /* check the function arguments */
  &&    (!items || (n > 0)) && (supp > 0) && (body > 0) && (head > 0));
  if (!items) {                 /* if no items given, use internal */
    items = rep->items; n = rep->cnt; }
  n -= 1;                       /* call function with explicit head */
  return isr_sxrule(rep, items, n, items[n], supp, body, head, eval);
}  /* isr_seqrule() */

/*--------------------------------------------------------------------*/

int isr_sxrule (ISREPORT *rep, const ITEM *ante, ITEM n, ITEM cons,
                RSUPP supp, RSUPP body, RSUPP head, double eval)
{                               /* --- report a sequence rule */
  ITEM i;                       /* buffer for the item counter */

  assert(rep                    /* check the function arguments */
  &&    (!ante || (n > 0)) && (cons >= 0)
  &&    (supp > 0) && (body > 0) && (head > 0));
  if (!ante) {                  /* if no ante given, use internal */
    ante = rep->items; n = rep->cnt; }
  if ((supp < rep->smin) || (supp > rep->smax))
    return 0;                   /* check the item set support */
  if ((n +1 < rep->zmin) || (n +1 > rep->zmax))
    return 0;                   /* check the item set size */
  rep->stats[n+1] += 1;         /* count the reported rule */
  rep->repcnt     += 1;         /* (for its size and overall) */
  #ifdef ISR_PATSPEC            /* if pattern spectrum functions */
  if (rep->psp && (psp_incfrq(rep->psp, n+1, supp, 1) < 0))
    return -1;                  /* if a pattern spectrum exists, */
  #endif                        /* count item set in pattern spectrum */
  if (rep->rulefn) {            /* if a reporting function is given */
    rep->eval = eval;           /* note the evaluation */
    rep->rulefn(rep, rep->ruledat, cons, body, head);
  }                             /* call the reporting function */
  if (!rep->file) return 0;     /* check for an output file */
  i = rep->cnt; rep->cnt = n+1; /* note the number of items */
  isr_puts(rep, rep->hdr);      /* print the record header */
  if (--n >= 0)                 /* print the first item in body */
    isr_puts(rep, rep->inames[*ante++]);
  while (--n >= 0) {            /* traverse the remaining items */
    isr_puts(rep, rep->sep);    /* print an item separator */
    isr_puts(rep, rep->inames[*ante++]);
  }                             /* print the next item */
  isr_puts (rep, rep->imp);     /* print the imp. sign and rule head */
  isr_puts (rep, rep->inames[cons]);
  isr_rinfo(rep, supp, body, head, eval);
  isr_putc (rep, '\n');         /* print the rule information */
  rep->cnt = i;                 /* restore the number of items */
  return 0;                     /* return 'ok' */
}  /* isr_sxrule() */

/*--------------------------------------------------------------------*/

int isr_extrule (ISREPORT *rep, const ITEM *items, ITEM n,
                 ITEM a, ITEM b, RSUPP body,
                 RSUPP supp, RSUPP head,
                 RSUPP salt, RSUPP halt, RSUPP join)
{                               /* --- report special seq. rule */
  ITEM i;                       /* buffer for the item counter */

  assert(rep                    /* check the function arguments */
  &&    (!items || (n > 0)) && (a >= 0) && (b >= 0)
  &&    (supp >= 0) && (body >= 0) && (head >= 0)
  &&    (salt >= 0) && (halt >= 0) && (join >= 0));
  if (!items) {                 /* if no items given, use internal */
    items = rep->items; n = rep->cnt; }
  if ((body < rep->smin) || (body > rep->smax))
    return 0;                   /* check the item set support */
  if ((n +1 < rep->zmin) || (n +1 > rep->zmax))
    return 0;                   /* check the item set size */
  rep->stats[n+1] += 1;         /* count the reported rule */
  rep->repcnt     += 1;         /* (for its size and overall) */
  if (!rep->file) return 0;     /* check for an output file */
  i = rep->cnt; rep->cnt = n+1; /* note the number of items */
  isr_puts(rep, rep->hdr);      /* print the record header */
  if (--n >= 0)                 /* print the first item in body */
    isr_puts(rep, rep->inames[*items++]);
  while (--n >= 0) {            /* traverse the remaining items */
    isr_puts(rep, rep->sep);    /* print an item separator */
    isr_puts(rep, rep->inames[*items++]);
  }                             /* print the next item */
  isr_puts (rep, rep->imp);     /* print the imp. sign and rule head */
  isr_puts (rep, rep->inames[a]);
  isr_puts (rep, rep->sep);     /* print the second rule head */
  isr_puts (rep, rep->inames[b]);
  isr_xinfo(rep, supp, body, head, salt, halt, join);
  isr_putc (rep, '\n');         /* print the rule information */
  rep->cnt = i;                 /* restore the number of items */
  return 0;                     /* return 'ok' */
}  /* isr_extrule() */

/*--------------------------------------------------------------------*/

void isr_reset (ISREPORT *rep)
{                               /* --- reset the output counters */
  ITEM n;                       /* number of items */

  assert(rep);                  /* check the function argument */
  rep->repcnt = 0;              /* reinit. number of reported sets */
  n = ib_cnt(rep->base);        /* clear the statistics array */
  memset(rep->stats, 0, (size_t)(n+1) *sizeof(size_t));
  #ifdef ISR_PATSPEC            /* if pattern spectrum functions */
  if (rep->psp) psp_clear(rep->psp);
  #endif                        /* clear the pattern spectrum */
}  /* isr_reset() */

/*--------------------------------------------------------------------*/

void isr_prstats (ISREPORT *rep, FILE *out, ITEM min)
{                               /* --- print item set statistics */
  ITEM i, n;                    /* loop variables */

  assert(rep && out);           /* check the function arguments */
  fprintf(out, "all: %"SIZE_FMT"\n", rep->repcnt);
  for (n = rep->size+1; --n >= 0; )
    if (rep->stats[n] != 0) break;
  for (i = min; i <= n; i++)    /* print set counters per set size */
    fprintf(out, "%3"ITEM_FMT": %"SIZE_FMT"\n", i, rep->stats[i]);
}  /* isr_prstats() */

/*--------------------------------------------------------------------*/
#ifdef ISR_PATSPEC

int isr_addpsp (ISREPORT *rep, PATSPEC *psp)
{                               /* --- add a pattern spectrum */
  assert(rep);                  /* check the function arguments */
  if (rep->psp) return 1;       /* if pattern spectrum exists, abort */
  if (!psp) {                   /* if to create a pattern spectrum */
    psp = psp_create(rep->zmin, rep->zmax, rep->smin, rep->smax);
    if (!psp) return -1;        /* create a pattern spectrum */
  }                             /* with the stored limits */
  rep->psp = psp;               /* note the pattern spectrum */
  return 0;                     /* return 'ok' */
}  /* isr_addpsp() */

/*--------------------------------------------------------------------*/

PATSPEC* isr_rempsp (ISREPORT *rep, int delpsp)
{                               /* --- add a pattern spectrum */
  PATSPEC *psp;                 /* existing pattern spectrum */

  assert(rep);                  /* check the function arguments */
  psp = rep->psp;               /* get the stored pattern spectrum */
  rep->psp = NULL;              /* and clear the reporter variable */
  if (!delpsp) return psp;      /* if not to delete it, return it */
  if (psp) psp_delete(psp);     /* delete existing pattern spectrum */
  return NULL;                  /* return that there is none anymore */
}  /* isr_rempsp() */

#endif
/*--------------------------------------------------------------------*/

int isr_sinfo (ISREPORT *rep, RSUPP supp, double wgt, double eval)
{                               /* --- print item set information */
  int        k, n = 0;          /* number of decimals, char. counter */
  double     sdbl, smax, wmax;  /* (maximum) support and weight */
  const char *s, *t;            /* to traverse the format */

  assert(rep);                  /* check the function arguments */
  if (!rep->info || !rep->file)
    return 0;                   /* check for a given format and file */
  sdbl = (double)supp;          /* get support as double prec. number */
  smax = (double)rep->supps[0]; /* get maximum support and */
  if (smax <= 0) smax = 1;      /* avoid divisions by zero */
  wmax =         rep->wgts[0];  /* get maximum weight  and */
  if (wmax <= 0) wmax = 1;      /* avoid divisions by zero */
  for (s = rep->info; *s; ) {   /* traverse the output format */
    if (*s != '%') {            /* copy everything except '%' */
      isr_putc(rep, *s++); n += 1; continue; }
    t = s++; k = getsd(s, &s);  /* get the number of signif. digits */
    switch (*s++) {             /* evaluate the indicator character */
      case '%': isr_putc(rep, '%'); n += 1;               break;
      case 'i': n += isr_intout(rep, (ptrdiff_t)rep->cnt);  break;
      #define int    1
      #define double 2
      #if RSUPP==double
      case 'a': n += isr_numout(rep,      sdbl,       k); break;
      case 'q': n += isr_numout(rep,      smax,       k); break;
      case 'Q': n += isr_numout(rep,      smax,       k); break;
      #else
      case 'a': n += isr_intout(rep, (ptrdiff_t)supp);    break;
      case 'q': n += isr_intout(rep, (ptrdiff_t)smax);    break;
      case 'Q': n += isr_intout(rep, (ptrdiff_t)smax);    break;
      #endif
      #undef int
      #undef double
      case 's': n += isr_numout(rep,      sdbl/smax,  k); break;
      case 'S': n += isr_numout(rep, 100*(sdbl/smax), k); break;
      case 'x': n += isr_numout(rep,      sdbl/smax,  k); break;
      case 'X': n += isr_numout(rep, 100*(sdbl/smax), k); break;
      case 'w': n += isr_numout(rep,      wgt,        k); break;
      case 'W': n += isr_numout(rep, 100* wgt,        k); break;
      case 'r': n += isr_numout(rep,      wgt /wmax,  k); break;
      case 'R': n += isr_numout(rep, 100*(wgt /wmax), k); break;
      case 'z': n += isr_numout(rep,      wgt *smax,  k); break;
      case 'e': n += isr_numout(rep,      eval,       k); break;
      case 'E': n += isr_numout(rep, 100* eval,       k); break;
      case 'p': n += isr_numout(rep,      eval,       k); break;
      case 'P': n += isr_numout(rep, 100* eval,       k); break;
      case  0 : --s;            /* print the requested quantity */
    default : isr_putsn(rep, t, k = (int)(s-t)); n += k; t = s; break;
    }                           /* otherwise copy characters */
  }
  return n;                     /* return the number of characters */
}  /* isr_sinfo() */

/*--------------------------------------------------------------------*/

int isr_rinfo (ISREPORT *rep, RSUPP supp, RSUPP body, RSUPP head,
               double eval)
{                               /* --- print ass. rule information */
  int        k, n = 0;          /* number of decimals, char. counter */
  double     smax;              /* maximum support (database size) */
  double     conf, lift;        /* buffers for computations */
  const char *s, *t;            /* to traverse the format */

  assert(rep);                  /* check the function arguments */
  if (!rep->info || !rep->file)
    return 0;                   /* check for a given format and file */
  smax = (double)rep->supps[0]; /* get the total transaction weight */
  if (smax <= 0) smax = 1;      /* avoid divisions by zero */
  for (s = rep->info; *s; ) {   /* traverse the output format */
    if (*s != '%') {            /* copy everything except '%' */
      isr_putc(rep, *s++); n += 1; continue; }
    t = s++; k = getsd(s, &s);  /* get the number of signif. digits */
    switch (*s++) {             /* evaluate the indicator character */
      case '%': isr_putc(rep, '%'); n += 1;                       break;
      case 'i': n += isr_intout(rep, (ptrdiff_t)rep->cnt);        break;
      #define int    1
      #define double 2
      #if RSUPP==double
      case 'a': n += isr_numout(rep,            supp,         k); break;
      case 'b': n += isr_numout(rep,            body,         k); break;
      case 'h': n += isr_numout(rep,            head,         k); break;
      case 'Q': n += isr_numout(rep,            smax,         k); break;
      #else
      case 'a': n += isr_intout(rep, (ptrdiff_t)supp);            break;
      case 'b': n += isr_intout(rep, (ptrdiff_t)body);            break;
      case 'h': n += isr_intout(rep, (ptrdiff_t)head);            break;
      case 'Q': n += isr_intout(rep, (ptrdiff_t)smax);            break;
      #endif
      #undef int
      #undef double
      case 's': n += isr_numout(rep,      (double)supp/smax,  k); break;
      case 'S': n += isr_numout(rep, 100*((double)supp/smax), k); break;
      case 'x': n += isr_numout(rep,      (double)body/smax,  k); break;
      case 'X': n += isr_numout(rep, 100*((double)body/smax), k); break;
      case 'y': n += isr_numout(rep,      (double)head/smax,  k); break;
      case 'Y': n += isr_numout(rep, 100*((double)head/smax), k); break;
      case 'c': conf = (body > 0) ? (double)supp/(double)body : 0;
                n += isr_numout(rep,      conf,               k); break;
      case 'C': conf = (body > 0) ? (double)supp/(double)body : 0;
                n += isr_numout(rep, 100* conf,               k); break;
      case 'l': lift = ((body <= 0) || (head <= 0)) ? 0
                     : ((double)supp*smax) /((double)body*(double)head);
                n += isr_numout(rep,      lift,               k); break;
      case 'L': lift = ((body <= 0) || (head <= 0)) ? 0
                     : ((double)supp*smax) /((double)body*(double)head);
                n += isr_numout(rep, 100* lift,               k); break;
      case 'e': n += isr_numout(rep,      eval,               k); break;
      case 'E': n += isr_numout(rep, 100* eval,               k); break;
      case  0 : --s;            /* print the requested quantity */
      default : isr_putsn(rep, t, k = (int)(s-t)); n += k; t = s; break;
    }                           /* otherwise copy characters */
  }
  return n;                     /* return the number of characters */
}  /* isr_rinfo() */

/*--------------------------------------------------------------------*/

int isr_xinfo (ISREPORT *rep, RSUPP supp, RSUPP body, RSUPP head,
               RSUPP salt, RSUPP halt, RSUPP join)
{                               /* --- print ext. rule information */
  int        k, n = 0;          /* number of decimals, char. counter */
  double     smax;              /* (maximum) support (database size) */
  double     conf, lift, x, y;  /* buffers for computations */
  const char *s, *t;            /* to traverse the format */

  assert(rep);                  /* check the function arguments */
  if (!rep->info || !rep->file)
    return 0;                   /* check for a given format and file */
  smax = (double)rep->supps[0]; /* get the total transaction weight */
  if (smax <= 0) smax = 1;      /* avoid divisions by zero */
  for (s = rep->info; *s; ) {   /* traverse the output format */
    if (*s != '%') {            /* copy everything except '%' */
      isr_putc(rep, *s++); n += 1; continue; }
    t = s++; k = getsd(s, &s);  /* get the number of signif. digits */
    switch (*s++) {             /* evaluate the indicator character */
      case '%': isr_putc(rep, '%'); n += 1;                       break;
      case 'i': n += isr_intout(rep,  (ptrdiff_t) rep->cnt+2);    break;
      #define int    1
      #define double 2
      #if RSUPP==double
      case 'a': n += isr_numout(rep,              supp,       k); break;
      case 'A': n += isr_numout(rep,              salt,       k); break;
      case 'b': n += isr_numout(rep,              body,       k); break;
      case 'h': n += isr_numout(rep,              head,       k); break;
      case 'H': n += isr_numout(rep,              halt,       k); break;
      case 'B': n += isr_numout(rep,              join,       k); break;
      case 'o': n += isr_numout(rep,              supp-join,  k); break;
      case 'O': n += isr_numout(rep,              salt-join,  k); break;
      case 'Q': n += isr_numout(rep,              smax,       k); break;
      #else
      case 'a': n += isr_intout(rep,  (ptrdiff_t) supp);          break;
      case 'A': n += isr_intout(rep,  (ptrdiff_t) salt);          break;
      case 'b': n += isr_intout(rep,  (ptrdiff_t) body);          break;
      case 'h': n += isr_intout(rep,  (ptrdiff_t) head);          break;
      case 'H': n += isr_intout(rep,  (ptrdiff_t) halt);          break;
      case 'B': n += isr_intout(rep,  (ptrdiff_t) join);          break;
      case 'o': n += isr_intout(rep,  (ptrdiff_t)(supp-join));    break;
      case 'O': n += isr_intout(rep,  (ptrdiff_t)(salt-join));    break;
      case 'Q': n += isr_intout(rep,  (ptrdiff_t) smax);          break;
      #endif
      #undef int
      #undef double
      case 's': n += isr_numout(rep,      (double)supp/smax,  k); break;
      case 'S': n += isr_numout(rep, 100*((double)supp/smax), k); break;
      case 't': n += isr_numout(rep,      (double)salt/smax,  k); break;
      case 'T': n += isr_numout(rep, 100*((double)salt/smax), k); break;
      case 'x': n += isr_numout(rep,      (double)body/smax,  k); break;
      case 'X': n += isr_numout(rep, 100*((double)body/smax), k); break;
      case 'y': n += isr_numout(rep,      (double)head/smax,  k); break;
      case 'Y': n += isr_numout(rep, 100*((double)head/smax), k); break;
      case 'z': n += isr_numout(rep,      (double)halt/smax,  k); break;
      case 'Z': n += isr_numout(rep, 100*((double)halt/smax), k); break;
      case 'w': n += isr_numout(rep,      (double)join/smax,  k); break;
      case 'W': n += isr_numout(rep, 100*((double)join/smax), k); break;
      case 'u': x  = (double)(supp-join);
                n += isr_numout(rep,      (double)x   /smax,  k); break;
      case 'U': x  = (double)(supp-join);
                n += isr_numout(rep, 100*((double)x   /smax), k); break;
      case 'v': x  = (double)(salt-join);
                n += isr_numout(rep,      (double)x   /smax,  k); break;
      case 'V': x  = (double)(salt-join);
                n += isr_numout(rep, 100*((double)x   /smax), k); break;
      case 'c': conf = (body > 0) ? (double)supp/(double)body : 0;
                n += isr_numout(rep,      conf,               k); break;
      case 'C': conf = (body > 0) ? (double)supp/(double)body : 0;
                n += isr_numout(rep, 100* conf,               k); break;
      case 'd': x  = (double)(body-supp);
                conf = (body > 0) ? (double)x   /(double)body : 0;
                n += isr_numout(rep,      conf,               k); break;
      case 'D': x  = (double)(body-supp);
                conf = (body > 0) ? (double)x   /(double)body : 0;
                n += isr_numout(rep, 100* conf,               k); break;
      case 'f': conf = (body > 0) ? (double)salt/(double)body : 0;
                n += isr_numout(rep,      conf,               k); break;
      case 'F': conf = (body > 0) ? (double)salt/(double)body : 0;
                n += isr_numout(rep, 100* conf,               k); break;
      case 'g': x  = (double)(body-salt);
                conf = (body > 0) ? (double)x   /(double)body : 0;
                n += isr_numout(rep,      conf,               k); break;
      case 'G': x  = (double)(body-salt);
                conf = (body > 0) ? (double)x   /(double)body : 0;
                n += isr_numout(rep, 100* conf,               k); break;
      case 'j': x  = (double)(supp-join);
                conf = (body > 0) ? (double)x   /(double)body : 0;
                n += isr_numout(rep,      conf,               k); break;
      case 'J': x  = (double)(supp-join);
                conf = (body > 0) ? (double)x   /(double)body : 0;
                n += isr_numout(rep, 100* conf,               k); break;
      case 'k': x  = (double)(salt-join);
                conf = (body > 0) ? (double)x   /(double)body : 0;
                n += isr_numout(rep,      conf,               k); break;
      case 'K': x  = (double)(salt-join);
                conf = (body > 0) ? (double)x   /(double)body : 0;
                n += isr_numout(rep, 100* conf,               k); break;
      case 'm': x  = (double)(supp-join); y = (double)(body-salt);
                conf = (y != 0) ? x/y : 0;
                n += isr_numout(rep,      conf,               k); break;
      case 'M': x  = (double)(supp-join); y = (double)(body-salt);
                conf = (y != 0) ? x/y : 0;
                n += isr_numout(rep, 100* conf,               k); break;
      case 'n': x  = (double)(salt-join); y = (double)(body-supp);
                conf = (y != 0) ? x/y : 0;
                n += isr_numout(rep,      conf,               k); break;
      case 'N': x  = (double)(salt-join); y = (double)(body-supp);
                conf = (y != 0) ? x/y : 0;
                n += isr_numout(rep, 100* conf,               k); break;
      case 'l': lift = ((body <= 0) || (head <= 0)) ? 0
                     : ((double)supp*smax) /((double)body*(double)head);
                n += isr_numout(rep,      lift,               k); break;
      case 'L': lift = ((body <= 0) || (head <= 0)) ? 0
                     : ((double)supp*smax) /((double)body*(double)head);
                n += isr_numout(rep, 100* lift,               k); break;
      case  0 : --s;            /* print the requested quantity */
      default : isr_putsn(rep, t, k = (int)(s-t)); n += k; t = s; break;
    }                           /* otherwise copy characters */
  }
  return n;                     /* return the number of characters */
}  /* isr_xinfo() */

/*--------------------------------------------------------------------*/

void isr_getinfo (ISREPORT *rep, const char *sel, double *vals)
{                               /* --- get item set information */
  double supp;                  /* support of current item set */
  double wgt;                   /* weight of current item set */
  double smax, wmax;            /* maximum support and weight */

  assert(rep && sel && vals);   /* check the function arguments */
  supp = (double)rep->supps[rep->cnt];
  smax = (double)rep->supps[0]; /* get (maximum) support and */
  if (smax <= 0) smax = 1;      /* avoid divisions by zero */
  wgt  =         rep->wgts [rep->cnt];
  wmax =         rep->wgts [0]; /* get (maximum) weight and */
  if (wmax <= 0) wmax = 1;      /* avoid divisions by zero */
  for (; *sel; sel++, vals++) { /* traverse the information selectors */
    switch (*sel) {             /* and evaluate them */
      case 'i': *vals = (double)rep->cnt; break;
      case 'n': *vals =      supp;        break;
      case 'd': *vals =      supp;        break;
      case 'a': *vals =      supp;        break;
      case 'q': *vals =      smax;        break;
      case 'Q': *vals =      smax;        break;
      case 's': *vals =      supp/smax;   break;
      case 'S': *vals = 100*(supp/smax);  break;
      case 'x': *vals =      supp/smax;   break;
      case 'X': *vals = 100*(supp/smax);  break;
      case 'w': *vals =      wgt;         break;
      case 'W': *vals = 100* wgt;         break;
      case 'r': *vals =      wgt /wmax;   break;
      case 'R': *vals = 100*(wgt /wmax);  break;
      case 'z': *vals = 100*(wgt *smax);  break;
      case 'e': *vals =      rep->eval;   break;
      case 'E': *vals = 100* rep->eval;   break;
      case 'p': *vals =      rep->eval;   break;
      case 'P': *vals = 100* rep->eval;   break;
      default : *vals =   0;              break;
    }                           /* store the corresponding value */
  }                             /* in the output vector */
}  /* isr_getinfo() */
