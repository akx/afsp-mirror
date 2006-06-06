/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void PQgroupCB (const double X2[], const struct PQ_CB *CBpar, double Eb[])

Purpose:
  Group DFT energy values into partial critical bands

Description:
  This routine takes a vector of DFT coefficients (squared magnitudes) and
  groups them into bands.

Parameters:
   -> const double X2[]
      DFT magnitude values
   -> const struct PQ_CB *CBpar
      Mapping tables
  <-  double Eb[]
      Output energies (excitation) for the bands

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.6 $  $Date: 2003/05/13 01:11:27 $

-------------------------------------------------------------------------*/

#include "PQevalAudio.h"

#define MAXV(a, b)	(((a) > (b)) ? (a) : (b))

#define EMIN	1e-12


void
PQgroupCB (const double X2[], const struct Par_CB *CBpar, double Eb[])

{
  int i, k;
  double Ea;
  const int Nc = CBpar->Nc;
  const double *Ul = CBpar->Ul;
  const double *Uu = CBpar->Uu;
  const int *kl = CBpar->kl;
  const int *ku = CBpar->ku;

  /* Compute the excitation in each band */
  for (i = 0; i < Nc; ++i) {
    Ea = Ul[i] * X2[kl[i]];	/* First bin */

    for (k = kl[i]+1; k < ku[i]; ++k)
      Ea += X2[k];					/* Middle bins */

    Ea += Uu[i] * X2[ku[i]];	/* Last bin */
    Eb[i] = MAXV (Ea, EMIN);
  }

 return;
}
