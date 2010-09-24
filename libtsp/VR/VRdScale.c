/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void VRdScale (double a, const double x[], double y[], int N)

Purpose:
  Multiply each element of a double array by a scalar value

Description:
  This routine multiplies each element of a double array by a scalar value,
    y[i] = a * x[i],  0 <= i < N.

Parameters:
   -> double a
      A scalar value
   -> const double x[]
      Input array (N elements)
  <-  double y[]
      Output array (N elements).  The output array can be the same as the
      input array.
   -> int N
      Number of elements in the arrays (may be zero).

Author / revision:
  P. Kabal  Copyright (C) 2009
  $Revision: 1.4 $  $Date: 2009/03/23 11:59:42 $

-------------------------------------------------------------------------*/

#include <libtsp.h>


void
VRdScale (double a, const double x[], double y[], int N)

{
  int i;

  for (i = 0; i < N; ++i)
    y[i] = a * x[i];

  return;
}
