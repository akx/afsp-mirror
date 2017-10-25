/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  enum UT_DS_T AFgetInfoSwap(const struct AF_info *AFInfo)

Purpose:
  Find and decode an information record specifying the data byte order

Description:
  This routine searches the information record structure for a record specifying
  the data byte order

  Bits/sample information record ID:
     "byte_order:" "sample_byte_order:"
  The information following the identifier keywords. The keywords are
     "n*ative", "N*ative", "1" => Do not swap bytes
     "s*wap",                  => Swap data bytes
     "l*ittle-endian", "01"    => File data is in little-endian byte order
     "b*ig-endian", "10"       => File data is in big-endian byte order
  The asterisk marks the point at which the keyword can be shortened.

Parameters:
  <-  enum UT_DS_T AFgetInfoSwap
      Byte order code:
       DS_UNDEF  - Record not found, or invalid byte order keyword
       DS_NATIVE - Do not swap bytes
       DS_SWAP   - Swap data bytes
       DS_EL     - File data is in little-endian byte order
       DS_EB     - FIle data is in big-endian byte order
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
#include <libtsp/UTpar.h>

/* Sampling rate keywords */
static const char *RecID[] = AF_INFOID_SWAP;
static const char *KeyTab[] = AF_INFOKEY_SWAP;
static enum UT_DS_T SwapVal[] = AF_INFOVAL_SWAP;


enum UT_DS_T
AFgetInfoSwap(const struct AF_info *AFInfo)
{
  const char *p;
  enum UT_DS_T SwapCode;
  int n;

/* Search for named records in an AFsp information structure */
  p = AFgetInfoRec (RecID, AFInfo);

  SwapCode = DS_UNDEF;
  if (p == NULL)        /* Record not found */
    return SwapCode;

  n = STkeyMatch (p, KeyTab);
  if (n < 0) {
    UTwarn ("AFgetInfoSwap - %s: \"%.16s\"", AFM_BadSwap, p);
    return SwapCode;
  }
  SwapCode = SwapVal[n];

  return SwapCode;
}
