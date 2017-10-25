/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  AFILE *AFopenWrite (const char Fname[], int Fformat, long int Nchan,
                      double Sfreq, FILE *fpinfo)

Purpose:
  Open an audio file for writing

Description:
  Use AFopnWrite instead. The difference between the routines is the scaling
  of the data.

Parameters:
  <-  AFILE *AFopenWrite
      Audio file pointer for the audio file
   -> const char Fname[]
      Character string specifying the file name.  The file name "-" means
      standard output.
   -> int Fformat
      Audio file format code, evaluated as the sum of a data format code and a
      file type,
        Fformat = Format + 256 * FtypeW
      Format: data format
        FD_MULAW8, ... , FD_TEXT
      FtypeW: output file type
        FTW_AU, ... , FTW_AIFF        = 1,      AU audio file
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
  $Revision: 1.84 $  $Date: 2017/04/11 17:50:56 $

-------------------------------------------------------------------------*/

#include <libtsp.h>
#include <AFpar.h>

#define FTW_FTYPE_MOD 256
#define FTW_dformat(code) ((code) % FTW_FTYPE_MOD)
#define FTW_ftype(code)   ((code) / FTW_FTYPE_MOD)


AFILE *
AFopenWrite (const char Fname[], int Fformat, long int Nchan, double Sfreq,
             FILE *fpinfo)

{
  AFILE *AFp;
  enum AF_FTW_T FtypeW;
  enum AF_FD_T Format;

  FtypeW = FTW_ftype (Fformat);
  Format = FTW_dformat (Fformat);
  AFopt.ScaleV = 2768;
  AFp = AFopnWrite (Fname, FtypeW, Format, Nchan, Sfreq, fpinfo);

  return AFp;
}
