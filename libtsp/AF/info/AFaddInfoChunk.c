/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int AFaddInfoChunk (const char Recs[], int Nc, struct AF_info *AFInfo)

Purpose:
  Append information record(s) to an information record structure

Description:
  The input consists of one or more information records.  Each record is null
  terminated.  These information records are appended to an information record
  structure.  Each information record normally consists of an identification
  string followed by text.

  A trailing null is appended if one is not present.

Parameters:
  <-  AFaddInfoChunk
      Number of characters added to the information record structure
   -> const char Recs[]
      Input null terminated information record(s)
   -> int Nc
      Number of characters in the information record(s), including the null
      terminating character of each record.
  <-> struct AF_info *AFInfo
      Information record structure

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.5 $  $Date: 2017/07/12 17:29:43 $

-------------------------------------------------------------------------*/

#include <string.h>

#include <libtsp.h>
#include <AFpar.h>
#include <libtsp/AFmsg.h>
#include <libtsp/nucleus.h>


int
AFaddInfoChunk (const char Recs[], int Nc, struct AF_info *AFInfo)

{
  int NI, Nt;

  /* Info record chunks can have several trailing nulls due to file padding */
  /* Find the last non-null character */
  for (; Nc > 0; --Nc) {
    if (Recs[Nc-1] != '\0')
      break;
  }

  NI = AFInfo->N;
  if (AFInfo == NULL || NI + Nc + 1 > AFInfo->Nmax) {
    UTwarn ("AFaddInfoChunk - %s", AFM_LongInfo);
    Nc = 0;
  }
  else {
    memcpy (&AFInfo->Info[NI], Recs, Nc);
    Nt = NI + Nc;
    AFInfo->Info[Nt++] = '\0';      /* Add a terminating null */
    ++Nc;
    AFInfo->N = Nt;
  }

  return Nc;
}
