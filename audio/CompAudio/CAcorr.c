/*------------- Telecommunications & Signal Processing Lab --------------
                          McGill University
Routine:
  void CAcorr (AFILE *AFp[2], long int Start[2], long int Nsamp,
               long int delay, long int Nsseg, struct Stats_T *Stats)

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
   -> long int delay
      delay of file B relative to file A
   -> long int Nsseg
      Segment length in samples for segmental SNR computations
  <-  struct Stats_T *Stats
      Structure containing the file statistics

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.21 $  $Date: 2003/07/14 11:28:54 $

-----------------------------------------------------------------------*/

#include <assert.h>
#include <math.h>	/* log10 */

#include <libtsp.h>
#include "CompAudio.h"

#define ABSV(x)		(((x) < 0) ? -(x) : (x))
#define MINV(a, b)	(((a) < (b)) ? (a) : (b))
#define MAXV(a, b)	(((a) > (b)) ? (a) : (b))
#define SQRV(x)		((x) * (x))

#define NBUF	2560

struct ActLev_W {
  double Ssx2;		/* Sum x[i]*x[i] for a segment */
  double Ssd2;		/* Sum (x[i]-y[i])(x[i]-y[i]) for a segment */
  int ks;		/* Segment sample counter */
};

static const struct Stats_T Stats_T_Init = {
  0.0, 0.0, 0.0, 0L, 0.0, 0L, 0, 0L, 0L, 0.0 };
static const struct ActLev_W ActLev_W_Init = { 0.0, 0.0, 0 };

    
static void
CA_corr (struct Stats_T *Stats, const double Xa[], const double Xb[], int N,
	 struct ActLev_W *AWork);


void
CAcorr (AFILE *AFp[2], long int Start[2], long int Nsamp, long int delay,
	long int Nsseg, struct Stats_T *Stats)

{
  double Xa[NBUF];
  double Xb[NBUF];
  int Nv;
  long int diffA, diffB;
  long int ioffs, Nrem;
  struct ActLev_W AWork;

/* Initialization */
  *Stats = Stats_T_Init;
  Stats->Nsseg = Nsseg;
  AWork = ActLev_W_Init;

/* Treat multi-channel files as single channel files */
  assert (AFp[0]->Nchan == AFp[1]->Nchan);
  delay *= AFp[0]->Nchan;
  Nsseg *= AFp[0]->Nchan;

  ioffs = 0L;
  diffA = MAXV (0, -delay) + Start[0];
  diffB = MAXV (0, delay) + Start[1];
  Nrem = Nsamp;
  while (Nrem) {

/* Read the audio files */
    Nv = (int) MINV (Nrem, NBUF);
    AFdReadData (AFp[0], ioffs+diffA, Xa, Nv);
    AFdReadData (AFp[1], ioffs+diffB, Xb, Nv);
    ioffs += Nv;
    Nrem -= Nv;

    CA_corr (Stats, Xa, Xb, Nv, &AWork);
  }

  return;
}

#define EPSD	(0.01/(32768.*32768.))


static void
CA_corr (struct Stats_T *Stats, const double Xa[], const double Xb[], int N,
	 struct ActLev_W *AWork)

{
  int i;
  double diff, Sx2, Sy2, Sxy;

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
    else
      Stats->Inrun = 0;

    /* Cross products */
    Sx2 += SQRV (Xa[i]);
    Sy2 += SQRV (Xb[i]);
    Sxy += Xa[i] * Xb[i];

    /* Segmental SNR update */
    AWork->Ssx2 += SQRV (Xa[i]);
    AWork->Ssd2 += SQRV (Xb[i] - Xa[i]);
    ++AWork->ks;
    if (AWork->ks >= Stats->Nsseg) {
      Stats->SNRlog += log10 (1.0 + AWork->Ssx2 / (EPSD + AWork->Ssd2));
      ++Stats->Nseg;
      *AWork = ActLev_W_Init;
    }
    /* EPSD ensures that the denominator is non-zero. It was set to
       0.01 for 16-bit data, i.e. a small fraction of the minimum
       non-zero value. This value is 0.01/(32768*32768) for unit
       full scale.
    */
  }

  Stats->Sx2 += Sx2;
  Stats->Sy2 += Sy2;
  Stats->Sxy += Sxy;

  return;
}
