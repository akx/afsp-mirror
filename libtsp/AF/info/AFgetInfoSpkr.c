/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  unsigned char *AFgetInfoSpkr (const struct AF_info *AFInfo)

Purpose:
  Find and decode an information record specifying loudspeaker locations

Description:
  This routine searches the information record structure for a record specifying
  loudspeaker configuration.

  Bits/sample information record ID:
      "loudspeakers:""

  The information following is a comma or whitespace separated list of the
  loudspeaker locations.  See AFsetSpeaker for a list of available speaker
  location names.  Examples are
      "loudspeakers: FL FR"
      "loudspeakers: FL FR FC"

Parameters:
  <-  unsigned char *AFgetInfoSpkr
      Loudspeaker configuration - This is a null terminated list of loudspeaker
      position codes as unsigned bytes.
   -> const struct AF_info *AFInfo
      AFsp information structure

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.2 $  $Date: 2017/07/03 22:25:14 $

-------------------------------------------------------------------------*/

#include <libtsp.h>
#include <AFpar.h>
#include <libtsp/AFinfo.h>

/* Loudspeakers keyword */
static const char *RecID[] = AF_INFOID_SPKR;


unsigned char *
AFgetInfoSpkr (const struct AF_info *AFInfo)

{
  const char *p;
  static unsigned char SpkrConfig[AF_MAXN_SPKR+1];

/* Search for named records in an AFsp information structure */
  p = AFgetInfoRec (RecID, AFInfo);

  SpkrConfig[0] = '\0';
  if (p == NULL)
    return SpkrConfig;

/* Convert the string to loudspeaker position codes */
  AFdecSpeaker (p, SpkrConfig, AF_MAXN_SPKR);

  return SpkrConfig;
}
