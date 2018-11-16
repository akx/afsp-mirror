/*------------- Telecommunications & Signal Processing Lab -------------
                           McGill University

Routine:
  int AOdecFI (struct AO_FIpar *FI)

Purpose:
  Decode input audio file options for audio utilities

Description:
  This routine decodes an option for input audio files.  The routine AOinitOpt
  (in file AOdecOpt.c) must be called first to initialize the option arguments.
  If this routine fails to find an input audio file option, the argument pointer
  is reset to allow another routine to try to decode the option.

  Options recognized:
      -t TYPE, --type=TYPE        Input file type
      -g GAIN, --gain=GAIN        Gain, e.g. "256" or "1/256"
      -P PAR, --parameters=PAR    Noheader file parameters

Parameters:
  <-  int AOdecFI
         -1 - End of arguments/options
          0 - Option not found
        >=1 - Option code
  <-> struct AO_FIpar *FI
      Input file parameters

Author / revision:
  P. Kabal  Copyright (C) 2018
  $Revision: 1.16 $  $Date: 2018/11/14 13:52:27 $

----------------------------------------------------------------------*/

#include <libtsp/nucleus.h>
#include <AO.h>

#define ROUTINE           "AOdecFI"
#define PGM               ((UTgetProg ())[0] == '\0' ? ROUTINE : UTgetProg ())
#define ERRSTOP(text,par) UThalt ("%s: %s: \"%s\"", PGM, text, par)

#define RET_ERROR -2
#define RET_END   -1

static const char *OTFI[] = {
  "-t#", "--t*ype=",
  "-g#", "--g*ain=",
  "-P#", "--p*arameters=",
  "**",
  NULL
};


int
AOdecFI (struct AO_FIpar *FI)

{
  const char *OptArg;
  int n, Sindex;
  double Nv, Dv;
  struct AO_CmdArg *Carg;

  /* Get the argument pointers */
  Carg = AOArgs ();

  if (Carg->EndOptions)
    return 0;

  /* Decode input file options */
  Sindex = Carg->Index;
  n = UTgetOption (&Carg->Index, Carg->Argc, Carg->Argv, OTFI, &OptArg);

  switch (n) {
  case RET_END:
    break;
  case RET_ERROR:
    UThalt ("%s: %s", PGM, AOM_BadOption);
    break;
  case 1:
  case 2:
    /* Type of input file */
    if (AFsetFileType (OptArg))
      ERRSTOP (AOM_BadFType, OptArg);
    FI->Ftype = AFopt.FtypeI;
    break;
  case 3:
  case 4:
    /* Gain for input files */
    if (STdecDfrac (OptArg, &Nv, &Dv))
      ERRSTOP (AOM_BadGain, OptArg);
    FI->Gain = Nv / Dv;
    break;
  case 5:
  case 6:
    /* Input file parameters */
    if (AFsetInputPar (OptArg))
      ERRSTOP (AOM_BadInputPar, OptArg);
    FI->InputPar = AFopt.InputPar;
    break;
  default:
    Carg->Index = Sindex;   /* Reset the index */
    n = 0;
    break;
  }

  return n;
}
