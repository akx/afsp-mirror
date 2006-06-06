/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void PQinitFMem (int Nc, double PCinit, struct PQ_FiltMem *FiltMem)

Purpose:
  Initialize filter memories

Description:
  This routine initializes filter memories.

Parameters:
   -> int Nc
      Number of bands
   -> double PCinit
      Initial value of pattern correction factors
  <-> struct PQ_FiltMem *FiltMem
      Filter memory structure

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.6 $  $Date: 2003/05/13 01:12:05 $

-------------------------------------------------------------------------*/

#include "PQevalAudio.h"


void
PQinitFMem (int Nc, double PCinit, struct PQ_FiltMem *Fmem)

{
  double *x, *xx;

  /* Allocate a buffer */
  x = (double *) UTmalloc (14 * Nc * sizeof (double));
  xx = x;

  /* Set up the individual buffers */
  Fmem->TDS.x[0] = xx;
  Fmem->TDS.x[1] = &xx[Nc];
  xx = xx + 2 * Nc;

  Fmem->Adap.P[0]  = &xx[0];
  Fmem->Adap.P[1]  = &xx[Nc];
  Fmem->Adap.PC[0] = &xx[2*Nc];
  Fmem->Adap.PC[1] = &xx[3*Nc];
  Fmem->Adap.Rn    = &xx[4*Nc];
  Fmem->Adap.Rd    = &xx[5*Nc];
  xx = xx + 6 * Nc;

  Fmem->Env.DE[0]   = &xx[0];
  Fmem->Env.DE[1]   = &xx[Nc];
  Fmem->Env.Ese[0]  = &xx[2*Nc];
  Fmem->Env.Ese[1]  = &xx[3*Nc];
  Fmem->Env.Eavg[0] = &xx[4*Nc];
  Fmem->Env.Eavg[1] = &xx[5*Nc];

  /* Initialize the filter memories */
  VRdZero (Fmem->TDS.x[0], Nc);
  VRdZero (Fmem->TDS.x[1], Nc);

  VRdZero (Fmem->Adap.P[0], Nc);
  VRdZero (Fmem->Adap.P[1], Nc);
  VRdSet (PCinit, Fmem->Adap.PC[0], Nc);
  VRdSet (PCinit, Fmem->Adap.PC[1], Nc);
  VRdZero (Fmem->Adap.Rn, Nc);
  VRdZero (Fmem->Adap.Rd, Nc);

  VRdZero (Fmem->Env.DE[0], Nc);
  VRdZero (Fmem->Env.DE[1], Nc);
  VRdZero (Fmem->Env.Ese[0], Nc);
  VRdZero (Fmem->Env.Ese[1], Nc);
  VRdZero (Fmem->Env.Eavg[0], Nc);
  VRdZero (Fmem->Env.Eavg[1], Nc);

  return;
}
