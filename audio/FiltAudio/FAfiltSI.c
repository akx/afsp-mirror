/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void FAfiltSI(AFILE *AFpI, AFILE *AFpO, long int NsampO, const double h[],
                int Ncof, int Nsub, int Ir, long int moffs)

Purpose:
  Filter an audio file with an FIR filter (sample rate change)

Description:
  This routine convolves a filter response with a rate-increased data sequence.
  Conceptually Ir-1 zeros are inserted between each sample from the input audio
  file to form this rate-increased data sequence. The filtered output is
  subsampled by a factor Nsub - only every Nsub'th output sample is calculated
  and stored into the output audio file.

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
  P. Kabal  Copyright (C) 2020
  $Revision: 1.24 $  $Date: 2020/11/23 18:31:11 $

-------------------------------------------------------------------------*/

#include <assert.h>
#include <libtsp.h>
#include "FiltAudio.h"

#define MINV(a, b)  (((a) < (b)) ? (a) : (b))
#define ICEILV(n, m)  (((n) + ((m) - 1)) / (m)) /* int n>=0, int m>0 */


void
FAfiltSI(AFILE *AFpI, AFILE *AFpO, long int NsampO, const double h[],
         int Ncof, int Nsub, int Ir, long int moffs)

{
  double Fbuf[NBUF];
  double *xmem, *x, *y;
  long int lmem, Nb;
  long int Nxmax, Nymax, Nx, Ny;
  long int noffs;
  long int mp, n, np, mro, mrn, k;
  long int NCOF, NSUB, IR;

/*
Notes:
  - The input signal d(.) is the data in the file, with d(0) corresponding to
    the first data value in the file.
  - Conceptually a new increased rate signal, di(.), is formed from the input
    signal d(.) by inserting Ir-1 zeros between each element of d(.),
      di(n*Ir) = d(n), with other elements of di(.) being zero.
  - Indexing:
     - m is an offset into di(.), referring to sample di(m).
     - n is an offset into d(.), referring to sample d(n).
     - m and n are related by m=n*Ir+mrn, where n=floor(m/Ir), mrn=m-n*Ir,
       0 <= mrn < Ir.
  - Each output point is formed as the dot product of the filter vector
    {h[0],...,h[Ncof-1]} with the data elements {di(m),...,di(m-Ncof+1)}.
    Because of the zeros in di(.), the dot product can use the vectors
    {h[mrn],h[mrn+Ir],...} and {d(n),d(n-1),...}. This dot product involves at
    most lmem+1 terms, where lmem=floor((Ncof-1)/Ir).
  - Only a subsampled set of the output values will be calculated. The first
    output sample y(0) will calculated with
      y(0) -->   h[0] <==> di(moffs)
               h[mro] <==> d(noffs), where noffs=floor(moffs/Ir),
                                             mro=moffs-noffs*Ir.            (1)
  - The k'th output sample will be calculated at position di(moffs+k*Nsub),
      y(k) -->   h[0] <==> di(mk), with mk=moffs+k*Nsub
               h[mrk] <==> d(nk),  with nk=floor(mk/Ir), mrk=mk-nk*Ir.

Batch processing:
  The data will be processed in frames by reading into a buffer x(j,n'). The
  frames of input samples will be of equal size Nx, except for the last batch.
  - For batch j,
      x(j,n') = d(j*Nx+n'+noffs), for 0 <= n' < Nx
  - Define a virtual buffer at the interpolated sampling rate indexed by batch
    number
      xi(j,m') = di((j*Nx+noffs)*Ir+m'),  for 0 <= m' < Nx*Ir
    For the first frame j=0 (see (1))
      xi(0,mro) = di(noffs*Ir+mro)
                = di(moffs)
  - Since d(.) and di() are related,
      x(j,n') = di((j*Nx+n'+noffs)*Ir)

Alignment for the first frame:
  For the first frame, the the output samples are taken at m'=mro+k*Nsub.

              |         |         |         |       /      |
              ^         ^         ^         ^              ^
  x(0,n')  n' 0         1         2         3       /      Nx-1
     d(n)  n  noffs               noffs+2

              |.........|.........|.........|...    /   ...|........
              ^   ^     ^         ^         ^              ^
  xi(0,m') m' 0   mro   Ir        2Ir       3Ir           (Nx-1)Ir
     di(m) m      moffs

                  |.....|.....|.....|.....|.....   /    .|.....|
                  ^     ^     ^     ^     ^              ^     ^
           k*Ir   0     Nsub  2Nsub 3Nsub 4Nsub    /           (Ny-1)Nsub
    y(k)   k      0     1     2     3     4              Ny-2  Ny-1

  Initialize
    m' = mro
  Given Nx input samples in the frame, the last output sample for the this frame
  satisfies
    m'+(Ny-1)*Nsub <= Nx*Ir - 1
                Ny <= (Nx*Ir-1-m')/Nsub + 1                                  (2)
  Choosing the largest integer Ny satisfying this inequality
    Ny = floor((Nx*Ir-1-m')/Nsub + 1
       = ceil((Nx*Ir-m')/Nsub)                                               (3)

Last frame:
  For the last frame, Ny will be set to the number of remaining output samples.
  Nx will then be chosen to input only the required input samples. Rewriting
  (2) to express Nx in terms of Ny,
    Nx >= ((Ny-1)*Nsub+m'+1) / Ir
  Choosing the smallest value that satisfies this inequality,
    Nx = ceil(((Ny-1)*Nsub+m'+1) / Ir)

Buffer Allocation:
  The allocated buffer will contain the filter memory (length lmem), and has
  room for Nxmax input samples and Nymax output samples. Let the buffer size be
  Nb,
    Nxmax + Nymax <= Nb
  The goal is to choose Nxmax and Nymax to allow the largest number of output
  samples to be processed in each frame. The fraction of the data buffer space
  allocated to Nxmax is about Nsub/(Nsub+Ir) and the fraction to Nymax is about
  Ir/(Nsub+Ir). Nxmax goes to zero for large Ir/Nsub ratios and Nymax goes
  towards zero for large Nsub/Ir ratios.

  Let Ny be chosen to be the largest value of Ny for the largest number of input
  samples Nxmax. Then from (3)
    Nxmax + Ny <= Nb, where Ny = ceil((Nxmax*Ir-m')/Nsub).
  The largest value of Ny occurs for m'=0
    Nymax = ceil((Nxmax*Ir)/Nsub)
  Then
              Nxmax + ceil(Nxmax*Ir/Nsub) <= Nb
    Nxmax + floor((Nxmax*Ir+Nsub-1)/Nsub) <= Nb
         Nxmax + floor((Nxmax*Ir-1)/Nsub) <= Nb-1
                Nxmax + (Nxmax*Ir-1)/Nsub <= Nb-1
                        Nxmax*(Nsub+Ir)-1 <= (Nb-1)*Nsub
                          Nxmax*(Nsub+Ir) <= (Nb-1)*Nsub+1
                                    Nxmax <= ((Nb-1)*Nsub+1)/(Nsub+Ir)

  The filtering will be processed in chunks of Nx input samples, except for a
  partial last frame. Let m' be first output position in the frame (mro for the
  first frame). After filtering a frame of Ny output values, the value of m' is
  reset,
    m' <- m' + Ny*Nsub - Nx*Ir
  Choosing the largest value of Ny given in (3), the updated value of m' is
    m' <- m' + ceil((Nx*Ir-m')/Nsub)*Nsub - Nx*Ir
       >= m' + Nx*Ir - m' - Nx*Ir
       >= 0
  From (2)
    m' <- m' + floor((Nx*Ir-m'+Nsub-1)/Nsub)*Nsub - Nx*Ir
       <= m' + Nx*Ir - m' + Nsub - 1 - Nx*Ir
        = Nsub - 1
  These results together show that if Ny is as large as possible for a given Nx,
  the updated value of m' satisfies
    0 <= m' < Nsub
  This result does not depend on the initial value of m', but requires that Ny
  is chosen to be as large as possible for a given Nx.
*/

/* All integer variables are long, except Ncof, Nsub and Ir  */
  NSUB = Nsub;
  IR = Ir;
  NCOF = Ncof;

  lmem = (NCOF-1) / IR;       /* floor */
  Nb = NBUF - lmem;           /* space for xmem, x and y */
  Nxmax = ((Nb-1)*NSUB + 1) / (NSUB+IR); /* floor if Nxmax > 0 */
  Nymax = ICEILV(Nxmax*IR, NSUB);
  assert(Nxmax + Nymax <= Nb);

  /* Set up the buffer pointers */
  xmem = Fbuf;
  x = xmem + lmem;
  y = x + Nxmax;

/* Initialization */
  noffs = moffs / IR;       /* calculate noffs, mro => moffs = noffs*IR + m */
  mro = moffs - noffs*IR;   /* moffs < 0 => -IR < mro <= 0  */
  if (mro < 0) {
    noffs = noffs - 1;      /* True floor */
    mro = mro + IR;         /* True modulus */
  }

/* Prime the input data array with the filter memory */
  n = noffs;
  AFdReadData(AFpI, n - lmem, xmem, (int) lmem);

/* Main processing loop */
  k = 0;
  mp = mro;
  while (k < NsampO) {

/* Choose Nx and Ny */
   Nx = Nxmax;
   Ny = ICEILV(Nx*IR - mp, NSUB);
   if (Ny > NsampO - k) {     /* Last partial frame */
     Ny = NsampO - k;
     Nx = ICEILV((Ny-1)*NSUB + mp + 1, IR);
   }
   assert( Ny >= 0 && Ny <= Nymax);

/* Read the input data into the input buffer */
    AFdReadData(AFpI, n, x, (int) Nx);
    n = n + Nx;

/* Set up the filter offset */
    np = mp / IR;     /* floor */
    mrn = mp - np*IR;

/* Convolve the input samples with the filter response */
/* &xmem[np] = &x[np-lmem] */
    FIdConvSI(&xmem[np], y, (int) Ny, h, Ncof, (int) mrn, Nsub, Ir);

/* Write the output buffer to the output audio file */
    AFdWriteData(AFpO, y, (int) Ny);
    k = k + Ny;
    if (k == NsampO)
      break;

/* Update the filter memory */
    VRdShift(xmem, (int) lmem, (int) Nxmax);

/* Update the pointer mp */
    mp = mp + Ny*NSUB - Nxmax*IR;
    assert(mp >= 0 && mp < NSUB);
  }
}
