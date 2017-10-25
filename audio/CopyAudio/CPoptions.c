/*------------- Telecommunications & Signal Processing Lab -------------
                           McGill University

Routine:
  void CPoptions (int argc, const char *argv[], int *Mode,
                  struct CP_FIpar FI[MAXIFILE], int *Nifiles,
                  struct CP_FOpar *FO)

Purpose:
  Decode options for CopyAudio

Description:
  This routine decodes options for CopyAudio.

Parameters:
   -> int argc
      Number of command line arguments
   -> const char *argv[]
      Array of pointers to argument strings
  <-  int *Mode
      Combine / concatenate mode
  <-  struct CP_FIpar FI[MAXIFILES]
      Input file parameters
  <-  int *Nifiles
      Number of input file names
  <-  struct CP_FOpar *FO
      Output file parameters
  <-  struct CP_Chgain *Chgain
      Channel gain matrix

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.83 $  $Date: 2017/09/15 00:37:53 $

----------------------------------------------------------------------*/

#include "CopyAudio.h"

/* Option table */
static const char *OptTable[] = {
  "-c", "--com*bine",
  "-C", "--con*catenate",
  "-cA#", "--chanA=",
  "-cB#", "--chanB=",
  "-cC#", "--chanC=",
  "-cD#", "--chanD=",
  "-cE#", "--chanE=",
  "-cF#", "--chanF=",
  "-cG#", "--chanG=",
  "-cH#", "--chanH=",
  "-cI#", "--chanI=",
  "-cJ#", "--chanJ=",
  "-cK#", "--chanK=",
  "-cL#", "--chanL=",
  NULL
};

static const char Ch[MAXNCO+1] = "ABCDEFGHIJKL";


void
CPoptions (int argc, const char *argv[], int *Mode,
           struct CP_FIpar FI[MAXIFILE], int *Nifiles, struct CP_FOpar *FO,
           struct CP_Chgain *Chgain)

{
  const char *OptArg;
  struct CP_FIpar FIx;
  int nF, i, n, mode, FIParSet;
  int Chset[MAXNCO];

/* Defaults */
  mode = M_COMB;

/* Input file defaults */
  FIParSet = 0;
  FIpar_INIT (&FIx);      /* Temp input file parameter structure */

/* Output file defaults */
  FOpar_INIT (FO);

/* Initialize the channel gain structure */
/* If the number of output channels in Chgain is zero, it is assumed that the
   number of output channels are mapped one-to-one to the input channels */
  Chgain_INIT (Chgain); /* Channels gains and offsets set to zero */
  VRiZero (Chset, MAXNCO);

/* Initialization */
  UTsetProg (PROGRAM);
  nF = 0;

/* Decode options */
/* For each argument try the following:
   - decode an input file option
   - if not successful, decode an output file option
   - if not successful, decode a help option
   - if not successful, decode a file name or a program option
*/
  AOinitOpt (argc, argv);
  while (1) {

    /* Decode input file options */
    n = AOdecFI (&FIx);
    if (n >= 1) {
      FIParSet = nF;    /* Flag, found input parameters FI[nF] */
      continue;
    }
    /* Decode output file options */
    n = AOdecFO (FO);
    if (n >= 1)
      continue;

    /* Decode help options */
    n = AOdecHelp (VERSION, CPMF_Usage);
    if (n >= 1)
      continue;

    /* Decode program options */
    n = AOdecOpt (OptTable, &OptArg);
    if (n == -1)
      break;

    switch (n) {
    case -2:
      UThalt (CPMF_Usage, PROGRAM);
      break;
    case 0:
      /* Filename argument */
      /* Filename tentatively assigned to both the output file and to input file
         nF-1.
         - The last file name will be the output file name
         - The other file names will be input file names (0 to nF-2)
       */
      ++nF;
      if (nF > MAXIFILE+1)
        UThalt ("%s: %s", PROGRAM, CPM_XFName);
      STcopyMax (OptArg, FO->Fname, FILENAME_MAX-1);
      STcopyMax (OptArg, FIx.Fname, FILENAME_MAX-1);
      if (nF <= MAXIFILE)
        FI[nF-1] = FIx;     /* Save input parameters */
      break;
    case 1:
    case 2:
      /* Combine mode */
      mode = M_COMB;
      break;
    case 3:
    case 4:
      /* Concatenate mode */
      mode = M_CONCAT;
      break;
    default:
      /* Channel gain expressions */
      i = (n - 5) / 2;
      CPdecChan (OptArg, i, Chgain);
      Chset[i] = 1;     /* Flag, gain set for output channel i */
      break;
    }
  }

/* Checks */
  if (nF < 2)
    UThalt ("%s: %s", PROGRAM, CPM_MFName);
  for (i = 0; i < Chgain->NCO; ++i) {  /* Check all output channel gains */
    if (Chset[i] == 0)
      UThalt ("%s: %s %c", PROGRAM, CPM_NoChanSpec, Ch[i]);
  }
  if (FIParSet >= nF - 1)     /* Input parameters set after last input file */
    UThalt ("%s: %s", PROGRAM, CPM_LateFPar);

  /* Check for too many stdin specs */
  AOstdin (FI, nF-1);

/* Set return values */
  *Mode = mode;
  *Nifiles = nF-1;

  return;
}
