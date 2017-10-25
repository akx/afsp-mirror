/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void AOsetFtypeW (struct AO_FOpar *FO)

Purpose:
  Determine / check the output file type

Description:
  This routine determines an output file type in the output file parameter
  structure. If the file type is set in the structure, that value is used.
  Otherwise the file type is determined by the output file name extension.
  If the file name extension is non-standard, a default file type is used.

  The mapping from file name extensions to file types is as follows.  The
  asterisk indicates that a shortened form can be used.
    .wav*e        - FTW_WAVE, Wave file
    .au           - FTW_AU, AU audio file
    .aif*f        - FTW_AIFF, AIFF sound file
    .afc or .aifc - FTW_AIFF_C, AIFF-C sound file
    .raw, .nh, or .dat - FTW_NH_NATIVE, headerless file
    .txt or .text - FTW_TEXT, text audio file (with header)

  The file type is checked for validity before returning. For use in an error
  message, the program name should be set using the routine UTsetProg.

Parameters:
  <-  void AFsetFtypeW
  <-> struct AO_FOpar *FO
      Output file parameters. The output file type is updated if appropriate.

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.10 $  $Date: 2017/06/22 11:48:41 $

-------------------------------------------------------------------------*/

#include <stdio.h>  /* FILENAME_MAX */

#include <AO.h>

#define ROUTINE   "AOsetFtypeW"
#define PGM       ((UTgetProg())[0] == '\0' ? ROUTINE : UTgetProg())

static const char *Ext_keys[] = {
  ".wav*e",
  ".au",
  ".aif*f",
  ".afc", ".aifc",
  ".raw", ".nh", ".dat",
  ".txt", ".text",
  NULL
};

/* Values corresponding to extensions */
static const int FtypeW_values[] = {
  FTW_WAVE,
  FTW_AU,
  FTW_AIFF,
  FTW_AIFF_C, FTW_AIFF_C,
  FTW_NH_NATIVE, FTW_NH_NATIVE, FTW_NH_NATIVE,
  FTW_TXAUD, FTW_TXAUD
};


void
AOsetFtypeW (struct AO_FOpar *FO)

{
  int n;
  char Ename[FILENAME_MAX];

  switch (FO->FtypeW) {
  case FTW_UNDEF:
    /* Get the file name extension */
    FLextName (FO->Fname, Ename);
    STstrLC (Ename, Ename);

    /* Check for a match to the extension */
    n = STkeyMatch (Ename, Ext_keys);
    if (n >= 0)
      FO->FtypeW = FtypeW_values[n];
    else
      FO->FtypeW = AO_FTYPEW_DEFAULT;
    break;
  case FTW_AU:
  case FTW_WAVE:
  case FTW_AIFF:
  case FTW_AIFF_C:
  case FTW_AIFF_C_SOWT:
  case FTW_WAVE_EX:
  case FTW_WAVE_NOEX:
  case FTW_NH_NATIVE:
  case FTW_NH_SWAP:
  case FTW_NH_EL:
  case FTW_NH_EB:
  case FTW_TXAUD:
    break;
  default:
    UThalt ("%s: %s", PGM, AOM_InvFTypeC);
  }

  return;
}
