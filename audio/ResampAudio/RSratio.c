/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void RSratio (double Sratio, int Ir, long int *Ns, double *Ds,
                long int NsMax, long int DsMax, FILE *fpinfo);

Purpose:
  Find a rational approximation to a given value

Description:
  This procedure finds a rational approximation to a given value Sratio,
    Sratio = Ns / Ds.
  Both Ns and Ds take on integer values if Sratio can be represented exactly
  as a ratio of integers.  Otherwise, Ns is set to unity and Ds is set
  to 1/Sratio.

  In addition, if possible, Ns and Ds are modified by multiplying each by the
  same value, such that the resultant Ns is divisible by Ir.

Parameters:
   -> double Sratio
      Input value to be represented
   -> int Ir
      Input value.  If possible the output value Ns will be such that Ir is
      a factor of Ns.
  <-  long int *Ns
      Output numerator value for the rational representation
  <-  double *Ds
      Output denominator value for the rational representation
      Integral part of the result
   -> long int NsMax
      Maximum value for Ns
   -> long int DsMax
      Maximum value for Ds
   -> FILE *fpinfo
      File pointer for printing filter information.  If fpinfo is not NULL,
      information about the filter is printed on the stream selected by fpinfo.

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.15 $  $Date: 2003/05/13 01:09:07 $

-------------------------------------------------------------------------*/

#include <assert.h>
#include <float.h>	/* DBL_EPSILON */

#include <libtsp.h>
#include "ResampAudio.h"

#define ABSV(x)		(((x) < 0) ? -(x) : (x))


void
RSratio (double Sratio, int Ir, long int *Ns, double *Ds, long int NsMax,
	 long int DsMax, FILE *fpinfo)

{
  long int Lgcd, LNs, LDs, M;
  double DDs;
  double Xratio;

  assert (Sratio > 0.0 && Ir > 0);

  /* Find a rational approximation to Sratio, Sratio = Ns / Ds */
  MSratio (Sratio, &LNs, &LDs, 0.0, NsMax, DsMax);

  /* We will consider the rational approximation to be "exact" if
       |Sratio - LNs/LDs| <= eps * Sratio
  */
  Xratio = (double) LNs / LDs;
  if (ABSV(Sratio - Xratio) >= ABSV(Sratio) * DBL_EPSILON) {
    LNs = 1L;
    DDs = 1.0 / Sratio;
    if (fpinfo != NULL)
      fprintf (fpinfo, RSMF_Ratio1, Sratio);
  }
  else {
    DDs = LDs;
    if (fpinfo != NULL)
      fprintf (fpinfo, RSMF_Ratio2, LNs, LDs);
  }

  /* For ordinary interpolation Ns = Ir.  In other cases, we multiply Ns and
     Ds by M such that the resultant Ns is divisible by Ir.
  */
  Lgcd = FNgcd (LNs, (long int) Ir);
  M = Ir / Lgcd;
  if (DDs <= DsMax/M && LNs <= NsMax/M) {
    DDs = M * LDs;
    LNs = M * LNs;
  }

  *Ds = DDs;
  *Ns = LNs;

  return;
}

