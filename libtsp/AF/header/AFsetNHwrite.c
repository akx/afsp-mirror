/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  AFILE *AFsetNHwrite (FILE *fp, struct AF_write *AF)

Purpose:
  Set file format information for a headerless audio file

Description:
  This routine sets the file format information for a headerless audio file.

Parameters:
  <-  AFILE AFsetNHwrite
      Audio file pointer for the audio file.  In case of error, the audio file
      pointer is set to NULL.
   -> FILE *fp
      File pointer for the audio file
  <-> struct AF_write *AFw
      Structure containing file parameters

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.32 $  $Date: 2003/04/27 03:08:12 $

-------------------------------------------------------------------------*/

static char rcsid [] = "$Id: AFsetNHwrite.c 1.32 2003/04/27 AFsp-v6r8 $";

#include <string.h>

#include <libtsp.h>
#include <libtsp/AFheader.h>
#include <libtsp/AFpar.h>
#include <libtsp/AFmsg.h>
#include <libtsp/UTpar.h>


AFILE *
AFsetNHwrite (FILE *fp, struct AF_write *AFw)

{
  AFILE *AFp;

/* Set the parameters for file access */
  AFw->DFormat.ScaleF = 1. / AF_SF[AFw->DFormat.Format];
  AFw->DFormat.Swapb = FTW_subtype (AFw->Ftype);

/* Check the swap code */
  if (! (AFw->DFormat.Swapb == DS_EB     || AFw->DFormat.Swapb == DS_EL ||
	 AFw->DFormat.Swapb == DS_NATIVE || AFw->DFormat.Swapb == DS_SWAP)) {
    UTwarn ("AFsetNHwrite - %s", AFM_NH_InvSwap);
    return NULL;
  }
 
  AFp = AFsetWrite (fp, FT_NH, AFw);

  return AFp;
}
