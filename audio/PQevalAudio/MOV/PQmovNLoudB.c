/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  double PQmovNLoudB (const double *M[2], const double *EP[2],
                      const struct Par_NLoud *NLoud);
  double PQavgNLoud (int Ndel, const struct MOVC_Mod *Mod, int Nchan, int Np)

Purpose:
  Calculate the noise loudness MOV precursor (PEAQ Basic version)
  Calculate the average noise loudness (PEAQ Basic Version)

Description:
  PQmovNLoudB:
  This routine calculates the modulation difference MOV precursors.

  PQavgNLoudB:
  This rouitne averages the modulation difference values.

Parameters:
  PQmovNLoudB:
  <-  double PQmovNLoudB
      Output noise loudness MOV precursor
   -> const double *M[2]
      Modulation patterns
   -> const double *EP[2]
      Excitation patterns
   -> const struct Par_NLoud *NLoud
      Modulation difference processing parameters

  PQavgNLoudB:
  <-  double PQavgNLoudB
      Model output variable
   -> int Ndel
      Number of PEAQ frames to skip at the start
   -> const struct MOVC_NLoud *NLoud
      Noise loudness values
   -> int Nchan
      Number of channels
   -> int Np
      Number of PEAQ frames

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.4 $  $Date: 2003/05/13 01:15:12 $

-------------------------------------------------------------------------*/

#include <math.h>

#include "PQevalAudio.h"

#define MAXV(a, b)	(((a) > (b)) ? (a) : (b))
#define SQRV(x)		((x) * (x))

static double
PQ_RMSAvg (const double x[], int N);


double
PQmovNLoudB (const double *M[2], const double *EP[2],
	     const struct Par_NLoud *NLoud)

{
  int m;
  double s, sref, stest, beta, a, b, NL;

  const int Nc = NLoud->Nc;
  const double *Et = NLoud->Et;

  static const double alpha = 1.5;
  static const double TF0 = 0.15;
  static const double S0 = 0.5;
  static const double NLmin = 0;
  static const double e = 0.23;

  s = 0;
  for (m = 0; m < Nc; ++m) {
    sref  = TF0 * M[0][m] + S0;
    stest = TF0 * M[1][m] + S0;
    beta = exp (-alpha * (EP[1][m] - EP[0][m]) / EP[0][m]);
    a = MAXV (0, stest * EP[1][m] - sref * EP[0][m]);
    b = Et[m] + sref * EP[0][m] * beta;
    s += pow (Et[m] / stest, e) * (pow (1 + a / b, e) - 1);
  }

  NL = (24. / Nc) * s;
  if (NL < NLmin)
    NL = 0;

  return NL;
}

/* Average noise loudness values */

double
PQavgNLoud (int Ndel, const struct MOVC_NLoud *Nloud, int Nchan, int Np)

{
  int j;
  double s, RmsNoiseLoudB;

  /* RMS average - delayed average and loudness threshold */
  s = 0;
  for (j = 0; j < Nchan; ++j)
    s += PQ_RMSAvg (&Nloud->NL[j][Ndel], Np-Ndel);
  RmsNoiseLoudB = s / Nchan;

  return RmsNoiseLoudB;
}

/* Square root of average of squared values */

static double
PQ_RMSAvg (const double x[], int N)

{
  int i;
  double s;

  s = 0;
  for (i = 0; i < N; ++i)
    s += SQRV(x[i]);

  if (N > 0)
    s = sqrt (s / N);

  return s;
}
