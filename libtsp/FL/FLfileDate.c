/*------------- Telecommunications & Signal Processing Lab -------------
                          McGill University

Routine:
  char *FLfileDate (FILE *fp, int format)

Purpose:
  Find the last modification time for an open file

Description:
  This routine finds the last modified time for a file specified by the file
  pointer to an open file.  This time is returned as a date/time string in one
  of a number of standard formats.

  Format 0 and Format 1:
    Standard C-language formats (without the trailing newline character).
    Format 1 includes the time zone abbreviation.  Formats 0 and 1 use
    abbreviations for the day of the week and the month.
  Formats 2 and  3:
    These formats adhere to the ISO 8601 standard, avoiding language dependent
    names (except for the time-zone code).
  Format 4:
    This formats adheres to the ISO 8601 standard using Z to indicate UTC.

    Format  Example                       time zone    typical length
      0     Sun Sep 16 01:03:52 1973      local time   24 + null
      1     Sun Sep 16 01:03:52 EST 1973  local time   28* + null
      2     1994-01-23 09:59:53 EST       local time   23* + null
      3     1994-01-23 14:59:53 UTC       GMT          23 + null
      4     1994-01-23 14:59:53 Z         GMT          21 + null
                                          (*) the time zone length can vary

Parameters:
  <-  char *FLfileDate
      Pointer to a character string for the date and time.  This is a pointer
      to an internal static storage area; each call to this routine overlays
      this storage.
   -> FILE *fp
      File pointer for the file
   -> int format
      Date / time format code, taking on values from 0 to 3

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.28 $  $Date: 2017/06/09 11:06:22 $

----------------------------------------------------------------------*/

#include <libtsp/sysOS.h>
#if (SY_OS == SY_OS_WINDOWS)
#  define _CRT_NONSTDC_NO_DEPRECATE   /* Allow Posix names */
#endif

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <libtsp.h>
#include <libtsp/nucleus.h>

#define MAXDATE 128


char *
FLfileDate (FILE *fp, int format)

{
  struct stat Fstat;
  int status;
  static char Datetime[MAXDATE+1];

  status = fstat (fileno (fp), &Fstat);
  if (status == 0)
    STcopyMax (UTctime (&Fstat.st_mtime, format), Datetime, MAXDATE);
  else
    Datetime[0] = '\0';

  return Datetime;
}
