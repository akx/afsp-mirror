/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  double AFgetInfoSfreq (const struct AF_info *AFInfo)

Purpose:
  Find and decode an information record specifying a sampling frequency

Description:
  This routine searches the information record structure for a record specifying
  the sampling frequency.

  Bits/sample information record ID:
      "sample_rate:" "sampling rate:"
  The information following the identifier consists of a float value or ratio
  of float values and an optional units specification ("Hz" or "kHz"). Examples
  are
      "sample_rate: 20E3 / 3 Hz"
      "sampling rate: 20000/3"
      "sample_rate: 16.6667 kHz"

Parameters:
  <-  double AFgetInfoSfreq
      Sampling rate determined from the information record. This is set to
      AF_SFREQ_UNDEF if the record was not found or a decoding error occurred.
   -> const struct AF_info *AFInfo
      AFsp information structure

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.6 $  $Date: 2017/07/03 22:25:14 $

-------------------------------------------------------------------------*/

#include <libtsp.h>
#include <libtsp/nucleus.h>
#include <AFpar.h>
#include <libtsp/AFinfo.h>
#include <libtsp/AFmsg.h>

/* Sampling rate keywords */
static const char *RecID[] = AF_INFOID_SFREQ;
static const char *UnitsTable[] = AF_INFOUNIT_SFREQ;
static const double UnitValues[] = AF_INFOSCALE_SFREQ;


double
AFgetInfoSfreq (const struct AF_info *AFInfo)

{
  const char *p;
  int Key, Nval;
  double Val[2], Sfreq;


/* Search for named records in an AFsp information structure */
  p = AFgetInfoRec (RecID, AFInfo);

  Sfreq = AF_SFREQ_UNDEF;
  if (p == NULL)        /* Sample rate record not found */
    return Sfreq;

  /* Decode the value */
  Nval = AFdecInfoVVU (p, 'D', Val, UnitsTable, &Key);
  if (Nval <= 0)
    UTwarn ("AFgetInfoSfreq - %s", AFM_Info_DecSfreq);
  else if (Val[0] <= 0 || (Nval == 2 && Val[1] <= 0))
    UTwarn ("AFgetInfoSfreq - %s", AFM_Info_InvSfreq);
  else if (Nval == 1)
    Sfreq = UnitValues[Key] * Val[0];
  else if (Nval == 2)
    Sfreq = UnitValues[Key] * (Val[0] / Val[1]);

  return Sfreq;
}
