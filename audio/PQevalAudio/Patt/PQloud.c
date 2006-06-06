/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  double PQloud (const double Ehs[], const struct Par_Loud *Loud)

Purpose:
  Calculate the total loudness for a frame

Description:
  This routine calculates the total loudness of a signal for a frame of data.

Parameters:
  <-  double PQloud
      Total loudness
   -> const double Ehs[]
      Excitation patterns
   -> const struct Par_Loud *Loud
      Structure with loudness tables

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.8 $  $Date: 2003/05/13 01:15:35 $

-------------------------------------------------------------------------*/

#include <math.h>

#include "PQevalAudio.h"

#define MAXV(a, b)	(((a) > (b)) ? (a) : (b))


double
PQloud (const double Ehs[], const struct Par_Loud *Loud)

{
  int m;
  double sN, Nm, Ntot;
  static const double e = 0.23;

  const int Nc = Loud->Nc;
  const double *s = Loud->s;
  const double *Et = Loud->Et;
  const double *Ets = Loud->Ets;

  sN = 0;
  for (m = 0; m < Nc; ++m) {
    Nm = Ets[m] * (pow (1 - s[m] + s[m] * Ehs[m] / Et[m], e) - 1);
    sN += MAXV (Nm, 0);
  }

  Ntot = (24. / Nc) * sN;

  return Ntot;
}
