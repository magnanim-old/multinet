#include <R.h>
#include <Rinternals.h>
#include <stdlib.h> // for NULL
#include <R_ext/Rdynload.h>


/* .Call calls */
extern SEXP _rcpp_module_boot_multinet(void);

static const R_CallMethodDef CallEntries[] = {
      {"_rcpp_module_boot_multinet", (DL_FUNC) &_rcpp_module_boot_multinet, 0},
      {NULL, NULL, 0}
   };
