/*------------- Telecommunications & Signal Processing Lab -------------
                           McGill University

Routine:
  void CAoptions (int argc, const char *argv[], int *delayL, int *delayU,
	          long int *Nsseg, struct CA_FIpar FI[2])

Purpose:
  Decode options for CompAudio

Description:
  This routine decodes options for CompAudio.

Parameters:
   -> int argc
      Number of command line arguments
   -> const char *argv[]
      Array of pointers to argument strings
  <-  int *delayL
      Starting delay
  <-  int *delayU
      End delay
  <-  long int *Nsseg
      Number of samples per segment (zero if unspecified)
  <-  struct CA_FIpar FI[2]
      Input file parameters

Author / revision:
  P. Kabal  Copyright (C) 1999
  $Revision: 1.45 $  $Date: 1999/06/05 00:54:10 $

----------------------------------------------------------------------*/

static char rcsid[] = "$Id: CAoptions.c 1.45 1999/06/05 AFsp-v6r8 $";

#include <assert.h>

#include <libtsp.h>
#include <AO.h>
#include "CompAudio.h"

#define ERRSTOP(text,par)	UThalt ("%s: %s: \"%s\"", PROGRAM, text, par)

/* Option table */
static const char *OptTable[] = {
  "-d#", "--d*elay=",
  "-s#", "--s*egment=",
  NULL
};


void
CAoptions (int argc, const char *argv[], int *delayL, int *delayU,
	   long int *Nsseg, struct CA_FIpar FI[2])

{
  struct CA_FIpar FIx;
  int n, nF, delL, delU, FIParSet;
  const char *OptArg;
  long int nsseg;

/* Input file defaults */
  FIParSet = 0;
  FIpar_INIT (&FIx);

  delL = 0;
  delU = 0;
  nsseg = 0;

/* Initialization */
  UTsetProg (PROGRAM);
  nF = 0;

/* Decode options */
  AOinitOpt (argc, argv);
  while (1) {

    /* Decode input file options */
    n = AOdecFI (&FIx);
    if (n >= 1) {
      FIParSet = nF;
      continue;
    }

    /* Decode help options */
    n = AOdecHelp (VERSION, CAMF_Usage);
    if (n >= 1)
      continue;

    /* Decode program options */
    n = AOdecOpt (OptTable, &OptArg);
    if (n == -1)
      break;

    switch (n) {
    case -2:
      UThalt (CAMF_Usage, PROGRAM);
      break;
    case 0:
      /* Filename argument */
      ++nF;
      if (nF > 2)
	UThalt ("%s: %s", PROGRAM, CAM_XFName);
      STcopyMax (OptArg, FIx.Fname, FILENAME_MAX-1);
      FI[nF-1] = FIx;
      break;
    case 1:
    case 2:
      /* Delay specification */
      if (STdecIrange (OptArg, &delL, &delU) || delL > delU)
	ERRSTOP (CAM_BadDelay, OptArg);
      break;
    case 3:
    case 4:
      /* Segment length */
      if (STdec1long (OptArg, &nsseg) || nsseg < 0)
	ERRSTOP (CAM_BadSegLen, OptArg);
      break;
    default:
      assert (0);
      break;
    }
  }

/* Error checks */
  if (nF == 0)
    UThalt ("%s: %s", PROGRAM, CAM_NoFName);
  if (FIParSet >= nF)
    UThalt ("%s: %s", PROGRAM, CAM_LateFPar);

/* Check for too many stdin specs */
  AOstdin (FI, nF);

/* Return values */
  if (nF == 1)
    FI[1].Fname[0] = '\0';
  *delayL = delL;
  *delayU = delU;
  *Nsseg = nsseg;

  return;
}
