/*------------- Telecommunications & Signal Processing Lab --------------
                          McGill University
Routine:
  void CAprcorr (const struct Stats_T *Stats)

Purpose:
  Print SNR values for two files

Description:
  This routine prints SNR values for two files.  Three types of SNR are
  printed: the conventional SNR, gain adjusted SNR and segmental SNR.  In
  addition, the number of differing samples and maximum difference is printed.

Parameters:
   -> const struct Stats_T *Stats
      Structure containing the cross-statistics

Author / revision:
  P. Kabal  Copyright (C) 1999
  $Revision: 1.15 $  $Date: 1999/06/05 00:54:10 $

-----------------------------------------------------------------------*/

static char rcsid[] = "$Id: CAprcorr.c 1.15 1999/06/05 AFsp-v6r8 $";

#include <float.h>	/* DBL_MAX */
#include <math.h>	/* log10 */

#include "CompAudio.h"

#define DB(x)		(10.0 * log10 (x))


void
CAprcorr (const struct Stats_T *Stats)

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

/* Number of differing samples */
  if (Stats->Nrun == 1)
    printf (CAMF_NumDiff1, Stats->Ndiff);
  else
    printf (CAMF_NumDiffN, Stats->Ndiff, Stats->Nrun);
  printf (CAMF_MaxDiff, Stats->Diffmax);

  return;
}
