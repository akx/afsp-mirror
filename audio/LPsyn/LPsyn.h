/*------------ Telecommunications & Signal Processing Lab --------------
                         McGill University

Routine:
  LPsyn.h

Description:
  Declarations for LPsyn

Author / revision:
  P. Kabal  Copyright (C) 2002
  $Revision: 1.21 $  $Date: 2002/11/06 14:06:29 $

----------------------------------------------------------------------*/

#ifndef LPsyn_h_
#define LPsyn_h_

#define PROGRAM "LPsyn"
#define VERSION	"v2r3e  2002-11-06"

#define MAXWINDOW	512
#define MAXFRAME	512
#define MAXNP		50

#include <libtsp.h>		/* typedef for AFILE */

#ifdef __cplusplus
extern "C" {
#endif

/* Prototypes */
void
LSoptions (int argc, const char *argv[], int *Fstats,
	   char Fname[4][FILENAME_MAX]);
void
LSlpcSyn (AFILE *AFpI, AFILE *AFpL, AFILE *AFpO, int Fstats, double pre,
	  int Lframe, int Np);
void
LSlpcPar (const char Fname[], float *pre, const float **Win, int *Lwin,
	  int *Woffs, int *Lframe, int *Np, float *bwexp);

#ifdef __cplusplus
}
#endif

#endif /* LPsyn_h_ */
