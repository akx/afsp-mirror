/*------------- Telecommunications & Signal Processing Lab --------------
                          McGill University
Routine:
  void CAstats (AFILE *AFp, long int Start, long int Nsamp,
                struct Stats_F Stats[], int Nchan)

Purpose:
  Gather statistics from an audio file

Description:
  This routine gathers statistics for an audio file.  The statistics are the
  sum of values, the sum of squared values, minimum value, and maximum value.
  In addition, the number overloads, the number of overload runs, the number of
  anomalous transitions, and the envelope activity level are returned.

Parameters:
   -> AFILE *AFp
      Audio file pointer for an audio file opened by AFopnRead
   -> long int Start
      Starting sample number
   -> long int Nsamp
      Number of samples.  If Nsamp == AF_NSAMP_UNDEF, the end-of-file is used.
  <-  struct Stats_F Stats[]
      Structures containing the file statistics (two structures for two
      channels; one structure for other numbers of channels)

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.30 $  $Date: 2003/07/11 14:40:06 $

-----------------------------------------------------------------------*/

#include <assert.h>
#include <limits.h>
#include <math.h>

#include <libtsp.h>
#include <libtsp/AFpar.h>
#include "CompAudio.h"

#define MINV(a, b)	(((a) < (b)) ? (a) : (b))
#define MAXV(a, b)	(((a) > (b)) ? (a) : (b))
#define ABSV(x)		(((x) < 0) ? -(x) : (x))
#define ICEILV(n, m)	(((n) + ((m) - 1)) / (m))	/* int n,m >= 0 */
#define SQRV(x)		((x) * (x))

#define NBUF	2560
#define MDB	15.9	/* Active speech offset (dB) */
#define NTHR	15	/* Number of levels for speech activity level */

struct SpAct_P {
  double c[NTHR];	/* Threshold values */
  double g;		/* Envelope filter parameter */
  long int I;		/* Hangover limit */
};

struct SpAct_S {
  long int a[NTHR];	/* Threshold counts */
  long int h[NTHR];	/* Hangover counts */
  double p;		/* Envelope intermediate filter state */
  double q;		/* Envelope value (state variable) */
};

struct Thresh_L {
  double Amax;		/* Value used to detect overloads */
  double Amin;		/* Value used to detect overloads */
  double Aup;		/* Upper anomalous transition region */
  double Alw;		/* Lower anomalous transition region */
};

#define P_OVERL		(2)
#define P_HIGH		(1)
#define PN_NORMAL	(0)
#define N_HIGH		(-1)
#define N_OVERL		(-2)
static const struct Stats_F Stats_F_Init = {
  0L, 0.0, 0.0, DBL_MAX, -DBL_MAX, 0L, 0L, 0L, PN_NORMAL, 0.0 };
 
static int
CA_INpar (struct SpAct_P *SpPar, double Sfreq);
static void
CA_INcount (struct SpAct_S *SpAct);
static void
CA_INThresh (int Format, double ScaleF, struct Thresh_L *Thresh);
static void
CA_stats (struct Stats_F *Stats, const struct Thresh_L *Thresh,
	  const double x[], int N, int Nsub);
static void
CA_SAcount (struct SpAct_S *SpAct, const struct SpAct_P *SpPar,
	    const double x[], int N, int Nsub);
static double
CA_ActLevel (const long int a[], const double c[], double Sx2, double MdB);


void
CAstats (AFILE *AFp, long int Start, long int Nsamp, struct Stats_F Stats[])

{
  double X[NBUF];
  int eof, k, is, it, m, Nv, Nr, Nsub, SA;
  long int ioffs, Ns, Nrem, Nchan;
  struct SpAct_P SpPar;
  struct SpAct_S *SpAct;
  struct Thresh_L Thresh;
  double Sxx;

  Nchan = AFp->Nchan;
  if (Nchan > 2)
    Nchan = 1;

  /* Turn off speech activity factor calculations for non-speech files - the
     active level will be returned as zero */
  SA = (AFp->Nchan <= 2 &&
	AFp->Sfreq >= SA_SFREQ_L && AFp->Sfreq <= SA_SFREQ_U);

  /* Set up the speech activity measurement parameters */
  Nsub = CA_INpar (&SpPar, AFp->Sfreq);

  /* Allocate storage for the speech activity variables structure */
  SpAct = (struct SpAct_S *) UTmalloc (Nchan * (sizeof (struct SpAct_S)));

  for (k = 0; k < Nchan; ++k) {

    /* Initialize the statistics variables */
    Stats[k] = Stats_F_Init;

    /* Initialize the speech activity variables */
    CA_INcount (&SpAct[k]);
  }

  /* Initialize the data-specific thresholds */
  CA_INThresh (AFp->Format, AFp->ScaleF, &Thresh);

  is = 0;
  it = 0;
  ioffs = Start;
  eof = (Nsamp == AF_NSAMP_UNDEF);
  if (eof)
    Nrem = LONG_MAX;
  else
    Nrem = Nsamp;

  while (Nrem > 0L) {

    /* Read the audio file */
    Nv = (int) MINV (Nrem, NBUF);
    Nr = AFdReadData (AFp, ioffs, X, Nv);
    if (eof && Nr < Nv) {
      Nrem = Nr;
      Nv = Nr;
    }
    ioffs += Nv;
    Nrem -= Nv;

    for (k = 0; k < Nchan; ++k) {

      /* Gather statistics */
      m = (it + k) % Nchan;	/* Offset, 0 to Nchan-1 */
      if (m < Nv)
	CA_stats (&Stats[k], &Thresh, &X[m], Nv - m, Nchan);

      /* Update the speech activity counts */
      if (SA) {
	m = (is + k) % (Nsub*Nchan);
	if (m < Nv)
	  CA_SAcount (&SpAct[k], &SpPar, &X[m], Nv - m, Nsub*Nchan);
      }
    }
    it += Nchan * ICEILV (Nv - it, Nchan) - Nv;
    is += (Nsub*Nchan) * ICEILV (Nv - is, Nsub*Nchan) - Nv;
  }
  /* Buffer pointers
     - Consider a block of samples, size Nchan
     - The pointer it gives the first sample from the channel 0 in the
       current buffer.  This value may point beyond the buffer end if there are
       no samples from channel 0 in the current buffer.
     - m(k) is a pointer to the first sample in the buffer for channel k.  Note
       that m can be less than it (partial block of samples before the first
       channel 0 sample), or m(k) can be larger than and equal to Nv (no
       samples from channel k in the buffer).
     - After processing a block of samples (possibly with no actual updates
       of the statistics, i.e. Nv-m(k) < 0 for all channels), it is updated to
       point to the first sample for channel 0 in the next buffer.  First
       consider appending new samples to the current buffer.  The first sample
       for channel 0 in the new part of the buffer is a multiple of Nchan
       beyond it and larger than or equal to Nv-it.
         ix = it + Nchan * ICEILV (Nv-it,Nchan).
       The new value of it is this value decreased by Nv, since the new buffer
       will overwrite the old buffer,
         it = it + Nchan * ICEILV (Nv-it,Nchan) - Nv.
       This expression always gives a positive value of it.
  */

/* Calculate the active speech level */
  for (k = 0; k < Nchan; ++k) {
    Ns = Stats[k].N / Nsub;	/* Number of subsampled values */
    Sxx = 0.0;
    if (Ns > 0)
      Sxx = Ns * (Stats[k].Sx2 / Stats[k].N);
    Stats[k].ActLev = CA_ActLevel (SpAct[k].a, SpPar.c, Sxx, MDB);
  }

  UTfree (SpAct);

  return;
}

/* Initialize the speech activity measurement parameters */

/* ITU-T Recommendation P.56 speech activity calculations:
   Find the largest subsampling factor that gives a sampling rate of at
   least EFREQ
*/

#define EFREQ	1000.0		/* Samping frequency for speech activity */
#define HSEC	0.2		/* Hangover time (seconds) */
#define T	0.03		/* Envelope time constant (seconds) */
#define MAXC	0.5		/* Largest threshold */

static int
CA_INpar (struct SpAct_P *SpPar, double Sfreq)

{
  int i, Nsub;

  Nsub = (int) (Sfreq / EFREQ);
  if (Nsub <= 0)
    Nsub = 1;

  SpPar->I = (long int) ceil (HSEC * Sfreq / Nsub);
  SpPar->g = exp (-Nsub / (Sfreq * T));

  /* Thresholds {... 1/8 1/4 1/2 1} * MAXC */
  for (i = 0; i < NTHR; ++i)
    SpPar->c[i] = pow (2.0, (double) (i - (NTHR-1))) * MAXC;

  return Nsub;
}

/* Initialize the speech activity counts */


static void
CA_INcount (struct SpAct_S *SpAct)

{
  int i;

  for (i = 0; i < NTHR; ++i) {
    SpAct->a[i] = 0L;
    SpAct->h[i] = LONG_MAX;
  }
  SpAct->p = 0.0;
  SpAct->q = 0.0;

  return;
}

/* Fill in the threshold values used to specify overload and anomalous
   transition regions
*/


static void
CA_INThresh (int Format, double ScaleF, struct Thresh_L *Thresh)

{
  static const struct Thresh_L Thresh_T[NFD] = {
    /*    Amax           Amin           Aup            Alw  */
  {          0.0,           0.0,          0.0,           0.0}, /* Undefined */
  {      32124.0,      -32124.0,      16384.0,      -16384.0}, /* mu-law */
  {      32256.0,      -32256.0,      16384.0,      -16384.0}, /* A-law */
  {      32767.0,      -32768.0,      16384.0,      -16384.0}, /* 8-b Uint */
  {      32767.0,      -32768.0,      16384.0,      -16384.0}, /* 8-b int */
  {      32767.0,      -32768.0,      16384.0,      -16384.0}, /* 16-b int */
  {    8388607.0,    -8388608.0,    4194304.0,    -4194304.0}, /* 24-b int */
  { 2147483647.0, -2147483648.0, 1073741824.0, -1073741824.0}, /* 32-b int */
  {          1.0,          -1.0,          0.5,          -0.5}, /* 32-b float */
  {          1.0,          -1.0,          0.5,          -0.5}, /* 64-b float */
  {          1.0,          -1.0,          0.5,          -0.5}, /* text */
  };

  /* Thresholds to determine the sample region
            x <= Amin      Negative overload
    Amin <  x <= Alw       Negative high
    Alw  <  x <  Aup       Normal region
    Aup  <= x <  Amax      Positive high
    Amax <= x              Positive overload
  */

  assert (Format >= 1 && Format <= NFD-1);

  *Thresh = Thresh_T[Format];
  if (ScaleF == 0.0)
    ScaleF = ABSV (1.0 / Thresh->Amin);
  Thresh->Amax *= ScaleF;
  Thresh->Amin *= ScaleF;
  Thresh->Aup  *= ScaleF;
  Thresh->Alw  *= ScaleF;

  return;
  }

/* Accumulate statistics for one buffer of data */

static void
CA_stats (struct Stats_F *Stats, const struct Thresh_L *Thresh,
	  const double x[], int N, int Nsub)

{
  double Sx, Sx2;
  int i;
  
  Sx = 0.0;
  Sx2 = 0.0;
  for (i = 0; i < N; i += Nsub) {

    ++Stats->N;

    /* Min and Max values */
    Stats->Vmax = MAXV (Stats->Vmax, x[i]);
    Stats->Vmin = MINV (Stats->Vmin, x[i]);

    /* Detect overloads and anomalous transitions */
    if (x[i] >= Thresh->Aup) {
      if (x[i] >= Thresh->Amax) {
	++Stats->Novload;
	if (Stats->Region != P_OVERL)
	  ++Stats->Nrun;
	Stats->Region = P_OVERL;
      }
      else {
	if (Stats->Region == N_HIGH)
	  ++Stats->Nanomal;
	Stats->Region = P_HIGH;
      }
    }
    else if (x[i] <= Thresh->Alw) {
      if (x[i] <= Thresh->Amin) {
	++Stats->Novload;
	if (Stats->Region != N_OVERL)
	  ++Stats->Nrun;
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
    Sx += x[i];
    Sx2 += SQRV (x[i]);
  }
  Stats->Sx += Sx;
  Stats->Sx2 += Sx2;

  return;
}

/* Update the speech activity counts */

#define P	SpAct->p
#define Q	SpAct->q
#define A	SpAct->a
#define H	SpAct->h

#define C	SpPar->c
#define G	SpPar->g
#define HI	SpPar->I


static void
CA_SAcount (struct SpAct_S *SpAct, const struct SpAct_P *SpPar,
	    const double x[], int N, int Nsub)

{
  int i, j;

  /* Speech activity calculations */
  for (i = 0; i < N; i += Nsub) {

    /* Smoothed envelope */
    P = G * P + (1.0 - G) * ABSV (x[i]);
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

  return;
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
CA_ActLevel (const long int a[], const double c[], double Sx2, double MdB)

{
  double ActLev, Mln, Alnj, Clnj, dACj, alpha, Alno, Alnjp, dACjp;
  int j;

  Alnjp = 0.0;	/* Stop compiler warning */
  dACjp = 0.0;

  /* Default return value */
  ActLev = 0.0;

  Mln = MdB * log(10.0) / 10.0;		/* log (m), where m = 10^(M/10) */

  /* Find the solution to Aln - Cln = Mln */
  /* Calculations are carried out in natural logs */
  for (j = 0; j < NTHR; ++j) {
    if (a[j] == 0)		/* a[j] is a decreasing function of j */
      break;

    Alnj = log (Sx2 / a[j]);		/* Assume Sx2 > 0, since a[j] > 0 */
    Clnj = 2.0 * log (c[j]);		/* log (c[j]^2) */
    dACj = Alnj - Clnj;
    if (j == 0) {
      if (dACj <= Mln) {
	ActLev = exp (0.5 * Alnj);
	break;
      }
    }
    else if (dACj <= Mln) {
      alpha = (Mln - dACjp) / (dACj - dACjp);
      Alno = Alnjp + alpha * (Alnj - Alnjp);
      ActLev = exp (0.5 * Alno);
      break;
    }
    Alnjp = Alnj;
    dACjp = dACj;
  }

  return ActLev;
}
