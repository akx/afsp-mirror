/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void AOsetDFormat (struct AO_FOpar *FO, AFILE *AFp[], int Nf)

Purpose:
  Determine a compatible output data format for the output file type

Description:
  This routine determines an output data format compatible with an output file
  type. If either is explicitly specified, that data format is used. If the
  output data format is not specified, it is chosen to have the same or greater
  precision as that of the input data formats.

  For use in error messages, the program name should be set using the routine
  UTsetProg.
  
  Output File Type:
    If the file type is set in the structure FO, that value is used. Otherwise
    the file type is determined by the output file name extension. If the file
    name extension is non-standard, a default file type is used.

    The mapping from file name extensions to file types is as follows.  The
    asterisk indicates that a shortened form can be used.
      .wav*e        - FTW_WAVE, Wave file
      .au           - FTW_AU, AU audio file
      .aif*f        - FTW_AIFF, AIFF sound file
      .afc or .aifc - FTW_AIFF_C, AIFF-C sound file
      .raw, .dat, or .nh - FTW_NH_NATIVE, headerless file
      .txt or .text - FTW_TXAUD, text file (with header)
  
    The file type is checked for validity and the file type in the structure FO
    is updated if necessary.
    
  Output File Data Format:
    If the data format is defined in the the structure FO, that value is used.
    Otherwise if no input files information is available (Nf == 0), a default
    output data format is used.

    If the data format for the input files is available in the array of
    structures AFp, those will be used to determine an output data format of
    appropriate precision.  First a list of possible data formats for the output
    file type is set up.  For each data format, there is an associated ranking
    in precision.  For instance 24-bit integer is ranked higher than 16-bit
    integer.  For input formats with mixed precisions, the output format will
    have the higher precision. Unless the data format is explicitly specified,
    the output precision will be at least 16 bits.

Parameters:
  <-  void AOsetDFormat
  <-  enum AF_FD_T AOsetDFormat
      Output code for the data format
   -> const struct AO_FOpar *FO
      Output file parameters.  The file type (FO->FtypeW) can be FTW_UNDEF.
      The data format (FO->DFormat.Format) can be FD_UNDEF.
   -> const AFILE *AFp[]
      Audio file pointers for the input files (Nf values)
   -> int Nf
      Number of input audio file pointers (can be zero)

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.12 $  $Date: 2017/06/26 18:56:42 $

-------------------------------------------------------------------------*/

#include <assert.h>
#include <stdio.h>  /* FILENAME_MAX */

#include <AO.h>

#define ROUTINE       "AOsetFormat"
#define PGM           ((UTgetProg ())[0] == '\0' ? ROUTINE : UTgetProg ())
#define MAXV(a, b)    (((a) > (b)) ? (a) : (b))
#define NELEM(array)  ((int) ((sizeof array) / (sizeof array[0])))

/* Resulting precision ranking for data formats. Note that float is considered
   to be more precise than 32-bit integer. This is done so that integer inputs
   will be represented with integer data formats on output.  Note that the
   minimum precision for the formats is that of 16-bit integer.  Thus mu-law,
   A-law, and the one-byte formats will be promoted to 16-bit precision.
     0 - undefined,
     1 - 8-bit unsigned or integer
     2 - mu-law or A-law
     3 - 16-bit integer,
     4 - 24-bit integer,
     5 - 32-bit integer,
     6 - float, text16, text
     7 - double

   The text formats are really floating point formats with 7 significant digits
   of precision - about the same as float.  text16 may give a more compact
   representation than text if the underlying data is integer-valued.  For such
   data tex16 avoids fractional values (with leading zeros) and/or an exponent.
    order:{undef, mu-law, A-law, u8, i8, i16, i24, i32, f4 f8, t16, t}
*/
static const int PrecD[] = {0, 2, 2, 2, 1, 1, 3, 4, 5, 6, 7, 6, 6};

/* Canonical data formats for each of the precisions */
static const enum AF_FD_T Prec2FD[8] = {FD_UNDEF, FD_INT8, FD_MULAW8, FD_INT16,
                                        FD_INT24, FD_INT32, FD_FLOAT32,
                                        FD_FLOAT64};

/* For the error message */
static const char *AF_FTWN[] = {
  "UNDEFINED",
  "AU audio file",
  "WAVE file",
  "AIFF sound file",
  "AIFF-C sound file",
  "AIFF-C/sowt sound file",
  "WAVE file",
  "WAVE file",
  "Headerless audio file",
  "Headerless audio file",
  "Headerless audio file",
  "Headerless audio file",
  "Text audio file"
};

static const enum AF_FD_T *
AO_AllowFD (enum AF_FTW_T FtypeW);


void
AOsetDFormat (struct AO_FOpar *FO, AFILE *AFp[], int Nf)

{
  int i, PrecI, PrecO;
  enum AF_FD_T Format;
  const enum AF_FD_T *AllowFD;

  assert (FTW_TXAUD+1 == NELEM (AF_FTWN));
  assert (NELEM (PrecD) == AF_NFD);

  /* Set the output file type if not set */
  AOsetFtypeW (FO);

  Format = FO->DFormat.Format;

  /* Special case */
  if (Format == FD_UNDEF && FO->FtypeW == FTW_TXAUD)
    Format = FD_TEXT;

  /* Set up allowable data formats for the output file type */
  AllowFD = AO_AllowFD (FO->FtypeW);

  if (Format == FD_UNDEF) {
    if (Nf <= 0)
      /* No input files, use the default format */
      Format = AllowFD[AO_DFORMATO_DEFAULT];
    else {
      /* Choose the output data format based on the input data formats */
      for (i = 0; i < Nf; ++i) {
        PrecO = MAXV (3, PrecD[Format]);    /* At least precision 3, int16 */
        PrecI = PrecD[AllowFD[AFp[i]->Format]];
        Format = Prec2FD[MAXV (PrecO, PrecI)];
      }
    }
  }

  /* Check for compatibility of the data format and file type */
  if (Format != AllowFD[Format])
    UThalt (AOMF_DataFType, PGM, AF_DTN[Format], AF_FTWN[FO->FtypeW]);

  /* Set the Format */
  FO->DFormat.Format = Format;
  
  return;
}

static const enum AF_FD_T *
AO_AllowFD (enum AF_FTW_T FtypeW)

{
  const enum AF_FD_T *AllowFD;

/* Conversion tables to allowable data formats for different file types */
  static const enum AF_FD_T Allow_AU[] = {
          FD_UNDEF, FD_ALAW8, FD_MULAW8, FD_MULAW8,
          FD_INT8, FD_INT8, FD_INT16, FD_INT24, FD_INT32,
          FD_FLOAT32, FD_FLOAT64, FD_INT16, FD_INT16};
  static const enum AF_FD_T Allow_WAVE[] = {
          FD_UNDEF, FD_ALAW8, FD_MULAW8, FD_MULAW8,
          FD_UINT8, FD_UINT8, FD_INT16, FD_INT24, FD_INT32,
          FD_FLOAT32, FD_FLOAT64, FD_INT16, FD_INT16};
  static const enum AF_FD_T Allow_AIFF[] = {
          FD_UNDEF, FD_INT16, FD_INT16, FD_INT16,
          FD_INT8, FD_INT8, FD_INT16, FD_INT24, FD_INT32,
          FD_INT16, FD_INT16, FD_INT16, FD_INT16};
  static const enum AF_FD_T Allow_AIFF_C[] = {
          FD_UNDEF, FD_ALAW8, FD_MULAW8, FD_MULAW8,
          FD_INT8, FD_INT8, FD_INT16, FD_INT24, FD_INT32,
          FD_FLOAT32, FD_FLOAT64, FD_INT16, FD_INT16};
  static const enum AF_FD_T Allow_NH[] = {
          FD_UNDEF, FD_ALAW8, FD_MULAW8, FD_MULAWR8,
          FD_UINT8, FD_INT8, FD_INT16, FD_INT24, FD_INT32,
          FD_FLOAT32, FD_FLOAT64, FD_TEXT16, FD_TEXT};
  static const enum AF_FD_T Allow_TXAUD[] = {
          FD_UNDEF, FD_TEXT16, FD_TEXT16, FD_TEXT16,
          FD_TEXT16, FD_TEXT16, FD_TEXT16, FD_TEXT16, FD_TEXT,
          FD_TEXT, FD_TEXT, FD_TEXT16, FD_TEXT};

  assert (NELEM (Allow_AU) == AF_NFD && NELEM (Allow_WAVE) == AF_NFD &&
          NELEM (Allow_AIFF) == AF_NFD && NELEM (Allow_AIFF_C) == AF_NFD &&
          NELEM (Allow_NH) == AF_NFD && NELEM (Allow_TXAUD) == AF_NFD);
          
  AllowFD = NULL;   /* Stop compiler warning */
  switch (FtypeW) {
  case FTW_AU:
    AllowFD = Allow_AU;
    break;
  case FTW_WAVE:
  case FTW_WAVE_EX:
  case FTW_WAVE_NOEX:
    AllowFD = Allow_WAVE;
    break;
  case FTW_AIFF:
  case FTW_AIFF_C_SOWT:
    AllowFD = Allow_AIFF;
    break;
  case FTW_AIFF_C:
    AllowFD = Allow_AIFF_C;
    break;
  case FTW_NH_NATIVE:
  case FTW_NH_SWAP:
  case FTW_NH_EL:
  case FTW_NH_EB:
    AllowFD = Allow_NH;
    break;
  case FTW_TXAUD:
    AllowFD = Allow_TXAUD;
    break;
  default:
    assert (0);
  }

  return AllowFD;
}
