/*------------ Telecommunications & Signal Processing Lab --------------
                         McGill University

Routine:
  AFdataio.h

Description:
  Function prototypes for the TSP library AF data I/O internal routines

Author / revision:
  P. Kabal  Copyright (C) 1999
  $Revision: 1.8 $  $Date: 1999/06/04 23:27:17 $

----------------------------------------------------------------------*/

#ifndef AFdataio_h_
#define AFdataio_h_

#include <stdio.h>	/* typedef for FILE */

#ifndef	AFILE_t_
#  define AFILE_t_
typedef struct AF_filepar AFILE;	/* Audio file parameters */
#endif

#include <limits.h>
#define AF_SEEK_END	LONG_MIN	/* Used as a flag to AFseek */

#ifdef __cplusplus
extern "C" {
#endif

char *
AFgetLine (FILE *fp, int *ErrCode);
int
AFquant (double x, const float Xq[], int Nreg);
int
AFrdAlaw (AFILE *AFp, float Fbuff[], int Nreq);
int
AFrdF4 (AFILE *AFp, float Fbuff[], int Nreq);
int
AFrdF8 (AFILE *AFp, float Fbuff[], int Nreq);
int
AFrdI1 (AFILE *AFp, float Fbuff[], int Nreq);
int
AFrdI2 (AFILE *AFp, float Fbuff[], int Nreq);
int
AFrdI3 (AFILE *AFp, float Fbuff[], int Nreq);
int
AFrdI4 (AFILE *AFp, float Fbuff[], int Nreq);
int
AFrdMulaw (AFILE *AFp, float Fbuff[], int Nreq);
int
AFrdTA (AFILE *AFp, float Fbuff[], int Nreq);
int
AFrdU1 (AFILE *AFp, float Fbuff[], int Nreq);
int
AFseek (FILE *fp, long int pos, int *ErrCode);
long int
AFtell (FILE *fp, int *ErrCode);
int
AFwrAlaw (AFILE *AFp, const float Fbuff[], int Nval);
int
AFwrF4 (AFILE *AFp, const float Fbuff[], int Nval);
int
AFwrF8 (AFILE *AFp, const float Fbuff[], int Nval);
int
AFwrI1 (AFILE *AFp, const float Fbuff[], int Nval);
int
AFwrI2 (AFILE *AFp, const float Fbuff[], int Nval);
int
AFwrI3 (AFILE *AFp, const float Fbuff[], int Nval);
int
AFwrI4 (AFILE *AFp, const float Fbuff[], int Nval);
int
AFwrMulaw (AFILE *AFp, const float Fbuff[], int Nval);
int
AFwrTA (AFILE *AFp, const float Fbuff[], int Nval);
int
AFwrU1 (AFILE *AFp, const float Fbuff[], int Nval);

#ifdef __cplusplus
}
#endif

#endif /* AFdataio_h_ */
