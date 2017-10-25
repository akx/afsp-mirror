/*------------ Telecommunications & Signal Processing Lab --------------
                         McGill University

Routine:
  nucleus.h

Description:
  Function prototypes for the TSP library internal (nucleus) routines

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 2.72 $  $Date: 2017/06/27 00:03:26 $

----------------------------------------------------------------------*/

#ifndef nucleus_h_
#define nucleus_h_

#include <stdio.h>  /* typedef for FILE */
#include <time.h>   /* typedef for time_t */

enum AF_FD_T;
enum AF_FIX_T;
enum AF_FT_T;
enum AF_FTW_T;
enum AF_OPT_T;

struct AF_ndata;
struct AF_read;
struct AF_write;

#ifndef AFILE_t_
# define  AFILE_t_
  typedef struct AF_filepar AFILE;  /* Audio file parameters */
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* ----- AF Prototypes ----- */
/* nucleus */
int
AFcheckDataPar (FILE *fp, int Lw, long int *Dstart, struct AF_ndata *NData,
                enum AF_FIX_T Fix);
struct AF_opt *
AFoptions (enum AF_OPT_T Cat);
int
AFpreSetWPar (enum AF_FTW_T FtypeW, enum AF_FD_T Dformat, long int Nchan,
              double Sfreq, struct AF_write *AFw);
void
AFprintAFpar (AFILE *AFp, const char Fname[], FILE *fpinfo);
AFILE *
AFsetRead (FILE *fp, enum AF_FT_T Ftype, const struct AF_read *AFr,
           enum AF_FIX_T Fix);
AFILE *
AFsetWrite (FILE *fp, enum AF_FT_T Ftype, const struct AF_write *AFw);

/* ----- FI Prototypes ----- */
double
FIrCosF (double x, double alpha);
double
FIxKaiser (double x, double alpha);

/* ----- FL Prototypes ----- */
int
FLdReadTF (FILE *fp, int MaxNval, int cc, double x[]);
int
FLexist (const char Fname[]);
int
FLfReadTF (FILE *fp, int MaxNval, int cc, float x[]);
long int
FLfileSize (FILE *fp);
int
FLjoinNames (const char Dname[], const char Bname[], char Fname[]);
int
FLseekable (FILE *fp);
int
FLterm (FILE *fp);

/* ----- MA Prototypes ----- */
int
MAfChFactor (const float *A[], float *L[], int N);
void
MAfLTSolve (const float *L[], const float b[], float x[], int N);
void
MAfTTSolve (const float *L[], const float b[], float x[], int N);

/* ----- MS Prototypes ----- */
double *
MScoefMC (double x1, double x2, double y1, double y2, double d1, double d2);
double
MSdSlopeMC (int k, const double x[], const double y[], int N);
double
MSevalMC (double x, double x1, double x2, double y1, double y2, double d1,
          double d2);
double
MSfSlopeMC (int k, const float x[], const float y[], int N);

/* ----- SP Prototypes ----- */
void
SPdPreFFT (double x[], double y[], int N, int Ifn);
void
SPdTrMat2 (double A[], int N, int Ifn);
void
SPfPreFFT (float x[], float y[], int N, int Ifn);
void
SPfTrMat2 (float A[], int N, int Ifn);

/* ----- ST Prototypes ----- */
int
STdec1val (const char String[], int Type, void *Val);
int
STdecNval (const char String[], int Nmin, int Nmax, int Type, void *Val,
           int *N);
int
STdecPair (const char String[], const char Delim[], int Type, void *Val1,
           void *Val2);
int
STdecSpair (const char String[], const char Delim[], int *I1, int *I2,
            const char *SymTab[]);
char *
STstrDots (const char Si[], int Maxchar);
char *
STstrstrNM (const char Si[], const char Ti[], int N, int M);
char *
STtrimIws (const char Si[]);
int
STtrimNMax (const char Si[], char So[], int N, int Maxchar);
int
STtrimTail (char Si[]);

/* ----- UT Prototypes ----- */
enum UT_DS_T
UTbyteCode (enum UT_DS_T Dbo);
enum UT_DS_T
UTbyteOrder (void);
int
UTcheckIEEE (void);
char *
UTctime (time_t *timer, int format);
double
UTdIEEE80 (const unsigned char b[10]);
int
UTdecOption (const char String[], const char Option[], const char **Arg);
void
UTeIEEE80 (double V, unsigned char b[10]);
char *
UTgetHost (void);
enum UT_DS_T
UTswapCode (enum UT_DS_T Dbo);
char *
UTgetUser (void);

/* ----- VR Prototypes ----- */
void
VRswapBytes (const void *BuffI, void *BuffO, int Size, int Nelem);

#ifdef __cplusplus
}
#endif

#endif /* nucleus_h_ */
