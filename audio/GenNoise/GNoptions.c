/*------------- Telecommunications & Signal Processing Lab -------------
                           McGill University

Routine:
  void GNoptions (int argc, const char *argv[], double *rms, int *seed,
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
  <-  double *rms
      Standard deviation for the noise samples, default 0.03
  <-  int *seed
      Seed for the random number generator, default 0
  <-  struct GN_FOpar *FO
      Output file parameters

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.35 $  $Date: 2003/11/06 13:24:09 $

----------------------------------------------------------------------*/

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
GNoptions (int argc, const char *argv[], double *rms, int *seed,
	   struct GN_FOpar *FO)

{
  const char *OptArg;
  int nF, n;
  double rmsx, Nv, Dv;
  int seedx;

/* Output file defaults */
  FOpar_INIT (FO);
  FO->Sfreq = AF_SFREQ_DEFAULT;

/* Defaults */
  rmsx = RMS_DEFAULT;
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
      if (STdecDfrac (OptArg, &Nv, &Dv) || Nv / Dv < 0.0)
	ERRSTOP (GNM_BadStdDev, OptArg);
      rmsx = Nv / Dv;
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
