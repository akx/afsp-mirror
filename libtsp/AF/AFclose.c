/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void AFclose (AFILE *AFp)

Purpose:
  Close an audio file

Description:
  This routine closes an audio file opened with AFopenRead or AFopenWrite.
  If the file was opened for write, the file header is updated with the number
  of samples in the file.  For both read and write operations, the audio file
  parameter structure associated with the file pointer is deallocated and the
  file is closed.

  If the file was opened for write, the number of overloads detected during
  write operations is reported.

Parameters:
   -> AFILE *AFp
      Audio file pointer for the audio file.  This structure is deallocated
      by this routine.

Author / revision:
  P. Kabal  Copyright (C) 2001
  $Revision: 1.41 $  $Date: 2001/11/30 13:36:26 $

-------------------------------------------------------------------------*/

static char rcsid[] = "$Id: AFclose.c 1.41 2001/11/30 AFsp-v6r8 $";

#include <assert.h>
#include <stdlib.h>		/* EXIT_FAILURE */

#include <libtsp.h>
#include <libtsp/AFheader.h>
#include <libtsp/AFmsg.h>
#include <libtsp/AFpar.h>


void
AFclose (AFILE *AFp)

{
/* Update the header for output files */
  if (AFp->Op == FO_WO) {

    assert (AFp->Format > 0 && AFp->Format < NFD);

    /* Check for Nsamp / Nchan / sample size consistency */
    if (AFp->Nsamp % AFp->Nchan != 0) {
      UTwarn ("AFclose - %s:", AFM_NSampNChan);
      UTwarn (AFMF_NSampNChan, "         ", AFp->Nsamp, AFp->Nchan);
    }

    /* Update the header */
    if (AFupdHead (AFp) && (AFoptions ())->ErrorHalt)
      exit (EXIT_FAILURE);

    /* Report the number of overloads */
    if (AFp->Novld > 0L)
      UTwarn (AFMF_NClip, "AFclose -", AFp->Novld);
  }

  else
    assert (AFp->Op == FO_RO);

/* Close the file */
  fclose (AFp->fp);

/* Reset some AFILE structure values */
  AFp->fp = NULL;
  AFp->Op = FO_NONE;
  UTfree ((void *) AFp->SpkrConfig);
  AFp->Hinfo.N = 0;
  UTfree ((void *) AFp->Hinfo.Info);

/* Deallocate the AFILE structure */
  UTfree ((void *) AFp);

  return;
}
