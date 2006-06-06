/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void PQadapt (const double *Ehs[2], const struct Par_Patt *Patt,
                double *EP[2], struct Mem_Adap *Adap) 

Purpose:
  Pattern processing

Description:
  This routine implements the pattern processing step in the PEAQ algorithm.

Parameters:
   -> const double *Ehs[2]
      Input Excitation patterns
   -> const struct Par_Patt *Patt
      Struct with processing parameters
  <-  double *EP[2]
      Ouptut spectrally adapted patterns
  <-> struct Mem_Adap *Adap
      Filter memory structure

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.8 $  $Date: 2003/05/13 01:15:35 $

-------------------------------------------------------------------------*/

#include <assert.h>
#include <math.h>

#include "PQevalAudio.h"

#define MAXV(a, b)	(((a) > (b)) ? (a) : (b))
#define MINV(a, b)	(((a) < (b)) ? (a) : (b))
#define SQRV(x)		((x) * (x))


void
PQadapt (const double *Ehs[2], const struct Par_Patt *Patt,
	 double *EP[2], struct Mem_Adap *Adap)

{
  int m, i, iL, iU, M1, M2;
  double sn, sd, s1, s2, CL;
  double R[2][PQ_MAXNC];

  double **P = Adap->P;
  double **PC = Adap->PC;
  double *Rn = Adap->Rn;
  double *Rd = Adap->Rd;

  const int Nc = Patt->Nc;
  const double *a = Patt->a;
  const double *b = Patt->b;

  /* Smooth the excitation patterns */
  /* Calculate the correlation terms */
  sn = 0;
  sd = 0;
  for (m = 0; m < Nc; ++m) {
    P[0][m] = a[m] * P[0][m] + b[m] * Ehs[0][m];
    P[1][m] = a[m] * P[1][m] + b[m] * Ehs[1][m];
    sn += sqrt (P[1][m] * P[0][m]);
    sd += P[1][m];
  }

  /* Level correlation */
  CL = SQRV (sn / sd);

  for (m = 0; m < Nc; ++m) {

    /* Scale one of the signals to match levels */
    if (CL > 1) {
      EP[0][m] = Ehs[0][m] / CL;
      EP[1][m] = Ehs[1][m];
    }
    else {
      EP[0][m] = Ehs[0][m];
      EP[1][m] = Ehs[1][m] * CL;
    }

    /* Calculate a pattern match correction factor */
    Rn[m] = a[m] * Rn[m] + EP[1][m] * EP[0][m];
    Rd[m] = a[m] * Rd[m] + EP[0][m] * EP[0][m];
    assert (Rd[m] > 0 && Rn[m] > 0);
    if (Rn[m] >= Rd[m]) {
      R[0][m] = 1;
      R[1][m] = Rd[m] / Rn[m];
    }
    else {
      R[0][m] = Rn[m] / Rd[m];
      R[1][m] = 1;
    }
  }

  /* Average the correction factors over M channels and smooth with time */
  M1 = Patt->M1;
  M2 = Patt->M2;
  for (m = 0; m < Nc; ++m) {
    iL = MAXV (m - M1, 0);
    iU = MINV (m + M2, Nc-1);
    s1 = 0;
    s2 = 0;
    for (i = iL; i <= iU; ++i) {
      s1 += R[0][i];
      s2 += R[1][i];
    }
    PC[0][m] = a[m] * PC[0][m] + b[m] * s1 / (iU-iL+1);
    PC[1][m] = a[m] * PC[1][m] + b[m] * s2 / (iU-iL+1);

    /* Final correction factor => spectrally adapted patterns */
    EP[0][m] = EP[0][m] * PC[0][m];
    EP[1][m] = EP[1][m] * PC[1][m];
  }

  return;
}
