/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  struct MOVB_PD PQmovPD (const double *Ehs[])
  void PQchanPD (int Nc, int Nchan, const struct PQ_MOVBI MOVBI[2],
	         double *Pc[], double *Qc)
  void PQavgPD (const struct MOVC_PD *PD, int Np,
                double *ADBB, double *MFPDB)

Purpose:
  Calculate the probability of detection values
  Average the probability of detection values 

Description:
  PQmovPD:
  This routine calculates the probability of detection for each frequency for a
  frame of data.

  PQchanPD:
  This routine compares probability of detection values across channels.

  PQavgPD:
  This routine average the probability of detection values to form the model
  output variables.

Parameters:
  PQmovPD:
  <-  struct MOVB_PD PQmovPD
      Probability of detection values
   -> const double *Ehs[]
      Excitation patterns

  PQchanPD:
   -> int Nc
      Number of bands
   -> int Nchan
      Number of channels
   -> const struct PQ_MOVBI MOVBI[2]
      Input values for each channel
  <-  double *Pc
      Output probability of detection value (max. over channels)
  <-  double *Qc
      Output total number of steps above the threshold 

  PQavgPD:
   -> const struct MOVC_PD *PD
      Probability of detection model output variable precursors
   -> int Np
      Number of PEAQ frames
  <-  double *ADBB
      Output average distorted block MOV
  <-  double *MFPDB
      Maximum filtered probability of detection MOV

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.7 $  $Date: 2003/05/13 01:15:13 $

-------------------------------------------------------------------------*/

#include <math.h>

#include "PQevalAudio.h"

#define MAXV(a, b)	(((a) > (b)) ? (a) : (b))
#define ABSV(x)		(((x) < 0) ? -(x) : (x))
#define DBP(x)		(10.0 * log10 (x))


struct MOVB_PD
PQmovPD (const double *Ehs[])

{
/* Polynomial approximation parameters */
  static const double c[] =
    {-0.198719, 0.0550197, -0.00102438, 5.05622e-6, 9.01033e-11};
  static const double d1 = 5.95072;
  static const double d2 = 6.39468;
  static const double g = 1.71332;

  /* Slope parameters */
  static const double bP = 4;
  static const double bM = 6;

  static const int Nc = PQ_NC_B;

  int m;
  double EdBR, EdBT, edB;
  double L, b, s;
  struct MOVB_PD PDV;
  
  for (m = 0; m < Nc; ++m) {
    EdBR = DBP (Ehs[0][m]);
    EdBT = DBP (Ehs[1][m]);
    edB = EdBR - EdBT;
    if (edB > 0) {
      L = 0.3 * EdBR + 0.7 * EdBT;
      b = bP;
    }
    else {
      L = EdBT;
      b = bM;
    }
    if (L > 0)
      s = d1 * pow (d2 / L, g)
	+ c[0] + L * (c[1] + L * (c[2] + L * (c[3] + L * c[4])));
    else
      s = 1e30;
    PDV.p[m] = 1 - pow (0.5, pow (edB / s, b)); /* Detection probability */
    PDV.q[m] = ABSV ((int) (edB)) / s;		/* Steps above threshold */
  }

  return PDV;
}

/* Compare prob. detection values across channels */


void
PQchanPD (int Nc, int Nchan, const struct PQ_MOVBI MOVBI[2],
	   double *Pc, double *Qc)

{
  int m;
  double Pr, Qcx, pbin, qbin;
  const struct MOVB_PD *PD1;
  const struct MOVB_PD *PD2;


  Pr = 1;
  Qcx = 0;
  if (Nchan > 1) {
    PD1 = &MOVBI[0].PDV;
    PD2 = &MOVBI[1].PDV;
    for (m = 0; m < Nc; ++m) {
      pbin = MAXV (PD1->p[m], PD2->p[m]);
      qbin = MAXV (PD1->q[m], PD2->q[m]);
      Pr = Pr * (1 - pbin);
      Qcx += qbin;
    }
  }
  else {
    PD1 = &MOVBI[0].PDV;
    for (m = 0; m < Nc; ++m) {
      Pr = Pr * (1 - PD1->p[m]);
      Qcx += PD1->q[m];
    }
  }

  *Pc = 1 - Pr;
  *Qc = Qcx;

  return;
}

/* Average probability of detection values */


void
PQavgPD (const struct MOVC_PD *PD, int Np, double *ADBB, double *MFPDB)

{
  int i, Nd;
  double Phc, Pcmax, Qsum;

  static const double c0 = 0.9;
  const double c1 = PD->c1;

  Phc = 0;
  Pcmax = 0;
  Qsum = 0;
  Nd = 0;
  for (i = 0; i < Np; ++i) {
    Phc = c0 * Phc + (1 - c0) * PD->Pc[i];
    Pcmax = MAXV (Pcmax * c1, Phc);

    if (PD->Pc[i] > 0.5) {
      ++Nd;
      Qsum += PD->Qc[i];
    }
  }

  if (Nd == 0)
    *ADBB = 0;
  else if (Qsum > 0)
    *ADBB = log10 (Qsum / Nd);
  else
    *ADBB = -0.5;

  *MFPDB = Pcmax;

  return;
}
