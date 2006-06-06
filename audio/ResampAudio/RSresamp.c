/*------------- Telecommunications & Signal Processing Lab -------------
                           McGill University

Routine:
  void RSresamp (AFILE *AFpI, AFILE *AFpO, double Sratio, long int Nout,
                 double toffs, const struct Fpoly_T *PF, FILE *fpinfo)

Purpose:
  Resample data from an audio file

Description:
  This routine resamples data from one audio file and writes it to another
  audio file.

Parameters:
   -> AFILE *AFpI
      Audio file pointer for the input audio file
   -> AFILE *AFpO
      Audio file pointer for the output audio file
   -> double Sratio
      Ratio of output sampling rate to input sampling rate
   -> long int Nout
      Number of output samples to be calculated
   -> double toffs
      Time offset into the input data.  This is the fractional sample value
      corresponding to the first filter coefficient.
   -> const struct Ffilt_T *Filt
      Filter coefficient structure
   -> FILE *fpinfo
      File pointer for printing filter information.  If fpinfo is not NULL,
      information about the filter is printed on the stream selected by fpinfo.

Author / revision:
  P. Kabal  Copyright (C) 2005
  $Revision: 1.18 $  $Date: 2005/02/01 04:23:08 $

----------------------------------------------------------------------*/

#include <assert.h>
#include <limits.h>

#include "ResampAudio.h"

#define MINV(a, b)	(((a) < (b)) ? (a) : (b))

#define MAXBUF	8192		/* Buffer size (per channel) */


void
RSresamp (AFILE *AFpI, AFILE *AFpO, double Sratio, long int Nout, double toffs,
	  const struct Fpoly_T *PF, FILE *fpinfo)

{
  int lmem, NbufI, NbufO, Nx, Ny, Nxm, i, j;
  long int k, Nchan, LNs;
  double Ds;
  double *buf, *x, *y, *xc, *yc;
  struct Tval_T Ts;

/*
  General interpolation:

  This routine implements both conventional interpolation and general
  interpolation.  In the general case, an interpolating filter is used to
  generate two points which bracket the desired output point position.  The
  process can be viewed as follows.

       |-----|     |------|     |-----|     |-------|    |-----|
  ---->| ^Ir |---->| H(z) |---->| D/C |---->| inter |--->| C/D |---->
       |-----|     |------|     |-----|     |-------|    |-----|
   x(n)       xi(k)        yi(k)       yc(t)         y(t)        y(m)
      sampling       h(n)        D to C      linear      sample
      rate 1:Ir     filter                 interpolation

  As an example consider changing the sampling rate from 8000 Hz to 44100 Hz
  using an interpolation filter with Ir=10.
  (1) Upsample x(n) by inserting Ir-1 zeros between each sample to form xi(k).
      The signal xi(k) has a sampling rate of 80 kHz, but with a repeating
      spectrum every 8 kHz.
  (2) Filter with H(z).  This passes frequencies below 4000 Hz and suppresses
      frequencies above 4000 Hz.  The resulting signal yi(k) has a repeating
      spectrum every 80 kHz.  The images at 8, 16, ..., 72 kHz have been
      suppressed.
  (3) Form impulses with the discrete-to-continuous converter.  The resulting
      continuous-time signal yc(t) has a repeating spectrum every 80 kHz.
  (4) Linearly interpolate between the digital sample values.  This process can
      be modelled as filtering with a continuous-time filter with a triangular
      impulse response.  The linear interpolating filter has a (sin(x)/x)^2
      frequency response with zeros at multiples of 80 kHz.  The zero response
      points are coincident with the images of the input spectrum of yc(t).
      The output signal y(t) will have energy mainly around zero.
  (5) Resample the continuous-time output at 44100 Hz.

  We can move the D/C block ahead of the filter H(z) if we replace h(n) with
  a transveral filter h(t).  Then we can form a composite continuous-time
  filter g(t) by convolving the linear interpolation filter with h(t).  The
  impulse response of the resulting filter is that of h(n) with linear
  interpolation between the sample values.

  The frequency response of g(t) can be obtained by multiplying H(w) by the
  (sin(x)/x)^2 response of the linear interpolator.  H(w) has a repeating
  spectrum every 80 kHz.  The first zero of the linear interpolating filter
  response occurs at 80 kHz.  This tends to suppress the image of the response
  at 80 kHz.  This suppression is incomplete however since the image extends
  4 kHz on each side of 80 kHz.  The worst suppression is for a frequency 4 kHz
  below 80 kHz.  In addition, the desired frequencies surrounding zero are
  subject to a slight lowpass effect, this effect being worst at 4 kHz.

  Consider different interpolation ratios Ir.  For large Ir, the images are
  confined to a relatively small distance from the zero of the linear
  interpolating filter and so are well suppressed.  We can measure the
  suppression at the bottom of the first image.  The suppression at other
  image frequencies will be better.  The following table shows the results
  for an input samplng rate fs and various values of Ir.

     Ir      attenuation
           freq fs-fs/(2Ir)
     6        42 dB
    10        51 dB
    20        64 dB
    24        67 dB

  Note that if we use sample-and-hold instead of linear interpolation (i.e.
  use the value of the closest bracketing sample), the attenuation will be
  half of these dB values.
*/

/*
  For the k'th output sample, the beginning of the filter is aligned with
  the input array at time t(k) (measured in samples of the input data),
    t(k) = t(0) + k * dt,
  where dt is the spacing between output samples.  The data is processed frame
  by frame.  Consider frame j,
    k[j] - first output sample in frame j
  The first time point in a frame is
    ts[j] = t(k[j])
          = t(0) + k[j] * dt.

  The time interval between input samples will, if possible, be expressed as a
  ratio of integers,
    dt = 1 / Sratio = Ds / Ns.
  If dt cannot be expressed as a ratio of integers of reasonable size, we set
  Ds = 1 and set Ns to be a floating point value.  If dt is rational, we adopt
  a time update procedure that prevents round-off error from accumulating.

  Filtering:

  Each output sample is created by filtering with an interpolation filter
  (interpolation factor Ir).  The interpolation filter is aligned on a position
  which is a multiple of 1/Ir.  The filter is used twice, once to the left of
  the desired position and once to the right of the desired position.

  The input signal x(.) is the data in the file, with x(0) corresponding to the
  first data value in the file.  Conceptually a new increased rate signal,
  xi(.), is formed from the input signal x(.) by inserting Ir-1 zeros between
  each element of x(.),
    xi(l*Ir) = x(l),
  with other elements of xi(.) being zero.

  Indexing:
    - m is an offset into xi(.), referring to sample xi(m).
    - l is an offset into x(), referring to sample x(l).
    - m and l are related by m=l*Ir+mr  where l=floor(m/Ir), mr=m-Ir*l,
      0 <= mr < Ir.
  Each output point is formed as the dot product of the filter vector
  {h[0],...,h[Ncof-1]} with the data elements {xi(m),...,xi(m-Ncof+1)}.
  Because of the zeros in xi(.), the dot product can use the vectors
  {h[mr],h[mr+Ir],...} and {x(l),x(l-1),...}.

  An output sample will be calculated at position Ir*t in the signal xi(.).
  This position will be bracketed by ml(t) and mu(t) where ml(t) and mu(t) are
  integer values.
    ml(t) = floor(Ir*t) <= Ir*t < ml(t) + 1 = mu(t)

  The value ml(t) can be expressed as
    ml(t) = Ir*ll + mlr,
  where
    ll = floor(t), mlr = floor(Ir*t) - Ir*ll.
  The value mlr is the filter offset for the convolution; filter element h[mlr]
  is aligned with input sample x(ll).

  Similarly mu(t) can be expressed as
    mu(t) = Ir*lu + mur.
  This decomposition can be obtained from that for ml(t) by setting mur equal
  to mlr+1, modulo Ir.  Then lu is set to ll, but incremented if mlr wraps
  around.

  Buffer sizes:
  The input buffer must contain lmem = Ncmax-1 past values for the filter
  memory.  The first output sample in a frame corresponds to time ts,
    ml(ts) = Ir*lsl + mslr .
  The last sample in a frame corresponds to time tf = ts + (Ny-1)*dt, where
  Ny is the number of output samples,
    ml(tf) = Ir*lfl + mflr .
  The value mu(tf) may involve access to data element lfl+1 (if mflr is equal
  to Ir-1).

  The number of input samples needed in a frame with Ny output samples is then
  bounded as follows,
    Nx <= lfl+1 - lsl + 1
        = lfl - lsl + 2
        = floor(ts + (Ny-1)/Sratio) - floor(ts) + 2
        < (Ny-1)/Sratio + 3 .

  Let the buffer size be MAXBUF,
    Nx + lmem + Ny < (Ny-1)/Sratio + 3 + lmem + Ny
                   <= MAXBUF
  Then we have a limit on the size of Ny
    Ny <= (Sratio * (MAXBUF - 3 - lmem) + 1) / (1 + Sratio)

  After finishing processing a buffer, lmem input samples have to be shifted
  downwards.  For long buffers, this shift is amortized over more samples.
  However long buffers imply a longer time delay before samples appear at the
  output.
*/

/* Split up the buffer */
  lmem = PF->Ncmax - 1;
  NbufO = (int) ((Sratio * (MAXBUF - 3 - lmem) + 1) / (1.0 + Sratio));
  NbufI = MAXBUF - NbufO;
  assert (NbufO < MAXBUF);

/* Allocate memory for the buffers */
/* Multi-channel data:
   The discussion above on filtering has assumed single channel data.  For
   multi-channel data, each channel is filtered separately with its own
   memory.

   For each input channel:
   - Extract the data from one channel from the channel-interleaved signal
     x, forming xc;
   - Filter one channel to form the output signal yc;
   - Place the output in the channel-interleaved vector y. 
*/
  assert (AFpI->Nchan == AFpO->Nchan);
  Nchan = AFpI->Nchan;
  if (Nchan == 1) {
    buf = (double *) UTmalloc ((NbufI + NbufO) * sizeof (double));
    x = buf;			/* x has NbufI elements */
    y = buf + NbufI;		/* y has NbufO elements */
    xc = x;			/* xc is an alias for x */
    yc = y;			/* yc is an alias for y */
  }
  else {
    buf = (double *) UTmalloc ((NbufI + NbufO) * (Nchan + 1) * sizeof (double));
    x = buf;			/* x has Nchan*NbufI elements */
    xc = buf + Nchan * NbufI;	/* xc has NbufI elements */
    yc = xc + NbufI;		/* yc has NbufO elements */
    y = yc + NbufO;		/* y has Nchan*NbufO elements */
  }

/*
  Express the sampling rate ratio in rational form
    Sratio = Ns / Ds
  The maximum value for Ds is determined by the need to avoid overflow of the
  calculation of the quantity dsr + ds + Ds*Ny,
    dsr + ds + Ds*Ny < Ns + Ds*Ny <= LMAX.
  Using Sratio = Ns/Ds, this can be expressed as
    Ds <= LMAX / (Sratio + Ny) .
*/
  RSratio (Sratio, PF->Ir, &LNs, &Ds, LONG_MAX,
	   (long int) (LONG_MAX / (Sratio + NbufO)), fpinfo);

  /*
    Express the time offset in the form,
      toffs = ls + (dsr + ds) / Ns,
  */
  RSexpTime (toffs, LNs, &Ts);

  RSrefresh (AFpI, 0L, x, 0);		/* initialize the buffer */
  k = 0;
  while (k < Nout) {

    Ny = (int) MINV (NbufO, Nout - k);
    Nx = (int) ((Ds * (Ny-1)) / LNs) + 3;
    Nxm = Nx + lmem;
    assert (Nxm <= NbufI);

    /* Refresh the input buffer */
    RSrefresh (AFpI, (Ts.n - lmem)*Nchan, x, Nxm*Nchan);

    /* Process one channel at a time */
    for (i = 0; i < Nchan; ++i) {

      if (Nchan > 1) {
	for (j = 0; j < Nxm; ++j)
	  xc[j] = x[j*Nchan + i];
      }

      /* Generate the output samples for channel i */
      RSinterp (xc, Nxm, yc, Ny, Ds, &Ts, PF);

      if (Nchan > 1) {
	for (j = 0; j < Ny; ++j)
	  y[j*Nchan + i] = yc[j];
      }
    }

    /* Write the output data */
    AFdWriteData (AFpO, y, Ny*Nchan);

    /* Update the sample pointers */
    k += Ny;
    RSincTime (&Ts, Ds*Ny);
  }

  /* Deallocate the storage */
  UTfree (buf);

  return;
}
