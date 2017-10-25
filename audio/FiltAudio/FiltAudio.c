/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  FiltAudio [options] -f FilterFile AFileI AFileO

Purpose:
  Filter data from an audio file

Description:
  This program takes an audio file and a filter coefficient file as input and
  produces a filtered output audio file.  Subsampling and interpolation factors
  can be optionally specified.  This program supports three types of filters,
  FIR, all-pole, and general IIR.  Filters are specified in filter files.

  Filter Files:
  The first record of a filter file indicates the type of filter.
    !FIR  - FIR filter, direct form
    !IIR  - IIR filter, cascade of biquad sections
    !ALL  - All-pole filter, direct form
  Subsequent records contain text lines with filter coefficients.  Comment
  records ('!' in the first position of the record) can be interspersed amongst
  the data.  Data records are free form, with data values separated by
  white-space (blanks, tabs and newlines).  Commas can also be used to separate
  data values, but only within records, i.e. a comma should not appear at the
  end of a record.

  FIR filters are specified by the direct-form coefficients h[i],
           N-1       -i
    H(z) = SUM h[i] z    .
           i=0

  IIR filters are implemented as the cascade of biquadratic filter sections,
  where each section has a z-transform,
             h(i,0)*z^2 + h(i,1)*z + h(i,2)
    H(i,z) = ------------------------------ .
                z^2 + h(i,3)*z + h(i,4)


  All-pole filters are specified by direct-form feedback coefficients,
            1                    N-1       -i
    H(z) = ----  ,  where C(z) = SUM h[i] z    .
           C(z)                  i=0

  For FIR filters, a sample rate change can be affected with interpolation
  and subsampling.  For IIR and all-pole filters, the output can be subsampled.
  Let Ir and Nsub be the interpolation and subsampling factors, respectively.
  Conceptually, the rate change is accomplished as follows.
   1: Ir-1 zeros are inserted between adjacent samples of the frequency shifted
      input to increase the sampling rate by a factor of Ir.
   2: The increased rate signal is filtered.
   3: The result of the filtering is subsampled by a factor of Nsub to form the
      output signal.

  The initial filter alignment and the number of output samples can be
  specified with options.  The filter alignment specifies the position of the
  filter relative to the input data for calculating the first output sample.
  For FIR filters, this alignment is relative to the increased rate input
  sequence.  Specifically, let the number of samples in the input file be Nin.
  The input can be considered to be an array x(0),...,x(Nin-1).  The increased
  rate sequence is xi(.), with xi(k*Ir)=x(k).  The first output sample is
  calculated with the beginning of the impulse response of the filter aligned
  with xi(idoffs).  The array xi(.) can be considered to be of length Nin*Ir;
  the first non-zero sample is xi(0)=x(0), the last non-zero sample is
  xi((Nin-1)*Ir).  Conceptually, the impulse impulse response is moved in
  steps of Nsub to create the output samples.

  The intent is that the output samples are the subset of the values the would
  be obtained if the infinite length sequence formed by padding out the input
  data on either end with zeros were to be filtered.  To this end, the filter
  calculations need warm-up points, particularly for the case that the initial
  filter alignment is not at the beginning of the input data.  For FIR filters,
  this is taken into account by reading previous input values into the filter
  memory.  For IIR and all-pole filters, previous outputs are also needed as
  warm-up points.  If the initial alignment is near the beginning of the data,
  the IIR filter is run from the beginning of the data to generate the warm-up
  points.  For larger alignment offsets, the IIR filter is backed up for a
  maximum of 1000 samples to provide the warm-up points.

  If the initial filter alignment is not explicitly specified, it is chosen to
  be zero, except for symmetric or anti-symmetric FIR filters.  In those cases
  the default alignment is Ncof/2-1 for even length filters and (Ncof-1)/2 for
  odd length filters.  If the number of output samples is not explicitly set,
  it is chosen to be Ir*Nin/Nsub.  For the case of Ir=1 and Nsub=1, this
  results in the same number of output samples as input samples.  If the
  initial filter alignment, offs, is explicitly specified, the number of output
  samples is chosen to be (Ir*Nin-offs)/Nsub.  This value can be overridden by
  explicitly setting the number of output samples.

Options:
  Input file name, AFileI:
      The environment variable AUDIOPATH specifies a list of directories to be
      searched for the input audio file.  Specifying "-" as the input file
      indicates that input is from standard input.
  Output file name, AFileO:
      The second file name is the output file.  Specifying "-" as the output
      file name indicates that output is to be written to standard output.  If
      the output file type is not explicitly given (-F option), the extension
      of the output file name is used to determine the file type.
        ".au"   - AU audio file
        ".wav"  - WAVE file
        ".aif"  - AIFF sound file
        ".afc"  - AIFF-C sound file
        ".raw"  - Headerless file (native byte order)
        ".txt"  - Text audio file (with header)
  -f FILTFILE, --filter_file=FILTFILE
     Text file containing filter coefficients.  The header of the file indicates
     the type of filter as described above.
  -i IR/NSUB, --interpolate=IR/NSUB
      Filter interpolation ratio, default 1.  The interpolation and subsampling
      factors are specified as a fraction Ir/Nsub.  Interpolation can only be
      used with FIR filters.
  -a OFFS, --alignment=OFFS
      Alignment of data relative to the filter.  The first output sample is
      calculated with the beginning of the filter response aligned with the
      specified sample of the interpolated data sequence.
  -g GAIN, --gain=GAIN
      A gain factor applied to the data from the INPUT file.  This gain applies
      to all channels in a file.  The gain value can be given as a real number
      (e.g., "0.003") or as a ratio (e.g., "1/256").
  -n NSAMPLE, --number_samples=NSAMPLE
      Number of samples (per channel) for the OUTPUT file.  If not specified,
      the number of samples is determined automatically.
  -F FTYPE, --file-type=FTYPE
      OUTPUT file type.  If this option is not specified, the file type is
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
      Data format for the OUTPUT file.
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
      The data formats available depend on the output file type.
        AU audio files:
          mu-law, A-law, 8/16/24/32-bit integer, 32/64-bit float
        WAVE files:
          mu-law, A-law, offset-binary 8-bit, 16/24/32-bit integer,
          32/64-bit float
        AIFF-C sound files:
          mu-law, A-law, 8/16/24/32-bit integer, 32/64-bit float
        AIFF and AIFF-C/sowt sound files:
          8/16/24/32-bit integer
        Headerless files:
          all data formats
        Text audio files:
          tex16, text
  -I INFO, --info=INFO
      Add an information record to the OUTPUT audio file.
  -h, --help
      Print a list of options and exit.
  -v, --version
      Print the version number and exit.

  See routine CopyAudio for a description of other parameters.
  -t FTYPE, --type=FTYPE
      Input file type and environment variable AF_FILETYPE
  -P PARMS, --parameters=PARMS
      Input file parameters and environment variable AF_INPUTPAR
  -D DFORMAT, --data-format=DFORMAT
      More details on allowed data formats for the output file
  -I INFO, --info-INFO
      Details on usage and default information records
  -S SPEAKERS, --speakers=SPEAKERS
      Loudspeaker configuration

Environment variables:
  AUDIOPATH:
    This environment variable specifies a list of directories to be searched
    when opening the input audio files.  Directories in the list are separated
    by colons (semicolons for Windows).

Author / version:
  P. Kabal / v10r1  2017-10-18  Copyright (C) 2017

-------------------------------------------------------------------------*/

#include <stdlib.h> /* EXIT_SUCCESS */
#include <string.h>

#include <libtsp.h>
#include <AFpar.h>
#include <libtsp/FIpar.h>
#include <AO.h>
#include "FiltAudio.h"

#define CHECKSYM(x,N) ((int) (1.00001 * VRdCorSym(x,N)))


int
main (int argc, const char *argv[])

{
  struct FA_FIpar FI;
  struct FA_FFpar FF;
  struct FA_FOpar FO;
  AFILE *AFpI, *AFpO;
  FILE *fpinfo;
  int FiltType, Ncof, Nsec;
  long int Nsamp, Nchan;
  double SfreqO;
  double SfreqI;
  double h[MAXCOF];

/* Get the input parameters */
  FAoptions (argc, argv, &FI, &FF, &FO);

/* If output is to stdout, use stderr for informational messages */
  if (strcmp (FO.Fname, "-") == 0)
    fpinfo = stderr;
  else
    fpinfo = stdout;

/* Open the input audio file */
  AOsetFIopt (&FI, 0, 0);
  FLpathList (FI.Fname, AFPATH_ENV, FI.Fname);
  AFpI = AFopnRead (FI.Fname, &Nsamp, &Nchan, &SfreqI, fpinfo);
  if (Nchan != 1)
    UThalt ("%s: %s", PROGRAM, FAM_XNchan);
  AFpI->ScaleF *= FI.Gain;  /* Gain absorbed into scaling factor */

/* Read the coefficient file */
  FiltType = FIdReadFilt (FF.Fname, MAXCOF, h, &Ncof, fpinfo);
  if (FiltType != FI_IIR && FiltType != FI_FIR && FiltType != FI_ALL)
    UThalt ("%s: %s", PROGRAM, FAM_BadFiltType);
  if (Ncof <= 0)
    UThalt ("%s: %s", PROGRAM, FAM_NoCoef);
  if (FiltType == FI_ALL && FF.Ir != 1)
    UThalt ("%s: %s", PROGRAM, FAM_APNoInt);
  if (FiltType == FI_IIR && FF.Ir != 1)
    UThalt ("%s: %s", PROGRAM, FAM_IIRNoInt);
  fprintf (fpinfo, "\n");

/* Open the output audio file */
  if (FO.SpkrConfig == NULL)
    FO.SpkrConfig = AFpI->SpkrConfig;
  SfreqO = (FF.Ir * (double) SfreqI) / FF.Nsub;
  AOsetDFormat (&FO, &AFpI, 1);
  AOsetFOopt (&FO);
  if (strcmp (FO.Fname, "-") != 0)
    FLbackup (FO.Fname);
  AFpO = AFopnWrite (FO.Fname, FO.FtypeW, FO.DFormat.Format, 1L, SfreqO, fpinfo);

/* Default alignment */
  if (FF.Doffs == DOFFS_DEF) {
    if (FiltType == FI_FIR && CHECKSYM (h, Ncof) != 0) {
      if (Ncof % 2 == 0)
  FF.Doffs = Ncof / 2 - 1;
      else
  FF.Doffs = (Ncof - 1) / 2;
    }
    else
      FF.Doffs = 0;
    if (FO.Nframe == AF_NFRAME_UNDEF)
      FO.Nframe = (FF.Ir * Nsamp) / FF.Nsub;
  }
  else if (FO.Nframe == AF_NFRAME_UNDEF)
    FO.Nframe = (FF.Ir * Nsamp - FF.Doffs) / FF.Nsub;

/* Filtering */
  if (FiltType == FI_FIR) {
    if (FF.Nsub == 1 && FF.Ir == 1)
      FAfiltFIR (AFpI, AFpO, FO.Nframe, h, Ncof, FF.Doffs);
    else
      FAfiltSI (AFpI, AFpO, FO.Nframe, h, Ncof, FF.Nsub, FF.Ir, FF.Doffs);
  }
  else if (FiltType == FI_IIR) {
    Nsec = Ncof / 5;
    FAfiltIIR (AFpI, AFpO, FO.Nframe, (const double (*)[5]) h, Nsec,
         FF.Nsub, FF.Doffs);
  }
  else if (FiltType == FI_ALL)
    FAfiltAP (AFpI, AFpO, FO.Nframe, h, Ncof, FF.Nsub, FF.Doffs);
  else
    UThalt ("%s: %s", PROGRAM, FAM_BadFiltType);

/* Close the audio files */
  AFclose (AFpI);
  AFclose (AFpO);

  return EXIT_SUCCESS;
}
