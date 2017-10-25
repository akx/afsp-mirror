/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int AF_checkDataPar (FILE *fp, int Lw, long int *Dstart,
                       const struct AF_ndata *NData, enum AF_FIX_T Fix);
Purpose:
  Check the data layout parameters for an input file

Description:
  This routine checks the parameters for an input file.  The data length (Ldata)
  and number of samples (Nsamp) parameters are checked for consistency and
  modified if necessary.

  The data length is the size of the area (in bytes) in the file reserved for
  for storing the data.  The sample data need not fully occupy this area.  If
  Ldata is undefined (equal to AF_LDATA_UNDEF), it is set to the size of the
  file, less the size of the header.  Ldata is checked against the actual size
  of the file.  It is an error for this value to exceed the actual space in the
  file.

  The number of samples, the data element size, and the number of channels
  are used to calculate the actual amount of space space occupied by the data.
  If this value exceeds the space available, this is an error.  If Nsamp is
  undefined (equal to AF_NSAMP_UNDEF), it is determined from Ldata, setting it
  equal to the integer number of samples that fit into Ldata.  If Nsamp is so
  determined, a warning message is issued if Ldata is not a multiple of the data
  element size.  A warning message is also printed if Nsamp is not a multiple of
  the number of channels.

  For certain file types, the Nsamp value needs to be "fixed".  Optional fixes
  can be specified for the case that Nsamp is larger or smaller than the data
  space available in the file.  The latter case is not normally an error.  If a
  fixup is done, a warning message is printed.  A fixup can also be specified
  for values of Nsamp which are too large.

  If the data does not have fixed length elements (for instance, text data),
  Nsamp may not be specified.  If the text file is seekable, the number of data
  records is determined and used to get Nsamp.

  The file must be initially positioned at the start of the audio data.

Parameters:
  <-  int AF_checkLdata
      Non-zero for an error
   -> int Lw
      For fixed length data items, Lw is the length of each sample value in
      bytes.  For text data, it should be zero.
  <-  long int *Dstart
      Offset to the first byte determined by calling AFtell().
  <-> struct AF_ndata *Ndata
   -> enum AF_FIX_T Fix
      Fix flag, sum of the following subcodes
         AF_FIX_NSAMP_HIGH - Reduce Nsamp if it larger than the data space
                             available
         AF_FIX_NSAMP_LOW  - Increase Nsamp if it is smaller than the data space
                             available
         AF_FIX_LDATA_HIGH - Reduce Ldata if it is larger than the data space
                             available

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.7 $  $Date: 2017/06/28 23:53:09 $

-------------------------------------------------------------------------*/

#include <libtsp/sysOS.h>
#if (SY_OS == SY_OS_WINDOWS)
#  define _CRT_SECURE_NO_WARNINGS     /* Allow fscanf */
#endif

#include <errno.h>

#include <libtsp.h>
#include <AFpar.h>
#include <libtsp/nucleus.h>
#include <libtsp/AFdataio.h>
#include <libtsp/AFheader.h>
#include <libtsp/AFmsg.h>

#define ICEILV(n,m) (((n) + ((m) - 1)) / (m)) /* int n,m >= 0 */

/* Local functions */
static int
AF_setNsamp (FILE *fp, int Lw, struct AF_ndata *NData, enum AF_FIX_T Fix);
static int
AF_checkNsamp (int Lw, struct AF_ndata *NData, enum AF_FIX_T Fix);
static long int
AF_NtextVal (FILE *fp);
static int
AF_setLdata (FILE *fp, long int Dstart, struct AF_ndata *NData,
             enum AF_FIX_T Fix);


int
AFcheckDataPar (FILE *fp, int Lw, long int *Dstart, struct AF_ndata *NData,
                enum AF_FIX_T Fix)

{
  enum AF_ERR_T ErrCode;

/* Preliminary checks */
  if (NData->Ldata != AF_LDATA_UNDEF && NData->Ldata < 0) {
    UTwarn ("AFsetRead - %s: \"%ld\"", AFM_BadLdata, NData->Ldata);
    return 1;
  }
  if (NData->Nsamp != AF_NSAMP_UNDEF && NData->Nsamp < 0) {
    UTwarn ("AFsetRead - %s: \"%ld\"", AFM_BadNsamp, NData->Nsamp);
    return 1;
  }

/* Save the current (Start-of-Data) position */
  *Dstart = 0;
  if (FLseekable (fp)) {
    ErrCode = AF_NOERR;
    *Dstart = AFtell (fp, &ErrCode);
    if (ErrCode)
      return 1;

    /* Find Ldata */
    if (Lw != 0) {
      /* Binary, seekable file */
      if (AF_setLdata (fp, *Dstart, NData, Fix))
        return 1;
    }
  }

/* Set Nsamp if necessary */
  if (AF_setNsamp (fp, Lw, NData, Fix))
    return 1;

/* Samples / channels consistency check */
  if (NData->Nchan <= 0) {
    UTwarn ("AFsetRead - %s: \"%ld\"", AFM_BadNchan, NData->Nchan);
    return 1;
  }
  if (NData->Nsamp != AF_NSAMP_UNDEF && (NData->Nsamp % NData->Nchan) != 0) {
    UTwarn ("AFsetRead - %s", AFM_NsampNchan);
    UTwarn (AFMF_NsampNchan, "           ", NData->Nsamp, NData->Nchan);
  }

  return 0;
}

/* Set Ldata for binary, seekable files */


static int
AF_setLdata (FILE *fp, long int Dstart, struct AF_ndata *NData,
             enum AF_FIX_T Fix)

{
  long int Dspace;

  Dspace = FLfileSize (fp) - Dstart;
  if (Dspace < 0)
    return 1;

  /* Set Ldata if necessary */
  if (NData->Ldata == AF_LDATA_UNDEF)
    NData->Ldata = Dspace;
  else if (NData->Ldata > Dspace) {
    if (Fix & AF_FIX_LDATA_HIGH) {
      UTwarn ("AFsetRead - %s", AFM_FixHiLdata);
      NData->Ldata = Dspace;
    }
    else {
      UTwarn ("AFsetRead - %s", AFM_ErrHiLdata);
      return 1;
    }
  }

  return 0;
}

/* Set and check Nsamp */

static int
AF_setNsamp (FILE *fp, int Lw, struct AF_ndata *NData, enum AF_FIX_T Fix)

{
/* Set Nsamp if necessary */
  if (Lw != 0) {
    if (NData->Nsamp == AF_NSAMP_UNDEF && NData->Ldata != AF_LDATA_UNDEF) {
      NData->Nsamp = NData->Ldata / Lw;
      if (Lw * NData->Nsamp != NData->Ldata)
        UTwarn ("AFsetRead - %s", AFM_NonIntNsamp);
    }
    else if (NData->Nsamp != AF_NSAMP_UNDEF) {
      /* Check Nsamp against Ldata */
      if (AF_checkNsamp (Lw, NData, Fix))
        return 1;
    }
  }
  /* Lw == 0, text file */
  else if (NData->Nsamp == AF_NSAMP_UNDEF && FLseekable (fp)) {
    NData->Nsamp = AF_NtextVal (fp);
    if (NData->Nsamp == AF_NSAMP_UNDEF)
      return 1; /* Error in AF_Nrec */
  }

  /* Check if an "unknown" Nsamp is allowed */
  if (NData->Nsamp == AF_NSAMP_UNDEF && ! AFopt.NsampND) {
    UTwarn ("AFsetRead - %s", AFM_NoNsamp);
    return 1;
  }

  return 0;
}

/* Check Nsamp against Ldata and fix if desired */


static int
AF_checkNsamp (int Lw, struct AF_ndata *NData, enum AF_FIX_T Fix)

{
  if (NData->Nsamp > LONG_MAX / Lw || NData->Nsamp * Lw > NData->Ldata) {
    if (Fix & AF_FIX_NSAMP_HIGH) {
      UTwarn ("AFsetRead - %s", AFM_FixHiNsamp);
      NData->Nsamp = NData->Ldata / Lw;
    }
    else {
      UTwarn ("AFsetRead - %s", AFM_ErrHiNsamp);
      return 1;
    }
  }
  else if (NData->Nsamp * Lw < NData->Ldata && (Fix & AF_FIX_NSAMP_LOW)) {
    UTwarn ("AFsetRead - %s", AFM_FixLoNsamp);
    NData->Nsamp = NData->Ldata / Lw;
  }

  return 0;
}

/*
  This routine finds the number of text fields in a file, starting at the
  current position in the file.  The file position is restored to the current
  position.
*/


static long int
AF_NtextVal (FILE *fp)

{
  long int pos, n;
  int nv;
  enum AF_ERR_T ErrCode;
  double Dv;

/* Save the file position */
  ErrCode = AF_NOERR;
  pos = AFtell (fp, &ErrCode);
  if (ErrCode)
    return 0;

/* Read until End-of-File */
  for (n = 0; ;++n) {
    errno = 0;
    nv = fscanf (fp, "%lg", &Dv);
    if (nv == 1)
      continue;
    else if (nv == EOF)
      break;
    else if (errno != 0) {
      UTsysMsg ("AFsetRead - %s %ld", AFM_ReadErrOffs, n);
      break;
    }
    else {
      UTwarn ("AFsetRead - %s %ld", AFM_ReadErrOffs, n);
      break;
    }
  }

/* Reposition the file */
  if (AFseek (fp, pos, &ErrCode))
    return AF_NSAMP_UNDEF;

  return n;
}
