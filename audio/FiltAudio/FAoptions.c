/*------------- Telecommunications & Signal Processing Lab -------------
                           McGill University

Routine:
  void FAoptions (int argc, const char *argv[], struct FA_FIpar *FI,
                  struct FA_FFpar *FF, struct FA_FOpar *FO)

Purpose:
  Decode options for FiltAudio

Description:
  This routine decodes options for FiltAudio.

Parameters:
   -> int argc
      Number of command line arguments
   -> const char *argv[]
      Array of pointers to argument strings
  <-  struct FA_FIpar *FI
      Input file parameters
  <-  struct FA_FFpar *FF
      Filter file parameters
  <-  struct FA_FOpar *FO
      Output file parameters

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.48 $  $Date: 2003/05/13 01:25:28 $

----------------------------------------------------------------------*/

#include <assert.h>

#include <libtsp.h>
#include <AO.h>
#include "FiltAudio.h"

#define ERRSTOP(text,par)	UThalt ("%s: %s: \"%s\"", PROGRAM, text, par)

/* Option table */
static const char *OptTable[] = {
  "-f#", "--f*ilter_file=",
  "-i#", "--int*erpolate=",
  "-a#", "--a*lignment=",
  NULL
};


void
FAoptions (int argc, const char *argv[], struct FA_FIpar *FI,
	   struct FA_FFpar *FF, struct FA_FOpar *FO)

{
  const char *OptArg;
  int nF, n;

/* Input file defaults */
  FIpar_INIT (FI);

/* Output file defaults */
  FOpar_INIT (FO);

/* Filter defaults */
  FFpar_INIT (FF);

/* Initialization */
  UTsetProg (PROGRAM);
  nF = 0;

/* Decode options */
  AOinitOpt (argc, argv);
  while (1) {

    /* Decode input file options */
    n = AOdecFI (FI);
    if (n >= 1)
      continue;

    /* Decode output file options */
    n = AOdecFO (FO);
    if (n >= 1)
      continue;

    /* Decode help options */
    n = AOdecHelp (VERSION, FAMF_Usage);
    if (n >= 1)
      continue;

    /* Decode program options */
    n = AOdecOpt (OptTable, &OptArg);
    if (n == -1)
      break;

    switch (n) {
    case -2:
      UThalt (FAMF_Usage, PROGRAM);
      break;

    case 0:
      /* Filename argument */
      ++nF;
      if (nF == 1)
	STcopyMax (OptArg, FI->Fname, FILENAME_MAX-1);
      else if (nF == 2)
	STcopyMax (OptArg, FO->Fname, FILENAME_MAX-1);
      else
	UThalt ("%s: %s", PROGRAM, FAM_XFName);
      break;
    case 1:
    case 2:
      /* Filter file */
      STcopyMax (OptArg, FF->Fname, FILENAME_MAX-1);
      break;
    case 3:
    case 4:
      /* Sampling rate ratio */
      if (STdecIfrac (OptArg, &(FF->Ir), &(FF->Nsub)) ||
	  FF->Ir <= 0 || FF->Nsub <= 0)
	ERRSTOP (FAM_BadRatio, OptArg);
      break;
    case 5:
    case 6:
      /* Alignment */
      if (STdec1long (OptArg, &FF->Doffs))
	ERRSTOP (FAM_BadAlign, OptArg);
      break;
    default:
      assert (0);
      break;
    }
  }

/* Checks, add defaults */
  if (nF < 2)
    UThalt ("%s: %s", PROGRAM, FAM_MFName);
  if (FF->Fname[0] == '\0')
    UThalt ("%s: %s", PROGRAM, FAM_NoFFile);

  return;
}
