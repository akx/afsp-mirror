/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  enum UT_DS_T UTbyteCode (enum UT_DS_T Dbo)

Purpose:
  Determine a byte order code

Description:
  This routine determines a byte order code given a byte swap code.  Given the
  swap code, the returned swap code is determined from the host byte order.

Parameters:
  <-  enum UT_DS_T UTbyteCode
      Byte order code, DS_EB or DS_EL
   -> enum UT_DS_T Dbo
      Data byte swap code.
      DS_EB     - Data is in big-endian byte order
      DS_EL     - Data is in little-endian byte order data
      DS_NATIVE - Data is in native byte order.  The output byte order code will
                  be DS_EB if the current host uses big-endian byte order and
                  DS_EL if the current host uses little-endian byte order.
      DS_SWAP   - Data is byte-swapped.  The output byte order code will be
                  DS_EL if the current host uses big-endian byte order and DS_EB
                  if the current host uses little-endian byte order.

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.6 $  $Date: 2017/09/22 00:33:17 $

-------------------------------------------------------------------------*/

#include <libtsp.h>
#include <libtsp/nucleus.h>
#include <libtsp/UTpar.h>
#include <libtsp/UTmsg.h>


enum UT_DS_T
UTbyteCode (enum UT_DS_T Dbo)

{
  static enum UT_DS_T Hbo = DS_UNDEF;

  switch (Dbo) {
  case DS_EB:
  case DS_EL:
    break;
  case DS_NATIVE:
    if (Hbo == DS_UNDEF)  /* Cache the host byte order */
      Hbo = UTbyteOrder ();
    Dbo = Hbo;
    break;
  case DS_SWAP:
    if (Hbo == DS_UNDEF)  /* Cache the host byte order */
      Hbo = UTbyteOrder ();
    if (Hbo == DS_EB)
      Dbo = DS_EL;
    else
      Dbo = DS_EB;
    break;
  default:
    UThalt ("UTbyteCode: %s", UTM_BadSwap);
  }

  return Dbo;
}
