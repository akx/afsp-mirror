/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  AFILE *AFrdTAhead (FILE *fp)

Purpose:
  Get file format information from a text audio file

Description:
  This routine reads the header for a text audio file.  The header information
  is used to set the file data format information in the audio file pointer
  structure.  A text audio file can be created using AFopnWrite.

  In a text audio file, data is stored as the text representation of data
  values, with values separated by white-space (blank, tab, newline, vertical
  tab, form feed, carriage return).  The audio data in a text audio file can be
  the text representation of integers or floating point numbers.

  The text file can have Unix line endings ("\n") or Windows line endings
  ("\r\n").

  Text audio files using text data have been used to store noise samples for the
  NOISEX-92 database.  The text on the header line following the file identifier
  ("%//\n" or "//\r\n") is returned as a "title:" information record stored in
  the audio file info structure.

  A sample NOISE-EX file header is shown below.  This header is divided into
  sections separated by the "%//" lines.
     %//
     %Voice Babble
     %File: babble.data.Z
     %//
     %
     %sampling rate: 19.98 KHz
     %A/D: 16 bit
     %pre-filter: anti-aliasing filter
     %
     %//
     %
     %Voice Babble acquired by recording samples from 1/2" B&K condensor
     %...
     %
     %//
     %
     %Taken from:
     %
     %NOISE-ROM-0 signal.019
     %...
     %
     %//
  The NOISE-EX file header specifies the sampling rate and data resolution. The
  number of channels is not explicitly defined.  As detailed below, this
  parameter will be set to the value as set be AFsetInputPar.

  The NOISE_EX header has 4 sections marked by "%//" separators.
  Section 1:
    The text of the first line is stored as a "title: <text> record" in the
    audio file information structure.  Additional lines are stored as
    information records.
  Section 2:
    Lines are stored as information records.
  Section 3:
    This section consists of multiple lines of text. These lines are
    concatenated with newline separators and stored as a "description:" record.
  Section 4:
    This section consists of multiple lines of text. These lines are
    concatenated with newline separators and stored as a "product:" record.

  For text audio files written by the AFsp routines, a simpler version of the
  format is created.  This format only has one section (Section 1 above).  The
  first line is a title.  Subsequent lines have keyword value records.  These
  records describe the data format.  An example AFsp header is shown below.
    %//
    %short title
    %date: 2017-05-18 18:41:03 UTC
    %program: CopyAudio
    %sample_rate: 8000
    %channels: 2
    %frames: 23493
    %full_scale: 32768
    %//

  Text audio file can have a header as short as the one line with the file
  identifier "%//".  If the number of samples/channel (frames) is missing, that
  value is ascertained from the length of the file.  Other missing data format
  information (number of channels, data full scale value, sampling rate) is
  filled in from parameters that can be set with routine AFsetInputPar.

  An example of a short header that explicitly defines data format parameters
  is as follows.
     %//
     %<title>
     %full_scale: 32768
     %sampling_rate: 8000
     %channels: 1
  This example uses canonical keywords.  The routines AFgetInfoFullScale,
  AFgetInfoSfreq, AFgetInfoChan, and AFgetInfoNbs provide for alternative
  keywords for the data format information.

Parameters:
  <-  AFILE *AFrdTAhead
      Audio file pointer for the audio file
   -> FILE *fp
      File pointer for the file

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.68 $  $Date: 2017/07/13 20:12:49 $

-------------------------------------------------------------------------*/

#include <math.h>
#include <string.h>

#include <libtsp.h>
#include <AFpar.h>
#include <libtsp/nucleus.h>
#include <libtsp/AFdataio.h>
#include <libtsp/AFheader.h>
#include <libtsp/AFinfo.h>
#include <libtsp/AFmsg.h>

#define POW2(x) (round(pow(2.,(double)(x))))

#define SAME_CSTR(str,ref)  (memcmp (str, ref, sizeof (str)) == 0)

#define TA_MAXINFO  1500
#define TA_MAXLHEAD 1500  /* Maximum number of characters in the header */
#define TA_IDu ("%//")    /* Unix style line termination */
#define TA_IDw ("%//\r")  /* Windows style line termination */

AF_READ_DEFAULT(AFr_default); /* Define the AF_read defaults */

/* Local functions */
static double
AF_getFullScale (struct AF_info *AFinfo);
static int
AF_procTAhead (FILE *fp, struct AF_read *AFr);


AFILE *
AFrdTAhead (FILE *fp)

{
  AFILE *AFp;

  struct AF_read AFr;
  char Info[TA_MAXINFO];
  long int Nchan, Nframe;
  int Err;
  double FullScale, Sfreq;

/* Defaults and initial values */
  AFr = AFr_default;
  AFr.RInfo.Info = Info;
  AFr.RInfo.N = 0;
  AFr.RInfo.Nmax = sizeof (Info);

/* Process the header, creating the info record structure */
  Err = AF_procTAhead (fp, &AFr);
  if (Err)
    return NULL;

/* Get the full scale value */
  FullScale = AF_getFullScale (&AFr.RInfo);
  if (FullScale == AF_FULLSCALE_UNDEF) {
    FullScale = AFopt.InputPar.FullScale;
    if (FullScale != AF_FULLSCALE_DEFAULT)
      UTwarn (AFMF_FullScInPar, "AFrdTAhead -", FullScale);
  }

  /* Get the number of channels */
  Nchan = AFgetInfoChan (&AFr.RInfo);
  if (Nchan <= 0) {
    Nchan = AFopt.InputPar.Nchan;
    UTwarn (AFMF_NchanInPar, "AFrdTAhead -", Nchan);
  }

/* Get the number of sample frames (can remain undefined) */
  Nframe = AFgetInfoFrame (&AFr.RInfo);

/* Get the sampling frequency */
  Sfreq = AFgetInfoSfreq (&AFr.RInfo);
  if (Sfreq == AF_SFREQ_UNDEF) {
    Sfreq = AFopt.InputPar.Sfreq;
    UTwarn (AFMF_SfreqInPar, "AFrdTAhead -", Sfreq);
  }

/* Set the parameters for file access */
  AFr.NData.Nchan = Nchan;
  if (Nframe != AF_NFRAME_UNDEF)
    AFr.NData.Nsamp = Nframe * Nchan;
  AFr.DFormat.Format = FD_TEXT;
  AFr.DFormat.FullScale = FullScale;
  AFr.Sfreq = Sfreq;

  AFp = AFsetRead (fp, FT_TXAUD, &AFr, AF_NOFIX);

  return AFp;
}

/* Process the header - write records to the info record structure */

static int
AF_procTAhead (FILE *fp, struct AF_read *AFr)

{
  char *line;
  int c;
  int poffs, soffs, ioffs;
  char Info3[TA_MAXINFO];
  char Info4[TA_MAXINFO];
  int nl, nc, iSec, Err;
  enum AF_ERR_T ErrCode;
  struct AF_info tInfo3, tInfo4;
  static char *RecIDtitle[] = AF_INFOID_TITLE;
  static char *RecIDdesc[]  = AF_INFOID_DESC;
  static char *RecIDprod[]  = AF_INFOID_PROD;

/* Initialize the temp Info structures */
  tInfo3.Info = Info3;
  tInfo3.N = 0;
  tInfo3.Nmax = sizeof (Info3);
  tInfo4.Info = Info4;
  tInfo4.N = 0;
  tInfo4.Nmax = sizeof (Info4);

/* Check for the file identifier */
  poffs = 0;
  ErrCode = AF_NOERR;
  line = AFgetLine (fp, &ErrCode);

  /* Compare with %//\n\0], length 5, or [%//\r\n\0] */
  if (line == NULL || ! (SAME_CSTR (TA_IDu, line) ||
                         SAME_CSTR (TA_IDw, line))) {
    UTwarn ("AFrdTAhead - %s", AFM_TA_BadId);
    return 1;
  }
  ioffs = (int) strlen (line) + 1;      /* Past '\n' */
  if (ioffs == sizeof (TA_IDu))
    AFsetChunkLim ("TA-u", poffs, ioffs, &AFr->ChunkInfo);
  else
    AFsetChunkLim ("TA-w", poffs, ioffs, &AFr->ChunkInfo);
  soffs = ioffs;      /* Section start */

/* Check the header information, line by line */
  iSec = 1;
  nl = 0;             /* No. lines in section (excluding "%//" line) */

/* ----- ------ */
  Err = 0;
  while (ioffs < TA_MAXLHEAD) {

    poffs = ioffs;    /* Line start */

    /* Check if still in the header */
    c = fgetc (fp);
    if (feof (fp)) {
      Err = 1;
      break;
    }
    else if (c != '%') {
      AFsetChunkLim ("EoH ", soffs, poffs, &AFr->ChunkInfo);
      break;
    }
    ioffs += 1;                   /* Account for '%' */

    /* Read a line (without the initial '%') */
    line = AFgetLine (fp, &ErrCode);
    if (ErrCode)
      break;
    ioffs += (int) strlen (line) + 1;   /* line length + '\n' */

    /* Trim trailing white-space (including trailing '\r' character) */
    nc = STtrimTail (line);

    if (strcmp ("//", line) == 0) {
      if (iSec == 1) {
        if (nl <= 2)
          AFsetChunkLim ("info", soffs, poffs, &AFr->ChunkInfo);
        else
          AFsetChunkLim ("fmt ", soffs, poffs, &AFr->ChunkInfo);
      }
      else if (iSec == 2)
        AFsetChunkLim ("fmt ", soffs, poffs, &AFr->ChunkInfo);
      else if (iSec == 3)
        AFsetChunkLim ("desc", soffs, poffs, &AFr->ChunkInfo);
      else if (iSec == 4)
        AFsetChunkLim ("prod", soffs, poffs, &AFr->ChunkInfo);
      ++iSec;
      soffs = poffs;
      nl = 0;
      continue;
    }
    else
      ++nl;

    if (nc <= 0)
      continue;

    /* Section 1: First line as title, others as they are */
    if (iSec == 1) {
      if (nl == 1)
        AFaddInfoRec (RecIDtitle[0], line, nc, &AFr->RInfo);
      else
        AFaddInfoChunk (line, nc + 1, &AFr->RInfo);
    }

    /* Section 2: Add Info records as they are to the Info structure */
    else if (iSec == 2)
      AFaddInfoChunk (line, nc + 1, &AFr->RInfo);

    /* Section 3: Save up Info records - process later */
    else if (iSec == 3)
      AFaddInfoChunk (line, nc + 1, &tInfo3);  /* Squirrel away in temp area */

    /* Section 4: Save up Info records - process later */
    else if (iSec == 4)
      AFaddInfoChunk (line, nc + 1, &tInfo4);  /* Squirrel away in temp area */

    else if (iSec == 5) {
      UTwarn ("AFrdTAhead - %s", AFM_TA_UnsHead);
      return 1;
    }
  }

/* ----- ------ */
  if (ioffs >= TA_MAXLHEAD) {
    UTwarn ("AFrdTAhead - %s", AFM_TA_UnsHead);
    return 1;
  }

  /* Restore the peek-ahead character */
  if (! ErrCode)
    ungetc (c, fp);

  if (FLseekable (fp) && ioffs != (int) AFtell (fp, &ErrCode))
    UTwarn ("AFrdTAhead - %s", AFM_TA_PosErr);

  /* Set the end-of-data to EoF - it is not possible to check the file byte
    limits of text data without actually reading all of the values
  */
  AFsetChunkLim ("data", ioffs, AF_EoF, &AFr->ChunkInfo);

  /* Process Section 3 Info records as "description: <xxx>\n<yyy>\n<zzz>..." */
  if (tInfo3.N > 0)
    AFaddInfoRec (RecIDdesc[0], tInfo3.Info, tInfo3.N, &AFr->RInfo);

  /* Process Section 4 Info records as "product: <xxx>\n<yyy>\n<zzz>..." */
  if (tInfo4.N > 0)
    AFaddInfoRec (RecIDprod[0], tInfo4.Info, tInfo4.N, &AFr->RInfo);

  return Err;
}

/* Determine the scale factor */


static double
AF_getFullScale (struct AF_info *AFInfo)

{
  struct AF_NbS AFNbS;
  double FullScale;

  /* Look for a full_scale info record */
  FullScale = AFgetInfoFullScale (AFInfo);

  /* Alternative: FullScale implied by bits/sample info record */
  if (FullScale == AF_FULLSCALE_UNDEF) {
    AFNbS = AFgetInfoNbS (AFInfo);
    if (AFNbS.Res > 0) {
      if (AFNbS.Res > 8)
        FullScale = POW2(AFNbS.Res - 1);
      else
        FullScale = POW2(AFNbS.Res);
    }
  }

  return FullScale;
}
