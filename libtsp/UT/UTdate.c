/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  char *UTdate (int format)

Purpose:
  Return a date/time string for the current time

Description:
  This routine returns a date/time string in a number of standard formats.
  Formats 0, 1 and 2 are in local time.  Format 3 gives the date and time in
  GMT (Universal Coordinated Time).

  Format 0 and Format 1:
    Standard C-language formats (without the trailing newline character).
    Format 1 includes the time zone abbreviation.  Formats 0 and 1 use
    abbreviations for the day of the week and the month.
  Formats 2 and  3:
    These formats adhere to the ISO 8601 standard, avoiding language dependent
    names (except for the time-zone code).
  Format 4:
    This formats adheres to the ISO 8601 standard using Z to indicate UTC.

        Format  Example                     time zone    typical length
          0   Sun Sep 16 01:03:52 1973      local time   24 + null
          1   Sun Sep 16 01:03:52 EST 1973  local time   28* + null
          2   1994-01-23 09:59:53 EST       local time   23* + null
          3   1994-01-23 14:59:53 UTC       GMT          23 + null
          4   1994-01-23 14:59:53 Z         GMT          21 + null
                                            (*) the time zone length can vary

Parameters:
  <-  char *UTdate
      Pointer to a character string for the date and time.  This is a pointer
      to an internal static storage area; each call to this routine overlays
      this storage.
   -> int format
      Date / time format code, taking on values from 0 to 3

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.20 $  $Date: 2017/06/09 12:56:04 $

-------------------------------------------------------------------------*/

#include <time.h>

#include <libtsp.h>
#include <libtsp/nucleus.h>

#define MAXDATE 64


char *
UTdate (int format)

{
  static char Datetime[MAXDATE+1];
  time_t tnow;

  tnow = time ((time_t *) NULL);
  STcopyMax (UTctime (&tnow, format), Datetime, MAXDATE);

  return Datetime;
}
