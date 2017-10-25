/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int STdec1float (const char String[], float *Fval)

Purpose:
  Decode a float value

Description:
  This routine decodes a string containing a numeric value.  The decoded datum
  is stored as a float value.  If a decoding error is detected, a warning
  message is printed and error status is set.  A warning messages is printed
  if extra data follows the requested value (this data is ignored).  Floating
  point underflow values are set to zero and a warning message is issued.

Parameters:
  <-  int STdec1float
      Error status,
        0 - no error
        1 - error: data format error
        2 - error: data value too large in magnitude
   -> const char String[]
      Input string
  <-  float *Fval
      Returned value.  This value is not changed if an error status is returned.

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.14 $  $Date: 2017/05/01 20:10:54 $

-------------------------------------------------------------------------*/

#include <errno.h>
#include <float.h>  /* FLT_MIN, FLT_MAX */
#include <stdlib.h> /* strtod */

#include <libtsp.h>
#include <libtsp/nucleus.h>
#include <libtsp/STmsg.h>

#define MAXC  23


int
STdec1float (const char String[], float *Fval)

{
  double dv;
  char *endstr;

   /* Trim initial white-space */
  String = STtrimIws (String);
  if (String[0] == '\0') {
    UTwarn ("STdec1float - %s", STM_EmptyData);
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
    UTwarn ("STdec1float - %s: \"%s\"", STM_DataErr, STstrDots (String, MAXC));
    return 1;
  }

  if ((errno == ERANGE && dv != 0.0) || dv < -FLT_MAX || dv > FLT_MAX) {
    UTwarn ("STdec1float - %s: \"%s\"", STM_InvVal, STstrDots (String, MAXC));
    return 2;
  }

  if ((errno == ERANGE && dv == 0.0) ||
      (dv < 0.0 && dv > -FLT_MIN) || (dv > 0.0 && dv < FLT_MIN))
    UTwarn ("STdec1float - %s: \"%s\"", STM_SmallVal, STstrDots (String, MAXC));

  /* Success. return the decoded value */
  *Fval = (float) dv;

  return 0;
}
