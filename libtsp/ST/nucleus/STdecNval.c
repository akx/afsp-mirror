/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int STdecNval (const char String[], int Nmin, int Nmax, int Type, void *Val,
                 int *N)

Purpose:
  Decode numeric values (variable number)

Description:
  This routine decodes a string containing numeric data.  Multiple data items
  data items in the string are separated by commas or white-space (as defined
  by the isspace routine).  The data type is an input parameter. If the number
  of data values in the string is less than a given minimum number or a decoding
  error is detected, an error message is printed and an error indication is
  returned.  A warning messages is printed if extra data follows the requested
  values (this data is ignored).

Parameters:
  <-  int STdecNval
      Error status,
        0 - no error
        1 - error, too few values or data format error
        2 - error, data value out of range
  2 - warning, data values too large or too small
   -> const char String[]
      Input string
   -> int Nmin
      Minimum number of values to be read (may be zero)
   -> int Nmax
      Maximum number of values to be read
   -> int Type
      Character code for the data type:
       'D' for double
       'F' for float
       'I' for int
       'L' for long int
       'U' for unsigned long int
  <-  void *Val
      Array of Nmax elements used to store the decoded values.  Only the first
      N values are modified.
  <-  int *N
      Actual number of values decoded.  In the case of an error, N indicates
      the number of values successfully decoded.  In that case, N may be less
      than Nmin.

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.16 $  $Date: 2017/05/24 16:56:43 $

-------------------------------------------------------------------------*/

#include <string.h>

#include <libtsp.h>
#include <libtsp/nucleus.h>
#include <libtsp/STmsg.h>

#define MAXC  256


int
STdecNval (const char String[], int Nmin, int Nmax, int Type, void *Val, int *N)

{

  char *token;
  int nc, n;
  const char *p;
  int status;
  char cbuf[MAXC+1];

/* Allocate temporary string space */
  nc = (int) strlen (String);
  if (nc > MAXC)
    token = (char *) UTmalloc (nc + 1);
  else
    token = cbuf;

/* Find the substrings */
  p = String;
  n = 0;
  status = 0;
  while  (p != NULL && n < Nmax) {
    p = STfindToken (p, ",", "", token, 2, nc);
    if (token[0] != '\0') {

      switch (Type) {
      case 'D':
        status = STdec1double (token, (void *) (((double *) Val) + n));
        break;
      case 'F':
        status = STdec1float (token, (void *) (((float *) Val) + n));
        break;
      case 'I':
        status = STdec1int (token, (void *) (((int *) Val) + n));
        break;
      case 'L':
        status = STdec1long (token, (void *) (((long int *) Val) + n));
        break;
      case 'U':
        status = STdec1ulong (token, (void *) (((unsigned long int *) Val) + n));
        break;
      }

      if (status != 0)
        break;
      ++n;
    }
    else {
      /* Null token string */
      if (p != NULL || n != 0 || Nmin != 0) {
        status = 1;
        UTwarn ("STdecNval - %s", STM_EmptyData);
      }
      break;
    }
  }

/* Check the number of values found */
  if (status != 1) {
    if (n < Nmin) {
      status = 1;
      UTwarn ("STdecNval - %s", STM_TooFewData);
    }
    if (p != NULL && n >= Nmax)
      UTwarn ("STdecNval - %s", STM_ExtraData);
  }

  if (token != cbuf)
    UTfree ((void *) token);
  *N = n;

  return status;
}
