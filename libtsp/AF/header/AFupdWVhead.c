/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int AFupdWVhead (AFILE *AFp)

Purpose:
  Update header information in a RIFF WAVE file

Description:
  This routine updates the data length fields of a RIFF WAVE file.  The file is
  assumed to have been opened with routine AFopnWrite.  This routine also writes
  information chunks at the end of the file.

Parameters:
  <-  int AFupdWVhead
      Error code, zero for no error
   -> AFILE *AFp
      Audio file pointer for an audio file opened by AFopnWrite

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.50 $  $Date: 2017/05/23 11:50:10 $

-------------------------------------------------------------------------*/

#include <assert.h>
#include <setjmp.h>
#include <string.h>

#include <libtsp.h>
#include <AFpar.h>
#include <libtsp/AFdataio.h>
#include <libtsp/AFheader.h>
#include <libtsp/WVpar.h>

/* setjmp / longjmp environment */
extern jmp_buf AFW_JMPENV;

/* Local functions */
#ifdef WV_TEXT_CHUNKS
static int
AF_setTextChunks (struct WV_CkRIFF *CkRIFF, struct AF_info *TInfo, char *Text);
static int
AF_wrTextChunks (FILE *fp, const struct WV_CkRIFF *CkRIFF);
#endif    /* WV_TEXT_CHUNKS */


int
AFupdWVhead (AFILE *AFp)

{
  int PCM;
  long int SLoffset, Nbytes, Ldata;
  UT_uint4_t val;

/* Set the long jump environment; on error return a 1 */
  if (setjmp (AFW_JMPENV))
    return 1;   /* Return from a header write error */

/* Add a padding byte (if needed) to the sound data; this padding byte is not
   included in the data chunk ckSize field, but is included in the RIFF chunk
   ckSize field
*/
  Ldata = AF_DL[AFp->Format] * AFp->Nsamp;
  Nbytes = AFp->Start + Ldata;
  Nbytes += WRPAD (AFp->fp, Nbytes, ALIGN); /* Write pad byte */

/* Check if the header length fields need updating */
if (AFp->Nframe != AF_NFRAME_UNDEF && AFp->Isamp  == AFp->Nframe * AFp->Nchan)
  return 0;

/* Update the "RIFF" chunk ckSize field (at the beginning of the file) */
  if (AFseek (AFp->fp, 4L, NULL)) /* Back at the beginning of the file */
    return 1;
  val = (UT_uint4_t) (Nbytes - 8);
  WHEAD_V (AFp->fp, val, DS_EL);

/* Header layout:
               RIFF  fmt   fact ...  data
               WAVE
   PCM         0-11  12-35   -      Start-8...
   non-PCM     0-11  12-37 38-49    Start-8...
   PCM-Ext     0-11  12-59   -      Start-8...
   non-PCM-Ext 0-11  12-59 60-71    Start-8...
*/
  PCM = (AFp->Format == FD_UINT8 || AFp->Format == FD_INT16 ||
         AFp->Format == FD_INT24 || AFp->Format == FD_INT32);
  SLoffset = 0;
  if (! PCM) {    /* Float/double or mu-law/A-law */
    if (AFp->Ftype == FT_WAVE)
      SLoffset = 46;
    else
      SLoffset = 68;
  }

/* Update the "fact" chunk SampleLength field */
  if (SLoffset > 0) {
    if (AFseek (AFp->fp, SLoffset, NULL))
      return 1;
    val = (UT_uint4_t) (AFp->Nsamp / AFp->Nchan);
    WHEAD_V (AFp->fp, val, DS_EL);
  }

/* Update the "data" chunk ckSize field */
  if (AFseek (AFp->fp, AFp->Start - 4, NULL))
    return 1;
  val = (UT_uint4_t) Ldata;
  WHEAD_V (AFp->fp, val, DS_EL); /* Number of data bytes */

  return 0;
}
