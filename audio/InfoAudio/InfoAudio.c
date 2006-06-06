/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  InfoAudio [options] AFile1 [AFile2 ... ]

Purpose:
  Print audio file information

Description:
  This program prints information about a number of audio files to standard
  output.  The full output consists of the file type and name (indented
  part of the output below), the file data parameters, and the information
  records.
     AU audio file: /usr/local/src/audio/InfoAudio/test/xx.au
       Number of samples : 23808  2001-02-04 14:12:50 UTC
       Sampling frequency: 8000 Hz
       Number of channels: 1 (14/16-bit integer) [FC]
    File name: xx.au
    Header length: 104
    Sampling frequency: 8000
    No. samples: 23808
    No. channels: 1
    Data type: integer16           (A-law8, mu-law8, unsigned8, integer8,
                                    integer16, float32, float64, or text)
    File byte order: big-endian    (big-endian, little-endian, or byte-stream)
    Host byte order: big-endian    (big-endian or little-endian)
    --Information records--
    date: 2001-02-04 14:12:50 UTC
    program: CopyAudio

  For single byte or text data, the "File byte order" field is set to
  "byte-stream".

  Non-printing characters (as defined by the routine isprint) in the
  information records are replaced by question marks.

Options:
  Input file(s): AFile1 [AFile2 ... ]:
      The environment variable AUDIOPATH specifies a list of directories to be
      searched for the input audio file(s).
  -t FTYPE, --type=FTYPE
      Input audio file type.  In the default automatic mode, the input file
      type is determined from the file header.  For input from a non-random
      access file (e.g. a pipe), the input file type can be explicitly
      specified to avoid the lookahead used to read the file header.  This
      option can be specified more than once.  Each invocation applies to the
      input files that follow the option.  See the description of the
      environment variable AF_FILETYPE below for a list of file types.
  -P PARMS, --parameters=PARMS
      Parameters to be used for headerless input files.  This option may be
      given more than once.  Each invocation applies to the input files that
      follow the option.  See the description of the environment variable
      AF_NOHEADER below for the format of the parameter specification.
  -i ICODE, --info_code=ICODE
      Parameter to choose the information to be printed.  The information that
      can be printed consists of three parts.  The ICODE parameter is the sum
      of the codes which control printing of the individual parts.
        1:  Print the file type and name
        2:  Print the file data parameters
        4:  Print the information records
      The default is to print all of the information (ICODE=7).  For ICODE=0,
      no information is printed, but the program return code indicates if all
      files were opened successfully.
  -h, --help
      Print a list of options and exit.
  -v, --version
      Print the version number and exit.

Environment variables:
  AF_FILETYPE:
  This environment variable defines the input audio file type.  In the default
  mode, the input file type is determined from the file header.
    "auto"       - determine the input file type from the file header
    "AU" or "au" - AU audio file
    "WAVE" or "wave" - WAVE file
    "AIFF" or "aiff" - AIFF or AIFF-C sound file
    "noheader"   - headerless (non-standard or no header) audio file
    "SPHERE"     - NIST SPHERE audio file
    "ESPS"       - ESPS sampled data feature file
    "IRCAM"      - IRCAM soundfile
    "SPPACK"     - SPPACK file
    "INRS"       - INRS-Telecom audio file
    "SPW"        - Comdisco SPW Signal file
    "CSL" or "NSP" - CSL NSP file
    "text"       - Text audio file

  AF_NOHEADER:
  This environment variable defines the data format for headerless or
  non-standard input audio files.  The string consists of a list of parameters
  separated by commas.  The form of the list is
    "Format, Start, Sfreq, Swapb, Nchan, ScaleF"
  Format: File data format
       "undefined" - Headerless files will be rejected
       "mu-law8"   - 8-bit mu-law data
       "A-law8"    - 8-bit A-law data
       "unsigned8" - offset-binary 8-bit integer data
       "integer8"  - two's-complement 8-bit integer data
       "integer16" - two's-complement 16-bit integer data
       "integer24" - two's-complement 24-bit integer data
       "integer32" - two's-complement 32-bit integer data
       "float32"   - 32-bit floating-point data
       "float64"   - 64-bit floating-point data
       "text"      - text data
  Start: byte offset to the start of data (integer value)
  Sfreq: sampling frequency in Hz (floating point number)
  Swapb: Data byte swap parameter
       "native"        - no byte swapping
       "little-endian" - file data is in little-endian byte order
       "big-endian"    - file data is in big-endian byte order
       "swap"          - swap the data bytes as the data is read
  Nchan: number of channels
    The data consists of interleaved samples from Nchan channels
  ScaleF: Scale factor
       "default" - Scale factor chosen appropriate to the type of data.
       "<number or ratio>" - Specify the scale factor to be applied to
          the data from the file.
  The default values for the audio file parameters correspond to the following
  string.
    "undefined, 0, 8000., native, 1, default"

  AUDIOPATH:
  This environment variable specifies a list of directories to be searched when
  opening the input audio files.  Directories in the list are separated by
  colons (semicolons for Windows).

Author / version:
  P. Kabal / v4r0a  2003-11-03  Copyright (C) 2006

-------------------------------------------------------------------------*/

#include <assert.h>
#include <ctype.h>
#include <stdlib.h>	/* EXIT_SUCCESS */
#include <string.h>

#include <libtsp.h>
#include <libtsp/nucleus.h>
#include <libtsp/AFheader.h>
#define AF_DATA_LENGTHS
#include <libtsp/AFpar.h>
#include "InfoAudio.h"

#define NELEM(array)	((int) ((sizeof array) / (sizeof array[0])))

#define EXTCODE(x,n)	((x / n) % 2)

static const char *DataFormat[] = {
  "undefined",
  "mu-law8",
  "A-law8",
  "unsigned8",
  "integer8",
  "integer16",
  "integer24",
  "integer32",
  "float32",
  "float64",
  "text"
};
static const char *DataSwap[] = {
  "big-endian",
  "little-endian",
  "native",		/* AFp->Swapb is native or swap */
  "swap"
};

static void
IA_nonPrint (struct AF_info *InfoS);
static void
IA_printInfo (const struct AF_info *InfoS);


int
main (int argc, const char *argv[])

{
  struct IA_FIpar FI[MAXFILE];
  AFILE *AFp;
  FILE *fpinfo;
  int Hbo, Dbo, i, Icode, Nfiles;
  long int Nsamp, Nchan;
  double Sfreq;

/* Option handling */
  IAoptions (argc, argv, &Icode, FI, &Nfiles);

/* Loop over the input files */
  for (i = 0; i < Nfiles; ++i) {

    if (EXTCODE (Icode, 1) != 0)
      fpinfo = stdout;
    else
      fpinfo = NULL;

    /* Open the audio file */
    AOsetFIopt (&FI[i], 0, 1);
    FLpathList (FI[i].Fname, AFPATH_ENV, FI[i].Fname);
    AFp = AFopnRead (FI[i].Fname, &Nsamp, &Nchan, &Sfreq, fpinfo);

    /* Print the audio file parameter information */
    if (EXTCODE (Icode, 2) != 0) {
      if (AFp->fp == stdin)
	printf (IAMF_stdin);
      else
	printf (IAMF_FName, FI[i].Fname);
      printf (IAMF_Header, AFp->Start, Sfreq, Nsamp / Nchan, Nchan,
	      DataFormat[AFp->Format]);
      Dbo = UTbyteCode (AFp->Swapb);
      Hbo = UTbyteOrder ();
      if (AF_DL[AFp->Format] <= 1)
	printf (IAMF_ByteStream);
      else
	printf (IAMF_FByteOrder, DataSwap[Dbo]);
      printf (IAMF_HByteOrder, DataSwap[Hbo]);
    }

    assert (NELEM (DataFormat) == NFD);
    assert (AFp->Format > 0);
    assert (AF_DL[AFp->Format] > 1 || AFp->Swapb == DS_NATIVE);

    /* Print the information strings (modifies the string) */
    if (EXTCODE (Icode, 4) != 0 && AFp->InfoS.N > 0) {

      /* Replace non-printing characters with a ? */
      IA_nonPrint (&AFp->InfoS);
      IA_printInfo (&AFp->InfoS);
    }

    AFclose (AFp);
    if (i < Nfiles-1)
      printf ("\n");

  }
  return EXIT_SUCCESS;
}

/* Replace non-printing characters by ? */

static void
IA_nonPrint (struct AF_info *InfoS)

{
  int i;

  for (i = 0; i < InfoS->N; ++i) {
    if (! (InfoS->Info[i] == '\0' || InfoS->Info[i] == '\n' ||
	   InfoS->Info[i] == '\t' || InfoS->Info[i] == '\r')
	&& ! isprint (InfoS->Info[i]))
      InfoS->Info[i] = '?';	/* Replace non-printing characters by ? */
  }

  return;
}

/* Print the information string */


static void
IA_printInfo (const struct AF_info *InfoS)

{
  int n, ls;

  printf (IAMF_InfoRec);
  n = 0;
  while (n < InfoS->N) {
    ls = strlen (&InfoS->Info[n]);
    if (n < InfoS->N - 1)	/* Don't print the last empty line */
      printf ("%s\n", &InfoS->Info[n]);
    n += ls + 1;
  }

  return;
}
