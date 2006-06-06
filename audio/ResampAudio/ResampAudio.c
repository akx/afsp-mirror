/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  ResampAudio [options] AFileI AFileO

Purpose:
  Resample data from an audio file

Description:
  This program resamples data from an audio file.  This process involves
  interpolating between the samples in the original file to create a new
  sequence of samples with a new spacing (sampling rate).  This program allows
  for an arbitrary ratio of output sampling rate to input rate.  The default
  settings give a high quality sample rate conversion.

Theory of Operation:
  The process used for interpolation depends on the ratio of the output
  sampling rate to the input sampling rate.
   1: If the output sampling rate over the input sampling rate is expressible
      as a ratio of small integers, the sample rate change process is done
      using a conventional interpolation filter designed for the interpolation
      factor (numerator of the sampling rate ratio) followed by subsampling by
      the subsampling factor (denominator of the sampling rate ratio).
   2: For the general case, an interpolating filter is designed using an
      interpolation factor of 24.  For each output sample, the interpolating
      filter is used to create two samples that bracket the desired sampling
      point.  Linear interpolation is used between these values to generate the
      output value.

  The default interpolation filter is a linear phase FIR filter designed by
  applying a Kaiser window to an ideal lowpass filter response.  The filter is
  characterized by a cutoff frequency, a window shape parameter, and the number
  of coefficients.  The window shape parameter (alpha) controls the passband
  ripple and the stopband attenuation.  For a fixed number of coefficients,
  decreasing ripple and increasing attenuation (larger alpha) come at the
  expense of a wider transition width.

  The cutoff of the default interpolation filter depends on the input and
  output sampling rates.  Let fsi be the sampling rate of the input signal and
  fso be the sampling rate of the output signal.
   1: fso > fsi.  The cutoff of the interpolation filter is set to fsi/2.
   2: fso < fsi.  The cutoff of the interpolation filter is set to fso/2.

  The default design aims for an 80 dB stopband attenuation and a transition
  width which is 15% of the cutoff frequency.  The attenuation directly
  determines alpha.  The value of alpha together with the transition width
  determines the number of filter coefficients.

  The parameters of the interpolating filter can also be set by the user.  The
  design parameters are the interpolation factor, the filter cutoff frequency,
  the Kaiser window parameter, and the number of filter coefficients.  The
  following table shows the effect of changing the Kaiser window parameter
  alpha.
     stopband   alpha  transition   passband
    attenuation         width D      ripple
       30 dB    2.210     1.536    +/- 0.270 dB
       40 dB    3.384     2.228    +/- 0.0864 dB
       50 dB    4.538     2.926    +/- 0.0274 dB
       60 dB    5.658     3.621    +/- 0.00868 dB
       70 dB    6.764     4.317    +/- 0.00275 dB
       80 dB    7.865     5.015    +/- 0.00089 dB  (default)
       90 dB    8.960     5.712    +/- 0.00027 dB
      100 dB   10.056     6.408    +/- 0.00009 dB
  The filter transition width parameter is D = (N-1) dF, where N is the number
  of filter coefficients and dF is the normalized transition width of the
  filter.

  Consider interpolating from 8 kHz to 44.1 kHz.  The interpolation ratio is
  441/80.  For this example we will design the filter for an oversampling ratio
  of 10.  This means that the filter will will be operating at a sampling rate
  of 80 kHz.  (The default filter for this program would have used an
  oversampling ratio of 24.)  The cutoff of the filter will be 4 kHz.  The
  stopband attenuation is to be 80 dB.  The attenuation requirement gives
  alpha=7.865.  The parameter D corresponding to this value of alpha is 5.015.
  A transition width which is 15% of the cutoff corresponds to a width of 600
  Hz.  The normalized filter transition width of dF = 600/80000 = 0.0075.
  Solving for D = (N-1) dF for the number of coefficients N, gives N = 670.
  It is common to choose N to be of the form 2*Ir*M+1, where Ir is the
  filter interpolation factor (here 10).  Such a time response has M sidelobes
  on either side of the reference point.  In this example, we can choose
  M = 34, giving N = 681 coefficients.

  If we designate the interpolation factor for the interpolation filter as Ir,
  about 1/Ir of the coefficients are used to calculate each output sample.  The
  number of coefficients needed for a given value of alpha and given transition
  width is proportional to Ir.  Increasing Ir improves the accuracy of
  the linear interpolation step and increases the total number of filter
  coefficients, but does not increase the computation effort time for the
  filtering operation.

  For the transition width expressed as a percentage of the cutoff frequency,
  the number of coefficients needed to calculate each output sample is
  approximately 2D/P where P is the fractional bandwidth (e.g. 0.15 for a
  15% transition width).  The number of coefficients (rounded up) used to
  calculate each interpolated point is shown in the following table.
     stopband   alpha  transition  no. coeffs per output
    attenuation         width D    15% trans. 25% trans.
       30 dB    2.210     1.536       22         14
       40 dB    3.384     2.228       31         19
       50 dB    4.538     2.926       41         25
       60 dB    5.658     3.621       50         30
       70 dB    6.764     4.317       59         36
       80 dB    7.865     5.015       68         42 (default)
       90 dB    8.960     5.712       78         47
      100 dB   10.056     6.408       87         53

  On a Windows PC (1 GHz processor), with the default filter (15% transition
  width), this program generates about 900,000 output samples per second for
  general interpolation and about twice that number for ordinary interpolation.

  The accuracy of the sample rate operation depends on the frequency content
  of the input signal.  Consider increasing the sampling rate for a speech file
  with a 8000 Hz sampling rate.  The spectrum of the signal repeats every
  8000 Hz.  The default filter uses a cutoff frequency of 4000 Hz with a
  transition width of 600 Hz.  The filter passband extends to 3700 Hz and the
  stopband starts at 4300 Hz.  The interpolation will be imperfect in that
  (1) frequencies falling in the lower part of the transition band will be
  attenuated and (2) frequencies falling in the upper part of the transition
  band (the image frequencies due to the repetition of the frequency response)
  will be only be partially attenuated.  If the input signal has little energy
  above 3700 Hz, then the error due to both effects will be small.  Tests
  with speech files indicate that the signal-to-distortion ratios after
  interpolation (say from 8000 Hz to 8001 Hz) range from 46 to 77 dB.  The
  poorest SDR occurs for signals that have significant energy above 3700 Hz.
  For a fixed stopband attenuation, the SDR can be improved by increasing the
  number of filter coefficients to affect a decrease in the transition band
  width.  However, the number of coefficients should not be too large, since
  filters with a large time span can introduce pre-echo effects.

  The interpolation filter can also be read in as a filter file.  For such a
  filter, the filter interpolation factor must be specified.

  The output sample positions are determined by the output sampling rate and
  a sample offset parameter.  The sample offset determines the position of the
  first output sample relative to the input samples.  The default is that the
  first output sample coincides with the first input sample.  The number of
  samples in the output file can also be specified.  The default is to make the
  time  corresponding to the end of the output (rounded to the nearest sample)
  be the same as the time corresponding to the end of the input.

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
  -s SFREQ, --srate=SFREQ
      Sampling frequency for the output file.
  -i SRATIO, --interpolate=SRATIO
      Ratio of the output sampling rate to the input sampling rate.  This
      argument is specified as a single number or as a ratio of the form N/D,
      where each of N and D can be floating point values.  This option is an
      alternate means to specify the output sampling rate.
  -a OFFS, --alignment=OFFS
      Time offset of the first output sample relative to the input data.  The
      units are samples of the input data.  This value can be specified as a
      single number or a ratio.
  -f FPARMS, --filter_spec=FPARMS
      Filter parameters.  The filter parameters are given as keyword values.
      There are two cases: the filter coefficients are supplied in a file or
      the filter is calculated as a Kaiser windowed lowpass filter.
        Filter file:
          file="file_name"  Input filter file name.  If specified, the filter
                            coefficients are read from the named file.
          ratio=Ir          Filter interpolation factor
          delay=Del         Filter delay in units of filter samples (default
                            for symmetrical filters is (N-1)/2, where N is the
                            number of coefficients).  The delay can be
                            specified as a single number or as a ratio.  The
                            filter delay must be supplied for non-symmetrical
                            filters.
        Windowed lowpass:
         ratio=Ir           Filter interpolation factor.  The default depends
                            on the ratio of output sampling frequency to
                            input sampling frequency.  This parameter can be
                            specified as a single number or as a ratio.
         cutoff=Fc          Filter cutoff in normalized frequency relative to
                            the filter interpolation factor (0 to Ir/2).  This
                            value can be specified as a single number or as a
                            ratio.  The default cutoff frequency is determined
                            from the input and output sampling rates.  For an
                            increase in sampling rate, it is set to 0.5.  For
			    a decrease in sampling rate it is set to
			    0.5*fso/fsi.
         atten=A            Filter stopband attenuation in dB.  The attenuation
                            must be at least 21 dB.  The default is 80.  The
                            attenuation is an alternate way to specify the
                            Kaiser window parameter alpha.
         alpha=a            Kaiser window parameter.  Zero corresponds to a
                            rectangular window (stopband attenuation 21 dB).
                            The default is 7.865 corresponding to a stopband
                            attenuation of 80 dB.
         N=Ncof             Number of filter coefficients.  The default is
                            to choose the number of coefficients to give a
                            transition band which is 15% of the cutoff
                            frequency.
         span=Wspan         Window span.  This is the span of the non-zero part
                            of the window.  The default window span is equal
                            to the number of filter coefficients minus one.
         offset=Woffs       Window offset in units of filter samples.  This
                            is the offset of the first filter sample from
                            the beginning of the window.  The default is a
                            fractional value determined from the fractional
                            part of the input sample offset value.
         gain=g             Passband gain.  The default gain is equal to
                            the filter interpolation factor.  This choice
                            reproduces signals within the passband with the
                            correct amplitude.
         write="file_name"  Output filter file name.  If specified, the filter
                            coefficients are written to the named file.
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
      A gain factor applied to the data from the input files.  This gain
      applies to all channels in a file.  The gain value can be given as a
      real number (e.g., "0.003") or as a ratio (e.g., "1/256"). This option
      may be given more than once.  Each invocation applies to the input files
      that follow the option.
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
       program: ResampAudio             program name
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

Examples:
   1: File copy.  Copy audio file abc.au to new.au.
        ResampAudio -i 1 abc.au new.au
   2: Delay the input signal.  The output samples are delayed by 1/8 sample
      from the input samples.
        ResampAudio -i 1 -a -1/8 abc.au new.au
   3: Change the sampling rate to 8001 Hz.
        ResampAudio -s 8001 abc.au new.au
   4: Change the sampling rate by an integral value (e.g. 8000 to 48000 Hz).
        ResampAudio -i 6 abc.au new.au

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
  P. Kabal / v5r1  2005-01-31  Copyright (C) 2006
-------------------------------------------------------------------------*/

#include <stdlib.h>	/* EXIT_SUCCESS */
#include <string.h>

#include <libtsp.h>
#include <libtsp/AFheader.h>
#include <libtsp/AFpar.h>
#include <libtsp/FIpar.h>
#include "ResampAudio.h"
#include <AO.h>


int
main (int argc, const char *argv[])

{
  struct RS_FIpar FI;
  struct RS_FOpar FO;
  int Ftype, Dformat;
  long int Nsamp, Nchan, NframeI;
  AFILE *AFpI, *AFpO;
  FILE *fpinfo;
  double SfreqI, Soffs, Sratio, FDel, toffs;
  struct Fspec_T Fspec;
  struct Fpoly_T PF;

/* Get the input parameters */
  RSoptions (argc, argv, &FI, &Soffs, &Sratio, &Fspec, &FO);

/* If output is to stdout, use stderr for informational messages */
  if (strcmp (FO.Fname, "-") == 0)
    fpinfo = stderr;
  else
    fpinfo = stdout;

/* Open the input audio file */
  AOsetFIopt (&FI, 0, 0);
  FLpathList (FI.Fname, AFPATH_ENV, FI.Fname);
  AFpI = AFopnRead (FI.Fname, &Nsamp, &Nchan, &SfreqI, fpinfo);
  AFpI->ScaleF *= FI.Gain;	/* Gain absorbed into scaling factor */

/* Get the interpolating filter coefficients */
  if (Sratio > 0.0)
    FO.Sfreq = Sratio * SfreqI;
  else
    Sratio = FO.Sfreq / SfreqI;
  RSintFilt (Sratio, Soffs, &Fspec, &PF, &FDel, fpinfo);
  fprintf (fpinfo, "\n");

/* Clean up allocated storage */
  UTfree (Fspec.FFile);
  UTfree (Fspec.WFile);

/* Open the output audio file */
  if (FO.SpkrConfig == NULL)
    FO.SpkrConfig = AFpI->SpkrConfig;
  Ftype = AOsetFtype (&FO);
  Dformat = AOsetDformat (&FO, &AFpI, 1);
  AOsetFOopt (&FO);
  if (strcmp (FO.Fname, "-") != 0)
    FLbackup (FO.Fname);
  AFpO = AFopnWrite (FO.Fname, Ftype, Dformat, Nchan, FO.Sfreq, fpinfo);

/* Default number of output samples, filter time offset */
  NframeI = AOnFrame (&AFpI, &FI, 1, AF_NFRAME_UNDEF);
  if (FO.Nframe == AF_NFRAME_UNDEF) /* Rounding */
    FO.Nframe = (long int) (((NframeI - 1L) - Soffs) * Sratio + 1.5);
  toffs = Soffs + FDel;			/* Time alignment, h[0] <-> toffs */

/* Sample interpolation */
  RSresamp (AFpI, AFpO, Sratio, FO.Nframe, toffs, &PF, fpinfo);

/* Close the audio files */
  AFclose (AFpI);
  AFclose (AFpO);

/* Clean up allocated storage */
  UTfree (PF.offs);
  MAdFreeMat (PF.hs);

  return EXIT_SUCCESS;
}
