/*------------- Telecommunications & Signal Processing Lab -------------
                           McGill University

Routine:
  CompAudio [options] AFileA [AFileB]

Purpose:
  Compare audio files, printing statistics

Description:
  This program gathers and prints statistics for one or two input audio files.
  With one input file, this program prints the statistics for that file. With
  two input files, the signal-to-noise ratio (SNR) of the second file relative
  to the first file is also printed. For this calculation, the first audio
  file is used as the reference signal. The "noise" is the difference between
  sample values in the files.

  For SNR comparisons between multi-channel audio files, the data are treated as
  as a single channel. For stereo files, the SNR calculated is the same as if
  the two channels contain the real and imaginary components of complex samples.

  For each file, the following statistical quantities are calculated and printed
  for each channel.
  Mean:
       Xm = SUM x(i) / N
  Standard deviation:
       sd = sqrt[ (SUM x(i)^2 - Xm^2) / (N-1) ]
  Max value:
       Xmax = max(x(i))
  Min value:
       Xmin = min(x(i))
  The above values are reported as a percent of full scale. For instance for
  a file with 16-bit integer data, full scale is 32768.
  Number of Overloads:
      Count of values at or exceeding the full scale value. For integer data
      from a saturating A/D converter, the presence of such values is an
      indication of a clipped signal.
  Number of Anomalous Transitions:
      An anomalous transition is defined as a transition from a normalized
      sample value greater than +0.5 directly to a sample value less than -0.5
      or vice versa. A large number of such transitions is an indication of
      corrupted data.
  Active Level:
      This measurement calculates the active speech level using Method B of
      ITU-T Recommendation P.56. The smoothed envelope of the signal is used
      to segment the signal into active and non-active regions. The active
      level is the average envelope value for the active region. The activity
      factor in percent is also reported. The speech activity measurements are
      calculated only for mono or stereo files with sampling rates appropriate
      for recording speech.

  An optional delay range can be specified when comparing files. The samples
  in file B are delayed or advanced relative to those in file A by each of the
  delay values in the delay range. For each delay, the SNR with optimized gain
  factor (see below) is calculated. For the delay corresponding to the largest
  SNR (with optimized gain), the full regalia of file comparison values is
  reported.

  In calculating the SNR for stereo files, the channels are considered to
  represent the real and imaginary parts of a complex signal. For multi-channel
  files, the comparisons are carried out on samples in each channel, unless the
  number of channels is large. In that case, the samples in the files are
  considered to be a single channel.

  Conventional SNR:
                  SUM |xa(i)|^2
        SNR = --------------------- .
              SUM |xa(i) - xb(i)|^2
      The corresponding value in dB is printed.

  SNR with the optimized gain factor:
        SNR = 1 / (1 - r^2) ,
      where r is the (normalized) correlation coefficient,
                      SUM xa(i)*xb'(i)
        r = ------------------------------------- .
            sqrt[ SUM |xa(i)|^2 * SUM |xb(i)|^2 ]
      The SNR value in dB is printed. This SNR calculation corresponds to using
      an optimized gain factor Sf for file B,
             SUM xa(i)*xb'(i)
        Sf = ---------------- .
              SUM |xb(i)|^2

  Segmental SNR:
      This is the average of SNR values calculated for segments of data. The
      default segment length corresponds to 16 ms (128 samples at a sampling
      rate of 8000 Hz). However if the sampling rate is such that the segment
      length is outside the range of 64 to 768 samples, the segment is set to
      the appropriate edge value. For each segment, the SNR is calculated as
                          SUM xa(i)^2
        SS(k) = 1 + ------------------------- .
                    eps + SUM [xa(i)-xb(i)]^2
      The term eps in the denominator prevents division by zero. The additive
      unity term discounts segments with SNR's less than unity. The final
      average segmental SNR in dB is calculated as
        SSNR = 10 * log10( 10^[SUM log10(SS(k)) / N] - 1 ) dB.
      The first term in the bracket is the geometric mean of SS(k). The
      subtraction of the unity term tends to compensate for the unity term in
      SS(k).

  If any of these SNR values is infinite, only the optimal gain factor is
  printed as part of the message (Sf is the optimized gain factor),
    "File A = Sf * File B".

  The comparison between the files also include maximum absolute difference
  between samples of the files and the number of samples that differ as a
  percentage of the number of samples in all channels.

  An example of the output for stereo files with integer 16-bit data over a
  range of delays is shown below.

    Delay:  20,  SNR = 5.25    dB  (File B Gain = 0.558)
    Delay:  21,  SNR = 65.4    dB  (File B Gain = 0.667)
    Delay:  22,  SNR = 5.25    dB  (File B Gain = 0.558)

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
    SNR      = 6.023   dB
    SNR      = 65.39   dB  (File B Gain = 0.667)
    Seg. SNR = 6.02    dB  (256 sample segments)
    Max Diff = 3077 (9.39%),  No. Diff = 46127 (573 runs)

Options:
  Input file(s), AFileA [AFileB]:
      The environment variable AUDIOPATH specifies a list of directories to be
      searched for the input audio file(s). One or two input files can be
      specified. For a single input file, specifying "-" as the input file
      indicates that input is from standard input (use the "-t" option to
      specify the format of the input data).
  -d DL:DU, --delay=DL:DU
      Specify a delay range (in samples). Each delay in the delay range
      represents a delay of file B relative to file A. A single delay value can
      also be specified. The default range is 0:0.
  -g GAIN, --gain=GAIN
      A gain factor applied to the data from the input files. This gain applies
      to all channels in a file. The gain value can be given as a real number
      (e.g., "0.003") or as a ratio (e.g., "1/256"). This option may be given
      more than once. Each invocation applies to the input files that follow
      the option.
  -l L:U, --limits=L:U
      Sample limits for the input files (numbered from zero). Each invocation
      applies to the input files that follow the option. The specification "L:"
      means from sample L to the end; "N" means from sample 0 to sample N-1.
  -h, --help
      Print a list of options and exit.
  -v, --version
      Print the version number and exit.

  See routine CopyAudio for a description of other options.
  -t FTYPE, --type=FTYPE
     Input file type and environment variable AF_FILETYPE
  -P PARMS, --parameters=PARMS
     Input file parameters and environment variable AF_INPUTPAR

Environment variables:
  AUDIOPATH:
    This environment variable specifies a list of directories to be searched
    when opening the input audio files. Directories in the list are separated by
    colons (semicolons for Windows).

Author / version:
  P. Kabal / v10r3  2020-11-30  Copyright (C) 2020

----------------------------------------------------------------------*/

#include <stdlib.h> /* EXIT_SUCCESS */
#include <string.h>

#include "CompAudio.h"

static long int
CA_CheckNchan(long int NchanA, long int NchanB);


int
main(int argc, const char *argv[])

{
  struct AO_FIpar FI[2];
  AFILE *AFp[2];
  long int Nsamp[2], Nchan[2], Start[2], Delay[2];
  long int Nframe, Ns, Nch;
  int i, NsampND, RAccess, Nfiles;
  double Sfreq[2], ScaleF[2];
  struct Stats_F *StatsFP[2];
  struct Stats_X StatsX;

/*  Get the input parameters */
  CAoptions(argc, argv, Delay, FI);
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
  for (i = 0; i < Nfiles; ++i) {
    AOsetFIopt(&FI[i], NsampND, RAccess);
    FLpathList(FI[i].Fname, AFPATH_ENV, FI[i].Fname);
    AFp[i] = AFopnRead(FI[i].Fname, &Nsamp[i], &Nchan[i], &Sfreq[i], stdout);
    ScaleF[i] = AFp[i]->ScaleF;   /* Native data scaling */
    AFp[i]->ScaleF *= FI[i].Gain; /* Gain absorbed into scaling factor */
  }

  /* Resolve the file limits */
  Nframe = AOnFrame(AFp, FI, Nfiles, AF_NFRAME_UNDEF);
  if (Nframe == AF_NFRAME_UNDEF)
    Ns = AF_NSAMP_UNDEF;
  else
    Ns = Nchan[0] * Nframe;   /* Total number of samples (all channels) */

  Start[0] = Nchan[0] * FI[0].Lim[0];
  if (Nfiles == 2)
    Start[1] = Nchan[1] * FI[1].Lim[0];

  /* Check and warn about Nchan */
  Nch = CA_CheckNchan(AFp[0]->Nchan, AFp[Nfiles-1]->Nchan);
  /*
    - Nch is either AFp[i]->Nchan or 1 (for a large number of channels)
    - Ns is the number of samples to be processed (all channels), starting from
      sample Start[i]
    - ScaleF[i] is the scale factor before introducing the user supplied gain;
      AFp[i]->ScaleF includes the user supplied gain.
  */

  if (Ns == 0L) {
    for (i = 0; i < Nfiles; ++i)
      AFclose(AFp[i]);
    exit(EXIT_SUCCESS);
  }

  /* Generate the file statistics, per channel for each file */
  for (i = 0; i < Nfiles; ++i) {
    StatsFP[i] = (struct Stats_F *) UTmalloc(Nch * sizeof(struct Stats_F));
    CAstats(AFp[i], Start[i], Ns, Nch, StatsFP[i]);
  }

  /* Find the cross file statistics over the delay range
     If a delay range is specified, print SNR / gain for each delay
     Return statistics for the delay with the maximum gain adjusted SNR
  */
  if (Nfiles == 2)
    StatsX = CAcomp(AFp, Start, Ns, Delay);

  /* ------ print statistics and file comparisons */
  /* Print the file statistics for each file */
  printf("\n");
  if (Nfiles == 1)
    CAprstat(StatsFP[0], Nch, ScaleF[0]);

  else if (StatsX.Ndiff == 0 && ScaleF[0] == ScaleF[1])
    /* Data identical in two files, print the statistics for only one file */
    CAprstat(StatsFP[0], Nch, ScaleF[0]);

  else {
    /* Data identical with different scale factors, or data differ */
    printf(CAMF_FileA);
    CAprstat(StatsFP[0], Nch, ScaleF[0]);
    printf(CAMF_FileB);
    CAprstat(StatsFP[1], Nch, ScaleF[1]);
  }

  /* File comparisons */
  if (Nfiles == 2)
    CAprcorr(&StatsX, Delay, ScaleF);

  /* Clean up */
  for (i = 0; i < Nfiles; ++i) {
    AFclose(AFp[i]);
    UTfree(StatsFP[i]);
  }

  return EXIT_SUCCESS;
}

/* Resolve the number of channels
   - Reduce Nchan if Nchan > CA_MaxNchan
*/


static long int
CA_CheckNchan(long int NchanA, long int NchanB)

{
  long int Nch;

  /* Set the number of channels to one if Nchan > MaxNchan */
  if (NchanA != NchanB)
    UThalt("%s: %s", PROGRAM, CAM_DiffNChan);
  if (NchanA > CA_MaxNchan) {
    UTwarn("%s: %s", PROGRAM, CAM_XNChan);
    Nch = 1;
  }
  else
    Nch = NchanA;

  return Nch;
}
