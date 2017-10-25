/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  enum UT_DS_T UTswapCode (enum UT_DS_T Dbo)

Purpose:
  Determine a swap code for decoding data

Description:
  This routine determines a swap code given a data byte ordering code.  Given
  the swap code for the data, the returned swap code is determined from the
  host byte order.

Parameters:
  <-  enum UT_DS_T UTswapCode
      Swap code, DS_NATIVE or DS_SWAP
   -> enum UT_DS_T Dbo
      Data byte swap code.
      DS_EB     - Data is in big-endian byte order.  The output swap code
                  will be DS_SWAP if the current host uses little-endian
                  byte order.
      DS_EL     - Data is in little-endian byte order data.  The output swap
                  code will be DS_SWAP if the current host uses big-endian
                  byte order.
      DS_NATIVE - Data is in native byte order
      DS_SWAP   - Data is byte-swapped

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.8 $  $Date: 2017/03/28 00:13:00 $

-------------------------------------------------------------------------*/

#include <libtsp.h>
#include <libtsp/nucleus.h>
#include <libtsp/UTpar.h>
#include <libtsp/UTmsg.h>


enum UT_DS_T
UTswapCode (enum UT_DS_T Dbo)

{
  static enum UT_DS_T Hbo = DS_UNDEF;

  switch (Dbo) {
  case DS_EB:
  case DS_EL:
    if (Hbo == DS_UNDEF)  /* Cache the host byte order */
      Hbo = UTbyteOrder ();
    if (Hbo == Dbo)
      Dbo = DS_NATIVE;
    else
      Dbo = DS_SWAP;
    break;
  case DS_SWAP:
  case DS_NATIVE:
    break;
  default:
    UThalt ("UTswapCode: %s", UTM_BadSwap);
  }

  return Dbo;
}
