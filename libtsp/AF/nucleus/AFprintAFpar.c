/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void AFprintAFpar(AFILE *AFp, const char Fname[], FILE *fpinfo)

Purpose:
  Print information about an audio file

Description:
  This routine optionally prints header information for an audio file. Certain
  information records in the audio file parameter structure affect the printout.
  - Text from a "date:" or "creation_date:" information record is printed
    instead of the file creation date/time obtained from the file system.
  - Text from a "title:", "name:", "comment:", "comments:", "utterance_id:",
    or "display_text:" information record printed as "Description: <text>".

    The printout is of the form:
      AIFF-C sound file: --path--/audiofiles/addf8.aifc
        Description: <text>
        Number of samples : 23808 (2.976 s)  YYYY-MM-dd hh:mm:ss UTC
        Sampling frequency: 8000 Hz
        Number of channels: 1 (16-bit integer)

Parameters:
  <-  void AFprintAFpar
   -> AFILE *AFp
      Audio file pointer for the audio file. If AFp is NULL, this routine is a
      no-op.
   -> const char Fname[]
      File name. If AFp->fp stdin or stdout, this string is not used.
   -> FILE *fpinfo
      File pointer for printing the audio file identification information. If
      fpinfo is NULL, no information is printed.

Author / revision:
  P. Kabal  Copyright (C) 2020
  $Revision: 1.85 $  $Date: 2020/11/26 11:33:35 $

-------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>

#include <libtsp.h>
#include <AFpar.h>
#include <libtsp/nucleus.h>
#include <libtsp/AFheader.h>
#include <libtsp/AFinfo.h>
#include <libtsp/AFmsg.h>

#define NC_SPKR_NAMES 40
#define NELEM(array)  ((int) ((sizeof array) / (sizeof array[0])))

/* File type names - must match codes in AFpar.h */
const char *AF_FTN[AF_NFT] = {
  NULL,
  "Audio file",   /* Headerless or non-standard audio file */
  "AU audio file",
  "WAVE file",
  "WAVE-ex file",
  "WAVE-BWF file",
  "WAVE-ex-BWF file",
  "AIFF sound file",
  "AIFF-C sound file",
  "NIST SPHERE audio file",
  "ESPS audio file",
  "IRCAM soundfile",
  "SPPACK file",
  "INRS-Telecom audio file",
  "Comdisco SPW Signal file",
  "CSL NSP file",
  "Text audio file"
};

/* Data type names (short version) */
const char *AF_DTN[] = {
  "unsupported",
  "8-bit A-law",
  "8-bit mu-law",
  "8-bit mu-law (bit-rev)",
  "offset-binary 8-bit integer",
  "8-bit integer",
  "16-bit integer",
  "24-bit integer",
  "32-bit integer",
  "32-bit float",
  "64-bit float",
  "text data",
  "text data"
};

/* Data lengths */
const enum AF_FDL_T AF_DL[] = {
  FDL_UNDEF,
  FDL_ALAW8,
  FDL_MULAW8,
  FDL_MULAWR8,
  FDL_UINT8,
  FDL_INT8,
  FDL_INT16,
  FDL_INT24,
  FDL_INT32,
  FDL_FLOAT32,
  FDL_FLOAT64,
  FDL_TEXT16,
  FDL_TEXT,
};

/* Local function */
static int
AF_Spkr_List(int Nchan, const unsigned char SpkrConfig[], char SpkrNames[],
             int MxN);


void
AFprintAFpar(AFILE *AFp, const char Fname[], FILE *fpinfo)

{
  const char *ft;
  const char *Datetime, *Title;
  char FullName[FILENAME_MAX];
  char SpkrNames[NC_SPKR_NAMES+1];
  int Res;
  static const char *RecIDtitle[] = AF_INFOID_TITLE;
  static const char *RecIDdate[] = AF_INFOID_DATE;

  if (AFp == NULL)
    return;

  assert(AFp->Op == FO_RO || AFp->Op == FO_WO);
  assert(AFp->Format > 0 && AFp->Format < AF_NFD);
  assert(NELEM(AF_DL) == AF_NFD && NELEM(AF_DTN) == AF_NFD);
  assert(AFp->Ftype > 0 && AFp->Ftype < AF_NFT);

/* Print the header information */
  if (fpinfo != NULL) {

    /* ---------------- */
    /* File type */
    if (AFp->Ftype == FT_NH && AFopt.InputPar.Start == 0L)
      ft = AFM_Headerless;
    else
      ft = AF_FTN[AFp->Ftype];
    fprintf(fpinfo, " %s: ", ft);

    /* File name */
    if (AFp->fp == stdin)
      fprintf(fpinfo, "<stdin>\n");
    else if (AFp->fp == stdout)
      fprintf(fpinfo, "<stdout>\n");
    else {
      FLfullName(Fname, FullName);
      fprintf(fpinfo, "%s\n", FullName);
    }

    /* ---------------- */
    /* Title */
    Title = AFgetInfoRec(RecIDtitle, &AFp->AFInfo);
    if (Title != NULL)
      fprintf(fpinfo, AFMF_Desc, STstrDots(Title, 56));

    /* ---------------- */
    /* Number of sample frames */
    if (AFp->Op == FO_RO) {
      if (AFp->Nsamp == AF_NSAMP_UNDEF)
        fprintf(fpinfo, AFM_NumFrameUnk);
      else
        fprintf(fpinfo, AFMF_NumFrame, AFp->Nsamp / AFp->Nchan);

      /* Duration */
      if (AFp->Nsamp != AF_NSAMP_UNDEF && AFp->Sfreq > 0.0)
        fprintf(fpinfo, " (%.4g s)", (AFp->Nsamp / AFp->Nchan) / AFp->Sfreq);

    }
    else if (AFp->Op == FO_WO) {
      if (AFp->Nframe == AF_NFRAME_UNDEF)
        fprintf(fpinfo, AFM_NumFrameNA);
      else
        fprintf(fpinfo, AFMF_NumFrame, AFp->Nframe);

      /* Duration */
      if (AFp->Nframe != AF_NFRAME_UNDEF && AFp->Sfreq > 0.0)
        fprintf(fpinfo, " (%.4g s)", AFp->Nframe / AFp->Sfreq);
    }

    /* Date */
    Datetime = AFgetInfoRec(RecIDdate, &AFp->AFInfo);
    if (Datetime == NULL  && AFp->fp != stdin)
      Datetime = FLfileDate(AFp->fp, 3);
    if (Datetime != NULL)
      fprintf(fpinfo, "  %.30s\n", Datetime);
    else
      fprintf(fpinfo, "\n");

    /* ---------------- */
    /* Sampling frequency */
    fprintf(fpinfo, AFMF_Sfreq, AFp->Sfreq);

    /* ---------------- */
    /* Number of channels and data format */
    fprintf(fpinfo, AFMF_NumChan, AFp->Nchan);
    Res = 8 * AF_DL[AFp->Format];
    if (AFp->NbS == Res || Res == 0)    /* RES == 0 => text format */
      fprintf(fpinfo, " (%s)", AF_DTN[AFp->Format]);
    else
      fprintf(fpinfo, " (%d/%s)", AFp->NbS, AF_DTN[AFp->Format]);

    /* Speaker configuration */
    AF_Spkr_List(AFp->Nchan, AFp->SpkrConfig, SpkrNames, NC_SPKR_NAMES);
    if (strlen(SpkrNames) > 0)
      fprintf(fpinfo, " [%s]", SpkrNames);
    fprintf(fpinfo, "\n");
  }
}

/* Modify the speaker list to include a shortened list of AF_SPKR_AUX values */
static int
AF_Spkr_List(int Nchan, const unsigned char SpkrConfig[], char SpkrNames[],
             int MxN)

{
  int Nspkr, Naux, NO, ncS;

  /* Generate the list of speaker positions */
  NO = AFspeakerNames(SpkrConfig, SpkrNames, MxN);
  if (NO == 0)
    return NO;

  /* Add a short or shortened list of AF_SP_AUX specifications */
  Nspkr = (int) strlen((const char *) SpkrConfig);
  if (NO == Nspkr) {
    Naux = Nchan - Nspkr;
    ncS = (int) strlen((const char *) SpkrNames);
    if (Naux == 1 && ncS + 2 <= MxN)
      STcatMax(" -", SpkrNames, MxN);
    else if (Naux == 2 && ncS + 4 <= MxN)
      STcatMax(" - -", SpkrNames, MxN);
    else if (Naux == 3 && ncS + 6 <= MxN)
      STcatMax(" - - -", SpkrNames, MxN);
    else if (Naux > 3 && ncS + 6 <= MxN)
      STcatMax(" - ...", SpkrNames, MxN);
  }

  return NO;
}
