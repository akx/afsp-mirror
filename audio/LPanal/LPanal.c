/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  LPanal [options] -p PFile -c LPFile AFileI [AFileO]

Purpose:
  LPC analysis on a speech file

Description:
  This program does linear prediction analysis on an audio file.  The output
  file is a file of linear prediction coefficients and the residual file.

  The steps involved in forming the linear prediction coefficients are as
  follows.
   1: Preemphasize the input signal (first difference filter)
   2: Window the data
   3: Calculate the autocorrelation coefficients for the windowed data
   4: Calculate the LPC coefficients
   5: Bandwidth expand the LPC coefficients

  The steps involved in forming the residual signal are as follows.
   1: Preemphasize the input signal (first difference filter)
   2: Use the LPC coefficients to form the prediction error residual signal.

Options:
  Input file name: AFileI:
      The environment variable AUDIOPATH specifies a list of directories to be
      searched for the input audio file.
  Output file name: AFileO:
      This name specifies the optional output file containing the linear
      prediction residual.  Specifying "-" as the output file indicates that
      output is to be written to standard output.
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
       program: LPanal                  program name
       parameters: <parameters>         list of analysis parameters
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

#include <stdlib.h>	/* EXIT_SUCCESS */
#include <string.h>

#include <libtsp.h>
#include <libtsp/AFpar.h>
#include "LPanal.h"


int
main (int argc, const char *argv[])

{
  char Fname[4][FILENAME_MAX];
  AFILE *AFpI, *AFpL, *AFpO;
  FILE *fpinfo;
  long int Nsamp, Nchan;
  int Lwin, Woffs, Lframe, Np;
  const float *Win;
  double Sfreq, pre, bwexp;
  char Info[MAXINFO+1];

/* Get the input parameters */
  LPoptions (argc, argv, Fname);

/* If output is to stdout, use stderr for informational messages */
  if (strcmp (Fname[2], "-") == 0 || strcmp (Fname[3], "-") == 0)
    fpinfo = stderr;
  else
    fpinfo = stdout;

/* Read the analysis parameters */
  LPlpcPar (Fname[0], &pre, &Win, &Lwin, &Woffs, &Lframe, &Np, &bwexp);

/* Open the input audio file */
  AFsetNHpar ("$NOHEADER_AUDIOFILE");
  FLpathList (Fname[1], "$AUDIOPATH", Fname[1]);
  AFpI = AFopnRead (Fname[1], &Nsamp, &Nchan, &Sfreq, fpinfo);
  if (Nchan != 1)
    UThalt ("%s: Multiple input channels not supported", PROGRAM);

/* Set the header information string */
  STcopyMax ((AFoptions ())->Uinfo, Info, MAXINFO);
  AFsetInfo (NULL);
  fprintf (fpinfo, "\n");

/* Open the LPC coefficient file */
  AFsetInfo (Info);		/* Header string including parameters */
  if (strcmp (Fname[2], "-") != 0)
    FLbackup (Fname[2]);
  AFpL = AFopnWrite (Fname[2], FTW_AU, FD_FLOAT32, Np, Sfreq/Lframe, fpinfo);

/* Open the output residual file */
  AFsetInfo (Info);
  if (Fname[3] != NULL) {
    if (strcmp (Fname[3], "-") != 0)
      FLbackup (Fname[3]);
    AFpO = AFopnWrite (Fname[3], FTW_AU, FD_FLOAT32, 1L, Sfreq, fpinfo);
  }
  else
    AFpO = NULL;

/* Process the input file */
  LPlpcAnal (AFpI, AFpL, AFpO, pre, Win, Lwin, Woffs, Lframe, Np, bwexp);

/* Close the files */
  AFclose (AFpI);
  AFclose (AFpL);
  if (AFpO != NULL)
    AFclose (AFpO);

  return EXIT_SUCCESS;
}
