/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int AFfWrTA (AFILE *AFp, const float Dbuff[], int Nval)

Purpose:
  Write text data to an audio file (float input values)

Description:
  These routines write a specified number of samples to an audio file.
  The input to these routine is a buffer of float values.  The output file
  contains the text representation of the data values.

Parameters:
  <-  int AFfWrTA
      Number of samples written.  If this value is less than Nval, an error
      has occurred.
   -> AFILE *AFp
      Audio file pointer for an audio file opened by AFopnWrite
   -> const float Dbuff[]
      Array of floats with the samples to be written
   -> int Nval
      Number of samples to be written, normally a multiple of AFp->Nchan

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.3 $  $Date: 2017/05/18 17:09:32 $

-------------------------------------------------------------------------*/

#include <AFpar.h>
#include <libtsp/AFdataio.h>


int
AFfWrTA (AFILE *AFp, const float Dbuff[], int Nval)

{
  int i, Nc, Nmod;

  /* Print a frame of samples on the same line */
  Nmod = AFp->Nchan;
  if (Nmod > 5 || Nval % AFp->Nchan != 0)
    Nmod = 1;

  for (i = 0; i < Nval; ++i) {
    if (i % Nmod < Nmod - 1)
      Nc = fprintf (AFp->fp, "%g ", AFp->ScaleF * Dbuff[i]);
    else
      Nc = fprintf (AFp->fp, "%g\n", AFp->ScaleF * Dbuff[i]);

    if (Nc < 0)   /* fprintf returns a negative valued error code */
      break;
  }

  return i;
}
