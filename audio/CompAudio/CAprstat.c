/*------------- Telecommunications & Signal Processing Lab --------------
                          McGill University
Routine:
  void CAprstat (const struct Stats_F Stats[], int Nchan)

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
   -> int Nchan
      Number of channels

Author / revision:
  P. Kabal  Copyright (C) 2002
  $Revision: 1.21 $  $Date: 2002/09/03 12:22:47 $

-----------------------------------------------------------------------*/

static char rcsid[] = "$Id: CAprstat.c 1.21 2002/09/03 AFsp-v6r8 $";

#include <math.h>		/* sqrt, modf */

#include "CompAudio.h"

#define MAXV(a, b)	(((a) > (b)) ? (a) : (b))

static const float Amax = 32767.;
static const float Amin = -32768.;


void
CAprstat (const struct Stats_F Stats[], int Nchan)

{
  double sd, sm, dint, ActFactor;
  int i;
  long int N;

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
	sd = sqrt ((Stats[i].Sx2 - (Stats[i].Sx * Stats[i].Sx) / N) /
		   MAXV (N-1, 1) );
      sm = Stats[i].Sx / N;
      printf (CAMF_SDevMean, sd, sm);
      printf (CAMF_MaxMin, Stats[i].Vmax, Stats[i].Vmin);

      /* For integer data */
      if (Stats[i].Vmax <= Amax && Stats[i].Vmin >= Amin &&
	  modf (Stats[i].Vmax, &dint) == 0.0 &&
	  modf (Stats[i].Vmin, &dint) == 0.0) {
	if (Stats[i].Novload > 0) {
	  if (Stats[i].Nrun == 1)
	    printf (CAMF_NumOvld1, Stats[i].Novload);
	  else
	    printf (CAMF_NumOvldN, Stats[i].Novload, Stats[i].Nrun);
	}
	if (Stats[i].Nanomal > 0)
	  printf (CAMF_NumAnom, Stats[i].Nanomal);
	if (Stats[i].ActLev > 0.0) {
	  ActFactor = (Stats[i].Sx2 / N) / (Stats[i].ActLev * Stats[i].ActLev);
	  printf(CAMF_ActLevel, Stats[i].ActLev, 100.0 * ActFactor);
	}
      }
    }
    else {
      printf (CAMF_SDevMeanX);
      printf (CAMF_MaxMinX);
    }
  }

  return;
}
