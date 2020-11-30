/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  CopyAudio [options] AFileA AFileB ... AFileO

Purpose:
  Copy audio files (combine/concatenate, scale and shift data)

Description:
  This program copies samples from one or more input audio files to an output
  audio file. The samples in the output file are linear combinations of the
  samples in the different channels in the input files. In the combine mode,
  data from multiple input files is combined. In the concatenate mode, data from
  multiple input files is concatenated.

  The output consists of linear combinations of the input data channels. For
  instance output channel A can be formed as the average of input channel A and
  input channel B. For multichannel data, the data is organized into sample
  frames, with samples from individual channels making up a frame.

  Sample limits may be specified for the input data. The same sample limits
  apply to each channel in a file. Sample limits are offset into the file data
  and are numbered from zero. Negative sample limits may be specified; the
  samples corresponding to negative indices have zero values. Similarly, sample
  limits beyond the end-of-file may be specified; samples beyond the end-of-file
  are assumed to have zero values.

  The combine and concatenate modes differ in how they treat multiple input
  files. For the combine mode, if there is more than one input file, channels
  assignments cross between input files. Consider two input files, with the
  first having 3 channels, and the second having 2 channels. The channels are
  labelled A to E, with channels A, B and C coming from the first input file and
  channels D and E coming from the second input file. For the combine mode, a
  single sample limits specification applies to all input files. The default
  sample limits correspond to the length of the longest input file. If
  necessary, the data from the input files is padded with zeros to a common
  length before combining.

  In the concatenate mode, the data in the input files is concatenated. The
  number of channels in each input file must be the same. For the concatenate
  mode, sample limits can be specified for each input file. The default sample
  limits for an input file correspond to the length of that file.

  For fixed point file data, the data is normalized to the range -1 to +1. This
  normalization becomes important when files with different data formats are
  combined.

  The default data format for the output file is chosen according to a data
  format promotion rule based on the data types of the input files. For a single
  input file, the output data format will be the same as the input data format
  as long as that data format is compatible with the output file type.

Options:
  Input file names, AFileI1 [AFileI2 ...]:
      The environment variable AUDIOPATH specifies a list of directories to be
      searched for the input audio file(s). Specifying "-" as the input file
      name indicates that input is from standard input  use the "-t" option to
      specify the format of the input data).
  Output file name, AFileO:
      The last file name is the output file. Specifying "-" as the output file
      name indicates that output is to be written to standard output. If the
      output file type is not explicitly given (-F option), the extension of
      the output file name is used to determine the file type.
        ".au"   - AU audio file
        ".wav"  - WAVE file
        ".aif"  - AIFF sound file
        ".afc"  - AIFF-C sound file
        ".raw"  - Headerless file (native byte order)
        ".txt"  - Text audio file (with header)
  -c, --combine
      Use the combine mode for multiple input files (default). If no channel
      scaling factors are specified, the number of output channels is the sum
      of the numbers of channels in the input files.
  -C, --concatenate
      Use the concatenate mode for multiple input files. If no channel scaling
      factors are specified, each output channel is equal to the concatenation
      of each channel in each input files.
  -g GAIN, --gain=GAIN
      A gain factor applied to the data from the input files. This gain applies
      to all channels in a file and is applied in addition to the channel
      scaling factors. The gain value can be given as a real number (e.g.,
      "0.003") or as a ratio (e.g., "1/256"). This option may be given more than
      once. Each invocation applies to the input files that follow the option.
  -l L:U, --limits=L:U
      Sample limits for the input files (numbered from zero). For the combine
      mode, the default sample limits correspond to the limits of the longest
      input audio file. For the concatenate mode, this option may be given more
      than once. Each invocation applies to the input files that follow the
      option. The specification "L:" means from sample L to the end; "N" means
      from sample 0 to sample N-1.
  -t FTYPE, --type=FTYPE
      Input audio file type. In the default automatic mode, the input file type
      is determined from the file header. For input from a non-random access
      file (e.g. a pipe), the input file type can be specified explicitly to
      avoid the lookahead needed to read the file header. This option can be
      specified more than once. Each invocation applies to the input files that
      follow the option. See the description of the environment variable
      AF_FILETYPE below for a list of file types.
  -P PARMS, --parameters=PARMS
      Parameters to be used to specify default parameters for INPUT files. This
      option may be given more than once. Each invocation applies to the input
      files that follow the option. See the description of the environment
      variable AF_INPUTPAR below for the format of the parameter specifications.
  -s SFREQ, --srate=SFREQ
      Sampling frequency for the output file, default from the input audio
      file(s). This option only changes the sampling frequency field in the
      output header; the audio data itself is unaffected. The sampling rate can
      be expressed as a single value or as a ratio of the form N/D, where each
      of N and D can be a floating point value.
  -n NSAMPLE, --number-samples=NSAMPLE
      Number of samples per channel for the output file. If not specified, the
      number of samples is equal to the sum of the numbers of samples in the
      input files (concatenate mode) or to the largest number of samples in any
      of the input files (combine mode).
  -cA GAINS, --cA=CGAINS
      The output data for output channel A is created by adding scaled samples
      from the specified input channels. For specifying channel scaling factors,
      input channels are labelled A, B, ... X, Y, Z. The scaling factor string
      CGAINS takes the form
        [+|-] [gain *] chan +|- [gain *] chan ... +|- offset,
      where chan is A through Z. The gains can be expressed as ratios, i.e. of
      the form "n/m" or as a real number. The offset is in normalized units,
      where an offset of one corresponds to full scale. Note that that the
      character "*" is a special character in Unix and should appear only within
      single quotes to prevent interpreting it as a wildcard character.
  -cB CGAINS, --chanB=CGAINS
      Channel scaling  factors for output channel B.
  ...
  -cL CGAINS, --chanL=CGAINS
      Channel scaling  factors for output channel L.
  -F FTYPE, --file-type=FTYPE
      Output file type. If this option is not specified, the file type is
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
      Optionally, the number of "significant" bits may be specified as a
      qualifier to the integer data formats, e.g. "integer16/12". The number of
      significant bits is for informational purposes only. The data formats
      available depend on the output file type.
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
          text16, text
      If the output file data type is not specified, the default data format is
      determined by ranking data types according to precision. The desired
      output precision is initially set that of integer16. The desired precision
      is compared to the precision of the data in the input files and increased
      if necessary. The output data type is determined by a table of available
      precisions for the each file type. The table is addressed by the desired
      precision.
  -S SPEAKERS, --speakers=SPEAKERS
      Mapping of output channels to speaker positions. The spatial positions
      of the loudspeakers are specified as a list of white-space or comma
      separated locations from the list below.
        "FL"    - Front Left            "FR"    - Front Right
        "FC"    - Front Center          "LFE1"  - Low Frequency Effects-1
        "BL"    - Back Left             "BR"    - Back Right
        "FLc"   - Front Left center     "FRc"   - Front Right center
        "BC"    - Back Center           "LFE2"  - Low Frequency Effects-2
        "SiL"   - Side Left             "SiR"   - Side Right
        "TpFL"  - Top Front Left        "TpFR"  - Top Front Right
        "TpFC"  - Top Front Centre      "TpC"   - Top Centre
        "TpBL"  - Top Back Left         "TpBR"  - Top Back Right
        "TpSiL" - Top Side Left         "TpSiR" - Top Side Right
        "TpBC"  - Top Back Centre       "BtFC"  - Bottom Front Centre
        "BtFL"  - Bottom Front Left     "BtFR"  - Bottom Front Right
        "FLw"   - Front Left wide       "FRw"   - Front Right wide
        "LS"    - Left Surround         "RS"    - Right Surround
        "LSd"   - Left Surround direct  "RSd"   - Right Surround direct
        "TpLS"  - Top Left Surround     "TpRS"  - Top Right Surround
        "-"     - No speaker assigned
      A speaker position can be associated with only one channel. For
      compatibility with previous usage, some synonyms are available. For
      instance,
        "LF"   -> "LFE1"
        "SL"   -> "SiL", ...
        "TpFL" -> "TFL", ...
      The following names can be used to set speaker location configurations.
        "Stereo" -> "FL FR"
        "Quad"   -> "FL, FR, BL BR"
        "5.1"    -> FL, FR, FC, LFE1, BR, BL
        "7.1"    -> FL, FC, FR, LFE1, BR, BL, SiL, SiR"
      WAVE extensible files store the speaker locations in the file header if
      certain constraints are in place. The list of available speaker locations
      are as follows.
        FL, FR, FC, LFE1, BL, BR FLc, FRc, BC, SiL, SiR, TpC, TpFL, TpFC, TpFR,
        TpBR, TpBC, TpBR"
      Assume M speaker locations are specified. These must be associated with
      the first M audio channels. The speaker locations must be in the same
      order as the list, but locations can be skipped. For example, the order
      "FL FR TpC" is valid, but "FL FC FR" is not.
  -I INFO, --info=INFO
      Add an information record to the output audio file. An empty INFO string
      turns off the generation standard information records (described below).
      The information record option can be repeated, with each invocation adding
      an information record. The suggested format for information records is an
      identifier followed by text. The text from an information record of the
      form "title: TITLE" will appear as part of the printout of the file
      information as a file is opened.
  -h, --help
      Print a list of options and exit.
  -v, --version
      Print the version number and exit.

  This program allows direct specification of the gains for 12 output channels
  and 26 input channels. The program can handle larger numbers of channels for
  the case that the input channels are in a one-to-one correspondence with
  the output channels. A gain factor applying to all channels can be specified
  with the -g or --gain option.

  By default, the output file contains standard information records. These are
  written to the file header.
    date: 2001-01-25 19:19:39 UTC   date
    program: CopyAudio              program name
    sampling_rate: 8000.5           non-integer sampling rate
      This record  is generated if the sampling frequency is not an integer
      value and the output file format does not support non-integer sampling
      rates in its header.
    loudspeakers: FL FR             loudspeaker locations
      This record is generated if the output file format does not support the
      specification of loudspeaker locations in its header.
    bits/sample: 12/16              bits per sample
      This record is generated if the bits per sample is less than the number of
      bits in the data format and the output file format does not support this
      specification in its header.
  This information can be changed with the header information string which is
  specified as one of the command line options. Structured information records
  should adhere to the above format with a named field terminated by a colon,
  followed by numeric data or text. Comments can follow as unstructured
  information. Within records, lines are delimited by a newline ('\n') control
  characters.

Examples:
   1: File convert.
      Copy audio file abc.au to abc.wav. The output audio file is a WAVE file
      with the same data type (if possible) as the input file. The number of
      channels in the output file is the same as the number of channels in the
      input file.
        CopyAudio abc.au abc.wav
   2: Concatenate audio data.
      Form an audio file which is the concatenation of the data from two input
      files. Both input files must have the same number of channels.
        CopyAudio -C abc1.au abc2.au abc12.au
   3: Difference between values.
      Create a two-channel output audio file from two single-channel input
      files. Each sample in the first output channel is the sum of corresponding
      samples in the input files, and the second output channel is the
      difference between corresponding samples in the input files.
        CopyAudio --combine abc1.au abc2.au -cA A+B -cB A-B sumdiff.au
   4: Scale sample values.
      Scale the samples in the input single-channel file by 0.5.
        CopyAudio --gain=1/2 abc.au scaled.au
      The same result can be obtained by specifying the gain for a (single)
      output channel.
        CopyAudio --chanA="0.5*A" abc.au scaled.au
   5: Byte-swap data values.
      Let the input audio file be headerless and contain 16-bit data. Create a
      headerless audio file with byte-swapped data.
        CopyAudio -P integer16 -F noheader-swap abc.au swap.au
   6: Extract samples.
      Extract samples 1000 to 1999 inclusive from the input audio file. The
      output audio file will have 1000 samples.
        CopyAudio -l 1000:1999 abc.au out.au
   7: Create a stereo file.
      Form a stereo (2-channel) audio file from two single channel audio files.
        CopyAudio abc1.wav abc2.wav -S "FL FR" stereo.wav
   8: Add a dc value to a file.
      If the (single channel) input file has a mean value of 8.63/32768, the
      output file will have a zero mean after adding -8.63/32768 to each sample.
        CopyAudio -cA"-8.63/32768" abc.au zeromean.au

Environment variables:
  AF_FILETYPE:
    This environment variable defines the input audio file type. In the default
    auto mode, the input file type is determined from the file header.
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
        "text-audio" - Text audio file with header

  AF_INPUTPAR:
    This environment variable sets the default data parameters for input audio
    files. These parameters are used when opening input audio files with
    unrecognized (non-standard) headers or files with no headers (raw audio
    files). Note that the parameters do not override values specified in the
    file headers of standard input files.

    The parameters for AF_INPUTPAR are determined from an input string which
    consists of a list of parameters separated by commas. The form of the list
    is
        "Format, Start, Sfreq, Swapb, Nchan, FullScale"
    The default values for the audio file parameters correspond to the following
    string.
        "undefined, 0, 8000., native, 1, default"
    Missing parameters (nothing between adjacent commas or a shortened list)
    leave the default values unchanged.
       Format: File data type
         "undefined" - Headerless or non-standard files will be rejected
         "mu-law8"   - 8-bit mu-law data
         "mu-lawR8"  - 8-bit bit-reversed mu-law data
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
       Sfreq: sampling frequency in Hz (floating point number or ratio)
       Swapb: Data byte swap parameter
         "native"        - no byte swapping
         "little-endian" - file data is in little-endian byte order
         "big-endian"    - file data is in big-endian byte order
         "swap"          - swap the data bytes as the data is read
       Nchan: number of channels
         The data consists of interleaved samples from Nchan channels
       FullScale: full scale value
         This vale is used to scale data from the file to fall in the range -1
         to +1. If the FullScale value is set to "default", the FullScale value
         is determined based on the type of data in the file.
           data type                  FullScale
           8-bit mu-law:              32768
           8-bit bit-reversed mu-law: 32768
           8-bit A-law:               32768
           8-bit integer:             256
           16-bit integer:            32768
           24-bit integer:            256*32768 (838808)
           32-bit integer:            65536*32768 (2147483648)
           float data:                1
           text16 data:               32768
           text data:                 1
         The value of FullScale can be specified as a value or ratio of values.

  AUDIOPATH:
    This environment variable specifies a list of directories to be searched
    when opening the input audio files. Directories in the list are separated by
    colons (semicolons for Windows).

Author / version:
  P. Kabal / v10r3  2020-11-30  Copyright (C) 2020

-------------------------------------------------------------------------*/

#include <stdlib.h> /* EXIT_SUCCESS */
#include <string.h>

#include "CopyAudio.h"

#define ICEILV(n, m)  (((n) + ((m) - 1)) / (m)) /* int n,m >= 0 */

static long int
CP_Nchan(AFILE *AFp[], int Nifiles, int Mode);
static double
CP_Sfreq(AFILE *AFp[], int Nifiles);


int
main(int argc, const char *argv[])

{
  struct AO_FIpar FI[MAXIFILE];
  struct AO_FOpar FO;
  struct CP_Chgain Chgain;
  AFILE *AFp[MAXIFILE], *AFpO;
  FILE *fpinfo;
  int NsampND, Nifiles, i, Mode;
  long int Nsamp, NchanI, NchanO;
  double SfreqI, SfreqO;

/* Get the input parameters */
  CPoptions(argc, argv, &Mode, FI, &Nifiles, &FO, &Chgain);

/* If output is to stdout, use stderr for informational messages */
  if (strcmp(FO.Fname, "-") == 0)
    fpinfo = stderr;
  else
    fpinfo = stdout;

/* Files with number of samples unknown:
   Non-random access input from standard input may result in the number of
   samples being unknown. The lower level routines can handle the number of
   samples being unknown only when there is a single input file.
   Concatenate mode:
     CPcopyChan or CPcombChan is called for one input file at a time. An unknown
     number of samples is allowed.
   Combine mode:
     CPcopyChan or CPcombChan is called for all input files at once. An unknown
     number of samples is only allowed if the number of input files is one or
     the limits are specified (one set of limits applies to all files).

     For multiple input files (Nifiles > 1) in the combine mode
     (Mode == M_COMB), for each input file which has frame limits which are not
     defined (FO.Nframe == AF_NFRAME_UNDEF && FI[i].Lim[1] == CP_LIM_UNDEF), we
     require that the sample limits for that file be known (NsampND = 0).
*/

/* Open the input files */
  for (i = 0; i < Nifiles; ++i) {
    if (Mode == M_COMB && Nifiles > 1 && FO.Nframe == AF_NFRAME_UNDEF &&
        FI[i].Lim[1] == AO_LIM_UNDEF)
      NsampND = 0;
    else
      NsampND = 1;
    AOsetFIopt(&FI[i], NsampND, 0);    /* Allow/deny undefined Nsamp */
    FLpathList(FI[i].Fname, AFPATH_ENV, FI[i].Fname);
    AFp[i] = AFopnRead(FI[i].Fname, &Nsamp, &NchanI, &SfreqI, fpinfo);
    AFp[i]->ScaleF *= FI[i].Gain; /* Gain absorbed into scaling factor */
  }

/* Find the number of input channels */
  NchanI = CP_Nchan(AFp, Nifiles, Mode);
  if (Chgain.NCI > NchanI)
    UThalt(CPMF_MNChan, PROGRAM, Chgain.NCI);
  if (Chgain.NCO == 0)
    NchanO = NchanI;
  else
    NchanO = Chgain.NCO;

/* Check and set the input limits */
  FO.Nframe = CPlim(Mode, AFp, FI, Nifiles, FO.Nframe);

/* Check the sampling frequency */
  if (FO.Sfreq != 0.0)
    SfreqO = FO.Sfreq;
  else
    SfreqO = CP_Sfreq(AFp, Nifiles);

  fprintf(fpinfo, "\n");

/* Open the output file */
  AOsetFOopt(&FO);
  AOsetDFormat(&FO, AFp, Nifiles);
  if (strcmp(FO.Fname, "-") != 0)
    FLbackup(FO.Fname);
  AFpO = AFopnWrite(FO.Fname, FO.FtypeW, FO.DFormat.Format, NchanO, SfreqO,
                    fpinfo);

/* Scale and copy samples */
  CPcopy(Mode, AFp, FI, Nifiles, &Chgain, FO.Nframe, AFpO);

/* Close the audio files */
  for (i = 0; i < Nifiles; ++i)
    AFclose(AFp[i]);
  AFclose(AFpO);

  return EXIT_SUCCESS;
}

/* Check the number of channels for each file. */

static long int
CP_Nchan(AFILE *AFp[], int Nifiles, int Mode)

{
  int i;
  long int Nchan;

  if (Mode == M_CONCAT) {
    /* Concatenate files
       Nchan - Each input file must have the same number of channels. The
               default number of output channels is the same as for each of
               the input files.
    */
    Nchan = AFp[0]->Nchan;
    for (i = 0; i < Nifiles; ++i) {
      if (AFp[i]->Nchan != Nchan)
      UThalt("%s: %s", PROGRAM, CPM_DiffNChan);
    }
  }
  else {  /* Mode == M_COMB */
    /* Combine files
       Nchan - The default number of output channels is the sum of the number
               of input channels.
    */
    Nchan = 0L;
    for (i = 0; i < Nifiles; ++i)
      Nchan += AFp[i]->Nchan;
  }

  return Nchan;
}

/* Check the sampling frequency */

static double
CP_Sfreq(AFILE *AFp[], int Nifiles)

{
  int i, Sfdiff;
  double Sf, Stot, Sfreq;

  Sfdiff = 0;
  Sf = AFp[0]->Sfreq;
  Stot = 0.0;
  for (i = 0; i < Nifiles; ++i) {
    if (AFp[i]->Sfreq != Sf)
      Sfdiff = 1;
    Stot += AFp[i]->Sfreq;
  }

  if (Sfdiff) {
    UTwarn("%s - %s", PROGRAM, CPM_DiffSFreq);
    Sfreq = Stot / Nifiles;
  }
  else
    Sfreq = Sf;

  return Sfreq;
}
