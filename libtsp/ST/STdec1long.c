/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int STdec1long (const char String[], long int *Lval)

Purpose:
  Decode a long integer value

Description:
  This routine decodes a string containing a numeric value.  The decoded datum
  is stored as a long integer value.  If a decoding error is detected, a
  warning message is printed and an error status value is returned.  A warning
  message is printed if extra data follows the requested value (this data is
  ignored).

Parameters:
  <-  int STdec1long
      Error status,
        0 - no error
        1 - error: data format error
        2 - error: data value out of range
   -> const char String[]
      Input string
  <-  long int *Lval
      Returned value.  This value is not changed if an error status is returned.

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.7 $  $Date: 2017/05/01 20:11:22 $

-------------------------------------------------------------------------*/

#include <errno.h>
#include <stdlib.h> /* strtol */

#include <libtsp.h>
#include <libtsp/nucleus.h>
#include <libtsp/STmsg.h>

#define MAXC  23


int
STdec1long (const char String[], long int *Lval)

{
  long int lv;
  char *endstr;

  /* Trim initial white-space */
  String = STtrimIws (String);
  if (String[0] == '\0') {
    UTwarn ("STdec1long - %s", STM_EmptyData);
    return 1;
  }

  errno = 0;
  lv = strtol (String, &endstr, 10);

  /* A decoding error is marked by
  (1) Empty String (handled above)
  (2) endstr pointing to String (which has been trimmed of leading white-space)
  (3) More non-white-space characters following the decoded string
  */
  endstr = STtrimIws (endstr);        /* Trim leading white-space on endstr */
  if (endstr[0] != '\0') {
    /* Data format error */
    UTwarn ("STdec1long - %s: \"%s\"", STM_DataErr, STstrDots (String, MAXC));
    return 1;
  }

  if (errno == ERANGE) {
    UTwarn ("STdec1long - %s: \"%s\"", STM_InvVal, STstrDots (String, MAXC));
    return 2;
  }

  /* Success. return the decoded value */
  *Lval = lv;

  return 0;
}
