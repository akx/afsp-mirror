/*------------- Telecommunications & Signal Processing Lab --------------
                          McGill University
Routine:
  void CASNR (const struct Stats_T *Stats, double *SNR, double *SNRG,
	      double *SF, double *SSNR)

Purpose:
  Calculate SNR values for two files

Description:
  This routine calculates SNR values for two files.  Three types of SNR are
  calculated: the conventional SNR, gain adjusted SNR and segmental SNR.

Parameters:
   -> const struct Stats_T *Stats
      Structure containing the cross-statistics
  <-  double *SNR
      Signal-to-noise ratio.  A value of DBL_MAX indicates an infinite SNR.
  <-  double *SNRG
      Gain optimized signal-to-noise ratio.  A value of DBL_MAX indicates an
      infinite SNR.
  <-  double *SF
      Gain factor for the gain optimized signal-to-noise ratio
  <-  double *SSNR
      Segmental signal-to-noise ratio.  This value is set to -1 if the
      segmental SNR is not available.

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.9 $  $Date: 2003/05/15 01:21:28 $

-----------------------------------------------------------------------*/

#include <float.h>	/* DBL_MAX */
#include <math.h>	/* log10 */

#include "CompAudio.h"


void
CASNR (const struct Stats_T *Stats, double *SNR, double *SNRG, double *SF,
       double *SSNR)

{
  double denom;

/* Conventional SNR */
/* (1) File A: zero
       (a) File B: zero
           SNR = infinite
       (b) File B: nonzero
           SNR = 0
   (2) File A: nonzero
       SNR calculated, can be 0 to infinity
*/
  if (Stats->Sx2 == 0.0) {
    if (Stats->Sy2 == 0.0)
      *SNR = DBL_MAX;
    else
      *SNR = 0.0;
  }
  else {
    denom = Stats->Sx2 - 2.0 * Stats->Sxy + Stats->Sy2;
    if (denom > 0.0)
      *SNR = Stats->Sx2 / denom;
    else
      *SNR = DBL_MAX;
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
  if (Stats->Ndiff == 0) {
    *SF = 1.0;
    *SNRG = DBL_MAX;
  }
  else if (Stats->Sy2 == 0.0) {
    *SF = 1.0;
    *SNRG = 1.0;
  }
  else if (Stats->Sx2 == 0.0) {
    *SF = 0.0;
    *SNRG = DBL_MAX;
  }
  else {
    *SF = Stats->Sxy / Stats->Sy2;
    denom = Stats->Sx2 * Stats->Sy2 - Stats->Sxy * Stats->Sxy;
    if (denom > 0.0)
      *SNRG = (Stats->Sx2 * Stats->Sy2) / denom;
    else
      *SNRG = DBL_MAX;
  }
  
  /* Segmental SNR */
  if (Stats->Nseg > 0)
    *SSNR = pow (10.0, (Stats->SNRlog / Stats->Nseg)) - 1.0;
  else
    *SSNR = -1.0;

  return;
}
