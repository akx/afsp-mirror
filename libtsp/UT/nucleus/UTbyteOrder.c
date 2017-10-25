/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  enum UT_DS_T UTbyteOrder (void)

Purpose:
  Determine the byte order for data storage

Description:
  This function returns a code indicating whether the storage order the current
  host is little-endian or big-endian.  For big-endian machines the constituent
  bytes in a multi-byte entity are stored starting with the most significant
  byte, ending with the least significant byte.  For little-endian machines,
  bytes are stored from least significant to most significant.

Parameters:
  <-  enum UT_DS_T UTbyteOrder
      Returned integer code
        0 - Big-endian storage order
        1 - Little-endian storage order

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.28 $  $Date: 2017/04/24 03:07:11 $

-------------------------------------------------------------------------*/

#include <assert.h>

#include <libtsp/nucleus.h>
#include <libtsp/UTpar.h>
#include <libtsp/UTtypes.h>

#define U4    ((int) sizeof (UT_uint4_t))

static union { UT_uint4_t U; unsigned char C[U4]; } u;
static const unsigned char C[4] = { 1, 2, 4, 8 };
static const UT_uint4_t I4L = 0x08040201;
static const UT_uint4_t I4B = 0x01020408;


enum UT_DS_T
UTbyteOrder (void)

{
  static enum UT_DS_T Hbo = DS_UNDEF;

  if (Hbo == DS_UNDEF) {  /* Cache the byte order */
    u.C[0] = C[0];
    u.C[1] = C[1];
    u.C[2] = C[2];
    u.C[U4-1] = C[3]; /* Implicit check that sizeof (UT_uint4_t) is 4 */
    if (u.U == I4L)
      Hbo = DS_EL;
    else if (u.U == I4B)
      Hbo = DS_EB;
    else
      assert (0); /* Funny byte order */
  }

  return Hbo;
}
