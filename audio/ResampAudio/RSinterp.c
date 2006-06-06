/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void RSinterp (const double x[], int Nxm, double y[], int Ny, double Ds,
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
   -> const double x[]
      Input data array of size Nx+lmem
   -> int Nxm
      Number of data samples
  <-  double y[]
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
  P. Kabal  Copyright (C) 2005
  $Revision: 1.12 $  $Date: 2005/02/01 04:18:47 $

-------------------------------------------------------------------------*/

#include <assert.h>

#include <libtsp.h>
#include "ResampAudio.h"

#define EPS	1E-10

static double
RS_conv (const double *xp, const double h[], int Nc);


void
RSinterp (const double x[], int Nxm, double y[], int Ny, double Ds,
	  const struct Tval_T *T, const struct Fpoly_T *PF)

{
  double yl, yh;
  int lmem, i, n, mr;
  int Ir;
  const double **hs;
  const int *offs, *Nc;
  double tir, p;
  struct Tval_T To;

  hs = (const double **) PF->hs;
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
      y[i] = (1.0 - p) * yl + p * yh;
    }

    /* Update the sample pointer (n, ds, dsr) - dsr remains unchanged */
    RSincTime (&To, Ds);
  }

  return;
}

/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  static double RS_conv (const double *xp, const double h[], int Nc)

Purpose:
  Generate one filtered value

Description:
  The procedure convolves a set of filter coefficients with an array of data.
  The input array is x[.].  The first N-1 samples of x[.] are past inputs.
        N-1
    y = SUM h[j] x[N-1-j]
        j=0

Parameters:
  <-  static double RS_conv
      Output value
   -> const double *xp
      Pointer to the last element to be processed, x[Nc-1]
   -> const double h[]
      Array of Nc filter coefficients
   -> int Nc
      Number of filter coefficients

Author / revision:
  P. Kabal  Copyright (C) 2005
  $Revision: 1.12 $  $Date: 2005/02/01 04:18:47 $

-------------------------------------------------------------------------*/

static double
RS_conv (const double *xp, const double h[], int Nc)

{
  int l, j;
  double y;
  const double *x;

/*
  There are many different ways to write this tight loop, including using
  indices (as here) or using pointers with decrement and increment.  Tests
  have shown that for all compilers tested, these different idioms result in
  equally fast execution.
*/

  /* Convolution with filter */
  x = xp - (Nc-1);
  y = 0.0;
  for (j = 0, l = Nc-1; j < Nc; ++j, --l)
    y += h[j] * x[l];

  return y;
}
