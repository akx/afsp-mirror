/*------------ Telecommunications & Signal Processing Lab --------------
                         McGill University

Routine:
  AFheader.h

Description:
  Function prototypes for the TSP library AF file internal routines

Author / revision:
  P. Kabal  Copyright (C) 2020
  $Revision: 1.57 $  $Date: 2020/11/25 17:56:06 $

----------------------------------------------------------------------*/

#ifndef AFheader_h_
#define AFheader_h_

#include <stdio.h>    /* typedef for FILE */
#include <setjmp.h>   /* typedef for jmp_buf */

#include <AFpar.h>

enum AF_FT_T;
enum AF_FTW_T;
enum AF_FD_T;
enum UT_DF_T;

#ifndef AFILE_t_
#  define AFILE_t_
typedef struct AF_filepar AFILE;  /* Audio file parameters */
#endif

/* Header read/write definitions */
#define RHEAD_SN(fp,string,N) \
  AFreadHead(fp, (void *) (string), 1, (int) (N), DS_NATIVE)
#define RHEAD_S(fp,string) RHEAD_SN(fp, string, sizeof(string))
#define RHEAD_V(fp,value,swap) \
  AFreadHead(fp, (void *) &(value), sizeof(value), 1, (int) (swap))
#define RSKIP(fp,nbytes) RHEAD_SN(fp, NULL, (int) (nbytes))
#define WHEAD_SN(fp,string,N) \
  AFwriteHead(fp, (const void *) (string), 1, (int) (N), DS_NATIVE)
#define WHEAD_S(fp,string) WHEAD_SN(fp, string, sizeof(string))
#define WHEAD_V(fp,value,swap) \
  AFwriteHead(fp, (const void *) &(value), sizeof(value), 1, \
              (enum UT_DS_T) (swap))

/* WRPAD: Add null bytes to bring size up to a multiple of align */
#define WRPAD(fp,size,align) \
  AFwriteHead(fp, NULL, 1, (int) (RNDUPV(size, align) - (size)), DS_NATIVE)
#define MCOPY(src,dest)   memcpy((void *) (dest), \
                                  (const void *) (src), sizeof(dest))

#define ICEILV(n,m) (((n) + ((m) - 1)) / (m))  /* int n,m >= 0 */
#define RNDUPV(n,m) ((m) * ICEILV(n, m))       /* Round up */
#define MINV(a, b)  (((a) < (b)) ? (a) : (b))

#define SAME_CSTR(str,ref)  (memcmp(str, ref, sizeof(str)) == 0)

#ifdef __cplusplus
extern "C" {
#endif

/* AF/header function prototypes */
int
AFfindFtype(FILE *fp);
AFILE *
AFrdAIhead(FILE *fp);
AFILE *
AFrdAUhead(FILE *fp);
AFILE *
AFrdBLhead(FILE *fp);
AFILE *
AFrdEShead(FILE *fp);
int
AFrdInfoText(FILE *fp, int Size, const char Ident[], struct AF_info *Info,
              int Align, int Mode);
#define AFrdInfoIdentText(fp, Size, Ident, Info, Align) \
                          AFrdInfoText(fp, Size, Ident, Info, Align, 0)
#define AFrdInfoAFspIdentText(fp, Size, Ident, Info, Align) \
                          AFrdInfoText(fp, Size, Ident, Info, Align, 1)
#define AFrdInfoAFspText(fp, Size, Info, Align) \
                          AFrdInfoText(fp, Size, NULL, Info, Align, 2)
AFILE *
AFrdINhead(FILE *fp);
AFILE *
AFrdNShead(FILE *fp);
AFILE *
AFrdSFhead(FILE *fp);
AFILE *
AFrdSPhead(FILE *fp);
AFILE *
AFrdSWhead(FILE *fp);
AFILE *
AFrdTAhead(FILE *fp);
AFILE *
AFrdWVhead(FILE *fp);
int
AFreadHead(FILE *fp, void *Buf, int size, int Nelem, int Swapb);
void
AFsetChunkLim(const char *ChunkID, long int Start, long int End,
               struct AF_chunkInfo *ChunkInfo);
AFILE *
AFsetNHread(FILE *fp);
AFILE *
AFsetNHwrite(FILE *fp, struct AF_write *AFw);
int
AFupdAIhead(AFILE *AFp);
int
AFupdAUhead(AFILE *AFp);
int
AFupdHead(AFILE *AFp);
int
AFupdWVhead(AFILE *AFp);
AFILE *
AFwrAIhead(FILE *fp, struct AF_write *AFw);
AFILE *
AFwrAUhead(FILE *fp, struct AF_write *AFw);
AFILE *
AFwrTAhead(FILE *fp, struct AF_write *AFw);
AFILE *
AFwrWVhead(FILE *fp, struct AF_write *AFw);
int
AFwriteHead(FILE *fp, const void *Buf, int Size, int Nv, int Swapb);

#ifdef __cplusplus
}
#endif

#endif /* AFheader_h_ */
