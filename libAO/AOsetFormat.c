/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int AOsetFormat (const struct AO_FOpar *FO, AFILE *AFp[], int Nf)

Purpose:
  Determine a compatible output data format and output file type

Description:
  Use AOsetFtype and AOsetDformat instead.

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
  $Revision: 1.32 $  $Date: 2003/05/12 23:54:00 $

-------------------------------------------------------------------------*/

#include <AO.h>

int
AOsetFormat (const struct AO_FOpar *FO, AFILE *AFp[], int Nf)

{
  int Ftype, Dformat;

  Ftype = AOsetFtype (FO);
  Dformat = AOsetDformat (FO, AFp, Nf);

  return (Dformat + 256 * Ftype);
}
