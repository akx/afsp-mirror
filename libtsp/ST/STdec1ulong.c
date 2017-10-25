/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int STdec1ulong (const char String[], unsigned long int *Uval)

Purpose:
  Decode an unsigned long integer value

Description:
  This routine decodes a string containing a numeric value.  The decoded datum
  is stored as a unsigned long integer value.  If a decoding error is detected,
  a warning message is printed and an error status value is returned.  A warning
  message is printed if extra data follows the requested value (this data is
  ignored).

Parameters:
  <-  int STdec1ulong
      Error status,
        0 - no error
        1 - error: data format error
        2 - error: data value out of range
   -> const char String[]
      Input string
  <-  unsigned long int *Uval
      Returned value.  This value is not changed if an error status is returned.

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.2 $  $Date: 2017/05/24 20:16:25 $

-------------------------------------------------------------------------*/

#include <errno.h>
#include <stdlib.h> /* strtol, strtoul */

#include <libtsp.h>
#include <libtsp/nucleus.h>
#include <libtsp/STmsg.h>

#define MAXC  23


int
STdec1ulong (const char String[], unsigned long int *Uval)

{
  long int lv;
  unsigned long int uv;
  char *endstr;

  /* Trim initial white-space */
  String = STtrimIws (String);
  if (String[0] == '\0') {
    UTwarn ("STdec1ulong - %s", STM_EmptyData);
    return 1;
  }

  uv = 0;     /* stop compiler warning */
  errno = 0;

  /* The case of unsigned long is problematic.  strtoul allows negative inputs
     which are converted to a positive value by adding the appropriate power of
     two.  The strategy here is
     (1) convert to a long int
     (2) if negative or a negative overflow, this is a format error for unsigned
     (3) if no positive overflow, copy the long int to unsigned long int
     (4) otherwise use strtoul on the string again, yielding a positive number
     (5) check for overflow
  */
  lv = strtol (String, &endstr, 10);

  /* A decoding error is marked by
  (1) Empty String (handled above)
  (2) endstr pointing to String (which has been trimmed of leading white-space)
  (3) More non-white-space characters following the decoded string
  (4) A negative return value (includes a negative overflow)
  */
  endstr = STtrimIws (endstr);        /* Trim leading white-space on endstr */
  if (endstr[0] != '\0') {
    /* Data format error */
    UTwarn ("STdec1ulong - %s: \"%s\"", STM_DataErr, STstrDots (String, MAXC));
    return 1;
  }

  /* Check for a negative value or negative overflow value */
  if (lv < 0) {
    UTwarn ("STdec1ulong - %s: \"%s\"", STM_InvVal, STstrDots (String, MAXC));
    return 2;
  }

  if (errno != ERANGE)
    uv = (unsigned long int) lv;
  else {                                  /* Positive overflow from strtol */
    errno = 0;                            /* Reset the error code */
    uv = strtoul (String, &endstr, 10);   /* Rescan with strtoul */
    if (errno == ERANGE) {
      UTwarn ("STdec1ulong - %s: \"%s\"", STM_InvVal, STstrDots (String, MAXC));
      return 2;
    }
  }

  /* Success. return the decoded value */
  *Uval = uv;

  return 0;
}
