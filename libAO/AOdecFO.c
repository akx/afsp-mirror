/*------------- Telecommunications & Signal Processing Lab -------------
                           McGill University

Routine:
  int AOdecFO (struct AO_FOpar *FO)

Purpose:
  Decode output audio file options for audio utilities

Description:
  This routine decodes an option for an output audio file.  The routine
  AOinitOpt must be called first to initialize the option arguments.  If this
  routine fails to find an input audio file option, the argument pointer is
  reset to allow another routine to try to decode the option.

Parameters:
  <-  int AOdecFO
         -1 - End of arguments/options
          0 - Option not found
        >=1 - Option code
  <-> struct AO_FOpar *FO
      Output file parameters

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.30 $  $Date: 2017/09/22 00:34:24 $

----------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>

#include <AO.h>

#define ROUTINE           "AOdecFO"
#define PGM               ((UTgetProg ())[0] == '\0' ? ROUTINE : UTgetProg ())
#define ERRSTOP(text,par) UThalt ("%s: %s: \"%s\"", PGM, text, par)
#define NELEM(array)      ((int) ((sizeof array) / (sizeof array[0])))

#define RET_ERROR -2
#define RET_END   -1

static const char *OTFO[] = {
  "-n#", "--n*umber-samples=",
  "-s#", "--sr*ate=",
  "-D#", "--d*ata-format=",
  "-F#", "--f*ile-type=",
  "-S#", "--sp*eakers=",
  "-I#", "--i*nfo=",
  "**",
  NULL
};

static struct AO_dformat
AO_decDFormat (const char string[]);
static enum AF_FTW_T
AO_decFtypeW (const char string[]);

/*
  For certain options, the corresponding item in the audio file options
  structure is set.  These options will be picked up when an output file
  is opened.
    Speaker locations: The corresponding audio file option is set, viz.
      AFopt->SpkrConfig
    Data format, number of bits/sample: The corresponding audio file option
    is set, viz.
      AFopt->NbS
    Information string: The corresponding audio file option is set, viz.
      AFopt->InfoS
*/


int
AOdecFO (struct AO_FOpar *FO)

{
  const char *OptArg;
  int n, Sindex, nc;
  double Nv, Dv;
  struct AO_CmdArg *Carg;

  Carg = AOArgs ();

  if (Carg->EndOptions)
    return 0;

  /* Decode output file options */
  Sindex = Carg->Index;
  n = UTgetOption (&Carg->Index, Carg->Argc, Carg->Argv, OTFO, &OptArg);

  switch (n) {
  case RET_END:
    break;
  case RET_ERROR:
    UThalt ("%s: %s", PGM, AOM_BadOption);
    break;
  case 1:
  case 2:
    /* Number of frames */
    if (STdec1long (OptArg, &FO->Nframe) || FO->Nframe <= 0L)
      ERRSTOP (AOM_BadNFrame, OptArg);
    break;
  case 3:
  case 4:
    /* Sampling rate */
    if (STdecDfrac (OptArg, &Nv, &Dv) || Dv == 0 || Nv / Dv <= 0.0)
      ERRSTOP (AOM_BadSFreq, OptArg);
    FO->Sfreq = Nv / Dv;
    break;
  case 5:
  case 6:
    /* Data format */
    FO->DFormat = AO_decDFormat (OptArg);
    AFopt.NbS = FO->DFormat.NbS;
    break;
  case 7:
  case 8:
    /* File types */
    FO->FtypeW = AO_decFtypeW (OptArg);
    break;
  case 9:
  case 10:
    /* Speaker positions */
    if (AFsetSpeaker (OptArg))
      ERRSTOP (AOM_BadSpkr, OptArg);
    if (AFopt.SpkrConfig != NULL) {
      UTfree (FO->SpkrConfig);
      nc = (int) strlen ((const char *) AFopt.SpkrConfig);
      FO->SpkrConfig = (unsigned char *) UTmalloc (nc + 1);
      STcopyMax ((const char *) AFopt.SpkrConfig, (char *) FO->SpkrConfig, nc);
    }
    break;
  case 11:
  case 12:
    /* Information string */
    AFsetInfo (OptArg);
    break;
  default:
    Carg->Index = Sindex;   /* Reset the index */
    n = 0;
    break;
  }

  return n;
}

/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  static int AO_decFtypeW (const char string[])

Purpose:
  Decode an audio write file type specification

Description:
  This routine decodes an audio write file type specification, returning the
  corresponding file type code.  In the case of an invalid specification, this
  routine stops with an error message.  For use in the error message, the
  program name should be set using the routine UTsetProg.

Parameters:
  <-  static int  AO_decFtypeW
      File type code
   -> const char string[]
      Input audio file type specifier

-------------------------------------------------------------------------*/

static const char *FtypeTab[] = {
  "AU", "au", "AFsp", "Sun", "sun",
  "W*AVE", "w*ave",
  "WAVE-EX", "wave-ex",
  "WAVE-NOEX", "wave-noex",
  "AIFF", "aiff",
  "AIFF-C", "aiff-c",
  "AIFF-C/sowt", "aiff-c/sowt",
  "nohead*er-native",
  "noheader-s*wap",
  "noheader-b*ig-endian",
  "noheader-l*ittle-endian",
  "text*-audio",
  NULL
};
#define NFTYPE    (NELEM (FtypeTab) - 1)
static const enum AF_FTW_T FtypeCode[NFTYPE] = {
  FTW_AU, FTW_AU, FTW_AU, FTW_AU, FTW_AU,
  FTW_WAVE, FTW_WAVE,
  FTW_WAVE_EX, FTW_WAVE_EX,
  FTW_WAVE_NOEX, FTW_WAVE_NOEX,
  FTW_AIFF, FTW_AIFF,
  FTW_AIFF_C, FTW_AIFF_C,
  FTW_AIFF_C_SOWT, FTW_AIFF_C_SOWT,
  FTW_NH_NATIVE,
  FTW_NH_SWAP,
  FTW_NH_EB,
  FTW_NH_EL,
  FTW_TXAUD
};


static enum AF_FTW_T
AO_decFtypeW (const char string[])

{
  int n;

  n = STkeyMatch (string, FtypeTab);
  if (n < 0)
    UThalt ("%s: %s: \"%s\"", PGM, AOM_BadFType, string);

  return FtypeCode[n];
}

/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  static struct AO_dformat AO_decDFormat (const char string[])

Purpose:
  Decode an audio file data format specification

Description:
  This routine decodes an audio file data format specification, returning the
  corresponding data format code.  In the case of an invalid specification,
  this routine stops with an error message.  For use in the error message, the
  program name should be set using the routine UTsetProg.

Parameters:
  <-  struct AO_dformat AOdecDFormat
      Data format structure
   -> const char string[]
      Input data format specifier

-------------------------------------------------------------------------*/

static const char *DFormatTab[AF_NFD] = {
  "mu-law*8",
  "mu-lawR*8",
  "A-law*8",
  "unsigned8",
  "integer8",
  "int*eger16",
  "integer24",
  "integer32",
  "float*32",
  "float64",
  "text16",
  "text",
  NULL
};
static const enum AF_FD_T DFormatCode[AF_NFD-1] = {
  FD_MULAW8,
  FD_MULAWR8,
  FD_ALAW8,
  FD_UINT8,
  FD_INT8,
  FD_INT16,
  FD_INT24,
  FD_INT32,
  FD_FLOAT32,
  FD_FLOAT64,
  FD_TEXT16,
  FD_TEXT
};

static struct AO_dformat
AO_decDFormat (const char string[])

{
  enum AF_FD_T Format;
  int n, Res, NbS;
  char *Par;
  struct AO_dformat DFormat;

  assert (NELEM (DFormatCode) == AF_NFD-1 && NELEM (DFormatTab) == AF_NFD);
  
  Par = (char *) UTmalloc ((int) strlen (string));
  n = STkeyXpar (string, "/", "", DFormatTab, Par);
  if (n < 0)
    UThalt ("%s: %s: \"%s\"", PGM, AOM_BadData, string);
  Format = DFormatCode[n];

  Res = 8 * AF_DL[Format];
  NbS = Res;
  if (strlen (Par) > 0) {
    STdec1int (Par, &NbS);
    if (NbS <= 0 || NbS > Res)
      UThalt ("%s: %s: \"%s\"", PGM, AOM_InvNbS, NbS);
  }

  DFormat.Format = Format;
  DFormat.NbS    = NbS;

  UTfree (Par);

  return DFormat;
}
