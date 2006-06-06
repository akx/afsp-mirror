/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  LPsyn [options] -p PFile -c LPFile AFileI [AFileO]

Purpose:
  LPC synthesis from a residual file

Description:
  This program does linear prediction synthesis given a file of residual
  samples and a file of linear prediction coefficients.  The output is an
  audio file containing the reconstructed signal.

  The steps involved in forming the linear prediction coefficients are as
  follows.
   1: Use the all-pole LPC filter to filter the residual signal
   2: Deemphasize the reconstructed signal

Options:
  Input file name: AFileI:
      The environment variable AUDIOPATH specifies a list of directories to be
      searched for the input residual file.
  Output file name: AFileO:
      This name specifies the optional output file containing the reconstructed
      signal.  Specifying "-" as the output file indicates that output is to be
      written to standard output.
  -p PFile, --parameter_file=PFile
      Parameter file.
  -c LPFile, --lpc_file=LPFile
      LPC predictor coefficient file.
  -s, --statistics
      Print frame-by-frame statistics.
  -I INFO, --info=INFO
      Audio file information string for the output file.
  -h, --help
      Print a list of options and exit.
  -v, --version
      Print the version number and exit.

  The analysis parameters are read from the parameter file.
    preemphasis_factor = float  ! preemphasis factor (0 to 1, default 0)
    window_length = int         ! analysis window length
    window_offset = int         ! initial offset of the center of the analysis
                                ! window from the center of the frame (negative
                                ! values mean that the analysis window precedes
                                ! frame, default  0)
    window_type = char          ! window type (Hamming or rectangular, default
                                ! Hamming)
    frame_length = int          ! frame size
    LPC_number = int            ! number of LPC coefficients
    LPC_BW_expansion = float    ! bandwidth expansion factor (1 gives no
                                ! bandwidth expansion, default 1)

  By default, the output file contains a standard audio file information
  string.
    Standard Audio File Information:
       date: 1994-01-25 19:19:39 UTC    date
       program: LPsyn                   program name
       parameters: <parameters>         list of parameters
  This information can be changed with the header information string which is
  specified as one of the command line options.  Structured information records
  should adhere to the above format with a named field terminated by a colon,
  followed by numeric data or text.  Comments can follow as unstructured
  information.
    Record delimiter: Newline character or the two character escape
        sequence "\" + "n".
    Line delimiter: Within records, lines are delimiteded by a carriage
        control character, the two character escape sequence "\" + "r",
        or the two character sequence "\" + newline.
  If the information string starts with a record delimiter, the header
  information string is appended to the standard header information.  If not,
  the user supplied header information string appears alone.

Environment variables:
  AUDIOPATH:
  This environment variable specifies a list of directories to be searched when
  opening the input audio files.  Directories in the list are separated by
  colons (semicolons for Windows).

Author / version:
  P. Kabal / v3r0a  2003-11-03  Copyright (C) 2006

-------------------------------------------------------------------------*/

#include <stdlib.h>
#include <string.h>

#include <libtsp.h>
#include <libtsp/AFpar.h>
#include "LPsyn.h"

#define MAXHEADER	1024


int
main (int argc, const char *argv[])

{
  AFILE *AFpI, *AFpL, *AFpO;
  FILE *fpout;
  char Fname[4][FILENAME_MAX];
  long int Nsamp, Nchan;
  int Lwin, Woffs, Lframe, Np;
  const float *Win;
  double Sfreq, pre, bwexp, Frate;
  long int Npx, Ntcoef;

/* Get the input parameters */
  LSoptions (argc, argv, Fname);

/* If output is to stdout, use stderr for informational messages */
  if (strcmp (Fname[1], "-") == 0)
    fpout = stderr;
  else
    fpout = stdout;

/* Read the analysis parameters */
  LSlpcPar (Fname[0], &pre, &Win, &Lwin, &Woffs, &Lframe, &Np, &bwexp);

/* Open the input residual file */
  FLpathList (Fname[1], "$AUDIOPATH", Fname[1]);
  AFpI = AFopnRead (Fname[1], &Nsamp, &Nchan, &Sfreq, fpout);
  if (Nchan != 1)
    UThalt ("%s: Multiple input channels not supported", PROGRAM);

/* Open the LPC coefficient file */
  FLpathList (Fname[2], "$AUDIOPATH", Fname[2]);
  AFpL = AFopnRead (Fname[2], &Ntcoef, &Npx, &Frate, fpout);
  if (Np != Npx)
    UThalt ("%s: No. LPC coefficients does not match parameter file", PROGRAM);
  if (Frate * Lframe != Sfreq)
    UThalt ("%s: Frame rate, frame length and sample rate are incompatible",
	    PROGRAM);
  if (Ntcoef * Lframe != Np * Nsamp)
    UThalt ("%s: No. residual samples incompatible with no. frames", PROGRAM);
  fprintf (fpout, "\n");

/* Open the output audio file */
  if (Fname[3][0] != '\0') {
    if (strcmp (Fname[3], "-") != 0)
      FLbackup (Fname[3]);
    AFpO = AFopnWrite (Fname[3], FTW_AU, FD_INT16, 1L, Sfreq, fpout);
  }
  else
    AFpO = NULL;

/* Process the residual file */
  LSlpcSyn (AFpI, AFpL, AFpO, pre, Lframe, Np);

/* Close the files */
  AFclose (AFpI);
  AFclose (AFpL);
  if (AFpO != NULL)
    AFclose (AFpO);

  return EXIT_SUCCESS;
}
