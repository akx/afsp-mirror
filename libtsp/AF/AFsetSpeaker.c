/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int AFsetSpeaker (const char String[])

Purpose:
  Set loudspeaker spatial positions

Description:
  This routine sets the mapping of output channels to speaker positions.  The
  spatial positions of the loudspeakers are specified as a list of comma and/or
  white-space separated locations from the list below.
    "FL"  - Front Left
    "FR"  - Front Right
    "FC"  - Front Center
    "LF"  - Low Frequency
    "BL"  - Back Left
    "BR"  - Back Right
    "FLC" - Front Left of Center
    "FRC" - Front Right of Center
    "BC"  - Back Center
    "SL"  - Side Left
    "SR"  - Side Right
    "TC"  - Top Center
    "TFL" - Top Front Left
    "TFC" - Top Front Center
    "TFR" - Top Front Right
    "TBL" - Top Back Lefty
    "TBC" - Top Back Center
    "TBR" - Top Back Right
    "-"   - none  (No associated speaker position)
  A speaker position can be associated with only one channel.  Only WAVE files
  store the speaker locations in the file header.  In the case of WAVE files,
  the subset of spatial positions associated with the channels must appear in
  the order given above.

Parameters:
  <-  int AFsetSpeaker
      Error flag, zero for no error
   -> const char String[]
      String containing the list of speaker positions

Author / revision:
  P. Kabal  Copyright (C) 2009
  $Revision: 1.5 $  $Date: 2009/03/23 12:02:34 $

-------------------------------------------------------------------------*/

#include <string.h>

#include <libtsp.h>
#include <libtsp/AFheader.h>
#include <libtsp/AFpar.h>

int
AFsetSpeaker(const char String[])

{
  unsigned char SpkrConfig[AF_MAXN_SPKR+1];
  int ErrCode, Nspkr;
  struct AF_opt *AFopt;

/* Set the parameters */
  ErrCode = AFdecSpeaker (String, SpkrConfig, AF_MAXN_SPKR);
  Nspkr = strlen ((const char *) SpkrConfig);

  AFopt = AFoptions ();
  UTfree (AFopt->SpkrConfig);
  AFopt->SpkrConfig = NULL;
  if (! ErrCode && Nspkr > 0) {
    AFopt->SpkrConfig = (unsigned char *) UTmalloc (Nspkr + 1);
    STcopyMax ((const char *) SpkrConfig, (char *) AFopt->SpkrConfig, Nspkr);
  }

  return ErrCode;
}
