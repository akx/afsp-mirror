/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  AFILE *AFopenRead (const char Fname[], long int *Nsamp, long int *Nchan,
		     float *Sfreq, FILE *fpinfo)

Purpose:
  Open an audio file for reading

Description:
  This routine opens an audio file for reading.  The companion routine
  AFreadData reads data from the file.  Routine AFclose should be used to
  close the file.

  This routine reads the audio file header and optionally prints the header
  information.  Several file header formats are supported.  For files with
  no header or an unrecognized header, the file format can be declared by
  calling routine AFsetNHpar.  A banner identifying the audio file and its
  parameters is printed.

  AU audio file:
    8-bit mu-law, 8-bit A-law, 8-bit integer, 16-bit integer, 24-bit integer,
    32-bit integer, 32-bit IEEE floating-point, and 64-bit IEEE floating-point
    data formats are supported.
  WAVE file:
    8-bit mu-law, 8-bit A-law, offset-binary 1-bit to 8-bit integer, 9-bit to
    32-bit integer, 32-bit IEEE floating-point, and 64-bit IEEE floating-point
    data formats are supported.
  AIFF or AIFF-C sound file:
    8-bit mu-law, 8-bit A-law, 1-bit to 32-bit integer,  32-bit IEEE
    floating-point, and 64-bit IEEE floating-point data formats are supported.
  NIST SPHERE audio file:
    8-bit mu-law and 16-bit integer data formats are supported.
  IRCAM soundfile:
    8-bit mu-law, 8-bit A-law, 8-bit integer, 16-bit integer, 32-bit integer,
    and 32-bit floating-point data formats are supported.
  ESPS sampled data feature file:
    16-bit integer, 32-bit integer, 32-bit floating-point, and 64-bit
    floating-point data formats are supported.
  SPPACK sampled data file:
    8-bit mu-law, 8-bit A-law and 16-bit integer data formats are supported.
  INRS-Telecommunications audio file:
    16-bit integer format is supported.
  Cadence SPW Signal file:
    8-bit integer, 16-bit integer, 32-bit floating-point, 64-point
    floating-point, and text formats are supported.
  CSL NSP file:
    16-bit integer format is supported.
  Text audio file:
    Data in character format (usually representing 16-bit integer values).
  Headerless audio file:
    The data format is specified by calling routine AFsetNHpar.

  For the fixed point file data representations, read operations return data
  values as follows. The scaling factor shown below is applied to the data in
  the file before it is returned.
     format            returned values
    8-bit mu-law   - [ -32124, +32124 ]  scaling factor 1
    8-bit A-law    - [ -32256, +32256 ]  scaling factor 1
    8-bit integer  - [ -16384, +16256 ]  scaling factor 128
    16-bit integer - [ -32768, +32767 ]  scaling factor 1
    24-bit integer - [ -32768, +32768 )  scaling factor 1/256
    32-bit integer - [ -32768, +32768 )  scaling factor 1/65536
  Floating-point data in the input audio file are scaled by 32768 (except for
  ESPS sample data feature files).  For file float data values between -1 and
  +1, the returned values will be in the range [ -32768, +32768 ].

  File type determination:
    The default behaviour is to determine the file type from the header of the
    input file.  However this requires look-ahead and is not possible with
    files that are not random access (for instance a file stream from a pipe).
    For both random access and sequential access files, the file type can be
    specified explicitly with a call to routine AFsetFileType, obviating the
    need to determine the file type from the header.

  Number of samples:
    In some types of audio file types, the number of samples is specified in
    the file header.  In others, the number of samples is known only if the
    file size can be determined, specifically if the file random access.  For
    input from a stream which does not allow random access (for instance a file
    stream from a pipe), the file size cannot be determined for those files
    without that information in the file header.
                        no. samples
       file type         in header
       AU                  yes
       WAVE                yes
       AIFF or AIFF-C      yes
       NIST SPHERE         yes
       IRCAM               no
       ESPS                yes
       SPPACK              yes
       INRS-Telecom        no
       Cadence SPW         no
       CSL NSP             yes
       Text file           no
       Headerless          no
  Notes:
  - Some AU audio files and some ESPS audio files do not specify the number of
    samples.
  - AIFF/AIFF-C files with a perverse header cannot be opened for non-random
    access.

  On encountering an error, the default behaviour is to print an error message
  and halt execution.

Parameters:
  <-  AFILE *AFopenRead
      Audio file pointer for the audio file..  The file name "-" means
      standard input.
   -> const char Fname[]
      Character string specifying the file name
  <-  long int *Nsamp
      Total number of samples in the file (all channels)
  <-  long int *Nchan
      Number of channels
  <-  float *Sfreq
      Sampling frequency
   -> FILE *fpinfo
      File pointer for printing audio file information.  If fpinfo is not NULL,
      information about the audio file is printed on the stream selected by
      fpinfo.

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.86 $  $Date: 2003/04/29 02:27:20 $

-------------------------------------------------------------------------*/

static char rcsid[] = "$Id: AFopenRead.c 1.86 2003/04/29 AFsp-v6r8 $";

#include <stdio.h>
#include <stdlib.h>		/* EXIT_FAILURE */
#include <string.h>

#include <libtsp.h>
#include <libtsp/nucleus.h>	/* FLseekable */
#include <libtsp/AFheader.h>
#include <libtsp/AFmsg.h>
#include <libtsp/AFpar.h>
#include <libtsp/sysOS.h>

#if (SY_OS != SY_OS_UNIX)
#  include <io.h>	/* setmode */
#  include <fcntl.h>	/* O_BINARY */
#endif



#define ERR_MSG	0
#define SYS_MSG	1

static void
AF_error (const char Fname[], int sysFlag);
static FILE*
AF_open_read_bin (const char Fname[]);

/* If AFopts_ErrorHalt is clear, execution continues after an error */


AFILE *
AFopenRead (const char Fname[], long int *Nsamp, long int *Nchan, float *Sfreq,
	    FILE *fpinfo)
{
  FILE *fp;
  AFILE *AFp;
  int Ftype;

/* Open the file for reading (binary mode) */
  fp = AF_open_read_bin (Fname);
  if (fp == NULL)
    return NULL;

/* Identify the file type */
  Ftype = AFfindType (fp);

/* Read the header information */
  switch (Ftype) {
  case FT_NH:
    AFp = AFsetNHread (fp, Fname);
    break;
  case FT_AU:
    AFp = AFrdAUhead (fp);
    break;
  case FT_WAVE:
    AFp = AFrdWVhead (fp);
    break;
  case FT_AIFF:
  case FT_AIFF_C:
    AFp = AFrdAIhead (fp);
    break;
  case FT_SPHERE:
    AFp = AFrdSPhead (fp);
    break;
  case FT_SF:
    AFp = AFrdSFhead (fp);
    break;
  case FT_ESPS:
    AFp = AFrdEShead (fp);
    break;
  case FT_SPPACK:
    AFp = AFrdBLhead (fp);
    break;
  case FT_INRS:
    AFp = AFrdINhead (fp);
    break;
  case FT_SPW:
    AFp = AFrdSWhead (fp);
    break;
  case FT_NSP:
    AFp = AFrdNShead (fp);
    break;
  case FT_TXAUD:
    AFp = AFrdTAhead (fp);
    break;
  default:
    AFp = NULL;	/* Error or unknown / unsupported file type */
    break;
  }

/* Error messages */
  if (AFp == NULL) {
    fclose (fp);
    AF_error (Fname, ERR_MSG);
    return NULL;
  }

/* Reset open parameters */
  (void) AFresetOptions (AF_OPT_INPUT);

/* Print the header information */
  AFprAFpar (AFp, Fname, fpinfo);

/* Return the file parameters */
  *Nsamp = AFp->Nsamp;
  *Nchan = AFp->Nchan;
  *Sfreq = (float) AFp->Sfreq;

  return AFp;
}

/* Open a file for reading in binary mode */


static FILE *
AF_open_read_bin (const char Fname[])

{
  FILE *fp;
  struct AF_opt *AFopt;

  if (strcmp (Fname, "-") == 0) {

    /* Input from standard input */
    fp = stdin;
#if (SY_OS != SY_OS_UNIX)
    if (setmode (fileno (fp), O_BINARY) == -1)
      UTwarn ("AFopenRead - %s", AFM_NoRBinMode);
#endif
  }

  else

    /* Input from a file */
    fp = fopen (Fname, "rb");	/* binary mode */

  if (fp == NULL) {
    AF_error (Fname, SYS_MSG);
    return NULL;
  }

/* Check whether the file must be random access */
  AFopt = AFoptions ();
  if (AFopt->RAccess != 0 && ! FLseekable (fp)) {
    UTwarn ("AFopenRead: %s", AFM_RRAccess);
    fclose (fp);
    AF_error (Fname, ERR_MSG);
    return NULL;
  }

  return fp;
}

/* Print an error message with the file name */


static void
AF_error (const char Fname[], int sysFlag)

{
  const char *fn;
  struct AF_opt *AFopt;

  if (strcmp (Fname, "-") == 0)
    fn = "<stdin>";
  else
    fn = Fname;

  if (sysFlag)
    UTsysMsg ("AFopenRead: %s \"%s\"", AFM_OpenRErr, fn);
  else
    UTwarn ("AFopenRead: %s \"%s\"", AFM_OpenRErr, fn);

  AFopt = AFoptions ();
  if (AFopt->ErrorHalt)
    exit (EXIT_FAILURE);

  return;
}
