/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void UTwarn (const char Warnmsg[], ...)

Purpose:
  Print a warning message

Description:
  This routine prints a warning message on stderr (standard error).  An example
  of the use of this routine is as follows.
    UTwarn ("XXProc - Output data clipped (%d values(s))", N);

Parameters:
   -> const char Warnmsg[]
      Character string to be printed.  This string can contain optional
      formatting codes.  The arguments corresponding to the formatting codes
      appear at the end of the argument list.  The input string should not
      normally have a terminating newline character, since this routine
      supplies a newline.
  ->  <args...>
      Arguments corresponding to the formatting codes.  The format string and
      the variable number of arguments is passed on to the system routine
      vprintf.

Author / revision:
  P. Kabal  Copyright (C) 1999
  $Revision: 1.21 $  $Date: 1999/06/04 22:41:07 $

-------------------------------------------------------------------------*/

static char rcsid[] = "$Id: UTwarn.c 1.21 1999/06/04 AFsp-v6r8 $";

#include <stdarg.h>		/* ANSI C variable-length argument list */

#include <libtsp.h>


void
UTwarn (const char Warnmsg[], ...)

{
  va_list ap;

  va_start (ap, Warnmsg);

/* Print the warning message */
  vfprintf (stderr, Warnmsg, ap);
  fprintf (stderr, "\n");

  va_end (ap);

  return;
}
