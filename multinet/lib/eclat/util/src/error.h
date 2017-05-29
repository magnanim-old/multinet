/*----------------------------------------------------------------------
  File    : error.h
  Contents: generic error reporting function definition
  Author  : Christian Borgelt
  History : 2010.12.11 file created
            2013.09.06 indication of how to get a usage message added
----------------------------------------------------------------------*/
#ifndef __ERROR__
#define __ERROR__
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#ifdef STORAGE
#include "storage.h"
#endif

/*----------------------------------------------------------------------
  Preprocessor Definitions
----------------------------------------------------------------------*/

#ifdef QUIET                    /* if quiet version */
  #define REPORT                /* do not add reporting code */
#else                           /* if verbose version */
  #define REPORT                /* add error reporting code */         \
  int        k;                 /* maximal error code */               \
  va_list    args;              /* list of variable arguments */       \
  const char *msg;              /* error message */                    \
                                                                       \
  assert(prgname);              /* check the program name */           \
  va_start(args, code);         /* start variable arguments */         \
  if      (code > 0) {          /* if an error message is given, */    \
    msg = va_arg(args, const char*);        /* print it directly */    \
    if (msg) fprintf(stderr, "\n%s: %s\n", prgname, msg); }            \
  else if (code < 0) {          /* if code and arguments are given */  \
    k = 1-(int)(sizeof(errmsgs)/sizeof(*errmsgs));                     \
    if (code < k) code = k;     /* check and adapt the error code */   \
    msg = errmsgs[-code];       /* get the error message format */     \
    if (!msg) msg = errmsgs[-k];/* check and adapt the message */      \
    fprintf(stderr, "\n%s: ", prgname);                                \
    vfprintf(stderr, msg, args);/* print the error message and */      \
    fputc('\n', stderr);        /* terminate the output line */        \
  }                                                                    \
  va_end(args);                 /* end variable arguments */
#endif

#if 0
    fprintf(stderr, "Call the program without any arguments "          \
                    "for a usage message.\n");                         \

#endif
/*--------------------------------------------------------------------*/

#define CLEANUP

/*--------------------------------------------------------------------*/

#ifdef STORAGE
#define SHOWMEM       showmem("at end of program", 1)
#else
#define SHOWMEM
#endif

/*--------------------------------------------------------------------*/

#define noexit(code)            /* do not exit at end of function */

/*--------------------------------------------------------------------*/

#define GENERROR(name,EXIT) \
static int name (int code, ...)                                        \
{                               /* --- print an error message */       \
  REPORT;                       /* report the error (print message) */ \
  CLEANUP;                      /* clean up memory and close files */  \
  SHOWMEM;                      /* show (final) memory usage */        \
  code = abs(code);             /* get positive error code */          \
  EXIT(code);                   /* abort the program (if requested) */ \
  return code;                  /* return (positive) error code */     \
}  /* name() */

#endif
