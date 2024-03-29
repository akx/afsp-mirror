/*------------ Telecommunications & Signal Processing Lab --------------
                         McGill University

Routine:
  InfoAudio.h

Description:
  Declarations for InfoAudio

Author / revision:
  P. Kabal  Copyright (C) 2020
  $Revision: 1.61 $  $Date: 2020/11/30 12:32:35 $

----------------------------------------------------------------------*/

#ifndef InfoAudio_h_
#define InfoAudio_h_

#include <libtsp.h>   /* p_ and const */
#include <AFpar.h>    /* struct AF_NHpar */
#include <AO.h>       /* struct AO_FIpar */

#define PROGRAM "InfoAudio"
#define VERSION "v10r3  2020-11-30"

#define AFPATH_ENV  "$AUDIOPATH"

#define MAXFILE   100

#define IA_FIpar  AO_FIpar  /* Input file structure */

/* Error messages */
#define IAM_BadInfoCode "Invalid info code parameter"
#define IAM_LateFPar    "Input file parameter specified after input files"
#define IAM_NoFName     "No filenames specified"
#define IAM_XFName      "Too many filenames specified"

/* Printout formats */
#define IAM_ByteStream  "File byte order: byte-stream\n"
#define IAM_ChunkInfo   "\n--File layout--\n"
#define IAM_FFormat     "\n--File format--\n"
#define IAM_InfoRec     "\n--Information records--\n"
#define IAM_stdin       "File name: <stdin>\n"

#define IAMF_FName      "File name: %s\n"
#define IAMF_Header     "Offset to data: %ld\n" \
                        "Sampling frequency: %.5g\n" \
                        "No. frames: %ld\n" \
                        "No. channels: %ld\n" \
                        "Data type: %s\n"
#define IAMF_FByteOrder "Data byte order: %s\n"
#define IAMF_HByteOrder "Host byte order: %s\n"

/* Usage */
#define IAMF_Usage "\
Usage: %s [options] AFile\n\
Options:\n\
  -i ICODE, --info_code=ICODE Information to be printed (ICODE: 1+2+4).\n\
  -t FTYPE, --type=FTYPE      Input file type,\n\
                              \"auto\", \"AIFF\", \"AU\", \"WAVE\", \"text-audio\"\n\
                              \"noheader\", \"IRCAM\", \"SPHERE\", \"ESPS\", \"INRS\",\n\
                              \"SPPACK\", \"SPW\", \"NSP\".\n\
  -P PARMS, --parameters=PARMS  Parameters for input files,\n\
                              \"Format,Start,Sfreq,Swapb,Nchan,FullScale\".\n\
  -h, --help                  Print a list of options and exit.\n\
  -v, --version               Print the version number and exit."

#ifdef __cplusplus
extern "C" {
#endif

/* Prototypes */

void
IAoptions(int argc, const char *argv[], int *Icode,
          struct IA_FIpar FI[MAXFILE], int *Nfiles);
void
IAfileInfo(const AFILE *AFp, const char Fname[], FILE *fpinfo);

#ifdef __cplusplus
}
#endif

#endif /* InfoAudio_h_ */
