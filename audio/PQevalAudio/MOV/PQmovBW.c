/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  struct MOVB_BW PQmovBW (const double *X2[], const struct Par_BW *BW)
  void PQavgBW (const struct MOVC_BW *BW, int Nchan, int Np,
	        double *BandwidthRefB, double *BandwidthTestB)

Purpose:
  Calculate the bandwidth model output variable precursors
  Calculate the average bandwith

Description:
  PQmovBW:
  This routine calculates the model output variable precursors related to
  bandwidth. The bandwidth precursors are set to a negative value for frames
  which fall below a threshold value.

  PQavgBW:
  This routine averages the bandwidth model output variables.

Parameters:
  PQmovBW:
  <-  struct MOVB_BW PWmovBW
      Model output variable precursors
   -> const double *X2[2]
      DFT (squared) values
   -> const struct Par_BW *BW
      Structure with parameters for the bandwidth calculation

  PQavgBW:
   -> const struct MOVC_BW *BW
      Input bandwidth MOV precursors
   -> int Nchan
      Number of channels
   -> int Np
      Number of PEAQ frames
  <-  double *BandwidthRefB
      MOV: Average bandwidth of the reference signal (DFT bins)
  <-  double *BandwidthTestB
      MOV: Average bandwidth of the test signal (DFT bins)

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.9 $  $Date: 2003/05/13 01:15:12 $

-------------------------------------------------------------------------*/
#include "PQevalAudio.h"

#define MAXV(a, b)	(((a) > (b)) ? (a) : (b))

static double
PQ_LinPosAvg (const double x[], int N);


struct MOVB_BW
PQmovBW (const double *X2[], const struct Par_BW *BW)

{
  int k, BWRef, BWTest;
  double Xth, XthR, XthT;
  struct MOVB_BW BWV;

  const int N = BW->N;
  const int kx = BW->kx;
  const int kl = BW->kl;

  /* Find the largest component above kx */
  Xth = X2[1][kx];
  for (k = kx+1; k < N; ++k)
    Xth = MAXV (Xth, X2[1][k]);

  /* BWRef and BWTest remain negative if the BW of the test signal
     does not exceed FR * Xth for kx-1 <= k <= kl+1
  */
  BWRef = -1;
  XthR = BW->FR * Xth;
  for (k = kx-1; k > kl; --k) {
    if (X2[0][k] >= XthR) {
      BWRef = k + 1;
      break;
    }
  }

  BWTest = -1;
  XthT = BW->FT * Xth;
  for (k = BWRef-1; k >= 0; --k) {
    if (X2[1][k] >= XthT) {
      BWTest = k + 1;
      break;
    }
  }

  BWV.BWTest = BWTest;
  BWV.BWRef = BWRef;

  return BWV;
}

/* Average bandwidth values, omitting frames with negative bandwidth (flagging
   very flat energies) */

void
PQavgBW (const struct MOVC_BW *BW, int Nchan, int Np,
	 double *BandwidthRefB, double *BandwidthTestB)

{
  int j;
  double sR, sT;

  sR = 0;
  sT = 0;
  for (j = 0; j < Nchan; ++j) {
    sR += PQ_LinPosAvg (BW->BWRef[j], Np);
    sT += PQ_LinPosAvg (BW->BWTest[j], Np);
  }
  *BandwidthRefB  = sR / Nchan;
  *BandwidthTestB = sT / Nchan;

  return;
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
