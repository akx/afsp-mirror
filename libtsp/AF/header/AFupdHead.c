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
  P. Kabal  Copyright (C) 2017
  $Revision: 1.13 $  $Date: 2017/05/15 15:42:48 $

-------------------------------------------------------------------------*/

#include <assert.h>

#include <libtsp.h>
#include <AFpar.h>
#include <libtsp/nucleus.h>
#include <libtsp/AFdataio.h>
#include <libtsp/AFheader.h>
#include <libtsp/AFmsg.h>


int
AFupdHead (AFILE *AFp)

{
  long int pos, Nframe;
  int Changed;
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
  case FT_WAVE_EX:
    AF_upd = AFupdWVhead;
    break;
  case FT_AIFF:
  case FT_AIFF_C:
    AF_upd = AFupdAIhead;
    break;
  case FT_NH:
  case FT_TXAUD:
    break;
  default:
    assert (0);
  }

  /* Check if the header needs updating */
  Nframe = AFp->Nsamp / AFp->Nchan;
  if (Nframe * AFp->Nchan != AFp->Nsamp)
    UTwarn ("AFupdhead - %s", AFM_NsampNchan);
  Changed = (AFp->Nframe != AF_NFRAME_UNDEF && AFp->Nframe != Nframe);
  if (Changed)
    UTwarn ("AFupdHead - %s", AFM_NsampMis);

  /* If an update function is enabled: Save the file position, update the
     header, restore the position */
  if (AF_upd != NULL && FLseekable (AFp->fp)) {
    pos = AFtell (AFp->fp, &AFp->Error);
    if (! AFp->Error) {
      AFp->Error = (*AF_upd) (AFp);         /* Update the header */
      AFseek (AFp->fp, pos, &AFp->Error);
    }
  }

  if (AFp->Error)
    UTwarn ("AFupdHead: %s", AFM_UpdHeadErr);

  return AFp->Error;
}
