/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  GenNoise [options] AFile

Purpose:
  Generate an audio file containing uncorrelated Gaussian noise samples

Description:
  This program generates zero-mean uncorrelated pseudo-random Gaussian
  deviates.  These white noise samples are written to an audio file.  Each
  invocation of this program generates a different random sequence.  Options
  are available to specify the number of samples, the sampling frequency and
  the data format of the output file.

Options:
  Output file name: AFile:
      Specifying "-" as the output file indicates that output is to be written
      to standard output.  If the output file type is not explicitly given (-F
      option), the extension of the output file name is used to determine the
      file type.
        ".au"   - AU audio file
        ".wav"  - WAVE file
        ".aif"  - AIFF sound file
        ".afc"  - AIFF-C sound file
        ".raw"  - Headerless file (native byte order)
        ".txt"  - Headerless file (text data)
  -d SDEV, --std_deviation=SDEV
      Standard deviation of the noise samples in normalized units, default
      0.03. The standard deviation can be given as a real number (e.g.,
      "0.03") or as a ratio (e.g., "983.04/32768").
  -x SEED, --seed=SEED
      Seed for the random number generator, default from current time.
  -n NSAMPLE, --number_samples=NSAMPLE
      Number of output samples to be generated.
  -s SFREQ, --srate=SFREQ
      Sampling frequency for the output audio file, default 8000.
  -F FTYPE, --file_type=FTYPE
      Output file type.  If this option is not specified, the file type is
      determined by the output file name extension.
        "AU" or "au"             - AU audio file
        "WAVE" or "wave"         - WAVE file
        "WAVE-NOEX" or "wave-noex" - WAVE file (no extensible data)
        "AIFF-C" "aiff-c"        - AIFF-C sound file
        "AIFF" or "aiff"         - AIFF sound file
        "noheader" or "noheader_native" - Headerless file (native byte
                                   order)
        "noheader_swap"          - Headerless file (byte swapped)
        "noheader_big-endian"    - Headerless file (big-endian byte
                                   order)
        "noheader_little-endian" - Headerless file (little-endian byte
                                   order)
  -D DFORMAT, --data_format=DFORMAT
      Data format for the output file, default integer16
        "mu-law8"   - 8-bit mu-law data
        "A-law8"    - 8-bit A-law data
        "unsigned8" - offset-binary 8-bit integer data
        "integer8"  - two's-complement 8-bit integer data
        "integer16" - two's-complement 16-bit integer data
        "integer24" - two's-complement 24-bit integer data
        "integer32" - two's-complement 32-bit integer data
        "float32"   - 32-bit floating-point data
        "float64"   - 64-bit floating-point data
        "text"      - text data
      The data formats available depend on the output file type.
      AU audio files:
        mu-law, A-law, 8/16/24/32-bit integer, 32/64-bit float
      WAVE files:
        mu-law, A-law, offset-binary 8-bit integer, 16/24/32-bit integer,
        32/64-bit float
      AIFF-C sound files:
        mu-law, A-law, 8/16/24/32-bit integer, 32/64-bit float
      AIFF sound files:
        8/16/24/32-bit integer
      Headerless files:
        all data formats
  -I INFO, --info=INFO
      Audio file information string for the output file.
  -h, --help
      Print a list of options and exit.
  -v, --version
      Print the version number and exit.

  By default, the output file contains a standard audio file information
  string.
    Standard Audio File Information:
       date: 1994-01-25 19:19:39 UTC    date
       program: GenNoise                program name
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

Author / version:
  P. Kabal / v5r0a  2003-11-06  Copyright (C) 2006

-------------------------------------------------------------------------*/

#include <stdlib.h>	/* EXIT_SUCCESS */
#include <string.h>

#include <libtsp.h>
#include <libtsp/AFpar.h>
#include <AO.h>
#include "GenNoise.h"

#define MINV(a, b)	(((a) < (b)) ? (a) : (b))
#define MAXBUF	5120


int
main (int argc, const char *argv[])

{
  struct GN_FOpar FO;
  AFILE *AFp;
  FILE *fpinfo;
  int i, n, seed, Ftype, Dformat;
  long int k;
  double rms;
  float x[MAXBUF];

/* Get the input parameters */
  GNoptions (argc, argv, &rms, &seed, &FO);

/* If output is to stdout, use stderr for informational messages */
  if (strcmp (FO.Fname, "-") == 0)
    fpinfo = stderr;
  else
    fpinfo = stdout;

/* Open the output file */
  Ftype = AOsetFtype (&FO);
  Dformat = AOsetDformat (&FO, NULL, 0);
  AOsetFOopt (&FO);
  if (strcmp (FO.Fname, "-") != 0)
    FLbackup (FO.Fname);
  AFp = AFopnWrite (FO.Fname, Ftype, Dformat, 1L, FO.Sfreq, fpinfo);

/* Generate the noise samples */
  MSrandSeed (seed);
  k = 0;
  while (k < FO.Nframe) {
    n = (int) MINV (FO.Nframe - k, MAXBUF);
    for (i = 0; i < n; ++i)
      x[i] = (float) MSfGaussRand (rms);
    k += n;
    AFfWriteData (AFp, x, n);
  }

/* Close the audio file */
  AFclose (AFp);

  return EXIT_SUCCESS;
}
