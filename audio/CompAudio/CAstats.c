/*------------- Telecommunications & Signal Processing Lab --------------
                          McGill University
Routine:
  void CAstats(AFILE *AFp, long int Start, long int Ns, long int Nch,
               struct Stats_F StatsF[])

Purpose:
  Gather data statistics from an audio file

Description:
  This routine gathers statistics for an audio file.

  A set of statistics is calculated for each channel, unless the number of
  channels is large. In that case, the samples for all channels are combined
  into a single channel. Some of the statistics are not meaningful for combined
  channel data.
  - The sum of values and the sum of squared values. These are used to compute
    the mean and standard deviation.
  - The minimum and maximum value.
  - The number of overloads and the number of overload runs.
  - The number of anomalous transitions is computed for each channel. If the
    channels are combined, this value is not appropriate and is set to zero.
  - The envelope activity factor.

Parameters:
   -> AFILE *AFp
      Audio file pointer for an audio file opened by AFopnRead
   -> long int Start
      Starting sample number
   -> long int Ns
      Number of samples (all channels). If Nsamp == AF_NSAMP_UNDEF, the
      end-of-file is used to determine the number of samples.
   -> long int Nch
      Number of effective channels.
  <-  struct Stats_F StatsF[]
      Structures containing the file statistics (Nch structures)

Author / revision:
  P. Kabal  Copyright (C) 2020
  $Revision: 1.44 $  $Date: 2020/11/27 02:07:49 $

-----------------------------------------------------------------------*/

#include <assert.h>
#include <limits.h>
#include <math.h>

#include "CompAudio.h"

#define MINV(a, b)    (((a) < (b)) ? (a) : (b))
#define MAXV(a, b)    (((a) > (b)) ? (a) : (b))
#define ABSV(x)       (((x) < 0) ? -(x) : (x))
#define ICEILV(n,m)   (((n) + ((m) - 1)) / (m))  /* int n,m >= 0 */
#define RNDUPV(n,m)   ((m) * ICEILV(n, m))       /* Round up */
#define SQRV(x)       ((x) * (x))
#define NELEM(array)  ((int) ((sizeof array) / (sizeof array[0])))

#define NBUF  2560
#define MDB   15.9  /* Active speech offset (dB) */
#define NTHR  15    /* Number of levels for speech activity level */

struct SpAct_P {
  double c[NTHR]; /* Threshold values */
  double g;       /* Envelope filter parameter */
  long int I;     /* Hangover limit */
};

struct SpAct_S {
  long int a[NTHR]; /* Threshold counts */
  long int h[NTHR]; /* Hangover counts */
  double p;         /* Envelope intermediate filter state */
  double q;         /* Envelope value (state variable) */
};

struct Thresh_L {
  double Amax;    /* Value used to detect overloads */
  double Amin;    /* Value used to detect overloads */
  double Aup;     /* Upper anomalous transition region */
  double Alw;     /* Lower anomalous transition region */
};

#define P_OVERL     (2)
#define P_HIGH      (1)
#define PN_NORMAL   (0)
#define N_HIGH      (-1)
#define N_OVERL     (-2)
static const struct Stats_F Stats_F_Init = {
  0L, 0.0, 0.0, DBL_MAX, -DBL_MAX, 0L, 0L, 0L, PN_NORMAL, 0.0 };

static int
CA_INpar(struct SpAct_P *SpPar, double Sfreq);
static void
CA_INcount(struct SpAct_S *SpAct);
static void
CA_INThresh(int Format, double ScaleF, int ChComb, struct Thresh_L *Thresh);
static void
CA_stats(struct Stats_F *Stats, const struct Thresh_L *Thresh,
         const double x[], int N, int Nsub);
static void
CA_SAcount(struct SpAct_S *SpAct, const struct SpAct_P *SpPar,
           const double x[], int N, int Nsub);
static double
CA_ActLevel(const long int a[], const double c[], double Sx2, double MdB);


void
CAstats(AFILE *AFp, long int Start, long int Nsamp, long int Nch,
        struct Stats_F StatsF[])

{
  double X[NBUF];
  int eof, k, is, i0, m, Nv, Nr, Nsub, SA, ChComb;
  long int ioffs, Ns, Nrem, Nchan;
  struct SpAct_P SpPar;
  struct SpAct_S *SpAct;
  struct Thresh_L Thresh;
  double Sxx;

  Nchan = AFp->Nchan;       /* Actual number of channels */
  ChComb = (Nch != Nchan);  /* Working with a single channel */

  /* Initialize statistics */
  for (k = 0; k < Nch; ++k)
    StatsF[k] = Stats_F_Init;

  /* Initialize the data-specific thresholds */
  CA_INThresh(AFp->Format, AFp->ScaleF, ChComb, &Thresh);

  /* Turn off speech activity factor calculations for non-speech files - the
     active level will be returned as zero */
  SA = (Nchan <= 2 && !ChComb &&
        AFp->Sfreq >= SA_SFREQ_L && AFp->Sfreq <= SA_SFREQ_U);

  if (SA) {
    /* Set up the speech activity measurement parameters */
    Nsub = CA_INpar(&SpPar, AFp->Sfreq);

    /* Allocate storage for the speech activity variables structure */
    SpAct = (struct SpAct_S *) UTmalloc(Nchan * sizeof(struct SpAct_S));

    /* Initialize the speech activity variables */
    for (k = 0; k < Nchan; ++k)
      CA_INcount(&SpAct[k]);
  } else {
    SpAct = NULL;   /* Keep compiler happy */
    Nsub = 1;
  }

  i0 = 0;     /* Index of first sample of channel 0 in the buffer */
  is = 0;     /* Index of first subsampled sample of channel 0 in the buffer */
  ioffs = Start;
  eof = (Nsamp == AF_NSAMP_UNDEF);
  if (eof)
    Nrem = LONG_MAX;
  else
    Nrem = Nsamp;

  while (Nrem > 0L) {

    /* Read the audio file */
    Nv = (int) MINV(Nrem, NBUF);
    Nr = AFdReadData(AFp, ioffs, X, Nv);
    if (eof && Nr < Nv) {
      Nrem = Nr;
      Nv = Nr;
    }
    ioffs += Nv;
    Nrem -= Nv;

    /* Gather statistics for each channel
       Sxx (sum of squares), Sx (sum of values), # overloads & runs, # anom
       Note: StatsF[k].Nrun = 0 if channels are combined
             StatsF[k].Nanomal = 0 if channels are combined
    */
    for (k = 0; k < Nch; ++k) {

      m = (i0 + k) % Nch;  /* Offset, 0 to Nch-1 */
      if (m < Nv)
        CA_stats(&StatsF[k], &Thresh, &X[m], Nv - m, Nch);
      if (ChComb) {
        StatsF[k].Nrun = 0L;
        StatsF[k].Nanomal = 0L;
      }
    }
    i0 += RNDUPV(Nv - i0, Nch) - Nv;

    /* Update the speech activity counts (subsampled buffer) */
    if (SA) {
      for (k = 0; k < Nchan; ++k) {
        m = (is + k) % (Nsub*Nchan);
        if (m < Nv)
          CA_SAcount(&SpAct[k], &SpPar, &X[m], Nv - m, Nsub*Nchan);
      }
      is += RNDUPV(Nv - is, Nsub*Nchan) - Nv;
    }
  }
  /* Buffer pointers
     - Consider a block of Nv samples
     - The pointer i0 gives the first sample from channel 0 in the current
       buffer. This value may point beyond the buffer end if there are no
       samples from channel 0 in the current buffer.
     - m(k) is a pointer to the first sample in the buffer for channel k. Note
       that m(k) can be less than i0 (partial block of samples before the first
       channel 0 sample), or m(k) can be larger than and equal to Nv (no samples
       from channel k in the buffer).
     - After processing a block of samples (possibly with no actual updates of
       the statistics, i.e. Nv-m(k) < 0 for all channels), i0 is updated to
       point to the first sample for channel 0 in the next buffer.
     - Consider appending new samples to the current buffer. The first sample
       for channel 0 in the new part of the buffer is a multiple of NchanX
       beyond i0 and larger than or equal to Nv-i0.
         ix = i0 + RNDUPV(Nv-i0,NchanX).
       The new value of i0 is this value decreased by Nv, since the new buffer
       will overwrite the old buffer,
         i0 = i0 + RNDUPV(Nv-i0,NchanX) - Nv.
       This expression always gives a non-negative value of i0.
  */

  /* Calculate the active speech level */
  if (SA) {
    for (k = 0; k < Nchan; ++k) {
      Ns = StatsF[k].N / Nsub; /* Number of subsampled values */
      Sxx = 0.0;
      if (Ns > 0)
        Sxx = Ns * (StatsF[k].Sxx / StatsF[k].N);
      StatsF[k].ActLev = CA_ActLevel(SpAct[k].a, SpPar.c, Sxx, MDB);
    }
    UTfree(SpAct);
  }

}

/* Initialize the speech activity measurement parameters */

/* ITU-T Recommendation P.56 speech activity calculations:
   Find the largest subsampling factor that gives a sampling rate of at
   least EFREQ
*/

#define EFREQ 1000.0   /* Samping frequency for speech activity */
#define HSEC  0.2      /* Hangover time (seconds) */
#define T 0.03         /* Envelope time constant (seconds) */
#define MAXC  0.5      /* Largest threshold */

static int
CA_INpar(struct SpAct_P *SpPar, double Sfreq)

{
  int i, Nsub;

  Nsub = (int) (Sfreq / EFREQ);
  if (Nsub <= 0)
    Nsub = 1;

  SpPar->I = (long int) ceil(HSEC * Sfreq / Nsub);
  SpPar->g = exp(-Nsub / (Sfreq * T));

  /* Thresholds {... 1/8 1/4 1/2 1} * MAXC */
  for (i = 0; i < NTHR; ++i)
    SpPar->c[i] = pow(2.0, (double) (i - (NTHR-1))) * MAXC;

  return Nsub;
}

/* Initialize the speech activity counts */


static void
CA_INcount(struct SpAct_S *SpAct)

{
  int i;

  for (i = 0; i < NTHR; ++i) {
    SpAct->a[i] = 0L;
    SpAct->h[i] = LONG_MAX;
  }
  SpAct->p = 0.0;
  SpAct->q = 0.0;
}

/* Fill in the threshold values used to specify overload and anomalous
   transition regions
*/


static void
CA_INThresh(int Format, double ScaleF, int ChComb, struct Thresh_L *Thresh)

{
  static const struct Thresh_L Thresh_T[] = {
    /*     Amax          Amin          Aup          Alw  */
  {          0.,           0.,          0.,           0.}, /* Undefined */
  {      32256.,      -32256.,      16384.,      -16384.}, /* A-law */
  {      32124.,      -32124.,      16384.,      -16384.}, /* mu-law */
  {      32124.,      -32124.,      16384.,      -16384.}, /* mu-lawR */
  {      32767.,      -32768.,      16384.,      -16384.}, /* 8-b uint */
  {      32767.,      -32768.,      16384.,      -16384.}, /* 8-b int */
  {      32767.,      -32768.,      16384.,      -16384.}, /* 16-b int */
  {    8388607.,    -8388608.,    4194304.,    -4194304.}, /* 24-b int */
  { 2147483647., -2147483648., 1073741824., -1073741824.}, /* 32-b int */
  {          1.,          -1.,         0.5,         -0.5}, /* 32-b float */
  {          1.,          -1.,         0.5,         -0.5}, /* 64-b float */
  {      32767.,      -32768.,      16384.,      -16384.}, /* text16 */
  {          1.,          -1.,         0.5,         -0.5}, /* text */
  };

  /* Thresholds to determine the sample region
            x <= Amin      Negative overload
    Amin <  x <= Alw       Negative high
    Alw  <  x <  Aup       Normal region
    Aup  <= x <  Amax      Positive high
    Amax <= x              Positive overload
  */

  assert(Format >= 1 && Format < AF_NFD);
  assert(NELEM(Thresh_T) == AF_NFD);

  *Thresh = Thresh_T[Format];
  if (ScaleF == 0.0)
    ScaleF = ABSV(1.0 / Thresh->Amin);
  Thresh->Amax *= ScaleF;
  Thresh->Amin *= ScaleF;
  Thresh->Aup  *= ScaleF;
  Thresh->Alw  *= ScaleF;

  /* If the data channels are combined, the anomalous transition count is not
     meaningful. In that case, change the thresholds for anomalous transitions
     to reduce the computational load. */
  if (ChComb) {
    Thresh->Aup = Thresh->Amax;
    Thresh->Alw = Thresh->Amin;
  }
}

/* Update statistics for one buffer of data */

static void
CA_stats(struct Stats_F *Stats, const struct Thresh_L *Thresh, const double x[],
         int N, int Nch)

{
  long double Sx;
  long double Sxx;
  int i;

  Sx = 0.0;
  Sxx = 0.0L;
  for (i = 0; i < N; i += Nch) {

    ++Stats->N;

    /* Min and Max values */
    Stats->Vmax = MAXV(Stats->Vmax, x[i]);
    Stats->Vmin = MINV(Stats->Vmin, x[i]);

    /* Detect overloads and anomalous transitions */
    if (x[i] >= Thresh->Aup) {
      if (x[i] >= Thresh->Amax) {
        ++Stats->Novload;
        if (Stats->Region != P_OVERL)
          ++Stats->Nrun;
        if (Stats->Region <= N_HIGH)
          ++Stats->Nanomal;
        Stats->Region = P_OVERL;
      }
      else {
        if (Stats->Region <= N_HIGH)
          ++Stats->Nanomal;
        Stats->Region = P_HIGH;
      }
    }
    else if (x[i] <= Thresh->Alw) {
      if (x[i] <= Thresh->Amin) {
        ++Stats->Novload;
        if (Stats->Region != N_OVERL)
          ++Stats->Nrun;
        if (Stats->Region >= P_HIGH)
          ++Stats->Nanomal;
        Stats->Region = N_OVERL;
      }
      else {
        if (Stats->Region == P_HIGH)
          ++Stats->Nanomal;
        Stats->Region = N_HIGH;
      }
    }
    else {
      Stats->Region = PN_NORMAL;
    }

    /* Accumulate the double sum and sum of products */
    Sx += (long double) x[i];
    Sxx += SQRV((long double) x[i]);
  }
  Stats->Sx += Sx;
  Stats->Sxx += Sxx;
}

/* Update the speech activity counts */

#define P SpAct->p
#define Q SpAct->q
#define A SpAct->a
#define H SpAct->h

#define C SpPar->c
#define G SpPar->g
#define HI  SpPar->I


static void
CA_SAcount(struct SpAct_S *SpAct, const struct SpAct_P *SpPar, const double x[],
           int N, int Nsub)

{
  int i, j;

  /* Speech activity calculations */
  for (i = 0; i < N; i += Nsub) {

    /* Smoothed envelope */
    P = G * P + (1.0 - G) * ABSV(x[i]);
    Q = G * Q + (1.0 - G) * P;

    /* Update the envelope activity counts */
    for (j = 0; j < NTHR; ++j) {
      if (Q >= C[j]) {
        ++A[j];
        H[j] = 0;
      }
      else if (H[j] < HI) {
        ++A[j];
        ++H[j];
      }
      else
        break;
    }
  }
}

/* Calculate the active speech level

  <-  double CA_ActLevel
      Active speech level
   -> long int a[]
      Activity counts (NTHR non-increasing values)
   -> double c[]
      Threshold levels (NTHR increasing values)
   -> double Sx2
      Sum of squares for the input signal
   -> double MdB
      Offset in dB
*/


static double
CA_ActLevel(const long int a[], const double c[], double Sx2, double MdB)

{
  double ActLev, Mln, Alnj, Clnj, dACj, alpha, Alno, Alnjp, dACjp;
  int j;

  Alnjp = 0.0;  /* Stop compiler warning */
  dACjp = 0.0;

  /* Default return value */
  ActLev = 0.0;

  Mln = MdB * log(10.0) / 10.0;   /* log(m), where m = 10^(M/10) */

  /* Find the solution to Aln - Cln = Mln */
  /* Calculations are carried out in natural logs */
  for (j = 0; j < NTHR; ++j) {
    if (a[j] == 0)    /* a[j] is a decreasing function of j */
      break;

    Alnj = log(Sx2 / a[j]);     /* Assume Sx2 > 0, since a[j] > 0 */
    Clnj = 2.0 * log(c[j]);     /* log(c[j]^2) */
    dACj = Alnj - Clnj;
    if (j == 0) {
      if (dACj <= Mln) {
        ActLev = exp(0.5 * Alnj);
        break;
      }
    }
    else if (dACj <= Mln) {
      alpha = (Mln - dACjp) / (dACj - dACjp);
      Alno = Alnjp + alpha * (Alnj - Alnjp);
      ActLev = exp(0.5 * Alno);
      break;
    }
    Alnjp = Alnj;
    dACjp = dACj;
  }

  return ActLev;
}
