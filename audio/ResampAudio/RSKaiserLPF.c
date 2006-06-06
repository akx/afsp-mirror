/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void RSKaiserLPF (double h[], int N, double Fc, double alpha, double Gain,
                    double Woffs, double Wspan)

Purpose:
  Generate a Kaiser windowed lowpass filter

Description:
  This routine calculates the impulse response of a symmetric lowpass filter
  which is obtained by applying a Kaiser window to a sin(x)/x function.
  Consider the continuous-time version of this response
                           sin(2 pi Fc t)
    h(t) = G * Kw(t/T, a)  -------------- ,
                                pi t
  where Kw(t/T,a) is a Kaiser window with parameter a, having a non-zero
  portion spanning -T <= t <= T.  The cutoff of the lowpass filter is Fc.  The
  gain of the filter is set to approximate G (Gain) in the passband.  The
  impulse response samples returned are

    h[n] = h(t-T+Woffs),  for 0 <= n <= N-1.

  References:
  J. F. Kaiser, "Nonrecursive digital filter design using the I0-sinh window
  function", Proc. 1974 IEEE Int. Symp. on Circuits and Syst., pp. 20-23,
  April 1974.

  A. Antoniou, Digital Filters: Analysis, Design and Applications, 2nd ed.,
  McGraw-Hill, 1993.

Parameters:
  <-  double h[]
      Array containing the impulse response samples.  The reference point in
      the array is at offset (N-1)/2 - Woffs.
   -> int N
      Number of impulse response samples
   -> double Fc
      Lowpass filter cutoff (normalized frequency, 0 to 1/2)
   -> double alpha
      Kaiser window parameter
   -> double Gain
      Low frequency gain
   -> double Woffs
      Offset of the first sample from the beginning of the non-zero part of
      the window (samples)
   -> double Wspan
      Span of the non-zero part of the Kaiser window in samples (2T in the
      formulation above, equal to N-1 to get the full window)

Author / revision:
  P. Kabal  Copyright (C) 2005
  $Revision: 1.12 $  $Date: 2005/02/01 04:20:01 $

-------------------------------------------------------------------------*/

#include <libtsp.h>
#include <libtsp/nucleus.h>
#include "ResampAudio.h"


void
RSKaiserLPF (double h[], int N, double Fc, double alpha, double Gain,
	     double Woffs, double Wspan)

{
  int i, k;
  double t, u, T, Ga;

  /* Multiply the Kaiser window by the sin(x)/x response */
  /* Use symmetry to reduce computations */
  Ga = 2.0 * Fc * Gain;
  T = 0.5 * Wspan;
  for (i = 0, k = N-1; i <= k; ++i, --k) {
    t = i - T + Woffs;
    u = k - T + Woffs;
    h[i] = Ga * FNsinc (2.0 * Fc * t) * FIxKaiser (t / T, alpha);
    if (-t == u)
      h[k] = h[i];
    else
      h[k] = Ga * FNsinc (2.0 * Fc * u) * FIxKaiser (u / T, alpha);
  }

  return;
}
