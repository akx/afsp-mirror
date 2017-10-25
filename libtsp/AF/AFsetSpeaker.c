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
            Name                    IEC 62574  AES 22.2 WAVE Ext
    "FL"    - Front Left                x        1/1       x
    "FR"    - Front Right               x        1/2       x
    "FC"    - Front Center              x        2/1       x
    "LFE1"  - Low Frequency Effects-1   x        2/2       x
    "BL"    - Back Left                 x        3/1       x
    "BR"    - Back Right                x        3/2       x
    "FLc"   - Front Left center         x        4/1       x
    "FRc"   - Front Right center        x        4/2       x
    "BC"    - Back Center               x        5/1       x
    "LFE2"  - Low Frequency Effects-2   x        5/2
    "SiL"   - Side Left                 x        6/1       x
    "SiR"   - Side Right                x        6/2       x
    "TpFL"  - Top Front Left            x        7/1       x
    "TpFR"  - Top Front Right           x        7/2       x
    "TpFC"  - Top Front Centre          x        8/1       x
    "TpC"   - Top Centre                x        8/2       x
    "TpBL"  - Top Back Left             x        9/1       x
    "TpBR"  - Top Back Right            x        9/2       x
    "TpSiL" - Top Side Left             x       10/1
    "TpSiR" - Top Side Right            x       10/2
    "TpBC"  - Top Back Centre           x       11/1       x
    "BtFC"  - Bottom Front Centre       x       11/2
    "BtFL"  - Bottom Front Left         x       12/1
    "BtFR"  - Bottom Front Right        x       12/2
    "FLw"   - Front Left wide           x
    "FRw"   - Front Right wide          x
    "LS"    - Left Surround             x
    "RS"    - Right Surround            x
    "LSd"   - Left Surround direct      x
    "RSd"   - Right Surround direct     x
    "TpLS"  - Top Left Surround         x
    "TpRS"  - Top Right Surround        x
    "-"     - No speaker assigned

  A speaker position can be associated with only one channel. For compatibility
  with previous usage, some synonyms are applicable. For instance
    "LF"   -> "LFE1"
    "SL"   -> "SiL", ...
    "TpFL" -> "TFL", ...

  Configuration Names:
  The following names can be used to set a speaker location configuration.
    "Stereo" : "FL FR"
    "Quad"   : "FL, FR, BL BR"
    "5.1"    : "FL, FR, FC, LFE1, BR, BL"
    "7.1"    : "FL, FC, FR, LFE1, BR, BL, SiL, SiR"

  WAVE extensible files:
  These files can store the speaker locations in the file header if certain
  constraints are in place.
  - The list of available speaker locations are:
    FL, FR, FC, LFE1, BL, BR FLc, FRc, BC, SiL, SiR, TpC, TpFL, TpFC, TpFR,
    TpBR, TpBC, TpBR
  - Assume M speaker locations are specified. These must be associated with the
    first M audio channels.
  - The speaker locations must be in the same order as the list, but specific
    locations can be skipped.
  - As an example, the order "FL FR TpC" is valid, but "FL FC FR" is not.

  AIFF-C files:
  Speaker locations are implicit in AIFC-C audio files.
  - Stereo: "FL FR"
  - 3 channel: "FL FR FC"
  - Quadraphonic" "FR FR BL BR"
  - 4 channel surround: "FL FC FR BC"
  - 6 channel surround: "FL FLc FC FRc FR BC"

  AU, AIFF, AIFF-C, and WAVE (non-extensible):
  Speaker locations appear as an information record of the form,
    "loudspeakers: FL FR LFE1"
  This speaker location record is one of the standard information records which
  appear in the file header.

Parameters:
  <-  int AFsetSpeaker
      Error flag, zero for no error
   -> const char String[]
      String containing the comma and/or white-space separated list of speaker
      positions

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.18 $  $Date: 2017/06/12 19:55:26 $

-------------------------------------------------------------------------*/

#include <string.h>

#include <libtsp.h>
#include <AFpar.h>
#include <libtsp/AFinfo.h>


int
AFsetSpeaker (const char String[])

{
  unsigned char SpkrConfig[AF_MAXN_SPKR+1];
  int Err, Nspkr;

 /* Set the parameters */
  Err = AFdecSpeaker (String, SpkrConfig, AF_MAXN_SPKR);
  if (! Err)
    Nspkr = (int) strlen ((const char *) SpkrConfig);
  else
    Nspkr = 0;

  UTfree (AFopt.SpkrConfig);
  AFopt.SpkrConfig = NULL;
  if (! Err && Nspkr > 0) {
    AFopt.SpkrConfig = (unsigned char *) UTmalloc (Nspkr + 1);
    STcopyMax ((const char *) SpkrConfig, (char *) AFopt.SpkrConfig, Nspkr);
  }

  return Err;
}
