/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  GenTone [options] AFile

Purpose:
  Generate an audio file containing a sine wave

Description:
  This program generates a sine wave of a given amplitude and phase.  The
  samples of the sine wave are written to an audio file.  Options are available
  to specify the number of samples, the sampling frequency and the data format
  of the output file.

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
  -f FREQ, --frequency=FREQ
      Frequency of the sine wave in Hertz, default 1000.
  -r SDEV, --rms=SDEV
      Root-mean-square value of the sinusoid in normalized units, default 0.03.
      The rms value can be given as a real number (e.g., "0.03") or as a ratio
      (e.g., "983.04/32768").
  -a AMPL, --amplitude=AMPL
      Maximum amplitude of the sine wave in normalized units.  The amplitude
      of the sine wave is specified by either the rms value or the maximum
      amplitude. The amplitude can be given as a real number (e.g., "0.0424")
      or as a ratio (e.g., "1390.2/32768"). The default amplitude corresponds
      to an RMS value of 0.03.
  -p PHASE, --phase PHASE
      Initial phase of the sinusoid (sine) in degrees, default 0.  Zero phase
      gives a sine, -90 degrees gives a cosine.
  -n NSAMPLE, --number-samples=NSAMPLE
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
        "mu-lawR8"  - 8-bit bit-reversed mu-law data
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
      Audio file information string for the output file.
  -h, --help
      Print a list of options and exit.
  -v, --version
      Print the version number and exit.

  See routine CopyAudio for a description of other parameters.
  -D DFORMAT, --data-format=DFORMAT
      Details on allowed data formats for the output file
  -I INFO, --info-INFO
      Details on usage and default information records
  -S SPEAKERS, --speakers=SPEAKERS
      Loudspeaker configuration

Author / version:
  P. Kabal / v10r2  2018-11-16  Copyright (C) 2018

-------------------------------------------------------------------------*/

#include <float.h>  /* DBL_EPSILON */
#include <limits.h>
#include <math.h>
#include <stdlib.h> /* EXIT_SUCCESS */
#include <string.h>

#include <libtsp.h>
#include <AFpar.h>
#include <AO.h>
#include "GenTone.h"

#define ABSV(x)   (((x) < 0) ? -(x) : (x))
#define MINV(a, b)  (((a) < (b)) ? (a) : (b))
#define PIx2    6.28318530717958647693

#define MAXBUF  5120


int
main (int argc, const char *argv[])

{
  struct GT_FOpar FO;
  AFILE *AFp;
  FILE *fpinfo;
  long int N, D, PND;
  int i, n;
  long int k;
  double Sratio, Xratio;
  double x[MAXBUF];
  struct GT_Sine Sine;

/* Get the input parameters */
  GToptions (argc, argv, &Sine, &FO);

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

/* Resolve the normalized sine wave frequency as a ratio of integers.
   The phase advance can then be calculated as a ratio of integers, with no
   accumulation of phase error.
*/
  Sratio = Sine.Freq / FO.Sfreq;
  MSratio (Sratio, &N, &D, 0.0, LONG_MAX / 2, LONG_MAX);
  Xratio = (double) N / D;
  if (ABSV (Sratio - Xratio) >= ABSV (Sratio) * DBL_EPSILON)
    fprintf (fpinfo, GTMF_SineFreq, Xratio * FO.Sfreq);
  N = (N % D);  /* Alias frequency */

/* Generate the sine wave samples */
  k = 0;
  PND = 0;
  while (k < FO.Nframe) {
    n = (int) MINV (FO.Nframe - k, MAXBUF);
    for (i = 0; i < n; ++i) {
      x[i] = Sine.Ampl * sin ((PIx2 / D) * PND + Sine.Phase);
      PND += N;
      if (PND > D)
        PND -= D;
    }
    k += n;
    AFdWriteData (AFp, x, n);
  }

/* Close the audio file */
  AFclose (AFp);

  return EXIT_SUCCESS;
}
