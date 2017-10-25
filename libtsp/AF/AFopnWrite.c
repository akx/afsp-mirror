/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  AFILE *AFopnWrite (const char Fname[], enum AF_FTW_T FtypeW,
                     enum AF_FD_T Format, long int Nchan, double Sfreq,
                     FILE *fpinfo)

Purpose:
  Open an audio file for writing

Description:
  This routine opens an audio file for writing.  This routine sets up the audio
  file parameters to write data of the given format to the audio file.  After
  writing data to the file, the routine AFclose should be called to update the
  file header information and close the file.

  By default, information records consisting, for instance, the date and the
  program creating the file (see UTsetProg) are written to the audio file header
  or trailer.  The routine AFsetInfo can be called before calling this routine
  to specify additional information to be written to the file.

  This routine can write AU audio files, AIFF, AIFF-C files, WAVE files, text
  audio files, and headerless audio files.

  AU audio file:
    8-bit mu-law, 8-bit A-law, 8/16/24/32-bit integer, and 32/64-bit IEEE
    floating-point data formats are supported.
  WAVE file:
    8-bit mu-law, 8-bit A-law, offset-binary 8-bit integer, 16/24/32-bit
    integer, and 32/64-bit IEEE floating-point, data formats are supported.
  AIFF sound file:
    8/16/24/32-bit integer data formats are supported.
  AIFF-C sound file:
    8-bit mu-law, 8-bit A-law, 8/16/24/32-bit integer, and 32/64-bit IEEE
    floating-point data formats are supported.
  Text audio file:
    text format data, one value to a line.  The data is written as a floating
    point value with 6 digits of precision (%g format).
  Headerless file:
    8-bit mu-law, 8-bit A-law, offset-binary 8-bit integer, 8/16/24/32-bit
    integer, 32/64-bit floating-point, and text data formats are supported.

  For the fixed point file data representations, input values in the range
  [-1,  +1) ([-0.5, +0.5) for 8-bit data) will be converted without clipping.
  The scaling factor shown below is applied to the data before they are
  written to the file.
     data format   scaling factor   file data values
    8-bit A-law     32,768         [-32,256, +32,256]
    8-bit mu-law    32,768         [-32,124, +32,124]
    8-bit integer   256            [-128, 127]
    16-bit integer  32,768         [-32,768, +32,767]
    24-bit integer  8,388,608      [-8,388,608, +8,388,607]
    32-bit integer  2,147,483,648  [-2,147,483,648, 2,147,483,647]
  For files containing floating-point data, values are scaled by unity.

  The file type codes (defined in AFpar.h) accepted are
    FTW_AU          - AU audio file
    FTW_WAVE        - WAVE audio file, upgraded to WAVE-ex if appropriate
    FTW_WAVE_EX     - WAVE audio file (extensible format)
    FTW_WAVE_NOEX   - WAVE audio file, do not upgrade to WAVE-ex
    FTW_AIFF        - AIFF audio file
    FTW_AIFF_C      - AIFF-C audio file
    FTW_AIFF_C_SOWT - AIFF-C audio file (PCM, byte swapped)
    FTW_NH_NATIVE   - noheader file, native byte order
    FTW_NH_SWAP     - noheader file, swap byte order
    FTW_NH_EL       - noheader file, little-endian data
    FTW_NH_EB       - noheader file, big-endian data
    FTW_TEXT        - Text audio file (with header)
  The data format codes (defined in AFpar.h) accepted are
    FD_ALAW8   - A-law 8-bit
    FD_MULAW8  - mu-law 8-bit
    FD_MULAWR8 - bit-reversed mu-law 8-bit
    FD_UINT8   - offset binary integer 8-bit
    FD_INT8    - integer 8-bit
    FD_INT16   - integer 16-bit
    FD_INT24   - integer 24-bit
    FD_INT32   - integer 32-bit
    FD_FLOAT32 - float 32-bit
    FD_FLOAT64 - float 64-bit
    FD_TEXT16  - text data scaled as 16-bit integers
    FD_TEXT    - text data
  The data types accepted by the different file types are as follows.
    AU audio files:
      mu-law, A-law, 8/16/24/32-bit integer, 32/64-bit float
    WAVE files:
      mu-law, A-law, offset-binary 8-bit, 16/24/32-bit integer, 32/64-bit float
    AIFF-C sound files:
      mu-law, A-law, 8/16/24/32-bit integer, 32/64-bit float
    AIFF and AIFF-C/sowt sound files:
      8/16/24/32-bit integer
    Text audio files:
      text data
    Headerless files:
      all data formats

Parameters:
  <-  AFILE *AFopnWrite
      Audio file pointer for the audio file
   -> const char Fname[]
      Character string specifying the file name.  The file name "-" means
      standard output.
   -> enum AF_FTW_T FtypeW
      File type code for the output file (distinguishes between subtypes)
        FTW_AU, FTW_WAVE, ..., FTW_WAVE_NOEX, ...
   -> enum AF_FD_T Format
      Data format code
        FD_MULAW8, ..., FD_TEXT
   -> long int Nchan
      Number of channels
   -> double Sfreq
      Sampling frequency
   -> FILE *fpinfo
      File pointer for printing audio file information.  If fpinfo is not NULL,
      information about the audio file is printed on the stream selected by
      fpinfo.

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.30 $  $Date: 2017/07/20 16:55:43 $

-------------------------------------------------------------------------*/

#include <libtsp/sysOS.h>
#if (SY_OS == SY_OS_WINDOWS)
#  define _CRT_NONSTDC_NO_DEPRECATE   /* Allow Posix names */
#  define _CRT_SECURE_NO_WARNINGS     /* Allow fopen */
#endif

#include <stdio.h>
#include <stdlib.h>   /* EXIT_FAILURE */
#include <string.h>

#include <libtsp.h>
#include <AFpar.h>
#include <libtsp/AFheader.h>
#include <libtsp/AFmsg.h>
#include <libtsp/nucleus.h>

#define ERR_MSG 0
#define SYS_MSG 1

/* Local functions */
static void
AF_error (const char Fname[], int sysFlag);
static FILE*
AF_open_write (const char Fname[]);

/* If ErrorHalt is clear, execution continues after an error */


AFILE *
AFopnWrite (const char Fname[], enum AF_FTW_T FtypeW, enum AF_FD_T Format,
            long int Nchan, double Sfreq,  FILE *fpinfo)

{
  AFILE *AFp;
  FILE *fp;
  struct AF_write AFw;

/* Set up and check the audio file parameters */
/* Parameters returned in AFw, file type is validated */
  if (AFpreSetWPar (FtypeW, Format, Nchan, Sfreq, &AFw)) {
    AF_error (Fname, ERR_MSG);
    return NULL;
  }

/* Open the file for writing */
  fp = AF_open_write (Fname);
  if (fp == NULL)
    return NULL;

  /* Some parameters in AFw may be updated */
  /* Write the appropriate header */
  /* Update file type specific items in AFw
     - AFw.Swapb
  */
  AFp = NULL;
  switch (AFw.FtypeW) {
  case FTW_AU:
    AFp = AFwrAUhead (fp, &AFw);
    break;
  case FTW_WAVE:
  case FTW_WAVE_EX:
  case FTW_WAVE_NOEX:
    AFp = AFwrWVhead (fp, &AFw);
    break;
  case FTW_AIFF:
  case FTW_AIFF_C:
  case FTW_AIFF_C_SOWT:
    AFp = AFwrAIhead (fp, &AFw);
    break;
  case FTW_NH_NATIVE:
  case FTW_NH_SWAP:
  case FTW_NH_EL:
  case FTW_NH_EB:
    AFp = AFsetNHwrite (fp, &AFw);
    break;
  case FTW_TXAUD:
    AFp = AFwrTAhead (fp, &AFw);
    break;
  default:
    UTwarn ("AFopnWrite: %s", AFM_BadFtypeC);
    break;
  }

/* Error messages */
  if (AFp == NULL) {
    fclose (fp);
    AF_error (Fname, ERR_MSG);
    return NULL;
  }

/* Reset output options */
  (void) AFoptions(AF_OPT_OUTPUT);

/* Print the header information */
  AFprintAFpar (AFp, Fname, fpinfo);

return AFp;
}

/* Open a file for writing */

#include <libtsp/sysOS.h>
#if (SY_OS == SY_OS_WINDOWS || SY_OS == SY_OS_CYGWIN)
#  include <io.h>     /* setmode */
#  include <fcntl.h>  /* IO_BINARY */
#endif

static FILE *
AF_open_write (const char Fname[])

{
  FILE *fp;

  if (strcmp (Fname, "-") == 0) {
    /* Output to standard output */
    fp = stdout;
#ifdef O_BINARY
    if (setmode (fileno(fp), O_BINARY) == -1)
      UTwarn ("AFopnWrite - %s", AFM_NoWBinMode);
#endif
  }
  else
    /* Output to a file */
    fp = fopen (Fname, "wb");

  if (fp == NULL) {
    AF_error (Fname, SYS_MSG);
    return NULL;
  }

  return fp;
}

/* Print an error message with the file name */


static void
AF_error (const char Fname[], int sysFlag)

{
  const char *fn;

  if (strcmp (Fname, "-") == 0)
    fn = "<stdout>";
  else
    fn = Fname;

  if (sysFlag)
    UTsysMsg ("AFopnWrite: %s \"%s\"", AFM_OpenWErr, fn);
  else
    UTwarn ("AFopnWrite: %s \"%s\"", AFM_OpenWErr, fn);
  if (AFopt.ErrorHalt)
    exit (EXIT_FAILURE);

  /* Reset options */
  (void) AFoptions (AF_OPT_OUTPUT);

  return;
}
