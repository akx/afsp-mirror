/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int AFrdU1 (AFILE *AFp, float Fbuff[], int Nreq)

Purpose:
  Read offset-binary 8-bit integer data from an audio file (return float)

Description:
  This routine reads a specified number of offset-binary 8-bit integer samples
  from an audio file.  The data in the file is converted to float format on
  output.

Parameters:
  <-  int AFrdU1
      Number of data values transferred from the file.  On reaching the end of
      the file, this value may be less than Nreq.
   -> AFILE *AFp
      Audio file pointer for an audio file opened by AFopenRead
  <-  float Fbuff[]
      Array of floats to receive the samples
   -> int Nreq
      Number of samples requested.  Nreq may be zero.

Author / revision:
  P. Kabal  Copyright (C) 2001
  $Revision: 1.11 $  $Date: 2001/03/22 21:45:54 $

-------------------------------------------------------------------------*/

static char rcsid [] = "$Id: AFrdU1.c 1.11 2001/03/22 AFsp-v6r8 $";

#include <libtsp/AFdataio.h>
#include <libtsp/AFpar.h>
#include <libtsp/UTtypes.h>

#define LW		FDL_UINT8
#define MINV(a, b)	(((a) < (b)) ? (a) : (b))
#define NBBUF		8192

#define UINT1_OFFSET	((UINT1_MAX+1)/2)

#define FREAD(buf,size,nv,fp)	(int) fread ((char *) buf, (size_t) size, \
					     (size_t) nv, fp)


int
AFrdU1 (AFILE *AFp, float Fbuff[], int Nreq)

{
  int is, N, i, Nr;
  uint1_t Buf[NBBUF/LW];
  float g;

  for (is = 0; is < Nreq; ) {

    /* Read data from the audio file */
    N = MINV (NBBUF / LW, Nreq - is);
    Nr = FREAD (Buf, LW, N, AFp->fp);

    /* Convert to float */
    /* For offset-binary 8-bit data, the zero-point is the value 128 */
    g = (float) AFp->ScaleF;
    for (i = 0; i < Nr; ++i, ++is)
      Fbuff[is] = g * (((int) Buf[i]) - UINT1_OFFSET);

    if (Nr < N)
      break;
  }

  return is;
}
