/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  double MSdNint (double x)

Purpose:
  Nearest integer function

Description:
  This routine returns the integer-valued double nearest x.  Specifically, the
  returned value is floor(x+0.5) for positive x and ceil(x-0.5) for negative
  x.

Parameters:
  <-  double MSdNint
      Returned integer value
   -> double x
      Input value

Author / revision:
  P. Kabal  Copyright (C) 1999
  $Revision: 1.6 $  $Date: 1999/06/04 22:08:35 $

-------------------------------------------------------------------------*/

static char rcsid[] = "$Id: MSdNint.c 1.6 1999/06/04 AFsp-v6r8 $";

#include <math.h>	/* floor and ceil */

#include <libtsp.h>


double
MSdNint (double x)

{
  double pint;

  if (x >= 0.0)
    pint = floor (x + 0.5);
  else
    pint = ceil (x - 0.5);

  return pint;
}
