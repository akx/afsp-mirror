/*------------- Telecommunications & Signal Processing Lab -------------
                           McGill University

Routine:
  void LSoptions (int argc, const char *argv[], char Fname[4][FILENAME_MAX])

Purpose:
  Decode options for LPsyn

Description:
  This routine decodes options for LPsyn.

Parameters:
   -> int argc
      Number of command line arguments
   -> const char *argv[]
      Array of pointers to argument strings
  <-  const char *Fname[4]
      File names: input parameter file,
                  input audio file,
                  LPC file,
                  output audio file

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.2 $  $Date: 2003/05/13 01:18:00 $

----------------------------------------------------------------------*/

#include <stdlib.h>	/* prototype for exit */

#include <libtsp.h>
#include <libtsp/AFpar.h>

#include "LPsyn.h"

#ifndef EXIT_SUCCESS
#  define EXIT_SUCCESS	0	/* Normally in stdlib.h */
#endif

#define NELEM(array)	((sizeof array) / (sizeof array[0]))

/* Option tables and usage message */
#define LOPT	(NELEM (OptTable) / 2)
static const char *nullTable[] = { NULL };
static const char *OptTable[] = {
  "-p#", "--par*ameter_file=",
  "-c#", "--lpc*_file=",
  "-h",  "--h*elp",
  "-v",  "--v*ersion",
  "--",
  NULL
};
static const char Usage[] = "\
Usage: %s [options] AFileI [AFileO]\n\
Options:\n\
  -p Pfile, --parameter_file=Pfile,\n\
                              Parameter file.\n\
  -c LPFile, --lpc_file=LPFile\n\
                              LPC predictor coefficient file.\n\
  -h, --help                  Print a list of options and exit.\n\
  -v, --version               Print the version number and exit.";

void
LSoptions (int argc, const char *argv[], char Fname[4][FILENAME_MAX])

{
  int Index;
  const char *OptArg;
  const char **optt;

  int nF, n, nn;

/* Defaults */
  Fname[2][0] = '\0';
  Fname[3][0] = '\0';

/* Initialization */
  UTsetProg (PROGRAM);
  nF = 0;

/* Decode options */
  Index = 1;
  optt = OptTable;
  while (Index < argc) {
    n = UTgetOption (&Index, argc, argv, optt, &OptArg);
    nn = ((n + 3) / 2) - 1;	/* n = -2 ==> nn = -1 */
    switch (nn) {
    case 0:
      /* Filename argument */
      ++nF;
      if (nF == 1)
	STcopyMax (OptArg, Fname[1], FILENAME_MAX-1);
      else if (nF == 2)
	STcopyMax (OptArg, Fname[3], FILENAME_MAX-1);
      else
	UThalt ("%s: Too many filenames specified", PROGRAM);
      break;
    case 1:
      /* Parameter file */
      STcopyMax (OptArg, Fname[0], FILENAME_MAX-1);
      break;
    case 2:
      /* LPC file */
      STcopyMax (OptArg, Fname[2], FILENAME_MAX-1);
      break;
    case LOPT-2:
      /* Help */
      UTwarn (Usage, PROGRAM);
      exit (EXIT_SUCCESS);
      break;
    case LOPT-1:
      /* Version */
      printf ("%s: %s\n", PROGRAM, VERSION);
      exit (EXIT_SUCCESS);
      break;
    case LOPT:
      /* Stop interpreting options */
      optt = nullTable;
      break;
    default:
      /* Option error */
      UThalt (Usage, PROGRAM);
      break;
    }
  }

/* Checks */
  if (nF < 1)
    UThalt ("%s: No input audio file", PROGRAM);
  if (Fname[0][0] == '\0')
    UThalt ("%s: No parameter file", PROGRAM);
  if (Fname[2][0] == '\0')
    UThalt ("%s: No LPC coefficient file", PROGRAM);

  return;
}
