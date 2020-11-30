/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  AFILE *AFsetNHread FILE *fp, const char Fname[])

Purpose:
  Get file format information for a headerless audio file

Description:
  This routine gets file information for an audio file with a non-standard
  header or with no header. File format information is passed to this routine by
  calling AFsetInputPar before calling this routine. This information used to
  set the file data format information in the audio file pointer structure.

Parameters:
  <-  AFILE *AFsetNHread
      Audio file pointer for the audio file
   -> FILE *fp
      File pointer for the file
   -> const char Fname[]
      File name

Author / revision:
  P. Kabal  Copyright (C) 2020
  $Revision: 1.83 $  $Date: 2020/11/25 17:54:50 $

-------------------------------------------------------------------------*/

#include <libtsp.h>
#include <AFpar.h>
#include <libtsp/nucleus.h>
#include <libtsp/AFdataio.h>
#include <libtsp/AFheader.h>
#include <libtsp/AFmsg.h>

AF_READ_DEFAULT(AFr_default); /* Define the AF_read defaults */


AFILE *
AFsetNHread(FILE *fp)

{
  AFILE *AFp;
  struct AF_InputPar *InputPar;
  struct AF_read AFr;

/* Defaults and initial values */
  AFr = AFr_default;

/* Get the file data parameters */
  InputPar = &AFopt.InputPar;

/* Check the data format */
  if (InputPar->Format == FD_UNDEF) {
    UTwarn("AFsetNHread - %s", AFM_NH_NoFormat);
    return NULL;
  }

/* Position at the start of data */
  if (RSKIP(fp, InputPar->Start) != InputPar->Start)
    return NULL;

/* Set the parameters for file access */
  AFr.Sfreq = InputPar->Sfreq;
  AFr.DFormat.Format = InputPar->Format;
  AFr.DFormat.Swapb = InputPar->Swapb;
  if (InputPar->FullScale == AF_FULLSCALE_DEFAULT)
    AFr.DFormat.FullScale = AF_FULLSCALE[InputPar->Format];
  else
    AFr.DFormat.FullScale = InputPar->FullScale;
  AFr.NData.Nchan = InputPar->Nchan;

  /* Set up the audio file structure */
  AFp = AFsetRead(fp, FT_NH, &AFr, AF_NOFIX);

  return AFp;
}
