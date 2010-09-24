/*------------ Telecommunications & Signal Processing Lab --------------
                         McGill University

Routine:
  UTtypes.h

Description:
  Type declarations for size dependent storage elements

Author / revision:
  P. Kabal  Copyright (C) 2009
  $Revision: 1.14 $  $Date: 2009/03/11 20:15:52 $

----------------------------------------------------------------------*/

#ifndef UTtypes_h_
#define UTtypes_h_

#include <float.h>		/* DBL_MAX, FLT_MAX */
#include <limits.h>

/* To avoid name conflicts with definitions in stdint.h, we prefix our
   names with UT. Here the numerical part of the name is the length in
   bytes (e.g. UT_INT8_MAX and typedef UT_int8_t. In stdint.h, the
   numerical part of the name is the length in bits (e.g. define INT64_MAX
   and typedef int64_t).
*/

/* For floating types, we have to assume the sizes, since the preprocessor
   does not allow floating constants or sizeof checks */
#define UT_FLOAT4_MAX	FLT_MAX
typedef float UT_float4_t;		/* 4-byte float */

# define UT_FLOAT8_MAX	DBL_MAX
typedef double UT_float8_t;		/* 8-byte float */

/* We don't have a good way of automatically determining the number of
   bytes in a (long double). For gcc under Cygwin, a long double is 12
   bytes long. On other systems and/or compilers it varies from 8 to 16 bytes.
*/

/*
 Normal case for integer sizes are
 (short int, int, long int) are (2,2,4), (2,4,4) or (2,4,8) bytes
 if (long long int) is available, the standard cases are assumed to be
 (2,2,4,8) (2,4,4,8), and (2,4,8,8).
*/
#if (SHRT_MAX == 32767)
# define UT_INT2_MAX	SHRT_MAX
# define UT_INT2_MIN	SHRT_MIN
# define UT_UINT2_MAX	USHRT_MAX
  typedef unsigned short int UT_uint2_t;	/* 2-byte unsigned int */
  typedef short int UT_int2_t;		/* 2-byte int */
#endif

#if (INT_MAX > SHRT_MAX)	/* (2,4,4) or (2,4,8) */
# define UT_INT4_MAX	INT_MAX
# define UT_INT4_MIN	INT_MIN
# define UT_UINT4_MAX	UINT_MAX
  typedef unsigned int UT_uint4_t;		/* 4-byte unsigned int */
  typedef int UT_int4_t;			/* 4-byte int */
#elif (LONG_MAX > SHRT_MAX)	/* (2,2,4) */
# define UT_INT4_MAX	LONG_MAX
# define UT_INT4_MIN	LONG_MIN
# define UT_UINT4_MAX	ULONG_MAX
  typedef unsigned long int UT_uint4_t;	/* 4-byte unsigned int */
  typedef long int UT_int4_t;		/* 4-byte int */
#endif

#if (LONG_MAX > INT_MAX && INT_MAX > SHRT_MAX) /* (2,4,8) */
# define UT_INT8_MAX	LONG_MAX
# define UT_INT8_MIN	LONG_MIN
# define UT_UINT4_MAX	ULONG_MAX
  typedef unsigned long int UT_uint8_t;	/* 8-byte unsigned int */
  typedef long int UT_int8_t;		/* 8-byte int */
#elif defined LONG_LONG_MAX	/* (2,4,4,8) */
# if (LONG_LONG_MAX > LONG_MAX)
#   define UT_INT8_MAX	LONG_LONG_MAX
#   define UT_INT8_MIN	LONG_LONG_MIN
#   define UT_UINT8_MAX	ULONG_LONG_MAX
    typedef unsigned long long int UT_uint8_t;
    typedef long long int UT_int8_t;	/* 8-byte int */
# endif
#endif

#define UT_UINT1_MAX	UCHAR_MAX
#define UT_INT1_MAX	SCHAR_MAX
#define UT_INT1_MIN	SCHAR_MIN
typedef unsigned char UT_uint1_t;	/* 1-byte unsigned int */
typedef signed char UT_int1_t;		/* 1-byte signed int */

#endif	/* UTtypes_h_ */
