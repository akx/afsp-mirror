/*------------- Telecommunications & Signal Processing Lab -------------
                           McGill University

Routine:
  void RSoptions (int argc, const char *argv[], struct FA_FIpar *FI,
                  double *Soffs, double *Sratio, struct Fspec_T *Fspec,
		  struct FA_FOpar *FO)

Purpose:
  Decode options for ResampAudio

Description:
  This routine decodes options for ResampAudio.

Parameters:
   -> int argc
      Number of command line arguments
   -> const char *argv[]
      Array of pointers to argument strings
  <-  struct RS_FIpar *FI
      Input file parameters
  <-  double *Soffs
      Sampel offset
  <-  double *Sratio
      Sampling rate ratio
  <-  struct Fspec_T *Fspec
      Filter specification structure
  <-  struct RS_FOpar *FO
      Output file parameters

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.19 $  $Date: 2003/05/13 01:09:07 $

----------------------------------------------------------------------*/

#include <assert.h>

#include <libtsp.h>
#include <AO.h>
#include "ResampAudio.h"

#define ERRSTOP(text,par)	UThalt ("%s: %s: \"%s\"", PROGRAM, text, par)

/* Option table */
static const char *OptTable[] = {
  "-i#", "--int*erpolate=",
  "-a#", "--a*lignment=",
  "-f#", "--f*ilter_spec=",
  NULL
};


void
RSoptions (int argc, const char *argv[], struct RS_FIpar *FI, double *Soffs,
	   double *Sratio, struct Fspec_T *Fspec, struct RS_FOpar *FO)

{
  const char *OptArg;
  int n, nF;
  double Nv, Dv, sratio, soffs;

/* Input file defaults */
  FIpar_INIT (FI);

/* Output file defaults */
  FOpar_INIT (FO);

/* Filter defaults */
  sratio = -1.0;
  soffs = 0.0;
  Fspec_T_INIT (Fspec);

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
    n = AOdecHelp (VERSION, RSMF_Usage);
    if (n >= 1)
      continue;

    /* Decode program options */
    n = AOdecOpt (OptTable, &OptArg);
    if (n == -1)
      break;

    switch (n) {
    case -2:
      UThalt (RSMF_Usage, PROGRAM);
      break;
    case 0:
      /* Filename argument */
      ++nF;
      if (nF == 1)
	STcopyMax (OptArg, FI->Fname, FILENAME_MAX-1);
      else if (nF == 2)
	STcopyMax (OptArg, FO->Fname, FILENAME_MAX-1);
      else
	UThalt ("%s: %s", PROGRAM, RSM_XFName);
      break;
    case 1:
    case 2:
      /* Sampling rate ratio */
      if (STdecDfrac (OptArg, &Nv, &Dv) || Nv <= 0 || Dv <= 0)
	ERRSTOP (RSM_BadRatio, OptArg);
      sratio = Nv / Dv;
      break;
    case 3:
    case 4:
      /* Alignment */
      if (STdecDfrac (OptArg, &Nv, &Dv))
	ERRSTOP (RSM_BadAlign, OptArg);
      soffs = Nv / Dv;
      break;
    case 5:
    case 6:
      /* Filter specs */
      RSfiltSpec (OptArg, Fspec);
      break;
    default:
      assert (0);
      break;
     }
  }

/* Checks, add defaults */
  if (nF < 2)
    UThalt ("%s: %s", PROGRAM, RSM_MFName);
  if (FO->Sfreq <= 0.0 && sratio <= 0.0)
    UThalt ("%s: %s", PROGRAM, RSM_NoSFreq);
  if (FO->Sfreq > 0.0 && sratio > 0.0)
    UThalt ("%s: %s", PROGRAM, RSM_SFreqRatio);

/* Set return values */
  *Soffs = soffs;
  *Sratio = sratio;

  return;
}
