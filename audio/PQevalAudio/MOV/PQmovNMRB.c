/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  struct MOVB_NMR PQmovNMRB (const double EbN[], const double Ehs[],
                             const struct Par_NMR *NMR)
  void PQavgNMRB (const struct MOVC_NMR *NMR, int Nchan, int Np,
                  double *TotalNMRB, double *RelDistFramesB)

Purpose:
  Calculate the noise-to-mask ratio (PEAQ Basic version)
  Calculate the NMR model output variables (PEAQ Basic version)

Description:
  PQmovNMRB:
  This routine calculates the average and maximum noise-to-mask ratio for a
  frame of data (PEAQ Basic Version).

  PQavgNMRB:
  This routine averages the NMR values to give the NMR model output variables.

Parameters:
  PQmovNMRB:
  <-  struct MOVB_NMR
      Output structure with NMR values
   -> const double EbN[]
      Noise patterns
   -> const double Ehs[]
      Excitation patterns
   -> const struct Par_NMR *NMR
      Structure with NMR tables

  PQavgNMRB:
   -> const struct MOVC_NMR *NMR
      Structure containing the NMR values for each PEAQ frame
   -> int Nchan
      Number of channels
   -> int Np
      Number of PEAQ frames
  <-  double *TotalNMRB
      Model Output variable
  <-  double *RelDistFramesB
      Model Output variable

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.7 $  $Date: 2003/05/13 01:15:13 $

-------------------------------------------------------------------------*/

#include <math.h>

#include "PQevalAudio.h"

#define DBP(x)		(10.0 * log10 (x))

static double
PQ_FractThr (double Thr, const double x[], int N);
static double
PQ_LinAvg (const double x[], int N);


struct MOVB_NMR
PQmovNMRB (const double EbN[], const double Ehs[], const struct Par_NMR *NMR)

{
  int m;
  double s, NMRm;
  struct MOVB_NMR NMRV;

  const int Nc = NMR->Nc;
  const double *gm = NMR->gm;

  NMRV.NMRmax = 0;
  s = 0;
  for (m = 0; m < Nc; ++m) {
    NMRm = EbN[m] / (gm[m] * Ehs[m]);
    s += NMRm;
    if (NMRm > NMRV.NMRmax)
      NMRV.NMRmax = NMRm;
  }
  NMRV.NMRavg = s / Nc;

  return NMRV;
}

/* Average NMR values */

void
PQavgNMRB (const struct MOVC_NMR *NMR, int Nchan, int Np, double *TotalNMRB,
	   double *RelDistFramesB)

{
  int j;
  double s, Thr;

  const double ThrdB = 1.5;	/* Threshold in dB */
  Thr = pow (10, ThrdB / 10);

  /* Average log values */
  s = 0;
  for (j = 0; j < Nchan; ++j)
    s += DBP (PQ_LinAvg (NMR->NMRavg[j], Np));
  *TotalNMRB = s / Nchan;

  s = 0;
  for (j = 0; j < Nchan; ++j)
    s += PQ_FractThr (Thr, NMR->NMRmax[j], Np);
  *RelDistFramesB = s / Nchan;

  return;
}

/* Fraction of values above a threshold */

static double
PQ_FractThr (double Thr, const double x[], int N)

{
  int Nv, i;
  double Fd;

  Nv = 0;
  for (i = 0; i < N; ++i) {
    if (x[i] > Thr)
      ++Nv;
  }

  if (N > 0)
    Fd = ((double) Nv) / N;
  else
    Fd = 0;

  return Fd;
}

/* Linear average */

static double
PQ_LinAvg (const double x[], int N)

{
  int i;
  double s;

  s = 0;
  for (i = 0; i < N; ++i)
    s += x[i];

  if (N > 0)
    s = s / N;

  return s;
}
