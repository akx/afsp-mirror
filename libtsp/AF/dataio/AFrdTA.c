/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int AFrdTA (AFILE *AFp, float Fbuff[], int Nreq)

Purpose:
  Read samples (text data) from an audio file (return float values)

Description:
  This routine reads a specified number of samples from an audio file.  The
  data in the file is converted to float format on output. The file must have
  been opened using subroutine AFopenRead.

  The data is expected to be one value to a line.

Parameters:
  <-  int AFrdTA
      Number of data values transferred from the file.  On reaching the end of
      the file, this value may be less than Nreq.
   -> AFILE *AFp
      Audio file pointer for an audio file opened by AFopenRead
  <-  float Fbuff[]
      Array of floats to receive the samples
   -> int Nreq
      Number of samples requested.  Nreq may be zero.

Author / revision:
  P. Kabal  Copyright (C) 1999
  $Revision: 1.18 $  $Date: 1999/06/11 22:34:11 $

-------------------------------------------------------------------------*/

static char rcsid [] = "$Id: AFrdTA.c 1.18 1999/06/11 AFsp-v6r8 $";

#include <ctype.h>
#include <stdlib.h>	/* strtod definition */

#include <libtsp.h>
#include <libtsp/AFdataio.h>
#include <libtsp/AFmsg.h>
#include <libtsp/AFpar.h>

#define MINV(a, b)	(((a) < (b)) ? (a) : (b))


int
AFrdTA (AFILE *AFp, float Fbuff[], int Nreq)

{
  char *line;
  int n, ErrCode;
  double Dv;
  char *endstr;
  double g;

/*
   This routine spends most of its time in strtod (sscanf is even slower)
   Tests:  24 Mb file with 4.7M samples
   - read data, copy to another file, 43 sec CPU
   - read data (strtod commented out), copy to another file, 16 sec CPU
*/

/* Read the data */
  ErrCode = 0;
  g = AFp->ScaleF;
  for (n = 0; n < Nreq; ++n) {
    line = AFgetLine (AFp->fp, &ErrCode);	/* prints error messages */
    if (line == NULL || ErrCode)
      break;
    Dv = strtod (line, &endstr);
    for (; isspace ((int)(*endstr)); ++endstr)
      ;
    if (endstr[0] != '\0') {
      UTwarn ("AFrdTA - %s", AFM_DataErr);
      ErrCode = AF_DECERR;
      break;
    }
    Fbuff[n] = (float) (g * Dv);
  }

/* Check for errors */
  if (ErrCode)
    AFp->Error = ErrCode;

  return n;
}
