/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int AOsetDformat (const struct AO_FOpar *FO, AFILE *AFp[], int Nf)

Purpose:
  Determine a compatible output data format for the output file type

Description:
  This routine determines an output data format compatible with an output
  file type.  If either is explicitly specified, that data format is used.
  If the output data format is not specified, it is chosen to have the same or
  greater precision as that of the input data formats.

  For use in error messages, the program name should be set using the routine
  UTsetProg.

Parameters:
  <-  int AOsetFormat
      Output code for the data format
   -> const struct AO_FOpar *FO
      Output file parameters.  The file type (FO->Ftype) can be FTW_UNDEF.
      The data format (FO->DFormat.Format) can be FD_UNDEF.
   -> const AFILE *AFp[]
      Audio file pointers for the input files (Nf values)
   -> int Nf
      Number of input audio file pointers (can be zero)

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.1 $  $Date: 2003/05/12 23:52:13 $

-------------------------------------------------------------------------*/

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

static const int *
AO_Allow (int Ftype);


int
AOsetDformat (const struct AO_FOpar *FO, AFILE *AFp[], int Nf)

{
  int i, Ftype, Dformat, Df;
  const int *Allow;

/*
   Format   Ftype
  defined   defined    Use the combination specified by the user, but give an
                       error if the combination is unsupported.
  defined   undefined  If the data format is text, use a noheader file
                       type, otherwise use the file type determined from the
		       file extension.
  undefined defined    Data format from input, modified by allowable output
                       data formats for the given file type.
  undefined undefined  Data format from input, output file type determined from
                       the file extension.
*/

  /* Set the output file type if not set */
  Ftype = AOsetFtype (FO);
  Allow = AO_Allow (Ftype);

  Dformat = FO->DFormat.Format;
  if (Dformat == FD_UNDEF) {

/* Data format not defined */
    /* Choose the output data format based on the input data format
       Data promotion rules
       - convert to allowable data formats for the output file type
       - for mixed input formats, find the resulting precision
       - find the canonical data format for that precision
    */
    if (Nf <= 0)
      Dformat = Allow[AO_DFORMATO_DEFAULT];
    else
      Dformat = Allow[AFp[0]->Format];

    for (i = 1; i < Nf; ++i) {
      Df = Allow[AFp[i]->Format];
      if (Df != Dformat)
	Dformat = DfPrec[MAXV (Prec[Df], Prec[Dformat])];
    }
  }

  else  {

/* File type defined, data format now defined (perhaps by default) */
    /* Check for compatibility of the data format and file type */
    if (Dformat != Allow[Dformat])
      UThalt (AOMF_DataFType, PGM, AF_DTN[Dformat], AF_FTWN[Ftype]);
  }

  return Dformat;
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
    Allow = NULL;
    UThalt ("%s: %s", PGM, AOM_InvFTypeC);
  }

  return Allow;
}
