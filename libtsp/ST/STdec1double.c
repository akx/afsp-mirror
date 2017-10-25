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
  if extra data follows the requested value (this data is ignored).  Floating
  point underflow values are set to zero and a warning message is issued.

Parameters:
  <-  int STdec1double
      Error status,
        0 - no error
        1 - error: data format error
        2 - error: data value too large in magnitude
   -> const char String[]
      Input string
  <-  double *Dval
      Returned value.  This value is not changed if an error status is returned.

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.8 $  $Date: 2017/05/01 20:10:37 $

-------------------------------------------------------------------------*/

#include <errno.h>
#include <stdlib.h> /* strtod */

#include <libtsp.h>
#include <libtsp/nucleus.h>
#include <libtsp/STmsg.h>

#define MAXC  23


int
STdec1double (const char String[], double *Dval)

{
  double dv;
  char *endstr;

   /* Trim initial white-space */
  String = STtrimIws (String);
  if (String[0] == '\0') {
    UTwarn ("STdec1double - %s", STM_EmptyData);
    return 1;
  }

  errno = 0;
  dv = strtod (String, &endstr);

  /* A decoding error is marked by
  (1) Empty String (handled above)
  (2) endstr pointing to String (which has been trimmed of leading white-space)
  (3) More non-white-space characters following the decoded string
  */
  endstr = STtrimIws (endstr);    /* Trim leading white-space on endstr */
  if (endstr[0] != '\0') {
    /* Data format error */
    UTwarn ("STdec1double - %s: \"%s\"", STM_DataErr, STstrDots (String, MAXC));
    return 1;
  }

  if (errno == ERANGE) {
    if (dv == 0.0)
      UTwarn ("STdec1double - %s: \"%s\"", STM_SmallVal, STstrDots (String, MAXC));
    else {
      UTwarn ("STdec1double - %s: \"%s\"", STM_InvVal, STstrDots (String, MAXC));
      return 2;
    }
  }

  /* Success. return the decoded value */
  *Dval = dv;

  return 0;
}
