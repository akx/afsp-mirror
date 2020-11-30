/*------------- Telecommunications & Signal Processing Lab --------------
                          McGill University
Routine:
  void CASNR(const struct Stats_X *StatsX)

Purpose:
  Calculate SNR values for two files

Description:
  This routine calculates SNR values for two files. Three types of SNR are
  calculated: the conventional SNR, gain adjusted SNR, and segmental SNR.

Parameters:
  <-  struct SNR_T CASNR
   -> const struct Stats_X *StatsX
      Structure containing the cross-file statistics

  double CASNR.SNR:
    Signal-to-noise ratio. A value of DBL_MAX indicates an infinite SNR.
  double CASNR.SNRG:
    Gain optimized signal-to-noise ratio. A value of DBL_MAX indicates an
    infinite SNR.
  double CASNR.SF:
    Gain factor for the gain optimized signal-to-noise ratio
  double CASNR.SSNR
    Segmental signal-to-noise ratio. This value is set to -1 if the segmental
    SNR is not available.
  long int CASNR.Nsseg
    Number of samples in each segment

Author / revision:
  P. Kabal  Copyright (C) 2020
  $Revision: 1.15 $  $Date: 2020/11/19 13:31:54 $

-----------------------------------------------------------------------*/

#include <float.h>  /* DBL_MAX */
#include <math.h> /* log10 */

#include "CompAudio.h"

struct SNR_X
CASNR(const struct Stats_X *StatsX)

{
  struct SNR_X SNRX;
  long double denom;

/* Conventional SNR */
/* (1) File A: zero
       (a) File B: zero
           SNR = infinite
       (b) File B: nonzero
           SNR = 0
   (2) File A: nonzero
       SNR calculated, can be 0 to infinity
*/
  if (StatsX->Sxx == 0.0L) {
    if (StatsX->Syy == 0.0L)
      SNRX.SNR = DBL_MAX;
    else
      SNRX.SNR = 0.0;
  }
  else {
    if (StatsX->See > 0.0L)
      SNRX.SNR = StatsX->Sxx / StatsX->See;
    else
      SNRX.SNR = DBL_MAX;
  }

/* Gain optimized SNR */
/* Four cases:
   (1) Equal files:  includes both files zero
       SF = 1;  SNRG = infinite
   (2) File B zero:  File A is not zero
       SF = 1;  SNRG = 1
   (3) File A zero:  File B is not zero
       SF = 0;  SNRG = infinite
   (4) Both File A and File B nonzero:
       SF as calculated, SNRG from 0 to infinity
*/
  if (StatsX->Ndiff == 0) {
    SNRX.SF = 1.0;
    SNRX.SNRG = DBL_MAX;
  }
  else if (StatsX->Syy == 0.0L) {
    SNRX.SF = 1.0;
    SNRX.SNRG = 1.0;
  }
  else if (StatsX->Sxx == 0.0L) {
    SNRX.SF = 0.0;
    SNRX.SNRG = DBL_MAX;
  }
  else {
    SNRX.SF = StatsX->Sxy / StatsX->Syy;
    denom = StatsX->Sxx - StatsX-> Sxy * (StatsX->Sxy / StatsX->Syy);
    if (denom > 0.0L)
      SNRX.SNRG = StatsX->Sxx / denom;
    else
      SNRX.SNRG = DBL_MAX;
  }

  /* Segmental SNR */
  if (StatsX->Nseg > 0)
    SNRX.SSNR = pow(10.0, (StatsX->SNRlog / StatsX->Nseg)) - 1.0;
  else
    SNRX.SSNR = -1.0;
  SNRX.Nsseg = StatsX->Nsseg;

  return SNRX;
}
