/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  AFILE *AFrdINhead (FILE *fp)

Purpose:
  Get file format information from an INRS-Telecommunications audio file

Description:
  This routine reads the header for an INRS-Telecommunications audio file. The
  header information is used to set the file data format information in the
  audio file pointer structure. The creation time information is returned as
  a "date" record in the information records in the audio file information
  structure.

  INRS-Telecommunications audio file:
   Offset Length Type    Contents
      0     4    float  Sampling Frequency (VAX float format)
      6    20    char   Creation time (e.g. " 8-DEC-1982 16:52:50")
     26     4    int    Number of speech samples in the file
    512    ...   --     Audio data
  The number of speech samples is checked against the total extent of the file.
  If unspecified, the file size is used to determine the number of samples.

  For INRS-Telecom files date information is embedded in the header.  This
  information is stored as a "date:" information record in the audio file
  information record structure.

Parameters:
  <-  AFILE *AFrdINhead
      Audio file pointer for the audio file
   -> FILE *fp
      File pointer for the file

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.82 $  $Date: 2017/07/12 17:27:51 $

-------------------------------------------------------------------------*/

#include <setjmp.h>
#include <string.h>

#include <libtsp.h>
#include <AFpar.h>
#include <libtsp/nucleus.h>
#include <libtsp/AFheader.h>
#include <libtsp/AFinfo.h>
#include <libtsp/AFmsg.h>
#include <libtsp/UTtypes.h>

#define LHEAD       512L
#define IN_NOSIZE   (~((UT_uint4_t) 0)) /* Unspecified data length */
#define NDT         20                  /* Date/time string length */
/* Common sampling frequencies (VAX floating-point values) in file byte order.
   The sampling frequencies recognized are 6500, 20000/3, 8000, 10000, 12000,
   16000, and 20000 Hz. */
#define NINRS 7
static const char *FM_INRS[NINRS] = {
  "\313\106\0\040",   "\320\106\125\125", "\372\106\0\0",   "\034\107\0\100",
  "\073\107\0\200",   "\172\107\0\0",     "\234\107\0\100"
};
static const double VSfreq[NINRS] = {
  6500., 20000./3., 8000., 10000., 12000., 16000., 20000.
};
static const char *ID[NINRS] = {
"IN-a", "IN-b", "IN-c", "IN-d", "IN-e", "IN-f", "IN-g"};

struct IN_head {
  char Sfreq[4];      /* Sampling freq (VAX float) */
/* UT_int2_t fill; */
/* char Datetime[NDT]; */
  UT_uint4_t Nsamp;   /* No. samples */
};
#define IN_MAXINFO  LHEAD

/* setjmp / longjmp environment */
extern jmp_buf AFR_JMPENV;

AF_READ_DEFAULT(AFr_default); /* Define the AF_read defaults */


AFILE *
AFrdINhead (FILE *fp)

{
  AFILE *AFp;
  int iSF;
  long int offs, poffs, Ldata;
  char Info[IN_MAXINFO];
  struct IN_head Fhead;
  struct AF_read AFr;
  static char *RecIDdate[] = AF_INFOID_DATE;

/* Set the long jump environment; on error return a NULL */
  if (setjmp (AFR_JMPENV))
    return NULL;  /* Return from a header read error */

/* Notes:
  - Very old INRS-Telecom audio files (generated on a PDP-11) have ~0 values
    for unwritten bytes in the header.
  - Old INRS-Telecom audio files used the Fhead.Nsamp field.  These files were
    an integral number of disk blocks long (512 bytes per disk block), with
    possibly part of the last block being unused.
  - Old INRS-Telecom audio files (generated on a PDP-11) have only an 18 byte
    date and time, followed by two 0 or two ~0 bytes.
*/

/* Defaults and initial values */
  AFr = AFr_default;
  AFr.RInfo.Info = Info;
  AFr.RInfo.N = 0;
  AFr.RInfo.Nmax = sizeof (Info);

/* Read in first part of the header */
  poffs = 0;
  offs  = RHEAD_S (fp, Fhead.Sfreq);

/* Use the sampling frequency as a file magic value */
  for (iSF = 0; iSF < NINRS; iSF++) {
    if (SAME_CSTR (Fhead.Sfreq, FM_INRS[iSF]))
      break;
  }
  if (iSF >= NINRS) {
    UTwarn ("AFrdINhead - %s", AFM_INRS_BadId);
    return NULL;
  }
  AFsetChunkLim (ID[iSF], poffs, offs, &AFr.ChunkInfo);
  poffs = offs;

  /* Read the rest of the header */
  offs += RSKIP (fp, 6L - offs);
  offs += AFrdInfoIdentText (fp, NDT, RecIDdate[0], &AFr.RInfo, 1);
  offs += RHEAD_V (fp, Fhead.Nsamp, DS_EL);
  AFsetChunkLim ("fmt ", poffs, offs, &AFr.ChunkInfo);

/* Position at the start of data */
  poffs = offs;
  offs += RSKIP (fp, LHEAD - offs);
  AFsetChunkLim ("skip", poffs, offs, &AFr.ChunkInfo);

/* Set the parameters for file access */
  AFr.Sfreq = VSfreq[iSF];
  AFr.DFormat.Format = FD_INT16;
  AFr.DFormat.Swapb = DS_EL;
  if (Fhead.Nsamp != 0 && Fhead.Nsamp != IN_NOSIZE) {
    AFr.NData.Nsamp = (long int) Fhead.Nsamp;
    Ldata = AFr.NData.Nsamp * AF_DL[AFr.DFormat.Format];
    AFsetChunkLim ("data", offs, offs + Ldata, &AFr.ChunkInfo);
  }
  else
    AFsetChunkLim ("data", offs, AF_EoF, &AFr.ChunkInfo);

  AFp = AFsetRead (fp, FT_INRS, &AFr, AF_NOFIX);

  return AFp;
}
