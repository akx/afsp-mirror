/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  double PQmovEHS (const double *X2[], const struct Par_EHS *EHS)
  double PQavgEHS (const struct MOVC_EHS *EHS, int Nchan, int Np)

Purpose:
  Calculate the error harmonic structure parameter
  Average the error harmonic structure values

Description:
  PQmovEHS:
  This routine calculates the error harmonic structure parameter for a frame
  of data.

  PQavbEHS:
  Average the per frame EHS values to give the final model output variable.

Parameters:
  PQmovEHS:
  <-  double PQmovEHS
      Maximum value of the correlation power spectrum
   -> const double xR[]
      Reference time samples
   -> const double xT[]
      Test time samples 
   -> const double *X2[]
      DFT energies
   -> const struct Par_EHS *EHS
      Structure with EHS parameters

  PQavgEHS:
  <-  double PQavgEHS
      EHS model output variable
   -> const struct MOVC_EHS *EHS
      Input EHS values
   -> int Nchan
      Number of channels
   -> int Np
      Number of PEAQ frames

Author / revision:
  P. Kabal  Copyright (C) 2009
  $Revision: 1.12 $  $Date: 2009/03/23 15:52:04 $

-------------------------------------------------------------------------*/

#include <assert.h>
#include <math.h>

#include <libtsp.h>
#include "PQevalAudio.h"

#define SQRV(x)		((x) * (x))
#define NLAG		256
#define NSUM		256
#define ND		(NLAG + NSUM - 1)
#define SUMSQ(x,N)	VRdDotProd(x,x,N)

static void
PQ_Corr (const double D0[], const double D1[], double C[], int NL, int M);
static double
PQ_FindPeak (const double c2[], int N);
static double
PQ_LinPosAvg (const double x[], int N);
static void
PQ_NCorr (const double C[], const double D[], double Cn[], int NL, int M);


double
PQmovEHS (const double xR[], const double xT[], const double *X2[],
	  const struct Par_EHS *EHS)

{
  int k, L;
  double D[ND];
  double C[NLAG];
  double Cw[NLAG];
  double c2[NLAG/2 + 1];
  double EnRef, EnTest;
  double EHSV;

  const int NL = EHS->NL;
  const int M = EHS->M;
  const double *Hw = EHS->Hw;

  /* Energy in the second half frame */
  EnRef  = SUMSQ (&xR[PQ_CB_ADV], PQ_NF - PQ_CB_ADV);
  EnTest = SUMSQ (&xT[PQ_CB_ADV], PQ_NF - PQ_CB_ADV);

  /* Set the return value to be negative for small energy frames */
  if (EnRef < EHS->EnThr && EnTest < EHS->EnThr)
    return -1.;

  L = NL + M - 1;
  assert (ND >= L && NLAG >= NL);

  /* Differences of log values */
  for (k = 0; k < L; ++k)
    D[k] = log (X2[1][k] / X2[0][k]);

  /* Compute correlation */
  PQ_Corr (D, D, C, NL, M);

  /* Normalize the correlations */
  PQ_NCorr (C, D, C, NL, M);

  /* Window the correlation */
  VRdMult (C, Hw, Cw, NL);

  /* DFT (scaling factor 1/NL included in window) */
  SPdRFFT (Cw, NL, 1);		/* in-place computation */

  /* Squared magnitude */
  VRdRFFTMSq (Cw, c2, NL);
  c2[0] = 0;		/* Remove mean (windowed) correlation */

  /* Search for a peak after a valley */
  EHSV = PQ_FindPeak (c2, NL/2+1);

  return EHSV;
}

/* Average EHS values (negative values flag low energy frames) */

double
PQavgEHS (const struct MOVC_EHS *EHS, int Nchan, int Np)

{
  int j;
  double s, EHSB;

  s = 0;
  for (j = 0; j < Nchan; ++j)
    s += PQ_LinPosAvg (EHS->EHS[j], Np);
  EHSB = 1000 * s / Nchan;

  return EHSB;
}
      

/* Correlation calculation */

#define NFFT 512


static void
PQ_Corr (const double D0[], const double D1[], double C[], int NL, int M)

{
  int n, m;
  double D0x[NFFT];
  double D1x[NFFT];
  double dx[NFFT];

  assert (NFFT >= NL + M - 1);

  /* Calculate the correlation using DFT's */
  VRdCopy (D0, D0x, M);
  VRdZero (&D0x[M], NFFT-M);	/* Zero padding */
  VRdCopy (D1, D1x, M+NL-1);
  VRdZero (&D1x[M+NL-1], NFFT-(M+NL-1));

  /* DFTs of the zero-padded sequences */
  SPdRFFT (D0x, NFFT, 1);
  SPdRFFT (D1x, NFFT, 1);

  /* Multiply the (complex) DFT sequences (D0x is conjugated) */
  dx[0] = D0x[0] * D1x[0];
  for (n = 1; n < NFFT/2; ++n) {
    m = NFFT/2 + n;	/* n => real part, m => imaginary part */
    dx[n] = D0x[n] * D1x[n] + D0x[m] * D1x[m];
    dx[m] = D0x[n] * D1x[m] - D0x[m] * D1x[n];
  }
  dx[NFFT/2] = D0x[NFFT/2] * D1x[NFFT/2];

  /* Inverse DFT */
  SPdRFFT (dx, NFFT, -1);

  /* Return the first NL elements */
  VRdCopy (dx, C, NL);

  return;
}

/* Normalize the correlation values */

static void
PQ_NCorr (const double C[], const double D[], double Cn[], int NL, int M)

{
  int i;
  double s0, sj, d;

  s0 = C[0];
  sj = s0;
  Cn[0] = 1;

  for (i = 1; i < NL; ++i) {
    sj += (SQRV (D[i+M-1]) - SQRV (D[i-1]));
    d = s0 * sj;
    if (d <= 0)
      Cn[i] = 1;
    else
      Cn[i] = C[i] / sqrt (d);
  }

  return;
}

/* Find the peak of the correlation power spectrum */

static double
PQ_FindPeak (const double c2[], int N)

{
  int n, Valley;
  double cprev, EHS, ct;

  assert (N >= 2);

  cprev = c2[0];
  Valley = 0;
  EHS = 0;

  /* Search for a peak after a valley */
  for (n = 1; n < N; ++n) {
    ct = c2[n];
    if (Valley) {
      if (ct > cprev && ct > EHS)	/* Rising */
	EHS = ct;
    }
    else if (ct < cprev)		/* Falling into a valley */
      Valley = 1;

    cprev = ct;
  }

  return EHS;
}

/* Average values, omitting values which are negative */

static double
PQ_LinPosAvg (const double x[], int N)

{
  int i, Nv;
  double s;

  Nv = 0;
  s = 0;
  for (i = 0; i < N; ++i) {
    if (x[i] >= 0) {
      s += x[i];
      ++Nv;
    }
  }

  if (Nv > 0)
    s = s / Nv;

  return s;
}
