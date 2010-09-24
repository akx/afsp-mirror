/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void LSlpcPar (const char Fname[], double *pre, const float **Win, int *Lwin,
                 int *Woffs, int *Lframe, int *Np, double *bwexp)

Purpose:
  Read LPC analysis/sythesis parameters

Description:
  This routine reads LPC analysis parameters from a parameter file.

Parameters:
   -> const char Fname[]
      Parameter file
  <-  double *pre
      Preemphasis factor
  <-  const float **Win
      Array of window coefficients
  <-  int *Lwin
      Number of window coefficients
  <-  int *Woffs
      Window offset relative to frame
  <-  int *Lframe
      Frame length
  <-  int *Np
      Number of LPC coefficients
  <-  double *bwexp
      Bandwidth expansion factor

Author / revision:
  P. Kabal  Copyright (C) 2009
  $Revision: 1.5 $  $Date: 2009/03/09 18:51:31 $

-------------------------------------------------------------------------*/

#include <libtsp/sysOS.h>
#ifdef SY_OS_WINDOWS
#  define _CRT_SECURE_NO_WARNINGS     /* Allow fopen, sprintf */
#endif

#include <libtsp.h>

#include "LPsyn.h"

enum {
  HAMMING = 0,
  RECT = 1
};

#define ERRSTOP(text,par)	UThalt ("%s: %s: \"%s\"", PROGRAM, text, par)

#define NENTRY	7
static const char *keytable[NENTRY+1]={
  "pre*emphasis_factor",
  "window_len*gth",
  "window_off*set",
  "window_t*ype",
  "frame_len*gth",
  "LPC_n*umber",
  "LPC_BW*_expansion",
  NULL
};
static const char *wkey[]={
  "H*amming",
  "r*ectangular",
  NULL
};


void
LSlpcPar (const char Fname[], double *pre, const float **Win, int *Lwin,
	  int *Woffs, int *Lframe, int *Np, double *bwexp)

{
  FILE *fp;
  char *Line;
  int ind;
  int Wtype;
  double prex, bwexpx;
  int Lwinx, Woffsx, Lframex, Npx;
  char line[80];
  static float Wdata[MAXWINDOW];

/* Default values */
  prex = 0.0;
  Lwinx = -1;
  Woffsx = 0;
  Wtype = HAMMING;
  Lframex = -1;
  Npx = -1;
  bwexpx = 1.0;

/* Open the parameter file */
  fp = fopen (Fname, "r");
  if (fp == NULL)
    UTerror ("%s: Cannot open file \"%s\"", PROGRAM, Fname);

/* Decode parameters */
  while (1) {
    Line = FLgetRec (fp, "!", "\\", 1);
    if (Line == NULL)
      break;
    ind = STkeyXpar (Line, "=", "\"\"", keytable, Line);
    if (ind < 0)
      break;
    switch (ind) {
    case 0:
      if (STdec1double (Line, &prex))
	ERRSTOP ("Invalid preemphasis value", Line);
      break;
    case 1:
      if (STdec1int (Line, &Lwinx) || Lwinx < 0 || Lwinx > MAXWINDOW)
	ERRSTOP ("Invalid window length", Line);
      break;
    case 2:
      if (STdec1int (Line, &Woffsx))
	ERRSTOP ("Invalid window offset value", Line);
      break;
    case 3:
      Wtype = STkeyMatch (Line, wkey);
      if (Wtype < 0)
	ERRSTOP ("Invalid window type", Line);
      break;
    case 4:
      if (STdec1int (Line, &Lframex) || Lframex < 0 || Lframex > MAXFRAME)
	ERRSTOP ("Invalid frame length", Line);
      break;
    case 5:
      if (STdec1int (Line, &Npx) || Npx < 0 || Npx > MAXNP)
	ERRSTOP ("Invalid number of LPC coefficients", Line);
      break;
    case 6:
      if (STdec1double (Line, &bwexpx))
	ERRSTOP ("Invalid LPC bandwidth expansion factor", Line);
      break;
    }
  }
  fclose (fp);

/* Set up the analysis window */
  if (Wtype == HAMMING)
    FIfWinHamm (Wdata, Lwinx, 0.46);
  else
    FIfWinHamm (Wdata, Lwinx, 0.0);

/* Error checks */
  if (Lframex < 0)
    UThalt ("%s: Frame length not set", PROGRAM);
  if (Npx < 0)
    UThalt ("%s: No. LPC coefficients not set", PROGRAM);
  if (Lwinx < 0)
    UThalt ("%s: Window length not set", PROGRAM);

/* Write the parameters to the header information string */
  /* (each line starts with "\n" to append the line) */
  AFsetInfo ("\nparameters:\\");
  sprintf (line, "\n  preemphasis_factor = %g\\", prex);
  AFsetInfo (line);
  sprintf (line, "\n  window_length = %d\\", Lwinx);
  AFsetInfo (line);
  sprintf (line, "\n  window_offset = %d\\", Woffsx);
  AFsetInfo (line);
  if (Wtype == HAMMING)
    AFsetInfo ("\n  window_type = Hamming\\");
  else
    AFsetInfo ("\n  window_type = rectangular\\");
  sprintf (line, "\n  frame_length = %d\\", Lframex);
  AFsetInfo (line);
  sprintf (line, "\n  LPC_number = %d\\", Npx);
  AFsetInfo (line);
  sprintf (line, "\n  LPC_BW_expansion = %g", bwexpx);
  AFsetInfo (line);

/* Return the values */
  *pre = prex;
  *Win = Wdata;
  *Lwin = Lwinx;
  *Woffs = Woffsx;
  *Lframe = Lframex;
  *Np = Npx;
  *bwexp = bwexpx;

  return;
}
