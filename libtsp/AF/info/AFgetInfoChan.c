/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  long int AFgetInfoChan (const struct AF_info *AFInfo)

Purpose:
  Find and decode an information record specifying the number of channels

Description:
  This routine searches the information record structure for a record specifying
  the number of channels

  The information record identifier is one of the following strings,
    "channels:", "channel_count:"

  The information following the identifier consists of an integer.  An example
  is
    "channels: 2"

Parameters:
  <-  long int AFgetInfoChan
      Number of channels value determined from the information record.  This is
      set to zero, if the record was not found or a decoding error occurred.
   -> const struct AF_info *AFInfo
      AFsp information structure

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.3 $  $Date: 2017/07/03 22:25:14 $

-------------------------------------------------------------------------*/

#include <libtsp.h>
#include <libtsp/nucleus.h>
#include <AFpar.h>
#include <libtsp/AFinfo.h>
#include <libtsp/AFmsg.h>

/* Sampling rate keywords */
static const char *RecID[] = AF_INFOID_NCHAN;


long int
AFgetInfoChan (const struct AF_info *AFInfo)

{
  const char *p;
  long int Nch, Nchan;
  int status;

/* Search for named records in an AFsp information structure */
  p = AFgetInfoRec (RecID, AFInfo);

  Nchan = 0;
  if (p == NULL)        /* Record not found */
    return Nchan;

  /* Decode the value */
  status = STdec1long (p, &Nch);
  if (status != 0)
    UTwarn ("AFgetInfoChan - %s", AFM_Info_DecNchan);
  else if (Nch <= 0)
    UTwarn ("AFgetInfoChan - %s", AFM_Info_InvNchan);
  else
    Nchan = Nch;

  return Nchan;
}
