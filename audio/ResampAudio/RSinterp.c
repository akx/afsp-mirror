/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void RSinterp (const float x[], int Nxm, float y[], int Ny, double Ds,
                 const struct Tval_T *T, const struct Fpoly_T *PF)

Purpose:
  Interpolate data values in an array using an interpolation filter

Description:
  This procedure generates output points at equally spaced (fractional) sample
  points.  The input array contains Nxm samples, including lmem filter memory
  samples, where lmem = PF->Ncmax-1.  The output points correspond to times
    t(i) = lmem + (dsr + ds + Ds*i) / Ns
  This procedure generates interpolated values which bracket the desired
  time point.  The bracketing points are separated by 1/PF->Ir.  Linear
  interpolation is used between the bracketing points to generate the output
  value corresponding to one time value.

Parameters:
   -> const float x[]
      Input data array of size Nx+lmem
   -> int Nxm
      Number of data samples
  <-  float y[]
      Output data array of size Ny
   -> int Ny
      Number of output values to be generated
   -> double Ds
      Normalizing value for the time increment specification
   -> const struct Tval_T *T
      Time value structure
   -> const struct Fpoly *PF
      Structure with the filter coefficients in a polyphase form

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.11 $  $Date: 2003/05/13 01:08:13 $

-------------------------------------------------------------------------*/

#include <assert.h>

#include <libtsp.h>
#include "ResampAudio.h"

#define EPS	1E-10

static float
RS_conv (const float *xp, const float h[], int Nc);


void
RSinterp (const float x[], int Nxm, float y[], int Ny, double Ds,
	  const struct Tval_T *T, const struct Fpoly_T *PF)

{
  float yl, yh;
  int lmem, i, n, mr;
  int Ir;
  const float **hs;
  const int *offs, *Nc;
  double tir, p;
  struct Tval_T To;

  hs = (const float **) PF->hs;
  offs = PF->offs;
  Nc = PF->Nc;
  Ir = PF->Ir;
  lmem = PF->Ncmax - 1;

  To = *T;
  To.n = lmem;

  for (i = 0; i < Ny; ++i) {

    tir = (To.dm + To.dmr) / To.M;	/* Fractional sample */

    /* Express tir = (mr + p) / Ir, where mr is an integer, 0 <= mr < Ir */
    mr = (int) (Ir * tir);
    p =  Ir * tir - mr;

    n = To.n;
    assert (n < Nxm);		/* Invalid time position */

/*
  The polyphase filter has an extra subfilter, subfilter Ir.  If the lower
  bracket to a time position uses subfilter Ir-1, then normally we would have
  to use subfilter 0 for the upper bracket, and appropriately shift the filter
  position.  Here we avoid this arithmetic by creating an extra subfilter,
  which is the same as subfilter 0.  The offset for this extra filter is that
  for subfilter 0 less one.
*/
    /* Sample point at the lower end of the bracketed range */
    if (p <= EPS)
      y[i] = RS_conv (&x[n-offs[mr]], hs[mr], Nc[mr]);

    /* Sample point at the upper end of the bracketed range */
    else if (p >= 1.0-EPS)
      y[i] = RS_conv (&x[n-offs[mr+1]], hs[mr+1], Nc[mr+1]);

    /* Sample point in the middle of the bracketed range */
    else {
      yl = RS_conv (&x[n-offs[mr]], hs[mr], Nc[mr]);
      yh = RS_conv (&x[n-offs[mr+1]], hs[mr+1], Nc[mr+1]);
      y[i] = (float) ((1.0 - p) * yl + p * yh);
    }

    /* Update the sample pointer (n, ds, dsr) - dsr remains unchanged */
    RSincTime (&To, Ds);
  }

  return;
}

/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  static float RS_conv (const float *xp, const float h[], int Nc)

Purpose:
  Generate one filtered value

Description:
  The procedure convolves a set of filter coefficients with an array of data.
  The input array is x[.].  The first N-1 samples of x[.] are past inputs.
        N-1
    y = SUM h[j] x[N-1-j]
        j=0

Parameters:
  <-  static float RS_conv
      Output value
   -> const float *xp
      Pointer to the last element to be processed, x[Nc-1]
   -> float h[]
      Array of Nc filter coefficients
   -> int Nc
      Number of filter coefficients

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.11 $  $Date: 2003/05/13 01:08:13 $

-------------------------------------------------------------------------*/

static float
RS_conv (const float *xp, const float h[], int Nc)

{
  int l, j;
  float y;
  const float *x;

/*
  There are many different ways to write this tight loop, including using
  indices (as here) or using pointers with decrement and increment.  Tests
  have shown that for all compilers tested, these different idioms result in
  equally fast execution.

  For the filtering operation, single precision arrays and a single precision
  accumulator are used.  For ANSI compilers this results in fast code.
  However, non-ANSI compilers will promote the multiplicands to double for the
  multiplication and then have to discard that precision for the accumulator.
  For those older compilers, a double accumulator results in faster code.
*/

  /* Convolution with filter */
  x = xp - (Nc-1);
  y = 0.0;
  for (j = 0, l = Nc-1; j < Nc; ++j, --l)
    y += h[j] * x[l];

  return y;
}
