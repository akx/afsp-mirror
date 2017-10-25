/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int STdec1val (const char String[], int Type, void *Val)

Purpose:
  Decode a numeric value

Description:
  This routine decodes a string containing a numeric value.  The data type is
  an input parameter.  If a decoding error is detected, a warning message is
  printed and an error status value is returned.  For double or float values,
  an underflow set to zero is not considered a range overflow.

Parameters:
  <-  int STdec1val
      Error status,
        0 - no error
        1 - format error
        2 - range warning
   -> const char String[]
      Input string
   -> int Type
       'D' for double
       'F' for float
       'I' for int
       'L' for long int
       'U' for unsigned long int
  <-  void *Val
      Returned value of the type indicated by Type.  This value is not changed
      for a format error.  For a range warning, the returned value is set to
      the appropriate representable value.

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.29 $  $Date: 2017/05/01 20:13:25 $

-------------------------------------------------------------------------*/

#include <assert.h>

#include <libtsp.h>
#include <libtsp/nucleus.h>


int
STdec1val (const char String[], int Type, void *Val)

{
  int status;

  status = 0;

  /* Return a value of the correct type */
  switch (Type) {
  case 'D':
    status = STdec1double (String, (double *) Val);
    break;
  case 'F':
    status = STdec1float (String, (float *) Val);
    break;
  case 'I':
    status = STdec1int (String, (int *) Val);
    break;
  case 'L':
    status = STdec1long (String, (long int *) Val);
    break;
  case 'U':
    status = STdec1ulong (String, (unsigned long int *) Val);
    break;
  default:
    assert (0);
    break;
  }

  return status;
}
