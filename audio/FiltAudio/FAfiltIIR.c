/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void FAfiltIIR (AFILE *AFpI, AFILE *AFpO, long int NsampO,
                  const double h[][5], int Nsec, int Nsub, long int loffs)

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
   -> long int loffs
      Data offset into the input data for the first output point

Author / revision:
  P. Kabal  Copyright (C) 2005
  $Revision: 1.16 $  $Date: 2005/02/01 13:24:34 $

-------------------------------------------------------------------------*/

#include <libtsp.h>
#include "FiltAudio.h"

#define ICEILV(n,m)	(((n) + ((m) - 1)) / (m))	/* int n,m >= 0 */
#define MAXV(a, b)	(((a) > (b)) ? (a) : (b))
#define MINV(a, b)	(((a) < (b)) ? (a) : (b))

#define NBUF	5120

static void
FA_writeSubData (AFILE *AFp0, long int k, int Nsub, const double x[], int Nx);


void
FAfiltIIR (AFILE *AFpI, AFILE *AFpO, long int NsampO, const double h[][5],
	   int Nsec, int Nsub, long int loffs)

{
  double x[NBUF];
  int mem, Nxmax, Nx;
  long int l, k, NyO;

/*
   Notes:
   - The input signal d(.) is the data in the file, with d(0) corresponding to
     the first data value in the file.
   - Indexing: l is an offset into d(), referring to sample d(l).
*/

/* Batch processing
   - The data will be processed in batches by reading into a buffer x(.,.).
     The batches of input samples will be of equal size, Nx, except for the
     last batch.  For batch j,
       x(j,l') = d(loffs+j*Nx+l'), for 0 <= l' < Nx,
   - The k'th output point y(k) is calculated at position d(loffs+k), that is
     the start of the impulse response, h(0), is aligned with d(loffs+k).
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
   The buffer is allocated to filter memory (mem) and the input data (Nx).
   The output data will overlay the input data.
*/
  mem = 2 * (Nsec + 1);
  Nxmax = NBUF - mem;
  if (Nxmax <= 0)
    UThalt ("%s: %s", PROGRAM, FAM_XIIRSect);
  NyO = (NsampO - 1) * Nsub + 1;

/* Main processing loop */
  /* if (l < loffs), processing warm-up points, no output */
  VRdZero (x, mem);
  l = MINV (loffs, MAXV (0, loffs - MAXWUP));
  k = 0;
  while (k < NyO) {

/* Read the input data into the input buffer */
    if (l < loffs)
      Nx = (int) MINV (Nxmax, loffs - l);
    else
      Nx = (int) MINV (Nxmax, NyO - k);
    AFdReadData (AFpI, l, &x[mem], Nx);

/* Convolve the input samples with the filter response */
    FIdFiltIIR (&x[mem-2], x, Nx, h, Nsec);

/* Write the output data to the output audio file */
    if (l >= loffs) {
      if (Nsub == 1)
	AFdWriteData (AFpO, &x[2], Nx);
      else
	FA_writeSubData (AFpO, k, Nsub, &x[2], Nx);
      k = k + Nx;
    }
    l = l + Nx;

/* Update the filter memory */
    VRdShift (x, mem, Nx);
  }

  return;
}

static void
FA_writeSubData (AFILE *AFp0, long int k, int Nsub, const double x[], int Nx)

{
  double xs[(NBUF+1)/2];  /* Only used for sub-sampling, i.e. when Nsub > 1 */
  int i, ist, m;

  ist = ICEILV(k, Nsub)*Nsub - k;
  for (m = 0, i = ist; i < Nx; ++m, i += Nsub) {
    xs[m] = x[i];
  }
  AFdWriteData (AFp0, xs, m);

  return;
}
