/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void VRdZero (double y[], int N)

Purpose:
  Zero an array of doubles

Description:
  This routine zeros N elements of a given double array,
    y[i] = 0.0,  0 <= i < N.

Parameters:
  <-  void VRdZero
  <-  double y[]
      Array of doubles (N elements)
   -> int N
      Number of elements

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.8 $  $Date: 2017/06/12 18:58:08 $

-------------------------------------------------------------------------*/

#include <libtsp.h>


void
VRdZero (double y[], int N)

{
  int i;

  for (i = 0; i < N; ++i)
    y[i] = 0.0;

  return;
}
