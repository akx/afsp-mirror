/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void VRfMult (const float x1[], const float x2[], float y[], int N)

Purpose:
  Element-by-element product of two float arrays

Description:
  This routine calculates the product of corresponding elements of two float
  arrays,
    y[i] = x1[i] * x2[i],  0 <= i < N.

Parameters:
   -> const float x1[]
      Input array (N elements)
   -> const float x2[]
      Input array (N elements)
  <-  float y[]
      Output array (N elements).  The output array can be the same as either
      of the input arrays.
   -> int N
      Number of elements in the arrays (may be zero)

Author / revision:
  P. Kabal  Copyright (C) 1999
  $Revision: 1.5 $  $Date: 1999/06/04 22:51:11 $

-------------------------------------------------------------------------*/

static char rcsid[] = "$Id: VRfMult.c 1.5 1999/06/04 AFsp-v6r8 $";

#include <libtsp.h>

void
VRfMult (const float x1[], const float x2[], float y[], int N)

{
  int i;

  for (i = 0; i < N; ++i)
    y[i] = x1[i] * x2[i];

  return;
}
