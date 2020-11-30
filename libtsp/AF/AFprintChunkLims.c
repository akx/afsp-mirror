/*------------- Telecommunications & Signal Processing Lab -------------
                           McGill University

Routine:
  void AFprintChunkLims(const AFILE *AFp, FILE *fpinfo)

Purpose:
  Print the header/data (chunk) structure for an audio file

Description:
  This formats and prints the file organization information. For IFF
  (Interchange File Format) audio files, the file is organized a chunks (named
  digital containers). These chunks are used to store auxiliary information
  and the audio data itself. Examples of IFF files supported by AFopnRead are
  AIFF/AIFF-C sound files, WAVE files, and CSL NSP files.

  For other file formats pseudo chunks are identified. These pseudo chunks
  typically include the file identification string, the data format information
  ("fmt "), and the data storage area ("data").

  The AFp->ChunkInfo structure has the following format,
    struct AF_chunkInfo {
      struct AF_chunkLim *ChunkLim[Nmax];
      int Nmax;             Size of ChunkLim
      int N;                Number of ChunkLim values set
    }
  where struct AF_ChunkLim is defined as
    struct AF_chunkLim {
      unsigned char ID[4];  Chunk ID
      long int Start;       Chunk start position in bytes
      long int End;         Last byte in the chunk
    }

Parameters:
  <-  void AFprintChunkLims
   -> const AFILE *AFp
      Audio file parameter structure
  <-> FILE *fpinfo
      Output stream for the file information. If fpinfo is NULL, no information
      is written.

Author / revision:
  P. Kabal  Copyright (C) 2020
  $Revision: 1.14 $  $Date: 2020/11/26 11:21:31 $

----------------------------------------------------------------------*/

#include <ctype.h>      /* isprint */
#include <string.h>     /* memcpy */

#include <libtsp.h>
#include <AFpar.h>
#include <libtsp/AFmsg.h>
#include <libtsp/nucleus.h>

#define MAXLEV      4
#define INDENT      5

struct Lev_Limits {
  char ID[4];
  long int Start;
  long int End;
};

static int
AF_checkChunk(FILE *fpinfo, char ID[4], long int Start, long int End,
              int AtEoF, int prelev, struct Lev_Limits LevLim[]);
static void
AF_sanString(char String[], int N);


void
AFprintChunkLims(const AFILE *AFp, FILE *fpinfo)

{
  const struct AF_chunkInfo *ChunkInfo;
  struct Lev_Limits LevLim[MAXLEV+1];
  long int Start, End, FLsize;
  int lev, prelev, n, N, indent, AtEoF;
  const struct AF_chunkLim *ChunkLim;
  char ID[4];

  if (fpinfo == NULL)
    return;

  ChunkInfo = &AFp->ChunkInfo;
  N = ChunkInfo->N;
  if (N <= 0)
    return;

  ChunkLim = ChunkInfo->ChunkLim;
  if (FLseekable(AFp->fp))
    FLsize = FLfileSize(AFp->fp);
  else
    FLsize = AF_EoF;

/* Initialize level 0 (big enough to contain any chunk */
  AtEoF = 0;
  prelev = 0;
  memcpy(LevLim[0].ID, "base", 4);
  LevLim[0].Start = 0;
  LevLim[0].End = AF_EoF;   /* Big positive number */

  for (n = 0; n <= N; ++n) {
    if (n < N) {
      memcpy(ID, ChunkLim[n].ID, 4);
      AF_sanString(ID, 4);
      Start = ChunkLim[n].Start;
      End = ChunkLim[n].End;
      if (End == AF_EoF && FLsize != AF_EoF)
        End = FLsize - 1;
    }
    else {                        /* Terminating level 1 chunk */
      AtEoF = 1;
      memcpy(ID, "EoF ", 4);
      Start = FLsize;
      End = AF_EoF;
    }

    /* Print a continuation line */
    if (n > 0 && Start > ChunkLim[n-1].Start + 8) {
      indent = (prelev - 1) * INDENT;
      fprintf(fpinfo, "%*s  ...\n", indent, " ");
    }

    /* Skip an empty chunk, e.g. zero length data chunks */
    if (!AtEoF && End != AF_EoF && End - Start + 1 == 0)   /* Empty chunk */
      continue;

    /* Get the new chunk level, check for chunk inconsistencies */
    lev = AF_checkChunk(fpinfo, ID, Start, End, AtEoF, prelev, LevLim);

    /* Print the start of the chunk */
    indent = (lev - 1) * INDENT;
    if (!AtEoF) {
      if (End != AF_EoF)
        fprintf(fpinfo, "%*s<%.4s> %ld -> %ld\n", indent, " ", ID, Start, End);
      else
        fprintf(fpinfo, "%*s<%.4s> %ld -> EoF\n", indent, " ", ID, Start);
    }
    else if (Start != AF_EoF)
      fprintf(fpinfo, "%*s<%.4s> %ld\n", indent, " ", ID, Start);
    else
      fprintf(fpinfo, "%*s<%.4s>\n", indent, " ", ID);

    prelev = lev;
  }
}

static int
AF_checkChunk(FILE *fpinfo, char ID[4], long int Start, long int End,
              int AtEoF, int prelev, struct Lev_Limits LevLim[])

{
  int i, lev;
  long int Gap;

/* Chunk rules
   1. Level zero has a single all encompassing chunk. Higher levels are
      contained in lower levels.
   2. The Start value for chunks must be strictly increasing since the chunks
      are peeled off the file in order.
   3. The Start value must abut the End values of chunks of the same or higher
      level up to the previous level.
   4. The Start value of a chunk can be after the Start value of the enclosing
      chunk, but the End value of the last chunk at a given level must match
      the End value of the enclosing chunk.
   5. No chunk can be strictly inside the chunk at the next higher level. By
      induction, this is also true for all higher levels.
*/

  /* Find the level which completely contains the current chunk */
  if (AtEoF)
    lev = 1;
  else {
    for (lev = prelev; lev >= 0; --lev) {
      if (Start >= LevLim[lev].Start && End <= LevLim[lev].End)
        break;
    }
    if (lev < MAXLEV)
      ++lev;
    else
      UTwarn("AFprintChunkLims - Chunks nested too deeply");
  }

  /* Check the rules */
  if (Start != AF_EoF) {
    for (i = prelev; i >= lev; --i) {
      Gap = Start - (LevLim[i].End + 1);
      if (Gap > 0) {
        if (!AtEoF)
          fprintf(fpinfo, AFMF_ChunkGap, LevLim[i].ID, ID);
        else
          fprintf(fpinfo, AFMF_ChunkGapEoF, LevLim[i].ID);
      }
      else if (Gap < 0) {
        if (!AtEoF)
          fprintf(fpinfo, AFMF_ChunkOvlp, ID, LevLim[i].ID);
        else
          fprintf(fpinfo, AFMF_ChunkPastEoF, LevLim[i].ID);
      }
    }
  }

  /* Set the level limits to the current limits */
  memcpy(LevLim[lev].ID, ID, 4);
  LevLim[lev].Start = Start;
  LevLim[lev].End = End;

  return lev;
}

static void
AF_sanString(char String[], int N)

{
  int i;

  /* Sanitize the string */
  for (i = 0; i < N; ++i) {
    if (!isprint((unsigned char) String[(unsigned)i]))
      String[i] = '?';
    }
}
