/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  CopyAudio [options] AFileA AFileB ... AFileO

Purpose:
  Copy audio files (combine/concatenate, scale and shift data)

Description:
  This program copies samples from one or more input audio files to an output
  audio file.  The samples in the output file are linear combinations of the
  samples in the different channels in the input files.  In the combine mode,
  data from multiple input files is combined.  In the concatenate mode, data
  from multiple input files is concatenated.

  The output consists of linear combinations of the input data channels.  For
  instance output channel A can be formed as the average of input channel A and
  input channel B.  For multichannel data, the data is organized into sample
  frames, with samples from individual channels making up a frame.

  Sample limits may be specified for the input data.  The same sample limits
  apply to each channel in a file.  Samples in the file are numbered from zero.
  Negative sample limits may be specified; the samples corresponding to 
  negative indices have zero values.  Similarly, sample limits beyond the
  end-of-file may be specified; samples beyond the end-of-file are assumed to
  have zero values.

  The combine and concatenate modes differ in how they treat multiple input
  files.  For the combine mode, if there is more than one input file, channels
  assignments cross between input files.  Consider two input files, with the
  first having 3 channels, and the second having 2 channels.  The channels are
  labelled A to E, with channels A, B and C coming from the first input file
  and channels D and E coming from the second input file.  For the combine
  mode, a single sample limits specification applies to all input files.  The
  default sample limits correspond to the length of the longest input file.
  If necessary, the data from the input files is padded with zeros to a common
  length before combining.

  In the concatenate mode, the data in the input files is concatenated.  The
  number of channels in each input file must be the same.  For the concatenate
  mode, sample limits can be specified for each input file.  The default sample
  limits for an input file correspond to the length of that file.

  For fixed point data, the data is normalized to the range -1 to +1.  This
  normalization becomes important when files with different data formats are
  combined.

  The default data format for the output file is chosen according to a data
  format promotion rule based on the data types of the input files.  For single
  input files, the output data format will be the same as the input data format
  as long as that data format is compatible with the output file type.

Options:
  Input file names: AFileA AFileB ...:
      The environment variable AUDIOPATH specifies a list of directories to be
      searched for the input audio file(s).  Specifying "-" as the input file
      name indicates that input is from standard input.
  Output file name: AFileO:
      The last file name is the output file.  Specifying "-" as the output file
      name indicates that output is to be written to standard output.  If the
      output file type is not explicitly given (-F option), the extension of
      the output file name is used to determine the file type.
        ".au"   - AU audio file
        ".wav"  - WAVE file
        ".aif"  - AIFF sound file
        ".afc"  - AIFF-C sound file
        ".raw"  - Headerless file (native byte order)
        ".txt"  - Headerless file (text data)
  -c, --combine
      Use the combine mode for multiple input files (default)
  -C, --concatenate
      Use the concatenate mode for multiple input files
  -g GAIN, --gain=GAIN
      A gain factor applied to the data from the input files.  This gain
      applies to all channels in a file and is applied in addition to the
      channel scaling factors.  The gain value can be given as a real number
      (e.g., "0.003") or as a ratio (e.g., "1/256"). This option may be
      given more than once.  This option may be given more than once.  Each
      invocation applies to the input files that follow the option.
  -l L:U, --limits=L:U
      Sample limits for the input files (numbered from zero).  For the combine
      mode, the default sample limits correspond to the limits of the longest
      input audio file.  For the concatenate mode, this option may be given
      more than once.  Each invocation applies to the input files that follow
      the option.  The specification ":" means the entire file; "L:" means from
      sample L to the end; ":U" means from sample 0 to sample U; "N" means from
      sample 0 to sample N-1.
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
  -s SFREQ, --srate=SFREQ
      Sampling frequency for the output file, default from the input audio
      file(s).  This option only changes the sampling frequency field in the
      output header; the audio data itself is unaffected.
  -n NSAMPLE, --number_samples=NSAMPLE
      Number of samples (per channel) for the output file.
  -cA CGAINS, --chanA=CGAINS
      Channel scaling  factors for output channel A.  The output data for the
      specified output channel is created by adding scaled samples from the
      specified input channels.  The default is to have each output channel
      equal to the corresponding input channel.  Input channels are labelled
      A, B, C, ... , R, S, T.  The channel scaling factor string takes the
      form
        [+|-] [gain *] chan +|- [gain *] chan ... +|- offset
      where chan is A through L.  The gains can be expressed as ratios, i.e.
      of the form "n/m".  The offset is in normalized units, where an offset
      of one corresponds to full scale.  Note that that the character "*" is
      a special character to Unix shells and should appear only within quotes
      to prevent the shell from interpreting it.
  -cB CGAINS, --chanB=CGAINS
      Channel scaling factors for output channel B.
        ...
  -cL CGAINS, --chanL=CGAINS
      Channel scaling factors for output channel L.
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
      Optionally, the number of "valid" bits may be specified as a qualifier
      to the data format, e.g. "integer16/12".  The number of valid bits is
      for informational purposes only.
  -S SPEAKERS, --speakers=SPEAKERS
      Mapping of output channels to speaker positions.  The spacial positions
      of the loudspeakers are specified as a list of white-space separated
      locations from the list below.
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
      A speaker position can be associated with only one channel.  In the case
      of WAVE files, the subset of spacial positions must appear in the order
      given above.
  -I INFO, --info=INFO
      Audio file information string for the output file.
  -h, --help
      Print a list of options and exit.
  -v, --version
      Print the version number and exit.

  This program allows direct specification of the gains for 12 output channels
  and 20 input channels.  The program can handle larger numbers of channels
  for the case that the input channels are in a one-to-one correspondence with
  the output channels.  A gain factor applying to all channels can be specified
  with the -g or --gain option.

  By default, the output file contains a standard audio file information
  string.
    Standard Audio File Information:
       date: 2001-01-25 19:19:39 UTC    date
       program: CopyAudio               program name
  This information can be changed with the information string which is
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
   1: File convert.
      Copy audio file abc.au to abc.wav.  The output audio file is a WAVE file
      with the same data type (if possible) as the input file.  The number of
      channels in the output file is the same as the number of channels in the
      input file.
        CopyAudio abc.au abc.wav
   2: Concatenate audio data.
      Form a audio file which is the concatenation of the data from two input
      files.
        CopyAudio -C abc1.au abc2.au abc12.au
   3: Difference between values.
      Create an output audio file in which each sample is the difference
      between corresponding samples in two single channel input audio files.
        CopyAudio --chanA=A-B abc1.au abc2.au diff.au
   4: Scale sample values.
      Scale the samples in the input single-channel file by 0.5.
        CopyAudio --gain=1/2 abc.au scaled.au
      The same result can be obtained by specifying the gain for the (single)
      output channel.
        CopyAudio --chanA="0.5*A" abc.au scaled.au
   5: Byte-swap data values.
      Let the input audio file be headerless and contain 16-bit data.  Create a
      headerless audio file with byte-swapped data.
        CopyAudio -P integer16 -F noheader_swap abc.au swap.au
   6: Extract samples.
      Extract samples 1000 to 1999 inclusive from the input audio file. The
      output audio file will have 1000 samples.
        CopyAudio -l 1000:1999 abc.au out.au
   7: Create a stereo file.
      Form a stereo (2-channel) audio file from two single channel audio files.
        CopyAudio abc1.wav abc2.wav -S "FL FR" stereo.wav
   8: Add a dc value to a file.
      If the input file has a mean value of 8.63/32768, the output file will
      have a zero mean after adding -8.63/32768 to each sample.
        CopyAudio --chanA="A-8.63/32768" abc.au zeromean.au

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
  The list can be shortened and elements skipped (nothing between adjacent
  commas.  The missing parameters take on default values.
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
  P. Kabal / v6r0  2003-05-08  Copyright (C) 2006

-------------------------------------------------------------------------*/

#include <stdlib.h>	/* EXIT_SUCCESS */
#include <string.h>

#include <libtsp.h>
#include <libtsp/AFheader.h>
#include <libtsp/AFpar.h>
#include <AO.h>
#include "CopyAudio.h"

#define ICEILV(n, m)	(((n) + ((m) - 1)) / (m))	/* int n,m >= 0 */


static long int
CP_Nchan (AFILE *AFp[], int Nifiles, int Mode);
static double
CP_Sfreq (AFILE *AFp[], int Nifiles);


int
main (int argc, const char *argv[])

{
  struct CP_FIpar FI[MAXIFILE];
  struct CP_FOpar FO;
  struct CP_Chgain Chgain;
  AFILE *AFp[MAXIFILE], *AFpO;
  FILE *fpinfo;
  int Ftype, Dformat;
  int NsampND, Nifiles, i, Mode;
  long int Nsamp, NchanI, NchanO;
  double SfreqI, SfreqO;

/* Get the input parameters */
  CPoptions (argc, argv, &Mode, FI, &Nifiles, &FO, &Chgain);

/* If output is to stdout, use stderr for informational messages */
  if (strcmp (FO.Fname, "-") == 0)
    fpinfo = stderr;
  else
    fpinfo = stdout;

/* Files with number of samples unknown:
   Non-random access input from standard input may result in the number of
   samples being unknown.  The lower level routines can handle the number of
   samples being unknown only when there is a single input file.
   Concatenate mode:
     CPcopySamp or CPcompSamp is called for one input file at a time.  An
     unknown number of samples is allowed.
   Combine mode:
     CPcopySamp or CPcompFile is called for all input files at once.  An
     unknown number of samples is only allowed if the number of input files
     is one or the limits are specified (one set of limits applies to all
     files).

     For multiple input files (Nifiles > 1) in the combine mode
     (Mode == M_COMB), for each input file which has frame limits which are not
     defined (FO.Nframe == AF_NFRAME_UNDEF && FI[i].Lim[1] == CP_LIM_UNDEF),
     we require that the sample limits for that file be known (NsampND = 0).
*/

/* Open the input files */
  for (i = 0; i < Nifiles; ++i) {
    if (Mode == M_COMB && Nifiles > 1 && FO.Nframe == AF_NFRAME_UNDEF &&
	FI[i].Lim[1] == CP_LIM_UNDEF)
      NsampND = 0;
    else
      NsampND = 1;
    AOsetFIopt (&FI[i], NsampND, 0);
    FLpathList (FI[i].Fname, AFPATH_ENV, FI[i].Fname);
    AFp[i] = AFopnRead (FI[i].Fname, &Nsamp, &NchanI, &SfreqI, fpinfo);
    AFp[i]->ScaleF *= FI[i].Gain;	/* Gain absorbed into scaling factor */
  }

/* Find the number of input channels */
  NchanI = CP_Nchan (AFp, Nifiles, Mode);
  if (Chgain.NI > NchanI)
    UThalt (CPMF_MNChan, PROGRAM, Chgain.NI);
  if (Chgain.NO == 0)
    NchanO = NchanI;
  else
    NchanO = Chgain.NO;

/* Check and set the input limits */
  FO.Nframe = CPlim (Mode, AFp, FI, Nifiles, FO.Nframe);

/* Check the sampling frequency */
  if (FO.Sfreq != 0.0)
    SfreqO = FO.Sfreq;
  else
    SfreqO = CP_Sfreq (AFp, Nifiles);

  fprintf (fpinfo, "\n");

/* Open the output file */
  AOsetFOopt (&FO);
  Ftype = AOsetFtype (&FO);
  Dformat = AOsetDformat (&FO, AFp, Nifiles);
  if (strcmp (FO.Fname, "-") != 0)
    FLbackup (FO.Fname);
  AFpO = AFopnWrite (FO.Fname, Ftype, Dformat, NchanO, SfreqO, fpinfo);

/* Scale and copy samples */
  CPcopy (Mode, AFp, FI, Nifiles, &Chgain, FO.Nframe, AFpO);

/* Close the audio files */
  for (i = 0; i < Nifiles; ++i)
    AFclose (AFp[i]);
  AFclose (AFpO);

  return EXIT_SUCCESS;
}

/* Check the number of channels for each file. */


static long int
CP_Nchan (AFILE *AFp[], int Nifiles, int Mode)

{
  int i;
  long int Nchan;

  if (Mode == M_CONCAT) {

    /* Concatenate files
       Nchan - Each input file must have the same number of channels.  The
               default number of output channels is the same as for each of
	       the input files.
    */
    Nchan = AFp[0]->Nchan;
    for (i = 0; i < Nifiles; ++i) {
      if (AFp[i]->Nchan != Nchan)
	UThalt ("%s: %s", PROGRAM, CPM_DiffNChan);
    }

  }
  else {	/* Mode == M_COMB */

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
CP_Sfreq (AFILE *AFp[], int Nifiles)

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
    UTwarn ("%s - %s", PROGRAM, CPM_DiffSFreq);
    Sfreq = Stot / Nifiles;
  }
  else
    Sfreq = Sf;

  return Sfreq;
}
