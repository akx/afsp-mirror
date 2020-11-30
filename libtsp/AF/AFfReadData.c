/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int AFfReadData(AFILE *AFp, long int offs, float Dbuff[], int Nreq)

Purpose:
  Read data from an audio file (return float values)

Description:
  This routine returns a specified number of samples at a given sample offset
  in an audio file. The data in the file is converted to float format on output.
  The sample data in the file is considered to be preceded and followed by
  zero-valued samples. Thus if the sample offset is negative or points to beyond
  the number of samples in the file, the appropriate number of zero-valued
  samples is returned. The file must have been opened using routine AFopnRead.

  This routine returns float data. This format gives 24 bits of precision. A
  loss of precision is possible if the input file contains double or 32-bit
  integer data.

  The following program fragment illustrates the use of this routine to read
  overlapping blocks of data. For the simpler case of sequential access to the
  data without overlap, the variable Lmem should be set to zero.
      float Dbuff[320]'
       ...
      AFp = AFopnRead(...);
       ...
      Lmem = 80;
      Lblock = 320;
      Nadv = Lblock - Lmem;
      offs = -Lmem;
      while (1) {
        Nout = AFfReadData(AFp, offs, Dbuff, Lblock);
        offs = offs + Nadv;
        if (Nout <= 0)
          break;
         ...
      }

  On encountering an error, the default behaviour is to print an error message
  and halt execution.

Parameters:
  <-  int AFfReadData
      Number of data values transferred from the file. On reaching the end of
      the file, this value may be less than Nreq, in which case the last
      elements are set to zero. This value can be used by the calling routine
      to determine when the data from the file has been exhausted.
   -> AFILE *AFp
      Audio file pointer for an audio file opened by AFopnRead
   -> long int offs
      Offset into the file in samples. If offs is positive, the first value
      returned is offs samples from the beginning of the data. The file data is
      considered to be preceded by zeros. Thus if offs is negative, the
      appropriate number of zeros will be returned. These zeros before the
      beginning of the data are counted as part of the count returned in Nout.
  <-  float Dbuff[]
      Array of floats to receive Nreq samples. The data is organized as
      sequential frames of samples, where each frame contains samples for each
      of the channels.
   -> int Nreq
      Number of samples requested. Nreq may be zero.

Author / revision:
  P. Kabal  Copyright (C) 2020
  $Revision: 1.15 $  $Date: 2020/11/26 11:21:31 $

-------------------------------------------------------------------------*/

#include <assert.h>
#include <stdlib.h>   /* EXIT_FAILURE */

#include <libtsp.h>
#include <AFpar.h>
#include <libtsp/AFdataio.h>
#include <libtsp/AFmsg.h>

#define MINV(a, b)  (((a) < (b)) ? (a) : (b))

/* Reading routines */
static int
(*AF_Read[AF_NFD])(AFILE *AFp, float Dbuff[], int Nreq) =
                   {NULL,     AFfRdAlaw, AFfRdMulaw, AFfRdMulawR,
                    AFfRdU1,  AFfRdI1,   AFfRdI2,    AFfRdI3,
                    AFfRdI4,  AFfRdF4,   AFfRdF8,    AFfRdTA,
                    AFfRdTA};

/*
  The option flag ErrorHalt affects error handling.
  If ErrorHalt is clear, execution continues after an error
   - If fewer than the requested number of elements are returned. To distinguish
     between an error and and end-of-file, AFp->Error must be examined.
   - An unexpected end-of-file is an error
   - AFp->Error must be zero on input to this routine
*/


int
AFfReadData(AFILE *AFp, long int offs, float Dbuff[], int Nreq)

{
  int i, Nv, Nr, Nout;

/* Check the operation  */
  assert(AFp->Op == FO_RO);
  assert(!AFp->Error);
  assert(AF_Read[AF_NFD-1] != NULL);
  assert(AFp->Format > 0 && AFp->Format < AF_NFD);

/* Fill in zeros at the beginning of data */
  if (offs < 0) {
    Nout = MINV(-offs, Nreq);
    for (i = 0; i < Nout; ++i)
      Dbuff[i] = 0;
    offs += Nout;
  }
  else
    Nout = 0;

/* Position the file */
  AFp->Error = AFposition(AFp, offs);

/* The file reading routines expect that the file is positioned at the data to
   be read. They use the following AFp fields:
     AFp->fp - file pointer
     AFp->Swapb - data swap indicator
     AFp->ScaleF - data scaling factor
  An error is detected on the outside by calling ferror() or by checking
  AFp->Error (for text data files).

  Errors:  Nr < Nreq - Nb  && ferror or AFp->Error set
  EOF:     Nr < Nreq - Nb  && ferror and AFp->Error not set

  This routine updates the following AFp values
    AFp->Error - Set for an error
    AFp->Isamp - Current data sample. This value is set to the current position
      before reading and incremented by the number of samples read.
    AFp->Nsamp - Number of samples (updated if not defined initially and EOF
      is detected)
*/

/* Transfer data from the file */
  if (AFp->Nsamp == AF_NSAMP_UNDEF)
    Nv = Nreq - Nout;
  else if (offs < 0)
    Nv = 0;
  else
    Nv = (int) MINV(Nreq - Nout, AFp->Nsamp - offs);     /* offs >= 0 */

  if (!AFp->Error && Nv > 0) {
    Nr = (*AF_Read[AFp->Format])(AFp, &Dbuff[Nout], Nv);
    Nout += Nr;
    AFp->Isamp += Nr;

/* Check for errors */
    if (Nr < Nv) {
      if (ferror(AFp->fp)) {
        UTsysMsg("AFfReadData - %s %ld", AFM_ReadErrOffs, AFp->Isamp);
        AFp->Error = AF_IOERR;
      }
      else if (AFp->Error)
        UTwarn("AFfReadData - %s %ld", AFM_ReadErrOffs, AFp->Isamp);
      else if (AFp->Nsamp != AF_NSAMP_UNDEF) {
        UTwarn("AFfReadData - %s %ld", AFM_UEoFOffs, AFp->Isamp);
        AFp->Error = AF_UEOF;
      }
      else
        AFp->Nsamp = AFp->Isamp;
    }
  }

/* Zeros at the end of the file */
  for (i = Nout; i < Nreq; ++i)
    Dbuff[i] = 0;

  if (AFp->Error && AFopt.ErrorHalt)
    exit(EXIT_FAILURE);

  return Nout;
}
