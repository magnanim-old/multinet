#include <Rcpp.h>

using namespace Rcpp;

RcppExport SEXP _rcpp_module_boot_multinet();

static const R_CallMethodDef CallEntries[] = {
    {"_rcpp_module_boot_multinet", (DL_FUNC) &_rcpp_module_boot_multinet, 0},
    {NULL, NULL, 0}
};

RcppExport void R_init_anRpackage(DllInfo *dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}
