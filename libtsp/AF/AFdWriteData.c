/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int AFdWriteData (AFILE *AFp, const double Dbuff[], int Nval)

Purpose:
  Write data to an audio file (double input values)

Description:
  This routine writes a specified number of samples to an audio file.  The
  double input data is converted to the audio file data representation.  The
  file data representation is set on opening the file with routine AFopnWrite.
  This routine writes data sequentially to the file.  A warning message is
  printed if the input values exceed the dynamic range of the file data
  representation.

Parameters:
  <-  int AFdWriteData
      Number of samples written
   -> AFILE *AFp
      Audio file pointer for an audio file opened by AFopnWrite
   -> const double Dbuff[]
      Array of doubles with Nval samples
   -> int Nval
      Number of samples to be written

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.14 $  $Date: 2017/06/27 18:27:25 $

-------------------------------------------------------------------------*/

#include <assert.h>
#include <stdlib.h>   /* EXIT_FAILURE */

#include <libtsp.h>
#include <AFpar.h>
#include <libtsp/AFdataio.h>
#include <libtsp/AFmsg.h>

#define MAXV(a, b)  (((a) > (b)) ? (a) : (b))

/* Writing routines */
static int
(*AF_Write[AF_NFD])(AFILE *AFp, const double Dbuff[], int Nval) =
                    {NULL,     AFdWrAlaw, AFdWrMulaw, AFdWrMulawR,
                     AFdWrU1,  AFdWrI1,   AFdWrI2,    AFdWrI3,
                     AFdWrI4,  AFdWrF4,   AFdWrF8,    AFdWrTA,
                     AFdWrTA};


int
AFdWriteData (AFILE *AFp, const double Dbuff[], int Nval)

{
  int Nw;
  long int Novld;

  assert (AFp->Op == FO_WO);
  assert (AF_Write[AF_NFD-1] != NULL);

/* The file writing routines write scaled data to the file.  They write to the
   current file position.  They use the following AFp fields:
     AFp->fp - file pointer
     AFp->Swapb - data swap flag
     AFp->ScaleF - data scaling factor
     AFp->Ovld - overload counter (updated)

  This routine updates the following AFp values
    AFp->Error - error flag
    AFp->Isamp - current data sample.  This value is incremented by the
      number of samples written.
    AFp->Nsamp - last sample (updated if AFp->Isamp is beyond it)
*/

/* Transfer data to the audio file */
  Novld = AFp->Novld; /* Save the value before writing */
  Nw = (*AF_Write[AFp->Format]) (AFp, Dbuff, Nval);
  AFp->Isamp += Nw;
  AFp->Nsamp = MAXV (AFp->Isamp, AFp->Nsamp);

/* Check for an error */
  if (Nw < Nval) {
    UTsysMsg ("AFdWriteData: %s", AFM_WriteErr);
    if (AFopt.ErrorHalt)
      exit (EXIT_FAILURE);
    AFp->Error = AF_IOERR;
  }

/* Check for overloads (print a message the first time only) */
  if (Novld == 0L && AFp->Novld != 0L)
    UTwarn ("AFdWriteData - %s", AFM_Clip);

  return Nw;
}
