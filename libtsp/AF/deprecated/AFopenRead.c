/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  AFILE *AFopenRead (const char Fname[], long int *Nsamp, long int *Nchan,
                     float *Sfreq, FILE *fpinfo)

Purpose:
  Open an audio file for reading

Description:
  Use AFopnRead instead. The difference between the routines is the scaling of
  the data.

Parameters:
  <-  AFILE *AFopenRead
      Audio file pointer for the audio file
   -> const char Fname[]
      Character string specifying the file name
  <-  long int *Nsamp
      Total number of samples in the file (all channels)
  <-  long int *Nchan
      Number of channels
  <-  float *Sfreq
      Sampling frequency
   -> FILE *fpinfo
      File pointer for printing audio file information.  If fpinfo is not NULL,
      information about the audio file is printed on the stream selected by
      fpinfo.

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.95 $  $Date: 2017/04/11 17:50:50 $

-------------------------------------------------------------------------*/

#include <libtsp.h>
#include <AFpar.h>


AFILE *
AFopenRead (const char Fname[], long int *Nsamp, long int *Nchan, float *Sfreq,
            FILE *fpinfo)
{
  AFILE *AFp;
  double SfreqD;

  AFopt.ScaleV = 32768;
  AFp = AFopnRead (Fname, Nsamp, Nchan, &SfreqD, fpinfo);

  *Sfreq = (float) SfreqD;

  return AFp;
}
