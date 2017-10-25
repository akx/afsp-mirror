/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  GenNoise [options] AFile

Purpose:
  Generate an audio file containing uncorrelated Gaussian noise samples

Description:
  This program generates zero-mean uncorrelated pseudo-random Gaussian
  deviates.  These white noise samples are written to an audio file.  Each
  invocation of this program generates a different random sequence unless the
  random number generator seed is specified.  Options are available to specify
  the number of samples, the sampling frequency and the data format of the
  output file.

Options:
  Output file name, AFile:
      Specifying "-" as the output file indicates that output is to be written
      to standard output.  If the output file type is not explicitly given (-F
      option), the extension of the output file name is used to determine the
      file type.
        ".au"   - AU audio file
        ".wav"  - WAVE file
        ".aif"  - AIFF sound file
        ".afc"  - AIFF-C sound file
        ".raw"  - Headerless file (native byte order)
        ".txt"  - Text audio file (with header)
  -d SDEV, --std_deviation=SDEV
      Standard deviation of the noise samples in normalized units, default 0.03.
      The standard deviation can be given as a real number (e.g., "0.03") or as
      a ratio (e.g., "983.04/32768").
  -x SEED, --seed=SEED
      Integer-valued seed for the random number generator, default from the
      current time.
  -n NSAMPLE, --number_samples=NSAMPLE
      Number of output samples to be generated.
  -s SFREQ, --srate=SFREQ
      Sampling frequency for the output audio file, default 8000.
  -F FTYPE, --file-type=FTYPE
      Output file type.  If this option is not specified, the file type is
      determined by the output file name extension.
        "AU" or "au"             - AU audio file
        "WAVE" or "wave"         - WAVE file. Whether or not to use the WAVE
                                   file extensible format is automatically
                                   determined.
        "WAVE-EX" or "wave-ex"   - WAVE file. Use the WAVE file extensible
                                   format.
        "WAVE-NOEX" or "wave-noex" - WAVE file; do not use the WAVE file
                                   extensible format
        "AIFF-C" or "aiff-c"     - AIFF-C sound file
        "AIFF-C/sowt" or "aiff-c/sowt" - AIFF-C (byte-swapped data)
        "AIFF" or "aiff"         - AIFF sound file
        "noheader" or "noheader-native" - Headerless file (native byte order)
        "noheader-swap"          - Headerless file (byte swapped)
        "noheader-big-endian"    - Headerless file (big-endian byte order)
        "noheader-little-endian" - Headerless file (little-endian byte order)
        "text-audio"             - Text audio file (with header)
  -D DFORMAT, --data-format=DFORMAT
      Data format for the output file.
        "mu-law8"   - 8-bit mu-law data
        "A-law8"    - 8-bit A-law data
        "unsigned8" - offset-binary 8-bit integer data
        "integer8"  - two's-complement 8-bit integer data
        "integer16" - two's-complement 16-bit integer data
        "integer24" - two's-complement 24-bit integer data
        "integer32" - two's-complement 32-bit integer data
        "float32"   - 32-bit IEEE floating-point data
        "float64"   - 64-bit IEEE floating-point data
        "text16"    - text data, scaled the same as 16-bit integer data
        "text"      - text data, scaled the same as float/double data
  -I INFO, --info=INFO
      Add an information record to the output audio file.
  -h, --help
      Print a list of options and exit.
  -v, --version
      Print the version number and exit.

  See routine CopyAudio for a description of other parameters.
  -D DFORMAT, --data-format=DFORMAT
      More details on allowed data formats for the output file
  -I INFO, --info-INFO
      Details on usage and default information records
  -S SPEAKERS, --speakers=SPEAKERS
      Loudspeaker configuration
Author / version:
  P. Kabal / v10r1  2017-10-18  Copyright (C) 2017

-------------------------------------------------------------------------*/

#include <stdlib.h> /* EXIT_SUCCESS */
#include <string.h>

#include <libtsp.h>
#include <AFpar.h>
#include <AO.h>
#include "GenNoise.h"

#define MINV(a, b)  (((a) < (b)) ? (a) : (b))
#define MAXBUF  5120


int
main (int argc, const char *argv[])

{
  struct GN_FOpar FO;
  AFILE *AFp;
  FILE *fpinfo;
  int i, n, seed;
  long int k;
  double rms;
  double x[MAXBUF];

/* Get the input parameters */
  GNoptions (argc, argv, &rms, &seed, &FO);

/* If output is to stdout, use stderr for informational messages */
  if (strcmp (FO.Fname, "-") == 0)
    fpinfo = stderr;
  else
    fpinfo = stdout;

/* Open the output file */
  AOsetDFormat (&FO, NULL, 0);
  AOsetFOopt (&FO);
  if (strcmp (FO.Fname, "-") != 0)
    FLbackup (FO.Fname);
  AFp = AFopnWrite (FO.Fname, FO.FtypeW, FO.DFormat.Format, 1L, FO.Sfreq, fpinfo);

/* Generate the noise samples */
  MSrandSeed (seed);
  k = 0;
  while (k < FO.Nframe) {
    n = (int) MINV (FO.Nframe - k, MAXBUF);
    for (i = 0; i < n; ++i)
      x[i] = MSfGaussRand (rms);
    k += n;
    AFdWriteData (AFp, x, n);
  }

/* Close the audio file */
  AFclose (AFp);

  return EXIT_SUCCESS;
}
