/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int AFseek (FILE *fp, long int pos, int *ErrCode)

Purpose:
  Seek to a position in a file

Description:
  This routine seeks to a given position in a file.

Parameters:
  <-  int AFseek
      Error status
   -> FILE *fp
      File pointer associated with the file
   -> long int pos
      Position in the file relative to the start of the file.  If pos is
      equal to AF_SEEK_END, the position is set to the end-of-file.
  <-> int ErrCode
      Error code.  This value is set if an error is detected, but otherwise
      remains unchanged.  If ErrCode is set on input, this routine is a no-op.
      ErrCode set to a NULL pointer is equivalent to ErrCode not being set.
      In this case, the error status is in the function return value.

Author / revision:
  P. Kabal  Copyright (C) 1999
  $Revision: 1.7 $  $Date: 1999/06/04 19:57:35 $

-------------------------------------------------------------------------*/

static char rcsid[] = "$Id: AFseek.c 1.7 1999/06/04 AFsp-v6r8 $";

#include <libtsp.h>
#include <libtsp/AFdataio.h>
#include <libtsp/AFmsg.h>
#include <libtsp/AFpar.h>


int
AFseek (FILE *fp, long int pos, int *ErrCode)

{
  int status;

  if (ErrCode == NULL)
    status = 0;
  else
    status = *ErrCode;

  if (! status) {
    if (pos == AF_SEEK_END)
      status = fseek (fp, 0L, SEEK_END);
    else
      status = fseek (fp, pos, SEEK_SET);
    if (status) {
      UTwarn ("AFseek: %s", AFM_FilePosErr);
    }

    if (status)
      status = AF_IOERR;
  }
  if (ErrCode != NULL)
    *ErrCode = status;

  return status;
}
