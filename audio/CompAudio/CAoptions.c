/*------------- Telecommunications & Signal Processing Lab -------------
                           McGill University

Routine:
  void CAoptions(int argc, const char *argv[], long int Delay[2],
                 struct AO_FIpar FI[2])

Purpose:
  Decode options for CompAudio

Description:
  This routine decodes options for CompAudio.

Parameters:
   -> int argc
      Number of command line arguments
   -> const char *argv[]
      Array of pointers to argument strings
  <-  long int Delay[2]
      Starting/end delay
  <-  struct AO_FIpar FI[2]
      Input file parameters

Author / revision:
  P. Kabal  Copyright (C) 2020
  $Revision: 1.52 $  $Date: 2020/11/23 18:30:35 $

----------------------------------------------------------------------*/

#include <assert.h>

#include "CompAudio.h"

#define ERRSTOP(text,par) UThalt("%s: %s: \"%s\"", PROGRAM, text, par)

/* Option table */
static const char *OptTable[] = {
  "-d#", "--d*elay=",
  "-l#", "--l*imits=",
  NULL
};


void
CAoptions(int argc, const char *argv[], long int Delay[2],
          struct AO_FIpar FI[2])

{
  struct AO_FIpar FIx;
  int n, nF, FIParSet;
  const char *OptArg;

/* Input file defaults */
  FIParSet = 0;
  FIpar_INIT(&FIx);

  Delay[0] = 0;
  Delay[1] = 0;

/* Initialization */
  UTsetProg(PROGRAM);
  nF = 0;

/* Decode options */
  AOinitOpt(argc, argv);
  while (1) {

    /* Decode input file options */
    n = AOdecFI(&FIx);
    if (n >= 1) {
      FIParSet = nF;
      continue;
    }

    /* Decode help options */
    n = AOdecHelp(VERSION, CAMF_Usage);
    if (n >= 1)
      continue;

    /* Decode program options */
    n = AOdecOpt(OptTable, &OptArg);
    if (n == -1)
      break;

    switch (n) {
    case -2:
      UThalt(CAMF_Usage, PROGRAM);
      break;
    case 0:
      /* Filename argument */
      ++nF;
      if (nF > 2)
        UThalt("%s: %s", PROGRAM, CAM_XFName);
      STcopyMax(OptArg, FIx.Fname, FILENAME_MAX-1);
      FI[nF-1] = FIx;
      break;
    case 1:
    case 2:
      /* Delay specification */
      if (STdecLlimits(OptArg, Delay, 1) || Delay[0] > Delay[1])
        ERRSTOP(CAM_BadDelay, OptArg);
      break;
    case 3:
    case 4:
      /* Limits specification */
      /* Reset the default limits - the option may be called more than once */
      FIx.Lim[0] = 0;
      FIx.Lim[1] = AO_LIM_UNDEF;
      if (STdecLlimits(OptArg, FIx.Lim, 2) ||
          (FIx.Lim[1] != AO_LIM_UNDEF && FIx.Lim[0] > FIx.Lim[1]))
        ERRSTOP(CAM_BadLimits, OptArg);
      break;
    default:
      assert(0);
      break;
    }
  }

/* Error checks */
  if (nF == 0)
    UThalt("%s: %s", PROGRAM, CAM_NoFName);
  if (FIParSet >= nF)
    UThalt("%s: %s", PROGRAM, CAM_LateFPar);

/* Check for too many stdin specs */
  AOstdin(FI, nF);

/* Return values */
  if (nF == 1)
    FI[1].Fname[0] = '\0';
}
