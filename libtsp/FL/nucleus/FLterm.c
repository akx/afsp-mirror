/*------------ Telecommunications & Signal Processing Lab -------------
                         McGill University

Routine:
  int FLterm (FILE *fp)

Purpose:
  Determine if a file pointer is associated with a terminal

Description:
  This routine determines if a file specified by its file pointer is associated
  with a terminal device.

Parameters:
  <-  int FLterm
      Return value, 1 for a terminal, 0 otherwise
   -> FILE *fp
      File pointer

Author / revision:
  P. Kabal  Copyright (C) 2009
  $Revision: 1.15 $  $Date: 2009/03/01 21:07:00 $

----------------------------------------------------------------------*/

#include <libtsp/sysOS.h>
#ifdef SY_OS_WINDOWS
#  define _CRT_NONSTDC_NO_DEPRECATE   /* Allow Posix names */
#endif

#include <stdio.h>	/* fileno (Section 8.2.1.1 of Posix) */

#include <libtsp/nucleus.h>

#if (SY_POSIX)
#  include <unistd.h>	/* isatty */
#else
#  include <io.h>
#endif


int
FLterm (FILE *fp)

{
  return (isatty (fileno (fp)) != 0);
}
