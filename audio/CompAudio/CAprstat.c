/*------------- Telecommunications & Signal Processing Lab --------------
                          McGill University
Routine:
  void CAprstat (const struct Stats_F Stats[], long int Nchan)

Purpose:
  Print statistics for an audio file

Description:
  This routine prints statistics for audio file data.  The standard information
  printed includes the mean, standard deviation, maximum value and minimum
  value.  For data which is restricted to the range [-32768,+32767], and the
  minimum and maximum values take on integer values, two  additional counts
  (if nonzero) are reported.  These are the number of overloads and the number
  of anomalous transitions.

Parameters:
   -> const struct Stats_F Stats[]
      Structure containing the file statistics
   -> long int Nchan
      Number of channels
   -> double ScaleF
      Scale factor. This should be the scale factor before any other user
      supplied scale factor is applied.

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.24 $  $Date: 2003/07/11 14:37:57 $

-----------------------------------------------------------------------*/

#include <math.h>		/* sqrt, modf */

#include "CompAudio.h"

#define MAXV(a, b)	(((a) > (b)) ? (a) : (b))
#define SQRV(x)		((x) * (x))


void
CAprstat (const struct Stats_F Stats[], long int Nchan, double ScaleF)

{
  double SFI, sd, sm, ActFactor;
  int i;
  long int N;

  /* Avoid divide by zero */
  if (ScaleF != 0.0)
    SFI = 1. / ScaleF;
  else
    SFI = 0.;

  for (i = 0; i < Nchan; ++i) {

    if (Nchan > 1)
      printf (CAMF_Channel, i+1);
    N = Stats[i].N;
    if (N != 0) {
      printf (CAMF_NumSamp, N);
      if (Stats[i].Vmax == Stats[i].Vmin)
	sd = 0.0;	/* Calculation below may give sqrt (-eps) */
      else
	/* sd^2 is an unbiased estimate of the variance (assuming independent
	   samples), but sd itself is not unbiased. However, the formula is
	   the one usually used for estimating the standard deviation.
	*/
	sd = sqrt ((Stats[i].Sx2 - SQRV (Stats[i].Sx) / N) / MAXV (N-1, 1) );
      sm = Stats[i].Sx / N;

      printf (CAMF_SDevMean, SFI * sd, 100. * sd, SFI * sm, 100. * sm);
      printf (CAMF_MaxMin, SFI * Stats[i].Vmax, 100. * Stats[i].Vmax,
	                   SFI * Stats[i].Vmin, 100. * Stats[i].Vmin);

      if (Stats[i].Novload > 0) {
	if (Stats[i].Nrun == 1)
	  printf (CAMF_NumOvld1, Stats[i].Novload);
	else
	  printf (CAMF_NumOvldN, Stats[i].Novload, Stats[i].Nrun);
      }

      if (Stats[i].Nanomal > 0)
	printf (CAMF_NumAnom, Stats[i].Nanomal);

      if (Stats[i].ActLev > 0.0) {
	ActFactor = (Stats[i].Sx2 / N) / SQRV (Stats[i].ActLev);
	printf (CAMF_ActLevel, SFI * Stats[i].ActLev, 100 * Stats[i].ActLev,
		100. * ActFactor);
      }
    }
  }

  return;
}
