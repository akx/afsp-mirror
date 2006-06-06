/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int RSrefresh (AFILE *AFpI, long int offs, double x[], int Nx)

Purpose:
  Read audio data into a buffer, reusing data in the buffer

Description:
  This procedure updates a data buffer with samples beginning at offset offs
  in the audio file.  It keeps internal pointers indicating the sample range
  present in the buffer.  If the offset offs is increasing with each call to
  this routine and the requested data partially overlaps the previous data,
  the old data is moved downward in the buffer and new data from the file is
  appended to the buffer.  In other cases (moving backward for instance), this
  routine reads all of the data from the file with no attempt to reuse any
  overlapping data.

  The internal buffer pointers can be reset by calling this routine with Nx
  equal to zero.  This routine returns the number of new samples read from the
  file.  On reaching the end-of-file, the number of samples read will be
  smaller than the number requested.  The buffer is however padded with zeros
  to its full length.
  
Parameters:
  <-  int RSrefresh
      Number of new samples read from the file
   -> AFILE *AFpI
      Audio file pointer for the audio file
   -> long int offs
      Sample offset into the file
  <-> double x[]
      Data buffer
   -> int Nx
      Buffer length

Author / revision:
  P. Kabal  Copyright (C) 2005
  $Revision: 1.6 $  $Date: 2005/02/01 04:22:28 $

-------------------------------------------------------------------------*/

#include <libtsp.h>
#include "ResampAudio.h"

#define MINV(a, b)	(((a) < (b)) ? (a) : (b))


int
RSrefresh (AFILE *AFpI, long int offs, double x[], int Nx)

{
  static long int lst = 0;
  static long int lnx = 0;
  int Nkeep, Nshift, Nout;

  /* Reset values */
  if (Nx == 0) {
    lst = 0;
    lnx = 0;
    return 0;
  }

  /* Shift useful data that is already in the buffer */
  if (offs < lnx && offs >= lst) {
    Nkeep = (int) MINV (lnx - offs, Nx);
    Nshift = (int) (offs - lst);
    VRdShift (x, Nkeep, Nshift);
  }
  else
    Nkeep = 0;

  /* Read more data */
  Nout = AFdReadData (AFpI, offs + Nkeep, &x[Nkeep], Nx - Nkeep);

  /* Reset the pointers */
  lst = offs;
  lnx = offs + Nx;

  return Nout;
}  
