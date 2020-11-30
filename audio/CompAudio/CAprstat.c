/*------------- Telecommunications & Signal Processing Lab --------------
                          McGill University
Routine:
  void CAprstat(const struct Stats_F StatsF[], long int Nch, double ScaleF)

Purpose:
  Print statistics for an audio file

Description:
  This routine prints statistics for audio file data. The standard information
  printed includes the mean, standard deviation, maximum value and minimum
  value. Additional counts (if nonzero) are reported. These are the number of
  overloads and the number of anomalous transitions.

Parameters:
   -> const struct Stats_F StatsF[]
      Structure containing the file statistics
   -> long int Nch
      Number of effective channels
   -> double ScaleF
      Scale factor. This is the scale factor before a user supplied gain is
      applied.

Author / revision:
  P. Kabal  Copyright (C) 2020
  $Revision: 1.31 $  $Date: 2020/11/27 02:07:35 $

-----------------------------------------------------------------------*/

#include <math.h>   /* sqrt, modf */

#include "CompAudio.h"

#define MAXV(a, b)  (((a) > (b)) ? (a) : (b))
#define SQRV(x)   ((x) * (x))


void
CAprstat(const struct Stats_F StatsF[], long int Nch, double ScaleF)

{
  double SFI, sd, sd2, sm, ActFactor;
  int i, PrV;
  long int N;

  SFI = 1.0 / ScaleF;
  PrV = (SFI >= 128 && SFI <= 32768);

  for (i = 0; i < Nch; ++i) {

    if (Nch > 1)
      printf(CAMF_Channel, i+1);
    N = StatsF[i].N;
    if (N != 0) {
      printf(CAMF_NumSamp, N);
      if (StatsF[i].Vmax == StatsF[i].Vmin)
        sd = 0.0; /* Calculation below may give sqrt(-eps) */
      else {
        sd2 = (StatsF[i].Sxx - SQRV(StatsF[i].Sx) / N) / MAXV(N-1, 1);
        sd = sqrt(sd2);
      }
  /* sd^2 is an unbiased estimate of the variance (assuming independent samples,
     but sd itself is not unbiased. However, the formula is the one usually used
     for estimating the standard deviation.
  */
      sm = StatsF[i].Sx / N;
      if (PrV)
        printf(CAMF_SDevMeanV, SFI * sd, 100. * sd, SFI * sm, 100. * sm);
      else
        printf(CAMF_SDevMeanP, 100 * sd, 100 * sm);
      if (PrV)
        printf(CAMF_MaxMinV, SFI * StatsF[i].Vmax, 100. * StatsF[i].Vmax,
               SFI * StatsF[i].Vmin, 100. * StatsF[i].Vmin);
      else
        printf(CAMF_MaxMinP, 100 * StatsF[i].Vmax, 100 * StatsF[i].Vmin);

      if (StatsF[i].Novload > 0) {
        if (StatsF[i].Nrun == 0 || StatsF[i].Nrun == StatsF[i].Novload)
          printf(CAMF_NumOvld, StatsF[i].Novload);
        else if (StatsF[i].Nrun == 1)               /* Overloads in 1 run */
          printf(CAMF_NumOvld1, StatsF[i].Novload);
        else
          printf(CAMF_NumOvldN, StatsF[i].Novload, StatsF[i].Nrun);
      }

      if (StatsF[i].Nanomal > 0)
        printf(CAMF_NumAnom, StatsF[i].Nanomal);

      if (StatsF[i].ActLev > 0.0) {
        ActFactor = (StatsF[i].Sxx / N) / SQRV(StatsF[i].ActLev);
        if (PrV)
          printf(CAMF_ActLevelV, SFI * StatsF[i].ActLev, 100 * StatsF[i].ActLev,
                 100. * ActFactor);
        else
          printf(CAMF_ActLevelP, 100 * StatsF[i].ActLev, 100. * ActFactor);
      }
    }
  }
}
