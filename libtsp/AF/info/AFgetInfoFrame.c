/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  long int AFgetInfoFrame (const struct AF_info *AFInfo)

Purpose:
  Find and decode an information record specifying the number of frames

Description:
  This routine searches the information record structure for a record specifying
  the number of frames

  Bits/sample information record ID:
      "frames:" "samples/channel:"

  The information following the identifier consists of an integer. Examples are
      "frames: 20010"
      "samples/channel: 40000"

Parameters:
  <-  long int AFgetInfoFrame
      Number of frames value determined from the information record.  This is
      set to AF_NFRAME_UNDEF, if the record was not found or a decoding error
      occurred.
   -> const struct AF_info *AFInfo
      AFsp information structure

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.4 $  $Date: 2017/07/03 22:25:14 $

-------------------------------------------------------------------------*/

#include <libtsp.h>
#include <libtsp/nucleus.h>
#include <AFpar.h>
#include <libtsp/AFinfo.h>
#include <libtsp/AFmsg.h>

/* Sampling rate keywords */
static const char *RecID[] = AF_INFOID_NFRAME;


long int
AFgetInfoFrame (const struct AF_info *AFInfo)

{
  const char *p;
  long int Nfr, Nframe;
  int status;

/* Search for named records in an AFsp information structure */
  p = AFgetInfoRec (RecID, AFInfo);

  Nframe = AF_NFRAME_UNDEF;
  if (p == NULL)        /* Record not found */
    return Nframe;

  /* Decode the value */
  status = STdec1long (p, &Nfr);
  if (status != 0)
    UTwarn ("AFgetInfoFrame - %s", AFM_Info_DecNframe);
  else if (Nfr < 0)     /* Allow Nframe == 0 */
    UTwarn ("AFgetInfoFrame - %s", AFM_Info_InvNframe);
  else
    Nframe = Nfr;

  return Nframe;
}
