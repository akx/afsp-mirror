/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  FiltAudio [options] -f FilterFile AFileI AFileO

Purpose:
  Filter data from a single channel audio file

Description:
  This program takes an audio file and a filter coefficient file as input and
  produces a filtered output audio file. Subsampling and interpolation factors
  can be optionally specified. This program supports three types of filters,
  FIR, all-pole, and general IIR. Filters are specified in filter files.

  Filter Files:
  The first record of a filter file indicates the type of filter.
    !FIR  - FIR filter, direct form
    !IIR  - IIR filter, cascade of biquad sections (5 coefficients per section)
    !ALL  - All-pole filter, direct form
  Subsequent records contain text lines with filter coefficients. Comment
  records ('!' in the first position of the record) can be interspersed amongst
  the data. Data records are free form, with data values separated by
  white-space (blanks, tabs and newlines). Commas can also be used to separate
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

  For FIR filters, a sample rate change can be affected with interpolation and
  subsampling. For IIR and all-pole filters, the output can be subsampled. Let
  Ir and Nsub be the interpolation and subsampling factors, respectively.
  Conceptually, the rate change is accomplished as follows.
   1: Ir-1 zeros are inserted between samples of the input to increase the
      sampling rate by a factor of Ir.
   2: The increased rate signal is filtered.
   3: The result of the filtering is subsampled by a factor of Nsub to form the
      output signal.

  The initial filter alignment and the number of output samples can be specified
  with options. The filter alignment specifies the position of the filter
  relative to the input data for calculating the first output sample. For FIR
  filters, this alignment is relative to the increased rate input sequence.
  Specifically, let the number of samples in the input file be Nin. The input
  can be considered to be an array x(0),...,x(Nin-1). The increased rate
  sequence is xi(.), with xi(k*Ir)=x(k). The first output sample is calculated
  with the beginning of the impulse response of the filter aligned with
  xi(idoffs). The array xi(.) can be considered to be of length Nin*Ir; the
  first non-zero sample is xi(0)=x(0), the last non-zero sample is
  xi((Nin-1)*Ir). Conceptually, the impulse impulse response is moved in steps
  of Nsub to create the output samples.

  The intent is that the output samples are the subset of the values the would
  be obtained if the infinite length sequence formed by padding out the input
  data on either end with zeros were to be filtered. To this end, the filter
  calculations need warm-up points, particularly for the case that the initial
  filter alignment is not at the beginning of the input data. For FIR filters,
  this is taken into account by reading previous input values into the filter
  memory. For IIR and all-pole filters, previous outputs are also needed as
  warm-up points. If the initial alignment is near the beginning of the data,
  the IIR filter is run from the beginning of the data to generate the warm-up
  points. For larger alignment offsets, the IIR filter is backed up for a
  maximum of 1000 samples to provide the warm-up points.

  If the initial filter alignment is not explicitly specified, it is chosen to
  be zero, except for symmetric or anti-symmetric FIR filters. In those cases
  the default alignment is Ncof/2-1 for even length filters and (Ncof-1)/2 for
  odd length filters. For symmetric/anti-symmetric FIR filters, this offset sets
  the group delay to 0 samples (odd number of coefficients) or to 1/Nsub samples
  (even number of coefficients).

  If both the offset and the number of output samples is not explicitly set, the
  number of output samples is floor(Ir*Nin/Nsub). For the case of Ir=1 and
  Nsub=1, this results in the same number of output samples as input samples. If
  the filter alignment offs is explicitly set and the number of samples is not
  explicitly set, the number of output samples is floor((Ir*Nin-offs)/Nsub.

Options:
  Input file name, AFileI:
      The environment variable AUDIOPATH specifies a list of directories to be
      searched for the input audio file. Specifying "-" as the input file
      indicates that input is from standard input (use the "-t" option to
      specify the format of the input data). The input file must have a single
      channel.
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
  -i IR/NSUB, --interpolate=IR/NSUB
      Filter interpolation ratio, default 1/1. The interpolation and subsampling
      factors are specified as a fraction Ir/Nsub. Interpolation can only be
      used with FIR filters.
  -a OFFS, --alignment=OFFS
      Alignment of data relative to the filter. The first output sample is
      calculated with the beginning of the filter response aligned with the
      specified sample of the interpolated data sequence. The default setting is
      described above.
  -g GAIN, --gain=GAIN
      A gain factor applied to the data from the input file. This gain applies
      to all channels in a file. The gain value can be given as a real number
      (e.g., "0.003") or as a ratio (e.g., "1/256").
  -n NSAMPLE, --number_samples=NSAMPLE
      Number of samples (per channel) for the output file. If not specified, the
      number of samples is set as described above.
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

Environment variables:
  AUDIOPATH:
    This environment variable specifies a list of directories to be searched
    when opening the input audio files. Directories in the list are separated by
    colons (semicolons for Windows).

Author / version:
  P. Kabal / v10r3  2020-11-30  Copyright (C) 2020

-------------------------------------------------------------------------*/

#include <stdlib.h> /* EXIT_SUCCESS */
#include <string.h>

#include <libtsp.h>
#include <AFpar.h>
#include <libtsp/FIpar.h>
#include <AO.h>
#include "FiltAudio.h"

#define CHECKSYM(x,N) ((int) (1.00001 * VRdCorSym(x,N)))

static void
FA_checkBuffer(int FiltType, int Ncof, int Nsub, int Ir);


int
main(int argc, const char *argv[])

{
  struct FA_FIpar FI;
  struct FA_FFpar FF;
  struct FA_FOpar FO;
  AFILE *AFpI, *AFpO;
  FILE *fpinfo;
  int FiltType, Ncof, Nsec, DoffsFlag;
  long int Nsamp, Nchan, NframeI;
  double SfreqI;
  double h[MAXCOF];

/* Get the input parameters */
  FAoptions(argc, argv, &FI, &FF, &FO);

/* If output is to stdout, use stderr for informational messages */
  if (strcmp(FO.Fname, "-") == 0)
    fpinfo = stderr;
  else
    fpinfo = stdout;

/* Open the input audio file */
  AOsetFIopt(&FI, 0, 0);       /* Nsamp must be known */
  FLpathList(FI.Fname, AFPATH_ENV, FI.Fname);
  AFpI = AFopnRead(FI.Fname, &Nsamp, &Nchan, &SfreqI, fpinfo);
  if (Nchan != 1)
    UThalt("%s: %s", PROGRAM, FAM_XNchan);
  AFpI->ScaleF *= FI.Gain;  /* Gain absorbed into scaling factor */

/* Default number of output samples */
  NframeI = AOnFrame(&AFpI, &FI, 1, AF_NFRAME_UNDEF);

/* Read the coefficient file */
  FiltType = FIdReadFilt(FF.Fname, MAXCOF, h, &Ncof, fpinfo);
  if (FiltType != FI_IIR && FiltType != FI_FIR && FiltType != FI_ALL)
    UThalt("%s: %s", PROGRAM, FAM_BadFiltType);
  if (Ncof <= 0)
    UThalt("%s: %s", PROGRAM, FAM_NoCoef);
  if (FiltType == FI_ALL && FF.Ir != 1)
    UThalt("%s: %s", PROGRAM, FAM_APNoInt);
  if (FiltType == FI_IIR && FF.Ir != 1)
    UThalt("%s: %s", PROGRAM, FAM_IIRNoInt);
  if (FO.Sfreq > 0 && FF.Nsub * FO.Sfreq != FF.Ir * SfreqI)
    UThalt("%s: %s", PROGRAM, FAM_BadSFreqRatio);
  fprintf(fpinfo, "\n");

/* Set the data offset */
  DoffsFlag = (FF.Doffs == DOFFS_UNDEF);
  if (DoffsFlag) {      /* Use default offset */
    if (FiltType == FI_FIR && CHECKSYM(h, Ncof) != 0) {
      if (Ncof % 2 == 0)
        FF.Doffs = Ncof / 2 - 1;
      else
        FF.Doffs = (Ncof - 1) / 2;
    }
    else
      FF.Doffs = 0;
  }

/* Set the number of output samples */
  if (FO.Nframe == AF_NFRAME_UNDEF) {
    if (DoffsFlag)
      FO.Nframe = (FF.Ir * NframeI) / FF.Nsub;
    else
      FO.Nframe = (FF.Ir * NframeI - FF.Doffs) / FF.Nsub;
  }

/* Set data format information into FO */
  AOsetDFormat(&FO, &AFpI, 1);
  FO.Sfreq = (FF.Ir * SfreqI) / FF.Nsub;
/* Set AFopt.Nframe, AFopt.Nbs, AFopt.SpkrConfig used by AFpreSetWPar */
  if (FO.SpkrConfig == NULL)
    FO.SpkrConfig = AFpI->SpkrConfig;
  AOsetFOopt(&FO);

/* Check output file parameters */
   FA_checkBuffer(FiltType, Ncof, FF.Nsub, FF.Ir);

/* Open the output audio file */
  if (strcmp(FO.Fname, "-") != 0)
    FLbackup(FO.Fname);
  AFpO = AFopnWrite(FO.Fname, FO.FtypeW, FO.DFormat.Format, 1L, FO.Sfreq,
                    fpinfo);

/* Filtering */
  switch (FiltType) {
    case FI_FIR:
      if (FF.Nsub == 1 && FF.Ir == 1)
        FAfiltFIR(AFpI, AFpO, FO.Nframe, h, Ncof, FF.Doffs);
      else
        FAfiltSI(AFpI, AFpO, FO.Nframe, h, Ncof, FF.Nsub, FF.Ir, FF.Doffs);
      break;
    case FI_IIR:
      Nsec = Ncof / 5;
      FAfiltIIR(AFpI, AFpO, FO.Nframe, (const double (*)[5]) h, Nsec,
                FF.Nsub, FF.Doffs);
      break;
    case FI_ALL:
      FAfiltAP(AFpI, AFpO, FO.Nframe, h, Ncof, FF.Nsub, FF.Doffs);
      break;
    default:
      UThalt("%s: %s", PROGRAM, FAM_BadFiltType);
     break;
  }

/* Close the audio files */
  AFclose(AFpI);
  AFclose(AFpO);

  return EXIT_SUCCESS;
}

static void
FA_checkBuffer(int FiltType, int Ncof, int Nsub, int Ir)

{
  int lmem, Nb, Nxmax, Nsec;

  switch (FiltType) {
    case FI_FIR:
      lmem = (Ncof-1) / Ir;
      Nb = NBUF - lmem;
      if (Nb <= 0)
        UThalt("%s: %s", PROGRAM, FAM_XNcof);
      if (Nsub != 1 || Ir != 1) {
        Nxmax = ((Nb-1)*Nsub + 1) / (Nsub+Ir);
        if (Nxmax <= 0)
          UThalt("%s: %s", PROGRAM, FAM_XNcofIr);
      }
      break;
    case FI_IIR:
      Nsec = Ncof / 5;
      lmem = 2 * (Nsec + 1);
      if (NBUF - lmem <= 0)
        UThalt("%s: %s", PROGRAM, FAM_XIIRSect);
      break;
    case FI_ALL:
      lmem = Ncof - 1;
      if (NBUF - lmem <= 0)
        UThalt("%s: %s", PROGRAM, FAM_XNcof);
      break;
    default:
      UThalt("%s: %s", PROGRAM, FAM_BadFiltType);
     break;
  }

}
