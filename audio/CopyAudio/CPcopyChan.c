/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  long int CPcopyChan (AFILE *AFp[], const long int StartF[], int Nifiles,
                       long int Nframe, long int MaxNframe, AFILE *AFpO)

Purpose:
  Copy audio file samples

Description:
  This routine copies input audio file(s) to an output audio file.  The input
  channels are spread out over one or more files.  All input channels appear
  in the output file.  The number of output sample frames can be specified in a
  number of ways.  Let the number of frames as determined by the file limits
  be Nframe and the maximum number of frames be MaxNframe.  Designate the
  number of frames to the end-of-file be EoF.  The combinations are as follows.
                                      MaxNframe
                             specified        unspecified

    Nframe   specified    min(Nframe, MaxNframe)  Nframe
           unspecified    min(MaxNframe, EoF)     EoF

Parameters:
  <-  long int CPcopyChan
      Number of sample frames written
   -> AFILE *AFp[]
      Array of Nifiles input audio file pointers
   -> const long int StartF[]
      Start frames for the input files
   -> int Nifiles
      Number of input files
   -> long int Nframe
      Number of output frames
   -> long int MaxNframe
      Maximum number of frames to be written to the output file
   -> AFILE *AFpO
      Output audio file pointer
 
Author / revision:
  P. Kabal  Copyright (C) 2005
  $Revision: 1.23 $  $Date: 2005/02/01 04:29:25 $

-------------------------------------------------------------------------*/

#include <assert.h>
#include <limits.h>

#include <libtsp.h>
#include "CopyAudio.h"

#define MINV(a, b)	(((a) < (b)) ? (a) : (b))
#define MAXV(a, b)	(((a) > (b)) ? (a) : (b))
#define ICEILV(n, m)	(((n) + ((m) - 1)) / (m))	/* int n,m >= 0 */

#define BFSIZE	5120

static long int
CP_copy1 (AFILE *AFpI, long int StartF, long int Nframe, int eof, AFILE *AFpO);
static long int
CP_copyNa (AFILE *AFp[], const long int StartF[], int Nifiles, long int Nframe,
	   AFILE *AFpO);
static long int
CP_copyNb (AFILE *AFp[], const long int StartF[], int Nifiles, long int Nframe,
	   AFILE *AFpO);


long int
CPcopyChan (AFILE *AFp[], const long int StartF[], int Nifiles,
	    long int Nframe, long int MaxNframe, AFILE *AFpO)

{
  long int NchanI, NchanMax, Nfw;
  int eof, j, Ns;

/*
   Cases:
   (1) single input file - the copying is sample by sample without regard to
       channels
   (N) multiple input files - the input channels have to be interleaved in the
       output.  The number of samples must be known.
       (a) The number of channels is such that one or more record from the
           file with the largest number of channels can fit into the buffer
	   along with the output record.
       (b) The number of channels is large.  The data from a single input file
           may have to be read in chunks.  Each chunk is immediately sent to
           the output.
*/

/* Determine the total number of input channels and the maximum number of
   input channels to be read from any file */
  NchanMax = 0;
  NchanI = 0;
  for (j = 0; j < Nifiles; ++j) {
    NchanMax = MAXV (NchanMax, AFp[j]->Nchan);
    NchanI += AFp[j]->Nchan;
  }
  assert (NchanI == AFpO->Nchan);

/* Single input file */
  if (Nifiles == 1) {
    eof = (Nframe == AF_NFRAME_UNDEF);
    if (MaxNframe != AF_NFRAME_UNDEF) {
      if (eof)
	Nframe = MaxNframe;
      else
	Nframe = MINV (Nframe, MaxNframe);
    }

    Nfw = CP_copy1 (AFp[0], StartF[0], Nframe, eof, AFpO);
  }
/* Multiple input files */
  else {
    Ns = (int) (BFSIZE / (NchanMax + AFpO->Nchan));
    if (Ns >= 1)
      Nfw = CP_copyNa (AFp, StartF, Nifiles, Nframe, AFpO);
    else
      Nfw = CP_copyNb (AFp, StartF, Nifiles, Nframe, AFpO);
  }

  return Nfw;
}

/* Copy one file */


static long int
CP_copy1 (AFILE *AFpI, long int StartF, long int Nframe, int eof, AFILE *AFpO)

{
  double Dbuff[BFSIZE];
  long int offr, ioffs, Nrem, Nj;
  int Nr, Nv;

/* There are 3 cases here:
   eof indicates that reading should stop at end-of-file (EOF)
   1. eof set & Nframe set, read Nframe or to EOF, whichever comes first
   2. eof set & Nframe not set, read to EOF
   3. eof not set (Nframe must be set), read Nframe samples, possibly
      reading past EOF (padding zeros)
*/
  assert (eof || Nframe != AF_NFRAME_UNDEF);

/* Copy sample by sample from the input file to the output file */
/* *** offsets and counts are sample offsets, not frame values *** */
  Nj = AFpI->Nchan;
  if (Nframe == AF_NFRAME_UNDEF)
    Nrem = LONG_MAX;
  else
    Nrem = Nj * Nframe;

  offr = 0L;
  while (Nrem > 0L) {
    Nv = (int) MINV (Nrem, BFSIZE);
    Nr = AFdReadData (AFpI, offr + Nj * StartF, Dbuff, Nv);

    /* There is a potential problem when we encounter EOF since the
       number of samples may not be a multiple of the number of channels.
       Here we round up to the next multiple of channels, resetting Nrem
       to the value it should have had before the read if we had known the
       number of samples in the file.
       Note: AFdReadData returns Nv samples, Nr of which are real samples
             and the rest are zeros.
    */
    if (eof && Nr < Nv) {
      ioffs = Nj * ICEILV (offr + Nr, Nj);
      if (ioffs != offr + Nr)
	UTwarn ("%s - %s", PROGRAM, CPM_NSampNChan);
      Nrem = ioffs - offr;	/* Reset Nrem */
      Nv = MINV (Nrem, Nv);	/* Reset Nv (no. samples to be written) */
      eof = 0;			/* Reset eof: do not test for EOF again */
    }
    Nrem -= Nv;
    offr += Nv;

    AFdWriteData (AFpO, Dbuff, Nv);
  }
  assert (Nj * (offr/Nj) == offr);

  return offr/Nj;
}

/* Copy multiple files, number of channels small */


static long int
CP_copyNa (AFILE *AFp[], const long int StartF[], int Nifiles, long int Nframe,
	   AFILE *AFpO)

{
  int i, j, k, n, Ns, Nj, NO, Nfr;
  long int offr, offs, Nrem, NchanMax;
  double Dbuff[BFSIZE];
  double *Dbuffi, *Dbuffo;

  assert (Nframe != AF_NFRAME_UNDEF);

/* Determine the maximum number of input channels to be read from any file */
  NchanMax = 0L;
  for (j = 0; j < Nifiles; ++j)
    NchanMax = MAXV (NchanMax, AFp[j]->Nchan);

/* Split the buffer space up for the input and output buffers */
  NO = (int) AFpO->Nchan;
  Ns = (int) (BFSIZE / (NchanMax + NO));
  assert (Ns > 0);

  Dbuffi = Dbuff;
  Dbuffo = Dbuff + ((int) NchanMax) * Ns;

  offr = 0L;
  Nrem = Nframe;
  while (Nrem > 0) {

    Nfr = (int) MINV (Nrem, Ns);
    n = 0;
    for (j = 0; j < Nifiles; ++j) {
      Nj = (int) AFp[j]->Nchan;
      offs = Nj * (offr + StartF[j]);
      AFdReadData (AFp[j], offs, Dbuffi, Nfr * Nj);
      for (k = 0; k < Nj; ++k) {
	for (i = 0; i < Nfr; ++i)
	  Dbuffo[i*NO+n+k] = Dbuffi[i*Nj+k];
      }
      n += Nj;
    }

    /* Write the samples to the output file */
    AFdWriteData (AFpO, Dbuffo, Nfr * NO);
    offr += Nfr;
    Nrem -= Nfr;
  }

  return Nframe;
}

/* Copy multiple files, number of channels large */


static long int
CP_copyNb (AFILE *AFp[], const long int StartF[], int Nifiles, long int Nframe,
	   AFILE *AFpO)

{
  int j, Nc;
  long int offr, offs, l, Nj;
  double Dbuff[BFSIZE];

  assert (Nframe != AF_NFRAME_UNDEF);

/* Large number of channels, handle one input file at a time */
  for (offr = 0L; offr < Nframe; ++offr) {
    for (j = 0; j < Nifiles; ++j) {
      Nj = AFp[j]->Nchan;
      for (l = 0L; l < Nj; l += Nc) {
	Nc = (int) MINV (Nj - l, BFSIZE);
	offs = Nj * (offr + StartF[j]) + l;
	AFdReadData (AFp[j], offs, Dbuff, Nc);
	AFdWriteData (AFpO, Dbuff, Nc);
      }
    }
  }

  return Nframe;
}
