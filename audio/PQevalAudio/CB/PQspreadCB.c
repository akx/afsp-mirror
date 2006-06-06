/*------------- Telecommunications & Signal Processing Lab -------------
                           McGill University

Routine:
  void PQspreadCB (const double E[], const struct Par_CB *CBpar, double Es[])

Purpose:
  Frequency spreading of the critical band energies

Description:
  This routine spreads the critical band energies.

Parameters:
   -> const double E[]
      Critical band energies
   -> const struct PQ_CB *CB
      Critical band parameters
  <-  double Es[]
      Output spread band energies

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.9 $  $Date: 2003/05/13 01:11:27 $

----------------------------------------------------------------------*/

#include <math.h>

#include "PQevalAudio.h"


void
PQspreadCB (const double E[], const struct Par_CB *CB, double Es[])

{
  int i, m;
  double gIL, gIU, aL, aLe, aUC, aUCE, En, r, a;
  double aUCEe[PQ_MAXNC];
  double Ene[PQ_MAXNC];
  static const double e = 0.4;	/* Power law for addition of spreading */

  const double *fc = CB->fc;
  const double *Bs = CB->Bs;
  const double dz = CB->dz;
  const int Nc = CB->Nc;

  /* Calculate energy dependent terms */
  aL = pow (10, -2.7 * dz);
  for (m = 0; m < Nc; ++m) {
    aUC = pow (10, (-2.4 - 23 / fc[m]) * dz);
    aUCE = aUC * pow (E[m], 0.2 * dz);
    gIL = (1 - pow (aL, m+1)) / (1 - aL);
    gIU = (1 - pow (aUCE, Nc-m)) / (1 - aUCE);
    En = E[m] / (gIL + gIU - 1);
    aUCEe[m] = pow (aUCE, e);
    Ene[m] = pow (En, e);
  }

  /* Lower spreading */
  Es[Nc-1] = Ene[Nc-1];
  aLe = pow (aL, e);
  for (m = Nc-2; m >= 0; --m)
    Es[m] = aLe * Es[m+1] + Ene[m];

  /* Upper spreading i > m */
  for (m = 0; m <= Nc-2; ++m) {
    r = Ene[m];
    a = aUCEe[m];
    for (i = m+1; i < Nc; ++i) {
      r = r * a;
      Es[i] += r;
    }
  }

  for (i = 0; i < Nc; ++i)
    Es[i] = pow (Es[i], 1/e) / Bs[i];

  return;
}
