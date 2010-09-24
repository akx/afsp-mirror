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
  P. Kabal  Copyright (C) 2009
  $Revision: 1.19 $  $Date: 2009/03/09 18:37:07 $

----------------------------------------------------------------------*/

#include <string.h>

#include <libtsp.h>
#define AF_DATA_LENGTHS
#include <libtsp/AFpar.h>
#include <AO.h>

#define ROUTINE		"AOdecFO"
#define PGM		((UTgetProg ())[0] == '\0' ? ROUTINE : UTgetProg ())
#define ERRSTOP(text,par)	UThalt ("%s: %s: \"%s\"", PGM, text, par)
#define NELEM(array)	((sizeof array) / (sizeof array[0]))

#define RET_ERROR	-2
#define RET_END		-1

static const char *OTFO[] = {
  "-n#", "--n*umber_samples=",
  "-s#", "--sr*ate=",
  "-D#", "--d*ata_format=",
  "-F#", "--f*ile_type=",
  "-S#", "--sp*eakers=",
  "-I#", "--i*nfo=",
  "**",
  NULL
};

static struct AO_dformat
AO_decDFormat (const char string[]);
static int
AO_decFType (const char string[]);

/*
  For certain options, the corresponding item in the audio file options
  structure is set.  These options will be picked up when an output file
  is opened.
    Speaker locations: The corresponding audio file option is set, viz.
      (AFoptions())->SpkrConfig
    Data format, number of bits/sample: The corresponding audio file option
    is set, viz.
      (AFoptions())->NbS
    Information string: The corresponding audio file option is set, viz.
      (AFoptions())->InfoS
*/


int
AOdecFO (struct AO_FOpar *FO)

{
  const char *OptArg;
  int n, Sindex, nc;
  double Nv, Dv;
  struct AO_CmdArg *Carg;
  struct AF_opt *AFopt;

  Carg = AOArgs ();
  AFopt = AFoptions ();

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
    if (STdecDfrac (OptArg, &Nv, &Dv) || Nv / Dv <= 0.0)
      ERRSTOP (AOM_BadSFreq, OptArg);
    FO->Sfreq = Nv / Dv;
    break;
  case 5:
  case 6:
    /* Data format */
    FO->DFormat = AO_decDFormat (OptArg);
    AFopt->NbS = FO->DFormat.NbS;
    break;
  case 7:
  case 8:
    /* File types */
    FO->Ftype = AO_decFType (OptArg);
    break;
  case 9:
  case 10:
    /* Speaker positions */
    if (AFsetSpeaker (OptArg))
      ERRSTOP (AOM_BadSpkr, OptArg);
    if (AFopt->SpkrConfig != NULL) {
      UTfree (FO->SpkrConfig);
      nc = strlen ((const char *) AFopt->SpkrConfig);
      FO->SpkrConfig = (unsigned char *) UTmalloc (nc + 1);
      STcopyMax ((const char *) AFopt->SpkrConfig, (char *) FO->SpkrConfig, nc);
    }
    break;
  case 11:
  case 12:
    /* Information string */
    FO->Info = OptArg;
    AFsetInfo (OptArg);
    break;
  default:
    Carg->Index = Sindex;		/* Reset the index */
    n = 0;
    break;
  }

  return n;
}

/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int AO_decFType (const char string[])

Purpose:
  Decode an audio file type specification

Description:
  This routine decodes an audio file type specification, returning the
  corresponding file type code.  In the case of an invalid specification, this
  routine stops with an error message.  For use in the error message, the
  program name should be set using the routine UTsetProg.

Parameters:
  <-  int AO_decFType
      File type code
   -> const char string[]
      Input audio file type specifier

Author / revision:
  P. Kabal  Copyright (C) 2009
  $Revision: 1.19 $  $Date: 2009/03/09 18:37:07 $

-------------------------------------------------------------------------*/

static const char *FTypeTab[] = {
  "AU", "au", "AFsp", "Sun", "sun",
  "W*AVE", "w*ave",
  "WAVE-NOEX", "wave-noex",
  "AIFF", "aiff",
  "AI*FF-C", "ai*ff-c",	/* Short forms go to AIFF-C rather than AIFF */
  "nohead*er_native",
  "noheader_s*wap",
  "noheader_b*ig-endian",
  "noheader_l*ittle-endian",
  NULL
};
#define NFTYPE		(NELEM (FTypeTab) - 1)
static const int FTypeCode[NFTYPE] = {
  FTW_AU, FTW_AU, FTW_AU, FTW_AU, FTW_AU,
  FTW_WAVE, FTW_WAVE,
  FTW_WAVE_NOEX, FTW_WAVE_NOEX,
  FTW_AIFF, FTW_AIFF,
  FTW_AIFF_C, FTW_AIFF_C,
  FTW_NH_NATIVE,
  FTW_NH_SWAP,
  FTW_NH_EB,
  FTW_NH_EL
};


static int
AO_decFType (const char string[])

{
  int n;

  n = STkeyMatch (string, FTypeTab);
  if (n < 0)
    UThalt ("%s: %s: \"%s\"", PGM, AOM_BadFType, string);

  return FTypeCode[n];
}

/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  struct AO_dformat AO_decDFormat (const char string[])

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

Author / revision:
  P. Kabal  Copyright (C) 2009
  $Revision: 1.19 $  $Date: 2009/03/09 18:37:07 $

-------------------------------------------------------------------------*/

static const char *DFormatTab[NFD] = {
  "m*u-law8",
  "A*-law8",
  "u*nsigned8",
  "integer8",
  "i*nteger16",
  "integer24",
  "integer32",
  "f*loat32",
  "float64",
  "t*ext",
  NULL
};
static const int DFormatCode[NFD-1] = {
  FD_MULAW8,
  FD_ALAW8,
  FD_UINT8,
  FD_INT8,
  FD_INT16,
  FD_INT24,
  FD_INT32,
  FD_FLOAT32,
  FD_FLOAT64,
  FD_TEXT
};

static struct AO_dformat
AO_decDFormat (const char string[])

{
  int n, Format, Res, NbS;
  char *Par;
  struct AO_dformat DFormat;

  Par = (char *) UTmalloc (strlen (string));
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
  DFormat.NbS = NbS;

  UTfree (Par);

  return DFormat;
}
