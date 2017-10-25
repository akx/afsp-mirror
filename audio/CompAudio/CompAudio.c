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
  complex values.  For more than two channels, the statistics of the individual
  channels are computed. For a very large number of channels, the audio files
  are treated as if they were single channel files.

  For each file, the following statistical quantities are calculated and
  printed for each channel. However if the number of channels is large, these
  values are calculated across all channels.
  Mean:
       Xm = SUM x(i) / N
  Standard deviation:
       sd = sqrt [ (SUM x(i)^2 - Xm^2) / (N-1) ]
  Max value:
       Xmax = max (x(i))
  Min value:
       Xmin = min (x(i))
  The above values are reported as a percent of full scale.  For instance for
  16-bit integer data, full scale is 32768.
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
      factor in percent is also reported. The speech activity measurements are
      calculated only for stereo or mono files with sampling rates appropriat
      for recording speech.

  An optional delay range can be specified when comparing files.  The samples
  in file B are delayed relative to those in file A by each of the delay values
  in the delay range.  For each delay, the SNR with optimized gain factor (see
  below) is calculated.  For the delay corresponding to the largest SNR (with
  optimized gain), the full regalia of file comparison values is reported.

  For multi-channel files, the comparisons are carried out over all samples in
  all channels. For stereo files, the two channels can be considered to
  represent the real and imaginary parts of a complex signal.

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
      length is outside the range of 64 to 1024 samples, the segment is set to
      the appropriate edge value.  For each segment, the SNR is calculated as
                          SUM xa(i)^2
        SS(k) = 1 + ------------------------- .
                    eps + SUM [xa(i)-xb(i)]^2
      The term eps in the denominator prevents division by zero.  The additive
      unity term discounts segments with SNR's less than unity.  The final
      average segmental SNR in dB is calculated as
        SSNR = 10 * log10 ( 10^[SUM log10 (SS(k)) / N] - 1 ) dB.
      The first term in the bracket is the geometric mean of SS(k).  The
      subtraction of the unity term tends to compensate for the unity term
      in SS(k).

  If any of these SNR values is infinite, only the optimal gain factor is
  printed as part of the message (Sf is the optimized gain factor),
    "File A = Sf * File B".

  An example of the output for a stereo file over a range of delays is shown
  below.

    Delay:  20,  SNR = 5.2461    dB  (Gain for File B = 0.55831)
    Delay:  21,  SNR = 65.394    dB  (Gain for File B = 0.66674)
    Delay:  22,  SNR = 5.2462    dB  (Gain for File B = 0.55831)

    File A:
     Channel 1:
       Number of Samples: 23829
       Std Dev = 855.02 (2.609%),  Mean = -9.9431 (-0.03034%)
       Maximum = 4774 (14.57%),  Minimum = -6156 (-18.79%)
       Active Level: 946.7 (2.889%), Activity Factor: 81.6%
     Channel 2:
       Number of Samples: 23829
       Std Dev = 427.5 (1.305%),  Mean = -4.9737 (-0.01518%)
       Maximum = 2387 (7.285%),  Minimum = -3078 (-9.393%)
       Active Level: 473.34 (1.445%), Activity Factor: 81.6%
    File B:
     Channel 1:
       Number of Samples: 23829
       Std Dev = 1282.4 (3.914%),  Mean = -14.886 (-0.04543%)
       Maximum = 7160 (21.85%),  Minimum = -9233 (-28.18%)
       Active Level: 1418.6 (4.329%), Activity Factor: 81.7%
     Channel 2:
       Number of Samples: 23829
       Std Dev = 641.2 (1.957%),  Mean = -7.4417 (-0.02271%)
       Maximum = 3580 (10.93%),  Minimum = -4617 (-14.09%)
       Active Level: 709.29 (2.165%), Activity Factor: 81.7%
    Best match at delay = 21
    SNR      = 6.0233   dB
    SNR      = 65.394   dB  (Gain for File B = 0.66674)
    Seg. SNR = 6.0203   dB  (256 sample segments)
    Max Diff = 3077 (9.39%),  No. Diff = 46127 (573 runs)

Options:
  Input file(s), AFileA [AFileB]:
      The environment variable AUDIOPATH specifies a list of directories to be
      searched for the input audio file(s).  Specifying "-" as the input file
      indicates that input is from standard input.  One or two input files can
      be specified.
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
  -h, --help
      Print a list of options and exit.
  -v, --version
      Print the version number and exit.

  See routine CopyAudio for a description of other parameters.
  -t FTYPE, --type=FTYPE
     Input file type and environment variable AF_FILETYPE
  -P PARMS, --parameters=PARMS
     Input file parameters and environment variable AF_INPUTPAR

Environment variables:
  AUDIOPATH:
    This environment variable specifies a list of directories to be searched
    when opening the input audio files.  Directories in the list are separated
    by colons (semicolons for Windows).

Author / version:
  P. Kabal / v10r1  2017-10-18  Copyright (C) 2017

----------------------------------------------------------------------*/

#include <stdlib.h> /* EXIT_SUCCESS */
#include <string.h>

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
    NsampND = 1;  /* Number of samples can be unknown */
  else
    RAccess = 1;  /* Must be random access (need to rewind) */

/* Open the input files */
  AFp[1] = NULL;
  for (i = 0; i < Nfiles; ++i) {
    AOsetFIopt (&FI[i], NsampND, RAccess);
    FLpathList (FI[i].Fname, AFPATH_ENV, FI[i].Fname);
    AFp[i] = AFopnRead (FI[i].Fname, &Nsamp[i], &Nchan[i], &Sfreq[i], stdout);
    ScaleF[i] = AFp[i]->ScaleF;   /* Native data scaling */
    AFp[i]->ScaleF *= FI[i].Gain; /* Gain absorbed into scaling factor */
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
    - Nch is either AFp[i]->Nchan or 1 (for a large number of channels)
    - Ns is the number of samples to be processed (starting from sample
      Start[i])
    - ScaleF[i] is the scale factor before introducing the user supplied gain;
      AFp[i]->ScaleF includes the user supplied gain
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
   - Reduce Nchan, if Nchan > CA_MaxNcchan
*/


static long int
CA_CheckNchan (long int NchanA, long int NchanB)

{
  long int Nchan;

  /* Set the number of channels to one if Nchan > 2 */
  if (NchanA != NchanB)
    UThalt ("%s: %s", PROGRAM, CAM_DiffNChan);
  if (NchanA > CA_MaxNchan) {
    UTwarn ("%s: %s", PROGRAM, CAM_XNChan);
    Nchan = 1;
  }
  else
    Nchan = NchanA;

  return Nchan;
}
