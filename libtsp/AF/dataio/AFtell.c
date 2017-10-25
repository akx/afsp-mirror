/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  long int AFtell (FILE *fp, enum AF_ERR_T *ErrCode)

Purpose:
  Determine the position in a file

Description:
  This routine returns the position in a file.  For binary files this will be
  the number of bytes from the beginning of the file.  For non-binary files,
  the returned value does not necessarily have this interpretation.  However,
  in all cases the returned value can be used as an argument to AFseek to
  return to the same position.

Parameters:
  <-  long int AFtell
      Position in the file, set to -1 for an error
   -> FILE *fp
      File pointer associated with the file
  <-> enum AF_ERR_T ErrCode
      Error code.  If ErrCode is initially set, this routine returns -1.
      The error code is set if an error is detected, but otherwise remains
      unchanged.

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.13 $  $Date: 2017/06/28 23:51:20 $

-------------------------------------------------------------------------*/

#include <errno.h>

#include <libtsp.h>
#include <AFpar.h>
#include <libtsp/AFdataio.h>
#include <libtsp/AFmsg.h>


long int
AFtell (FILE *fp, enum AF_ERR_T *ErrCode)

{
  long int pos;

  pos = -1L;
  if (! *ErrCode) {
    errno = 0;
    pos = ftell (fp);
    if (pos == -1L && errno) {
      UTsysMsg ("AFtell: %s", AFM_NoFilePos);
      *ErrCode = AF_IOERR;
    }
  }

  return pos;
}
