/*------------ Telecommunications & Signal Processing Lab --------------
                         McGill University

Routine:
  InfoAudio.h

Description:
  Declarations for InfoAudio

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.43 $  $Date: 2003/11/03 13:09:19 $

----------------------------------------------------------------------*/

#ifndef InfoAudio_h_
#define InfoAudio_h_

#define PROGRAM "InfoAudio"
#define VERSION	"v4r0a  2003-11-03"

#define AFPATH_ENV	"$AUDIOPATH"

#define MAXFILE		100

#include <libtsp.h>		/* p_ and const */
#include <libtsp/AFpar.h>	/* struct AF_NHpar */
#include <AO.h>			/* struct AO_FIpar */

#define IA_FIpar	AO_FIpar	/* Input file structure */

/* Error messages */
#define IAM_BadInfoCode	"Invalid info code parameter"
#define IAM_LateFPar	"Input file parameter specified after input files"
#define IAM_NoFName	"No filenames specified"
#define IAM_XFName	"Too many filenames specified"

/* Printout formats */
#define IAMF_FName	"File name: %s\n"
#define IAMF_stdin	"File name: <stdin>\n"
#define IAMF_Header "\
Header length: %ld\n\
Sampling frequency: %.5g\n\
No. samples: %ld\n\
No. channels: %ld\n\
Data type: %s\n"
#define IAMF_FByteOrder	"File byte order: %s\n"
#define IAMF_ByteStream "File byte order: byte-stream\n"
#define IAMF_HByteOrder	"Host byte order: %s\n"
#define IAMF_InfoRec	"--Information records--\n"

/* Usage */
#define IAMF_Usage "\
Usage: %s [options] AFile\n\
Options:\n\
  -i ICODE, --info_code=ICODE Information to be printed (ICODE: 1+2+4).\n\
  -t FTYPE, --type=FTYPE      Input file type,\n\
                              \"auto\", \"AU\", \"WAVE\", \"AIFF\", \"noheader\",\n\
                              \"SPHERE\", \"ESPS\", \"IRCAM\", \"SPPACK\",\n\
                              \"INRS\", \"SPW\", \"NSP\", or \"text\".\n\
  -P PARMS, --parameters=PARMS  Parameters for headerless input files,\n\
                              \"Format,Start,Sfreq,Swapb,Nchan,ScaleF\".\n\
  -h, --help                  Print a list of options and exit.\n\
  -v, --version               Print the version number and exit."


#ifdef __cplusplus
extern "C" {
#endif

/* Prototypes */

void
IAoptions (int argc, const char *argv[], int *Icode,
	   struct IA_FIpar FI[MAXFILE], int *Nfiles);

#ifdef __cplusplus
}
#endif

#endif /* InfoAudio_h_ */
