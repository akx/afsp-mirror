/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int AOsetFtype (const struct AO_FOpar *FO)

Purpose:
  Determine an output file type

Description:
  This routine determines an output file type from the output file parameter
  structure. If the file type is set in the structure, that value is used.
  Otherwise the file type is determined by the output file name extension.
  If the file name extension is non-standard, a default file type is used.

  The mapping from file name extensions to file types is as follows.  The
  asterisk indicates that a shortened form can be used.
    .wav*e        - FTW_WAVE, Wave file
    .au           - FTW_AU, AU audio file
    .aif*f        - FTW_AIFF, AIFF sound file
    .afc or .aifc - FTW_AIFF_C, AIFF-C sound file
    .raw, .txt, .dat, or .nh - FTW_NH_NATIVE, headerless file
  A special case occurs if the file type is not resolved via the file name
  extension and the data format is text. In that case, the file type is set
  to headerless.

  For use in error messages, the program name should be set using the routine
  UTsetProg.

Parameters:
  <-  int AOsetFormat
      Output code for the file type
   -> const struct AO_FOpar *FO
      Output file parameters

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.1 $  $Date: 2003/05/12 23:54:38 $

-------------------------------------------------------------------------*/

#include <stdio.h>	/* FILENAME_MAX */

#include <libtsp.h>		/* defines AFILE, used by AFpar.h */
#include <libtsp/AFpar.h>
#include <AO.h>

#define ROUTINE		"AOsetFtype"
#define PGM		((UTgetProg ())[0] == '\0' ? ROUTINE : UTgetProg ())

static const char *Ext_keys[] = {
  ".wav*e",
  ".au",
  ".aif*f",
  ".afc", ".aifc",
  ".raw", ".nh", ".dat", ".txt",
  NULL
};

/* Values corresponding to extensions */
static const int Ftype_values[] = {
  FTW_WAVE,
  FTW_AU,
  FTW_AIFF,
  FTW_AIFF_C, FTW_AIFF_C,
  FTW_NH_NATIVE, FTW_NH_NATIVE, FTW_NH_NATIVE, FTW_NH_NATIVE,
};


int
AOsetFtype (const struct AO_FOpar *FO)

{
  int Ftype, n;

  char Ename[FILENAME_MAX];

  Ftype = FO->Ftype;
  if (Ftype != FTW_UNDEF)
    return Ftype;

  /* Get the file name extension */
  FLextName (FO->Fname, Ename);
  STstrLC (Ename, Ename);

  /* Check for a match to the extension */
  n = STkeyMatch (Ename, Ext_keys);
  if (n >= 0)
    Ftype = Ftype_values[n];
  else if (FO->DFormat.Format == FD_TEXT)
    Ftype = FTW_NH_NATIVE;
  else
    Ftype = AO_FTYPEO_DEFAULT;

  return Ftype;
}
