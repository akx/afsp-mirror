/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int UTcheckIEEE (void)

Purpose:
  Check if host uses IEEE standard 754 format for float values

Description:
  This routine determines if the current host uses IEEE standard 754 format to
  represent float values.

Parameters:
  <-  int UTcheckIEEE
      Flag, 1 if host uses IEEE float format, 0 otherwise

Author / revision:
  P. Kabal  Copyright (C) 1999
  $Revision: 1.13 $  $Date: 1999/06/04 22:42:58 $

-------------------------------------------------------------------------*/

static char rcsid [] = "$Id: UTcheckIEEE.c 1.13 1999/06/04 AFsp-v6r8 $";

#include <libtsp/nucleus.h>
#include <libtsp/UTtypes.h>

/* Note:
   On many Unix machines, this routine could instead check the setting of
   _IEEE in values.h.
*/

/* IEEE float value check values */
static const uint4_t IEEEiv = 0xc3268000;
static const float4_t IEEEfv = -166.5;


int
UTcheckIEEE (void)

{
  union {
    float4_t fv;
    uint4_t iv;
  } Floatv;

/* Store a float value and check the bit pattern of the result */
  Floatv.fv = IEEEfv;
  if (Floatv.iv == IEEEiv)
    return 1;
  else
    return 0;
}
