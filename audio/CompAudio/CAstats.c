/*------------- Telecommunications & Signal Processing Lab --------------
                          McGill University
Routine:
  void CAstats (AFILE *AFp, long int StartF, long int Nsamp,
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
      Audio file pointer for an audio file opened by AFopenRead
   -> long int StartF
      Starting frame number
   -> long int Nsamp
      Number of samples.  If Nframe == AF_NSAMP_UNDEF, the end-of-file is
      used.
  <-  struct Stats_F Stats[]
      Structures containing the file statistics
   -> int Nchan
      Number of channels

Author / revision:
  P. Kabal  Copyright (C) 2001
  $Revision: 1.25 $  $Date: 2001/11/10 05:07:27 $

-----------------------------------------------------------------------*/

static char rcsid[] = "$Id: CAstats.c 1.25 2001/11/10 AFsp-v6r8 $";

#include <limits.h>
#include <math.h>

#include <libtsp.h>
#include "CompAudio.h"

#define MINV(a, b)	(((a) < (b)) ? (a) : (b))
#define MAXV(a, b)	(((a) > (b)) ? (a) : (b))
#define ABSV(x)		(((x) < 0) ? -(x) : (x))
#define ICEILV(n, m)	(((n) + ((m) - 1)) / (m))	/* int n,m >= 0 */

#define NBUF	2560
#define MDB	15.9		/* Active speech offset (dB) */

static int
CA_INpar (struct SpAct_P *SpPar, double Sfreq);
static void
CA_INcount (struct SpAct_S *SpAct);
static void
CA_stats (struct Stats_F *Stats, const float x[], int N, int Nsub);
static void
CA_SAcount (struct SpAct_S *SpAct, const struct SpAct_P *SpPar,
	    const float x[], int N, int Nsub);
static double
CA_ActLevel (const long int a[], const float c[], double Sx2, double MdB);


void
CAstats (AFILE *AFp, long int Start, long int Nsamp, struct Stats_F Stats[],
	 int Nchan)

{
  float X[NBUF];
  int eof, i, is, it, iss, itt, Nv, Nr, Nsub;
  long int ioffs, Ns, Nrem;
  struct SpAct_P SpPar;
  struct SpAct_S *SpAct;
  double Sxx;

  /* Set up the speech activity measurement parameters */
  Nsub = CA_INpar (&SpPar, (double) AFp->Sfreq);

  /* Allocate storage for the speech activity variables structure */
  SpAct = (struct SpAct_S *) UTmalloc (Nchan * (sizeof (struct SpAct_S)));

  for (i = 0; i < Nchan; ++i) {

    /* Initialize the statistics variables */
    Stats_F_INIT (&Stats[i]);

    /* Initialize the speech activity variables */
    CA_INcount (&SpAct[i]);
  }

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
    Nr = AFreadData (AFp, ioffs, X, Nv);
    if (eof && Nr < Nv) {
      Nrem = Nr;
      Nv = Nr;
    }
    ioffs += Nv;
    Nrem -= Nv;

    for (i = 0; i < Nchan; ++i) {

      /* Gather statistics */
      itt = (it + i) % Nchan;
      CA_stats (&Stats[i], &X[itt], Nv - itt, Nchan);

      /* Update the speech activity counts */
      iss = (is + i) % (Nsub*Nchan);
      CA_SAcount (&SpAct[i], &SpPar, &X[iss], Nv - iss, Nsub*Nchan);
    }
    it = it + Nchan * ICEILV (Nv - it, Nchan) - Nv;
    is = is + (Nsub*Nchan) * ICEILV (Nv - is, Nsub*Nchan) - Nv;
  }

/* Calculate the active speech level */
  for (i = 0; i < Nchan; ++i) {
    Ns = ICEILV (Stats[i].N, Nsub);	/* Number of subsampled values */
    if (Stats[i].N > 0)
      Sxx = Ns * Stats[i].Sx2 / Stats[i].N;
    else
      Sxx = 0.0;
    Stats[i].ActLev = CA_ActLevel (SpAct[i].a, SpPar.c, Sxx, MDB);
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


static int
CA_INpar (struct SpAct_P *SpPar, double Sfreq)

{
  int i, Nsub;

  Nsub = (int) (Sfreq / EFREQ);
  if (Nsub <= 0)
    Nsub = 1;

  SpPar->I = (long int) ceil (HSEC * Sfreq / Nsub);
  SpPar->g = (float) exp (-Nsub / (Sfreq * T));

  for (i = 0; i < NTHR; ++i)
    SpPar->c[i] = (float) pow (2.0, (double) i);

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
  SpAct->p = 0.0F;
  SpAct->q = 0.0F;

  return;
}

/* Accumulate statistics for one buffer of data */

#define VMAX		Stats->Vmax
#define VMIN		Stats->Vmin
#define INRANGE		Stats->Inrange
#define NRUN		Stats->Nrun
#define NOVLOAD		Stats->Novload
#define NANOMAL		Stats->Nanomal

static const float Amax = 32767.F;
static const float Amin = -32768.F;
static const float Aup = 16383.F;
static const float Alw = -16384.F;

static void
CA_stats (struct Stats_F *Stats, const float x[], int N, int Nsub)

{
  double Sx, Sx2;
  int i;
  
  Sx = 0.0;
  Sx2 = 0.0;
  Stats->N += ICEILV (N, Nsub);
  for (i = 0; i < N; i += Nsub) {

    /* Min and Max values */
    VMAX = MAXV (VMAX, x[i]);
    VMIN = MINV (VMIN, x[i]);

    /* Detect overloads and anomalous transitions */
    if (x[i] > Aup) {
      if (x[i] >= Amax) {
	++NOVLOAD;
	if (INRANGE != 2)
	  ++NRUN;
	INRANGE = 2;
      }
      else {
	if (INRANGE == -1)
	  ++NANOMAL;
	INRANGE = 1;
      }
    }
    else if (x[i] < Alw) {
      if (x[i] <= Amin) {
	++NOVLOAD;
	if (INRANGE != -2)
	  ++NRUN;
	INRANGE = -2;
      }
      else {
	if (INRANGE == 1)
	  ++NANOMAL;
	INRANGE = -1;
      }
    }
    else {
      INRANGE = 0;
    }

    /* Accumulate the double sum and sum of products */
    Sx += x[i];
    Sx2 += (double) x[i] * (double) x[i];
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
	    const float x[], int N, int Nsub)

{
  int i, j;

  /* Speech activity calculations */
  for (i = 0; i < N; i += Nsub) {

    /* Smoothed envelope */
    P = G * P + (1.0F-G) * ABSV (x[i]);
    Q = G * Q + (1.0F-G) * P;

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
   -> float c[]
      Threshold levels (NTHR increasing values)
   -> double Sx2
      Sum of squares for the input signal
   -> double MdB
      Offset in dB
*/


static double
CA_ActLevel (const long int a[], const float c[], double Sx2, double MdB)

{
  double ActLev, Mln, Alnj, Clnj, dACj, alpha, Alno, Alnjp, dACjp;
  int j;

  /* Default return value */
  ActLev = 0.0;

  Mln = MdB * log(10.0) / 10.0;		/* log (m), where m = 10^(M/10) */

  /* Find the solution to Aln - Cln = Mln */
  /* Calculations are carried out in natural logs */
  for (j = 0; j < NTHR; ++j) {
    if (a[j] == 0)
      break;

    Alnj = log (Sx2 / a[j]);
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
