/*------------- Telecommunications & Signal Processing Lab -------------
                           McGill University

Routine:
  CompAudio [options] AFileA [AFileB]

Purpose:
  Compare audio files, printing statistics

Description:
  This program gathers and prints statistics for one or two input audio files.
  With one input file, this program prints the statistics for that file.  With
  two input files, the signal-to-noise ratio (SNR) of the second file relative
  to the first file is also printed.  For this calculation, the first audio
  file is used as the reference signal.  The "noise" is the difference between
  sample values in the files.  

  For SNR comparisons between two-channel audio files, the data is treated as
  complex values.  For more than two channels, the audio files are treated as
  if they were single channel files.

  For each file, the following statistical quantities are calculated and
  printed for each channel.
  Mean:
       Xm = SUM x(i) / N
  Standard deviation:
       sd = sqrt [ (SUM x(i)^2 - Xm^2) / (N-1) ]
  Max value:
       Xmax = max (x(i))
  Min value:
       Xmin = min (x(i))

  These values are reported as a percent of full scale.  For instance for
  16-bit data, full scale is 32768.
  Number of Overloads:
      Count of values at or exceeding the full scale value.  For integer data
      from a saturating A/D converter, the presence of such values is an
      indication of a clipped signal.
  Number of Anomalous Transitions:
      An anomalous transition is defined as a transition from a sample value
      greater than +0.5 full scale directly to a sample value less than -0.5
      full scale or vice versa.  A large number of such transitions is an
      indication of byte-swapped data.
  Active Level:
      This measurement calculates the active speech level using Method B of
      ITU-T Recommendation P.56.  The smoothed envelope of the signal is used
      to divide the signal into active and non-active regions.  The active
      level is the average envelope value for the active region.  The activity
      factor in percent is also reported.  

  An optional delay range can be specified when comparing files.  The samples
  in file B are delayed relative to those in file A by each of the delay values
  in the delay range.  For each delay, the SNR with optimized gain factor (see
  below) is calculated.  For the delay corresponding to the largest SNR, the
  full regalia of file comparison values is reported.

  Conventional SNR:
                  SUM |xa(i)|^2
        SNR = --------------------- .
              SUM |xa(i) - xb(i)|^2
      The corresponding value in dB is printed.

  SNR with optimized gain factor:
        SNR = 1 / (1 - r^2) ,
      where r is the (normalized) correlation coefficient,
                       SUM xa(i)*xb'(i)
        r = -------------------------------------- .
            sqrt [ SUM |xa(i)|^2 * SUM |xb(i)|^2 ]
      The SNR value in dB is printed.  This SNR calculation corresponds to
      using an optimized gain factor Sf for file B,
             SUM xa(i)*xb'(i)
        Sf = ---------------- .
              SUM |xb(i)|^2

  Segmental SNR:
      This is the average of SNR values calculated for segments of data.  The
      default segment length corresponds to 16 ms (128 samples at a sampling
      rate of 8000 Hz).  However if the sampling rate is such that the segment
      length is less than 64 samples or more than 1024 samples, the segment
      length is set to 256 samples.  For each segment, the SNR is calculated
      as
                          SUM xa(i)^2
        SS(k) = 1 + ------------------------- .
                    eps + SUM [xa(i)-xb(i)]^2
      The term eps in the denominator prevents divides by zero.  The additive
      unity term discounts segments with SNR's less than unity.  The final
      average segmental SNR in dB is calculated as
        SSNR = 10 * log10 ( 10^[SUM log10 (SS(k)) / N] - 1 ) dB.
      The first term in the bracket is the geometric mean of SS(k).  The
      subtraction of the unity term tends to compensate for the unity term
      in SS(k). 

  If any of these SNR values is infinite, only the optimal gain factor is
  printed as part of the message (Sf is the optimized gain factor),
    "File A = Sf * File B".

Options:
  Input file(s): AFileA [AFileB]:
      The environment variable AUDIOPATH specifies a list of directories to be
      searched for the input audio file(s).  Specifying "-" as the input file
      indicates that input is from standard input.
  -d DL:DU, --delay=DL:DU
      Specify a delay range (in samples).  Each delay in the delay range
      represents a delay of file B relative to file A.  The default range is
      0:0.
  -s SAMP, --segment=SAMP
      Segment length (in samples) to be used for calculating the segmental
      signal-to-noise ratio.  The default is a length corresponding to 16 ms.
  -g GAIN, --gain=GAIN
      A gain factor applied to the data from the input files.  This gain
      applies to all channels in a file.  The gain value can be given as a
      real number (e.g., "0.003") or as a ratio (e.g., "1/256"). This option
      may be given more than once.  Each invocation applies to the input files
      that follow the option.
  -l L:U, --limits=L:U
      Sample limits for the input files (numbered from zero).  Each invocation
      applies to the input files that follow the option.  The specification
      ":" means the entire file; "L:" means from sample L to the end; ":U"
      means from sample 0 to sample U; "N" means from sample 0 to sample N-1.
  -t FTYPE, --type=FTYPE
      Input audio file type.  In the default automatic mode, the input file
      type is determined from the file header.  For input from a non-random
      access file (e.g. a pipe), the input file type can be explicitly
      specified to avoid the lookahead used to read the file header.  This
      option can be specified more than once.  Each invocation applies to the
      input files that follow the option.  See the description of the
      environment variable AF_FILETYPE below for a list of file types.
  -P PARMS, --parameters=PARMS
      Parameters to be used for headerless input files.  This option may be
      given more than once.  Each invocation applies to the input files that
      follow the option.  See the description of the environment variable
      AF_NOHEADER below for the format of the parameter specification.
  -h, --help
      Print a list of options and exit.
  -v, --version
      Print the version number and exit.

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
  P. Kabal / v5r1  2003-07-11  Copyright (C) 2003

----------------------------------------------------------------------*/

#include <stdlib.h>	/* EXIT_SUCCESS */
#include <string.h>

#include <libtsp.h>
#include <libtsp/AFpar.h>
#include <AO.h>
#include "CompAudio.h"

static long int
CA_CheckNchan (long int NchanA, long int NchanB);


int
main (int argc, const char *argv[])

{
  struct CA_FIpar FI[2];
  AFILE *AFp[2];
  long int Nsamp[2], Nchan[2], Start[2];
  long int Nframe, Ns, Nsseg, Nch;
  long int delayL, delayU, delayM;
  int NsampND, RAccess, i, Nfiles;
  double Sfreq[2], ScaleF[2];
  struct Stats_F *StatsF;
  struct Stats_T StatsT;

/*  Get the input parameters */
  CAoptions (argc, argv, &delayL, &delayU, &Nsseg, FI);
  if (FI[1].Fname[0] == '\0')
    Nfiles = 1;
  else
    Nfiles = 2;

/* Input file access options */
  NsampND = 0;
  RAccess = 0;
  if (Nfiles == 1)
    NsampND = 1;	/* Number of samples can be unknown */
  else
    RAccess = 1;	/* Must be random access (need to rewind) */

/* Open the input files */
  AFp[1] = NULL;
  for (i = 0; i < Nfiles; ++i) {
    AOsetFIopt (&FI[i], NsampND, RAccess);
    FLpathList (FI[i].Fname, AFPATH_ENV, FI[i].Fname);
    AFp[i] = AFopnRead (FI[i].Fname, &Nsamp[i], &Nchan[i], &Sfreq[i], stdout);
    ScaleF[i] = AFp[i]->ScaleF;		/* Native data scaling */
    AFp[i]->ScaleF *= FI[i].Gain;	/* Gain absorbed into scaling factor */
  }

  /* Resolve the file limits */
  Nframe = AOnFrame (AFp, FI, Nfiles, AF_NFRAME_UNDEF);
  if (Nframe == AF_NFRAME_UNDEF)
    Ns = AF_NSAMP_UNDEF;
  else
    Ns = Nchan[0] * Nframe;

  Start[0] = Nchan[0] * FI[0].Lim[0];
  if (Nfiles == 2)
    Start[1] = Nchan[1] * FI[1].Lim[0];

  /* Check and warn about Nchan */
  if (Nfiles == 1)
    Nch = CA_CheckNchan (AFp[0]->Nchan, AFp[0]->Nchan);
  else
    Nch = CA_CheckNchan (AFp[0]->Nchan, AFp[1]->Nchan);

  /*
    - Nch is the possibly reduced number of channels, while AFp[i]->Nchan is
      the real number of channels
    - Ns is the number of samples to be processed (starting from sample
      Start[i])
    - ScaleF[i] is the scale factor before introducting the user supplied
      gain; AFp[i]->ScaleF includes the user supplied gain
  */

  if (Ns == 0L)
    exit (EXIT_SUCCESS);

/* File statistics */
  StatsF = (struct Stats_F *)
    UTmalloc (Nfiles * Nch * (sizeof (struct Stats_F)));
  for (i = 0; i < Nfiles; ++i)
    CAstats (AFp[i], Start[i], Ns, &StatsF[i*Nch]);

  /* Find the cross file statistics over the delay range */
  if (Nfiles == 2)
    CAcomp (AFp, Start, Ns, Nsseg, delayL, delayU, &delayM, &StatsT);

/* File A statistics */
  printf ("\n");
  if (Nfiles == 2 && (StatsT.Ndiff > 0 || ScaleF[0] != ScaleF[1]))
    printf (CAMF_FileA);
  CAprstat (&StatsF[0*Nch], Nch, ScaleF[0]);

/* File comparisons */
  if (Nfiles == 2) {

    if (StatsT.Ndiff > 0 || ScaleF[0] != ScaleF[1]) {
      printf (CAMF_FileB);
      CAprstat (&StatsF[1*Nch], Nch, ScaleF[1]);
    }

    if (StatsT.Ndiff == 0) {
      /* Identical files */
      printf ("\n");
      if (delayL < delayU || delayL != 0)
	printf (CAMF_AeqBdelay, delayM);
      else
	printf (CAMF_AeqB);
    }
    else {
      /* File B statistics */
      if (delayL < delayU)
	printf (CAMF_BestMatch, delayM);
      else if (delayL != 0)
	printf (CAMF_Delay, delayM);

      CAprcorr (&StatsT, ScaleF);
    }
  }

/* Clean up */
  for (i = 0; i < Nfiles; ++i)
    AFclose (AFp[i]);
  UTfree (StatsF);

  return EXIT_SUCCESS;
}

/* Resolve the number of channels
   - Reduce Nchan, if Nchan > 2
*/


static long int
CA_CheckNchan (long int NchanA, long int NchanB)

{
  long int Nchan;

  /* Set the number of channels to one if Nchan > 2 */
  if (NchanA != NchanB)
    UThalt ("%s: %s", PROGRAM, CAM_DiffNChan);
  if (NchanA > 2) {
    UTwarn ("%s: %s", PROGRAM, CAM_XNChan);
    Nchan = 1;
  }
  else
    Nchan = NchanA;

  return Nchan;
}
