/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int PQreadChan (AFILE *AFp, long int Nchan, long int offs, double *x[],
                  int Nreq)

Purpose:
  Read channel data from an audio file

Description:
  This routine returns a specified number of data frames at a given frame
  offset in an audio file.  The data in the file is converted to double format
  on output.

  For multichannel data, the file contains channel-interleaved data. This
  routine de-interleaves the data. The output array will contain Nreq samples
  from the first channel, followed by Nreq samples from the second channel, and
  so on.

Parameters:
  <-  int PQreadChan
      Number of data frames transferred from the file.  On reaching the end of
      the file, the value may be less than Nreq, in which case the last frames
      are set to zero.
   -> AFILE *AFp
      Audio file pointer for an audio file opened by AFopenRead
   -> long int offs
      Offset into the file in frames.  If offs is positive, the first value
      returned is offs frames from the beginning of the data.
  <-  double *x[]
      Array of Nchan pointers to the channel buffers
   -> int Nreq
      Number of data frames requested.  Nreq may be zero.

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.6 $  $Date: 2003/05/13 01:12:34 $

-------------------------------------------------------------------------*/

#include <assert.h>
#include <libtsp.h>

#define NBUFF	2048
#define ICEILV(n,m)	(((n) + ((m) - 1)) / (m))	/* int n,m >= 0 */
#define MINV(a, b)	(((a) < (b)) ? (a) : (b))


int
PQreadChan (AFILE *AFp, long int Nchan, long int offs, double *x[], int Nreq)

{
  int Nf, Nv, N, is, i, k;
  double Tbuff[NBUFF];

  assert (Nchan <= NBUFF);

  Nf = 0;	/* Number of frames read */
  is = 0;

  while (is < Nreq) {

    /* Read data from the file */
    N = MINV (Nreq - is, NBUFF / Nchan);
    Nv = AFdReadData (AFp, (offs + is) * Nchan, Tbuff, N * Nchan);
    Nf += ICEILV (Nv, Nchan);

    /* De-interleave */
    for (k = 0; k < Nchan; ++k) {
      for (i = 0; i < N; ++i)
	x[k][i+is] = Tbuff[i*Nchan+k];
    }
    is += N;
  }

  return Nf;
}
