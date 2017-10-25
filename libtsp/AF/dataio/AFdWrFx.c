/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int AFdWrF4 (AFILE *AFp, const double Dbuff[], int Nval)
  int AFdWrF8 (AFILE *AFp, const double Dbuff[], int Nval)

Purpose:
  Write 32-bit float data to an audio file (double input values)
  Write 64-bit float data to an audio file (double input values)

Description:
  These routines write a specified number of float samples to an audio file.
  The input to this routine is a buffer of double values.

Parameters:
  <-  int AFdWrFx
      Number of samples written.  If this value is less than Nval, an error
      has occurred.
   -> AFILE *AFp
      Audio file pointer for an audio file opened by AFopnWrite
   -> const double Dbuff[]
      Array of doubles with the samples to be written
   -> int Nval
      Number of samples to be written

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.13 $  $Date: 2017/05/24 16:07:40 $

-------------------------------------------------------------------------*/

#include <AFpar.h>
#include <libtsp/AFdataio.h>
#include <libtsp/UTtypes.h>

#define LW4   FDL_FLOAT32
#define LW8   FDL_FLOAT64


int
AFdWrF4 (AFILE *AFp, const double Dbuff[], int Nval)

{
  int is, N, Nw, i;
  UT_float4_t Buf[NBBUF/LW4];

/* Write data to the audio file */
  is = 0;
  while (is < Nval) {
    N = MINV (NBBUF / LW4, Nval - is);
    for (i = 0; i < N; ++i) {
      Buf[i] = (UT_float4_t) (AFp->ScaleF * Dbuff[i+is]);
      if (AFp->Swapb == DS_SWAP)
        BSWAP4 (&Buf[i]);
    }
    Nw = FWRITE (Buf, LW4, N, AFp->fp);
    is += Nw;
    if (Nw < N)
      break;
  }

  return is;
}

int
AFdWrF8 (AFILE *AFp, const double Dbuff[], int Nval)

{
  int is, N, Nw, i;
  UT_float8_t Buf[NBBUF/LW8];

/* Write data to the audio file */
  is = 0;
  while (is < Nval) {
    N = MINV (NBBUF / LW8, Nval - is);
    for (i = 0; i < N; ++i) {
      Buf[i] = AFp->ScaleF * Dbuff[i+is];
      if (AFp->Swapb == DS_SWAP)
        BSWAP8 (&Buf[i]);
    }
    Nw = FWRITE (Buf, LW8, N, AFp->fp);
    is += Nw;
    if (Nw < N)
      break;
  }

  return is;
}
