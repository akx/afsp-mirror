/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void PQmodPatt (const double *Es[2], const struct Par_MPatt *MPatt,
	          double *M[2], double ERavg[], struct Mem_Env *Env)

Purpose:
  Modulation pattern processing

Description:
  This routine generates modulation patterns.

Parameters:
   -> const double *Es[2]
      Unsmeared excitation patterns.
   -> const struct Par_MPatt *MPatt
      Modulation processing parameters
  <-  double *M[2]
      Modulation measures
  <-  double ERavg[]
      Modified excitation patterns
  <-> struct Mem_Env *Env
      Filter memory structure

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.8 $  $Date: 2003/05/13 01:15:35 $

-------------------------------------------------------------------------*/

#include <math.h>

#include "PQevalAudio.h"

#define ABSV(x)		(((x) < 0) ? -(x) : (x))


void
PQmodPatt (const double *Es[2], const struct Par_MPatt *MPatt,
	   double *M[2], double ERavg[], struct Mem_Env *Env)

{
  int m, i;
  double Ee;
  const int Nc = MPatt->Nc;
  const double *a = MPatt->a;
  const double *b = MPatt->b;
  double **DE = Env->DE;
  double **Ese = Env->Ese;
  double **Eavg = Env->Eavg;
  static const double Fss = PQ_FS / PQ_CB_ADV;
  static const double e = 0.3;

  for (i = 0; i < 2; ++i) {
    for (m = 0; m < Nc; ++m) {
      Ee = pow (Es[i][m], e);
      DE[i][m] = a[m] * DE[i][m] + b[m] * Fss * ABSV (Ee - Ese[i][m]);
      Eavg[i][m] = a[m] * Eavg[i][m] + b[m] * Ee;
      Ese[i][m] = Ee;

      M[i][m] = DE[i][m] / (1 + Eavg[i][m]/0.3);
    }
  }

  VRdCopy (Eavg[0], ERavg, Nc);

  return;
}
