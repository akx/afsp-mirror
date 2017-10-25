/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int AFdRdTA (AFILE *AFp, double Dbuff[], int Nreq)

Purpose:
  Read samples (text data) from an audio file (return double values)

Description:
  These routines read a specified number of samples from an audio file.  The
  data in the file is converted to double format on output.

  This routines reads values until (i) Nreq values have been decoded, or (ii)
  end-of-file is reached, or (iii) a decoding error is encountered.

  If the input file contains values larger in magnitude than a double can
  represent, the decoded values are set to HUGE_VAL with the appropriate sign.
  In any implementations of the C-library, HUGE_VAL is infinity.  If the input
  file contains values smaller in magnitude than a double can represent, the
  decoded values are set to zero.

Parameters:
  <-  int AFdRdTA
      Number of data values transferred from the file.  On reaching the end of
      the file, this value may be less than Nreq.
   -> AFILE *AFp
      Audio file pointer for an audio file opened by AFopnRead
  <-  double Dbuff[]
      Array of doubles to receive the samples
   -> int Nreq
      Number of samples requested.  Nreq may be zero.

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.12 $  $Date: 2017/06/09 15:05:06 $

-------------------------------------------------------------------------*/

#include <libtsp/sysOS.h>
#if (SY_OS == SY_OS_WINDOWS)
#  define _CRT_SECURE_NO_WARNINGS     /* Allow fscanf */
#endif

#include <errno.h>
#include <float.h>

#include <libtsp.h>
#include <AFpar.h>
#include <libtsp/AFdataio.h>


int
AFdRdTA (AFILE *AFp, double Dbuff[], int Nreq)

{
  int nv, n;
  enum AF_ERR_T ErrCode;
  double Dv;

  ErrCode = AF_NOERR;

/* Read the data */

  for (n = 0; n < Nreq; ++n) {
    errno = 0;
    nv = fscanf (AFp->fp, "%lg", &Dv);
    if (nv == 1)
      Dbuff[n] = AFp->ScaleF * Dv;
    else if (nv == EOF)
      break;
    else if (errno != 0) {
      ErrCode = AF_IOERR;
      break;
    }
    else {
      ErrCode = AF_DECERR;
      break;
    }
  }

/* Check for errors */
  if (ErrCode)
    AFp->Error = ErrCode;

  return n;
}
