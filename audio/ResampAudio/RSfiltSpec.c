/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void RSfiltSpec (const char String[], struct Fspec_T *Fspec)

Purpose:
  Decode interpolation filter specifications

Description:
  This routine sets interpolation filter specifications from values specified
  in an input string.

Parameters:
   -> const char String[]
      String containing the list of filter specifications
  <-> struct Fspec_T *Fspec
      Structure with the decoded filter specifications

Author / revision:
  P. Kabal  Copyright (C) 2009
  $Revision: 1.14 $  $Date: 2009/03/09 18:44:30 $

-------------------------------------------------------------------------*/

#include <math.h>
#include <string.h>

#include <libtsp.h>
#include "ResampAudio.h"

#define WS_STRIP	1

#define ERRSTOP(text,par)	UThalt ("%s: %s: \"%s\"", PROGRAM, text, par)

static const char *keytable [] = {
  "file",
  "write",
  "ratio",
  "del*ay",
  "g*ain",
  "cut*off",
  "atten*uation",
  "alpha",
  "N*cof",
  "span",
  "offset",
  NULL
};


void
RSfiltSpec (const char String[], struct Fspec_T *Fspec)

{
  int ind, nt, nc;
  const char *p;
  char *token;
  double DD, DN, atten;

/* Allocate temporary storage */
  nt = strlen (String);
  token = (char *) UTmalloc (nt + 1);

/* Separate the parameters */
  p = String;
  while (p != NULL) {
    p = STfindToken (p, ",", "\"\"", token, WS_STRIP, nt);
    if (token[0] != '\0') {

      /* Decode the parameter values */
      ind = STkeyXpar (token, "=", "\"\"", keytable, token);
      if (ind < 0)
	ERRSTOP (RSM_BadKey, token);

      switch (ind) {

      /* file = */
      case 0:
	UTfree ((void *) Fspec->FFile);
	nc = strlen (token);
	if (nc <= 0)
	  UThalt ("%s: %s", PROGRAM, RSM_NoCoef);
	Fspec->FFile = (char *) UTmalloc (nc + 1);
	STcopyMax (token, Fspec->FFile, nc);
	break;

      /* write = */
      case 1:
	UTfree ((void *) Fspec->WFile);
	nc = strlen (token);
	if (nc <= 0)
	  UThalt ("%s: %s", PROGRAM, RSM_NoFName);
	Fspec->WFile = (char *) UTmalloc (nc + 1);
	STcopyMax (token, Fspec->WFile, nc);
	break;

      /* ratio = */
      case 2:
	if (STdec1int (token, &Fspec->Ir) || Fspec->Ir <= 0)
	  ERRSTOP (RSM_BadInterp, token);
	break;

      /* delay = */
      case 3:
	if (STdecDfrac (token, &DN, &DD))
	  ERRSTOP (RSM_BadDelay, token);
        Fspec->Del = DN / DD;
	break;

      /* gain = */
      case 4:
	if (STdecDfrac (token, &DN, &DD))
	  ERRSTOP (RSM_BadFGain, token);
        Fspec->Gain = DN / DD;
	break;

      /* cutoff = */
      case 5:
	if (STdec1double (token, &Fspec->Fc) || Fspec->Fc <= 0.0
	    || Fspec->Fc > 0.5)
	  ERRSTOP (RSM_BadFCutoff, token);
	break;

      /* attenuation = */
      case 6:
	if (STdec1double (token, &atten) || atten < 21.0)
	  ERRSTOP (RSM_BadAtten, token);
	Fspec->alpha = RSKattenXalpha (atten);
	break;

      /*  alpha = */
      case 7:
	if (STdec1double (token, &Fspec->alpha) || Fspec->alpha < 0.0)
	  ERRSTOP (RSM_BadWinPar, token);
	break;

      /* N = */
      case 8:
	if (STdec1int (token, &Fspec->Ncof) || Fspec->Ncof <= 0)
	  ERRSTOP (RSM_BadNCoef, token);
	break;

      /* span = */
      case 9:
	if (STdec1double (token, &Fspec->Wspan) || Fspec->Wspan <= 0.0)
	  ERRSTOP (RSM_BadWinSpan, token);
	break;

      /* offset = */
      case 10:
	if (STdec1double (token, &Fspec->Woffs))
	  ERRSTOP (RSM_BadWinOffs, token);
	break;

      }
    }
  }
  UTfree ((void *) token);

  return;
}
