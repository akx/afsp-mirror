/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int AFreadData (AFILE *AFp, long int offs, float Fbuff[], int Nreq)

Purpose:
  Read data from an audio file (return float values)

Description:
  This routine returns a specified number of samples at a given sample offset
  in an audio file.  The data in the file is converted to float format on
  output.  The sample data in the file is considered to be preceded and
  followed by zero-valued samples.  Thus if the sample offset is negative or
  points to beyond the number of samples in the file, the appropriate number
  of zero-valued samples is returned.  The file must have been opened using
  routine AFopenRead.

  The following program fragment illustrates the use of this routine to read
  overlapping frames of data.  For the simpler case of sequential access to the
  data without overlap, the variable Lmem should be set to zero.

    AFp = AFopenRead (...);
    ...
    Lmem =...
    Lframe =...
    Nadv = Lframe-Lmem;
    offs = -Lmem;
    while (1) {
      Nout = AFreadData (AFp, offs, Fbuff, Lframe);
      offs = offs+Nadv;
      if (Nout == 0)
        break;
      ...
    }

  On encountering an error, the default behaviour is to print an error message
  and halt execution.

Parameters:
  <-  int AFreadData
      Number of data values transferred from the file.  On reaching the end of
      the file, this value may be less than Nreq, in which case the last
      elements are set to zero.  This value can be used by the calling routine
      to determine when the data from the file has been exhausted.
   -> AFILE *AFp
      Audio file pointer for an audio file opened by AFopenRead
   -> long int offs
      Offset into the file in samples.  If offs is positive, the first value
      returned is offs samples from the beginning of the data.  The file data
      is considered to be preceded by zeros.  Thus if offs is negative, the
      appropriate number of zeros will be returned.  These zeros before the
      beginning of the data are counted as part of the count returned in Nout.
  <-  float Fbuff[]
      Array of floats to receive the Nreq samples
   -> int Nreq
      Number of samples requested.  Nreq may be zero.

Author / revision:
  P. Kabal  Copyright (C) 2002
  $Revision: 1.48 $  $Date: 2002/02/15 03:25:35 $

-------------------------------------------------------------------------*/

static char rcsid[] = "$Id: AFreadData.c 1.48 2002/02/15 AFsp-v6r8 $";

#include <assert.h>
#include <stdlib.h>		/* EXIT_FAILURE */
#include <string.h>

#include <libtsp.h>
#include <libtsp/nucleus.h>
#include <libtsp/AFdataio.h>
#include <libtsp/AFmsg.h>
#define AF_DATA_LENGTHS
#include <libtsp/AFpar.h>

#define MINV(a, b)	(((a) < (b)) ? (a) : (b))
#define MAXV(a, b)	(((a) > (b)) ? (a) : (b))

#define AFSEEK(AFp,boff)	AFseek ((AFp)->fp, (AFp)->Start + (boff), NULL)

/* Reading routines */
static int
(*AF_Read[NFD])(AFILE *AFp, float Fbuff[], int Nreq) =
  { NULL,	AFrdMulaw,	AFrdAlaw,	AFrdU1,		AFrdI1,
    AFrdI2,	AFrdI3,		AFrdI4,		AFrdF4,		AFrdF8,
    AFrdTA };

static int
AF_position (AFILE *AFp, long int offs);
static int
AF_skipNRec (AFILE *AFp, long int N);
static int
AF_skipNVal (AFILE *AFp, long int N);

/*
  The option flag ErrorHalt affects error handling.
  If ErrorHalt is clear, execution continues after an error
   - Fewer than requested elements are returned.  To distinguish between an
     error and and end-of-file, AFp->Error must be examined.
   - An unexpected end-of-file is an error
   - AFp->Error must be zero on input to this routine
*/


int
AFreadData (AFILE *AFp, long int offs, float Fbuff[], int Nreq)

{
  int i, Nb, Nv, Nr, Nout;

/* Check the operation  */
  assert (AFp->Op == FO_RO);
  assert (! AFp->Error);

/* Fill in zeros at the beginning of data */
  Nb = (int) MAXV (0, MINV (-offs, Nreq));
  for (i = 0; i < Nb; ++i) {
    Fbuff[i] = 0.0F;
    ++offs;
  }
  Nout = Nb;

/* Position the file */
  if (offs != AFp->Isamp && offs >= 0L &&
      (AFp->Nsamp == AF_NSAMP_UNDEF || offs < AFp->Nsamp))
    AFp->Error = AF_position (AFp, offs);

/* The file reading routines read data from the file and return the data as
   scaled float values.  They expect that the file is positioned at the data
   to be read.  They use the following AFp fields:
     AFp->fp - file pointer
     AFp->Swapb - data swap indicator
     AFp->ScaleF - data scaling factor
  An error is detected on the outside by calling ferror() or by checking
  AFp->Error (for text data files).

  Errors:  Nr < Nreq - Nb  && ferror or AFp->Error set
  EOF:     Nr < Nreq - Nb  && ferror and AFp->Error not set

  This routine updates the following AFp values
    AFp->Error - Set for an error
    AFp->Isamp - Current data sample.  This value is set to the current
      position before reading and incremented by the number of samples read.
    AFp->Nsamp - Number of samples (updated if not defined initially and EOF
      is detected)
*/

/* Transfer data from the file */
  if (AFp->Nsamp == AF_NSAMP_UNDEF)
    Nv = Nreq - Nout;
  else
    Nv = (int) MINV (Nreq - Nout, AFp->Nsamp - offs);

  if (! AFp->Error && Nv > 0) {
    Nr = (*AF_Read[AFp->Format]) (AFp, &Fbuff[Nb], Nv);
    Nout += Nr;
    AFp->Isamp += Nr;

/* Check for errors */
    if (Nr < Nv) {
      if (ferror (AFp->fp)) {
	UTsysMsg ("AFreadData - %s %ld", AFM_ReadErrOffs, AFp->Isamp);
	AFp->Error = AF_IOERR;
      }
      else if (AFp->Error)
	UTwarn ("AFreadData - %s %ld", AFM_ReadErrOffs, AFp->Isamp);
      else if (AFp->Nsamp != AF_NSAMP_UNDEF) {
	UTwarn ("AFreadData - %s %ld", AFM_UEoFOffs, AFp->Isamp);
	AFp->Error = AF_UEOF;
      }
      else
	AFp->Nsamp = AFp->Isamp;
    }
  }

/* Zeros at the end of the file */
  for (i = Nout; i < Nreq; ++i)
    Fbuff[i] = 0.0F;

  if (AFp->Error && (AFoptions ())->ErrorHalt)
    exit (EXIT_FAILURE);
  return Nout;
}

/* Position the file to sample offset offs
   NB: Assumes:  offs >= 0, offs < AFp->Nsamp (if AFp->Nsamp is defined)
*/

static int
AF_position (AFILE *AFp, long int offs)

{
  int Lw, ErrCode;

/*
  Notes:
  offs  < 0           Nop    (Must be checked outside!)
        < AFp->Isamp  seek
       == AFp->Isamp  Nop
        < AFp->Nsamp  seek or read
       >= AFp->Nsamp  Nop    (Must be checked outside!)
  AFp->Nsamp == AF_NSAMP_UNDEF, the file is not random access
                (on encountering EOF, the value of AFp->Nsamp is set)
             == value, can be either random access or not
*/
  ErrCode = 0;
  Lw = AF_DL[AFp->Format];
  if (Lw > 0) {

/* Fixed length records */
    if (FLseekable (AFp->fp)) {
      ErrCode = AFSEEK (AFp, Lw * offs);
      if (! ErrCode)
	AFp->Isamp = offs;
    }
    else if (offs < AFp->Isamp) {
      UTwarn ("AFeadData: %s", AFM_MoveBack);
      ErrCode = AF_IOERR;
    }
    else
      ErrCode = AF_skipNVal (AFp, offs - AFp->Isamp);
  }

  else {

/* Variable length (text) records */
    if (offs < AFp->Isamp) {
      if (FLseekable (AFp->fp)) {
	ErrCode = AFSEEK (AFp, 0L);	/* Rewinding to start of data */
	if (! ErrCode)
	  AFp->Isamp = 0L;
      }
      else {
	UTwarn ("AFreadData: %s", AFM_MoveBack);
	ErrCode = AF_IOERR;
      }
    }

    /* Move forward to the desired position */
    if (! ErrCode)
      ErrCode = AF_skipNRec (AFp, offs - AFp->Isamp);
  }

  return ErrCode;
}

/* Skip N values */

#define NBUF	256

#define FREAD(buf,size,nv,fp)	(int) fread ((char *) buf, (size_t) size, \
					     (size_t) nv, fp)

static int
AF_skipNVal (AFILE *AFp, long int N)

{
  long int nl;
  int Lw, Nv, Nreq, ErrCode;
  double Buf[NBUF];

  Lw = AF_DL[AFp->Format];

  /* Read N values  */
  nl = 0L;
  while (nl < N) {
    Nreq = (int) MINV ((int) (sizeof Buf) / Lw, N - nl);
    Nv = FREAD (Buf, Lw, Nreq, AFp->fp);
    nl += Nv;
    if (Nv < Nreq)
      break;
  }
  AFp->Isamp += nl;

  ErrCode = 0;
  if (nl < N) {
    if (ferror (AFp->fp)) {
      UTsysMsg ("AFreadData: %s %ld", AFM_ReadErrOffs, AFp->Isamp);
      ErrCode = AF_IOERR;
    }
    else if (AFp->Nsamp != AF_NSAMP_UNDEF) {
      UTwarn ("AFreadData: %s %ld", AFM_UEoFOffs, AFp->Isamp);
      ErrCode = AF_UEOF;
    }
    else
      AFp->Nsamp = AFp->Isamp;
  }

  return ErrCode;
}

/* Skip N records */


static int
AF_skipNRec (AFILE *AFp, long int N)

{
  char *p;
  int ErrCode;
  long int nl;

  /* Read N lines of input */
  ErrCode = 0;
  for (nl = 0; nl < N; ++nl) {
    p = AFgetLine (AFp->fp, &ErrCode);
    if (p == NULL || AFp->Error)
      break;
  }
  AFp->Isamp += nl;

  if (nl < N) {
    if (ErrCode)
      UTwarn ("AFreadData: %s %ld", AFM_ReadErrOffs, AFp->Isamp);
    else if (AFp->Nsamp != AF_NSAMP_UNDEF) {
      UTwarn ("AFreadData: %s %ld", AFM_UEoFOffs, AFp->Isamp);
      ErrCode = AF_UEOF;
    }
    else
      AFp->Nsamp = AFp->Isamp;
  }

  return ErrCode;
}
