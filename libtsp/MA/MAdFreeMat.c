/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void MAdFreeMat (double *A[])

Purpose:
  Free an allocated double matrix

Description:
  This routine frees the space occupied by a double matrix.  This routine frees
  up space allocated by routine MAdAllocMat.

Parameters:
  <-  void MAdFreeMat
   -> double *A[]
      Pointer to an array of row pointers.  If A is NULL, no action is taken.

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.10 $  $Date: 2017/06/12 19:35:29 $

-------------------------------------------------------------------------*/

#include <libtsp.h>


void
MAdFreeMat (double *A[])

{
  if (A != NULL) {
    UTfree ((void *) A[0]);
    UTfree ((void *) A);
  }

  return;
}
