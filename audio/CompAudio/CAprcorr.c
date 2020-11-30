/*------------- Telecommunications & Signal Processing Lab --------------
                          McGill University
Routine:
  void CAprcorr(struct Stats_X *StatsX, const long int Delay[2],
                const double ScaleF[2])

Purpose:
  Print SNR values / differences for two files

Description:
  This routine prints SNR values for two files. Three types of SNR are printed:
  the conventional SNR, gain adjusted SNR and segmental SNR. In addition, the
  number of differing samples and maximum difference is printed.

Parameters:
   -> const struct Stats_X *StatsX
      Structure containing the cross-statistics
   -> const double Delay[2]
      Delay range
   -> const double ScaleF[2]
      Native scale factor for each file. These should be the scale factors
      before any other user supplied scale factors are applied.

Author / revision:
  P. Kabal  Copyright (C) 2020
  $Revision: 1.22 $  $Date: 2020/11/27 02:07:12 $

-----------------------------------------------------------------------*/

#include <float.h>  /* DBL_MAX */
#include <math.h> /* log10 */

#include "CompAudio.h"

#define DB(x)   (10.0 * log10(x))


void
CAprcorr(struct Stats_X *StatsX, const long int Delay[2],
         const double ScaleF[2])

{
  struct SNR_X SNRX;
  double SFI;
  int PrV;

  /* Equal data, no need for other values */
  if (StatsX->Ndiff == 0) {
    /* Identical files */
    printf("\n");
    if (Delay[0] < Delay[1] || Delay[0] != 0)
      printf(CAMF_AeqBdelay, StatsX->Delay);
    else
      printf(CAMF_AeqB);
    return;
  }

  /* Print best delay */
  if (Delay[0] < Delay[1])
    printf(CAMF_BestMatch, StatsX->Delay);
  else if (Delay[0] != 0)
    printf(CAMF_Delay, StatsX->Delay);

  PrV = 0;
  if (ScaleF[0] == ScaleF[1]) {
    SFI = 1.0 / ScaleF[0];
    PrV = (SFI >= 128 && SFI <= 32768);
  }

/* Calculate the SNR values */
  SNRX = CASNR(StatsX);

  /* Ordinary SNR */
  printf(CAMF_SNR, DB(SNRX.SNR));

  /* Gain optimized SNR */
  if (SNRX.SNRG != DBL_MAX)
    printf(CAMF_SNRwGain, DB(SNRX.SNRG), SNRX.SF);
  else
    printf(CAMF_AxB, SNRX.SF);

  /* Segmental SNR */
  if (SNRX.SSNR >= 0.0)
    printf(CAMF_SegSNR, DB(SNRX.SSNR), SNRX.Nsseg);

  /* Differences */
  if (PrV) {
    if (StatsX->Nrun == 0 || StatsX->Nrun == StatsX->Ndiff)
      printf(CAMF_DiffV, SFI * StatsX->Diffmax, 100. * StatsX->Diffmax,
             StatsX->Ndiff);
    else if (StatsX->Nrun == 1)
      printf(CAMF_DiffV1, SFI * StatsX->Diffmax, 100. * StatsX->Diffmax,
             StatsX->Ndiff);
    else
      printf(CAMF_DiffVN, SFI * StatsX->Diffmax, 100. * StatsX->Diffmax,
             StatsX->Ndiff, StatsX->Nrun);
  }
  else {
    if (StatsX->Nrun == 0 || StatsX->Nrun == StatsX->Ndiff)
      printf(CAMF_DiffP, 100. * StatsX->Diffmax, StatsX->Ndiff);
    else if (StatsX->Nrun == 1)
      printf(CAMF_DiffP1, 100. * StatsX->Diffmax, StatsX->Ndiff);
    else
      printf(CAMF_DiffPN, 100. * StatsX->Diffmax, StatsX->Ndiff, StatsX->Nrun);
  }
}
