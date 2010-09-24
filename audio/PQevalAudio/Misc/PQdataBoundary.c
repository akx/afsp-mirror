/*------------- Telecommunications & Signal Processing Lab -------------
                           McGill University

Routine:
  void PQdataBoundary (AFILE *AFp, long int Nchan, long int StartS,
                       long int Ns, long int Lim[2])
Purpose:
  Find the data boundaries for an audio file

Description:
  The data boundary is determined with a 5 sample sliding window. Searching
  from the beginning and the end, the data boundary is found when the sum of
  the absolute values of the samples in the window exceeds 200.

Parameters:
   -> AFILE *AFp
      Audio file pointer
   -> long int Nchan
      Number of channels
   -> long int StartS
      Starting sample for the search
   -> long int Ns
      Number of samples for the search
  <-  long int Lim[2]
      Returned sample limits

Author / revision:
  P. Kabal  Copyright (C) 2009
  $Revision: 1.11 $  $Date: 2009/03/23 15:50:12 $

----------------------------------------------------------------------*/

#include <assert.h>
#include <math.h>

#include <libtsp.h>
#include "PQevalAudio.h"

#define PQ_L	5
#define NBUFF	2048
#define PQ_ATHR	200.

#define MAXV(a, b)	(((a) > (b)) ? (a) : (b))
#define MINV(a, b)	(((a) < (b)) ? (a) : (b))

static int
PQ_DataStart (const double x[], int N, int L, double Thr);
static int
PQ_DataEnd (const double x[], int N, int L, double Thr);


void
PQdataBoundary (AFILE *AFp, long int Nchan, long int StartS, long int Ns,
		long int Lim[2])

{
  int k, Nf;
  long int is, ie, js, EndS;
  double **x;

  assert (Nchan <= 2);

  /* Set up the buffer */
  x = MAdAllocMat (Nchan, NBUFF);

/* Search from the beginning of the file */
  Lim[0] = -1;
  is = StartS;
  EndS = StartS + Ns - 1;
  while (is <= EndS) {
    Nf = (int) MINV (EndS - is + 1, NBUFF);
    PQreadChan (AFp, Nchan, is, x, Nf);
    for (k = 0; k < Nchan; ++k)
      Lim[0] = MAXV(Lim[0], PQ_DataStart (x[k], Nf, PQ_L, PQ_ATHR));
    if (Lim[0] >= 0) {
      Lim[0] += is;
      break;
    }
    is += NBUFF - (PQ_L-1);
  }

  /* Search from the end of the file */
  /* This loop is written as if it is going in a forward direction
     - When the "forward" position is i, the "backward" position is
       EndS - (i - StartS + 1) + 1
  */
  Lim[1] = -1;
  is = StartS;
  while (is <= EndS) {
    Nf = (int) MINV (EndS - is + 1, NBUFF);
    ie = is + Nf - 1;			/* Forward limits [is, ie] */
    js = EndS - (ie - StartS + 1) + 1;	/* Backward limits [js, js+Nf-1] */
    PQreadChan (AFp, Nchan, js, x, Nf);
    for (k = 0; k < Nchan; ++k)
      Lim[1] = MAXV(Lim[1], PQ_DataEnd (x[k], Nf, PQ_L, PQ_ATHR));
    if (Lim[1] >= 0) {
      Lim[1] += js;
      break;
    }
    is += NBUFF - (PQ_L-1);
  }

  /* Sanity checks */
  assert ((Lim[0] >= 0 && Lim[1] >= 0) || (Lim[0] < 0 && Lim[1] < 0));
  assert (Lim[0] <= Lim[1]);

  if (Lim[0] < 0) {
    Lim[0] = 0;
    Lim[1] = 0;
  }

  MAdFreeMat (x);

  return;
}

static int
PQ_DataStart (const double x[], int N, int L, double Thr)

{
  int i, ib, M;
  double s;

  ib = -1;
  s = 0;
  M = MINV (N, L);
  for (i = 0; i < M; ++i)
    s += fabs (x[i]);
  if (s > Thr) {
    ib = 0;
    return ib;
  }

  for (i = 1; i <= N-L; ++i) {			/* i is the first sample */
    s += fabs (x[i+L-1]) - fabs (x[i-1]);	/* L samples apart */
    if (s > Thr) {
      ib = i;
      return ib;
    }
  }

  return ib;
}

static int
PQ_DataEnd (const double x[], int N, int L, double Thr)

{
  int i, ie, M;
  double s;

  ie = -1;
  s = 0;
  M = MINV (N, L);
  for (i = N-M; i < N; ++i)
    s += fabs (x[i]);
  if (s > Thr) {
    ie = N-1;
    return ie;
  }
  i = N - 1;

  for (i = N-2; i >= L-1; --i) {		/* i is the last sample */
    s += fabs (x[i-L+1]) - fabs (x[i+1]);	/* L samples apart */
    if (s > Thr) {
      ie = i;
      return ie;
    }
  }

  return ie;
}
