/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void UThalt (const char Errmsg[], ...)

Purpose:
  Print an error message, stop with error status set

Description:
  This routine prints an error message to stderr (standard error) and then
  terminates execution with exit status set to EXIT_FAILURE.  An example of the
  use of this routine is as follows.
    UThalt ("%s: Invalid option", PROGRAM);

Parameters:
   -> const char Errmsg[]
      Character string to be printed.  This string can contain optional
      formatting codes.  The arguments corresponding to the formatting codes
      appear at the end of the argument list.  The input string should not
      normally have a terminating newline character, since this routine
      supplies a newline.
   -> <args...>
      Arguments corresponding to the formatting codes.  The format string and
      the variable number of arguments is passed on to the system routine
      vprintf.

Author / revision:
  P. Kabal  Copyright (C) 1999
  $Revision: 1.21 $  $Date: 1999/06/04 22:41:07 $

-------------------------------------------------------------------------*/

static char rcsid[] = "$Id: UThalt.c 1.21 1999/06/04 AFsp-v6r8 $";

#include <stdarg.h>
#include <stdlib.h>	/* EXIT_FAILURE */

#include <libtsp.h>


void
UThalt (const char Errmsg[], ...)

{
  va_list ap;

  va_start (ap, Errmsg);

/* Print the error message */
  vfprintf (stderr, Errmsg, ap);
  fprintf (stderr, "\n");

  va_end (ap);

  exit (EXIT_FAILURE);
}
