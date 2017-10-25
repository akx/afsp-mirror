/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  struct AF_NbS AFgetInfoNbS (const struct AF_info *AFInfo)

Purpose:
  Find and decode an information record specifying the number of bits used to
  represent each sample

Description:
  This routine searches the information record structure for a record specifying
  the number of bits per sample.

  Bits/sample information record ID:
      "bits/sample:", "bits_per_sample:", "sample_sig_bits:", "A/D:"
  The information following the identifier consists of one or two integer
  values formatted as NbS/Res or NbS alone.  NbS is the number of significant
  bits in each sample and Res is the number of bits in the word used to store
  those bits.  If NbS appears alone, Res is set to the smallest multiple of
  8-bits greater than or equal to NbS.

  By convention, the container size Res is the smallest multiple of 8 bits
  that will contain the significant bits.  Also by convention, the NbS bits
  are stored in the most significant positions of the Res bit word.  The
  record can optionally specify the units "bit*s", "b", or "Bit*s". The
  asterisk marks the shortest substring that must match.  Examples are
      "bits/sample: 20/24 bits"
      "A/D: 16 b"
      "bits/sample: 12 / 16"

Parameters:
  <-  struct AF_NbS AFgetInfoNbS
      The structure contains the two integer values NbS is the number of
      significant bits per sample and Res is the container size in bits.  Both
      values are set to zero if the record was not found or a decoding error
      occurred; otherwise both are positive.
   -> const struct AF_info *AFInfo
      AFsp information structure

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.8 $  $Date: 2017/07/03 22:25:14 $

-------------------------------------------------------------------------*/

#include <libtsp.h>
#include <libtsp/AFinfo.h>
#include <libtsp/AFmsg.h>

#define ICEILV(n,m) (((n) + ((m) - 1)) / (m)) /* int n,m >= 0 */
#define RNDUPV(n,m) ((m) * ICEILV (n, m))     /* Round up */

AF_NBS_INIT(AFNbS_init);  /* Define the AF_NbS defaults */

/* ID's and Units */
static const char *RecID[] = AF_INFOID_NBS;
static const char *UnitsTable[] = AF_INFOUNIT_NBS;


struct AF_NbS
AFgetInfoNbS (const struct AF_info *AFInfo)

{
  const char *p;
  struct AF_NbS AFNbS;
  int Nval;
  int Val[2];

  AFNbS = AFNbS_init;

  /* Search for named records in an AFsp information structure */
  p = AFgetInfoRec (RecID, AFInfo);
  if (p != NULL) {

    /* Decode the values */
    Nval = AFdecInfoVVU (p, 'I', Val, UnitsTable, NULL);
    if (Nval <= 0) {
      UTwarn ("AFgetInfoNbS - %s", AFM_Info_DecNbS);
      return AFNbS;
    }
    else if (Nval == 1)
      Val[1] = RNDUPV (Val[0], 8);    /* Res set to a multiple of 8 bits */

    /* Check the values */
    if (Val[0] > Val[1] || Val[0] < 0 || Val[1] > AF_RES_MAX)
      UTwarn ("AFgetInfoNbS - %s", AFM_Info_InvNbS);
    else {
      AFNbS.NbS = Val[0];
      AFNbS.Res = Val[1];
    }

  }

  return AFNbS;
}
