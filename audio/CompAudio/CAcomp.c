/*------------- Telecommunications & Signal Processing Lab --------------
                          McGill University
Routine:
  void CAcomp (AFILE *AFp[2], long int Start[2], long int Nsamp,
               long int Nsseg, long int delayL, long int delayU,
	       long int *delayM, struct Stats_T *Stats)

Purpose:
  Gather correlation statistics for two audio files over a range of delays

Description:
  This routine gathers statistics for an audio files over a range of delays.
  The delay values runs from delayL to delayU.  For each delay the gain
  optimized SNR is calculated.  This value is printed for each delay value if
  delayL < delayU.  The cross-file statistics for the delay which maximizes the
  SNR are returned.

Parameters:
   -> AFILE *AFp[2]
      Audio file pointers
   -> long int Start[2]
      Start samples
   -> long int Nsamp
      Number of samples
   -> long int Nsseg
      Segment length in samples for segmental SNR computations. If Nsseg is
      zero, a default value is used.
   -> long int delayL
      Start delay value.  Normally delayL <= delayU.  If this condition is not
      satisfied, no statistics are calculated.
   -> long int delayU
      End delay value
  <-  long int *delayM
      Delay of file B relative to file A which maximizes the gain adjusted SNR.
  <-  struct Stats_T *Stats
      Structure containing the cross-file statistics corresponding to the delay
      value delayM

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.23 $  $Date: 2003/07/11 14:35:36 $

-----------------------------------------------------------------------*/

#include <float.h>	/* DBL_MAX */
#include <math.h>	/* log10 */

#include "CompAudio.h"

#define ABSV(x)		(((x) < 0) ? -(x) : (x))
#define DB(x)		(10.0 * log10 (x))

static double
CA_sfreq (double SfreqA, double SfreqB);
static long int
CA_nsseg (long int Nsseg, double Sfreq);


void
CAcomp (AFILE *AFp[2], long int Start[2], long int Nsamp, long int Nsseg,
	long int delayL, long int delayU, long int *delayM,
	struct Stats_T *Stats)

{
  struct Stats_T Statst;
  long int delay;
  double Sfreq, SNR, SNRG, SNRGmax, SF, SSNR;

/* Find the segment length */
  Sfreq = CA_sfreq ((double) AFp[0]->Sfreq, (double) AFp[1]->Sfreq);
  Nsseg = AFp[0]->Nchan * CA_nsseg (Nsseg, Sfreq);

/* The cross-channel statistics are based on treating 2-channel data as if
   it were complex data.  For the conventional SNR values, this is equivalent
   to treating the data as one channel data.

   For the optimized gain version of SNR, there are several choices for the
   gain factor:
   - gain factor is a 2 x 2 matrix allowing mixing and scaling
   - gain factor is a scalar for each channel
   - gain factor is a scalar, the same value being applied to each channel
   For the last case, the 2-channel computations are exactly the same as if
   the data were treated as one channel data.  This is the case that is
   implemented.
*/

/* Loop over the delays */
  SNRGmax = -DBL_MAX;
  SNRG = SNRGmax;
  for (delay = delayL; delay <= delayU; ++delay) {

    /* Cross product terms */
    CAcorr (AFp, Start, Nsamp, delay, Nsseg, &Statst);

    if (delayL != delayU) {

      /* Calculate the SNR values */
      CASNR (&Statst, &SNR, &SNRG, &SF, &SSNR);

      /* Print the match statistics */
      if (SNRG != DBL_MAX)
	printf (CAMF_DelaySNR, delay, DB (SNRG), SF);
      else
	printf (CAMF_DelayAxB, delay, SF);

      /* Save the best match statistics - if the best match occurs at several
	 delays, use the delay with the smallest absolute value */
      if (SNRG > SNRGmax ||
	  (SNRG == SNRGmax && ABSV (delay) <= ABSV (*delayM))) {
	SNRGmax = SNRG;
	*Stats = Statst;
	*delayM = delay;
      }
    }
    else {
      *Stats = Statst;
      *delayM = delay;
    }
  }

  return;
}

/* Resolve the sampling frequency */


static double
CA_sfreq (double SfreqA, double SfreqB)

{
  double Sfreq;

  if (SfreqA != SfreqB) {
    UTwarn ("%s - %s", PROGRAM, CAM_DiffSFreq);
    Sfreq = 0.5 * (SfreqA + SfreqB);
  }
  else
    Sfreq = SfreqA;

  return Sfreq;
}

/* Choose a block size */

#define DSEGTIME	16E-3	/* Segment size in seconds */
#define NSSEG_MAX	1024
#define NSSEG_MIN	64
#define NSSEG_MID	256


static long int
CA_nsseg (long int Nsseg, double Sfreq)

{
  /* Choose a block size, 16 ms by default */
  if (Nsseg == 0) {
    Nsseg = (long int) MSdNint (DSEGTIME * Sfreq);
    if (Nsseg < NSSEG_MIN || Nsseg > NSSEG_MAX) {
      Nsseg = NSSEG_MID;
      UTwarn (CAMF_SegSize, PROGRAM, Nsseg);
    }
  }

  return Nsseg;
}
