/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  enum AF_ERR_T AFposition (AFILE *AFp, long int offs)

Purpose:
  Position an audio file to a given sample offset

Description:
  This routine positions a file to a given position.

Parameters:
  <-  enum AF_ERR_T AFposition
      Error indication, zero for no error
   -> AFILE *AFp
      Audio file pointer for an audio file opened by AFopnRead
   -> long int offs
      Offset into the file in samples.  If offs is negative or beyond the
      end-of-file, no action is taken. If offs is beyond the current file
      position, the position is moved to offs either by seeking (random access
      binary file) or by reading (non-random access or text data). If offset
      is before offs, move by seeking (random access binary file) or by
      rewinding and reading (random access text file).

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.12 $  $Date: 2017/06/09 15:05:31 $

-------------------------------------------------------------------------*/

#include <libtsp/sysOS.h>
#if (SY_OS == SY_OS_WINDOWS)
#  define _CRT_SECURE_NO_WARNINGS     /* Allow fscanf */
#endif

#include <errno.h>
#include <string.h>

#include <libtsp.h>
#include <AFpar.h>
#include <libtsp/nucleus.h>
#include <libtsp/AFdataio.h>
#include <libtsp/AFmsg.h>

#define AFSEEK(AFp,boff)  AFseek ((AFp)->fp, (AFp)->Start + (boff), NULL)

/* Local functions */
static enum AF_ERR_T
AF_skipNFields (AFILE *AFp, long int N);
static enum AF_ERR_T
AF_skipNVal (AFILE *AFp, long int N);


int
AFposition (AFILE *AFp, long int offs)

{
  int Lw;
  enum AF_ERR_T ErrCode;

/*
  Notes:
  offs  < 0           Nop
        < AFp->Isamp  seek
       == AFp->Isamp  Nop
        < AFp->Nsamp  seek or read
       >= AFp->Nsamp  Nop
  AFp->Nsamp == AF_NSAMP_UNDEF, the file is not random access
                (on encountering EOF, the value of AFp->Nsamp is set)
             == value, can be either random access or not
*/
  ErrCode = AF_NOERR;

  /* Quick exit */
  if (offs == AFp->Isamp || offs < 0L ||
      (AFp->Nsamp != AF_NSAMP_UNDEF && offs >= AFp->Nsamp))
    return ErrCode;

  Lw = AF_DL[AFp->Format];
  if (Lw > 0) {

/* Fixed length records */
    if (FLseekable (AFp->fp)) {
      ErrCode = AFSEEK (AFp, Lw * offs);
      if (! ErrCode)
        AFp->Isamp = offs;
    }
    else if (offs < AFp->Isamp) {
      UTwarn ("AFposition: %s", AFM_MoveBack);
      ErrCode = AF_IOERR;
    }
    else
      ErrCode = AF_skipNVal (AFp, offs - AFp->Isamp);
  }

  else {

/* Variable length (text) records */
    if (offs < AFp->Isamp) {
      if (FLseekable (AFp->fp)) {
        ErrCode = AFSEEK (AFp, 0L); /* Rewinding to start of data */
        if (! ErrCode)
          AFp->Isamp = 0L;
      }
      else {
        UTwarn ("AFposition: %s", AFM_MoveBack);
        ErrCode = AF_IOERR;
      }
    }

    /* Move forward to the desired position */
    if (! ErrCode)
      ErrCode = AF_skipNFields (AFp, offs - AFp->Isamp);
  }

  return ErrCode;
}

/* Skip N values */

#define NBUF  256


static enum AF_ERR_T
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

  ErrCode = AF_NOERR;
  if (nl < N) {
    if (ferror (AFp->fp)) {
      UTsysMsg ("AFposition: %s %ld", AFM_ReadErrOffs, AFp->Isamp);
      ErrCode = AF_IOERR;
    }
    else if (AFp->Nsamp != AF_NSAMP_UNDEF) {
      UTwarn ("AFposition: %s %ld", AFM_UEoFOffs, AFp->Isamp);
      ErrCode = AF_UEOF;
    }
    else
      AFp->Nsamp = AFp->Isamp;
  }

  return ErrCode;
}

/* Skip N records */


static int
AF_skipNFields (AFILE *AFp, long int N)

{
  int nv;
  enum AF_ERR_T ErrCode;
  long int n;
  double Dv;

/* Read N values */
  ErrCode = AF_NOERR;
  for (n = 0; n < N; ++n) {
    errno = 0;
    nv = fscanf (AFp->fp, "%lg", &Dv);
    if (nv == 1)
      continue;
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
  AFp->Isamp += n;

  if (n < N) {
    if (ErrCode)
      UTsysMsg ("AFposition: %s %ld", AFM_ReadErrOffs, AFp->Isamp);
    else if (AFp->Nsamp != AF_NSAMP_UNDEF) {
      UTwarn ("AFposition: %s %ld", AFM_UEoFOffs, AFp->Isamp);
      ErrCode = AF_UEOF;
    }
    else
      AFp->Nsamp = AFp->Isamp;
  }

  return ErrCode;
}
