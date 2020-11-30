/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int AFupdAUhead(AFILE *AFp)

Purpose:
  Update header information in an AU audio file

Description:
  This routine updates the data length field of an AU format audio file. The
  file is assumed to have been opened with routine AFopnWrite.

Parameters:
  <-  int AFupdAUhead
      Error code, zero for no error
   -> AFILE *AFp
      Audio file pointer for an audio file opened by AFopnWrite

Author / revision:
  P. Kabal  Copyright (C) 2020
  $Revision: 1.46 $  $Date: 2020/11/25 17:54:50 $

-------------------------------------------------------------------------*/

#include <setjmp.h>

#include <libtsp.h>
#include <AFpar.h>
#include <libtsp/AFdataio.h>
#include <libtsp/AFheader.h>
#include <libtsp/AFmsg.h>
#include <libtsp/AUpar.h>

/* setjmp / longjmp environment */
extern jmp_buf AFW_JMPENV;


int
AFupdAUhead(AFILE *AFp)

{
  UT_uint4_t val;
  int Changed;
  long int Ldata;

/* Set the long jump environment; on error return a 1 */
  if (setjmp(AFW_JMPENV))
    return 1;   /* Return from a header write error */

/* Check if the header needs updating */
  Changed = (AFp->Nframe != AF_NFRAME_UNDEF &&
             AFp->Nframe != AFp->Nsamp / AFp->Nchan);
  if (Changed)
    UTwarn("AFupdAUhead - %s", AFM_NsampUpd);

/* Update the Ldata field in the header */
  if (AFp->Nframe == AF_NFRAME_UNDEF || Changed) {
    Ldata = AF_DL[AFp->Format] * AFp->Nsamp;
    val = (UT_uint4_t) Ldata;
    if (AFseek(AFp->fp, 8L, NULL))    /* Move to the header data length field */
      return 1;
    WHEAD_V(AFp->fp, val, DS_EB);
  }

  return 0;
}
