/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  double VRfDotProd (const float x1[], const float x2[], int N)

Purpose:
  Dot product of two float arrays (double result)

Description:
  This routine calculates the dot product of two float arrays x1 and x2.  The
  dot product is calculated internally as a double and returned as a double
  value.
                 N-1
    VRfDotProd = SUM x1[i] * x2[i] .
                 i=0

Parameters:
  <-  double VRfDotProd
      Dot product
   -> const float x1[]
      Input array (N elements)
   -> const float x2[]
      Input array (N elements)
   -> int N
      Number of elements in the arrays (may be zero)

Author / revision:
  P. Kabal  Copyright (C) 1999
  $Revision: 1.4 $  $Date: 1999/06/04 22:51:11 $

-------------------------------------------------------------------------*/

static char rcsid[] = "$Id: VRfDotProd.c 1.4 1999/06/04 AFsp-v6r8 $";

#include <libtsp.h>


double
VRfDotProd (const float x1[], const float x2[], int N)

{
  int i;
  double sum;

  sum = 0.0;
  for (i = 0; i < N; ++i)
    sum += (double) x1[i] * x2[i];

  return sum;
}
