/*------------- Telecommunications & Signal Processing Lab -------------
                           McGill University

Routine:
  void PQgenTables (int Version, const struct PQ_Opt *PQopt,
                    struct PQ_Par *PQpar)

Purpose:
  Generate tables and parameters for PQevalAudio

Description:
  This routine fills in tables and parameters for PQevalAudio.

Parameters:
   -> int Version
      PEAQ version, PQ_BASIC or PQ_ADVANCED
   -> const struct PQ_Opt *PQopt
      Processing options
  <-  struct PQ_par *PQpar
      Tables and parameters

Author / revision:
  P. Kabal  Copyright (C) 2009
  $Revision: 1.20 $  $Date: 2009/03/23 15:51:07 $

----------------------------------------------------------------------*/

#include <assert.h>
#include <math.h>

#include <libtsp.h>
#include "PQevalAudio.h"

#include "PQCB.h"
#include "PQnNet.h"

#define DBPI(x)		(pow (10., x / 10.))	/* Inverse dB (power) */
#define DBVI(x)		(pow (10., x / 20.))	/* Inverse dB (magnitude) */
#define MAXV(a, b)	(((a) > (b)) ? (a) : (b))
#define MINV(a, b)	(((a) < (b)) ? (a) : (b))
#define PI		3.14159265358979323846
#define HANN(x,N)	FIdWinHamm(x,N,0.5)

static void
PQ_Adap_Tables (const double fc[], int Nc, struct Par_Patt *Patt);
static void
PQ_BW_Par (int NF, double Fs, struct Par_BW *BW);
static void
PQ_CBmap (const double fl[], const double fu[], int Nc, struct Par_CB *CB);
static void
PQ_CB_Tables (int Version, struct Par_CB *CB);
static void
PQ_EHS_Tables (struct Par_EHS *EHS);
static void
PQ_FFT_Tables (double Lp, struct Par_FFT *FFT);
static double
PQ_FFTScale (int NF, double Amax, double fcN, double Lp);
static void
PQ_Loud_Tables (const double fc[], int Nc, struct Par_Loud *Loud);
static void
PQ_ModDiff_Tables (const double fc[], int Nc, struct Par_MDiff *MDiff);
static void
PQ_ModPatt_Tables (const double fc[], int Nc, struct Par_MPatt *MPatt);
static void
PQ_NLoud_Tables (const double fc[], int Nc, struct Par_NLoud *NLoud);
static void
PQ_NNet_Tables (int Version, int ClipMOV, struct Par_NNet *NNet);
static void
PQ_NMR_Tables (int Nc, struct Par_NMR *NMR);
static void
PQ_TDS_Tables (const double fc[], int Nc, struct Par_TDS *TDS); 
static double
PQ_gp (double fcN, double W, int NF);


void
PQgenTables (int Version, const struct PQ_Opt *PQopt, struct PQ_Par *PQpar)

{
  int Nc;
  const double *fc;

  /* FFT tables */
  PQ_FFT_Tables (PQopt->Lp, &PQpar->FFT);

  /* Critical band tables */
  PQ_CB_Tables (Version, &PQpar->CB);
  Nc = PQpar->CB.Nc;
  fc = PQpar->CB.fc;

  /* Time domain spreading tables */
  PQ_TDS_Tables (fc, Nc, &PQpar->TDS); 

  /* Pattern adaptation tables */
  PQ_Adap_Tables (fc, Nc, &PQpar->Patt);

  /* Loudness tables */
  PQ_Loud_Tables (fc, Nc, &PQpar->Loud);

  /* Modulation pattern tables */
  PQ_ModPatt_Tables (fc, Nc, &PQpar->MPatt);

  /* Modulation difference tables */
  PQ_ModDiff_Tables (fc, Nc, &PQpar->MDiff);

  /* Noise loudness tables */
  PQ_NLoud_Tables (fc, Nc, &PQpar->NLoud);

  /* Bandwidth measurement parameters */
  PQ_BW_Par (PQ_NF, PQ_FS, &PQpar->BW);

  /* Noise-to-mask tables */
  PQ_NMR_Tables (Nc, &PQpar->NMR);

  /* EHS tables */
  PQ_EHS_Tables (&PQpar->EHS);

  /* Neural net tables */
  PQ_NNet_Tables (Version, PQopt->ClipMOV, &PQpar->NNet);

  return;
}

/* FFT tables */

static void
PQ_FFT_Tables (double Lp, struct Par_FFT *FFT)

{
  double GL;

  /* Hann window for FFT analysis */
  HANN (FFT->hw, PQ_NF);		/* Standard scaling */

  /* Scale the window */
  GL = PQ_FFTScale (PQ_NF, PQ_AMAX, PQ_FCS/PQ_FS, Lp);
  VRdScale (GL, FFT->hw, FFT->hw, PQ_NF);

  return;
}
/* ---------------------------------------- */
/* FFT processing scaling factor to compensate for loudness */

static double
PQ_FFTScale (int NF, double Amax, double fcN, double Lp)

{
  double W, gp, GL;

/* Calculate the gain for the Hann Window
  - level Lp (SPL) corresponds to a sine with normalized frequency
    fcN and a peak value of Amax
*/
  W = NF - 1;
  gp = PQ_gp (fcN, W, NF);
  GL = DBVI (Lp) / (gp * (Amax / 4.) * W);

  return GL;
}
/* ---------------------------------------- */
/* Calculate the peak factor. The signal is a sinusoid windowed with
   a Hann window. The sinusoid frequency falls between DFT bins. The
   peak of the frequency response (on a continuous frequency scale) falls
   between DFT bins. The largest DFT bin value is the peak factor times
   the peak of the continuous response.
     fcN - Normalized sinusoid frequency (0-1)
     NF  - Frame (DFT) length samples
     NW  - Window length samples
*/

static double
PQ_gp (double fcN, double W, int NF)

{
  int k;
  double df, dfN, dfW, gp;

  /* Distance to the nearest DFT bin */
  df = 1.0 / NF;
  k = (int) (fcN / df);		/* DFT bin number */
  dfN = MINV ((k+1) * df - fcN, fcN - k * df);

  dfW = dfN * W;
  gp = sin(PI * dfW) / (PI * dfW * (1 - dfW*dfW));

  return gp;
}
/* Critical band tables */

static void
PQ_CB_Tables (int Version, struct Par_CB *CB)

{
  double f[PQ_NF/2+1];
  double E1[PQ_MAXNC];
  double Es1[PQ_MAXNC];

  /* Middle and outer ear weighting function */
  VRdLinInc (0, 0.5*PQ_FS, f, PQ_NF/2 + 1);	/* Frequency array */
  PQWOME (f, CB->W2, PQ_NF/2 + 1);

  if (Version == PQ_BASIC) {
    CB->Nc = CB_B.Nc;
    CB->dz = CB_B.dz;
    CB->fc = CB_B.fc;
    PQ_CBmap (CB_B.fl, CB_B.fu, CB_B.Nc, CB);
  }
  else if (Version == PQ_ADVANCED) {
    CB->Nc = CB_A.Nc;
    CB->dz = CB_A.dz;
    CB->fc = CB_A.fc;
    PQ_CBmap (CB_A.fl, CB_A.fu, CB_A.Nc, CB);
  }
  else
    assert (0);

  /* Spreading normalization factor (Bs) */
  /* The normalization factors are defined to give a unit energy output for a
     unit energy input. We define the initial normalization factors to be
     unity. We then run the spreading routine with unit input energy. The
     resulting spread energies are the required normalization factors.
  */
  VRdSet (1.0, E1, CB->Nc);
  VRdSet (1.0, CB->Bs, CB->Nc);
  PQspreadCB (E1, CB, Es1);
  VRdCopy (Es1, CB->Bs, CB->Nc);

  /* Internal noise term */
  PQintNoise (CB->fc, CB->EIN, CB->Nc);

  return;
}
/* ---------------------------------------- */
/* Critical band grouping tables */

static void
PQ_CBmap (const double fl[], const double fu[], int Nc, struct Par_CB *CB)

{
  int i, k;
  double fli, fui;

  static const int NF = PQ_NF;
  static const double df = PQ_FS / PQ_NF;

  for (i = 0; i < Nc; ++i) {
    fli = fl[i];
    fui = fu[i];

    for (k = 0; k < NF/2; ++k) {
      if ((k+0.5)*df > fli) {
	CB->kl[i] = k;			/* First bin in band i */
	CB->Ul[i] = (MINV (fui, (k+0.5)*df) - MAXV(fli, (k-0.5)*df)) / df;
	break;
      }
    }
    for (k = NF/2; k >= 0; --k) {
      if ((k-0.5)*df < fui) {
	CB->ku[i] = k;			/* Last bin in band i */
	if (CB->kl[i] == CB->ku[i])
	  CB->Uu[i] = 0;		/* Single bin in band */
	else
	  CB->Uu[i] = (MINV(fui, (k+0.5)*df) - MAXV(fli, (k-0.5)*df)) / df;
	break;
      }
    }
  }

  return;
}

/* Time domain spreading tables */

static void
PQ_TDS_Tables (const double fc[], int Nc, struct Par_TDS *TDS)

{
  static const double Fss = PQ_FS / PQ_CB_ADV;

  /* Number of bands */
  TDS->Nc = Nc;

 /* Difference equation coefficents */
  PQtConst (0.030, 0.008, Fss, fc, TDS->a, TDS->b, Nc);

  return;
}

/* Tables for pattern adaptation */

static void
PQ_Adap_Tables (const double fc[], int Nc, struct Par_Patt *Patt)

{
  double Fss;

  Patt->Nc = Nc;

  /* The number of bands uniquely identifies the processing scenario */
  Fss = 0;
  if (Nc == PQ_NC_B) {
    Fss = PQ_FS / PQ_CB_ADV;
    Patt->M1 = 3;
    Patt->M2 = 4;
  }
  else if (Nc == PQ_NC_CB_A) {
    Fss = PQ_FS / PQ_CB_ADV;
    Patt->M1 = 1;
    Patt->M2 = 2;
  }
  else if (Nc == PQ_NC_FB_A) {
    Fss = PQ_FS / PQ_FB_ADV;
    Patt->M1 = 1;
    Patt->M1 = 1;
  }
  else
    assert (0);

  /* Difference equation coefficents */
  PQtConst (0.050, 0.008, Fss, fc, Patt->a, Patt->b, Nc);

  return;
}

/* Tables for the loudness calculation */

#define E0	(1e4)	/* Reference loudness */

static void
PQ_Loud_Tables (const double fc[], int Nc, struct Par_Loud *Loud)

{
  int m;
  double c;
  double *s;
  double *Et;
  static const double e = 0.23;

  /* Gain constant */
  c = 0;
  if (Nc == PQ_NC_B || Nc == PQ_NC_CB_A)
    c = 1.07664;
  else if (Nc == PQ_NC_FB_A)
    c = 1.26539;
  else
    assert (0);

  /* Number of bands */
  Loud->Nc = Nc;

  /* Excitation index */
  PQexIndex (fc, Loud->s, Nc);
  s = Loud->s;

  /* Energy threshold */
  PQenThresh (fc, Loud->Et, Nc);

  /* Precompute the fixed multiplicative term (per frequency) */
  Et = Loud->Et;
  for (m = 0; m < Nc; ++m)
    Loud->Ets[m] = c * pow (Et[m] / (s[m] * E0), e);

  return;
}

/* Tables for the modulation pattern calculations */

static void
PQ_ModPatt_Tables (const double fc[], int Nc, struct Par_MPatt *MPatt)

{
  static const double Fss = PQ_FS / PQ_CB_ADV;

  /* Number of bands */
  MPatt->Nc = Nc;

  /* Difference equation coefficents */
  PQtConst (0.050, 0.008, Fss, fc, MPatt->a, MPatt->b, Nc);
  return;
}

/* Tables for the modulation difference calculations */

static void
PQ_ModDiff_Tables (const double fc[], int Nc, struct Par_MDiff *MDiff)

{
  int m;
  double Et[PQ_MAXNC];
  static const double e = 0.3;

  /* Number of bands */
  MDiff->Nc = Nc;

  /* Energy threshold */
  PQintNoise (fc, Et, Nc);
  for (m = 0; m < Nc; ++m)
    MDiff->Ete[m] = pow (Et[m], e);

  return;
}

/* Tables for the noise loudness calculations */

static void
PQ_NLoud_Tables (const double fc[], int Nc, struct Par_NLoud *NLoud)

{
  /* Number of bands */
  NLoud->Nc = Nc;

  /* Energy threshold */
  PQintNoise (fc, NLoud->Et, Nc);

  return;
}

/* Parameters for BW calculations */

#define ROUND(x)	((int) (x + 0.5))	/* x >= 0 */
#define PQ_FX	21586.
#define PQ_FL	8109.
#define PQ_FRdB	10.
#define PQ_FTdB	5.


static void
PQ_BW_Par (int NF, double Fs, struct Par_BW *BW)

{
  BW->N = NF/2;				/* Limit from pseudo-code */
  BW->kx = ROUND (PQ_FX / Fs * NF);	/* 921 */
  BW->kl = ROUND (PQ_FL / Fs * NF);   	/* 346 */
  BW->FR = DBPI (PQ_FRdB);
  BW->FT = DBPI (PQ_FTdB);

  return;
}

/* Tables for NMR calculation */

static void
PQ_NMR_Tables (int Nc, struct Par_NMR *NMR)

{
  int m;
  double dz, mdB;

  /* The number of bands uniquely identifies the processing scenario */
  dz = 0;
  if (Nc == PQ_NC_B)
    dz = CB_B.dz;
  else if (Nc == PQ_NC_CB_A)
    dz = CB_A.dz;
  else
    assert (0);

  NMR->Nc = Nc;
  for (m = 0; m < Nc; ++m) {
    if (m <= 12 / dz)
      mdB = 3;
    else
      mdB = 0.25 * m * dz;
    NMR->gm[m] = DBPI (-mdB);
  }

  return;
}

/* EHS tables */

#define LOG2(x)	(log(x) / log(2))
#define FMAX	9000.
#define ENTHR	8000.
#define LTWIN	2*PQ_NL-1

static void
PQ_EHS_Tables (struct Par_EHS *EHS)

{
  int log2, M, NL;
  double c;
  double twin[LTWIN];

  log2 = (int) ceil (LOG2((FMAX / PQ_FS) * PQ_NF));
  NL = (int) (pow (2., log2 - 1) + 0.5);
  M = NL;

  EHS->EnThr = ENTHR;
  EHS->NL = NL;
  EHS->M = M;

  /* Scaled Hann window (includes DFT scaling factor 1/NL) */
  HANN (twin, LTWIN);
  c = (1. / NL) * sqrt (8. / 3.);
  VRdScale (c, &twin[NL-1], EHS->Hw, NL);

  return;
}

/* Neural net tables */

static void
PQ_NNet_Tables (int Version, int ClipMOV, struct Par_NNet *NNet)

{
  int i;

  NNet->ClipMOV = ClipMOV;

  if (Version == PQ_BASIC) {
    NNet->I = NNet_B.I;
    NNet->J = NNet_B.J;
    NNet->amin = NNet_B.amin;
    NNet->amax = NNet_B.amax;
    for (i = 0; i < NNet_B.I; ++i)
      NNet->wx[i] = &NNet_B.wx[i][0];
    NNet->wxb = NNet_B.wxb;
    NNet->wy = NNet_B.wy;
    NNet->wyb = NNet_B.wyb;
    NNet->bmin = NNet_B.bmin;
    NNet->bmax = NNet_B.bmax;
  }
  else if (Version == PQ_ADVANCED) {
    NNet->I = NNet_A.I;
    NNet->J = NNet_A.J;
    NNet->amin = NNet_A.amin;
    NNet->amax = NNet_A.amax;
    for (i = 0; i < NNet_A.I; ++i)
      NNet->wx[i] = &NNet_A.wx[i][0];
    NNet->wxb = NNet_A.wxb;
    NNet->wy = NNet_A.wy;
    NNet->wyb = NNet_A.wyb;
    NNet->bmin = NNet_A.bmin;
    NNet->bmax = NNet_A.bmax;
  }
  else
    assert (0);

  return;
}
