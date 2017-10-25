/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
AFILE *AFwrAUhead (FILE *fp, struct AF_write *AFw)

Purpose:
  Write header information to an AU audio file

Description:
  This routine writes header information to an AU audio file.  The information
  in the file header is shown below.

  AU audio file header:
      Bytes     Type    Contents
     0 ->  3    int    File identifier
     4 ->  7    int    Header size H (bytes)
     8 -> 11    int    Audio data length (bytes)
    12 -> 15    int    Data encoding format
    16 -> 19    int    Sample rate (samples per second)
    20 -> 23    int    Number of interleaved channels
    24 -> 27    int    AFsp identifier ("AFsp)")
    28 -> H-1   --     Additional header information

Parameters:
  <-  AFILE *AFwrAUhead
      Audio file pointer for the audio file.  This routine allocates the
      space for this structure.  If an error is detected, a NULL pointer is
      returned.
   -> FILE *fp
      File pointer for the audio file
  <-> struct AF_write *AFw
      Structure containing file parameters

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.90 $  $Date: 2017/05/08 03:35:03 $

-------------------------------------------------------------------------*/

#include <assert.h>
#include <setjmp.h>
#include <string.h>

#include <libtsp.h>
#include <AFpar.h>
#include <libtsp/nucleus.h>
#include <libtsp/AFheader.h>
#include <libtsp/AFmsg.h>
#include <libtsp/AUpar.h>

#define ALIGN   4 /* Header length is a multiple of ALIGN */

/* setjmp / longjmp environment */
extern jmp_buf AFW_JMPENV;

/* Local function */
static int
AF_setDencod (int Format);


AFILE *
AFwrAUhead (FILE *fp, struct AF_write *AFw)

{
  AFILE *AFp;
  struct AU_head Fhead;
  int Lhead;

/* Set the long jump environment; on error return a NULL */
  if (setjmp (AFW_JMPENV))
    return NULL; /* Return from a header write error */

/* Leave room for the header information */
  if (AFw->WInfo.N > 0)
    Lhead = RNDUPV (AU_LHMIN + (sizeof FM_AFSP) - 1 + AFw->WInfo.N, ALIGN);
  else
    Lhead = RNDUPV (AU_LHMIN + 4L, ALIGN);

/* Set up the fixed header parameters */
  MCOPY (FM_AU, Fhead.Magic);
  Fhead.Lhead = Lhead;
  if (AFw->Nframe == AF_NFRAME_UNDEF)
    Fhead.Ldata = AU_NOSIZE;
  else
    Fhead.Ldata = AFw->Nframe * AFw->Nchan * AF_DL[AFw->DFormat.Format];
  Fhead.Dencod = AF_setDencod (AFw->DFormat.Format);
  Fhead.Srate = (UT_uint4_t) (AFw->Sfreq + 0.5);  /* Rounding */
  Fhead.Nchan = (UT_uint4_t) AFw->Nchan;

/* Write out the header */
  AFw->DFormat.Swapb = DS_EB;
  WHEAD_S (fp, Fhead.Magic);
  WHEAD_V (fp, Fhead.Lhead, DS_EB);
  WHEAD_V (fp, Fhead.Ldata, DS_EB);
  WHEAD_V (fp, Fhead.Dencod, DS_EB);
  WHEAD_V (fp, Fhead.Srate, DS_EB);
  WHEAD_V (fp, Fhead.Nchan, DS_EB);

/* AFsp information records */
  if (AFw->WInfo.N > 0) {
    WHEAD_SN (fp, FM_AFSP, (sizeof FM_AFSP) - 1); /* Omit null char */
    WHEAD_SN (fp, AFw->WInfo.Info, AFw->WInfo.N);
    WRPAD (fp, AFw->WInfo.N, ALIGN);
  }
  else
    WHEAD_SN (fp, "\0\0\0\0", 4);

/* Set the parameters for file access */
  AFp = AFsetWrite (fp, FT_AU, AFw);

  return AFp;
}

/* Set up the encoding parameters */


static int
AF_setDencod (int Format)

{
  int Dencod;

  switch (Format) {
  case FD_MULAW8:
    Dencod = AU_MULAW8;
    break;
  case FD_ALAW8:
    Dencod = AU_ALAW8;
    break;
  case FD_INT8:
    Dencod = AU_LIN8;
    break;
  case FD_INT16:
    Dencod = AU_LIN16;
    break;
  case FD_INT24:
    Dencod = AU_LIN24;
    break;
  case FD_INT32:
    Dencod = AU_LIN32;
    break;
  case FD_FLOAT32:
    Dencod = AU_FLOAT32;
    break;
  case FD_FLOAT64:
    Dencod = AU_DOUBLE64;
    break;
  default:
    Dencod = AU_UNSPEC;  /* Error */
    assert (0);
  }

  return Dencod;
}
