/*------------ Telecommunications & Signal Processing Lab --------------
                         McGill University

Routine:
  AFheader.h

Description:
  Function prototypes for the TSP library AF file internal routines

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.32 $  $Date: 2003/01/27 12:59:25 $

----------------------------------------------------------------------*/

#ifndef AFheader_h_
#define AFheader_h_

#include <stdio.h>	/* typedef for FILE */
#include <setjmp.h>	/* typedef for jmp_buf */

struct AF_opt;				/* Audio file routine options */
struct AF_NHpar;			/* Headerless audio file parameters */
struct AF_infox;			/* Header information string */
struct AF_info;				/* Header information string */
struct AF_infoX;       			/* Header information string */
struct AF_read;				/* File format and data parameters */
struct AF_write;			/* File format and data parameters */

#ifndef	AFILE_t_
#  define AFILE_t_
typedef struct AF_filepar AFILE;	/* Audio file parameters */
#endif

/* Header read/write definitions */
#define RHEAD_S(fp,string) \
	AFreadHead (fp, (void *)(string), 1, (int) (sizeof (string)), \
		    DS_NATIVE)
#define RHEAD_SN(fp,string,N) \
	AFreadHead (fp, (void *) (string), 1, (int) (N), DS_NATIVE)
#define RHEAD_V(fp,value,swap) \
	AFreadHead (fp, (void *) &(value), (int) (sizeof (value)), 1, \
		    (int) (swap))
#define RSKIP(fp,nbytes) \
	AFreadHead (fp, NULL, 1, (int) (nbytes), DS_NATIVE)
#define WHEAD_S(fp,string) \
	AFwriteHead (fp, (const void *) (string), 1, (int) (sizeof (string)), \
		     DS_NATIVE)
#define WHEAD_SN(fp,string,N) \
	AFwriteHead (fp, (const void *) (string), 1, (int) (N), DS_NATIVE)
#define WHEAD_P(fp,string) \
	AFwriteHead (fp, (const void *) (string), 1, (int) (strlen (string)), \
		     DS_NATIVE)
#define WHEAD_V(fp,value,swap) \
	AFwriteHead (fp, (const void *) &(value), (int) (sizeof (value)), 1, \
		     (int) (swap))

#ifdef __cplusplus
extern "C" {
#endif

void
AFaddHtext (const char Ident[], const char line[], int Size,
	    struct AF_infoX *Hinfo);
int
AFdecSpeaker (const char String[], unsigned char *SpkrConfig, int MaxN);
int
AFcheckSpeakers (const unsigned char *SpkrConfig);
int
AFfindType (FILE *fp);
const char *
AFgetHrec (const char name[], const struct AF_info *Hinfo);
struct AF_opt *
AFoptions (void);
void
AFprAFpar (AFILE *AFp, const char Fname[], FILE *fpinfo);
int
AFpreSetWPar (int Fformat, long int Nchan, double Sfreq, struct AF_write *AFw);
AFILE *
AFrdAIhead (FILE *fp);
AFILE *
AFrdAUhead (FILE *fp);
AFILE *
AFrdBLhead (FILE *fp);
AFILE *
AFrdEShead (FILE *fp);
int
AFrdHinfo (FILE *fp, int Size, struct AF_infoX *Hinfo, int Align);
int
AFrdHtext (FILE *fp, int Size, const char Ident[], struct AF_infoX *Hinfo,
	   int Align);
AFILE *
AFrdINhead (FILE *fp);
AFILE *
AFrdNShead (FILE *fp);
AFILE *
AFrdSFhead (FILE *fp);
AFILE *
AFrdSPhead (FILE *fp);
AFILE *
AFrdSWhead (FILE *fp);
AFILE *
AFrdTAhead (FILE *fp);
AFILE *
AFrdWVhead (FILE *fp);
int
AFreadHead (FILE *fp, void *Buf, int size, int Nelem, int Swapb);
struct AF_opt *
AFresetOptions (unsigned int Cat);
AFILE *
AFsetNHread (FILE *fp, const char Fname[]);
AFILE *
AFsetNHwrite (FILE *fp, struct AF_write *AFw);
AFILE *
AFsetRead (FILE *fp, int Ftype, const struct AF_read *AFr, int Fix);
AFILE *
AFsetWrite (FILE *fp, int Ftype, const struct AF_write *AFw);
int
AFspeakerNames (const unsigned char *SpkrConfig, char *SpkrNames, int MaxNc);
int
AFupdAIhead (AFILE *AFp);
int
AFupdAUhead (AFILE *AFp);
int
AFupdHead (AFILE *AFp);
int
AFupdWVhead (AFILE *AFp);
AFILE *
AFwrAIhead (FILE *fp, struct AF_write *AFw);
AFILE *
AFwrAUhead (FILE *fp, struct AF_write *AFw);
AFILE *
AFwrWVhead (FILE *fp, struct AF_write *AFw);
int
AFwriteHead (FILE *fp, const void *Buf, int Size, int Nv, int Swapb);

#ifdef __cplusplus
}
#endif

#endif /* AFheader_h_ */
