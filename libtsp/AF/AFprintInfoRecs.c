/*------------- Telecommunications & Signal Processing Lab -------------
                           McGill University

Routine:
  void AFprintInfoRecs(const AFILE *AFp, FILE *fpinfo)

Purpose:
  Print all information records for an audio file

Description:
  This formats and prints information records for an audio file. Non-printing
  control characters are replaced by a scarab ('\xA4') character. Long lines
  are truncated with a trailing ... to indicate truncation.

  When an input file containing AFsp information records is opened with
  AFopnRead, information records are read and stored in the audio file parameter
  structure. The following records affect the audio file information that is
  stored in the audio file parameter structure or printed when the file is
  opened.
  - "date:" or "creation_date:" record
    The data and time in that record is printed when the file is opened rather
    than the file modification date obtained from the file system.
  - "sample_rate:" record
    The sample rate in the record is compared to the sample rate from the file
    header. If the file header sample rate from the header is an integer value
    and agrees with the rounded value of the value in the record, the value from
    the record used to set the Sfreq variable in the audio file parameter
    structure.
  - "loudspeakers:" record
    The loudspeaker locations are decoded and the result is stored in the
    SpkrConfig array in the audio file parameter structure. The list of speaker
    positions is printed when the file is opened, along with the other file
    information.
  - "name:", "title:", "comment:", or "display_text:" record
    This information record printed as "Description: <text>" when the file is
    opened.

  Information records found in input audio files are returned in the AFInfo
  structure which is part of the audio file parameter structure. The
  information records are determined from the audio file as follows.
  AU files:
    Text information precedes the audio data. If the "AFsp" identifier is
    present, the remaining text is stored as information records in the audio
    file parameter structure. Otherwise the text is stored as an "info:"
    information record.
  AIFF and AIFF-C files:
    The text information can appear in several different header chunks. This
    information is extracted and stored as information records in the audio file
    parameter structure.
       "NAME" chunk text => "name:" information record
       "AUTH" chunk text => "author" information record
       "(c) " chunk text => "copyright:" information record
       "ANNO" chunk text => "annotation:" information record
    If an "ANNO" chunk text has an "AFsp" tag as a preamble, the remaining text
    is stored as individual information records
  WAVE files:
    Information records are generated from "afsp", "DISP", "LIST", or "bext"
    chunks. The information is extracted and stored as information records in
    the audio file parameter structure.

       "afsp" chunk - Text is extracted and stored as individual information
          records.
       "DISP" chunk - text is stored as a "display_text:" information record.
       "LIST" chunk - INFO text has further subcategories as follows.
         "IARL" text => "archival_location:" information record.
         "IART" text => "artist:" information record.
         "ICMS" text => "commissioned:" information record.
         "ICMT" text => "comment:" information record.
         "ICOP" text => "copyright:" information record.
         "ICRD" text => "creation_date:" information record.
         "IENG" text => "engineer:" information record.
         "IGNR" text => "genre:" information record.
         "IKEY" text => "keywords:" information record.
         "IMED" text => "medium:" information record.
         "INAM" text => "name:" information record.
         "IPRD" text => "product:" information record.
         "ISBJ" text => "subject:" information record.
         "ISFT" text => "software:" information record.
         "ISRC" text => "source:" information record.
         "ISRF" text => "source_form:" information record.
         "ITCH" text => "technician:" information record.
       "bext" chunk - information (Broadcast WAVE files) is extracted and
         returned as information records.
  SPPACK files:
    "comment:" and "command:" information records
  ESPS sampled data feature files:
    The following information record identifiers are picked up from the file
    header.
       "date:"            "header_version:"       "program:"
       "program_version:" "program_compile_date:" "start_time:"
       "max_value:"
  INRS-Telecom files:
    "date:" information record
  NSP files:
    "comment:" information record
  IRCAM soundfiles:
    "SF_COMMENT:" information record
  NIST SPHERE files:
    Text in the header is parsed and stored as information records. The
    identifier is picked up from the header. Possible identifiers include,
       "database_id:"            "database_version:"
       "utterance_id:"           "sample_min:"
       "sample_max:"             "conversation_id:"
       "microphone:"             "prompt_id:"
       "recording_date:"         "recording_environment:"
       "recording_site:"         "recording_time:"
       "sample_checksum:"        "speaker_sentence_number:"
       "speaker_session_number:" "speaker_utterance_number:"
       "speaker_id:"             "speaker_id_a:"
       "speaker_id_b:"           "speaking_mode:"
       "utterance_id:".
  Cadence SPW Signal files:
    A "comment:" information record is generated from the second line of a
    header. Keyword lines in the header generate information records.
  Text audio files (with header):
    A "title:" information record is generated from the second line of the
    header. Keyword lines in the header generate information records. A
    multi-line description is returned as a "description:" information record.

Parameters:
  <-  void AFprintInfoRecs
   -> const AFILE *AFp
      Audio file parameter structure
  <-> FILE *fpinfo
      Output stream for the file information. If fpinfo is NULL, no information
      is written.

Author / revision:
  P. Kabal  Copyright (C) 2020
  $Revision: 1.16 $  $Date: 2020/11/30 12:28:08 $

----------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>         /* strlen */

#include <libtsp.h>
#include <AFpar.h>
#include <libtsp/nucleus.h> /* STstrDots */

struct LS {
  int nc;     /* Number of characters in line */
  int skip;   /* Delimiter size */
};

static struct LS
AF_findNL(int ist, const char Rec[], int NRec);
static void
AF_printInfoLine(FILE *fpinfo, const char Preamble[], const char String[],
                 int nchr);
static void
AF_procInfoRec(const char Rec[], int Lrec, FILE *fpinfo);


void
AFprintInfoRecs(const AFILE *AFp, FILE *fpinfo)

{
  int n, Lrec;
  const struct AF_info *AFInfo;

  if (fpinfo == NULL)
    return;

  AFInfo = &AFp->AFInfo;

  /* AFInfo.Info
  - Should be terminated by a '\0'
  - Should not have a terminating '\n' (considered white-space)
  */
  if (AFInfo->N == 0)
    return;
  assert(AFInfo->N > 0 && AFInfo->Info[AFInfo->N-1] == '\0');

/* Loop over records */
  n = 0;
  while (n < AFInfo->N) {
    Lrec = (int) strlen(&AFInfo->Info[n]);
    AF_procInfoRec(&AFInfo->Info[n], Lrec, fpinfo);
    n += Lrec + 1;             /* Skip over '\0' */
  }
}

static void
AF_procInfoRec(const char Rec[], int Lrec, FILE *fpinfo)

{
  int ist, N;
  const char *p;
  struct LS RLine;

  /* Check for a multi-line record */
  ist = 0;
  RLine = AF_findNL(ist, Rec, Lrec);
  if (RLine.skip == 0)

    /* Single line record */
    AF_printInfoLine(fpinfo, "", Rec, Lrec);

  else {

    /* Multi-line record */
    /* Look for a record ID on the first line; print on a separate line */
    p = memchr(Rec, ':', RLine.nc);
    if (p != NULL) {
      N = (int) (p - Rec) + 1;
      AF_printInfoLine(fpinfo, "", Rec, N);
      if (Rec[N] == ' ')
        ++N;
      ist = N;
    }

    /* Print the lines */
    while (ist < Lrec) {
      RLine = AF_findNL(ist, Rec, Lrec);

      /* Print the line, indented */
      AF_printInfoLine(fpinfo, "  ", &Rec[ist], RLine.nc);
      ist += RLine.nc + RLine.skip;   /* Skip over delimiter */
    }
  }
}


/* Find a newline delimiter
  Newlines are marked by LF, CR, NUL, VT, FF, or CR/LF characters. Information
  is returned in structure LS with fields nc and skip.
  - ist is the start of the line
  - nc is the number of characters in the line before the delimiter, i.e.
    the line is Rec[ist:ist+nc-1].
  - skip is the number of characters in the delimiter. The next start of line
    should be set to ist + nc - 1 + skip. The skip field is set to zero if no
    newline delimiter is found.
*/

static struct LS
AF_findNL(int ist, const char Rec[], int Lrec)

{
  int i;
  struct LS RLine;

  RLine.nc = Lrec - ist;
  RLine.skip = 0;
  for (i = ist; i < Lrec; ++i) {
    if (Rec[i] == '\r') {
      RLine.nc = i - ist;
      RLine.skip = 1;
      if (i < Lrec - 1 && Rec[i+1] == '\n')
        RLine.skip = 2;
      break;
    }
    else if (Rec[i] == '\n' || Rec[i] == '\f' || Rec[i] == '\v' ||
             Rec[i] == '\0') {
      RLine.nc = i - ist;
      RLine.skip = 1;
      break;
    }
  }

  return RLine;

}

#define MINV(a, b)  (((a) < (b)) ? (a) : (b))
#define MAXCHR      80

static void
AF_printInfoLine(FILE *fpinfo, const char Preamble[], const char String[],
                 int nchr)

{
  int nm, np;
  char Line[MAXCHR+2];

  np =(int) strlen(Preamble);
  nm = MINV(nchr, MAXCHR+1 - np);
  memcpy(Line, String, nm);    /* One character extra for STstrDots */
  Line[nm] = '\0';
  fprintf(fpinfo, "%s%s\n", Preamble, STstrDots(Line, MAXCHR - np));
}
