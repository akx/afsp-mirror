/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void RSwriteCof (const char Fname[], const struct Fspec_T *Fs,
                   const float h[]);

Purpose:
  Write a filter coefficient file

Description:
  This routine writes a coefficient file for an interpolating filter.  The data
  written to the file includes a header.  If an input file name (Fs->FFile) is
  specified, then the filter ratio and delay are written as comments to the
  file header.  Otherwise the parameters for a Kaiser windowed lowpass filter
  filter used as an interpolating filter are written as comments to the file
  header.  This header is followed by the filter coefficients.  The file name
  is also printed on standard output.

Parameters:
   -> const char Fname[]
      File name for the coefficient file
   -> const struct Fspec_T *Fs
      Filter specification structure
   -> const float h[]
      Filter coefficients

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.9 $  $Date: 2003/05/13 01:09:07 $

-------------------------------------------------------------------------*/

#include <libtsp.h>
#include "ResampAudio.h"


void
RSwriteCof (const char Fname[], const struct Fspec_T *Fs, const float h[])

{
  FILE *fp;
  char Fullname[FILENAME_MAX];
  int i;

/* Open the coefficient file, print the name to stdout */
  FLbackup (Fname);
  fp = fopen (Fname, "w");
  if (fp == NULL)
    UTerror ("%s: %s", PROGRAM, RSM_OpenCErr);
  FLfullName (Fname, Fullname);
  printf (RSMF_CoefFile, Fullname);

/* Header */
  if (Fs->FFile == NULL)
    fprintf (fp, RSMF_KWFile,
	     Fs->Ir, Fs->Fc, Fs->alpha, Fs->Gain,
	     Fs->Del, Fs->Woffs, Fs->Wspan);
  else
    fprintf (fp, RSMF_IntFile, Fs->Ir, Fs->Del);

/* Coefficient values */
  for (i = 0; i < Fs->Ncof; ++i) {
    fprintf (fp, "%15.7g", h[i]);
    if ((i + 1) % 5 == 0 || i + 1 == Fs->Ncof)
      fprintf (fp, "\n");
  }

  fclose (fp);
}
