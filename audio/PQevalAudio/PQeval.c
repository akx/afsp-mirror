/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  struct PQ_MOVBI PQeval (const double xR[], const double xT[],
                          const struct PQ_Par *PQpar, struct PQ_FiltMem *Fmem)

Purpose:
  Calculate PEAQ FFT model values for one frame (PEAQ Basic version)

Description:
  This routine calculates the PEAQ model output variable precursors for one
  frame (PEAQ Basic version).

Parameters:
  <-  struct PQ_MOVBI PQeval
      Model output variable precursors
   -> const double xR[]
      Reference signal
   -> const double xT[]
      Test signal
   -> const struct PQ_Par *PQpar
      Tables and parameters
  <-> struct PQ_FiltMem *Fmem
      Filter parameters and memory

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.14 $  $Date: 2003/05/13 01:10:26 $

-------------------------------------------------------------------------*/

#include <assert.h>
#include <math.h>
#include <stdio.h>

#include <libtsp.h>
#include "PQevalAudio.h"

static void
PQ_DFTFrame (const double x[], const double hw[], double X2[], int NF);
static void
PQ_excitCB (const double *X2[2], double EbN[], double *Es[2],
	    const struct Par_CB *CB);


struct PQ_MOVBI
PQeval (const double xR[], const double xT[], const struct PQ_Par *PQpar,
	struct PQ_FiltMem *Fmem)

{
  double xFFT[2 * PQ_NF];
  double *X2[2] = {&xFFT[0], &xFFT[PQ_NF]};
  double xCB[6 * PQ_NC_B];
  double *Es[2]  = {&xCB[0*PQ_NC_B], &xCB[PQ_NC_B]};
  double *Ehs[2] = {&xCB[2*PQ_NC_B], &xCB[3*PQ_NC_B]};
  double *EP[2]  = {&xCB[4*PQ_NC_B], &xCB[5*PQ_NC_B]};
  double xMod[2 * PQ_MAXNC];
  double *M[2] = {&xMod[0], &xMod[PQ_MAXNC]};
  double ERavg[PQ_MAXNC];

  double EbN[PQ_NC_B];
  struct PQ_MOVBI MOVB;

  static const int NF = PQ_NF;

  /* Windowed DFT */
  PQ_DFTFrame (xR, PQpar->FFT.hw, X2[0], NF);
  PQ_DFTFrame (xT, PQpar->FFT.hw, X2[1], NF);

  /* Critical band grouping and frequency spreading */
  PQ_excitCB ((const double **) X2, EbN, Es, &PQpar->CB);

  /* Time domain smoothing => "Excitation patterns" */
  PQtimeSpread (Es[0], &PQpar->TDS, Ehs[0], Fmem->TDS.x[0]);
  PQtimeSpread (Es[1], &PQpar->TDS, Ehs[1], Fmem->TDS.x[1]);

  /* Level and pattern adaptation => "Spectrally adapted patterns" */
  PQadapt ((const double **) Ehs, &PQpar->Patt, EP, &Fmem->Adap);

  /* Modulation patterns */
  PQmodPatt ((const double **) Es, &PQpar->MPatt, M, ERavg, &Fmem->Env);

  /* Loudness calculation */
  MOVB.LoudV.NRef  = PQloud (Ehs[0], &PQpar->Loud);
  MOVB.LoudV.NTest = PQloud (Ehs[1], &PQpar->Loud);

  /* Modulation differences */
  MOVB.MDiffV = PQmovModDiffB ((const double **) M, ERavg, &PQpar->MDiff);

  /* Noise Loudness */
  MOVB.NLoudV.NL = PQmovNLoudB ((const double **) M, (const double **) EP,
			        &PQpar->NLoud);

  /* Bandwidth */
  MOVB.BWV = PQmovBW ((const double **) X2, &PQpar->BW);

  /* Noise-to-mask ratios */
  MOVB.NMRV = PQmovNMRB (EbN, Ehs[0], &PQpar->NMR);

  /* Probability of detection */
  MOVB.PDV = PQmovPD ((const double **) Ehs);

  /* Error harmonic structure */
  MOVB.EHSV.EHS = PQmovEHS (xR, xT, (const double **) X2, &PQpar->EHS);

  return MOVB;
}
/*  Process a frame of time samples, returning a squared magnitude spectrum */

static void
PQ_DFTFrame (const double x[], const double hw[], double X2[], int NF)

{
  double X[PQ_NF];

  assert (NF == PQ_NF);

  /* Window the data */
  VRdMult (x, hw, X, NF);

  /* Real FFT */
  SPdRFFT (X, NF, +1);

  /* Squared magnitude */
  VRdRFFTMSq (X, X2, NF);

  return;
}

/* DFT values to critical band values */

static void
PQ_excitCB (const double *X2[2], double EbN[], double *Es[2],
	    const struct Par_CB *CB)

{
  int k;
  double Xw2[2][PQ_NF/2+1];
  double XwN2[PQ_NF/2+1];
  double E[2][PQ_NC_B];
  double Eb[2][PQ_NC_B];

  const int Nc = CB->Nc;
  static const int NF = PQ_NF;

  /* Outer and middle ear filtering */
  VRdMult (X2[0], CB->W2, Xw2[0], NF/2+1);
  VRdMult (X2[1], CB->W2, Xw2[1], NF/2+1);

  /* Form the difference magnitude signal */
  for (k = 0; k <= NF/2; ++k)
    XwN2[k] = Xw2[0][k] - 2 * sqrt (Xw2[0][k] * Xw2[1][k]) + Xw2[1][k];

  /* Group into partial critical bands */
  PQgroupCB (Xw2[0], CB, Eb[0]);
  PQgroupCB (Xw2[1], CB, Eb[1]);
  PQgroupCB (XwN2, CB, EbN);	/* Noise patterns */

  /* Add internal noise => "Pitch patterns" */
  VRdAdd (Eb[0], CB->EIN, E[0], Nc);
  VRdAdd (Eb[1], CB->EIN, E[1], Nc);

  /* Critical band spreading => "Unsmeared excitation patterns" */
  PQspreadCB (E[0], CB, Es[0]);
  PQspreadCB (E[1], CB, Es[1]);

  return;
}
