/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void RSincTime (struct Tval_T *T, double di)

Purpose:
  Increment a time value expressed as residues

Description:
  This procedure takes a given time t0 expressed in terms of an integer part
  and a fractional part.  The representation is
    t0 = n0 + (dm0 + dmr0)/M,
  where n is an integer value, ds is a positive integer value less than M, and
  dmr is a positive fractional value, 0 <= dmr < 1.  The value of t0 is
  incremented by di/M, to give a new value t1,
    t1 = t0 + di/M
       = n1 + (dm1 + dmr1)/M .
  Note that dsr remains unchanged if di is an integer.  In such a case, the
  increment occurs without accumulation of error.

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.4 $  $Date: 2003/05/13 01:08:13 $

-------------------------------------------------------------------------*/

#include <math.h>

#include "ResampAudio.h"


void
RSincTime (struct Tval_T *T, double di)

{
  double dx;
  long int ni, lx;

  if (di == floor (di)) {
    lx = T->dm + (long int) di;
    ni = lx / T->M;
    T->n += ni;
    T->dm = lx - T->M * ni;
  }
  else {
    dx = T->dm + T->dmr + di;
    ni = (long int) floor (dx / T->M);
    T->n += ni;
    dx = dx - ni * T->M;
    T->dm = (long int) floor (dx);
    T->dmr = dx - T->dm;
  }
  return;
}
