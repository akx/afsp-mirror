/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void FAfiltFIR (AFILE *AFpI, AFILE *AFpO, long int NsampO, double h[],
                  int Ncof, long int loffs)

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
   -> long int loffs
      Data offset into the input data for the first output point

Author / revision:
  P. Kabal  Copyright (C) 2005
  $Revision: 1.15 $  $Date: 2005/02/01 13:24:07 $

-------------------------------------------------------------------------*/

#include <libtsp.h>
#include "FiltAudio.h"

#define MINV(a, b)	(((a) < (b)) ? (a) : (b))

#define NBUF	5120


void
FAfiltFIR (AFILE *AFpI, AFILE *AFpO, long int NsampO, const double h[],
	   int Ncof, long int loffs)

{
  double x[NBUF];
  int lmem, Nxmax, Nx;
  long int l, k;

/*
   Notes:
   - The input signal d(.) is the data in the file, with d(0) corresponding to
     the first data value in the file.
   - Indexing: l is an offset into d(), referring to sample d(l).
   - Each output point is formed as the dot product of the filter vector
     {h[0],...,h[Ncof-1]} with the data elements {d(l),...,d(l-Ncof+1)}.
*/

/* Batch processing
   - The data will be processed in batches by reading into a buffer x(.,.).
     The batches of input samples will be of equal size, Nx, except for the
     last batch.  For batch j,
       x(j,l') = d(loffs+j*Nx+l'), for 0 <= l' < Nx,
   - The k'th output point y(k) is calculated at position d(loffs+k).
       y(k) --> h[0] <==> d(l),    where l=loffs+k
                h[0] <==> x(j,l').
   - For batch j=0,
       l = loffs  - pointer to d(loffs),
       l' = 0     - pointer to x(0,0) = d(loffs),
       k = 0      - pointer to y(0).
   - For each batch, k and l' advance by Nx,
       k <- k + Nx,
       l' <- l' + Nx.
   - When the index l' for x(j,l') advances beyond Nx, we bring l' back
     into range by subtracting Nx from it and incrementing the batch number,
*/

/* Buffer allocation
   Let the total buffer size be Nb.  This is allocated to filter memory (lmem)
   and the input data (Nx).  The output data will overlay the input data.
*/
  lmem = Ncof - 1;
  Nxmax = NBUF - lmem;
  if (Nxmax <= 0)
    UThalt ("FAfiltFIR: Too many filter coefficients");

/* Prime the array */
  l = loffs;
  AFdReadData (AFpI, l - lmem, &x[0], lmem);

/* Main processing loop */
  k = 0;
  while (k < NsampO) {

/* Read the input data into the input buffer */
    Nx = (int) MINV (Nxmax, NsampO - k);
    AFdReadData (AFpI, l, &x[lmem], Nx);
    l = l + Nx;

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
