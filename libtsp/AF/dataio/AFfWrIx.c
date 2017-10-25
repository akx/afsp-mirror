/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int AFfWrU1 (AFILE *AFp, const float Dbuff[], int Nval)
  int AFfWrI1 (AFILE *AFp, const float Dbuff[], int Nval)
  int AFfWrI2 (AFILE *AFp, const float Dbuff[], int Nval)
  int AFfWrI3 (AFILE *AFp, const float Dbuff[], int Nval)
  int AFfWrI4 (AFILE *AFp, const float Dbuff[], int Nval)

Purpose:
  Write offset-binary 8-bit integer data to an audio file (float input values)
  Write 8-bit integer data to an audio file (float input values)
  Write 16-bit integer data to an audio file (float input values)
  Write 24-bit integer data to an audio file (float input values)
  Write 32-bit integer data to an audio file (float input values)

Description:
  These routines write a specified number of integer samples to an audio file.
  The input to this routine is a buffer of float values.

Parameters:
  <-  int AFfWrIx
      Number of samples written.  If this value is less than Nval, an error
      has occurred.
   -> AFILE *AFp
      Audio file pointer for an audio file opened by AFopnWrite
   -> const float Dbuff[]
      Array of floats with the samples to be written
   -> int Nval
      Number of samples to be written

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.2 $  $Date: 2017/04/24 03:01:44 $

-------------------------------------------------------------------------*/

#include <math.h>

#include <AFpar.h>
#include <libtsp/nucleus.h>
#include <libtsp/AFdataio.h>
#include <libtsp/UTtypes.h>

#define LW1   FDL_INT8
#define LW2   FDL_INT16
#define LW3   FDL_INT24
#define LW4   FDL_INT32



/* Round, clip, return integer value */
static inline UT_int4_t AF_RndOL (double Dv, double Dmin, double Dmax,
                                  long int *Novld)
{
  Dv = round (Dv);
  if (Dv > Dmax) {
    Dv = Dmax;
    ++*Novld;
  }
  else if (Dv < Dmin) {
    Dv = Dmin;
    ++*Novld;
  }

  return ((UT_int4_t) Dv);

}

/* Demote a 4 byte integer to a 3 byte integer
   Iv - integer (4 bytes) in host byte order
   Buf - output (3 chars) in host byte order
   HBO - Host byte order (DS_EL or DS_EB)
*/
static inline void
AF_DemoteB43 (UT_int4_t Iv, unsigned char *Buf, int HBO)
{
  unsigned char *cp;
  cp = (unsigned char *) &Iv;

  if (HBO == DS_EL) {
    Buf[0] = cp[0];
    Buf[1] = cp[1];
    Buf[2] = cp[2];
  }
  else {
    Buf[0] = cp[1];
    Buf[1] = cp[2];
    Buf[2] = cp[3];
  }

  return;
}

int
AFfWrU1 (AFILE *AFp, const float Dbuff[], int Nval)

{
  int is, N, Nw, i;
  UT_uint1_t Buf[NBBUF/LW1];

/* Write data to the audio file */
  is = 0;
  while (is < Nval) {
    N = MINV (NBBUF / LW1, Nval - is);
    for (i = 0; i < N; ++i)
      Buf[i] = AF_RndOL (AFp->ScaleF * Dbuff[i+is] + UT_UINT1_OFFSET,
                         0, UT_UINT1_MAX, &AFp->Novld);

    Nw = FWRITE (Buf, LW1, N, AFp->fp);
    is += Nw;
    if (Nw < N)
      break;
  }

  return is;
}

int
AFfWrI1 (AFILE *AFp, const float Dbuff[], int Nval)

{
  int is, N, Nw, i;
  UT_int1_t Buf[NBBUF/LW1];

/* Write data to the audio file */
  is = 0;
  while (is < Nval) {
    N = MINV (NBBUF / LW1, Nval - is);
    for (i = 0; i < N; ++i)
      Buf[i] = AF_RndOL (AFp->ScaleF * Dbuff[i+is], UT_INT1_MIN, UT_INT1_MAX,
                         &AFp->Novld);

    Nw = FWRITE (Buf, LW1, N, AFp->fp);
    is += Nw;
    if (Nw < N)
      break;
  }

  return is;
}

int
AFfWrI2 (AFILE *AFp, const float Dbuff[], int Nval)

{
  int is, N, Nw, i;
  UT_int2_t Buf[NBBUF/LW2];

/* Write data to the audio file */
  is = 0;
  while (is < Nval) {
    N = MINV (NBBUF / LW2, Nval - is);
    for (i = 0; i < N; ++i) {
      Buf[i] = AF_RndOL (AFp->ScaleF * Dbuff[i+is], UT_INT2_MIN, UT_INT2_MAX,
                         &AFp->Novld);
      if (AFp->Swapb == DS_SWAP)
        BSWAP2 (&Buf[i]);
    }
    Nw = FWRITE (Buf, LW2, N, AFp->fp);
    is += Nw;
    if (Nw < N)
      break;
  }

  return is;
}

int
AFfWrI3 (AFILE *AFp, const float Dbuff[], int Nval)

{
  int is, N, Nw, i, Hbo;
  UT_int4_t Iv;
  unsigned char Buf[NBBUF];

/* Write data to the audio file */
  Hbo = UTbyteOrder ();
  is = 0;
  while (is < Nval) {
    N = MINV (NBBUF / LW3, Nval - is);
    for (i = 0; i < N; ++i) {
      Iv = AF_RndOL (AFp->ScaleF * Dbuff[i+is], UT_INT3_MIN, UT_INT3_MAX,
                     &AFp->Novld);
      AF_DemoteB43 (Iv, &Buf[i*LW3], Hbo);
      if (AFp->Swapb == DS_SWAP)
        BSWAP3 (&Buf[i*LW3]);
    }
    Nw = FWRITE (Buf, LW3, N, AFp->fp);
    is += Nw;
    if (Nw < N)
      break;
  }

  return is;
}

int
AFfWrI4 (AFILE *AFp, const float Dbuff[], int Nval)

{
  int is, N, Nw, i;
  UT_int4_t Buf[NBBUF/LW4];

/* Write data to the audio file */
  is = 0;
  while (is < Nval) {
    N = MINV (NBBUF / LW4, Nval - is);
    for (i = 0; i < N; ++i) {
      Buf[i] = AF_RndOL (AFp->ScaleF * Dbuff[i+is], UT_INT4_MIN, UT_INT4_MAX,
                         &AFp->Novld);
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
