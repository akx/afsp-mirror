/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void AFsetChunkLim (const char *ChunkID, long int Start, long int End,
                      struct AF_chunkInfo ChunkInfo)

Purpose:
  Set file header chunk data into a ChunkInfo structure

Description:
  This routine inserts a chunk info record into ChunkInfo structure.  The
  information records the ID for the chunk, and the start and end of the chunk.
  The chunk info structure is initialized and expanded as necessary.  This
  routine should be called for each chunk in order.

Parameters:
   <- void AFsetChunkLim
   -> const char *ChunkID
      Chunk identifier, 4 characters.  Short identifiers will be padded out with
      blanks.
   -> long int Start
      Chunk start offset from the beginning of the file.  The start value points
      to the beginning of the chunk.  For IFF files, Start points to the chunk
      identifier.
   -> long int End
      Next byte past the chunk, encompassing any padding at the end of the
      chunk.  Use AF_EoF if the size of the chunk is unknown and the chunk
      extends to the end of the file.
  <-> struct AF_chunkInfo *ChunkInfo
      Structure to store the chunk information.

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.5 $  $Date: 2017/06/30 17:25:27 $

-------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>     /* memcpy */

#include <libtsp.h>
#include <AFpar.h>
#include <libtsp/nucleus.h>

#define NINC    10  /* Allocation steps */
#define MINV(a, b)  (((a) < (b)) ? (a) : (b))

void
AFsetChunkLim (const char *ChunkID, long int Start, long int End,
               struct AF_chunkInfo *ChunkInfo)

{
  int i, nc;

  assert (Start >= 0 && Start <= End);

  if (Start == End)
    return;

  /* Allocate or re-allocate storage */
  if (ChunkInfo->ChunkLim == NULL) {
    ChunkInfo->N = 0;
    ChunkInfo->Nmax = NINC;
    ChunkInfo->ChunkLim =
                       UTmalloc (ChunkInfo->Nmax * sizeof (struct AF_chunkLim));
  }
  else if (ChunkInfo->N >= ChunkInfo->Nmax) {
    ChunkInfo->Nmax += NINC;
    ChunkInfo->ChunkLim = UTrealloc (ChunkInfo->ChunkLim,
                                 ChunkInfo->Nmax * sizeof (struct AF_chunkLim));
  }

  /* Add limits for another chunk */
  ++ChunkInfo->N;
  i = ChunkInfo->N - 1;
  nc = MINV ((int) strlen (ChunkID), 4);
  if (nc < 4)
    memcpy (ChunkInfo->ChunkLim[i].ID, "    ", 4);
  memcpy (ChunkInfo->ChunkLim[i].ID, ChunkID, nc);
  ChunkInfo->ChunkLim[i].Start = Start;
  if (End == AF_EoF)
    ChunkInfo->ChunkLim[i].End = AF_EoF;
  else
    ChunkInfo->ChunkLim[i].End = End - 1;

  return;
}
