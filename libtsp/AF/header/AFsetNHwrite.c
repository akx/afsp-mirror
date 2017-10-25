/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  AFILE *AFsetNHwrite (FILE *fp, struct AF_write *AFw)

Purpose:
  Set file format information for a headerless audio file

Description:
  This routine sets the file format information for a headerless audio file.

Parameters:
  <-  AFILE *AFsetNHwrite
      Audio file pointer for the audio file.  This routine allocates the
      space for this structure.  If an error is detected, a NULL pointer is
      returned.
   -> FILE *fp
      File pointer for the audio file
  <-> struct AF_write *AFw
      Structure containing file parameters

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.42 $  $Date: 2017/05/08 03:34:21 $

-------------------------------------------------------------------------*/

#include <string.h>
#include <assert.h>

#include <libtsp.h>
#include <AFpar.h>
#include <libtsp/nucleus.h>
#include <libtsp/AFheader.h>
#include <libtsp/AFmsg.h>
#include <libtsp/UTpar.h>


AFILE *
AFsetNHwrite (FILE *fp, struct AF_write *AFw)

{
  AFILE *AFp;
  enum UT_DS_T Swapb;

  switch (AFw->FtypeW) {
  case FTW_NH_EB:
    Swapb = DS_EB;
    break;
  case FTW_NH_EL:
    Swapb = DS_EL;
    break;
  case FTW_NH_NATIVE:
    Swapb = DS_NATIVE;
    break;
  case FTW_NH_SWAP:
    Swapb = DS_SWAP;
    break;
  default:
    assert (0);
  }

  /* Check the swap code */
  AFw->DFormat.Swapb = UTswapCode(Swapb);  /* Now DS_NATIVE or DS_SWAP */

  /* Create the audio file structure */
  AFp = AFsetWrite (fp, FT_NH, AFw);

  return AFp;
}
