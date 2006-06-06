/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  struct MOVB_MDiff PQmovModDiffB (const double *M[2], const double ERavg[],
                                   const struct Par_MDiff *MDiff)
  void PQavgModDiffB (int Ndel, const struct MOVC_MDiff *MDiff, int Nchan,
                      int Np, double *WinModDiff1B, double *AvgModDiff1B,
	              double *AvgModDiff2B)

Purpose:
  Calculate modulation difference MOV precursors (PEAQ Basic version)
  Average modulation differences (PEAQ Basic version)

Description:
  PQmovModDiffB:
  This routine calculates the modulation difference MOV precursors.

  PQavgModDiffB:
  This routine averages the modulation difference values to give the
  final MOV values.

Parameters:
  PQmovModDiffB:
  <-  struct MOVB_MDiff PQmovModB
      Output modulation differene MOV precursors
   -> const double *M[2]
      Modulation patterns
   -> const double ERavg[]
      Modified excitation patterns
   -> const struct Par_MDiff *MDiff
      Modulation difference processing parameters

  PQavgModDiffB:
   -> int Ndel
      Number of PEAQ frames to skip at the start
   -> const struct MOVC_MDiff *MDiff
      Modulation difference values
   -> int Nchan
      Number of channels
   -> int Np
      Number of PEAQ frames
  <-  double *WinModDiff1B
      Model output variable
  <-  double *AvgModDiff1B
      Model output variable
  <-  double *AvgModDiff2B
      Model output variable

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.5 $  $Date: 2003/05/13 01:15:12 $

-------------------------------------------------------------------------*/

#include <math.h>

#include "PQevalAudio.h"

static double
PQ_WinAvg (int L, const double x[], int N);
static double
PQ_WtAvg (const double x[], const double W[], int N);


struct MOVB_MDiff
PQmovModDiffB (const double *M[2], const double ERavg[],
	       const struct Par_MDiff *MDiff)

{
  int m;
  double s1B, s2B, Wt, num1B, num2B, MD1B, MD2B;
  struct MOVB_MDiff MDiffV;

  const int Nc = MDiff->Nc;
  const double *Ete = MDiff->Ete;

  /* Parameters */
  static const double negWt2B = 0.1;
  static const double offset1B = 1.0;
  static const double offset2B = 0.01;
  static const double levWt = 100;

/* Mt1B - Modulation Difference 1B
   Mt2B - Modulation Difference 2B
   Wt   - Weighting value
*/
  s1B = 0;
  s2B = 0;
  Wt = 0;
  for (m = 0; m < Nc; ++m) {
    if (M[0][m] > M[1][m]) {
      num1B = M[0][m] - M[1][m];
      num2B = negWt2B * num1B;
    }
    else {
      num1B = M[1][m] - M[0][m];
      num2B = num1B;
    }
    MD1B = num1B / (offset1B + M[0][m]);
    MD2B = num2B / (offset2B + M[0][m]);
    s1B += MD1B;
    s2B += MD2B;
    Wt += ERavg[m] / (ERavg[m] + levWt * Ete[m]);
  }

  MDiffV.Mt1B = (100. / Nc) * s1B;
  MDiffV.Mt2B = (100. / Nc) * s2B;
  MDiffV.Wt = Wt;

  return MDiffV;
}

void
PQavgModDiffB (int Ndel, const struct MOVC_MDiff *MDiff, int Nchan, int Np,
	       double *WinModDiff1B, double *AvgModDiff1B,
	       double *AvgModDiff2B)

{
  int j, L;
  double s;

  const double tavg = 0.1;
  const double Fss = PQ_FS / PQ_CB_ADV;

  /* Windowed average - delayed average */
  L = (int) floor (tavg * Fss);		/* 100 ms sliding window length */
  s = 0;
  for (j = 0; j < Nchan; ++j)
    s += PQ_WinAvg (L, &MDiff->Mt1B[j][Ndel], Np-Ndel);
  *WinModDiff1B = s / Nchan;

  /* Weighted linear average - delayed average */
  s = 0;
  for (j = 0; j < Nchan; ++j)
    s += PQ_WtAvg (&MDiff->Mt1B[j][Ndel], &MDiff->Wt[j][Ndel], Np-Ndel);
  *AvgModDiff1B = s / Nchan;

  /* Weighted linear average - delayed average */
  s = 0;
  for (j = 0; j < Nchan; ++j)
    s += PQ_WtAvg (&MDiff->Mt2B[j][Ndel], &MDiff->Wt[j][Ndel], Np-Ndel);
  *AvgModDiff2B = s / Nchan;

  return;
}

/* Sliding window (L samples) average */

static double
PQ_WinAvg (int L, const double x[], int N)
{
  int i, m;
  double s, t;

  s = 0;
  for (i = L-1; i < N; ++i) {
    t = 0;
    for (m = 0; m < L; ++m)
      t += sqrt (x[i-m]);
    s += pow (t / L, 4);
  }

  if (N >= L)
    s = sqrt (s / (N - L + 1));

  return s;
}

/* Weighted average */

static double
PQ_WtAvg (const double x[], const double W[], int N)

{
  int i;
  double s;
  double sW;

  s = 0;
  sW = 0;
  for (i = 0; i < N; ++i) {
    s += W[i] * x[i];
    sW += W[i];
  }

  if (N > 0)
    s = s / sW;

  return s;
}
