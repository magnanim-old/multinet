/*----------------------------------------------------------------------
  File    : arrays.h
  Contents: some basic array operations, especially for pointer arrays
  Author  : Christian Borgelt
  History : 1996.09.16 file created as arrays.h
            1999.02.04 long int changed to int (32 bit systems)
            2001.06.03 function  ptr_shuffle() added
            2002.01.02 functions for basic data types added
            2002.03.03 functions #_reverse() added
            2003.08.21 functions #_heapsort() added
            2007.01.16 shuffle functions for basic data types added
            2008.08.01 renamed to arrays.h, some functions added
            2008.10.05 functions #_select() added
            2010.07.31 index array sorting functions added
            2011.09.28 function ptr_mrgsort() added (merge sort)
            2012.06.03 functions for data type long int added
            2013.03.07 direction parameter added to sorting functions
            2013.03.10 binary search and bisection functions separated
            2013.03.20 adapted return values and arguments to ptrdiff_t
            2013.03.27 index sorting for types int, long and ptrdiff_t
            2015.07.30 object functions added (up to maximum size)
----------------------------------------------------------------------*/
#ifndef __ARRAYS__
#define __ARRAYS__
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "fntypes.h"

#define diff_t        ptrdiff_t /* signed variant of size_t */
#define OBJ_MAXSIZE   256       /* maximum size of objects */

/*----------------------------------------------------------------------
  Type Definitions
----------------------------------------------------------------------*/
typedef int int_CMPFN (int    i1, int    i2, void *data);
typedef int lng_CMPFN (long   i1, long   i2, void *data);
typedef int dif_CMPFN (diff_t i1, diff_t i2, void *data);

/*----------------------------------------------------------------------
  Functions for Arrays of Basic Data Types
----------------------------------------------------------------------*/
extern void   sht_clear    (short  *array, size_t n);
extern void   sht_copy     (short  *dst,   short *src, size_t n);
extern void   sht_move     (short  *array, size_t off, size_t n,
                                           size_t pos);
extern void   sht_select   (short  *array, size_t n,
                                           size_t k, RANDFN *rand);
extern void   sht_shuffle  (short  *array, size_t n, RANDFN *rand);
extern void   sht_reverse  (short  *array, size_t n);
extern void   sht_qsort    (short  *array, size_t n, int dir);
extern void   sht_heapsort (short  *array, size_t n, int dir);
extern size_t sht_unique   (short  *array, size_t n);
extern diff_t sht_bsearch  (short  key, const short  *array, size_t n);
extern size_t sht_bisect   (short  key, const short  *array, size_t n);

/*--------------------------------------------------------------------*/

extern void   int_clear    (int    *array, size_t n);
extern void   int_copy     (int    *dst,   int *src, size_t n);
extern void   int_move     (int    *array, size_t off, size_t n,
                                           size_t pos);
extern void   int_select   (int    *array, size_t n,
                                           size_t k, RANDFN *rand);
extern void   int_shuffle  (int    *array, size_t n, RANDFN *rand);
extern void   int_reverse  (int    *array, size_t n);
extern void   int_qsort    (int    *array, size_t n, int dir);
extern void   int_heapsort (int    *array, size_t n, int dir);
extern size_t int_unique   (int    *array, size_t n);
extern diff_t int_bsearch  (int    key, const int    *array, size_t n);
extern size_t int_bisect   (int    key, const int    *array, size_t n);

/*--------------------------------------------------------------------*/

extern void   lng_clear    (long   *array, size_t n);
extern void   lng_copy     (long   *dst,   long *src, size_t n);
extern void   lng_move     (long   *array, size_t off, size_t n,
                                           size_t pos);
extern void   lng_select   (long   *array, size_t n,
                                           size_t k, RANDFN *rand);
extern void   lng_shuffle  (long   *array, size_t n, RANDFN *rand);
extern void   lng_reverse  (long   *array, size_t n);
extern void   lng_qsort    (long   *array, size_t n, int dir);
extern void   lng_heapsort (long   *array, size_t n, int dir);
extern size_t lng_unique   (long   *array, size_t n);
extern diff_t lng_bsearch  (long   key, const long   *array, size_t n);
extern size_t lng_bisect   (long   key, const long   *array, size_t n);

/*--------------------------------------------------------------------*/

extern void   dif_clear    (diff_t *array, size_t n);
extern void   dif_copy     (diff_t *dst,   diff_t *src,   size_t n);
extern void   dif_move     (diff_t *array, size_t off,    size_t n,
                                           size_t pos);
extern void   dif_select   (diff_t *array, size_t n,
                                           size_t k, RANDFN *rand);
extern void   dif_shuffle  (diff_t *array, size_t n, RANDFN *rand);
extern void   dif_reverse  (diff_t *array, size_t n);
extern void   dif_qsort    (diff_t *array, size_t n, int dir);
extern void   dif_heapsort (diff_t *array, size_t n, int dir);
extern size_t dif_unique   (diff_t *array, size_t n);
extern diff_t dif_bsearch  (diff_t key, const diff_t *array, size_t n);
extern size_t dif_bisect   (diff_t key, const diff_t *array, size_t n);

/*--------------------------------------------------------------------*/

extern void   siz_clear    (size_t *array, size_t n);
extern void   siz_copy     (size_t *dst,   size_t  *src, size_t n);
extern void   siz_move     (size_t *array, size_t off, size_t n,
                                           size_t pos);
extern void   siz_select   (size_t *array, size_t n,
                                           size_t k, RANDFN *rand);
extern void   siz_shuffle  (size_t *array, size_t n, RANDFN *rand);
extern void   siz_reverse  (size_t *array, size_t n);
extern void   siz_qsort    (size_t *array, size_t n, int dir);
extern void   siz_heapsort (size_t *array, size_t n, int dir);
extern size_t siz_unique   (size_t *array, size_t n);
extern diff_t siz_bsearch  (size_t key, const size_t *array, size_t n);
extern size_t siz_bisect   (size_t key, const size_t *array, size_t n);

/*--------------------------------------------------------------------*/

extern void   flt_clear    (float  *array, size_t n);
extern void   flt_copy     (float  *dst,   float *src, size_t n);
extern void   flt_move     (float  *array, size_t off, size_t n,
                                           size_t pos);
extern void   flt_select   (float  *array, size_t n,
                                           size_t k, RANDFN *rand);
extern void   flt_shuffle  (float  *array, size_t n, RANDFN *rand);
extern void   flt_reverse  (float  *array, size_t n);
extern void   flt_qsort    (float  *array, size_t n, int dir);
extern void   flt_heapsort (float  *array, size_t n, int dir);
extern size_t flt_unique   (float  *array, size_t n);
extern diff_t flt_bsearch  (float  key, const float  *array, size_t n);
extern size_t flt_bisect   (float  key, const float  *array, size_t n);

/*--------------------------------------------------------------------*/

extern void   dbl_clear    (double *array, size_t n);
extern void   dbl_copy     (double *dst,   double *src, size_t n);
extern void   dbl_move     (double *array, size_t off,  size_t n,
                                           size_t pos);
extern void   dbl_select   (double *array, size_t n,
                                           size_t k, RANDFN *rand);
extern void   dbl_shuffle  (double *array, size_t n, RANDFN *rand);
extern void   dbl_reverse  (double *array, size_t n);
extern void   dbl_qsort    (double *array, size_t n, int dir);
extern void   dbl_heapsort (double *array, size_t n, int dir);
extern size_t dbl_unique   (double *array, size_t n);
extern diff_t dbl_bsearch  (double key, const double *array, size_t n);
extern size_t dbl_bisect   (double key, const double *array, size_t n);

/*----------------------------------------------------------------------
  Functions for Pointer Arrays
----------------------------------------------------------------------*/
extern void   ptr_clear    (void *array, size_t n);
extern void   ptr_copy     (void *dst, void *src, size_t n);
extern void   ptr_move     (void *array, size_t off, size_t n,
                                         size_t pos);
extern void   ptr_select   (void *array, size_t n,
                                         size_t k, RANDFN *rand);
extern void   ptr_shuffle  (void *array, size_t n, RANDFN *rand);
extern void   ptr_reverse  (void *array, size_t n);
extern void   ptr_qsort    (void *array, size_t n, int dir,
                            CMPFN *cmp, void *data);
extern void   ptr_heapsort (void *array, size_t n, int dir,
                            CMPFN *cmp, void *data);
extern int    ptr_mrgsort  (void *array, size_t n, int dir,
                            CMPFN *cmp, void *data, void *buf);
extern size_t ptr_unique   (void *array, size_t n,
                            CMPFN *cmp, void *data, OBJFN *del);
extern diff_t ptr_bsearch  (const void *key, const void *array,
                            size_t n, CMPFN *cmp, void *data);
extern size_t ptr_bisect   (const void *key, const void *array,
                            size_t n, CMPFN *cmp, void *data);

/*----------------------------------------------------------------------
  Functions for Object Arrays (with object size up to OBJ_MAXSIZE)
----------------------------------------------------------------------*/
extern void   obj_clear    (void *array, size_t n, size_t size);
extern void   obj_copy     (void *dst, void *src,
                            size_t n, size_t size);
extern void   obj_move     (void *array, size_t off, size_t n,
                            size_t pos, size_t size);
extern void   obj_select   (void *array, size_t n, size_t size,
                                         size_t k, RANDFN *rand);
extern void   obj_shuffle  (void *array, size_t n, size_t size,
                                                   RANDFN *rand);
extern void   obj_reverse  (void *array, size_t n, size_t size);
extern void   obj_qsort    (void *array, size_t n, size_t size,
                            int dir, CMPFN *cmp, void *data);
extern void   obj_heapsort (void *array, size_t n, size_t size,
                            int dir, CMPFN *cmp, void *data);
extern size_t obj_unique   (void *array, size_t n, size_t size,
                            CMPFN *cmp, void *data);
extern diff_t obj_bsearch  (const void *key, const void *array,
                            size_t n, size_t size,
                            CMPFN *cmp, void *data);
extern size_t obj_bisect   (const void *key, const void *array,
                            size_t n, size_t size,
                            CMPFN *cmp, void *data);

/*----------------------------------------------------------------------
  Functions for Integer Index Arrays
----------------------------------------------------------------------*/
extern void   i2i_qsort    (int *index, size_t n, int dir,
                            const int    *array);
extern void   i2i_heapsort (int *index, size_t n, int dir,
                            const int    *array);
extern void   i2l_qsort    (int *index, size_t n, int dir,
                            const long   *array);
extern void   i2l_heapsort (int *index, size_t n, int dir,
                            const long   *array);
extern void   i2x_qsort    (int *index, size_t n, int dir,
                            const diff_t *array);
extern void   i2x_heapsort (int *index, size_t n, int dir,
                            const diff_t *array);
extern void   i2z_qsort    (int *index, size_t n, int dir,
                            const size_t *array);
extern void   i2z_heapsort (int *index, size_t n, int dir,
                            const size_t *array);
extern void   i2f_qsort    (int *index, size_t n, int dir,
                            const float  *array);
extern void   i2f_heapsort (int *index, size_t n, int dir,
                            const float  *array);
extern void   i2d_qsort    (int *index, size_t n, int dir,
                            const double *array);
extern void   i2d_heapsort (int *index, size_t n, int dir,
                            const double *array);
extern void   i2p_qsort    (int *index, size_t n, int dir,
                            const void   **array,
                            CMPFN *cmp, void *data);
extern void   i2p_heapsort (int *index, size_t n, int dir,
                            const void   **array,
                            CMPFN *cmp, void *data);
extern void   i2c_qsort    (int *index, size_t n, int dir,
                            int_CMPFN *cmp, void *data);
extern void   i2c_heapsort (int *index, size_t n, int dir,
                            int_CMPFN *cmp, void *data);

/*----------------------------------------------------------------------
  Functions for Long Integer Index Arrays
----------------------------------------------------------------------*/
extern void   l2i_qsort    (long *index, size_t n, int dir,
                            const int    *array);
extern void   l2i_heapsort (long *index, size_t n, int dir,
                            const int    *array);
extern void   l2l_qsort    (long *index, size_t n, int dir,
                            const long   *array);
extern void   l2l_heapsort (long *index, size_t n, int dir,
                            const long   *array);
extern void   l2x_qsort    (long *index, size_t n, int dir,
                            const diff_t *array);
extern void   l2x_heapsort (long *index, size_t n, int dir,
                            const diff_t *array);
extern void   l2z_qsort    (long *index, size_t n, int dir,
                            const size_t *array);
extern void   l2z_heapsort (long *index, size_t n, int dir,
                            const size_t *array);
extern void   l2f_qsort    (long *index, size_t n, int dir,
                            const float  *array);
extern void   l2f_heapsort (long *index, size_t n, int dir,
                            const float  *array);
extern void   l2d_qsort    (long *index, size_t n, int dir,
                            const double *array);
extern void   l2d_heapsort (long *index, size_t n, int dir,
                            const double *array);
extern void   l2p_qsort    (long *index, size_t n, int dir,
                            const void   **array,
                            CMPFN *cmp, void *data);
extern void   l2p_heapsort (long *index, size_t n, int dir,
                            const void   **array,
                            CMPFN *cmp, void *data);
extern void   l2c_qsort    (long *index, size_t n, int dir,
                            lng_CMPFN *cmp, void *data);
extern void   l2c_heapsort (long *index, size_t n, int dir,
                            lng_CMPFN *cmp, void *data);

/*----------------------------------------------------------------------
  Functions for ptrdiff_t Index Arrays
----------------------------------------------------------------------*/
extern void   x2i_qsort    (diff_t *index, size_t n, int dir,
                            const int    *array);
extern void   x2i_heapsort (diff_t *index, size_t n, int dir,
                            const int    *array);
extern void   x2l_qsort    (diff_t *index, size_t n, int dir,
                            const long   *array);
extern void   x2l_heapsort (diff_t *index, size_t n, int dir,
                            const long   *array);
extern void   x2x_qsort    (diff_t *index, size_t n, int dir,
                            const diff_t *array);
extern void   x2x_heapsort (diff_t *index, size_t n, int dir,
                            const diff_t *array);
extern void   x2z_qsort    (diff_t *index, size_t n, int dir,
                            const size_t *array);
extern void   x2z_heapsort (diff_t *index, size_t n, int dir,
                            const size_t *array);
extern void   x2f_qsort    (diff_t *index, size_t n, int dir,
                            const float  *array);
extern void   x2f_heapsort (diff_t *index, size_t n, int dir,
                            const float  *array);
extern void   x2d_qsort    (diff_t *index, size_t n, int dir,
                            const double *array);
extern void   x2d_heapsort (diff_t *index, size_t n, int dir,
                            const double *array);
extern void   x2p_qsort    (diff_t *index, size_t n, int dir,
                            const void   **array,
                            CMPFN *cmp, void *data);
extern void   x2p_heapsort (diff_t *index, size_t n, int dir,
                            const void   **array,
                            CMPFN *cmp, void *data);
extern void   x2c_qsort    (diff_t *index, size_t n, int dir,
                            dif_CMPFN *cmp, void *data);
extern void   x2c_heapsort (diff_t *index, size_t n, int dir,
                            dif_CMPFN *cmp, void *data);

/*----------------------------------------------------------------------
  Preprocessor Definitions
----------------------------------------------------------------------*/
#define sht_clear(a,n)      memset(a, 0, (n)*sizeof(short))
#define int_clear(a,n)      memset(a, 0, (n)*sizeof(int))
#define lng_clear(a,n)      memset(a, 0, (n)*sizeof(long))
#define dif_clear(a,n)      memset(a, 0, (n)*sizeof(diff_t))
#define siz_clear(a,n)      memset(a, 0, (n)*sizeof(size_t))
#define flt_clear(a,n)      memset(a, 0, (n)*sizeof(float))
#define dbl_clear(a,n)      memset(a, 0, (n)*sizeof(double))
#define ptr_clear(a,n)      memset(a, 0, (n)*sizeof(void*))
#define obj_clear(a,n,z)    memset(a, 0, (n)*(z))

#define sht_copy(d,s,n)     memmove(d, s, (n)*sizeof(short))
#define int_copy(d,s,n)     memmove(d, s, (n)*sizeof(int))
#define lng_copy(d,s,n)     memmove(d, s, (n)*sizeof(long))
#define dif_copy(d,s,n)     memmove(d, s, (n)*sizeof(diff_t))
#define siz_copy(d,s,n)     memmove(d, s, (n)*sizeof(size_t))
#define flt_copy(d,s,n)     memmove(d, s, (n)*sizeof(float))
#define dbl_copy(d,s,n)     memmove(d, s, (n)*sizeof(double))
#define ptr_copy(d,s,n)     memmove(d, s, (n)*sizeof(void*))
#define obj_copy(d,s,n,z)   memmove(d, s, (n)*(z))

#endif
