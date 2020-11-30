/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int AF_write AFpreSetWPar(enum AF_FTW_T FtypeW, enum AF_FD_T Format,
                            long int Nchan, double Sfreq, struct AF_write *AFw)

Purpose:
  Create a parameter structure for writing an audio file

Description:
  This routine creates a parameter structure for writing to an audio file. The
  structure is filled in with default values, values supplied as input, or user
  supplied options (via for example, AFsetInfo, AFsetNHpar, or AFsetSpeaker)

  Fields set from the input values:
    AFw->DFormat.Format
    AFw->Sfreq
    AFw->Nchan
    AFw->FtypeW
  Fields set from options:
    AFw->DFormat.NbS
    AFw->SpkrConfig
    AFw->WInfo        Space allocated in AF_genInfo, de-allocated next call
    AFw->Nframe
  Fields preset to tentative values
    AFw->DFormat.ScaleF
    AFw->DFormat.Swapb

Parameters:
  <-  int AFpreSetWPar
      Error flag, 0 for no error
   -> enum AF_FTW_T FtypeW
      File type code
   -> enum AF_FD_T Format
      Data format code
   -> long int Nchan
      Number of channels
   -> double Sfreq
      Sampling frequency
  <-  struct AF_write *AFw
      Pointer to an audio write structure

Author / revision:
  P. Kabal  Copyright (C) 2020
  $Revision: 1.60 $  $Date: 2020/11/26 11:33:35 $

-------------------------------------------------------------------------*/

#include <libtsp/sysOS.h>
#if (SY_OS == SY_OS_WINDOWS)
#  define _CRT_SECURE_NO_WARNINGS     /* Allow sprintf */
#endif

#include <assert.h>
#include <math.h> /* floor */
#include <string.h>

#include <libtsp.h>
#include <AFpar.h>
#include <libtsp/nucleus.h>
#include <libtsp/AFheader.h>
#include <libtsp/AFinfo.h>
#include <libtsp/AFmsg.h>

#define NELEM(array)  ((int) (sizeof(array) / (int) sizeof(array[0])))
#define MCOPYN(src,dest,n)   memcpy((void *) (dest), \
                                    (const void *) (src), n)

/* Local functions */
static int
AF_checkDT(enum AF_FTW_T FtypeW, enum AF_FD_T Format);
static int
AF_checkIEEE(enum AF_FTW_T FtypeW, enum AF_FD_T Format);
static int
AF_setNbS(enum AF_FD_T Format, int NbS);
static void
AF_setSpeaker(const unsigned char *SpkrI, int Nchan,
              unsigned char *SpkrConfig);
static struct AF_info
AF_genInfo(const enum AF_STDINFO_T StdInfo, const struct AF_info *UInfo,
           const struct AF_write *AFw);
static int
AF_stdInfo(const struct AF_write *AFw, char *SInfo);


int
AFpreSetWPar(enum AF_FTW_T FtypeW, enum AF_FD_T Format, long int Nchan,
             double Sfreq, struct AF_write *AFw)

{
/* File type and data format */
  AFw->FtypeW = FtypeW;
  AFw->DFormat.Format = Format;

/* Set file data parameters */
  AFw->Sfreq = Sfreq;
  AFw->DFormat.FullScale = AF_FULLSCALE_DEFAULT;  /* Set below after checks */
  AFw->DFormat.Swapb = DS_NATIVE;       /* Tentative */
  AFw->DFormat.NbS = AF_setNbS(AFw->DFormat.Format, AFopt.NbS);
  AFw->Nchan = Nchan;
  AFw->Nframe = AFopt.Nframe;  /* From the options structure */
  AF_setSpeaker(AFopt.SpkrConfig, Nchan, AFw->SpkrConfig);

/* Error Checks */
  if (AF_checkDT(FtypeW, AFw->DFormat.Format))
    return 1;
  if (AF_checkIEEE(FtypeW, AFw->DFormat.Format))
    return 1;

  if (AFw->Nchan <= 0L) {
    UTwarn("AFopnWrite - %s: %ld", AFM_BadNchan, AFw->Nchan);
    return 1;
  }
  if (AFw->Nframe != AF_NFRAME_UNDEF && AFw->Nframe < 0L) {
    UTwarn("AFopnWrite - %s: %ld", AFM_BadNframe, AFw->Nframe);
    return 1;
  }

  /* After data type has been verified, set the full scale value */
  AFw->DFormat.FullScale = AF_FULLSCALE[AFw->DFormat.Format];

  /* Info string:
     The space for the info string is allocated within AF_genInfo.  It is
     reclaimed each time AF_genInfo is called.
  */
  AFw->WInfo = AF_genInfo(AFopt.StdInfo, &AFopt.UInfo, AFw);

  return 0;
}

/* Check for file type / data type compatibility */


static int
AF_checkDT(enum AF_FTW_T FtypeW, enum AF_FD_T Format)

{
  int Err;

/*
  undef  Alaw  ulaw ulawR uint8  int8 int16 int24 int32 flt32 flt64 text16 text
*/
  static const int DT_AU[] = {
     0,    1,    1,    0,    0,    1,    1,    1,    1,    1,    1,    0,    0};
  static const int DT_WAVE[] = {
     0,    1,    1,    0,    1,    0,    1,    1,    1,    1,    1,    0,    0};
  static const int DT_AIFF_C[] = {
     0,    1,    1,    0,    0,    1,    1,    1,    1,    1,    1,    0,    0};
  static const int DT_AIFF_C_SOWT[] = {
     0,    0,    0,    0,    0,    1,    1,    1,    1,    0,    0,    0,    0};
  static const int DT_AIFF[] = {
     0,    0,    0,    0,    0,    1,    1,    1,    1,    0,    0,    0,    0};
  static const int DT_NH[] = {
     0,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1};
  static const int DT_TXAUD[] = {
     0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    1,    1};

  assert(NELEM(DT_AU) == AF_NFD     && NELEM(DT_WAVE) == AF_NFD &&
         NELEM(DT_AIFF_C) == AF_NFD && NELEM(DT_AIFF_C_SOWT) == AF_NFD &&
         NELEM(DT_AIFF) == AF_NFD   && NELEM(DT_NH) == AF_NFD &&
         NELEM(DT_TXAUD) == AF_NFD);

  Err = 0;
  if (Format < 1 || Format >= AF_NFD) {
    UTwarn("AFopnWrite - %s: \"%d\"", AFM_BadDataC, Format);
    Err = 1;
    return Err;
  }

  switch (FtypeW) {
  case FTW_AU:
    if (DT_AU[Format] == 0) {
      UTwarn("AFopnWrite - %s: \"%s\"", AFM_AU_UnsData, AF_DTN[Format]);
      Err = 1;
    }
    break;
  case FTW_WAVE:
  case FTW_WAVE_EX:
  case FTW_WAVE_NOEX:
    if (DT_WAVE[Format] == 0) {
      UTwarn("AFopnWrite - %s: \"%s\"", AFM_WV_UnsData, AF_DTN[Format]);
      Err = 1;
    }
    break;
  case FTW_AIFF_C:
    if (DT_AIFF_C[Format] == 0) {
      UTwarn("AFopnWrite - %s: \"%s\"", AFM_AIFFC_UnsData, AF_DTN[Format]);
      Err = 1;
    }
    break;
  case FTW_AIFF_C_SOWT:
    if (DT_AIFF_C_SOWT[Format] == 0) {
      UTwarn("AFopnWrite - %s: \"%s\"", AFM_AIFFC_X_UnsData, AF_DTN[Format]);
      Err = 1;
    }
    break;
  case FTW_AIFF:
    if (DT_AIFF[Format] == 0) {
      UTwarn("AFopnWrite - %s: \"%s\"", AFM_AIFF_UnsData, AF_DTN[Format]);
      Err = 1;
    }
    break;
  case FTW_NH_NATIVE:
  case FTW_NH_SWAP:
  case FTW_NH_EL:
  case FTW_NH_EB:
    if (DT_NH[Format] == 0) {
      UTwarn("AFopnWrite - %s: \"%s\"", AFM_NH_UnsData, AF_DTN[Format]);
      Err = 1;
    }
    break;
  case FTW_TXAUD:
    if (DT_TXAUD[Format] == 0) {
      UTwarn("AFopnWrite - %s: \"%s\"", AFM_NH_UnsData, AF_DTN[Format]);
      Err = 1;
    }
    break;
  default:
    UTwarn("AFopnWrite - %s: %d", AFM_BadFtypeC, FtypeW);
    Err = 1;
    break;
  }
  return Err;
}

/* Check for IEEE data format */

static int
AF_checkIEEE(enum AF_FTW_T FtypeW, enum AF_FD_T Format)

{
  int Err;

  Err = 0;
  switch (FtypeW) {
  case FTW_AU:
    if ((Format == FD_FLOAT32 || Format == FD_FLOAT64) && !UTcheckIEEE ()) {
      UTwarn("AFwrAUhead - %s", AFM_NoIEEE);
      Err = 1;
    }
    break;
  case FTW_WAVE:
  case FTW_WAVE_EX:
  case FTW_WAVE_NOEX:
    if ((Format == FD_FLOAT32 || Format == FD_FLOAT64) && !UTcheckIEEE ()) {
      UTwarn("AFwrWVhead - %s", AFM_NoIEEE);
      Err = 1;
    }
    break;
  case FTW_AIFF_C:
    if ((Format == FD_FLOAT32 || Format == FD_FLOAT64) && !UTcheckIEEE ()) {
      UTwarn("AFwrAIhead - %s", AFM_NoIEEE);
      Err = 1;
    }
    break;
  default:
    break;
  }

  return Err;
}

/* Set and check the number of bits per sample
   Res is the data word length:
   - Special values for text files (Res = 0 and NbS = 0)
   - Integer formats NbS > 0 and NbS <= Res
   - Non-integer formats NbS = Res
   Normally 8*ceil(NbS/8) = Res. This is enforced by some audio file formats.
   Here combinations such as 23/32 are passed as OK.
*/


static int
AF_setNbS(enum AF_FD_T Format, int NbS)

{
  int Res;

  Res = 8 * AF_DL[Format];
  if (NbS == 0)
    NbS = Res;

  /* Res == 0 for text files */
  if (Res > 0 && (NbS <= 0 || NbS > Res)) {
    UTwarn(AFMF_InvNbS, "AFopnWrite -", NbS, Res);
    NbS = Res;
  }
  else if (Res == 0 && NbS != 0) {
    UTwarn("AFopnWrite - %s", AFM_BadNbS);
    NbS = Res;
  }

  /* Warn and reset NbS for inappropriate formats */
  if (NbS != Res &&
      !(Format == FD_UINT8 || Format == FD_INT8 || Format == FD_INT16 ||
        Format == FD_INT24 || Format == FD_INT32)) {
    UTwarn("AFopnWrite - %s", AFM_InaNbS);
    NbS = Res;
  }

  return NbS;
}

/* Set the speaker configuration */


static void
AF_setSpeaker(const unsigned char *SpkrI, int Nchan, unsigned char *SpkrConfig)

{
  int Nspkr;

  if (SpkrI != NULL) {
    SpkrConfig[0] = '\0';
    Nspkr = (int) strlen((const char *) SpkrI);
    if (Nspkr > Nchan) {
      Nspkr = Nchan;
      UTwarn("AFopnWrite - %s", AFM_ExSpkr);
    }
    if (Nspkr > AF_MAXN_SPKR)
      UTwarn("AFopnWrite - %s", AFM_XSpkr);
    else
      STcopyNMax((const char *) SpkrI, (char *) SpkrConfig, Nspkr,
                 AF_MAXN_SPKR);
  }
  else
    SpkrConfig[0] = '\0';

  return;
}

#define MAX_SINFO 512  /* Maximum size of Sinfo */
                       /* See AF_stdInfo */


static struct AF_info
AF_genInfo(const enum AF_STDINFO_T StdInfo, const struct AF_info *UInfo,
           const struct AF_write *AFw)

{
  char SInfo[MAX_SINFO];
  int Nc, ns, nu;
  static struct AF_info WInfo = {NULL, 0, 0};

  /* Size of the standard info records */
  if (StdInfo == AF_STDINFO_ON)
    ns = AF_stdInfo(AFw, SInfo);
  else {
    ns = 0;
    SInfo[0] = '\0';
  }

/* Size of the user specified info records */
  if (UInfo->Info != NULL)
    nu = UInfo->N;
  else
    nu = 0;

/* Allocate storage */
  Nc = nu + ns;
  UTfree((void *) WInfo.Info);
  WInfo.Info = (char *) UTmalloc(Nc);
  WInfo.Nmax = Nc;

/* Form the output information records */
  MCOPYN(SInfo, WInfo.Info, ns);
  if (UInfo->Info != NULL)
    MCOPYN(UInfo->Info, &WInfo.Info[ns], nu);
  WInfo.N = Nc;

/* Return a pointer to the information records */
  return WInfo;
}

/* Generate the standard information records
     "date: xxx"
   + "program: xxx"
   + "sample_rate: xxx"       (only if non-integer)
   + "bits_per_sample: xx/yy" (only if not equal to full precision)
   + "loudspeakers: xx yy"    (only if specified)

   The returned function value is the number of characters.
*/

static int
AF_stdInfo(const struct AF_write *AFw, char *Sinfo)

{
  int N, NbS, Res, Nspkr, NO;
  char *p;
  double Sfreq;
  const unsigned char *SpkrConfig;
  char SpkrNames[AF_MAXNC_SPKRNAMES+1];
  static const char *RecIDchan[]      = AF_INFOID_NCHAN;
  static const char *RecIDdate[]      = AF_INFOID_DATE;
  static const char *RecIDnbs[]       = AF_INFOID_NBS;
  static const char *RecIDnframe[]    = AF_INFOID_NFRAME;
  static const char *RecIDfullscale[] = AF_INFOID_FULLSCALE;
  static const char *RecIDprog[]      = AF_INFOID_PROG;
  static const char *RecIDsfreq[]     = AF_INFOID_SFREQ;
  static const char *RecIDspkr[]      = AF_INFOID_SPKR;

/* Sinfo length (size MAX_SINFO)
   Each record has a fixed part ("date: ") and a variable part ("...").
  Record          Fixed    Variable
  Name            Length    Max      Total
   Date             6        40       46+1  *actually 6+23
   Program Name     9        40       49+1  *long names may get truncated
   Channels        10        20       30+1  long int
   Frames           8        20       28+1  long int
   Sampling Freq.  13        15       28+1  double
   Number of Bits  13         5       18+1  int/int
   Loudspeakers    14       172      186+1  AF_MAXNC_SPKRNAMES = 172
                                     -----
                                      392
*/
  /* "date: 1994-01-23 14:59:53 UTC" */
  N = sprintf(Sinfo, "%s %.40s", RecIDdate[0], UTdate(3));
  ++N;      /* Move past null */

  /* "program: <Name of program>" */
  p = UTgetProg();
  if (*p != '\0') {
    N += sprintf(&Sinfo[N], "%s %.40s", RecIDprog[0], p);
    ++N;
  }

  /* "sample_rate: 1.234567E+123" */
  Sfreq = AFw->Sfreq;
  if (AFw->FtypeW == FTW_TXAUD || (Sfreq > 0.0 && Sfreq != floor (Sfreq))) {
    N += sprintf(&Sinfo[N], "%s %.7g", RecIDsfreq[0], Sfreq);
    ++N;
  }

  if (AFw->FtypeW == FTW_TXAUD) {
    N += sprintf(&Sinfo[N], "%s %ld", RecIDchan[0], AFw->Nchan);
    ++N;
    if (AFw->Nframe != AF_NFRAME_UNDEF) {
      N += sprintf(&Sinfo[N], "%s %ld", RecIDnframe[0], AFw->Nframe);
      ++N;
    }
    N += sprintf(&Sinfo[N], "%s %.7g", RecIDfullscale[0],
                 AFw->DFormat.FullScale);
    ++N;
  }

  /* "bits_per_sample: 24/32" */
  /* AIFF/AIFC have this, as well as WAVE-ex => WAVE will be promoted to
     WAVE-ex */
  if (!(AFw->FtypeW == FTW_WAVE        || AFw->FtypeW == FTW_WAVE_EX ||
        AFw->FtypeW == FTW_AIFF        || AFw->FtypeW == FTW_AIFF_C) ||
        AFw->FtypeW == FTW_AIFF_C_SOWT || AFw->FtypeW == FTW_TXAUD) {
    Res = 8 * AF_DL[AFw->DFormat.Format];
    NbS = AFw->DFormat.NbS;
    if (NbS != 0 && NbS < Res) {
      N += sprintf(&Sinfo[N], "%s %d/%d", RecIDnbs[0], NbS, Res);
      ++N;
    }
  }

  /* "loudspeakers: FL FR ..." */
  SpkrConfig = AFw->SpkrConfig;
  Nspkr = (int) strlen((const char *) SpkrConfig);
  if (Nspkr > 0) {
    NO = AFspeakerNames(SpkrConfig, SpkrNames, AF_MAXNC_SPKRNAMES);
    if (NO != Nspkr)
      UTwarn("AFopnWrite - %s", AFM_XSpkr);
    else {
      N += sprintf(&Sinfo[N], "%s %s", RecIDspkr[0], SpkrNames);
      ++N;
    }
  }

  assert(N <= MAX_SINFO);

  return N;
}
