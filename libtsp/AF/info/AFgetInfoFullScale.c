/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  double AFgetInfoFullScale (const struct AF_info *AFInfo)

Purpose:
  Find and decode an information record specifying file data full scale value

Description:
  This routine searches the information record structure for a record specifying
  the file data full scale value.

  Bits/sample information record ID:
      "full_scale:"
  The information following the identifier consists of a float value or ratio
  of float values. Examples are
      "full_scale: 256/2"
      "full_scale: 32768"

Parameters:
  <-  double AFgetInfoFullScale
      Full scale value determined from the information record.  This is set to
      AF_FULLSCALE_UNDEF if the record was not found or a decoding error
      occurred.
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
static const char *RecID[] = AF_INFOID_FULLSCALE;


double
AFgetInfoFullScale (const struct AF_info *AFInfo)

{
  const char *p;
  int Nval;
  double Val[2], FullScale;

/* Search for named records in an AFsp information structure */
  p = AFgetInfoRec (RecID, AFInfo);

  FullScale = AF_FULLSCALE_UNDEF;
  if (p == NULL)        /* Record not found */
    return FullScale;

  /* Check the decoded values */
  Nval = AFdecInfoVVU (p, 'D', Val, NULL, NULL);
  if (Nval <= 0)
    UTwarn ("AFgetInfoFullScale - %s", AFM_Info_DecFullScale);
  else if (Val[0] <= 0 || (Nval == 2 && Val[1] <= 0))
    UTwarn ("AFgetInfoFullScale - %s", AFM_Info_InvFullScale);
  else if (Nval == 1)
    FullScale = Val[0];
  else if (Nval == 2)
    FullScale = Val[0] / Val[1];

  return FullScale;
}
