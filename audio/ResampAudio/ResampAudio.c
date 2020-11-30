/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  ResampAudio [options] AFileI AFileO

Purpose:
  Resample data from an audio file

Description:
  This program resamples data from an audio file. This process involves
  interpolating between the samples in the original file to create a new
  sequence of samples with a new spacing (sampling rate). This program allows
  for an arbitrary ratio of output sampling rate to input rate. The default
  settings give a high quality sample rate conversion.

Theory of Operation:
  The process used for interpolation depends on the ratio of the output sampling
  rate to the input sampling rate.
   1: The output/input sampling rate is a ratio of small integers.
      The sample rate change process is done using a conventional interpolation
      filter designed for the interpolation factor (numerator of the sampling
      rate ratio) followed by subsampling by the subsampling factor (denominator
      of the sampling rate ratio).
   2: General case.
      An interpolating filter is designed using an interpolation factor of 24.
      For each output sample, the interpolating filter is used to create two
      samples that bracket the desired sampling point. Linear interpolation is
      used between these values to generate the output value.

  The default interpolation filter is a linear phase FIR filter designed by
  applying a Kaiser window to an ideal lowpass filter response. The filter is
  characterized by a cutoff frequency, a window shape parameter, and the number
  of coefficients. The window shape parameter (alpha) controls the passband
  ripple and the stopband attenuation. For a fixed number of coefficients,
  decreasing ripple and increasing attenuation (larger alpha) come at the
  expense of a wider transition width.

  The cutoff of the default interpolation filter depends on the input and output
  sampling rates. Let fsi be the sampling rate of the input signal and fso be
  the sampling rate of the output signal.
   1: fso > fsi. The cutoff of the interpolation filter is set to fsi/2.
   2: fso < fsi. The cutoff of the interpolation filter is set to fso/2.

  The default design aims for an 80 dB stopband attenuation and a transition
  width which is 15% of the cutoff frequency. The attenuation directly
  determines alpha. The value of alpha together with the transition width
  determines the number of filter coefficients.

  The parameters of the interpolating filter can also be set by the user. The
  design parameters are the interpolation factor, the filter cutoff frequency,
  the Kaiser window parameter, and the number of filter coefficients. The
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

  Consider interpolating from 8 kHz to 44.1 kHz. The interpolation ratio is
  441/80. For this example we will design the filter for an oversampling ratio
  of 10. This means that the filter will will be operating at a sampling rate
  of 80 kHz. (The default filter for this program would have used an
  oversampling ratio of 24.) The cutoff of the filter will be 4 kHz. The
  stopband attenuation is to be 80 dB. The attenuation requirement gives
  alpha=7.865. The parameter D corresponding to this value of alpha is 5.015.
  A transition width which is 15% of the cutoff corresponds to a width of 600
  Hz. The normalized filter transition width of dF = 600/80000 = 0.0075.
  Solving for D = (N-1) dF for the number of coefficients N, gives N = 670. It
  is common to choose N to be of the form 2*Ir*M+1, where Ir is the filter
  interpolation factor (here 10). Such a time response has M sidelobes on either
  side of the reference point. In this example, we can choose M = 34, giving
  N = 681 coefficients.

  If we designate the interpolation factor for the interpolation filter as Ir,
  about 1/Ir of the coefficients are used to calculate each output sample. The
  number of coefficients needed for a given value of alpha and given transition
  width is proportional to Ir. Increasing Ir improves the accuracy of the
  linear interpolation step and increases the total number of filter
  coefficients, but does not increase the computation effort time for the
  filtering operation.

  For the transition width expressed as a percentage of the cutoff frequency,
  the number of coefficients needed to calculate each output sample is
  approximately 2D/P where P is the fractional bandwidth (e.g. 0.15 for a 15%
  transition width). The number of coefficients (rounded up) used to calculate
  each interpolated point is shown in the following table.
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

  The accuracy of the sample rate operation depends on the frequency content of
  the input signal. Consider increasing the sampling rate for a speech file
  with a 8000 Hz sampling rate. The spectrum of the signal repeats every 8000
  Hz. The default filter uses a cutoff frequency of 4000 Hz with a transition
  width of 600 Hz. The filter passband extends to 3700 Hz and the stopband
  starts at 4300 Hz. The interpolation will be imperfect in that (1) frequencies
  falling in the lower part of the transition band will be attenuated and
  (2) frequencies falling in the upper part of the transition band (the image
  frequencies due to the repetition of the frequency response) will be only be
  partially attenuated. If the input signal has little energy above 3700 Hz,
  then the error due to both effects will be small. Tests with speech files
  indicate that the signal-to-distortion ratios after interpolation (say from
  8000 Hz to 8001 Hz) range from 46 to 77 dB. The poorest SDR occurs for signals
  that have significant energy above 3700 Hz. For a fixed stopband attenuation,
  the SDR can be improved by increasing the number of filter coefficients to
  affect a decrease in the transition band width. However, the number of
  coefficients should not be too large, since filters with a large time span can
  introduce pre-echo effects.

  The interpolation filter can also be read in as a filter file. For such a
  filter, the filter interpolation factor must be specified.

  The output sample positions are determined by the output sampling rate and a
  sample offset parameter. The sample offset determines the position of the
  first output sample relative to the input samples. The default is that the
  first output sample coincides with the first input sample. The number of
  samples in the output file can also be specified. The default is to make the
  time corresponding to the end of the output (rounded to the nearest sample) be
  the same as the time corresponding to the end of the input.

Options:
  Input file name, AFileI:
      The environment variable AUDIOPATH specifies a list of directories to be
      searched for the input audio file. Specifying "-" as the input file
      indicates that input is from standard input.
  Output file name, AFileO:
      The second file name is the output file. Specifying "-" as the output file
      name indicates that output is to be written to standard output. If the
      output file type is not explicitly given (-F option), the extension of the
      output file name is used to determine the file type.
        ".au"   - AU audio file
        ".wav"  - WAVE file
        ".aif"  - AIFF sound file
        ".afc"  - AIFF-C sound file
        ".raw"  - Headerless file (native byte order)
        ".txt"  - Text audio file (with header)
  -s SFREQ, --srate=SFREQ
      Sampling frequency for the output file. The sampling rate can be expressed
      as a single value or as a ratio of the form N/D, where each of N and D can
      be a floating point value.
  -i SRATIO, --interpolate=SRATIO
      Ratio of the output sampling rate to the input sampling rate. This
      argument is specified as a single number or as a ratio of the form N/D,
      where each of N and D can be a floating point value. This option is an
      alternate means to specify the output sampling rate.
  -a OFFS, --alignment=OFFS
      Time offset of the first output sample relative to the input data. The
      units are samples of the input data. This value can be specified as a
      single number or a ratio.
  -f FPARMS, --filter_spec=FPARMS
      Filter parameters. The filter parameters are given as keyword values.
      There are two cases: the filter coefficients are supplied in a file or
      the filter is calculated as a Kaiser windowed lowpass filter.
        Filter file:
          file="file_name"  Input filter file name. If specified, the filter
                            coefficients are read from the named file.
          ratio=Ir          Filter interpolation factor
          delay=Del         Filter delay in units of filter samples (default
                            for symmetrical filters is (N-1)/2, where N is the
                            number of coefficients). The delay can be specified
                            as a single number or as a ratio. The filter delay
                            must be supplied for non-symmetrical filters.
        Windowed lowpass:
         ratio=Ir           Filter interpolation factor. The default depends on
                            the ratio of output sampling frequency to input
                            sampling frequency. This parameter can be specified
                            as a single number or as a ratio.
         cutoff=Fc          Filter cutoff in normalized frequency relative to
                            the filter interpolation factor (0 to Ir/2). This
                            value can be specified as a single number or as a
                            ratio. The default cutoff frequency is determined
                            from the input and output sampling rates. For an
                            increase in sampling rate, it is set to 0.5. For
                            a decrease in sampling rate it is set to
                            0.5*fso/fsi.
         atten=A            Filter stopband attenuation in dB. The attenuation
                            must be at least 21 dB. The default is 80. The
                            attenuation is an alternate way to specify the
                            Kaiser window parameter alpha.
         alpha=a            Kaiser window parameter. Zero corresponds to a
                            rectangular window (stopband attenuation 21 dB).
                            The default is 7.865 corresponding to a stopband
                            attenuation of 80 dB.
         N=Ncof             Number of filter coefficients. The default is to
                            choose the number of coefficients to give a
                            transition band which is 15% of the cutoff
                            frequency.
         span=Wspan         Window span. This is the span of the non-zero part
                            of the window. The default window span is equal to
                            the number of filter coefficients minus one.
         offset=Woffs       Window offset in units of filter samples. This is
                            the offset of the first filter sample from the
                            beginning of the window. The default is a fractional
                            value determined from the fractional part of the
                            input sample offset value.
         gain=g             Passband gain. The default gain is equal to the
                            filter interpolation factor. This choice reproduces
                            signals within the passband with the correct
                            amplitude.
         write="file_name"  Output filter file name. If specified, the filter
                            coefficients are written to the named file.
  -n NSAMPLE, --number_samples=NSAMPLE
      Number of samples (per channel) for the output file.
  -g GAIN, --gain=GAIN
      A gain factor applied to the data from the input files. This gain applies
      to all channels in a file. The gain value can be given as a real number
      (e.g., "0.003") or as a ratio (e.g., "1/256").
  -F FTYPE, --file-type=FTYPE
      output file type. If this option is not specified, the file type is
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
  -h, --help
      Print a list of options and exit.
  -v, --version
      Print the version number and exit.

  See routine CopyAudio for a description of other options.
  -t FTYPE, --type=FTYPE
      Input file type and environment variable AF_FILETYPE
  -P PARMS, --parameters=PARMS
      Input file parameters and environment variable AF_INPUTPAR
  -D DFORMAT, --data-format=DFORMAT
      Details on allowed data formats for the output file
  -I INFO, --info-INFO
      Details on usage and default information records
  -S SPEAKERS, --speakers=SPEAKERS
      Loudspeaker configuration

Examples:
   1: File copy. Copy audio file abc.au to new.au.
        ResampAudio -i 1 abc.au new.au
   2: Delay the input signal. The output samples are delayed by 1/8 sample.
        ResampAudio -i 1 -a -1/8 abc.au new.au
   3: Change the sampling rate to 8001 Hz.
        ResampAudio -s 8001 abc.au new.au
   4: Change the sampling rate by an integral value (e.g. 8000 to 48000 Hz).
        ResampAudio -i 6 abc.au new.au

Environment variables:
  AUDIOPATH:
  This environment variable specifies a list of directories to be searched when
  opening the input audio files. Directories in the list are separated by
  colons (semicolons for Windows).

Author / version:
  P. Kabal / v10r3  2020-11-30  Copyright (C) 2020
-------------------------------------------------------------------------*/

#include <stdlib.h> /* EXIT_SUCCESS */
#include <string.h>

#include <libtsp/FIpar.h>
#include "ResampAudio.h"
#include <AO.h>


int
main(int argc, const char *argv[])

{
  struct RS_FIpar FI;
  struct RS_FOpar FO;
  long int Nsamp, Nchan, NframeI;
  AFILE *AFpI, *AFpO;
  FILE *fpinfo;
  double SfreqI, Soffs, Sratio, FDel, toffs;
  struct Fspec_T Fspec;
  struct Fpoly_T PF;

/* Get the input parameters */
  RSoptions(argc, argv, &FI, &Soffs, &Sratio, &Fspec, &FO);

/* If output is to stdout, use stderr for informational messages */
  if (strcmp(FO.Fname, "-") == 0)
    fpinfo = stderr;
  else
    fpinfo = stdout;

/* Open the input audio file */
/* Unless AFopt.NsampND is set, the number of samples will be available */
  AOsetFIopt(&FI, 0, 0);
  FLpathList(FI.Fname, AFPATH_ENV, FI.Fname);
  AFpI = AFopnRead(FI.Fname, &Nsamp, &Nchan, &SfreqI, fpinfo);
  AFpI->ScaleF *= FI.Gain;  /* Gain absorbed into scaling factor */

/* Check the interpolation ratio / sampling frequency */
  if (Sratio > 0.0 && FO.Sfreq > 0.0) {
    if (FO.Sfreq != Sratio * SfreqI)
      UThalt("%s: %s", PROGRAM, RSM_BadSFreqRatio);
  }
  else if (Sratio > 0.0)
    FO.Sfreq = Sratio * SfreqI;
  else if (FO.Sfreq > 0.0)
    Sratio = FO.Sfreq / SfreqI;
  else
    UThalt("%s: %s", PROGRAM, RSM_MRatio);

/* Generate the interpolating filter coefficients */
  RSintFilt(Sratio, Soffs, &Fspec, &PF, &FDel, fpinfo);
  fprintf(fpinfo, "\n");

/* Clean up allocated storage */
  UTfree(Fspec.FFile);
  UTfree(Fspec.WFile);

/* Default number of output samples, filter time offset */
  NframeI = AOnFrame(&AFpI, &FI, 1, AF_NFRAME_UNDEF);
  if (FO.Nframe == AF_NFRAME_UNDEF)
    FO.Nframe = (long int) (((NframeI - 1L) - Soffs) * Sratio + 1.5);
  toffs = Soffs + FDel;     /* Time alignment, h[0] <-> toffs */

/* Set data format information into FO */
  AOsetDFormat(&FO, &AFpI, 1);
/* Set AFopt.Nframe, AFopt.Nbs, AFopt.SpkrConfig used by AFpreSetWPar */
  if (FO.SpkrConfig == NULL)
    FO.SpkrConfig = AFpI->SpkrConfig;
  AOsetFOopt(&FO);

/* Open the output audio file */
  if (strcmp(FO.Fname, "-") != 0)
    FLbackup(FO.Fname);
  AFpO = AFopnWrite(FO.Fname, FO.FtypeW, FO.DFormat.Format, Nchan, FO.Sfreq,
                    fpinfo);

/* Default number of output samples, filter time offset */
  NframeI = AOnFrame(&AFpI, &FI, 1, AF_NFRAME_UNDEF);
  if (FO.Nframe == AF_NFRAME_UNDEF) /* Rounding */
    FO.Nframe = (long int) (((NframeI - 1L) - Soffs) * Sratio + 1.5);
  toffs = Soffs + FDel;     /* Time alignment, h[0] <-> toffs */

/* Sample interpolation */
  RSresamp(AFpI, AFpO, Sratio, FO.Nframe, toffs, &PF, fpinfo);

/* Close the audio files */
  AFclose(AFpI);
  AFclose(AFpO);

/* Clean up allocated storage */
  UTfree(PF.offs);
  MAdFreeMat(PF.hs);

  return EXIT_SUCCESS;
}
