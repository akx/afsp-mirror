/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void CPcopy (int Mode, AFILE *AFp[], const struct CP_FIpar FI[], int Nifiles,
               const struct CP_Chgain *Chgain, long int Nframe, AFILE *AFpO)

Purpose:
  Copy audio file samples to an output file (linear combinations of values)

Description:
  This routine copies samples from input audio files and writes them to an
  output audio file.  The samples in the output file are linear combinations
  of the input channels.  The input channels are spread out over one or more
  input files.

Parameters:
   -> int Mode
      Combine / concatenate mode
   -> AFILE *AFp[]
      Array of Nifiles input audio file pointers
   -> const struct CP_FIpar FI[]
      Input file parameter structures
   -> int Nifiles
      Number of input files
   -> const struct CP_Chgain *Chgain
      Structure with channel gains
   -> long int Nframe
      Number of output frames
   -> AFILE *AFpO
      Output audio file pointer

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.7 $  $Date: 2003/05/13 01:34:27 $

-------------------------------------------------------------------------*/

#include <assert.h>

#include <libtsp.h>
#include "CopyAudio.h"

#define MINV(a, b)	(((a) < (b)) ? (a) : (b))
#define MAXV(a, b)	(((a) > (b)) ? (a) : (b))


void  CPcopy (int Mode, AFILE *AFp[], const struct CP_FIpar FI[], int Nifiles,
	      const struct CP_Chgain *Chgain, long int Nframe, AFILE *AFpO)

{
  int i, DiffLim, Conflict;
  long int MaxNfr, Nfx, Nfw, NfL;
  long int StartF[MAXIFILE], Nfr[MAXIFILE];

/* Create the start frame and number of frames arrays */
  Conflict = 0;
  DiffLim = 0;
  MaxNfr = AF_NFRAME_UNDEF;
  for (i = 0; i < Nifiles; ++i) {
    StartF[i] = FI[i].Lim[0];
    if (FI[i].Lim[1] != CP_LIM_UNDEF) {
      Nfr[i] = MAXV (FI[i].Lim[1] - StartF[i] + 1L, 0L);
      if (MaxNfr != AF_NFRAME_UNDEF)
	MaxNfr = MAXV (Nfr[i], MaxNfr);
      else
	MaxNfr = Nfr[i];
    }
    else
      Nfr[i] = AF_NFRAME_UNDEF;

    /* Check for incompatible limits and number of frames */
    if (Mode == M_COMB) {
      if (i > 0 && Nfr[i] != Nfr[i-1])
	DiffLim = 1;
      if (Nframe != AF_NFRAME_UNDEF && Nfr[i] != AF_NFRAME_UNDEF &&
	  Nframe != Nfr[i])
	Conflict = 1;
    }
  }

  if (Mode == M_COMB) {

    /* Combine, process all input files at once */
    if (MaxNfr != AF_NFRAME_UNDEF && Nframe != AF_NFRAME_UNDEF)
      MaxNfr = Nframe;
    if (Chgain->NO == 0)
      Nfw = CPcopyChan (AFp, StartF, Nifiles, Nframe, MaxNfr, AFpO);
    else
      Nfw = CPcombChan (AFp, StartF, Nifiles, Nframe, Chgain, MaxNfr, AFpO);
    assert (Nframe == AF_NFRAME_UNDEF || Nfw == Nframe);
  }

  else {

    NfL = Nframe;		/* Number of frames left to be written */
    for (i = 0; i < Nifiles; ++i) {

      /* For the last file, if Nframe is defined, set Nfr[i] to force the
	 exact number of frames to be written
      */
      Nfx = Nfr[i];
      if (NfL != AF_NFRAME_UNDEF && i == Nifiles - 1)
	Nfx = NfL;

      /* Concatenate, process one file at a time */
      if (Chgain->NO == 0)
	Nfw = CPcopyChan (&AFp[i], &StartF[i], 1, Nfx, NfL, AFpO);
      else
	Nfw = CPcombChan (&AFp[i], &StartF[i], 1, Nfx, Chgain, NfL, AFpO);
      assert (Nfx == AF_NFRAME_UNDEF || Nfw == Nfx);

      if (Nfr[i] != AF_NFRAME_UNDEF && NfL != AF_NFRAME_UNDEF &&
	  (NfL < Nfr[i] || (i == Nifiles && NfL != Nfr[i])))
	Conflict = 1;

      if (NfL != AF_NFRAME_UNDEF)
	NfL -= Nfw;
    }
    assert (NfL == AF_NFRAME_UNDEF || NfL == 0L);
  }

  /* Warning message (different limits message has been given earlier) */
  if (! DiffLim && Conflict)
    UTwarn ("%s - %s", PROGRAM, CPM_ConfNFrame);

  return;
}
