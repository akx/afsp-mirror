/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void FAfiltSI (AFILE *AFpI, AFILE *AFpO, long int NsampO, const double h[],
                 int Ncof, int Nsub, int Ir, long int moffs)

Purpose:
  Filter an audio file with an FIR filter (sample rate change)

Description:
  This routine convolves a filter response with a rate-increased data sequence.
  Conceptually Ir-1 zeros are inserted between each sample from the input audio
  file to form this rate-increased data sequence.  The output is subsampled by
  a factor Nsub; only every Nsub'th output sample is actually calculated and
  stored into the output audio file.

Parameters:
   -> AFILE *AFpI
      Audio file pointer for the input audio file
   -> AFILE *AFpO
      Audio file pointer for the output audio file
   -> long int NsampO
      Number of output samples to be calculated
   -> const double h[]
      Array of Ncof FIR filter coefficients
   -> int Ncof
      Number of filter coefficients
   -> int Nsub
      Subsampling factor
   -> int Ir
      Interpolation factor
   -> long int moffs
      Data offset into the rate-increased data for the first output point

Author / revision:
  P. Kabal  Copyright (C) 2005
  $Revision: 1.20 $  $Date: 2005/02/01 13:25:03 $

-------------------------------------------------------------------------*/

#include <libtsp.h>
#include "FiltAudio.h"

#define MINV(a, b)	(((a) < (b)) ? (a) : (b))
#define MAXV(a, b)	(((a) > (b)) ? (a) : (b))
#define ICEILV(n, m)	(((n) + ((m) - 1)) / (m))	/* int n,m >= 0 */

#define NBUF	5120


void
FAfiltSI (AFILE *AFpI, AFILE *AFpO, long int NsampO, const double h[],
	  int Ncof, int Nsub, int Ir, long int moffs)

{
  double Fbuf[NBUF];
  double *x, *y;
  long int lmem;
  long int Nxmax, Nymax, Nx, Ny;
  long int loffs;
  long int mr;
  long int Ni;
  long int l, lp, mp, k;
  long int NCOF, NSUB, IR;

/*
   Notes:
   - The input signal d(.) is the data in the file, with d(0) corresponding to
     the first data value in the file.
   - Conceptually a new increased rate signal, di(.), is formed from the input
     signal d(.) by inserting Ir-1 zeros between each element of d(.),
       di(l*Ir) = d(l), with other elements of di(.) being zero.
   - Indexing:
       - m is an offset into di(.), referring to sample di(m).
       - l is an offset into d(), referring to sample d(l).
       - m and l are related by m=l*Ir+mr  where l=floor(m/Ir), mr=m-Ir*l,
	 0 <= mr < Ir.
   - Each output point is formed as the dot product of the filter vector
     {h[0],...,h[Ncof-1]} with the data elements {di(m),...,di(m-Ncof+1)}.
     Because of the zeros in di(.), the dot product can use the vectors
     {h[mr],h[mr+Ir],...} and {d(l),d(l-1),...}.  This dot product involves at
     most lmem+1 terms, where lmem=floor((Ncof-1)/Ir).
   - Only a subsampled set of the output values will actually be calculated.
     The k'th output sample will be calculated at position di(moffs+k*Nsub),
       y(k) -->  h[0] <==> di(m), where m=moffs+k*Nsub
                h[mr] <==> d(l),  with l=floor(m/Ir), mr=m-l*Ir.
*/

/* Batch processing
   - The data will be processed in batches by reading into a buffer x(.,.).
     The batches of input samples will be of equal size, Nx, except for the
     last batch.  For batch j,
       x(j,l') = d(loffs+j*Nx+l'), for 0 <= l' < Nx,
               = di(loffs*Ir+j*Ir*Nx+l'*Ir)
   - Introduce a virtual buffer at the increased sampling rate.  This buffer
     will also be indexed by batch number,
       xi(j,m') = di(loffs*Ir+j*Ir*Nx+m'),  for 0 <= m' < Ir*Nx,
        x(j,l') = xi(j,l'*Ir).
   - The k'th output point y(k) is calculated at position di(moffs+k*Nsub).
       y(k) --> h[0] <==> di(m),    where m=moffs+k*Nsub
               h[mr] <==> xi(j,m'),       l=floor(m/Ir), mr-m-l*Ir
               h[mr] <==> d(l),
               h[mr] <==> x(j,l').
   - For batch j=0,
       l = loffs  - pointer to d(loffs),
       m' = mri   - pointer to xi(0,mri) = di(moffs), mri=moffs-Ir*loffs,
       l' = 0     - pointer to xi(0,0) = d(loffs),
       k = 0      - pointer to y(0).
   - The virtual buffer xi(j,m') contains Ir*Nx points for each batch, with
     m' taking on values 0 <= m < Ir*Nx.  If mst' points to the initial
     alignment for the filter within a batch, the number of output points that
     can be calculated within that batch is determined from
       mst' + k*Nsub < Ir*Nx
     Choosing the largest k which satisfies this, Ny = k+1,
       Ny = ceil((Ir*Nx-mst')/Nsub)
   - For each batch, k advances by Ny, while m' advances by Ny*Nsub
       k <- k + Ny,
       m' <- m' + Ny*Nsub.
   - When the index m' for xi(j,m') advances beyond Ir*Nx, we bring m' back
     into range by subtracting Ir*Nx from it and incrementing the batch number,
     In fact, if Nsub is large enough relative to Nx, we may need to increment
     the batch number by more than one to bring m' back into range.
       xi(j+J,m') = xi(j,m'-J*Ir*Nx).
*/

/* Buffer allocation
   Let the total buffer size be Nb.  This is allocated to filter memory (lmem),
   the input data (Nx) and the output data (Ny).  The problem is to choose Nx
   and Ny to utilize as much of the buffer space as possible.
     Ny = ceil((Ir*Nx-mst')/Nsub)
   Let Nb'=Nb-lmem.  The requirement is that Nx + Ny <= Nb'.
     Nx + ceil((Ir*Nx-mst')/Nsub) <= Nb'
     Nsub*Nx + (Ir*Nx-mst') <= Nsub*Nb'
     Nx <= (Nsub*Nb' + mst') / (Nsub + Ir)
   The largest value of Nx which still guarantees enough room for Ny for any
   mst' is given by setting mst' to zero.
*/

/* All integer variables are long, except Ncof, Nsub and Ir  */
  NSUB = Nsub;
  IR = Ir;
  NCOF = Ncof;

  lmem = (NCOF - 1) / IR;		/* floor */
  Nxmax = (NSUB * (NBUF - lmem)) / (NSUB + IR);	/* floor if Nxmax > 0 */
  if (Nxmax <= 0)
    UThalt ("FAfiltSI: No. coeff. and/or interp. ratio too large");
  x = Fbuf;
  y = Fbuf + lmem + Nxmax;
  Nymax = ICEILV (IR * Nxmax, NSUB);
  if (lmem + Nxmax + Nymax > NBUF)
    UThalt ("FAfiltSI: Consistency check on buffer sizes failed");

/* Initialization
   The data buffer is x[.] with x[l'+lmem] = x(j,l') = d(loffs+j*Nx+l')
 */
  loffs = moffs / IR;			/* floor */
  mp = moffs - IR * loffs;
  if (mp < 0) {
    loffs = loffs - 1;
    mp = mp + IR;
  }

/* Prime the array */
  l = loffs;
  AFdReadData (AFpI, l - lmem, &x[0], (int) lmem);

/* Main processing loop */
  k = 0;
  while (k < NsampO) {

/* Set up the filter memory */
    lp = mp / IR;			/* floor */
    mr = mp - IR * lp;

/* Read the input data into the input buffer */
    Ni = ICEILV ((NsampO - k - 1) * NSUB + mp + 1, IR);
    Nx = MINV (Nxmax, Ni);
    AFdReadData (AFpI, l, &x[lmem], (int) Nx);
    l = l + Nx;

/* Convolve the input samples with the filter response */
    Ny = ICEILV (IR * Nx - mp, NSUB);
    if (Ny > Nymax || Ny < 0 || Nx < 0 || mp < 0)
      UThalt ("FAfiltSI: Consistency check on no. values failed");
    FIdConvSI (&x[lp], y, (int) Ny, h, Ncof, (int) mr, Nsub, Ir);
    k = k + Ny;
    mp = mp + NSUB * Ny;

/* Write the output buffer to the output audio file */
    AFdWriteData (AFpO, y, (int) Ny);

/* Update the filter memory */
    VRdShift (x, (int) lmem, (int) Nx);
    mp = mp - IR * Nx;
  }

  return;
}
