/*----------------------------------------------------------------------
  File    : ruleval.h
  Contents: rule evaluation measures
  Author  : Christian Borgelt
  History : 2011.07.22 file created
            2012.02.15 function re_supp() added (rule support)
            2013.03.29 adapted to type changes in module tract (SUPP)
            2014.05.15 functions re_cprob() and re_import() added
----------------------------------------------------------------------*/
#ifndef __RULEVAL__
#define __RULEVAL__
#include "tract.h"

/*----------------------------------------------------------------------
  Preprocessor Definitions
----------------------------------------------------------------------*/
/* --- rule evaluation function identifiers --- */
#define RE_NONE        0        /* no measure / constant zero */
#define RE_SUPP        1        /* rule support (body and head) */
#define RE_CONFIDENCE  2        /* rule confidence */
#define RE_CONF        2        /* rule confidence */
#define RE_CONFDIFF    3        /* confidence diff. to prior */
#define RE_LIFT        4        /* lift value (conf./prior) */
#define RE_LIFTDIFF    5        /* difference of lift value    to 1 */
#define RE_LIFTQUOT    6        /* difference of lift quotient to 1 */
#define RE_CONVICTION  7        /* conviction */
#define RE_CVCT        7        /* conviction */
#define RE_CVCTDIFF    8        /* difference of conviction  to 1 */
#define RE_CVCTQUOT    9        /* difference of conv. quot. to 1 */
#define RE_CPROB      10        /* conditional probability ratio */
#define RE_IMPORT     11        /* importance */
#define RE_IMPORTANCE 11        /* importance */
#define RE_CERTAINTY  12        /* certainty factor */
#define RE_CERT       12        /* certainty factor */
#define RE_CHI2       13        /* normalized chi^2 measure */
#define RE_CHI2PVAL   14        /* p-value from chi^2 measure */
#define RE_YATES      15        /* normalized chi^2 measure (Yates) */
#define RE_YATESPVAL  16        /* p-value from chi^2 measure (Yates) */
#define RE_INFO       17        /* information diff. to prior */
#define RE_INFOPVAL   18        /* p-value from info diff. */
#define RE_FETPROB    19        /* Fisher's exact test (prob.) */
#define RE_FETCHI2    20        /* Fisher's exact test (chi^2) */
#define RE_FETINFO    21        /* Fisher's exact test (info.) */
#define RE_FETSUPP    22        /* Fisher's exact test (supp.) */
#define RE_FNCNT      23        /* number of evaluation functions */

/*----------------------------------------------------------------------
  Type Definitions
----------------------------------------------------------------------*/
typedef double RULEVALFN (SUPP supp, SUPP body, SUPP head, SUPP base);

/*----------------------------------------------------------------------
  Rule Evaluation Functions
----------------------------------------------------------------------*/
extern double re_none      (SUPP supp, SUPP body, SUPP head, SUPP base);
extern double re_supp      (SUPP supp, SUPP body, SUPP head, SUPP base);
extern double re_conf      (SUPP supp, SUPP body, SUPP head, SUPP base);
extern double re_confdiff  (SUPP supp, SUPP body, SUPP head, SUPP base);
extern double re_lift      (SUPP supp, SUPP body, SUPP head, SUPP base);
extern double re_liftdiff  (SUPP supp, SUPP body, SUPP head, SUPP base);
extern double re_liftquot  (SUPP supp, SUPP body, SUPP head, SUPP base);
extern double re_cvct      (SUPP supp, SUPP body, SUPP head, SUPP base);
extern double re_cvctdiff  (SUPP supp, SUPP body, SUPP head, SUPP base);
extern double re_cvctquot  (SUPP supp, SUPP body, SUPP head, SUPP base);
extern double re_cprob     (SUPP supp, SUPP body, SUPP head, SUPP base);
extern double re_import    (SUPP supp, SUPP body, SUPP head, SUPP base);
extern double re_cert      (SUPP supp, SUPP body, SUPP head, SUPP base);
extern double re_chi2      (SUPP supp, SUPP body, SUPP head, SUPP base);
extern double re_chi2pval  (SUPP supp, SUPP body, SUPP head, SUPP base);
extern double re_yates     (SUPP supp, SUPP body, SUPP head, SUPP base);
extern double re_yatespval (SUPP supp, SUPP body, SUPP head, SUPP base);
extern double re_info      (SUPP supp, SUPP body, SUPP head, SUPP base);
extern double re_infopval  (SUPP supp, SUPP body, SUPP head, SUPP base);
extern double re_fetprob   (SUPP supp, SUPP body, SUPP head, SUPP base);
extern double re_fetchi2   (SUPP supp, SUPP body, SUPP head, SUPP base);
extern double re_fetinfo   (SUPP supp, SUPP body, SUPP head, SUPP base);
extern double re_fetsupp   (SUPP supp, SUPP body, SUPP head, SUPP base);

extern RULEVALFN* re_function (int id);
extern int        re_dir      (int id);

#endif
