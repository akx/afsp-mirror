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
  P. Kabal  Copyright (C) 2001
  $Revision: 1.12 $  $Date: 2001/10/09 02:34:54 $

----------------------------------------------------------------------*/

static char rcsid[] = "$Id: FLterm.c 1.12 2001/10/09 AFsp-v6r8 $";

#include <stdio.h>	/* fileno */

#include <libtsp/nucleus.h>
#include <libtsp/sysOS.h>

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
