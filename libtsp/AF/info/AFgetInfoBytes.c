/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int AFgetInfoBytes (const struct AF_info *AFInfo)

Purpose:
  Find and decode an information record specifying the number of bytes used to
  represent each sample

Description:
  This routine searches the information record structure for a record specifying
  the number of bytes per sample.

  Bytes/sample information record ID:
      "bytes/sample:", sample_n_bytes:"
  The information following the identifier consists of an integer. Examples are
      "bytes/sample: 3"
      "sample_n_bytes: 2"

Parameters:
  <-  int AFgetInfoBytes
      This is the decoded value of the number of bytes.  It is set to zero if
      no appropriate information record was found.
   -> const struct AF_info *AFInfo
      AFsp information structure

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.3 $  $Date: 2017/07/03 22:25:13 $

-------------------------------------------------------------------------*/

#include <libtsp.h>
#include <libtsp/nucleus.h>
#include <AFpar.h>
#include <libtsp/AFinfo.h>
#include <libtsp/AFmsg.h>

/* Bytes keywords */
static const char *RecID[] = AF_INFOID_BYTES;


int
AFgetInfoBytes (const struct AF_info *AFInfo)

{
  const char *p;
  int status, Lw, Nbytes;

  /* Search for named records in an AFsp information structure */
  p = AFgetInfoRec (RecID, AFInfo);

  Nbytes = 0;
  if (p == NULL)        /* Record not found */
    return Nbytes;

  /* Decode the value */
  status = STdec1int (p, &Lw);
  if (status != 0)
    UTwarn ("AFgetInfoBytes - %s", AFM_Info_DecBytes);
  else if (Lw <= 0 || 8 * Lw > AF_RES_MAX)
    UTwarn ("AFgetInfoBytes - %s", AFM_Info_InvBytes);
  else
    Nbytes = Lw;

  return Nbytes;
}
