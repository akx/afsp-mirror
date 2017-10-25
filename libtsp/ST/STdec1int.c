/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int STdec1int (const char String[], int *Ival)

Purpose:
  Decode an integer value

Description:
  This routine decodes a string containing a numeric value.  The decoded datum
  is stored as an integer value.  If a decoding error is detected, a warning
  message is printed and an error status value is returned.  A warning message
  is printed if extra data follows the requested value (this data is ignored).

Parameters:
  <-  int STdec1int
      Error status,
        0 - no error
        1 - error: data format error
        2 - error: data value out of range
   -> const char String[]
      Input string
  <-  int *Ival
      Returned value.  This value is not changed if an error status is returned.

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.13 $  $Date: 2017/05/01 20:11:11 $

-------------------------------------------------------------------------*/

#include <errno.h>
#include <limits.h> /* INT_MIN, INT_MAX */
#include <stdlib.h> /* strtol */

#include <libtsp.h>
#include <libtsp/nucleus.h>
#include <libtsp/STmsg.h>

#define MAXC  23


int
STdec1int (const char String[], int *Ival)

{
  long int lv;
  char *endstr;

  /* Trim initial white-space */
  String = STtrimIws (String);
  if (String[0] == '\0') {
    UTwarn ("STdec1int - %s", STM_EmptyData);
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
    UTwarn ("STdec1int - %s: \"%s\"", STM_DataErr, STstrDots (String, MAXC));
    return 1;
  }

  if (errno == ERANGE || lv < INT_MIN || lv > INT_MAX) {
    UTwarn ("STdec1int - %s: \"%s\"", STM_InvVal, STstrDots (String, MAXC));
    return 2;
  }

  /* Success. return the decoded value */
  *Ival = (int) lv;

  return 0;
}
