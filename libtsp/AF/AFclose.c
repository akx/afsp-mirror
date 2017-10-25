/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void AFclose (AFILE *AFp)

Purpose:
  Close an audio file

Description:
  This routine closes an audio file opened with AFopnRead or AFopnWrite.
  If the file was opened for write, the file header is updated with the number
  of samples in the file.  For both read and write operations, the audio file
  parameter structure associated with the file pointer is deallocated and the
  file is closed.

  If the file was opened for write, the number of overloads detected during
  write operations is reported.

Parameters:
  <-  void AFclose
   -> AFILE *AFp
      Audio file pointer for the audio file.  This structure is deallocated
      by this routine.

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.56 $  $Date: 2017/06/12 19:55:25 $

-------------------------------------------------------------------------*/

#include <assert.h>
#include <stdlib.h>   /* EXIT_FAILURE */

#include <libtsp.h>
#include <AFpar.h>
#include <libtsp/nucleus.h>
#include <libtsp/AFheader.h>
#include <libtsp/AFmsg.h>


void
AFclose (AFILE *AFp)

{
  /* Quiet return if the audio file structure is NULL */
  if (AFp == NULL)
    return;

/* Update the header for output files */
  if (AFp->Op == FO_WO) {

    assert (AFp->Format > 0 && AFp->Format < AF_NFD);

    if (! AFp->Error) { /* If an error has occurred, skip the updates */

      /* Check for Nsamp / Nchan / sample size consistency */
      if (AFp->Nsamp % AFp->Nchan != 0) {
        UTwarn ("AFclose - %s:", AFM_NsampNchan);
        UTwarn (AFMF_NsampNchan, "         ", AFp->Nsamp, AFp->Nchan);
      }

      /* Update the header */
      if (AFupdHead (AFp) && AFopt.ErrorHalt)
        exit (EXIT_FAILURE);

      /* Report the number of overloads */
      if (AFp->Novld > 0L)
        UTwarn (AFMF_NClip, "AFclose -", AFp->Novld);
    }
  }

  else
    assert (AFp->Op == FO_RO);

/* Close the file */
  fclose (AFp->fp);

/* Reset some AFILE structure values */
  AFp->fp = NULL;
  AFp->Op = FO_NONE;
  UTfree ((void *) AFp->SpkrConfig);
  UTfree ((void *) AFp->AFInfo.Info);
  UTfree ((void *) AFp->ChunkInfo.ChunkLim);

/* Deallocate the AFILE structure */
  UTfree ((void *) AFp);

  return;
}
