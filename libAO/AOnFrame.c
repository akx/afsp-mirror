/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  long int AOnFrame (AFILE *AFp[], const struct AO_FIpar FI[], int Nifiles,
                     long int Nframe)

Purpose:
  Find the number of sample frames

Description:
  This routine determines the frame limits for a set of input files.  The
  frame limits can be determined in a number of ways.
    Nframe - Number of output frames.  If the number of frames is
      AF_NFRAME_UNDEF, this value is undefined.
    FI[i].Lim  - Frame limits for each input file.  The limits consist of two
      values: the start frame and the end frame.  If the end frame is set to
      AO_LIM_UNDEF, then the end frame is undefined.
    AFp[i]->Nsamp - Number of samples in the input file.  If this value is
      AF_NSAMP_UNDEF, this value is undefined.

  1: If the number of sample frames (Nframe) is specified, this value is used.
  2: If the frame limits are specified, the number of frames is determined
     from the maximum number of frames specified for any one file.
  3: The number of frames is determined from the maximum of the number of
     frames in the input files.

Parameters:
  <-  long int AOnFrame
      Number of sample frames (AF_NFRAME_UNDEF if not defined)
   -> AFILE *AFp[]
      Array of Nifiles input audio file pointers
   -> const struct AO_FIpar FI[]
      Input file parameters
   -> int Nifiles
      Number of input files
   -> long int Nframe
      Number of frames.  If this value is AF_NFRAME_UNDEF, it is not used.
 
Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.9 $  $Date: 2003/05/09 12:32:37 $

-------------------------------------------------------------------------*/

#include <libtsp.h>
#include <AO.h>

#define MAXV(a, b)	(((a) > (b)) ? (a) : (b))
#define ICEILV(n, m)	(((n) + ((m) - 1)) / (m))	/* int n,m >= 0 */

#define ROUTINE		"AOnFrame"
#define PGM		((UTgetProg ())[0] == '\0' ? ROUTINE : UTgetProg ())

static long int
AO_NframeAF (AFILE *AFp[], const struct AO_FIpar FI[], int Nifiles);
static long int
AO_NframeFI (const struct AO_FIpar FI[], int Nifiles);


long int
AOnFrame (AFILE *AFp[], const struct AO_FIpar FI[], int Nifiles,
	  long int Nframe)

{
  long int NframeT;

  if (Nframe != AF_NFRAME_UNDEF)
    NframeT = Nframe;

  else {
    NframeT = AO_NframeFI (FI, Nifiles);
    if (NframeT == AF_NFRAME_UNDEF)
      NframeT = AO_NframeAF (AFp, FI, Nifiles);
  }

  return NframeT;
}

/* Find the number of sample frames (maximum of limits) */


static long int
AO_NframeFI (const struct AO_FIpar FI[], int Nifiles)

{
  int i, DiffN, NotAllLim;
  long int Nframe, Nfri;

/*
  - Nfri - number of frames for file i
  - Nframe - max number of frames
  - DiffN - true if Nframe and Nfri differ
*/
  DiffN = 0;
  NotAllLim = 0;
  Nframe = AF_NFRAME_UNDEF;
  for (i = 0; i < Nifiles; ++i) {
    if (FI[i].Lim[1] != AO_LIM_UNDEF)
      Nfri = MAXV (FI[i].Lim[1] - FI[i].Lim[0] + 1L, 0L);
    else
      Nfri = AF_NFRAME_UNDEF;

    if (i > 0 && Nframe != Nfri) {
      if (Nframe == AF_NFRAME_UNDEF || Nfri == AF_NFRAME_UNDEF)
	NotAllLim = 1;
      else
	DiffN = 1;
    }

    if (Nframe == AF_NFRAME_UNDEF)
      Nframe = Nfri;
    else if (Nfri != AF_NFRAME_UNDEF)
      Nframe = MAXV (Nframe, Nfri);
  }

  if (NotAllLim)
    UTwarn ("%s - %s", PGM, AOM_NotAllLim);
  else if (DiffN)
    UTwarn ("%s - %s", PGM, AOM_DiffLim);

  return Nframe;
}

/* Get the number of frames from the number of samples in the input files */


static long int
AO_NframeAF (AFILE *AFp[], const struct AO_FIpar FI[], int Nifiles)

{
  int i, DiffN;
  long int Nframe, Nfri;

  DiffN = 0;
  Nframe = 0L;
  for (i = 0; i < Nifiles; ++i) {
    if (AFp[i]->Nsamp == AF_NSAMP_UNDEF) {
      Nframe = AF_NFRAME_UNDEF;
      break;
    }
    Nfri = ICEILV (AFp[i]->Nsamp, AFp[i]->Nchan) - FI[i].Lim[0];
    if (i > 0 && Nframe != Nfri)
      DiffN = 1;
    Nframe = MAXV (Nframe, Nfri);
  }

  if (Nifiles > 1 && Nframe == AF_NFRAME_UNDEF)
    UTwarn ("%s - %s", PGM, AOM_NotAllNSamp);
  else if (DiffN)
    UTwarn ("%s - %s", PGM, AOM_DiffNSamp);

  return Nframe;
}
