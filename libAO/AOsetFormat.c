/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int AOsetFormat (const struct AO_FOpar *FO, AFILE *AFp[], int Nf)

Purpose:
  Determine a compatible output data format and output file type

Description:
  This routine determines an output data format and an output file type.  If
  either is explicitly specified, that data format or file type is used.  If
  the output data format is not specified, it is chosen to have the same or
  greater precision as that of the input data formats.  If the output data
  file type is not specified, it is determined based on the extension of the
  output file name, or failing that, based on a default file type.

  For use in error messages, the program name should be set using the routine
  UTsetProg.

Parameters:
  <-  int AOsetFormat
      Output code for the file type and data format
   -> const struct AO_FOpar *FO
      Output file parameters.  The file type (FO->Ftype) can be FTW_UNDEF.
      The data format (FO->DFormat.Format) can be FD_UNDEF.
   -> const AFILE *AFp[]
      Audio file pointers for the input files (Nf values)
   -> int Nf
      Number of input audio file pointers (can be zero)

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.31 $  $Date: 2003/01/27 14:21:51 $

-------------------------------------------------------------------------*/

static char rcsid[] = "$Id: AOsetFormat.c 1.31 2003/01/27 AFsp-v6r8 $";

#include <assert.h>
#include <stdio.h>	/* FILENAME_MAX */

#include <libtsp.h>		/* defines AFILE, used by AFpar.h */
#define AF_DATA_TYPE_NAMES
#define AF_OUTPUT_FILE_TYPE_NAMES
#include <libtsp/AFpar.h>
#include <AO.h>

#define ROUTINE		"AOsetFormat"
#define PGM		((UTgetProg ())[0] == '\0' ? ROUTINE : UTgetProg ())
#define MAXV(a, b)	(((a) > (b)) ? (a) : (b))

/* Resulting precision for mixed data formats */
/* 0 - undefined, 1 - 16-bit, 2 - 24-bit, 3 - 32-bit, 4 - float, 5 - double */
static const int Prec[NFD] = { 0, 1, 1, 1, 1, 1, 2, 3, 4, 5, 1};

/* Canonical data formats for each of the precisions */
static const int DfPrec[6] = {FD_UNDEF, FD_INT16, FD_INT24, FD_INT32,
			      FD_FLOAT32, FD_FLOAT64};

static struct AO_FOpar
AO_setFtype (const struct AO_FOpar *FO);
static const int *
AO_Allow (int Ftype);


int
AOsetFormat (const struct AO_FOpar *FO, AFILE *AFp[], int Nf)

{
  struct AO_FOpar FOx;
  int i, Df;
  const int *Allow;

/*
   Format   Ftype
  defined   defined    Use the combination specified by the user, but give an
                       error if the combination is unsupported.
  defined   undefined  If the data format is text, use a noheader file
                       type, otherwise use the file type determined from the
		       file extension.
  undefined defined    Data type from input, modified by allowable output data
                       types for the given file type.
  undefined undefined  Data type from input, output file type determined from
                       the file extension.
*/

  /* Set the output file type if not set */
  FOx = AO_setFtype (FO);

  if (FOx.DFormat.Format == FD_UNDEF) {

/* Data type not defined */
    /* Choose the output data type based on the input data types
       Data promotion rules
       - convert to allowable data formats for the output file type
       - for mixed input formats, find the resulting precision
       - find the canonical data format for that precision
    */
    Allow = AO_Allow (FOx.Ftype);
    if (Nf <= 0)
      FOx.DFormat.Format = Allow[AO_DFORMATO_DEFAULT];
    else
      FOx.DFormat.Format = Allow[AFp[0]->Format];
    for (i = 1; i < Nf; ++i) {
      Df = Allow[AFp[i]->Format];
      if (Df != FOx.DFormat.Format)
	FOx.DFormat.Format = DfPrec[MAXV (Prec[Df], Prec[FOx.DFormat.Format])];
    }
  }

  else  {

/* Data type defined, data type now defined (perhaps by default) */
    /* Check for compatibility of the data type and file type */
    Allow = AO_Allow (FOx.Ftype);
    if (FOx.DFormat.Format != Allow[FOx.DFormat.Format])
      UThalt (AOMF_DataFType, PGM,
	      AF_DTN[FOx.DFormat.Format], AF_FTWN[FOx.Ftype]);
  }

  return FTW_code (FOx.Ftype, FOx.DFormat.Format);
}

static const int *
AO_Allow (int Ftype)

{
  const int *Allow;

/* Conversion tables to allowable data formats for different file types */
  static const int Allow_AU[NFD] = {
    FD_UNDEF, FD_MULAW8, FD_ALAW8, FD_INT8, FD_INT8, FD_INT16, FD_INT24,
    FD_INT32, FD_FLOAT32, FD_FLOAT64, FD_INT16
  };
  static const int Allow_WAVE[NFD] = {
    FD_UNDEF, FD_MULAW8, FD_ALAW8, FD_UINT8, FD_UINT8, FD_INT16, FD_INT24,
    FD_INT32, FD_FLOAT32, FD_FLOAT64, FD_INT16
  };
  static const int Allow_AIFF[NFD] = {
    FD_UNDEF, FD_INT16, FD_INT16, FD_INT8, FD_INT8, FD_INT16, FD_INT24,
    FD_INT32, FD_INT16, FD_INT16, FD_INT16
  };
  static const int Allow_AIFF_C[NFD] = {
    FD_UNDEF, FD_MULAW8, FD_ALAW8, FD_INT8, FD_INT8, FD_INT16, FD_INT24,
    FD_INT32, FD_FLOAT32, FD_FLOAT64, FD_INT16
  };
  static const int Allow_NH[NFD] = {
    FD_UNDEF, FD_MULAW8, FD_ALAW8, FD_UINT8, FD_INT8, FD_INT16, FD_INT24,
    FD_INT32, FD_FLOAT32, FD_FLOAT64, FD_TEXT
  };

  Allow = NULL;
  switch (Ftype) {
  case FTW_AU:
    Allow = Allow_AU;
    break;
  case FTW_WAVE:
  case FTW_WAVE_NOEX:
    Allow = Allow_WAVE;
    break;
  case FTW_AIFF:
    Allow = Allow_AIFF;
    break;
  case FTW_AIFF_C:
    Allow = Allow_AIFF_C;
    break;
  case FTW_NH_EB:
  case FTW_NH_EL:
  case FTW_NH_NATIVE:
  case FTW_NH_SWAP:
    Allow = Allow_NH;
    break;
  default:
    UThalt ("%s: %s", PGM, AOM_InvFTypeC);
  }

  return Allow;
}

/* Set the file output type depending on the file name extension.
     .wav => WAVE file
     .au  => AU file
     .aif => AIFF-C file
     .raw => Headerless file (native byte order)
     .txt => Headerless file (text data)
   The last case also sets the Format field of the output structure.

   If no match to the extension is found, the output is set to the default file
   type.  An exception occurs is the data format is text.  In that case the
   output file type is set to Headerless.
*/

static const char *Ext_keys[] = {
  ".txt", ".asc*ii",
  ".wav*e",
  ".au",
  ".aif*f", ".aif*c",
  ".raw", ".nh",
  NULL
};

/* Values corresponding to extensions */
static const int Ftype_values[] = {
  FTW_NH_NATIVE, FTW_NH_NATIVE,
  FTW_WAVE,
  FTW_AU,
  FTW_AIFF_C, FTW_AIFF_C,
  FTW_NH_NATIVE, FTW_NH_NATIVE
};


static struct AO_FOpar
AO_setFtype (const struct AO_FOpar *FO)

{
  int n;
  struct AO_FOpar FOx;
  char Ename[FILENAME_MAX];

  FOx = *FO;
  if (FOx.Ftype != FTW_UNDEF)
    return FOx;

  /* Get the file name extension */
  FLextName (FOx.Fname, Ename);
  STstrLC (Ename, Ename);

  /* Check for a match to the extension */
  n = STkeyMatch (Ename, Ext_keys);
  if (n >= 0) {
    FOx.Ftype = Ftype_values[n];
    if (n == 0 || n == 1) {	/* text file extension */
      if (FOx.DFormat.Format != FD_UNDEF && FOx.DFormat.Format != FD_TEXT)
	UThalt ("%s: %s", PGM, AOM_BadText);
      FOx.DFormat.Format = FD_TEXT;
    }
  }
  else if (FOx.DFormat.Format == FD_TEXT)
    FOx.Ftype = FTW_NH_NATIVE;
  else
    FOx.Ftype = AO_FTYPEO_DEFAULT;

  return FOx;
}
