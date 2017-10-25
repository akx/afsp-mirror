/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int AFdRdU1 (AFILE *AFp, double Dbuff[], int Nreq)
  int AFdRdI1 (AFILE *AFp, double Dbuff[], int Nreq)
  int AFdRdI2 (AFILE *AFp, double Dbuff[], int Nreq)
  int AFdRdI3 (AFILE *AFp, double Dbuff[], int Nreq)
  int AFdRdI4 (AFILE *AFp, double Dbuff[], int Nreq)

Purpose:
  Read offset-binary 8-bit integer data from an audio file (return doubles)
  Read 8-bit integer data from an audio file (return double values)
  Read 16-bit integer data from an audio file (return double values)
  Read 24-bit integer data from an audio file (return double values)
  Read 32-bit integer data from an audio file (return double values)

Description:
  These routines read a specified number of integer samples from an audio file.
  The data in the file is converted to double format on output.

Parameters:
  <-  int AFdRdIx
      Number of data values transferred from the file.  On reaching the end of
      the file, this value may be less than Nreq.
   -> AFILE *AFp
      Audio file pointer for an audio file opened by AFopnRead
  <-  double Dbuff[]
      Array of doubles to receive the samples
   -> int Nreq
      Number of samples requested.  Nreq may be zero.

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.10 $  $Date: 2017/04/24 02:48:02 $

-------------------------------------------------------------------------*/

#include <AFpar.h>
#include <libtsp/nucleus.h>
#include <libtsp/AFdataio.h>
#include <libtsp/UTtypes.h>

#define LW1   FDL_INT8
#define LW2   FDL_INT16
#define LW3   FDL_INT24
#define LW4   FDL_INT32


/* Promote a 3 byte integer to a 4 byte integer
   Buf - (3 chars) in host byte order
   HBO - Host byte order (DS_EL or DS_EB)
*/
static inline UT_int4_t
AF_PromoteB34 (unsigned char *Buf, int HBO)
{
  UT_int4_t Iv;
  unsigned char *cp;

  cp = (unsigned char *) &Iv;
  if (HBO == DS_EL) {
    cp[0] = 0;      cp[1] = Buf[0];
    cp[2] = Buf[1]; cp[3] = Buf[2];  /* Most significant end */
  }
  else {
    cp[0] = Buf[0]; cp[1] = Buf[1];  /* Most significant end */
    cp[2] = Buf[2]; cp[3] = 0;
  }
  return (Iv/256);   /* Shift, extending the sign */
};

int
AFdRdU1 (AFILE *AFp, double Dbuff[], int Nreq)

{
  int is, N, i, Nr;
  UT_uint1_t Buf[NBBUF/LW1];

  for (is = 0; is < Nreq; ) {

    /* Read data from the audio file */
    N = MINV (NBBUF / LW1, Nreq - is);
    Nr = FREAD (Buf, LW1, N, AFp->fp);

    /* Convert and scale */
    /* For offset-binary 8-bit data, the zero-point is the value 128 */
    for (i = 0; i < Nr; ++i, ++is)
      Dbuff[is] = AFp->ScaleF * ((int) Buf[i] - UT_UINT1_OFFSET);

    if (Nr < N)
      break;
  }

  return is;
}

int
AFdRdI1 (AFILE *AFp, double Dbuff[], int Nreq)

{
  int is, N, i, Nr;
  UT_int1_t Buf[NBBUF/LW1];

  for (is = 0; is < Nreq; ) {

    /* Read data from the audio file */
    N = MINV (NBBUF / LW1, Nreq - is);
    Nr = FREAD (Buf, LW1, N, AFp->fp);

    /* Scale */
    for (i = 0; i < Nr; ++i, ++is)
      Dbuff[is] = AFp->ScaleF * Buf[i];

    if (Nr < N)
      break;
  }

  return is;
}

int
AFdRdI2 (AFILE *AFp, double Dbuff[], int Nreq)

{
  int is, N, i, Nr;
  UT_int2_t Buf[NBBUF/LW2];

  for (is = 0; is < Nreq; ) {

    /* Read data from the audio file */
    N = MINV (NBBUF / LW2, Nreq - is);
    Nr = FREAD (Buf, LW2, N, AFp->fp);

    /* Byte swap and scale */
    for (i = 0; i < Nr; ++i, ++is) {
      if (AFp->Swapb == DS_SWAP)
        BSWAP2 (&Buf[i]);
      Dbuff[is] = AFp->ScaleF * Buf[i];
    }

    if (Nr < N)
      break;
  }

  return is;
}

int
AFdRdI3 (AFILE *AFp, double Dbuff[], int Nreq)

{
  int is, N, i, Nr, Hbo;
  unsigned char Buf[NBBUF];

  Hbo = UTbyteOrder ();
  for (is = 0; is < Nreq; ) {

    /* Read data from the audio file */
    N = MINV (NBBUF/LW3, Nreq - is);
    Nr = FREAD (Buf, LW3, N, AFp->fp);

    /* Byte swap and scale */
    for (i = 0; i < Nr; ++i, ++is) {
      if (AFp->Swapb == DS_SWAP)
        BSWAP3 (&Buf[i*LW3]);
      Dbuff[is] = AFp->ScaleF * AF_PromoteB34 (&Buf[i*LW3], Hbo);
    }

    if (Nr < N)
      break;
  }

  return is;
}

int
AFdRdI4 (AFILE *AFp, double Dbuff[], int Nreq)

{
  int is, N, i, Nr;
  UT_int4_t Buf[NBBUF/LW4];

  for (is = 0; is < Nreq; ) {

    /* Read data from the audio file */
    N = MINV (NBBUF / LW4, Nreq - is);
    Nr = FREAD (Buf, LW4, N, AFp->fp);

    /* Byte swap and scale */
    for (i = 0; i < Nr; ++i, ++is) {
      if (AFp->Swapb == DS_SWAP)
        BSWAP4 (&Buf[i]);
      Dbuff[is] = AFp->ScaleF * Buf[i];
    }

    if (Nr < N)
      break;
  }

  return is;
}
