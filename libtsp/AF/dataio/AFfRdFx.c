/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int AFfRdF4 (AFILE *AFp, float Dbuff[], int Nreq)
  int AFfRdF8 (AFILE *AFp, float Dbuff[], int Nreq)

Purpose:
  Read 32-bit float data from an audio file (return float values)
  Read 64-bit float data from an audio file (return float values)

Description:
  These routines read a specified number of float samples from an audio file.
  The data in the file is converted to float format on output.

Parameters:
  <-  int AFfRdFx
      Number of data values transferred from the file.  On reaching the end of
      the file, this value may be less than Nreq.
   -> AFILE *AFp
      Audio file pointer for an audio file opened by AFopnRead
  <-  float Dbuff[]
      Array of floats to receive the samples
   -> int Nreq
      Number of samples requested.  Nreq may be zero.

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.3 $  $Date: 2017/05/24 16:08:16 $

-------------------------------------------------------------------------*/

#include <AFpar.h>
#include <libtsp/AFdataio.h>
#include <libtsp/UTtypes.h>

#define LW4   FDL_FLOAT32
#define LW8   FDL_FLOAT64


int
AFfRdF4 (AFILE *AFp, float Dbuff[], int Nreq)

{
  int is, N, i, Nr;
  UT_float4_t Buf[NBBUF/LW4];

  for (is = 0; is < Nreq; ) {

    /* Read data from the audio file */
    N = MINV (NBBUF / LW4, Nreq - is);
    Nr = FREAD (Buf, LW4, N, AFp->fp);

    /* Byte swap and scale */
    for (i = 0; i < Nr; ++i, ++is) {
      if (AFp->Swapb == DS_SWAP)
        BSWAP4 (&Buf[i]);
      Dbuff[is] = (float) (AFp->ScaleF * Buf[i]);
    }

    if (Nr < N)
      break;
  }

  return is;
}

int
AFfRdF8 (AFILE *AFp, float Dbuff[], int Nreq)

{
  int is, N, i, Nr;
  UT_float8_t Buf[NBBUF/LW8];

  for (is = 0; is < Nreq; ) {

    /* Read data from the audio file */
    N = MINV (NBBUF / LW8, Nreq - is);
    Nr = FREAD (Buf, LW8, N, AFp->fp);

    /* Byte swap and scale */
    for (i = 0; i < Nr; ++i, ++is) {
      if (AFp->Swapb == DS_SWAP)
        BSWAP8 (&Buf[i]);
      Dbuff[is] = (float) (AFp->ScaleF * Buf[i]);
    }
    if (Nr < N)
      break;
  }

  return is;
}
