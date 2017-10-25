/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  InfoAudio [options] AFile1 [AFile2 ... ]

Purpose:
  Print audio file information

Description:
  This program prints information about a number of audio files to standard
  output.  The full output consists of the file type and name (indented part of
  the output below), the file data parameters, information records, and if
  appropriate, the chunk structure of the file.

     AIFF-C sound file: --path--/audiofiles/addf8.aifc
       Samples / channel : 23808 (2.976 s)  YYYY-MM-dd hh:mm:ss UTC
       Sampling frequency: 8000 Hz
       Number of channels: 1 (16-bit integer)

    --File format--
    File name: ../../../audiofiles/addf8.aifc
    Header length: 166
    Sampling frequency: 8000
    No. frames: 23808
    No. channels: 1
    Data type: integer16
    File byte order: big-endian
    Host byte order: little-endian

    --Information records--
    date: YYYY-MM-dd hh:mm:ss UTC
    program: CopyAudio

    -- Chunk layout--
     <FORM> 0 -> 47781
         <AIFC> 8 -> 11
         <COMM> 12 -> 57
           ...
         <FVER> 58 -> 69
           ...
         <ANNO> 70 -> 149
           ...
         <SSND> 150 -> 47781
           ...
     <EoF > 47782

  Non-printing characters (as defined by the routine isprint) in the information
  records are replaced by question marks.

Options:
  Input file names, AFile1 [AFile2 ... ]:
      The environment variable AUDIOPATH specifies a list of directories to be
      searched for the input audio file(s).
  -i ICODE, --info_code=ICODE
      Parameter to choose the information to be printed.  The information that
      can be printed consists of four parts.  The ICODE parameter is the sum
      of the codes which control printing of the individual parts.
        1:  Print the standard information: file name, filetype, no. samples ...
        2:  Print the file data parameters
        4:  Print the information records
        8:  Print the chunk configuration (only for AIFF/WAVE/NSP files)
      The default is to print all of the information (ICODE=15).  For ICODE=0,
      no information is printed, but the program return code indicates if all
      files were opened successfully.
  -h, --help
      Print a list of options and exit.
  -v, --version
      Print the version number and exit.

  See routine CopyAudio for a description of other parameters.
  -t FTYPE, --type=FTYPE
     Input file type and environment variable AF_FILETYPE
  -P PARMS, --parameters=PARMS
     Input file parameters and environment variable AF_INPUTPAR

Environment variables:
  AUDIOPATH:
    This environment variable specifies a list of directories to be searched
    when opening the input audio files.  Directories in the list are separated
    by colons (semicolons for Windows).

Author / version:
  P. Kabal / v10r1  2017-10-18  Copyright (C) 2017

-------------------------------------------------------------------------*/

#include <stdlib.h> /* EXIT_SUCCESS */

#include "InfoAudio.h"

#define EXTCODE(x,n)  ((x / n) % 2)


int
main (int argc, const char *argv[])

{
  struct IA_FIpar FI[MAXFILE];
  AFILE *AFp;
  FILE *fpinfo;
  int i, Icode, Nfiles;
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
    fpinfo = stdout;

    /* Print the audio file parameter information */
    if (EXTCODE (Icode, 2) != 0) {
      fprintf (fpinfo, IAM_FFormat);
      IAfileInfo (AFp, FI[i].Fname, fpinfo);
    }

    /* Print the information strings */
    if (EXTCODE (Icode, 4) != 0 && AFp->AFInfo.N > 0) {
      fprintf (fpinfo, IAM_InfoRec);
      AFprintInfoRecs (AFp, fpinfo);
    }

    /* Print chunk information */
    if (EXTCODE (Icode, 8) != 0 && AFp->ChunkInfo.N > 0) {
      fprintf (fpinfo, IAM_ChunkInfo);
      AFprintChunkLims (AFp, fpinfo);
    }

    AFclose (AFp);
    if (i < Nfiles-1)
      fprintf (fpinfo, "\n");

  }
  return EXIT_SUCCESS;
}
