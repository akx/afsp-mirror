/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  long int CPcombChan (AFILE *AFp[], const long int StartF[], int Nifiles,
                       long int Nframe, const struct CP_Chgain *Chgain,
		       long int MaxNframe, AFILE *AFpO)

Purpose:
  Combine audio file samples (linear combinations of values)

Description:
  This routine combines samples from input audio files and writes them to an
  output audio file.  The samples in the output file are linear combinations
  of the input channels.  The input channels are spread out over one or more
  input files.  The number of output sample frames can be specified in a
  number of ways.  Let the number of frames as determined by the file limits
  be Nframe and the maximum number of frames be MaxNframe.  Designate the
  number of frames to the end-of-file be EoF.  The combinations are as follows.
                                      MaxNframe
                             specified        unspecified

    Nframe   specified    min(Nframe, MaxNframe)  Nframe
           unspecified    min(MaxNframe, EoF)     EoF

Parameters:
  <-  long int CPcombChan
      Number of sample frames written
   -> AFILE *AFp[]
      Array of Nifiles input audio file pointers
   -> const long int StartF[]
      Start frames for the input files
   -> int Nifiles
      Number of input files
   -> long int Nframe
      Number of output frames
   -> const struct CP_Chgain *Chgain
      Structure with channel gains
   -> long int MaxNframe
      Maximum number of frames to be written to the output file
   -> AFILE *AFpO
      Output audio file pointer

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.25 $  $Date: 2003/05/13 01:34:10 $

-------------------------------------------------------------------------*/

#include <assert.h>

#include <libtsp.h>
#include "CopyAudio.h"

#define MINV(a, b)	(((a) < (b)) ? (a) : (b))
#define MAXV(a, b)	(((a) > (b)) ? (a) : (b))
#define ICEILV(n, m)	(((n) + ((m) - 1)) / (m))	/* int n,m >= 0 */

#define BFSIZE	5120		/* At least MAXNI + MAXNO */

static long int
CP_comb1 (AFILE *AFpI, long int StartF, long int Nframe,
	  const struct CP_Chgain *Chgain, long int MaxNframe, AFILE *AFpO);
static long int
CP_combN (AFILE *AFp[], const long int StartF[], int Nifiles,
	  long int Nframe, const struct CP_Chgain *Chgain, AFILE *AFpO);


long int
CPcombChan (AFILE *AFp[], const long int StartF[], int Nifiles,
	    long int Nframe, const struct CP_Chgain *Chgain,
	    long int MaxNframe, AFILE *AFpO)

{
  long int Nfw;

/*
   Cases:
   (1) single input file - combine samples from different input channels
   (N) multiple input files - combine the input channels from the different
       input files.  The number of samples must be known.
*/
  if (Nifiles == 1)
    Nfw = CP_comb1 (AFp[0], StartF[0], Nframe, Chgain, MaxNframe, AFpO);
  else
    Nfw = CP_combN (AFp, StartF, Nifiles, Nframe, Chgain, AFpO);

  return Nfw;
}

/* Combine samples from one input file */


static long int
CP_comb1 (AFILE *AFpI, long int StartF, long int Nframe,
	  const struct CP_Chgain *Chgain, long int MaxNframe, AFILE *AFpO)

{
  int eof, i, k, m, NO, Nc, Nr, Ns, Nfv, Nfr;
  long int offr, offs, Nj, Nfrem;
  double g;
  double Dbuff[BFSIZE];
  double *Dbuffi, *Dbuffo;

  NO = (int) AFpO->Nchan;
  Nj = AFpI->Nchan;
  assert (AFpO->Nchan == Chgain->NO);
  assert (Nj >= Chgain->NI);

/* Split the buffer space up for the input and output buffers */
  Nc = (int) MINV (MAXNI, Nj);
  Ns = BFSIZE / (Nc + NO);
  Dbuffi = Dbuff;
  Dbuffo = Dbuff + Nc * Ns;

/* Main loop */
  eof = (Nframe == AF_NFRAME_UNDEF);
  if (MaxNframe == AF_NFRAME_UNDEF) {
    if (eof)
      Nfrem = LONG_MAX;
    else
      Nfrem = Nframe;
  }
  else {
    if (eof)
      Nfrem = MaxNframe;
    else
      Nfrem = MINV (Nframe, MaxNframe);
  }

  offr = 0L;
  while (Nfrem > 0L) {

    Nfv = (int) MINV (Nfrem, Ns);

    for (k = 0; k < NO; ++k) {
      for (i = 0; i < Nfv; ++i)
	Dbuffo[i*NO+k] = Chgain->Offset[k];	/* dc offset */
    }

    /* Read Nc channels (out of Nj) channels from the input file */
    offs = Nj * (offr + StartF);
    if (Nc == Nj) {
      Nr = AFdReadData (AFpI, offs, Dbuffi, Nfv * Nc);
      Nfr = ICEILV (Nr, Nc);
    }
    else {
      for (i = 0; i < Nfv; ++i) {
	Nr = AFdReadData (AFpI, offs + i * Nj, &Dbuffi[i*Nc], Nc);
	if (Nr == 0 && eof)
	  break;
      }
      Nfr = i;
    }

    if (Nfr < Nfv && eof) {
      Nfrem = Nfr;
      Nfv = Nfrem;
    }
    Nfrem -= Nfv;
    offr += Nfv;

    /* Add the contribution from the input buffer to the output buffer */
    for (m = 0; m < Nc; ++m) {
      for (k = 0; k < NO; ++k) {
	g = Chgain->Gain[k][m];
	if (g != 0.0) {
	  for (i = 0; i < Nfv; ++i)
	    Dbuffo[i*NO+k] += g * Dbuffi[i*Nc+m];
	}
      }
    }

    /* Write the samples to the output file */
    AFdWriteData (AFpO, Dbuffo, Nfv * NO);
  }

  return offr;
}

/* Multiple input channels */


static long int
CP_combN (AFILE *AFp[], const long int StartF[], int Nifiles, long int Nframe,
	  const struct CP_Chgain *Chgain, AFILE *AFpO)

{
  int i, j, k, m, n, Nt, NI, NO, Nc, Ns, Nfr, NchanMax;
  long int offr, offs, Nj, Nrem;
  double g;
  double Dbuff[BFSIZE];
  double *Dbuffi, *Dbuffo;

  assert (Nframe != AF_NFRAME_UNDEF);
  assert (AFpO->Nchan == Chgain->NO);

/* Number of channels to be read (all files), maximum MAXNI */
  NI = 0;
  for (j = 0; j < Nifiles; ++j)
    NI += (int) MINV (AFp[j]->Nchan, MAXNI - NI);
  assert (NI >= Chgain->NI);

/* Maximum number of channels from any single file */
  Nt = 0;
  NchanMax = 0;
  for (j = 0; j < Nifiles; ++j) {
    Nc = (int) MINV (AFp[j]->Nchan, NI - Nt);
    NchanMax = MAXV (NchanMax, Nc);
    Nt += Nc;
  }

  NO = (int) AFpO->Nchan;

/* Split the buffer space up for the input and output buffers */
  Ns = BFSIZE / (NchanMax + NO);
  Dbuffi = Dbuff;
  Dbuffo = Dbuff + NchanMax * Ns;

/*
   The copying operation takes scaled samples from the input si(n,i)
   (channel n, sample i) and sums them to form so(k,i) (channel k, sample i)

     so(k,i) = SUM g(k,n) * si(n,i)
              n
   However, samples from different channels are interleaved.  Thus so(k,i) is
   really so(i * No + k), where No is the number of output channels.  For the
   input data, the channels appear in different files.

   The copying operation loops over the input channels n, the output channels
   k and the samples i.  The inner loop is chosen to be over the samples, so
   that this loop can be skipped if the gain for an input/output channel
   combination is zero.  The looping over input channels is actually two loops;
   one over files and the other for channels within a file.  The loop over
   files is outermost, so that the data from the input files need only be read
   once.
*/

/* Main loop */
  offr = 0L;
  Nrem = Nframe;
  while (Nrem > 0) {

    Nfr = (int) MINV (Nrem, Ns);
    for (k = 0; k < NO; ++k) {
      for (i = 0; i < Nfr; ++i)
	Dbuffo[i*NO+k] = Chgain->Offset[k];	/* dc offset */
    }
    n = 0;
    Nt = 0;
    for (j = 0; j < Nifiles; ++j) {
      Nj = AFp[j]->Nchan;
      Nc = (int) MINV (NI - Nt, Nj);

      /* Read Nc channels (out of Nj) channels from file j */
      offs = Nj * (offr + StartF[j]);
      if (Nc == Nj)
	AFdReadData (AFp[j], offs, Dbuffi, Nfr * Nc);
      else if (Nc > 0) {
	for (i = 0; i < Nfr; ++i)
	  AFdReadData (AFp[j], offs + i * Nj, &Dbuffi[i*Nc], Nc);
      }
      Nt += Nc;

      /* Add the contribution from file j to the output */
      for (m = 0; m < Nc; ++m) {
	for (k = 0; k < NO; ++k) {
	  g = Chgain->Gain[k][n];
	  if (g != 0.0) {
	    for (i = 0; i < Nfr; ++i)
	      Dbuffo[i*NO+k] += g * Dbuffi[i*Nc+m];
	  }
	}
	++n;
      }
    }
    /* Write the samples to the output file */
    AFdWriteData (AFpO, Dbuffo, Nfr * NO);
    offr += Nfr;
    Nrem -= Nfr;
  }

  return Nframe;
}
