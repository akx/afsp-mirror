/*------------- Telecommunications & Signal Processing Lab --------------
                          McGill University
Routine:
  Stats_X CAcorr(AFILE *AFp[2], const long int Start[2], long int Nsamp,
                 long int delay, long int Nsseg)

Purpose:
  Gather correlation statistics for two audio files

Description:
  This routine gathers cross-statistics for audio files. The statistics are
  the sum of cross-products, number of differences, maximum difference,
  number of difference runs, and the accumulated log SNR values for segments
  of length Nsseg.

Parameters:
  <-  Stats_X CAcorr
      Structure containing the file statistics for delay
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

Author / revision:
  P. Kabal  Copyright (C) 2020
  $Revision: 1.31 $  $Date: 2020/11/27 02:06:55 $

-----------------------------------------------------------------------*/

#include <assert.h>
#include <math.h> /* log10 */

#include "CompAudio.h"

#define ABSV(x)     (((x) < 0) ? -(x) : (x))
#define MINV(a, b)  (((a) < (b)) ? (a) : (b))
#define MAXV(a, b)  (((a) > (b)) ? (a) : (b))
#define SQRV(x)     ((x) * (x))

#define NBUF  2560

struct ActLev_W {
  double Ssx2;    /* Sum x[i]*x[i] for a segment */
  double Ssd2;    /* Sum (x[i]-y[i])(x[i]-y[i]) for a segment */
  int ks;         /* Segment sample counter */
};

static const struct Stats_X Stats_X_Init = {
  0.0L, 0.0L, 0.0L, 0.0L, 0L, 0L, 0.0, 0L, 0, 0L, 0L, 0.0 };
static const struct ActLev_W ActLev_W_Init = { 0.0, 0.0, 0 };


static void
CA_corr(struct Stats_X *StatsX, const double Xa[], const double Xb[], int N,
        struct ActLev_W *AWork);


struct Stats_X
CAcorr(AFILE *AFp[2], const long int Start[2], long int Nsamp, long int delay,
       long int Nsseg)

{
  double Xa[NBUF];
  double Xb[NBUF];
  int Nv;
  long int Edelay, diffA, diffB;
  long int ioffs, Nrem;
  struct Stats_X StatsX;
  struct ActLev_W AWork;

/* Initialization */
  StatsX = Stats_X_Init;
  StatsX.Nsseg = Nsseg;
  StatsX.Delay = delay;
  AWork = ActLev_W_Init;

/* Treat multi-channel files as single channel files */
  assert(AFp[0]->Nchan == AFp[1]->Nchan);
  delay *= AFp[0]->Nchan;
  Nsseg *= AFp[0]->Nchan;

/*
  A delay of D samples, means that an event that occurs at time n, now occurs at
  time n+D, means that Xa[n] aligns with Xb[n-D]
  Alignment               pos D             neg D
         0      N-1     0      N-1         D   0      N-1
    Xa = xxxxxxxx    => xxxxxxxx....    => ....xxxxxxxx
    Xb = yyyyyyyy    => ....yyyyyyyy    => yyyyyyyy....
         0      N-1    -D   0      N-1     0      N-1
 
  The file data is conceptually extended with zeros at either end. The length of
  vectors after shifting is N+|D|. If |D| is greater than Nsamp, the effective
  delay can be set to +/-Nsamp - there will be no overlap of Xa and Xb.
*/
  ioffs = 0L;
  Edelay = MAXV(-Nsamp, MINV(Nsamp, delay));
  diffA = MINV(0L, +Edelay) + Start[0];
  diffB = MINV(0L, -Edelay) + Start[1];
  Nrem = Nsamp + ABSV(Edelay);
  while (Nrem) {

/* Read the audio files */
    Nv = (int) MINV(Nrem, NBUF);
    AFdReadData(AFp[0], ioffs+diffA, Xa, Nv);
    AFdReadData(AFp[1], ioffs+diffB, Xb, Nv);
    ioffs += Nv;
    Nrem -= Nv;

    CA_corr(&StatsX, Xa, Xb, Nv, &AWork);
  }

  return StatsX;
}

#define EPSD  (0.01/(32768.*32768.))


static void
CA_corr(struct Stats_X *StatsX, const double Xa[], const double Xb[], int N,
        struct ActLev_W *AWork)

{
  int i;
  double diff;
  long double Sxx, Syy, Sxy, See;

  /* Data comparisons */
  Sxx = 0.0L;
  Syy = 0.0L;
  Sxy = 0.0L;
  See = 0.0L;

/* Computational considerations:
  The problem is to avoid loss of precision when calculating the sum of
  products. Consider integer M-bit data (sign + M-1 bits) and calculating the
  sum of squares. The product of two M-bit values has 2M-1 bits (sign + 2M
  bits). If the standard deviation of the values is SD, the sum of N squares
  will be S = N * SD^2 * 2^(2M-1).

  For instance, for M = 24, and SD = 1/16, S = N * 2^39. For a double precision
  accumulator with 53 bits of mantissa + sign, N is limited to 2^15 samples.
  This is a small fraction of a second for data at 48 kHz sampling rate. If N is
  larger than this value, precision is lost.

  The situation is much better for 16 bit data. N can be 2^16 bigger before
  precision is lost.

  Using a long double accumulator when the long double is 80 bits with 64 bits
  of mantissa, N can be 2^16 bigger. This is the case for long double using the
  gcc compiler on PC's. In MS visual the long double type is the same as double.

  The loss of precision is somewhat mitigated by forming partial sums, buffer by
  buffer.
*/

  for (i = 0; i < N; ++i) {

    if (Xa[i] != Xb[i]) {
      if (StatsX->Inrun == 0) {
        ++StatsX->Nrun;
        StatsX->Inrun = 1;
      }
      ++StatsX->Ndiff;
      diff = Xa[i] - Xb[i];
      StatsX->Diffmax = MAXV(StatsX->Diffmax, ABSV(diff));
    }
    else
      StatsX->Inrun = 0;

    /* Cross products */
    Sxx += SQRV((long double) Xa[i]);
    Syy += SQRV((long double) Xb[i]);
    Sxy += (long double) Xa[i] * (long double) Xb[i];
    See += SQRV((long double) Xa[i] - (long double) Xb[i]);

    /* Segmental SNR update */
    AWork->Ssx2 += SQRV(Xa[i]);
    AWork->Ssd2 += SQRV(Xb[i] - Xa[i]);
    ++AWork->ks;
    if (AWork->ks >= StatsX->Nsseg) {
      StatsX->SNRlog += log10(1.0 + AWork->Ssx2 / (EPSD + AWork->Ssd2));
      ++StatsX->Nseg;
      *AWork = ActLev_W_Init;
    }
    /* EPSD ensures that the denominator is non-zero. It was set to 0.01 for
       16-bit data, i.e. a small fraction of the minimum non-zero value. This
       value is 0.01/(32768*32768) for unit full scale.
    */
  }

  StatsX->Sxx += Sxx;
  StatsX->Syy += Syy;
  StatsX->Sxy += Sxy;
  StatsX->See += See;
}
