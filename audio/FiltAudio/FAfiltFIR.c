/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void FAfiltFIR (AFILE *AFpI, AFILE *AFpO, long int NsampO, double h[],
                  int Ncof, long int noffs)

Purpose:
  Filter an audio file with an FIR filter

Description:
  This routine convolves the data from the input audio file with an FIR filter
  response.

Parameters:
   -> AFILE *AFpI
      Audio file pointer for the input audio file
   -> AFILE *AFpO
      Audio file pointer for the output audio file
   -> long int NsampO
      Number of output samples to be calculated
   -> double h[]
      Array of Ncof FIR filter coefficients
   -> int Ncof
      Number of filter coefficients
   -> long int noffs
      Data offset into the input data for the first output point

Author / revision:
  P. Kabal  Copyright (C) 2018
  $Revision: 1.17 $  $Date: 2018/11/12 18:03:36 $

-------------------------------------------------------------------------*/

#include <libtsp.h>
#include "FiltAudio.h"

#define MINV(a, b)  (((a) < (b)) ? (a) : (b))


void
FAfiltFIR (AFILE *AFpI, AFILE *AFpO, long int NsampO, const double h[],
           int Ncof, long int noffs)

{
  double x[NBUF];
  int lmem, Nxmax, Nx;
  long int n, k;

/*
Notes:
  - The input signal d(.) is the data in the file, with d(0) corresponding to
    the first data value in the file.
  - Indexing: n is an offset into d(), referring to sample d(n).
  - Each output point is formed as the dot product of the filter vector
    {h[0],...,h[Ncof-1]} with the data elements {d(n),...,d(n-Ncof+1)}.

Batch processing:
  - The data will be processed in batches by reading into a buffer x(.,.).  The
    batches of input samples will be of equal size, Nx, except for the last
    batch.  For batch j,
      x(j,n') = d(noffs+j*Nx+n'), for 0 <= n' < Nx.
  - The k'th output point y(k) is calculated at position d(noffs+k).
      y(k) --> h[0] <==> d(n),    where n=noffs+k
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
  The buffer holds the filter memory (lmem) and the input data (Nx).  The output
  data overlay the input data.
*/

  lmem = Ncof - 1;
  Nxmax = NBUF - lmem;

/* Prime the array */
  n = noffs;
  AFdReadData (AFpI, n - lmem, &x[0], lmem);

/* Main processing loop */
  k = 0;
  while (k < NsampO) {

/* Read the input data into the input buffer */
    Nx = (int) MINV (Nxmax, NsampO - k);
    AFdReadData (AFpI, n, &x[lmem], Nx);
    n = n + Nx;

/* Convolve the input samples with the filter response */
    FIdConvol (x, x, Nx, h, Ncof);

/* Write the output data to the output audio file */
    AFdWriteData (AFpO, x, Nx);
    k = k + Nx;

/* Update the filter memory */
    VRdShift (x, lmem, Nx);
  }

  return;
}
