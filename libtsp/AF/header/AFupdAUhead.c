/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int AFupdAUhead (AFILE *AFp)

Purpose:
  Update header information in an AU audio file

Description:
  This routine updates the data length field of an AU format audio file.  The
  file is assumed to have been opened with routine AFopenWrite.

Parameters:
  <-  int AFupdAUhead
      Error code, zero for no error
   -> AFILE *AFp
      Audio file pointer for an audio file opened by AFopenWrite

Author / revision:
  P. Kabal  Copyright (C) 1999
  $Revision: 1.36 $  $Date: 1999/06/04 20:07:54 $

-------------------------------------------------------------------------*/

static char rcsid [] = "$Id: AFupdAUhead.c 1.36 1999/06/04 AFsp-v6r8 $";

#include <setjmp.h>

#include <libtsp.h>
#include <libtsp/AFdataio.h>
#include <libtsp/AFheader.h>
#define AF_DATA_LENGTHS
#include <libtsp/AFpar.h>
#include <libtsp/AUpar.h>

/* setjmp / longjmp environment */
extern jmp_buf AFW_JMPENV;


int
AFupdAUhead (AFILE *AFp)

{
  uint4_t val;
  long int Ldata;

/* Set the long jump environment; on error return a 1 */
  if (setjmp (AFW_JMPENV))
    return 1;		/* Return from a header write error */

  Ldata = AF_DL[AFp->Format] * AFp->Nsamp;

/* Move to the header data length field */
  val = (uint4_t) Ldata;
  if (AFseek (AFp->fp, 8L, NULL))
    return 1;
  WHEAD_V (AFp->fp, val, DS_EB);

  return 0;
}
