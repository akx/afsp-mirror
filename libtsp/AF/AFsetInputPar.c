/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int AFsetInputPar (const char String[])

Purpose:
  Set defaults for input audio files from a string specification

Description:
  This routine sets the default audio file data parameters.  These parameters
  are used when opening input audio files with unrecognized (non-standard)
  headers or files with no headers (raw audio files).  Note that the parameters
  do not override values specified in the file headers of standard input files.

  This routine must be called before opening the file with AFopnRead.  The
  parameters for AFsetInputPar are determined from an input string which
  consists of a list of parameters separated by commas.  The form of the list is
    "Format, Start, Sfreq, Swapb, Nchan, FullScale"
  Format: File data format
       "undefined" - Headerless files will be rejected
       "A-law8"    - 8-bit A-law data
       "mu-law8"   - 8-bit mu-law data
       "mu-lawR8"  - 8-bit bit-reversed mu-law data
       "unsigned8" - offset-binary 8-bit integer data
       "integer8"  - two's-complement 8-bit integer data
       "integer16" - two's-complement 16-bit integer data
       "integer24" - two's-complement 24-bit integer data
       "integer32" - two's-complement 32-bit integer data
       "float32"   - 32-bit floating-point data
       "float64"   - 64-bit floating-point data
       "text16"    - text data scaled as 16-bit integers
       "text"      - text data
  Start: byte offset to the start of data (integer value)
  Sfreq: sampling frequency in Hz (floating-point value)
  Swapb: data byte swap parameter
       "native" - no byte swapping
       "little-endian" - file data is in little-endian byte order and will be
         swapped if the current host uses big-endian byte order
       "big-endian" - data is in big-endian byte order and will be swapped if
         the current host uses little-endian byte order
       "swap" - swap the data bytes
  Nchan: number of channels
    The data consists of interleaved samples from Nchan channels
  FullScale: full scale value for the data in the file.
    This value is used to scale data from the file to fall in the range -1 to
    +1.  If the FullScale value is set to "default", the FullScale value is
    determined based on the type of data in the file as shown below.
         data type     FullScale
       8-bit A-law:               32768
       8-bit mu-law:              32768
       8-bit bit-reversed mu-law: 32768
       8-bit integer:             256
       16-bit integer:            32768
       24-bit integer:            256*32768
       32-bit integer:            65536*32768
       float data:                1
       text16 data:               32768
       text data:                 1
    The value of FullScale can be specified as a value or ratio of values.
  The default values for the audio file parameters correspond to the following
  string.
    "undefined, 0, 8000., native, 1, default"

  Leading and trailing white-space is removed from each item.  Any of the
  parameters may be omitted, in which case whatever value has been previously
  set remains in effect for that parameter.  The string ",512, 10000." would set
  the Start and Sfreq parameters and leave the other parameters unchanged.

  If the input string contains has a leading '$', the string is assumed to
  specify the name of an environment variable after the '$'.  This routine uses
  the value of this environment variable to determine the parameters.  If this
  routine is called as AFsetInputPar("$AF_SETINPUTPAR"), this routine would look
  for the parameter string in environment variable AF_SETINPUTPAR.

Parameters:
  <-  int AFsetInputPar
      Error flag, zero for no error
   -> const char String[]
      String containing the list of parameters for headerless files or the name
      of an environment variable (with a leading $)

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.76 $  $Date: 2017/07/03 22:14:14 $

-------------------------------------------------------------------------*/

#include <libtsp/sysOS.h>
#if (SY_OS == SY_OS_WINDOWS)
#  define _CRT_NONSTDC_NO_DEPRECATE   /* Allow Posix names */
#  define _CRT_SECURE_NO_WARNINGS     /* Allow getenv */
#endif

#include <stdlib.h> /* getenv prototype */
#include <string.h>

#include <libtsp.h>
#include <AFpar.h>
#include <libtsp/AFmsg.h>

#define N_NHPAR   6
#define NCBUF     512

/* Keyword tables */
static const char *Format_keys[AF_NFD+1] = {
  "undef*ined",
  "A-law*8",
  "mu-law*8",     /* "mu-law" => mu-law8 */
  "mu-lawR*8",
  "unsigned8",
  "integer8",
  "int*eger16",   /* "int" => integer16 */
  "integer24",
  "integer32",
  "float*32",     /* "float" => float32 */
  "float64",
  "text16",
  "text",
  NULL
};
static const char *Swapb_keys[] = {
  "n*ative", "l*ittle-endian", "b*ig-endian", "s*wap", NULL };
static const char *Def_key[] = {
  "def*ault", NULL};

/* Values corresponding to keywords */
static const int Format_values[AF_NFD] = {
  FD_UNDEF,
  FD_ALAW8, FD_MULAW8, FD_MULAWR8, FD_UINT8, FD_INT8, FD_INT16,
  FD_INT24, FD_INT32, FD_FLOAT32, FD_FLOAT64, FD_TEXT16, FD_TEXT
};
static const int Swapb_values[] = {
  DS_NATIVE, DS_EL, DS_EB, DS_SWAP };


int
AFsetInputPar (const char String[])

{
  char cbuf[NCBUF+1];
  int n, k, nc, Err;
  int WSFlag;
  long int LV;
  const char *p;
  char *token;
  double DN, DD;
  struct AF_InputPar InputPar;

/* Copy the current values */
  InputPar = AFopt.InputPar;

/* Check for an environment variable */
  if (String[0] == '$') {
    p = getenv (&String[1]);
    if (p == NULL)
      p = "";
  }
  else
    p = String;

/* Set up the token buffer */
  nc = (int) strlen (p);
  if (nc <= NCBUF)
    token = cbuf;
  else
    token = (char *) UTmalloc (nc + 1);

/* Separate the parameters */
  Err = 0;
  WSFlag = 1;   /* Comma separator, remove white-space around tokens */
  for (k = 0; k < N_NHPAR && p != NULL; ++k) {
    p = STfindToken (p, ",", "", token, WSFlag, nc);
    if (token[0] != '\0') {

      /* Decode the parameter values */
      switch (k) {
      case 0:
        n = STkeyMatch (token, Format_keys);
        if (n < 0) {
          UTwarn ("AFsetInputPar - %s: \"%.10s\"", AFM_BadFormat, token);
          Err = 1;
        }
        else
          InputPar.Format = Format_values[n];
        break;
      case 1:
        if (STdec1long (token, &LV) || LV < 0 ) {
          UTwarn ("AFsetInputPar - %s: \"%.10s\"", AFM_BadStart, token);
          Err = 1;
        }
        else
          InputPar.Start = LV;
        break;
      case 2:
        if (STdecDfrac (token, &DN, &DD) || DD == 0 || DN / DD <= 0) {
          UTwarn ("AFsetInputPar - %s: \"%.10s\"", AFM_BadSfreq, token);
          Err = 1;
        }
        InputPar.Sfreq = DN / DD;
        break;
      case 3:
        n = STkeyMatch (token, Swapb_keys);
        if (n < 0) {
          UTwarn ("AFsetInputPar - %s: \"%.10s\"", AFM_BadSwap, token);
          Err = 1;
        }
        else
          InputPar.Swapb = Swapb_values[n];
        break;
      case 4:
        if (STdec1long (token, &LV) || LV < 0) {
          UTwarn ("AFsetInputPar - %s: \"%.10s\"", AFM_BadNchan, token);
          Err = 1;
        }
        else
          InputPar.Nchan = LV;
        break;
      case 5:
        if (STkeyMatch (token, Def_key) == 0)
          InputPar.FullScale = AF_FULLSCALE_DEFAULT;
        else {
          if (STdecDfrac (token, &DN, &DD) || DD == 0) {
            UTwarn ("AFsetInputPar - %s: \"%.10s\"", AFM_BadFullScale, token);
            Err = 1;
          }
          else
            InputPar.FullScale = DN / DD;
        }
        break;
      }
    }
  }
  if (p != NULL) {
    UTwarn ("AFsetNHpar - %s", AFM_TooManyPar);
    Err = 1;
  }

/* Deallocate the buffer */
  if (nc > NCBUF)
    UTfree ((void *) token);

/* Set the parameters */
  if (! Err)
    AFopt.InputPar = InputPar;

  return Err;
}
