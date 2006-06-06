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
  Subsequent records contain filter coefficients.  Comment records ('!' in the
  first position of the record) can be interspersed amongst the data.  Data
  records are free form, with data values separated by white space (blanks,
  tabs and newlines).  Commas can also be used to separate data values, but
  only within records, i.e. a comma should not appear at the end of a record.

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
  be zero, except for symmetric or anti-symmetric FIR filters. In those cases
  the default alignment is Ncof/2-1 for even length filters and (Ncof-1)/2 for
  odd length filters.  If the number of output samples is not explicitly set,
  it is chosen to be Ir*Nin/Nsub.  For the case of Ir=1 and Nsub=1, this 
  results in the same number of output samples as input samples.  If the
  initial filter alignment, offs, is explicitly specified, the number of output
  samples is chosen to be (Ir*Nin-offs)/Nsub.  This value can be overridden by
  explicitly setting the number of output samples.

Options:
  Input file name: AFileI:
      The environment variable AUDIOPATH specifies a list of directories to be
      searched for the input audio file.  Specifying "-" as the input file
      indicates that input is from standard input.
  Output file name: AFileO:
      The second file name is the output file.  Specifying "-" as the output
      file name indicates that output is to be written to standard output.  If
      the output file type is not explicitly given (-F option), the extension
      of the output file name is used to determine the file type.
        ".au"   - AU audio file
        ".wav"  - WAVE file
        ".aif"  - AIFF sound file
        ".afc"  - AIFF-C sound file
        ".raw"  - Headerless file (native byte order)
        ".txt"  - Headerless file (text data)
  -f FILTFILE, --filter_file=FILTFILE
      Filter file name.
  -i IR/NSUB, --interpolate=IR/NSUB
      Filter interpolation ratio, default 1.  The interpolation and subsampling
      factors are specified as a fraction Ir/Nsub.  Interpolation can only be
      used with FIR filters.
  -a OFFS, --alignment=OFFS
      Alignment of data relative to the filter.  The first output sample is
      calculated with the beginning of the filter response aligned with the
      specified sample of the interpolated data sequence.
  -n NSAMPLE, --number_samples=NSAMPLE
      Number of samples (per channel) for the output file.
  -t FTYPE, --type=FTYPE
      Input audio file type.  In the default automatic mode, the input file
      type is determined from the file header.  For input from a non-random
      access file (e.g. a pipe), the input file type can be explicitly
      specified to avoid the lookahead used to read the file header.  See the
      description of the environment variable AF_FILETYPE below for a list of
      file types.
  -P PARMS, --parameters=PARMS
      Parameters to be used for headerless input files.  See the description
      of the environment variable AF_NOHEADER below for the format of the
      parameter specification.
  -g GAIN, --gain=GAIN
      A gain factor applied to the data from the input file.  This gain applies
      to all channels in a file.  The gain value can be given as a real number
      (e.g., "0.003") or as a ratio (e.g., "1/256").
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
      Data format for the output file.
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
  -S SPEAKER, --speaker=SPEAKER
      Mapping of the output channel to a speaker position.  The spacial
      position of the loudspeakers is one of the following.
        "FL"  - Front Left
        "FR"  - Front Right
        "FC"  - Front Center
        "LF"  - Low Frequency
        "BL"  - Back Left
        "BR"  - Back Right
        "FLC" - Front Left of Center
        "FRC" - Front Right of Center
        "BC"  - Back Center
        "SL"  - Side Left
        "SR"  - Side Right
        "TC"  - Top Center
        "TFL" - Top Front Left
        "TFC" - Top Front Center
        "TFR" - Top Front Right
        "TBL" - Top Back Lefty
        "TBC" - Top Back Center
        "TBR" - Top Back Right
        "-"   - none
  -I INFO, --info=INFO
      Audio file information string for the output file.
  -h, --help
      Print a list of options and exit.
  -v, --version
      Print the version number and exit.

  By default, the output file contains a standard audio file information
  string.
    Standard Audio File Information:
       date: 2001-01-25 19:19:39 UTC    date
       program: FiltAudio               program name
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
  AF_FILETYPE:
  This environment variable defines the input audio file type.  In the default
  mode, the input file type is determined from the file header.
    "auto"       - determine the input file type from the file header
    "AU" or "au" - AU audio file
    "WAVE" or "wave" - WAVE file
    "AIFF" or "aiff" - AIFF or AIFF-C sound file
    "noheader"   - headerless (non-standard or no header) audio file
    "SPHERE"     - NIST SPHERE audio file
    "ESPS"       - ESPS sampled data feature file
    "IRCAM"      - IRCAM soundfile
    "SPPACK"     - SPPACK file
    "INRS"       - INRS-Telecom audio file
    "SPW"        - Comdisco SPW Signal file
    "CSL" or "NSP" - CSL NSP file
    "text"       - Text audio file

  AF_NOHEADER:
  This environment variable defines the data format for headerless or
  non-standard input audio files.  The string consists of a list of parameters
  separated by commas.  The form of the list is
    "Format, Start, Sfreq, Swapb, Nchan, ScaleF"
  Format: File data format
       "undefined" - Headerless files will be rejected
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
  Start: byte offset to the start of data (integer value)
  Sfreq: sampling frequency in Hz (floating point number)
  Swapb: Data byte swap parameter
       "native"        - no byte swapping
       "little-endian" - file data is in little-endian byte order
       "big-endian"    - file data is in big-endian byte order
       "swap"          - swap the data bytes as the data is read
  Nchan: number of channels
    The data consists of interleaved samples from Nchan channels
  ScaleF: Scale factor
       "default" - Scale factor chosen appropriate to the type of data. The
          scaling factors shown below are applied to the data in the file.
          8-bit mu-law:    1/32768
          8-bit A-law:     1/32768
          8-bit integer:   128/32768
          16-bit integer:  1/32768
          24-bit integer:  1/(256*32768)
          32-bit integer:  1/(65536*32768)
          float data:      1
       "<number or ratio>" - Specify the scale factor to be applied to
          the data from the file.
  The default values for the audio file parameters correspond to the following
  string.
    "undefined, 0, 8000., native, 1, default"

  AUDIOPATH:
  This environment variable specifies a list of directories to be searched when
  opening the input audio files.  Directories in the list are separated by
  colons (semicolons for Windows).

Author / version:
  P. Kabal / v6r1  2005-02-01  Copyright (C) 2006

-------------------------------------------------------------------------*/

#include <stdlib.h>	/* EXIT_SUCCESS */
#include <string.h>

#include <libtsp.h>
#include <libtsp/AFpar.h>
#include <libtsp/FIpar.h>
#include <AO.h>
#include "FiltAudio.h"

#define CHECKSYM(x,N)	((int) (1.00001 * VRdCorSym(x,N)))


int
main (int argc, const char *argv[])

{
  struct FA_FIpar FI;
  struct FA_FFpar FF;
  struct FA_FOpar FO;
  AFILE *AFpI, *AFpO;
  FILE *fpinfo;
  int Ftype, Dformat;
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
  AFpI->ScaleF *= FI.Gain;	/* Gain absorbed into scaling factor */

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
  Ftype = AOsetFtype (&FO);
  Dformat = AOsetDformat (&FO, &AFpI, 1);
  AOsetFOopt (&FO);
  if (strcmp (FO.Fname, "-") != 0)
    FLbackup (FO.Fname);
  AFpO = AFopnWrite (FO.Fname, Ftype, Dformat, 1L, SfreqO, fpinfo);

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
