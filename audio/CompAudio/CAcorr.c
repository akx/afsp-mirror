/*------------- Telecommunications & Signal Processing Lab --------------
                          McGill University
Routine:
  void CAcorr (AFILE *AFp[2], long int Start[2], long int Nsamp, int delay,
               long int Nsseg, struct Stats_T *Stats)

Purpose:
  Gather correlation statistics for two audio files

Description:
  This routine gathers cross-statistics for audio files.  The statistics are
  the sum of cross-products, number of differences, maximum difference,
  number of difference runs, and the accumulated log SNR values for segments
  of length Nsseg.

Parameters:
   -> AFILE *AFp[2]
      Audio file pointers
   -> long int Start[2]
      Start samples
   -> long int Nsamp
      Number of samples
   -> int delay
      delay of file B relative to file A
   -> int Nsseg
      Segment length in samples for segmental SNR computations
  <-  struct Stats_T *Stats
      Structure containing the file statistics

Author / revision:
  P. Kabal  Copyright (C) 1999
  $Revision: 1.17 $  $Date: 1999/06/12 21:51:21 $

-----------------------------------------------------------------------*/

static char rcsid[] = "$Id: CAcorr.c 1.17 1999/06/12 AFsp-v6r8 $";

#include <math.h>	/* fabs, log10 */

#include <libtsp.h>
#include "CompAudio.h"

#define ABSV(x)		(((x) < 0) ? -(x) : (x))
#define MINV(a, b)	(((a) < (b)) ? (a) : (b))
#define MAXV(a, b)	(((a) > (b)) ? (a) : (b))

#define NBUF	2560

static void
CA_corr (struct Stats_T *Stats, const float Xa[], const float Xb[], int N);


void
CAcorr (AFILE *AFp[2], long int Start[2], long int Nsamp, int delay,
	long int Nsseg, struct Stats_T *Stats)

{
  float Xa[NBUF];
  float Xb[NBUF];
  int Nv, diffA, diffB;
  long int ioffs, Nrem;

/* Initialization */
  Stats_T_INIT (Stats, Nsseg);

  ioffs = 0L;
  diffA = MAXV (0, -delay) + Start[0];
  diffB = MAXV (0, delay) + Start[1];
  Nrem = Nsamp;
  while (Nrem) {

/* Read the audio files */
    Nv = (int) MINV (Nrem, NBUF);
    AFreadData (AFp[0], ioffs+diffA, Xa, Nv);
    AFreadData (AFp[1], ioffs+diffB, Xb, Nv);
    ioffs += Nv;
    Nrem -= Nv;

    CA_corr (Stats, Xa, Xb, Nv);
  }

  return;
}



static void
CA_corr (struct Stats_T *Stats, const float Xa[], const float Xb[], int N)

{
  int i;
  double Sx2, Sy2, Sxy;
  float diff;

  /* Data comparisons */
  Sx2 = 0.0;
  Sy2 = 0.0;
  Sxy = 0.0;

  for (i = 0; i < N; ++i) {

    if (Xa[i] != Xb[i]) {
      if (Stats->Inrun == 0) {
	++Stats->Nrun;
	Stats->Inrun = 1;
      }
      ++Stats->Ndiff;
      diff = Xa[i] - Xb[i];
      Stats->Diffmax = MAXV (Stats->Diffmax, ABSV (diff));
    }
    else {
      Stats->Inrun = 0;
    }

    /* Cross products */
    Sx2 += (double) Xa[i] * (double) Xa[i];
    Sy2 += (double) Xb[i] * (double) Xb[i];
    Sxy += (double) Xa[i] * (double) Xb[i];

    /* Segmental SNR update */
    Stats->Ssx2 += (double) Xa[i] * (double) Xa[i];
    Stats->Ssd2 += (double) (Xb[i] - Xa[i]) * (double) (Xb[i] - Xa[i]);
    ++Stats->ks;
    if (Stats->ks >= Stats->Nsseg) {
      Stats->SNRlog += log10 (1.0 + Stats->Ssx2 / Stats->Ssd2);
      ++Stats->Nseg;
      Stats->Ssx2 = 0.0;
      Stats->Ssd2 = 1e-2;
      Stats->ks = 0;
    }

  }
  Stats->Sx2 += Sx2;
  Stats->Sy2 += Sy2;
  Stats->Sxy += Sxy;

  return;
}
