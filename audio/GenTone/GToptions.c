/*------------- Telecommunications & Signal Processing Lab -------------
                           McGill University

Routine:
  void GToptions (int argc, const char *argv[], struct FT_Sine *Sine,
                  struct GT_FOpar *FO)

Purpose:
  Decode options for GenTone

Description:
  This routine decodes options for GenTone.

Parameters:
   -> int argc
      Number of command line arguments
   -> const char *argv[]
      Array of pointers to argument strings
  <-  struct GT_Sine *Sine
      Sine wave parameters
  <-  struct GT_FOpar *FO
      Output file parameters

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.11 $  $Date: 2003/11/06 13:25:58 $

----------------------------------------------------------------------*/

#include <assert.h>
#include <math.h>

#include <libtsp.h>
#include <libtsp/AFpar.h>
#include <AO.h>
#include "GenTone.h"

#define PI	3.14159265358979323846
#define DEG_RAD		(PI / 180.0)

#define ERRSTOP(text,par)	UThalt ("%s: %s: \"%s\"", PROGRAM, text, par)

/* Option table */
static const char *OptTable[] = {
  "-f#", "--freq*uency=",
  "-r#", "--rms=",
  "-a#", "--amp*litude=",
  "-p#", "--phase=",
  NULL
};


void
GToptions (int argc, const char *argv[], struct GT_Sine *Sine,
	   struct GT_FOpar *FO)

{
  const char *OptArg;
  int nF, n;
  double Nv, Dv;

/* Output file defaults */
  FOpar_INIT (FO);
  FO->Sfreq = AF_SFREQ_DEFAULT;

/* Defaults */
  Sine->Freq = AF_SFREQ_DEFAULT / 8.0;
  Sine->Ampl = AMPL_DEF;

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
    n = AOdecHelp (VERSION, GTMF_Usage);
    if (n >= 1)
      continue;

    /* Decode program options */
    n = AOdecOpt (OptTable, &OptArg);
    if (n == -1)
      break;

    switch (n) {
    case -2:
      UThalt (GTMF_Usage, PROGRAM);
      break;
    case 0:
      /* Filename argument */
      ++nF;
      if (nF > 1)
	UThalt ("%s: %s", PROGRAM, GTM_XFName);
      STcopyMax (OptArg, FO->Fname, FILENAME_MAX-1);
      break;
    case 1:
    case 2:
      /* frequency */
      if (STdecDfrac (OptArg, &Nv, &Dv) || Nv / Dv < 0.0)
	ERRSTOP (GTM_BadFreq, OptArg);
      Sine->Freq = Nv / Dv;
      break;
    case 3:
    case 4:
      /* rms value */
      if (STdecDfrac (OptArg, &Nv, &Dv) || Nv / Dv < 0.0)
	ERRSTOP (GTM_BadRMS, OptArg);
      Sine->Ampl = SQRT2 * Nv / Dv;
      break;
    case 5:
    case 6:
      /* amplitude */
      if (STdecDfrac (OptArg, &Nv, &Dv) || Nv / Dv < 0.0)
	ERRSTOP (GTM_BadAmpl, OptArg);
      Sine->Ampl = Nv / Dv;
      break;
    case 7:
    case 8:
      /* phase */
      if (STdecDfrac (OptArg, &Nv, &Dv))
	ERRSTOP (GTM_BadPhase, OptArg);
      Sine->Phase =  DEG_RAD * Nv / Dv;	/* Radians */
      break;
    default:
      assert (0);
      break;
    }
  }

/* Checks */
  if (nF < 1)
    UThalt ("%s: %s", PROGRAM, GTM_NoFName);
  if (FO->Nframe == AF_NFRAME_UNDEF)
    UThalt ("%s: %s", PROGRAM, GTM_NoNsamp);

  return;
}
