/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int AFupdHead (AFILE *AFp)

Purpose:
  Update the header for an audio file

Description:
  This routine updates the header for an audio file opened for write.

Parameters:
  <-  int AFupdHead
      Error code, 0 for no error
   -> AFILE *AFp
      Audio file pointer for the audio file

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.7 $  $Date: 2003/01/27 14:14:04 $

-------------------------------------------------------------------------*/

static char rcsid[] = "$Id: AFupdHead.c 1.7 2003/01/27 AFsp-v6r8 $";

#include <assert.h>

#include <libtsp.h>
#include <libtsp/nucleus.h>
#include <libtsp/AFdataio.h>
#include <libtsp/AFheader.h>
#include <libtsp/AFmsg.h>
#include <libtsp/AFpar.h>


int
AFupdHead (AFILE *AFp)

{
  long int pos;
  int (*AF_upd)(AFILE *AFp);

  assert (AFp->Op == FO_WO);
  assert (! AFp->Error);

  /* Set up the appropriate routine */
  AF_upd = NULL;
  switch (AFp->Ftype) {
  case FT_AU:
    AF_upd = AFupdAUhead;
    break;
  case FT_WAVE:
    AF_upd = AFupdWVhead;
    break;
  case FT_AIFF:
  case FT_AIFF_C:
    AF_upd = AFupdAIhead;
    break;
  case FT_NH:
    break;
  default:
    assert (0);
  }

  /* Save the file position, update the header, restore the position */
  if (AF_upd != NULL && FLseekable (AFp->fp)) {
    pos = AFtell (AFp->fp, &AFp->Error);
    if (! AFp->Error) {
      AFp->Error = (*AF_upd) (AFp);
      AFseek (AFp->fp, pos, &AFp->Error);
    }
  }

  if (AFp->Error)
    UTwarn ("AFupdHead: %s", AFM_UpdHeadErr);

  return AFp->Error;
}
