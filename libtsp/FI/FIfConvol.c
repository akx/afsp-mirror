/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void FIfConvol (const float x[], float y[], int Nout, const float h[],
		  int Ncof)

Purpose:
  Filter a signal with an FIR filter

Description:
  The procedure convolves a set of filter coefficients with an array of data.
  The input array is x[.].  The first lmem = Ncof-1 samples of x[.] are past
  inputs.  The first output point is calculated with filter coefficient h[0]
  aligned with x[lmem].  The last output point is calculated with h[0] aligned
  with x[lmem+(Nout-1)].

  The output array can share storage with the input array, with the output data
  overlaying the input data.  Consider an array x[] with lmem+Nout elements.
  The first lmem elements are past input samples; the next Nout values are new
  input samples.  To share input/output storage, invoke this routine as
    FIconvol (x, x, Nout, h, Ncof)
  On return, the first Nout elements of x[] are the output values.  The last
  lmem elements are the past input values needed for the next invocation of
  FIconvol.

Parameters:
   -> const float x[]
      Input array of data.  Let lmem=Ncof-1.  The first output point point is
      calculated as follows
        y[0] = h[0]*x[lmem] + h[1]*x[lmem-1] + h[2]*x[lmem-2] + ...
      The array x must have lmem+((Nout-1))+1 elements.
  <-  float y[]
      Output array of samples.  The order of operations is such that this array
      can share storage with the array x.
   -> int Nout
      Number of output samples to be calculated
   -> const float h[]
      Array of Ncof filter coefficients
   -> int Ncof
      Number of filter coefficients

Author / revision:
  P. Kabal  Copyright (C) 2002
  $Revision: 1.12 $  $Date: 2002/03/25 16:09:56 $

-------------------------------------------------------------------------*/

static char rcsid[] = "$Id: FIfConvol.c 1.12 2002/03/25 AFsp-v6r8 $";

#include <libtsp.h>

void
FIfConvol (const float x[], float y[], int Nout, const float h[], int Ncof)

{
  int m;
  int j;
  double sum;
  const float *xp;

  /* Loop over output points */
  xp = &x[Ncof-1];
  for (m = 0; m < Nout; ++m) {

    /* Convolution */
    sum = 0.0;
    for (j = 0; j < Ncof; ++j) {
      sum += h[j] * xp[m-j];
    }
    y[m] = (float) sum;
  }
}
