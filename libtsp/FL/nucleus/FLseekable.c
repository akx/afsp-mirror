/*------------ Telecommunications & Signal Processing Lab -------------
                         McGill University

Routine:
  int FLseekable (FILE *fp)

Purpose:
  Determine if an I/O stream is random access.

Description:
  This routine determines if an I/O stream associated with a given file pointer
  is random access.

Parameters:
  <-  int FLseekable
      Return value, 1 if the file is random access, 0 otherwise
   -> FILE *fp
      File pointer

Author / revision:
  P. Kabal  Copyright (C) 2001
  $Revision: 1.9 $  $Date: 2001/12/10 22:33:33 $

----------------------------------------------------------------------*/

static char rcsid[] = "$Id: FLseekable.c 1.9 2001/12/10 AFsp-v6r8 $";

#include <stdio.h>	/* fileno */
#include <sys/types.h>
#include <sys/stat.h>

#include <libtsp/nucleus.h>

#ifndef S_ISREG		/* Defined by POSIX */
#  define S_ISREG(m)	(((m) & S_IFMT) == S_IFREG)
#endif


int
FLseekable (FILE *fp)

{
  struct stat Fstat;
  int status;

  status = fstat (fileno (fp), &Fstat);
  if (status == 0 && S_ISREG (Fstat.st_mode))
    return 1;
  else
    return 0;
}
