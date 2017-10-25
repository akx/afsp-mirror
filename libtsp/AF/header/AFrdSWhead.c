/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  AFILE *AFrdSWhead (FILE *fp)

Purpose:
  Get file format information from a Cadence SPW Signal file

Description:
  This routine reads the header for a Cadence SPW Signal file.  The header
  information is used to set the file data format information in the audio file
  pointer structure.

  Cadence SPW Signal file header:
   Offset Length Type    Contents
      0     15   char   File identifier ("$SIGNAL_FILE 9\n")
  The remaining header consists of lines of text data, with each line terminated
  by a newline character.  The header is divided into sections with a section
  header marked by a string starting with a "$" character.  An example header is
  shown below.
      $SIGNAL_FILE 9
      $USER_COMMENT
      <comment line(s)>
      $COMMON_INFO
      SPW Version        = 3.10
      System Type        = <machine> (e.g. "sun4")
      Sampling Frequency = <Sfreq>   (e.g. "8000")
      Starting Time      = 0
      $DATA_INFO
      Number of points   = <Nsamp>   (e.g. "2000")
      Signal Type        = <type>    ("Double", "Float", "Fixedpoint")
      Fixed Point Format = <16,0,t>  (optional) 16-bit integer, signed
      Complex Format     = Real_Imag (optional)
      $DATA <data_type>              ("ASCII", "BINARY")
  This routine does not support ASCII data with embedded comments.

  Text in the comment field of a Cadence SPW Signal file is returned as a
  "comment:" information record in the audio file parameter structure.  All
  lines between section markers are stored as information records.  For instance,
  the "SPW Version" line in the example is stored as "SPW Version: 3.10".

Parameters:
  <-  AFILE *AFrdSWhead
      Audio file pointer for the audio file
   -> FILE *fp
      File pointer for the file

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.54 $  $Date: 2017/07/13 20:12:43 $

-------------------------------------------------------------------------*/

#include <string.h>

#include <libtsp.h>
#include <AFpar.h>
#include <libtsp/nucleus.h>
#include <libtsp/AFdataio.h>
#include <libtsp/AFheader.h>
#include <libtsp/AFinfo.h>
#include <libtsp/AFmsg.h>

#define SW_ID "$SIGNAL_FILE 9"
#define SW_MAXLHEAD 1024      /* Maximum number of characters in the header */
#define SW_MAXINFO  SW_MAXLHEAD

enum SW_HDR_T {
  SW_SIGNAL_FILE_9 = 0,
  SW_USER_COMMENT  = 1,
  SW_COMMON_INFO   = 2,
  SW_DATA_INFO     = 3,
  SW_DATA_ASCII    = 4,
  SW_DATA_BINARY   = 5
};
enum SW_HDR_T SW_sect[] = {
  SW_SIGNAL_FILE_9,
  SW_USER_COMMENT,
  SW_COMMON_INFO,
  SW_DATA_INFO,
  SW_DATA_ASCII,
  SW_DATA_BINARY
};
static const char *SW_SectTab[] = {
  SW_ID,
  "$USER_COMMENT",
  "$COMMON_INFO",
  "$DATA_INFO",
  "$DATA ASCII",
  "$DATA BINARY",
  NULL
};
static const char *SW_ckID[] = {
  "SIG9",
  "COMT",
  "INFO",
  "fmt ",
  "DASC",
  "DBIN",
  "data"
};
static const char *SW_DTtab[] = {
  "Double",
  "Float",
  "Fixed-point",
  "Integer",
  NULL
};
enum {
  SW_DOUBLE     = 0,
  SW_FLOAT      = 1,
  SW_FIXEDPOINT = 2,
  SW_INTEGER    = 3
};

static const char *RecID_SysType[] = {"System Type:", NULL};
static const char *RecID_SigType[] = {"Signal Type:", NULL};
static const char *RecID_FPFormat[] = {"Fixed Point Format:", NULL};
/* Unsupported format options (note: no ':" on "Asynchronous") */
static const char *RecID_UnsOpt[] = {"Complex Format:", "Asynchronous", NULL};

AF_READ_DEFAULT(AFr_default); /* Define the AF_read defaults */

/* Local functions */
static int
AF_procSWhead (FILE *fp, int *BinData, int DataOffs[2], struct AF_read *AFr);


AFILE *
AFrdSWhead (FILE *fp)

{
  AFILE *AFp;
  int n, Err;
  int BinData, DataOffs[2];
  long int Nchan, Nframe, FLsize;
  double Sfreq, FullScale;
  enum UT_DS_T Fbo;
  enum AF_FD_T Format;
  const char *text;
  char Info[SW_MAXINFO];
  struct AF_read AFr;

/* Defaults and initial values */
  AFr = AFr_default;
  AFr.RInfo.Info = Info;
  AFr.RInfo.N = 0;
  AFr.RInfo.Nmax = sizeof (Info);

/* Process the header, creating info records */
  Err = AF_procSWhead (fp, &BinData, DataOffs, &AFr);
  if (Err)
    return NULL;

  text = AFgetInfoRec (RecID_UnsOpt, &AFr.RInfo);
  if (text != NULL) {
    UTwarn ("AFrSWhead - %s", AFM_SW_UnsOpt);
    return NULL;
  }

/* Set the number of channels */
  Nchan = 1;

/* Get the number of sample frames (can remain undefined) */
  Nframe = AFgetInfoFrame (&AFr.RInfo);

/* Get the sampling frequency */
  Sfreq = AFgetInfoSfreq (&AFr.RInfo);
  if (Sfreq == AF_SFREQ_UNDEF) {
    Sfreq = AFopt.InputPar.Sfreq;
    UTwarn (AFMF_SfreqInPar, "AFrdSWhead -", Sfreq);
  }

/* Set the byte swap code */
  Fbo = DS_EB;
  if (BinData) {
    text = AFgetInfoRec (RecID_SysType, &AFr.RInfo);
    if (text != NULL && ! (strcmp (text, "sun4") == 0 ||
                           strcmp (text, "hp700") == 0))
      UTwarn (AFMF_SW_UnsSys, "AFrdSWhead -", text);
  }

/* Set the data type */
  FullScale = AF_FULLSCALE_DEFAULT;
  text = AFgetInfoRec (RecID_SigType, &AFr.RInfo);
  n = STkeyMatch (text, SW_DTtab);
  if (n < 0) {
    UTwarn (AFMF_SW_UnsSigType, "AFrdSWhead:", text);
    return NULL;
  }
  Format = FD_UNDEF;
  if (BinData) {
    if (n == SW_DOUBLE)
      Format = FD_FLOAT64;
    else if (n == SW_FLOAT)
      Format = FD_FLOAT32;
    else if (n == SW_FIXEDPOINT)
      Format = FD_INT16;
    else if (n == SW_INTEGER)
      Format = FD_INT32;
  }
  else {
    Format = FD_TEXT;
    if (n == SW_DOUBLE || n == SW_FLOAT)
      FullScale = 1;
    else if (n == SW_FIXEDPOINT)
      FullScale = AF_FULLSCALE_INT16;
    else if (n == SW_INTEGER)
      FullScale = AF_FULLSCALE_INT32;
  }

/* Modify the FullScale value if "Fixed Point Fomat" is set */
  text = AFgetInfoRec (RecID_FPFormat, &AFr.RInfo);
  if (text != NULL) {
    if (strcmp (text, "<16,15,t>") == 0) {
      Format = FD_TEXT16;
      FullScale = AF_FULLSCALE_INT16;   /* Sign + 15 bits for integer part */
    }
    else if (strcmp (text, "<16,0,t>") == 0)
      FullScale = 1;                    /* Sign + 15 bits for fractional part */
    else {
      UTwarn (AFMF_SW_UnsFixFormat, "AFrdSWhead:", text);
      return NULL;
    }
  }

/* Set the parameters for file access */
  AFr.NData.Nchan = Nchan;
  if (Nframe != AF_NFRAME_UNDEF)
    AFr.NData.Nsamp = Nframe * Nchan;
  AFr.DFormat.Format = Format;
  AFr.DFormat.FullScale = FullScale;
  AFr.DFormat.Swapb = Fbo;
  AFr.Sfreq = Sfreq;

/* Set up the data limits */
  FLsize = AF_EoF;
  if (BinData && AFr.NData.Nsamp != AF_NSAMP_UNDEF)
    FLsize = DataOffs[1] + AFr.NData.Nsamp * AF_DL[Format];
  if (BinData)
    AFsetChunkLim ("DBIN", DataOffs[0], FLsize, &AFr.ChunkInfo);
  else
    AFsetChunkLim ("DASC", DataOffs[0], FLsize, &AFr.ChunkInfo);
  AFsetChunkLim ("data", DataOffs[1], FLsize, &AFr.ChunkInfo);

  AFp = AFsetRead (fp, FT_SPW, &AFr, AF_NOFIX);
  return AFp;
}

/* Process the header - write records to the info record structure */

static int
AF_procSWhead (FILE *fp, int *BinData, int DataOffs[2], struct AF_read *AFr)

{
  char *line;
  char *p;
  char Token[SW_MAXLHEAD+1];
  enum AF_ERR_T ErrCode;
  int nc, iSec, Err;
  int soffs, ioffs, poffs;

/* Check for the file identifier */
  soffs = 0;
  ErrCode = AF_NOERR;
  line = AFgetLine (fp, &ErrCode);
  if (line == NULL || ! SAME_CSTR (SW_ID, line)) {
    UTwarn ("AFrdSWhead - %s", AFM_SW_BadId);
    return 1;
  }
  ioffs = (int) strlen (line) + 1;      /* Past '\n' */

/* Check the header information, line by line */
  Err = 0;
  iSec = 0;
  while (ioffs < SW_MAXLHEAD) {

    poffs = ioffs;    /* Line start */

    /* Read a line */
    line = AFgetLine (fp, &ErrCode);
    if (ErrCode)
      break;
    ioffs += (int) strlen (line) + 1;   /* line length + '\n' */

    /* Trim trailing white-space */
    nc = STtrimTail (line);

    if (line[0] == '$') {

      /* Finish a section */
      AFsetChunkLim (SW_ckID[iSec], soffs, poffs, &AFr->ChunkInfo);

      /* Identify a new section */
      soffs = poffs;
      iSec = STkeyMatch (line, SW_SectTab);
      if (iSec < 0) {
        Err = 1;
        break;
      }
      else if (iSec == SW_DATA_BINARY || iSec == SW_DATA_ASCII)
        break;        /* At data */
      else
        continue;     /* Get next line */
    }

    if (iSec == SW_USER_COMMENT) {
      if (nc > 0)
        AFaddInfoRec ("comment:", line, nc, &AFr->RInfo);
    }
    else if (iSec == SW_COMMON_INFO || iSec == SW_DATA_INFO) {
      /* Check for an equal sign separator */
      p = STfindToken (line, "=", "", Token, 1, SW_MAXLHEAD);
      if (p != NULL) {
        STcatMax (":", Token, SW_MAXLHEAD);
        nc = (int) strlen (p);
        AFaddInfoRec (Token, p, nc, &AFr->RInfo);
      }
      /* No equal sign */
      else
        AFaddInfoRec (Token, NULL, 0, &AFr->RInfo);
    }
  }

/* Set the binary data flag and data limits */
  *BinData = (iSec == SW_DATA_BINARY);
  DataOffs[0] = soffs;
  DataOffs[1] = ioffs;

  if (! (iSec == SW_DATA_ASCII || iSec == SW_DATA_BINARY) ||
      Err == 1 || ioffs >= SW_MAXLHEAD) {
    UTwarn ("AFrdSWhead - %s", AFM_SW_UnsHead);
    return 1;
  }

  if (FLseekable (fp) && ioffs != (int) AFtell (fp, &ErrCode)) {
    UTwarn ("AFrdSWhead  - %s", AFM_SW_PosErr);
    return 1;
  }

  return Err;
}
