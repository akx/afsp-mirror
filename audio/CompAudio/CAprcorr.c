/*------------- Telecommunications & Signal Processing Lab --------------
                          McGill University
Routine:
  void CAprcorr (const struct Stats_T *Stats, const double ScaleF[2])

Purpose:
  Print SNR values for two files

Description:
  This routine prints SNR values for two files.  Three types of SNR are
  printed: the conventional SNR, gain adjusted SNR and segmental SNR.  In
  addition, the number of differing samples and maximum difference is printed.

Parameters:
   -> const struct Stats_T *Stats
      Structure containing the cross-statistics
   -> const double ScaleF[2]
      Native scale factor for each file. These should be the scale factors
      before any other user supplied scale factors are applied.

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.17 $  $Date: 2003/05/18 14:51:40 $

-----------------------------------------------------------------------*/

#include <float.h>	/* DBL_MAX */
#include <math.h>	/* log10 */

#include "CompAudio.h"

#define DB(x)		(10.0 * log10 (x))


void
CAprcorr (const struct Stats_T *Stats, const double ScaleF[2])

{
  double SNR, SNRG, SF, SSNR;

/* Calculate the SNR values */
  CASNR (Stats, &SNR, &SNRG, &SF, &SSNR);

  /* Ordinary SNR */
  printf (CAMF_SNR, DB (SNR));

  /* Gain optimized SNR */
  if (SNRG != DBL_MAX)
    printf (CAMF_SNRwGain, DB (SNRG), SF);
  else
    printf (CAMF_AxB, SF);

  /* Segmental SNR */
  if (SSNR >= 0.0)
    printf (CAMF_SegSNR, DB (SSNR), Stats->Nsseg);


  /* Differences */
  if (ScaleF[0] == ScaleF[1]) {
    if (Stats->Nrun == 1)
      printf (CAMF_DiffAP1, Stats->Diffmax / ScaleF[0], 100. * Stats->Diffmax,
	      Stats->Ndiff);
    else
      printf (CAMF_DiffAP2, Stats->Diffmax / ScaleF[0], 100. * Stats->Diffmax,
	      Stats->Ndiff, Stats->Nrun);
  }
  else {
    if (Stats->Nrun == 1)
      printf (CAMF_DiffP1, 100. * Stats->Diffmax, Stats->Ndiff);
    else
      printf (CAMF_DiffP2, 100. * Stats->Diffmax, Stats->Ndiff, Stats->Nrun);
  }
  return;
}
