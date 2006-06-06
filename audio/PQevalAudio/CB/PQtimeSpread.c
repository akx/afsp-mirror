/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void PQtimeSpread (const double Es[], const struct Par_TDS *TDS,
                     double Ehs[], double Ef[])

Purpose:
  Time spread excitation patterns

Description:
  This routine spreads excitation patterns in time.

Parameters:
   -> double Es[]
      Input energy values
   -> const struct PQ_TDS *TDS
      Time domain spreading parameters
  <-  double Ehs[]
      Output time spread energy values
  <-> double Ef[]
      Filter memories

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.6 $  $Date: 2003/05/13 01:11:27 $

-------------------------------------------------------------------------*/

#include "PQevalAudio.h"

#define MAXV(a, b)	(((a) > (b)) ? (a) : (b))


void
PQtimeSpread (const double Es[], const struct Par_TDS *TDS, double Ehs[],
	      double Ef[])

{
  int m;
  const int Nc = TDS->Nc;
  const double *a = TDS->a;
  const double *b = TDS->b;

  for (m = 0; m < Nc; ++m) {
    Ef[m] = a[m] * Ef[m] + b[m] * Es[m];
    Ehs[m] = MAXV (Ef[m], Es[m]);
  }

  return;
}
