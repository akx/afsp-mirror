/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  enum AF_FT_T AFfindFtype (FILE *fp)

Purpose:
  Identify the format of an input audio file

Description:
  This routine tries to determine the format for an input audio file.  It checks
  the header of the file to identify the type of file.  This check consists of
  looking for a file magic value in a known location in the header of the file.
  The file must be positioned at the start of file. The file position is
  restored to the start of file.

  The routine AFsetNHpar can be used to control the behaviour of this routine
  for audio files which do not have a header (raw audio files) or audio files
  which have a header which is not recognized by this routine.

Parameters:
  <-  enum AF_FT_T AFfindType
      Integer code specifying the type of file,
        FT_ERROR    = -1, error
        FT_UNKNOWN  = 0, unknown audio file format
        FT_NH       = 1, headerless (non-standard or no header) audio file
        FT_AU       = 2, AU audio file
        FT_WAVE     = 3, WAVE file
        * FT_WAVE_EX     = 4, WAVE file (extensible format)
        * FT_WAVE_BWF    = 5, WAVE file (with BWF extension)
        * FT_WAVE_EX_BWF = 6 WAVE file (extensible format, with BWF extension)
        FT_AIFF     = 7, AIFF sound file
        FT_AIFF_C   = 8, AIFF-C sound file
        FT_SPHERE   = 9, NIST SPHERE audio file
        FT_ESPS     = 10, ESPS sampled data feature file
        FT_SF       = 11, IRCAM soundfile
        FT_SPPACK   = 12, SPPACK file
        FT_INRS     = 13, INRS-Telecom audio file
        FT_SPW      = 14, Comdisco SPW Signal file
        FT_NSP      = 15, CSL NSP file
        FT_TEXT     = 16, Text audio file
        FT_UNSUP    = 256, unsupported audio file format
      The file subtypes marked with an asterisk will not be returned by this
      routine; the subtype will be determined when the file header is processed.
   -> FILE *fp
      File pointer associated with the audio file

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.100 $  $Date: 2017/09/22 17:17:36 $

-------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>

#include <libtsp.h>
#include <AFpar.h>
#include <libtsp/nucleus.h>
#include <libtsp/AFheader.h>
#include <libtsp/AFmsg.h>

#define NELEM(array)  ((int) ((sizeof array) / (sizeof array[0])))

/* Index i corresponds to code (i + FT_UNSUP) */
#define FTU_CODE(index) (FT_UNSUP + index)
#define FTU_INDEX(code) (code - FT_UNSUP)

struct AF_magicV {
  long int offs;       /* Byte offset to file magic */
  const char *Magic;   /* File magic string */
  int Len;             /* File magic string length */
};
struct AF_magic {
  enum AF_FT_T Ftype;       /* File type */
  int Lhead;                /* Minimum header length */
  struct AF_magicV Magic1;  /* File magic 1 */
  struct AF_magicV Magic2;  /* File magic 2 */
};

#define MAGIC_S(ftype, lhead, offs, string) \
                {(ftype), (lhead), \
                {(offs), (string), sizeof (string) - 1}, \
                {0L, NULL, 0}}
#define MAGIC_D(ftype, lhead, offs1, string1, offs2, string2) \
                {(ftype), (lhead), \
                {(offs1), (string1), sizeof (string1) - 1}, \
                {(offs2), (string2), sizeof (string2) - 1}}

/* Audio file magic values */
static const struct AF_magic MagicS[] = {

/* ===== Supported audio file formats */
/*         File Type, min length, offset, ID string */
  /* AU audio file */
  MAGIC_S (FT_AU, 24, 0L, ".snd"),
  /* WAVE file */
  MAGIC_D (FT_WAVE, 44, 0L, "RIFF", 8L, "WAVE"),
  /* AIFF sound file */
  MAGIC_D (FT_AIFF, 54, 0L, "FORM", 8L, "AIFF"),
  /* AIFF-C sound file */
  MAGIC_D (FT_AIFF_C, 72, 0L, "FORM", 8L, "AIFC"),
  /* NIST SPHERE audio file */
  MAGIC_S (FT_SPHERE, 1024, 0L, "NIST_1A\n"),
  /* ESPS sampled data feature file */
  MAGIC_S (FT_ESPS, 333, 16L, "\0\0\152\32"),
  /* ESPS sampled data feature file (swapped) */
  MAGIC_S (FT_ESPS, 333, 16L, "\32\152\0\0"),
  /* INRS-Telecom audio file: 6500 Hz */
  MAGIC_S (FT_INRS, 512, 0L, "\313\106\0\040"),
  /* INRS-Telecom audio file: 20000/3 Hz */
  MAGIC_S (FT_INRS, 512, 0L, "\320\106\125\125"),
  /* INRS-Telecom audio file: 8000 Hz */
  MAGIC_S (FT_INRS, 512, 0L, "\372\106\0\0"),
  /* INRS-Telecom audio file: 10000 Hz */
  MAGIC_S (FT_INRS, 512, 0L, "\034\107\0\100"),
  /* INRS-Telecom audio file: 12000 Hz */
  MAGIC_S (FT_INRS, 512, 0L, "\073\107\0\200"),
  /* INRS-Telecom audio file: 16000 Hz */
  MAGIC_S (FT_INRS, 512, 0L, "\172\107\0\0"),
  /* INRS-Telecom audio file: 20000 Hz */
  MAGIC_S (FT_INRS, 512, 0L,"\234\107\0\100"),
  /* IRCAM soundfile, VAX - native */
  MAGIC_S (FT_SF, 1024, 0L, "\144\243\001\0"),
  /* IRCAM soundfile, VAX - byte reversed */
  MAGIC_S (FT_SF, 1024, 0L, "\0\001\243\144"),
  /* IRCAM soundfile, Sun - big-endian */
  MAGIC_S (FT_SF, 1024, 0L, "\144\243\002\0"),
  /* IRCAM soundfile, Sun - little-endian */
  MAGIC_S (FT_SF, 1024, 0L, "\0\002\243\144"),
  /* IRCAM soundfile, MIPS - big-endian */
  MAGIC_S (FT_SF, 1024, 0L, "\144\243\003\0"),
  /* IRCAM soundfile, MIPS - little-endian */
  MAGIC_S (FT_SF, 1024, 0L, "\0\003\243\144"),
  /* IRCAM soundfile, Next - big-endian */
  MAGIC_S (FT_SF, 1024, 0L, "\144\243\004\0"),
  /* SPPACK file */
  MAGIC_S (FT_SPPACK, 512, 252L, "\100\303"),
  MAGIC_S (FT_SPPACK, 512, 252L, "\303\100"),
  /* Comdisco SPW Signal file */
  MAGIC_S (FT_SPW, 208, 0L, "$SIGNAL_FILE 9\n"),
  /* Text audio file */
  MAGIC_S (FT_NSP, 60, 0L, "FORMDS16"),
  MAGIC_S (FT_TXAUD, 5, 0L, "%//\n"),
  MAGIC_S (FT_TXAUD, 6, 0L, "%//\r\n"),

/* ===== Unsupported audio file formats */
  /* RF64/WAVE file */
  MAGIC_D (FTU_CODE(0), 104, 0L, "RF64", 8L, "WAVE"),
  /* RIFX/WAVE file */
  MAGIC_D (FTU_CODE(1), 44, 0L, "RIFX", 8L, "WAVE"),
  /* Comdisco SPW (version 2.8.2 format) */
  MAGIC_S (FTU_CODE(2), 90, 0L, "\n$\n"),
  /* Sound Blaster VOC file */
  MAGIC_S (FTU_CODE(3), 26, 0L, "Creative Voice File\032\032"),
  /* Amiga SVX file */
  MAGIC_D (FTU_CODE(4), 20, 0L, "FORM", 8L, "8SVX"),
  /* Amiga maud file */
  MAGIC_D (FTU_CODE(5), 40, 0L, "FORM", 8L, "MAUD"),
  /* sndtool file */
  MAGIC_S (FTU_CODE(6), 96, 0L, "SOUND\032"),
  /* Psion alaw */
  MAGIC_S (FTU_CODE(7), 32, 0L, "ALawSoundFile**"),
  /* SampleVision file */
  MAGIC_S (FTU_CODE(8), 112, 0L, "SOUND SAMPLE DATA 2.1"),
  /* Audio Visual Research file */
  MAGIC_S (FTU_CODE(9), 128, 0L, "2BIT"),
  /* Entropic Esignal file */
  MAGIC_S (FTU_CODE(10), 48, 0L, "Esignal\n"),
  /* Macintosh HCOM file */
  MAGIC_D (FTU_CODE(11), 128, 65L, "FSSD", 128L, "HCOM"),
  /* Gravis Ultrasound Patch file */
  MAGIC_S (FTU_CODE(12), 240, 0L, "GF1PATCH"),
  /* Gold sample file */
  MAGIC_S (FTU_CODE(13), 10, 0L, "GOLD SAMPLE"),
  /* SRFS file */
  MAGIC_S (FTU_CODE(14), 4, 0L, "SRFS"),
  /* SoundFont 2.0 file */
  MAGIC_D (FTU_CODE(15), 18, 0L, "RIFF", 8L, "sfbk"),
  /* DiamondWare Digitized */
  MAGIC_S (FTU_CODE(16), 56, 0L, "DiamondWare Digitized\n\0\032"),
  /* MIDI file  */
  MAGIC_S (FTU_CODE(17), 4, 0L, "MThd"),
  /* RealAudio file  */
  MAGIC_S (FTU_CODE(18), 4, 0L, ".ra\375"),
  /* MP3 files, *** duplicate codes *** */
  MAGIC_S (FTU_CODE(19), 2, 0L, "\377\373"),
  MAGIC_S (FTU_CODE(19), 2, 0L, "\377\372"),
  /* Nyvalla DSP StdHead files *** duplicate codes *** */
  MAGIC_S (FTU_CODE(20), 1024, 0L, "file=samp\r\n"),
  MAGIC_S (FTU_CODE(20), 1024, 0L, "data=int16\r\nfile=samp\r\n"),
  /* ILS Sampled Data files */
  MAGIC_S (FTU_CODE(21), 512, 124, "\0\203\225\175"),
  /* Littmann E4000 Digital Stethoscope files */
  MAGIC_S (FTU_CODE(22), 51, 22, "E4000 digital stethoscope")
};

/* The file signature tables are a bit messy to maintain
   - The total number of supported and named unsupported file signatures to be
     tested is NTEST.
   - Supported file types are sequentially numbered from FT_NH (1) to FT_TXAUD
     in AFpar.h.  File type FT_UNKNOWN is 0.  Sequential numbering allows for
     indexing into a table of descriptions (extern array initialized in
     AFprintAFpar).
   - Additional codes (larger than FT_TXAUD) are FT_AUTO (triggers automatic
     mode - check file header), and FT_UNSUP (unsupported file type).  FT_UNSUP
     is larger than any other file type code.
   - Each supported file signature is assigned a file type code from FT_NH to
     FT_TXAUD.
   - Each named unsupported file type is assigned a code FT_UNSUP, ... .  The
     macro FTU_CODE(index) maps index 0, ... to FT_UNSUP + index.  The index
     value maps one-to-one to a list of names of the unsupported file types.
     That table is of size NUNSUP.
   - If an additional unsupported type is added, the codes and descriptions must
     be re-jiggered.
     The entries following the new named unsupported file type signature must be
     updated.
*/

#define NTEST NELEM (MagicS)

/* Descriptions of unsupported audio file formats */
static const char *FTU_desc[] = {
  "RF64 WAVE file",
  "RIFX WAVE file",
  "Comdisco SPW (version 2.8.2) Signal file",
  "Sound Blaster VOC file",
  "Amiga SVX file",
  "Amiga maud file",
  "sndtool file",
  "Psion alaw file",
  "SampleVision file",
  "Audio Visual Research file",
  "Esignal file",
  "Macintosh HCOM file",
  "Gravis Ultrasound Patch file",
  "Gold sample file",
  "SRFS file",
  "SoundFont file",
  "DiamondWare Digitized file",
  "MIDI file",
  "RealAudio file",
  "MPEG-1 Layer III",
  "Nyvalla DSP StdHead file",
  "ILS Sampled Data file",
  "Littmann Digital Stethoscope file"
};
#define NUNSUP NELEM (FTU_desc)

/* LHEADBUF is the number of header bytes to be read */
#define LHEADBUF  (252 + 2)  /* SPPACK requires the most lookahead */

#define FREAD(buf,size,nv,fp) (int) fread ((char *) buf, (size_t) size, \
                                           (size_t) nv, fp)

/* Local functions */
static int
AF_checkMagic (const char Header[], int Nh, const struct AF_magic MagicS[],
               int N);
static int
AF_compMagic (const char Header[], int Nh, const struct AF_magicV *MagicV);


enum AF_FT_T
AFfindFtype (FILE *fp)

{
  int Nh;
  enum AF_FT_T Ftype;
  char Header[LHEADBUF];

  /* Check that the last (unsupported) file code in MagicS points to the last
     description */
  assert (FTU_INDEX (MagicS[NTEST-1].Ftype) == NUNSUP - 1);

  /* Check if the file type has been preset*/
  Ftype = AFopt.FtypeI;
  if (Ftype == FT_AUTO) {

    /* Get the file size */
    if (! FLseekable (fp)) {
      UTwarn ("AFfindFtype - %s", AFM_RAuto);
      return FT_ERROR;
    }

    /* Read in (part of) the header */
    Nh = FREAD (Header, 1, LHEADBUF, fp);
    rewind (fp);    /* Rewind the file */
    if (Nh < LHEADBUF && ferror (fp)) {
      UTsysMsg ("AFfindFtype - %s", AFM_ReadErr);
      return FT_ERROR;
    }

    /* Test for the audio file magic values */
    Ftype = AF_checkMagic (Header, Nh, MagicS, NTEST);

    if (Ftype == FT_UNKNOWN) {

      /* Check if a headerless (non-standard) file should be accepted */
      if (AFopt.InputPar.Format != FD_UNDEF)
        Ftype = FT_NH;
      else
        UTwarn ("AFfindFtype - %s", AFM_UnkFtype);
    }

    else if (Ftype >= FT_UNSUP) {

      /* No supported match */
      UTwarn ("AFfindFtype - %s: %s", AFM_UnsupFtype,
              FTU_desc[FTU_INDEX (Ftype)]);
      Ftype = FT_UNSUP;
    }
  }

  return Ftype;
}

/* Check for the file magics, return FT_UNKNOWN if no match */


static int
AF_checkMagic (const char Header[], int Nh, const struct AF_magic MagicS[],
               int N)

{
  int i, Ftype;

  Ftype = FT_UNKNOWN;
  for (i = 0; i < N; ++i) {
    if ((Nh == LHEADBUF || Nh >= MagicS[i].Lhead) &&
        AF_compMagic (Header, Nh, &MagicS[i].Magic1) &&
        AF_compMagic (Header, Nh, &MagicS[i].Magic2)) {
      Ftype = MagicS[i].Ftype;
      break;
    }
  }
  return Ftype;
}

/* Compare magic strings, return 1 for a match */
/* A reference string which is NULL or zero length string gives a match */


static int
AF_compMagic (const char Header[], int Nh, const struct AF_magicV *MagicV)

{
  int match;

  match = (MagicV->Magic == NULL || MagicV->Len == 0 ||
           (Nh >= MagicV->offs + MagicV->Len &&
            memcmp (&Header[MagicV->offs], MagicV->Magic, MagicV->Len) == 0));

  return match;
}
