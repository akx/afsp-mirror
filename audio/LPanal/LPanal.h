/*------------ Telecommunications & Signal Processing Lab --------------
                         McGill University

Routine:
  LPanal.h

Description:
  Declarations for LPanal

Author / revision:
  P. Kabal  Copyright (C) 2002
  $Revision: 1.22 $  $Date: 2002/11/06 14:05:49 $

----------------------------------------------------------------------*/

#ifndef LPanal_h_
#define LPanal_h_

#define PROGRAM "LPanal"
#define VERSION	"v2r3d  2002-11-06"

#define MAXWINDOW	512
#define MAXFRAME	512
#define MAXNP		50

#include <libtsp.h>	/* typedef for AFILE */

#ifdef __cplusplus
extern "C" {
#endif

/* Prototypes */
void
LPoptions (int argc, const char *argv[], int *Fstats,
	   char Fname[4][FILENAME_MAX]);
void
LPlpcAnal (AFILE *AFpI, AFILE *AFpL, AFILE *AFpO, int Fstats, double pre,
	   const float Win[], int Lwin, int Woffs, int Lframe, int Np,
	   double bwexp);
void
LPlpcPar (const char Fname[], float *pre, const float **Win, int *Lwin,
	  int *Woffs, int *Lframe, int *Np, float *bwexp);

#ifdef __cplusplus
}
#endif

#endif /* LPanal_h_ */
