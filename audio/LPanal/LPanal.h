/*------------ Telecommunications & Signal Processing Lab --------------
                         McGill University

Routine:
  LPanal.h

Description:
  Declarations for LPanal

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.24 $  $Date: 2003/11/03 13:09:58 $

----------------------------------------------------------------------*/

#ifndef LPanal_h_
#define LPanal_h_

#define PROGRAM "LPanal"
#define VERSION	"v3r0a  2003-11-03"

#define MAXWINDOW	512
#define MAXFRAME	512
#define MAXNP		50
#define MAXINFO		1024

#include <libtsp.h>	/* typedef for AFILE */

#ifdef __cplusplus
extern "C" {
#endif

/* Prototypes */
void
LPoptions (int argc, const char *argv[], char Fname[4][FILENAME_MAX]);
void
LPlpcAnal (AFILE *AFpI, AFILE *AFpL, AFILE *AFpO, double pre,
	   const float Win[], int Lwin, int Woffs, int Lframe, int Np,
	   double bwexp);
void
LPlpcPar (const char Fname[], double *pre, const float **Win, int *Lwin,
	  int *Woffs, int *Lframe, int *Np, double *bwexp);

#ifdef __cplusplus
}
#endif

#endif /* LPanal_h_ */
