/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  long int CPlim (int Mode, AFILE *AFp[], const struct CP_FIpar FI[],
                  int Nifiles, long int Nframe)

Purpose:
  Find the number of output frames

Description:
  This routine determines the frame limits for a set of input files.  The
  frame limits can be determined in a number of ways.
    Nframe - Number of output frames.  If the number of frames is
      AF_NFRAME_UNDEF, this value is undefined.
    FI[i].Lim  - Frame limits for each input file.  The limits consist of two
      values: the start frame and the end frame.  If the end frame is set to
      CP_LIM_UNDEF, then the end frame is undefined.
    AFp[i]->Nsamp - Number of samples in the input file.  If this value is
      AF_NSAMP_UNDEF, this value is undefined.

  Combine Mode:
    In this mode, several files in parallel contribute samples to the output.
    1: If the number of output frames is specified, this value is used.
    2: If the frame limits are specified, the number of frames is determined
       from the maximum number of frames specified for any one file.
    3: The number of frames is determined from the maximum of the number of
       frames in the input files.

  Concatenate Mode:
    In this mode, data from several files is concatenated.
    1: If the number of output frames is specified, this value is used.
    2: The number of frames is the total of the number of frames for each input
       file.  This is either taken from the frame limits of the file, or if not
       defined, taken to be the number of frames in the file.

Parameters:
  <-  long int CPlim
      Number of sample frames (AF_NFRAME_UNDEF if not defined)
   -> int Mode
      M_COMB - combine mode
      M_CONCAT - concatenate mode
   -> AFILE *AFp[]
      Array of Nifiles input audio file pointers
   -> const struct CP_FIpar FI[]
      Input file parameters
   -> int Nifiles
      Number of input files
   -> long int Nframe
      Number of frames to be written to the output file.  If this value is
      AF_NFRAME_UNDEF, it is not used.
 
Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.9 $  $Date: 2003/05/13 01:33:07 $

-------------------------------------------------------------------------*/

#include <libtsp.h>
#include <AO.h>
#include "CopyAudio.h"

#define MAXV(a, b)	(((a) > (b)) ? (a) : (b))
#define ICEILV(n, m)	(((n) + ((m) - 1)) / (m))	/* int n,m >= 0 */


long int
CPlim (int Mode, AFILE *AFp[], const struct CP_FIpar FI[], int Nifiles,
       long int Nframe)

{
  int i;
  long int Nfri, NframeT;

  if (Nframe != AF_NFRAME_UNDEF)

      NframeT = Nframe;

  else if (Mode == M_CONCAT) {

    /* Concatenate files
       NframeT - The default number of output frames is the sum of frames of
                 the input files.
    */
    NframeT = 0L;
    for (i = 0; i < Nifiles; ++i) {
      Nfri = AOnFrame (&AFp[i], &FI[i], 1, AF_NFRAME_UNDEF);
      if (Nfri != AF_NFRAME_UNDEF && NframeT != AF_NFRAME_UNDEF)
	NframeT += Nfri;
      else
	NframeT = AF_NFRAME_UNDEF;
    }
  }

  else			/* Mode == M_COMB */
    /* Combine files
       NframeT - The default number of output frames is the maximum of the
                 number of input frames.
    */
    NframeT = AOnFrame (AFp, FI, Nifiles, AF_NFRAME_UNDEF);

  return NframeT;
}
