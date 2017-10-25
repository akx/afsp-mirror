/*------------- Telecommunications & Signal Processing Lab -------------
                           McGill University

Routine:
  void IAfileInfo (const AFILE *AFp, const char Fname[], FILE *fpinfo)

Purpose:
  Print file parameters

Description:
  This formats and prints the file parameters.

Parameters:
  <-  void IAfileInfo
   -> const AFILE *AFp
      Point to audio file information structure
   -> const char Fname[]
      File name\
  <-> FILE *fpinfo
      Output stream for the file information.  If fpinfo is NULL, no information
      is written.

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.4 $  $Date: 2017/06/26 23:58:54 $

----------------------------------------------------------------------*/

#include <assert.h>

#include <libtsp.h>
#include <libtsp/nucleus.h>
#include "InfoAudio.h"

static const char *DataFormat[] = {
  "undefined",
  "A-law8",
  "mu-law8",
  "mu-law8 (bit-rev)",
  "unsigned8",
  "integer8",
  "integer16",
  "integer24",
  "integer32",
  "float32",
  "float64",
  "text16",
  "text"
};
static const char *DataSwap[] = {
  "big-endian",
  "little-endian",
  "native",   /* AFp->Swapb is native or swap */
  "swap"
};

#define NELEM(array)  ((int) ((sizeof array) / (sizeof array[0])))


void
IAfileInfo (const AFILE *AFp, const char Fname[], FILE *fpinfo)

{
  int Dbo, Hbo;

  /* Sanity checks */
  assert (NELEM (DataFormat) == AF_NFD);
  assert (AFp->Format > 0);
  assert (AF_DL[AFp->Format] > 1 || AFp->Swapb == DS_NATIVE);

  if (fpinfo == NULL)
    return;

  if (AFp->fp == stdin)
      fprintf (fpinfo, IAM_stdin);
  else
    fprintf (fpinfo, IAMF_FName, Fname);
  fprintf (fpinfo, IAMF_Header, AFp->Start, AFp->Sfreq, AFp->Nsamp / AFp->Nchan,
          AFp->Nchan, DataFormat[AFp->Format]);
  Dbo = UTbyteCode (AFp->Swapb);
  Hbo = UTbyteOrder ();
  if (AF_DL[AFp->Format] <= 1)      /* Text and one-byte formats */
    fprintf (fpinfo, IAM_ByteStream);
  else
    fprintf (fpinfo, IAMF_FByteOrder, DataSwap[Dbo]);
  fprintf (fpinfo, IAMF_HByteOrder, DataSwap[Hbo]);

  return;
}
