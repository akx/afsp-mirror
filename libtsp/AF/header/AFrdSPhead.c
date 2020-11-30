/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  AFILE *AFrdSPhead(FILE *fp)

Purpose:
  Get file format information from a NIST SPHERE audio file

Description:
  This routine reads the header for a NIST SPHERE audio file. The header
  information is used to set the file data format information in the audio file
  pointer structure.

  NIST SPHERE audio file header:
   Offset Length Type    Contents
      0     8    char   File identifier ("NIST_1A\n")
      8     8    char   Header length ("   1024\n")
     16    ...   struct Object-oriented records, e.g.
                          "speaker_id -s3 001\n
                           sample_rate -i 16000\n
                          ...
                           end_head\n"
   1024    ...   --     Audio data

Parameters:
  <-  AFILE *AFrdSPhead
      Audio file pointer for the audio file
   -> FILE *fp
      File pointer for the file

Author / revision:
  P. Kabal  Copyright (C) 2020
  $Revision: 1.104 $  $Date: 2020/11/26 11:29:47 $

-------------------------------------------------------------------------*/

#include <assert.h>
#include <setjmp.h>
#include <stdlib.h>         /* strtod */
#include <string.h>         /* memchr */

#include <libtsp.h>
#include <AFpar.h>
#include <libtsp/nucleus.h>
#include <libtsp/AFheader.h>
#include <libtsp/AFinfo.h>
#include <libtsp/AFmsg.h>

/* NIST SPHERE file identifier as a character string */
#define SPH_ident  "NIST_1A\n"
#define SPH_hsize  "   1024\n"
#define SPH_hend   "end_head\n"

#define LHEAD   1024
#define SPH_LI   (((int) sizeof(SPH_ident)) - 1)
#define SPH_LH   (((int) sizeof(SPH_hsize)) - 1)
#define SPH_LE   (((int) sizeof(SPH_hend)) - 1)

#define SPH_TYPE_INVALID  0
#define SPH_TYPE_INTEGER  1
#define SPH_TYPE_REAL     2
#define SPH_TYPE_STRING   3

/*
NIST_1A Format Specification, 1024 byte ASCII header:
NIST_1A\n
   1024\n

The first line specifies the header type and the second line specifies the
header length. Each of these lines are 8 bytes long.

The remaining object-oriented variable portion is composed of object-type-value
"triple" lines which have the following format:

<LINE> --> <TRIPLE>\n |
           <COMMENT>\n |
           <TRIPLE><COMMENT>\n

  <TRIPLE> --> <OBJECT><space><TYPE><space><VALUE><OPT-SPACES>

    <OBJECT> --> <PRIMARY-SUBOBJECT> |
                 <PRIMARY-SUBOBJECT><SECONDARY-SUBOBJECT>

    <PRIMARY-SUBOBJECT> --> <ALPHA> | <ALPHA><ALPHA-NUM-STRING>
    <SECONDARY-SUBOBJECT> --> _<ALPHA-NUM-STRING> |
                              _<ALPHA-NUM-STRING><SECONDARY-SUBOBJECT>

    <TYPE> --> -<INTEGER-FLAG> | -<REAL-FLAG> | -<STRING-FLAG>

      <INTEGER-FLAG> --> i
      <REAL-FLAG> --> r
      <STRING-FLAG> --> s<DIGIT-STRING>

    <VALUE> --> <INTEGER> | <REAL> | <STRING>  (depending on object type)

      <INTEGER> --> <SIGN><DIGIT-STRING>
      <REAL> --> <SIGN><DIGIT-STRING>.<DIGIT-STRING>

    <OPT-SPACES> --> <SPACES> | NULL

  <COMMENT> --> ;<STRING>  (excluding embedded new-lines)

<ALPHA-NUM-STRING> --> <ALPHA-NUM> | <ALPHA-NUM><ALPHA-NUM-STRING>
<ALPHA-NUM> --> <DIGIT> | <ALPHA>
<ALPHA> --> a | ... | z | A | ... | Z
<DIGIT-STRING> --> <DIGIT> | <DIGIT><DIGIT-STRING>
<DIGIT> --> 0 | ... | 9
<SIGN> --> + | - | NULL
<SPACES> --> <space> | <SPACES><space>
<STRING> -->  <CHARACTER> | <CHARACTER><STRING>
<CHARACTER> --> char(0) | char(1) | ... | char(255)

The following fields are required for proper SPHERE I/O handling.

  Field_name         Field_type
  ----------         ----------
  sample_count           -i      (number of samples per channel)
  sample_n_bytes         -i
  channel_count          -i

The following fields are optional:

  Field_name         Field_type
  ----------         ----------
  sample_checksum        -i
  sample_coding          -s       (If missing, defaults to 'pcm')
  sample_byte_format     -s       (If missing, defaults to the Host's
                                   natural byte format.)
The following fields are conditionally required:

  Field_name         Field_type   Required if:
  ----------         ----------   ------------
  sample_rate            -i       (the 'sample_coding' field is missing, or
                                   contains 'pcm' or 'ulaw')

Possible values for these fields are:

  sample_count -> 1 .. MAXINT
  sample_n_bytes -> 1 | 2
  channel_count -> 1 .. 32
  sample_byte_format -> 01 | 10 | 1 (also obsolete 'shortpack-v0')
  sample_checksum -> 0 .. 32767
  sample_rate -> 1 .. MAXINT
  sample_coding -> pcm | ulaw | pcm,embedded-shorten-vX.X |
                   ulaw,embedded-shorten-vX.X |
                   pcm,embedded-wavpack-X.X |
                   ulaw,embedded-wavpack-X.X |
                   pcm,embedded-shortpack-X.X

The single object "end_head" marks the end of the active header and the
remaining unused header space is undefined. A sample header is included below.

Example SPHERE header from the TIMIT corpus (NIST Speech Disc 1-1.1):
NIST_1A
   1024
database_id -s5 TIMIT
database_version -s3 1.0
utterance_id -s8 aks0_sa1
channel_count -i 1
sample_count -i 63488
sample_rate -i 16000
sample_min -i -6967
sample_max -i 7710
sample_n_bytes -i 2
sample_byte_format -s2 01
sample_sig_bits -i 16
end_head

*/
/* To decode the NIST SPHERE header information, this routine first squirrels
   away all of the information as information records in the audio file
   parameter structure. The OBJECT becomes the information record identifier,
   after adding a terminating ':'. For real and integer VALUE's, the text is
   placed in the record text. String VALUE's can in theory contain any of the
   256 possible characters. The limitations of the text record format means
   some modifications occur. For instance NUL characters are changed to
   line-feed characters. Also trailing white-space is eliminated in the record
   text.

  The records that describe the data format are extracted and used to set the
  file data format parameters.
*/

/* setjmp / longjmp environment */
extern jmp_buf AFR_JMPENV;

/* Local function prototypes */
static int
SPH_checkID(const char Header[]);
static int
SPH_dataFormat(FILE *fp, struct AF_read *AFr);
static char *
SPH_decTriple(char *p_st, char *p_last, struct AF_info *AFInfo);
static int
SPH_decType(char *p_typ, int Ltyp, int *Lval);
static int
SPH_EoH(char *p_st, char *p_NL);
static char *
SPH_findNL(char *p_st, char *p_last);
static char *
SPH_findSP(char *p_st, char *p_last);

AF_READ_DEFAULT(AFr_default); /* Define the AF_read defaults */


AFILE *
AFrdSPhead(FILE *fp)

{
  AFILE *AFp;
  struct AF_read AFr;
  char Header[LHEAD];
  char Info[LHEAD];
  long int poffs, offs;
  char *p_st, *p_last, *p_NL;
  int EoH, hEoF, hend;

/* Set the long jump environment; on error return a NULL */
  if (setjmp(AFR_JMPENV))
    return NULL;  /* Return from a header read error */

/* Defaults and initial values */
  AFr = AFr_default;
  AFr.RInfo.Info = Info;
  AFr.RInfo.N = 0;
  AFr.RInfo.Nmax = LHEAD;

/* Read the header ID strings */
  poffs = 0;
  offs  = RHEAD_SN(fp, &Header[0], SPH_LI);
  offs += RHEAD_SN(fp, &Header[SPH_LI], SPH_LH);
  AFsetChunkLim("NIST", poffs, offs, &AFr.ChunkInfo);
  poffs = offs;

/* Check the file identifier / header length */
  if (SPH_checkID(Header))
    return NULL;

/* Read the rest of the header */
  offs += RHEAD_SN(fp, &Header[SPH_LI + SPH_LH], LHEAD - offs);
  assert(offs == LHEAD);

/* Pick off information records in the header */
  p_st = &Header[SPH_LI + SPH_LH];
  p_last = &Header[LHEAD-1];
  EoH = 0;
  while (p_st <= p_last) {

    /* Find the next NL */
    p_NL = SPH_findNL(p_st, p_last);
    if (p_NL == NULL)
      break;

    /* Skip over empty lines or full line comments */
    if (p_NL == p_st || *p_st == ';') {
      p_st = p_NL + 1;
      continue;
    }

    /* Look for an end of header line */
    EoH = SPH_EoH(p_st, p_NL);
    if (EoH)
      break;

    /* Decode Triple, store as an Info Record */
    p_NL = SPH_decTriple(p_st, p_last, &AFr.RInfo);
    if (p_NL == NULL)
      break;

    p_st = p_NL + 1;                /* Skip over NL */
  }

  if (!EoH) {
    UTwarn("AFrdSPhead - %s", AFM_SP_BadHead);
    return NULL;
  }

/* Mark the header chunks */
  hEoF = (int) (p_st - &Header[0]);   /* Offset into Header */
  AFsetChunkLim("fmt ", poffs, hEoF, &AFr.ChunkInfo);
  p_st = p_NL + 1;                    /* Skip over NL */
  hend = (int) (p_st - &Header[0]);
  AFsetChunkLim("EoH ", hEoF, hend, &AFr.ChunkInfo);
  AFsetChunkLim("skip", hend, offs, &AFr.ChunkInfo);

/* Decode the data format, Fill in AFr data specs */
  if (SPH_dataFormat(fp, &AFr))
    return NULL;
  AFsetChunkLim("data", offs, offs + AFr.NData.Ldata, &AFr.ChunkInfo);

/* Set the parameters for file access */
  AFp = AFsetRead(fp, FT_SPHERE, &AFr, AF_NOFIX);

  return AFp;
}

/* NIST SPHERE
   Specs Require:
     PCM and mu-law:    Nchan, Nframe, NbS, Sfreq
   Specs Optional:
     SwapCode, defaulting to native
     sample_format, defaulting to PCM
  Notes:
   1. Only 1-byte mu-law and 2 byte PCM (uncompressed) formats are supported.
   2. Having a swap code default of "native" for multi-byte data is not
      reasonable. This routine enforces specification of the byte order for PCM
      data.
   3. Both "ulaw" and "mu-law" are accepted. The "pculaw" code appears in some
      SPHERE files. It is a bit reversed version of mu-law.
   4. Some multi-channel NIST SPHERE files have "channels_interleaved -s4 TRUE".
      The AFsp routines do not support a non-interleaved multi-channel input.
   5. Some NIST SPHERE files use the sample_format record to specify the
      compression type. This record is normally used to specify the byte order.
   6. Some NIST files (switchboard data base, for instance) have a sample_count
      which is the total number of samples, not the samples/channel as specified
      in the NIST SPHERE documentation. Here we check for a sample_count which
      is too large for the file size and fix it.
*/

static const char *RecIDSC[] = {"sample_coding:", NULL};
static const char *RecIDSF[] = {"sample_byte_format:", NULL};
static const char *RecIDCI[] = {"channels_interleaved:", NULL};

static int
SPH_dataFormat(FILE *fp, struct AF_read *AFr)
{
 int DF;
 enum UT_DS_T SwapCode;
 const char *p;
 int Lw;
 long int Nchan, Nframe;
 double Sfreq;

/* Decode the sample format */
/* Special case: "shortpack-v0" appears in "sample_format" */
  DF = 1;       /* PCM as default */
  p = AFgetInfoRec(RecIDSF, &AFr->RInfo);
  if (p != NULL && strcmp (p, "shortpack-v0") == 0)
    DF = 0;   /* Not supported */
  else {
    p = AFgetInfoRec(RecIDSC, &AFr->RInfo);
    if (p != NULL) {
      if (strcmp("pcm", p) == 0)
        DF = 1;
      else if (strcmp("ulaw", p) == 0 || strcmp("mu-law", p) == 0)
        DF = 2;   /* mu-law */
      else if (strcmp("pculaw", p) == 0)
        DF = 3;   /* bit-reversed mu-law */
      else
        DF = 0;
    }
  }
  if (DF == 0) {      /* p is not NULL */
    UTwarn("AFrdSPhead - %s: \"%s\"", AFM_SP_UnsData, p);
    return 1;
  }

/* Required for PCM and/or mu-law */
  SwapCode = AFgetInfoSwap(&AFr->RInfo);
  Lw     = AFgetInfoBytes(&AFr->RInfo);
  Nchan  = AFgetInfoChan(&AFr->RInfo);
  Sfreq  = AFgetInfoSfreq(&AFr->RInfo);
  Nframe = AFgetInfoFrame(&AFr->RInfo);

  if (Nchan == 0 || Nframe == AF_NFRAME_UNDEF ||
      Sfreq == AF_SFREQ_UNDEF  || (DF == 1 &&
                                  (SwapCode == DS_UNDEF || Lw == 0))) {
    UTwarn("AFrdSPhead - %s", AFM_SP_IncFmt);
    return 1;
  }

  if (DF == 1 && Lw != 2) {
    UTwarn("AFrdSPhead - %s", AFM_SP_UnsPCM);
    return 1;
  }
  if ((DF == 2 || DF == 3) && Lw > 1) {      /* Allow Lw == 0 */
    UTwarn("AFrdSPhead - %s", AFM_SP_UnsMulaw);
    return 1;
  }

  /* Check for "channels_interleaved" */
  if (Nchan > 1) {
    p = AFgetInfoRec(RecIDCI, &AFr->RInfo);
    if (p != NULL && strcmp (p, "TRUE") != 0)
      UTwarn("AFrdSPhead - %s", AFM_SP_NoInter);
  }

  /* sample_count should return samples/channel, but in some cases is too large
     by a factor of Nchan */
  if (Nchan > 1 && FLseekable(fp) &&
      Lw * Nframe * Nchan + LHEAD > FLfileSize(fp) && Nframe % Nchan == 0)
    Nframe = Nframe / Nchan;

  /* Fill in the AFr data specs */
  if (DF == 1)
    AFr->DFormat.Format = FD_INT16;
  else if (DF == 2) {
    AFr->DFormat.Format = FD_MULAW8;
    Lw = 1;
    SwapCode = DS_NATIVE;
  }
  else {
    AFr->DFormat.Format = FD_MULAWR8;
    Lw = 1;
    SwapCode = DS_NATIVE;
  }
  AFr->DFormat.Swapb = SwapCode;

  AFr->NData.Ldata = Lw * Nframe * Nchan;
  AFr->NData.Nsamp = Nframe * Nchan;
  AFr->NData.Nchan = Nchan;
  AFr->Sfreq = Sfreq;

  return 0;
}


/* Check the header ID / length */


static int
SPH_checkID(const char Header[])
{
  if (memcmp(Header, SPH_ident, SPH_LI) != 0 ||
      memcmp(&Header[SPH_LI], SPH_hsize, SPH_LH) != 0) {
    UTwarn("AFrdSPhead - %s", AFM_SP_BadId);
    return 1;
  }

  return 0;
}

/* Check for end of header */


static int
SPH_EoH(char *p_st, char *p_NL)
{
  int nc;

  nc = (int) (p_NL - p_st + 1);
  if (nc == SPH_LE && memcmp(SPH_hend, p_st, nc) == 0)
    return 1;

  return 0;
}

/* Find the next NL in the header string */


static char *
SPH_findNL(char *p_st, char *p_last)

{
  char *p_NL;

  p_NL = memchr(p_st, '\n', (int) (p_last - p_st + 1));

  return p_NL;
}

/* Decode a TRIPLE */
/* Returns a possibly updated NL location */
/* Modifies the header, inserting '\0' and ':' to create substrings */

static char *
SPH_decTriple(char *p_st, char *p_last, struct AF_info *AFInfo)

{
  char *p_SP, *p_NL;
  char *p_obj, *p_typ, *p_sc;
  int Lobj, Ltyp, Type, Lval;

  /* Find the next NL (again) */
  p_NL = SPH_findNL(p_st, p_last);
  if (p_NL == NULL)
    return NULL;

  /* Find the first space */
  p_SP = SPH_findSP(p_st, p_NL);
  if (p_SP == NULL)
    return NULL;

  /* Isolate the OBJECT */
  p_obj = p_st;
  Lobj = (int) (p_SP - p_st);
  if (Lobj <= 0)
    return NULL;
  p_st = p_SP + 1;

  /* Find the next space */
  p_SP = SPH_findSP(p_st, p_NL);
  if (p_SP == NULL)
    return NULL;

  /* Isolate the TYPE code */
  p_typ = p_st;
  Ltyp = (int) (p_SP - p_st);
  if (Ltyp < 2)
    return NULL;
  *p_SP = '\0';     /* Terminate the type string */
  p_st = p_SP + 1;

  /* Decode the type code, Lval is zero, except for string values */
  Type = SPH_decType(p_typ, Ltyp, &Lval);
  if (Type == SPH_TYPE_INVALID)
    return NULL;

  /* Find a trailing comment on integer/real values */
  if (Type != SPH_TYPE_STRING) {
    Lval = (int) (p_NL - p_st);
    p_sc = memchr(p_st, ';', Lval);
    if (p_sc != NULL)
      Lval = (int) (p_sc - p_st);     /* AFaddInfoRec trims white-space */
  }

  /* Handle the case of a string value which overlaps the new-line */
  while (p_st + Lval > p_NL) {
    p_NL = SPH_findNL(p_NL + 1, p_last);
    if (p_NL == NULL)
      return NULL;
  }

  /* Create the info record identifier string (modifies Header) */
  *(p_obj + Lobj) = ':';          /* Overwrites ' ' */
  *(p_obj + Lobj + 1) = '\0';     /* Overwrites '-' */

  /* Add an information record */
  AFaddInfoRec(p_obj, p_st, Lval, AFInfo);

  return p_NL;
}

/* Find the next blank in the header string */


static char *
SPH_findSP(char *p_st, char *p_last)

{
  char *p_SP;
  p_SP = memchr(p_st, ' ', (int) (p_last - p_st + 1));

  return p_SP;
}

/* Decode TYPE */
/* The input string is NUL terminated of length Ltyp (not including the NUL
   terminator.
*/


static int
SPH_decType(char *p_typ, int Ltyp, int *Lval)

{
  int Type, Lv;
  char *p_sl, *p;

  Type = SPH_TYPE_INVALID;
  Lv = 0;
  if (Ltyp == 2 && memcmp("-i", p_typ, 2) == 0)
    Type = SPH_TYPE_INTEGER;
  else if (Ltyp == 2 && memcmp("-r", p_typ, 2) == 0)
    Type = SPH_TYPE_REAL;
  else if (Ltyp > 2 && memcmp("-s", p_typ, 2) == 0) {
    p_sl = p_typ + 2;
    Lv = (int) strtol(p_sl, &p, 10);
    if (*p == '\0' && Lv > 0)   /* Check for successful conversion */
      Type = SPH_TYPE_STRING;
  }

  *Lval = Lv;
  return Type;
}
