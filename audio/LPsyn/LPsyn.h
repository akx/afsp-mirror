/*------------ Telecommunications & Signal Processing Lab --------------
                         McGill University

Routine:
  LPsyn.h

Description:
  Declarations for LPsyn

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.23 $  $Date: 2003/11/03 13:10:37 $

----------------------------------------------------------------------*/

#ifndef LPsyn_h_
#define LPsyn_h_

#define PROGRAM "LPsyn"
#define VERSION	"v3r0a  2003-11-03"

#define MAXWINDOW	512
#define MAXFRAME	512
#define MAXNP		50

#include <libtsp.h>		/* typedef for AFILE */

#ifdef __cplusplus
extern "C" {
#endif

/* Prototypes */
void
LSoptions (int argc, const char *argv[], char Fname[4][FILENAME_MAX]);
void
LSlpcSyn (AFILE *AFpI, AFILE *AFpL, AFILE *AFpO, double pre,
	  int Lframe, int Np);
void
LSlpcPar (const char Fname[], double *pre, const float **Win, int *Lwin,
	  int *Woffs, int *Lframe, int *Np, double *bwexp);

#ifdef __cplusplus
}
#endif

#endif /* LPsyn_h_ */
