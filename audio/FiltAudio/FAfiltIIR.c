/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void FAfiltIIR(AFILE *AFpI, AFILE *AFpO, long int NsampO,
                 const double h[][5], int Nsec, int Nsub, long int noffs)

Purpose:
  Filter an audio file with an IIR filter

Description:
  This routine convolves the data from the input audio file with an IIR filter
  response.

Parameters:
   -> AFILE *AFpI
      Audio file pointer for the input audio file
   -> AFILE *AFpO
      Audio file pointer for the output audio file
   -> long int NsampO
      Number of output samples to be calculated
   -> const double h[][5]
      Array of Nsec IIR filter sections
   -> int Nsec
      Number of filter sections
   -> int Nsub
      Subsampling factor
   -> long int noffs
      Data offset into the input data for the first output point

Author / revision:
  P. Kabal  Copyright (C) 2020
  $Revision: 1.20 $  $Date: 2020/11/23 18:31:10 $

-------------------------------------------------------------------------*/

#include <libtsp.h>
#include "FiltAudio.h"

#define ICEILV(n,m) (((n) + ((m) - 1)) / (m)) /* int n,m >= 0 */
#define MAXV(a, b)  (((a) > (b)) ? (a) : (b))
#define MINV(a, b)  (((a) < (b)) ? (a) : (b))

static void
FA_writeSubData(AFILE *AFp0, long int k, int Nsub, const double x[], int Nx);


void
FAfiltIIR(AFILE *AFpI, AFILE *AFpO, long int NsampO, const double h[][5],
          int Nsec, int Nsub, long int noffs)

{
  double x[NBUF];
  int lmem, Nxmax, Nx;
  long int n, k, NyO;

/*
Notes:
  - The input signal d(.) is the data in the file, with d(0) corresponding to
    the first data value in the file.
  - Indexing: n is an offset into d(), referring to sample d(n).

Batch processing:
  - The data will be processed in batches by reading into a buffer x(.,.). The
    batches of input samples will be of equal size, Nx, except for the last
    batch. For batch j,
      x(j,n') = d(noffs+j*Nx+n'), for 0 <= n' < Nx,
  - The k'th output point y(k) is calculated at position d(noffs+k) - the start
    of the impulse response, h(0), is aligned with d(noffs+k).
      y(k) --> h[0] <==> d(n),    where l=loffs+k
               h[0] <==> x(j,n').
  - For batch j=0,
      n = noffs  - pointer to d(noffs),
      n' = 0     - pointer to x(0,0) = d(noffs),
      k = 0      - pointer to y(0).
  - For each batch, k and n' advance by Nx,
      k <- k + Nx,
      n' <- n' + Nx.
  - When the index n' for x(j,n') advances beyond Nx, we bring n' back into
    range by subtracting Nx from it and incrementing the batch number,

Buffer allocation:
  The buffer holds the filter memory (lmem) and the input data (Nx). The output
  output data overlay the input data.
*/

  lmem = 2 * (Nsec + 1);
  Nxmax = NBUF - lmem;
  NyO = (NsampO - 1) * Nsub + 1;

/* Main processing loop */
  /* if (n < noffs), processing warm-up points, no output */
  VRdZero(x, lmem);
  n = MINV(noffs, MAXV(0, noffs - MAXWUP));
  k = 0;
  while (k < NyO) {

/* Read the input data into the input buffer */
    if (n < noffs)
      Nx = (int) MINV(Nxmax, noffs - n);
    else
      Nx = (int) MINV(Nxmax, NyO - k);
    AFdReadData(AFpI, n, &x[lmem], Nx);

/* Convolve the input samples with the filter response */
    FIdFiltIIR(&x[lmem-2], x, Nx, h, Nsec);

/* Write the output data to the output audio file */
    if (n >= noffs) {
      if (Nsub == 1)
        AFdWriteData(AFpO, &x[2], Nx);
      else
        FA_writeSubData(AFpO, k, Nsub, &x[2], Nx);
      k = k + Nx;
    }
    n = n + Nx;

/* Update the filter memory */
    VRdShift(x, lmem, Nx);
  }
}

static void
FA_writeSubData(AFILE *AFp0, long int k, int Nsub, const double x[], int Nx)

{
  double xs[(NBUF+1)/2];  /* Only used for sub-sampling, i.e. when Nsub > 1 */
  int i, ist, m;

  ist = ICEILV(k, Nsub)*Nsub - k;
  for (m = 0, i = ist; i < Nx; ++m, i += Nsub) {
    xs[m] = x[i];
  }
  AFdWriteData(AFp0, xs, m);
}
