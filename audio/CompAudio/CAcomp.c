/*------------- Telecommunications & Signal Processing Lab --------------
                          McGill University
Routine:
  struct Stats_X CAcomp(AFILE *AFp[2], const long int Start[2], long int Nsamp,
                        long int Nsseg, const long int Delay[2])

Purpose:
  Gather correlation statistics for two audio files over a range of delays

Description:
  This routine gathers statistics for an audio files over a range of delays. The
  delay values runs from delayL to delayU. For each delay the gain optimized SNR
  is calculated. This value is printed for each delay value if delayL <= delayU.
  The cross-file statistics for the delay which maximizes the SNR are returned.

Parameters:
  <- struct Stats_X CAcomp
      Structure containing the cross-file statistics corresponding to the delay
      value which gives the best segmental SNR.
   -> AFILE *AFp[2]
      Audio file pointers
   -> const long int Start[2]
      Start samples
   -> long int Nsamp
      Number of samples
   -> long int Nsseg
      Segment length in samples for segmental SNR computations. If Nsseg is
      zero, a default value is used.
   -> const long int Delay[2]
      Start/end delay value. Normally Delay[0] <= Delay[1]. If this condition is
      not satisfied, no statistics are calculated.

Author / revision:
  P. Kabal  Copyright (C) 2020
  $Revision: 1.32 $  $Date: 2020/11/27 02:06:32 $

-----------------------------------------------------------------------*/

#include <float.h>  /* DBL_MAX */
#include <math.h> /* log10 */

#include "CompAudio.h"

#define ABSV(x)   (((x) < 0) ? -(x) : (x))
#define MINV(a, b)  (((a) < (b)) ? (a) : (b))
#define MAXV(a, b)  (((a) > (b)) ? (a) : (b))
#define DB(x)   (10.0 * log10(x))

static double
CA_Nsseg(double SfreqA, double SfreqB);


struct Stats_X
CAcomp(AFILE *AFp[2], const long int Start[2], long int Nsamp,
       const long int Delay[2])

{
  struct Stats_X StatsXD, StatsX;
  struct SNR_X SNRX;
  long int Nsseg, delay;
  double SNRGmax;

/* Check Sfreq, find the segment length for segmental SNR */
  Nsseg = AFp[0]->Nchan * CA_Nsseg(AFp[0]->Sfreq, AFp[1]->Sfreq);

/* The cross-channel statistics are based on treating 2-channel data as complex
   data. For the conventional SNR values, this is equivalent to treating the
   data as one channel data.

   For the optimized gain version of SNR, there are several choices for the gain
   factor:
   - gain factor is a 2 x 2 matrix allowing mixing and scaling
   - gain factor is a scalar for each channel
   - gain factor is a scalar, the same value being applied to each channel
   For the last case, the 2-channel computations are exactly the same as if the
   data are treated as one channel data. This is the case that is implemented.
*/

/* Loop over the delays */
  SNRGmax = -DBL_MAX;
  for (delay = Delay[0]; delay <= Delay[1]; ++delay) {

    /* Cross product terms */
    StatsXD = CAcorr(AFp, Start, Nsamp, delay, Nsseg);

    if (Delay[0] != Delay[1]) {

      /* Calculate the SNR values */
      SNRX = CASNR(&StatsXD);        /* Use SSNR to find best delay */

      /* Print the SNR for different delays */
      if (SNRX.SNRG != DBL_MAX)
        printf(CAMF_DelaySNR, delay, DB(SNRX.SNRG), SNRX.SF);
      else
        printf(CAMF_DelayAxB, delay, SNRX.SF);

      /* Save the best match statistics - if the best match occurs at several
         delays, use the delay with the smallest absolute value */
      if (SNRX.SNRG > SNRGmax ||
          (SNRX.SNRG == SNRGmax && ABSV(delay) <= ABSV(StatsX.Delay))) {
        SNRGmax = SNRX.SNRG;
        StatsX = StatsXD;
      }
    }
    else {
      StatsX = StatsXD;
    }
  }
  return StatsX;
}

/* Resolve the sampling frequency, choose the segment length in frames */


static double
CA_Nsseg(double SfreqA, double SfreqB)

{
  double Sfreq;
  long int Nsseg;

  if (SfreqA != SfreqB) {
    UTwarn("%s - %s", PROGRAM, CAM_DiffSFreq);
    Sfreq = 0.5 * (SfreqA + SfreqB);
  }
  else
    Sfreq = SfreqA;

  /* Choose a block size, 16 ms by default */
  Nsseg = (long int) MSdNint(SN_SEGTIME * Sfreq);
  if (Nsseg < SN_NSEG_MIN || Nsseg > SN_NSEG_MAX) {
    Nsseg = MINV(SN_NSEG_MAX, MAXV(SN_NSEG_MIN, Nsseg));
  }

  return Nsseg;
}
