/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  double RSKalphaXatt (double alpha)
  double RSKalphaXD (double alpha)
  double RSKattenXalpha (double atten)

Purpose:
  Calculate specification values for a Kaiser windowed lowpass filter design

Description:
  This module contains routines to calculate and convert specification values
  for a Kaiser windowed lowpass filter design.  The window shape parameter
  alpha, the stopband attenuation, and the normalized transition width are
  related by empirical formulas.

  References:
  J. F. Kaiser, "Nonrecursive digital filter design using the I0-sinh window
  function", Proc. 1974 IEEE Int. Symp. on Circuits and Syst., pp. 20-23,
  April 1974.

  A. Antoniou, Digital Filters: Analysis, Design and Applications, 2nd ed.,
  McGraw-Hill, 1993.

Parameters:
  <-  double RSKalphaXatten
      Attenuation value in dB corresponding to a given alpha value
   -> double alpha
      Alpha value for the Kaiser window

  <-  double RSKalphaXD
      Normalized transition width corresponding to a given alpha value
   -> double alpha
      Alpha value for the Kaiser window

  <-  double RSKattenXalpha
      Alpha value for a Kaiser window
   -> double atten
      Stopband attenuation in dB (min 21)


Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.6 $  $Date: 2003/05/13 01:09:07 $

-------------------------------------------------------------------------*/

#include <math.h>

#include "ResampAudio.h"

#define K1	0.5842
#define K2	0.07886
#define K3	0.1102
#define EPS	1E-6

/* Empirical formulas for alpha, attenuation (dB), and window width */

/* Convert attenuation in dB to alpha */


double
RSKattenXalpha (double atten)

{
  double alpha;

  if (atten <= 21.0)
    alpha = 0.0;
  else if (atten <= 50.0)
    alpha = K1 * pow (atten-21.0, 0.4) + K2 * (atten-21.0);
  else
    alpha = K3 * (atten - 8.7);

  return alpha;
}

/* Convert alpha to attenuation in dB */

double
RSKalphaXatt (double alpha)

{
  double attenL, attenU, atten;
  int i;

  alpha = fabs (alpha);

  if (alpha < RSKattenXalpha (50.0)) {
    /* Binary search for the attenuation value */
    attenL = 21.0;
    attenU = 50.0;
    for (i = 0; i <15; ++i) {
      atten = 0.5 * (attenL + attenU);
      if (alpha > RSKattenXalpha (atten))
	attenL = atten;
      else
	attenU = atten;
    }
    atten = 0.5 * (attenL + attenU);
  }

  else
    atten = alpha / K3 + 8.7;

  return atten;
}

/* Convert alpha to normalized transition width D */

double
RSKalphaXD (double alpha)

{
  double atten, D;

  atten = RSKalphaXatt (alpha);

  if (atten <= 21.0)
    D = 0.9222;
  else
    D = (atten - 7.95) / 14.36;

  return D;
}
