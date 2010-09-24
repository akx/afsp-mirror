/*------------- Telecommunications & Signal Processing Lab -------------
                           McGill University

Routine:
  void PQoptions (int argc, const char *argv[], struct PQ_Opt *Opt,
                  struct PQ_FIpar FI[2])

Purpose:
  Decode options for PQevalAudio

Description:
  This routine decodes options for PQevalAudio.

Parameters:
   -> int argc
      Number of command line arguments
   -> const char *argv[]
      Array of pointers to argument strings
  <-  struct PQ_Opt *Opt
      Processing options
  <-  struct PQ_FIpar FI[2]
      Input file parameters

Author / revision:
  P. Kabal  Copyright (C) 2009
  $Revision: 1.10 $  $Date: 2009/03/14 22:38:47 $

----------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>

#include <libtsp.h>
#include <AO.h>
#include "PQevalAudio.h"

#define ERRSTOP(text,par)	UThalt ("%s: %s: \"%s\"", PROGRAM, text, par)

static void
PQ_decOpt (const char String[], struct PQ_Opt *PQopt);

/* Option table */
static const char *OptTable[] = {
  "-L#", "--lev*elSPL=",
  "-i#", "--i*nfo=",
  "-o#", "--opt*ions=",
  NULL
};


void
PQoptions (int argc, const char *argv[], struct PQ_Opt *PQopt,
	   struct PQ_FIpar FI[2])

{
  struct PQ_FIpar FIx;
  int n, nF, FIParSet;
  const char *OptArg;

/* Input file defaults */
  FIParSet = 0;
  FIpar_INIT (&FIx);

/* Option defaults */
  OPTIONS_INIT (PQopt);

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
    n = AOdecHelp (VERSION, PQMF_Usage);
    if (n >= 1)
      continue;

    /* Decode program options */
    n = AOdecOpt (OptTable, &OptArg);
    if (n == -1)
      break;

    switch (n) {
    case -2:
      UThalt (PQMF_Usage, PROGRAM);
      break;
    case 0:
      /* Filename argument */
      ++nF;
      if (nF > 2)
	UThalt ("%s: %s", PROGRAM, PQM_XFName);
      STcopyMax (OptArg, FIx.Fname, FILENAME_MAX-1);
      FI[nF-1] = FIx;
      break;
    case 1:
    case 2:
      /* Listening level */
      if (STdec1double (OptArg, &PQopt->Lp))
	ERRSTOP (PQM_BadLevel, OptArg);
      break;
    case 3:
    case 4:
      /* Frame info */
      if (STdec1int (OptArg, &PQopt->Ni))
	ERRSTOP (PQM_BadInfo, OptArg);
      break;
    case 5:
    case 6:
      /* Options */
      PQ_decOpt (OptArg, PQopt);
      break;
    default:
      assert (0);
      break;
    }
  }

/* Error checks */
  if (nF == 0)
    UThalt ("%s: %s", PROGRAM, PQM_NoFName);
  if (FIParSet >= nF)
    UThalt ("%s: %s", PROGRAM, PQM_LateFPar);

/* Check for too many stdin specs */
  AOstdin (FI, nF);

/* Return values */
  if (nF == 1)
    FI[1].Fname[0] = '\0';

  return;
}
 
/* Decode options keywords */

#define WS_STRIP	1


static void
PQ_decOpt (const char String[], struct PQ_Opt *PQopt)

{
  int ind, nt;
  const char *p;
  char *token;

  static const char *keytable [] = {
    "clip*_MOV",
    "no_clip*_MOV",
    "PC*_init",
    "PD*_factor",
    "overlap*_delay",
    "no_overlap*_delay",
    "data*_bounds",
    "no_data*_bounds",
    "end*_min",
    NULL
  };

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
	ERRSTOP (PQM_BadKey, token);

      switch (ind) {

      /* clip_MOV */
      case 0:
	PQopt->ClipMOV = 1;
	break;

      /* no_clip_MOV */
      case 1:
	PQopt->ClipMOV = 0;
	break;

      /* PC_init = */
      case 2:
	if (STdec1double (token, &PQopt->PCinit))
	  ERRSTOP (PQM_BadPCinit, token);
	break;

      /*  PD_factor = */
      case 3:
	if (STdec1double (token, &PQopt->PDfactor))
	  ERRSTOP (PQM_BadPDfactor, token);
	break;

      /* overlap_delay */
      case 4:
	PQopt->OverlapDelay = 1;
	break;

      /* no_overlap_delay */
      case 5:
	PQopt->OverlapDelay = 0;
	break;

      /* data_bounds */
      case 6:
	PQopt->DataBounds = 1;
	break;

      /* no_data_bounds */
      case 7:
	PQopt->DataBounds = 0;
	break;

      /* end_min */
      case 8:
	if (STdec1int (token, &PQopt->EndMin) ||
	    PQopt->EndMin <=0 || PQopt->EndMin > PQ_NF)
	  ERRSTOP (PQM_BadEndMin, token);
	break;
      }
    }
  }
  UTfree ((void *) token);

  return;
}
