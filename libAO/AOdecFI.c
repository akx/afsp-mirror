/*------------- Telecommunications & Signal Processing Lab -------------
                           McGill University

Routine:
  int AOdecFI (struct AO_FIpar *FI)

Purpose:
  Decode input audio file options for audio utilities

Description:
  This routine decodes an option for input audio files.  The routine AOinitOpt
  must be called first to initialize the option arguments.  If this routine
  fails to find an input audio file option, the argument pointer is reset to 
  allow another routine to try to decode the option.

  Options recognized:
      -t TYPE, --type=TYPE        Input file type
      -l LIMITS, --limits=LIMITS  Frame limits, e.g., "25:999" or ":" or ...
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
  P. Kabal  Copyright (C) 2003
  $Revision: 1.11 $  $Date: 2003/05/12 23:51:12 $

----------------------------------------------------------------------*/

#include <libtsp.h>
#include <libtsp/nucleus.h>
#include <AO.h>

#define ROUTINE		"AOdecFI"
#define PGM		((UTgetProg ())[0] == '\0' ? ROUTINE : UTgetProg ())
#define ERRSTOP(text,par)	UThalt ("%s: %s: \"%s\"", PGM, text, par)

#define RET_ERROR	-2
#define RET_END		-1

static const char *OTFI[] = {
  "-t#", "--t*ype=",
  "-l#", "--l*imits=",
  "-g#", "--g*ain=",
  "-P#", "--p*arameters=",
  "**",
  NULL
};

static int
AO_decLrange (const char String[], const long int D[2], long int Lim[2]);


int
AOdecFI (struct AO_FIpar *FI)

{
  const char *OptArg;
  int n, Sindex;
  double Nv, Dv;
  struct AO_CmdArg *Carg;
  struct AF_opt *AFopt;
  long int D[2] = {0, AO_LIM_UNDEF};

  AFopt = AFoptions ();

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
    FI->Ftype = AFopt->FtypeI;
    break;
  case 3:
  case 4:
    /* Limits specification */
    if (AO_decLrange (OptArg, D, FI->Lim)
	|| (FI->Lim[1] != AO_LIM_UNDEF && FI->Lim[0] > FI->Lim[1]))
      ERRSTOP (AOM_BadLimits, OptArg);
    break;
  case 5:
  case 6:
    /* Gain for input files */
    if (STdecDfrac (OptArg, &Nv, &Dv))
      ERRSTOP (AOM_BadGain, OptArg);
    FI->Gain = Nv / Dv;
    break;
  case 7:
  case 8:
    /* Headerless input parameters */
    if (AFsetNHpar (OptArg))
      ERRSTOP (AOM_BadNHPar, OptArg);
    FI->NHpar = AFopt->NHpar;
    break;
  default:
    Carg->Index = Sindex;		/* Reset the index */
    n = 0;
    break;
  }

  return n;
}

/* Decode a range value, with default values
   "123:345"	returns (123, 345)
   "123:"	returns (123, D[1])
   ":345"	returns (D[0], 345)
   "123"	returns (D[0], D[0] + 123 - 1)
*/
#define DP_EMPTY		0
#define DP_LVAL			1
#define DP_DELIM		2
#define DP_RVAL			4	


static int
AO_decLrange (const char String[], const long int D[2], long int Lim[2])

{
  int status;
  long int L0, L1;

  /* Decode the range values */
  status = STdecPair (String, ":", 'L', (void *) (&L0), (void *) (&L1));

  switch (status) {
  case (DP_LVAL):
    Lim[0] = D[0];
    Lim[1] = D[0] + L0 - 1;
    break;
  case (DP_DELIM):
    Lim[0] = D[0];
    Lim[1] = D[1];
    break;
  case (DP_LVAL + DP_DELIM):
    Lim[0] = L0;
    Lim[1] = D[1];
    break;
  case (DP_DELIM + DP_RVAL):
    Lim[0] = D[0];
    Lim[1] = L1;
    break;
  case (DP_LVAL + DP_DELIM + DP_RVAL):
    Lim[0] = L0;
    Lim[1] = L1;
    break;
  }

  return (status <= 0);
}
