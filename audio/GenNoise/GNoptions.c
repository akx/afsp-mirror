/*------------- Telecommunications & Signal Processing Lab -------------
                           McGill University

Routine:
  void GNoptions (int argc, const char *argv[], float *rms, int *seed,
                  struct GN_FOpar *FO)

Purpose:
  Decode options for GenNoise

Description:
  This routine decodes options for GenNoise.

Parameters:
   -> int argc
      Number of command line arguments
   -> const char *argv[]
      Array of pointers to argument strings
  <-  float *rms
      Standard deviation for the noise samples, default 1000
  <-  int *seed
      Seed for the random number generator, default 0
  <-  struct GN_FOpar *FO
      Output file parameters

Author / revision:
  P. Kabal  Copyright (C) 1999
  $Revision: 1.33 $  $Date: 1999/06/05 01:30:21 $

----------------------------------------------------------------------*/

static char rcsid[] = "$Id: GNoptions.c 1.33 1999/06/05 AFsp-v6r8 $";

#include <assert.h>

#include <libtsp.h>
#include <libtsp/AFpar.h>
#include <AO.h>
#include "GenNoise.h"

#define ERRSTOP(text,par)	UThalt ("%s: %s: \"%s\"", PROGRAM, text, par)

/* Option table */
static const char *OptTable[] = {
  "-d#", "--st*d_deviation=",
  "-x#", "--se*ed=",
  NULL
};


void
GNoptions (int argc, const char *argv[], float *rms, int *seed,
	   struct GN_FOpar *FO)

{
  const char *OptArg;
  int nF, n;
  float rmsx;
  int seedx;

/* Output file defaults */
  FOpar_INIT (FO);
  FO->Sfreq = AF_SFREQ_DEFAULT;

/* Defaults */
  rmsx = 1000.0;
  seedx = 0;

/* Initialization */
  UTsetProg (PROGRAM);
  nF = 0;

/* Decode options */
  AOinitOpt (argc, argv);
  while (1) {

    /* Decode output file options */
    n = AOdecFO (FO);
    if (n >= 1)
      continue;

    /* Decode help options */
    n = AOdecHelp (VERSION, GNMF_Usage);
    if (n >= 1)
      continue;

    /* Decode program options */
    n = AOdecOpt (OptTable, &OptArg);
    if (n == -1)
      break;

    switch (n) {
    case -2:
      UThalt (GNMF_Usage, PROGRAM);
      break;
    case 0:
      /* Filename argument */
      ++nF;
      if (nF > 1)
	UThalt ("%s: %s", PROGRAM, GNM_XFName);
      STcopyMax (OptArg, FO->Fname, FILENAME_MAX-1);
      break;
    case 1:
    case 2:
      /* Standard deviation */
      if (STdec1float (OptArg, &rmsx) || rmsx < 0.0)
	ERRSTOP (GNM_BadStdDev, OptArg);
      break;
    case 3:
    case 4:
      /* Seed */
      if (STdec1int (OptArg, &seedx) || seedx < 0)
	ERRSTOP (GNM_BadSeed, OptArg);
      break;
    default:
      assert (0);
      break;
    }
  }

/* Checks */
  if (nF < 1)
    UThalt ("%s: %s", PROGRAM, GNM_NoFName);
  if (FO->Nframe == AF_NFRAME_UNDEF)
    UThalt ("%s: %s", PROGRAM, GNM_NoNsamp);

/* Set return values */
  *rms = rmsx;
  *seed = seedx;

  return;
}
