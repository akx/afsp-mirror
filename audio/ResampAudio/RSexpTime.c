/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void RSexpTime (double t, long int M, struct Tval_T *T)

Purpose:
  Express a value in terms of residues

Description:
  This procedure expresses a given value t in terms of an integer part and a
  fractional part,
    t = floor(t) + frac(t)
      = n + (dm + dmr) / M,
  where n is an integer value, dm is a positive integer less than M, and dmr
  is a positive fractional value, 0 <= dmr < 1.  The values dm and dmr are
  calculated as follows,
    dm = floor(M * frac(t))
    dmr = frac(M * frac(t)).
  This representation for a value, allows for the value to be incremented
  by an integer multiple of 1/M without error accumulation.

Parameters:
   -> double t
      Input value to be represented
   -> long int M
      Input value used to perform the modulo operations
  <-  struct Tval_T *T;
      Structure containing the expanded time value

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.8 $  $Date: 2003/05/13 01:08:13 $

-------------------------------------------------------------------------*/

#include <math.h>

#include "ResampAudio.h"

#define EPS	1E-8

/*
   The value t might (due to numerical round-off) be such that the fractional
   value dmr is eps or 1-eps, where eps is a small number.  Here we "force"
   dmr to be zero if it is near zero.  If dmr is near one, we set it to zero
   and add one to dm (this increment in dm may propagate to n also).
*/


void
RSexpTime (double t, long int M, struct Tval_T *T)

{
  long int n;
  long int dm;
  double dmr;
  
  n = (long int) floor (t);
  dm = (long int) floor (M * (t - n));
  dmr = M * (t - n) -  dm;

  /* Move msr if it is nearly an integer */
  if (dmr < EPS)
    dmr = 0.0;
  else if (dmr > 1.0-EPS) {
    dmr = 0.0;
    ++dm;
    if  (dm >= M) {
      dm = 0;
      ++n;
    }
  }

  T->n = n;
  T->dm = dm;
  T->dmr = dmr;
  T->M = M;

  return;
}
