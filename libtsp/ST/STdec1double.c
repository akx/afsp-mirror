/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int STdec1double (const char String[], double *Dval)

Purpose:
  Decode a double value

Description:
  This routine decodes a string containing a numeric value.  The decoded datum
  is stored as a double value.  If a decoding error is detected, a warning
  message is printed and error status is set.  A warning messages is printed
  if extra data follows the requested value (this data is ignored).

Parameters:
  <-  int STdec1double
      Error status,
        0 - no error
        1 - error, too few values or data format error
	2 - warning, data values too large or too small
   -> const char String[]
      Input string
  <-  double *Dval
      Returned value.  This value is not changed if an error status is
      returned.

Author / revision:
  P. Kabal  Copyright (C) 1999
  $Revision: 1.4 $  $Date: 1999/06/04 22:32:50 $

-------------------------------------------------------------------------*/

static char rcsid[] = "$Id: STdec1double.c 1.4 1999/06/04 AFsp-v6r8 $";

#include <libtsp.h>
#include <libtsp/nucleus.h>


int
STdec1double (const char String[], double *Dval)

{
  return (STdec1val (String, 'D', (void *) Dval));
}
