/*------------- Telecommunications & Signal Processing Lab -------------
                           McGill University

Routine:
  void IAoptions (int argc, const char *argv[], int *Icode,
                  struct IA_FIpar FI[MAXFILE], int *Nfiles)

Purpose:
  Decode options for InfoAudio

Description:
  This routine decodes options for InfoAudio.

Parameters:
   -> int argc
      Number of command line arguments
   -> const char *argv[]
      Array of pointers to argument strings
  <-  int *Icode
      Flag to select the amount of output, default 7
  <-  struct IA_FIpar FI[MAXFILE]
      Input file parameters
  <-  int *Nfiles
      Number of input file names

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.31 $  $Date: 2003/05/13 01:19:47 $

----------------------------------------------------------------------*/

#include <assert.h>

#include <libtsp.h>
#include <AO.h>
#include "InfoAudio.h"

#define ERRSTOP(text,par)	UThalt ("%s: %s: \"%s\"", PROGRAM, text, par)

/* Option table  */
static const char *OptTable[] = {
  "-i#", "--info_code=",
  NULL
};


void
IAoptions (int argc, const char *argv[], int *Icode,
	   struct IA_FIpar FI[MAXFILE], int *Nfiles)

{
  struct IA_FIpar FIx;
  int n, nF, icode, FIParSet;
  const char *OptArg;

/* Default values */
  FIParSet = 0;
  FIpar_INIT (&FIx);

  icode = 7;

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
    n = AOdecHelp (VERSION, IAMF_Usage);
    if (n >= 1)
      continue;

    /* Decode program options */
    n = AOdecOpt (OptTable, &OptArg);
    if (n == -1)
      break;

    switch (n) {
    case -2:
      UThalt (IAMF_Usage, PROGRAM);
      break;
    case 0:
      /* Filename argument */
      ++nF;
      if (nF > MAXFILE)
	UThalt ("%s: %s", PROGRAM, IAM_XFName);
      STcopyMax (OptArg, FIx.Fname, FILENAME_MAX-1);
      FI[nF-1] = FIx;
      break;
    case 1:
    case 2:
      /* Info code parameter */
      if (STdec1int (OptArg, &icode) || icode < 0 || icode > 7)
	ERRSTOP (IAM_BadInfoCode, OptArg);
      break;
    default:
      assert (0);
      break;
    }
  }

/* Error checks */
  if (nF <= 0)
    UThalt ("%s: %s", PROGRAM, IAM_NoFName);
  if (FIParSet >= nF)
    UThalt ("%s: %s", PROGRAM, IAM_LateFPar);

  /* Check for too many stdin specs */
  AOstdin (FI, nF);

/* Return parameter */
  *Icode = icode;
  *Nfiles = nF;

  return;
}
