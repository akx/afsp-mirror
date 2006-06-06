/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void PQframeMOVB (const struct PQ_MOVBI MOVI[], int Nchan, int i,
                    struct PQ_MOVBC *MOVC)
  void PQavgMOVB (const struct PQ_MOVBC *MOVC, int Nchan, int Nwup, int Np,
                  double MOV[])

Purpose:
  Move frame MOV values to a structure storing the time history
  Time average to produce the final MOV's

Description:
  PQframeMOV:
  This routine takes a frame-based structure of MOV precursors and moves them
  to a new structure which is more amenable to time averaging.
  PQavgMOVB:
  This routine averages the MOV precursors to generate the final MOV's

Parameters:
  PQframeMOVB:
   -> const struct PQ_MOVBI MOVI[]
      Input structure with the MOV precursors for a single frame
   -> int Nchan
      Number of channels
   -> int i
      Current frame number
  <-> struct PQ_MOVBC *MOVC
      Updated output structure

  PQavgMOVB:
   -> const struct PQ_MOVBC *MOVC
      MOV precursors
   -> int Nchan
      Number of channels
   -> int Nwup
      Number of warmup frames processed. This value is used to determine if
      additional frames need to be skipped before averaging the modulation
      difference and noise loudness values. 
   -> int Np
      Number of PEAQ frames
  <-  double MOV[]
      Output MOV's

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.6 $  $Date: 2003/05/13 01:15:12 $

-------------------------------------------------------------------------*/

#include <math.h>

#include "PQevalAudio.h"

#define MAXV(a, b)	(((a) > (b)) ? (a) : (b))
#define MINV(a, b)	(((a) < (b)) ? (a) : (b))


void
PQframeMOVB (const struct PQ_MOVBI MOVI[], int Nchan, int i,
	     struct PQ_MOVBC *MOVC)

{
  int j;

  for (j = 0; j < Nchan; ++j) {

    /* Modulation differences */
    MOVC->MDiff.Mt1B[j][i] = MOVI[j].MDiffV.Mt1B;
    MOVC->MDiff.Mt2B[j][i] = MOVI[j].MDiffV.Mt2B;
    MOVC->MDiff.Wt[j][i]   = MOVI[j].MDiffV.Wt;

    /* Noise loudness */
    MOVC->NLoud.NL[j][i] = MOVI[j].NLoudV.NL;

    /* Total loudness */
    MOVC->Loud.NRef[j][i]  = MOVI[j].LoudV.NRef;
    MOVC->Loud.NTest[j][i] = MOVI[j].LoudV.NTest;

    /* Bandwidth */
    MOVC->BW.BWRef[j][i]  = MOVI[j].BWV.BWRef;
    MOVC->BW.BWTest[j][i] = MOVI[j].BWV.BWTest;

    /* Noise-to-mask ratio */
    MOVC->NMR.NMRavg[j][i] = MOVI[j].NMRV.NMRavg;
    MOVC->NMR.NMRmax[j][i] = MOVI[j].NMRV.NMRmax;

    /* Error harmonic structure */
    MOVC->EHS.EHS[j][i] = MOVI[j].EHSV.EHS;
  }

  /* Probability of detection (collapse frequency bands) */
  PQchanPD (PQ_NC_B, Nchan, MOVI, &MOVC->PD.Pc[i], &MOVC->PD.Qc[i]);

  return;
}

void
PQavgMOVB (const struct PQ_MOVBC *MOVC, int Nchan, int Nwup, int Np,
	   double MOV[])

{
  int N500ms, N50ms, Nloud, Ndel;
  const double tdel = 0.5;
  const double tex = 0.050;
  const double Fss = PQ_FS / PQ_CB_ADV;

  /* BandwidthRefB, BandwidthTestB */
  PQavgBW (&MOVC->BW, Nchan, Np, &MOV[0], &MOV[1]);

  /* Total NMRB, RelDistFramesB */
  PQavgNMRB (&MOVC->NMR, Nchan, Np, &MOV[2], &MOV[10]);

  /* WinModDiff1B, AvgModDiff1B, AvgModDiff2B */
  N500ms = (int) ceil (tdel * Fss);	/* 0.5 sec delay */
  Ndel = MAXV (0, N500ms - Nwup);
  PQavgModDiffB (Ndel, &MOVC->MDiff, Nchan, Np, &MOV[3], &MOV[6], &MOV[7]);

  /* RmsNoiseLoudB */
  N50ms = (int) ceil (tex * Fss);	/* 50 ms delay */
  Nloud = PQloudTest (&MOVC->Loud, Nchan, Np);
  Ndel = MAXV (Nloud + N50ms, Ndel);
  MOV[8] = PQavgNLoud (Ndel, &MOVC->NLoud, Nchan, Np);

  /* ADBB, MFPDB */
  PQavgPD (&MOVC->PD, Np, &MOV[4], &MOV[9]);

  /* EHSB */
  MOV[5] = PQavgEHS (&MOVC->EHS, Nchan, Np);

  return;
}
