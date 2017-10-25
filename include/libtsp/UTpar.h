/*------------ Telecommunications & Signal Processing Lab --------------
                         McGill University

Routine:
  UTpar.h

Description:
  Declarations for the libtsp utility routines.

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.8 $  $Date: 2017/05/24 16:23:29 $

----------------------------------------------------------------------*/

#ifndef UTpar_h_
#define UTpar_h_

/* Machine and data byte order codes */
enum UT_DS_T {
  DS_UNDEF  = -1,  /* undefined */
  DS_EB     =  0,  /* big-endian */
  DS_EL     =  1,  /* little-endian */
  DS_NATIVE =  2,  /* native */
  DS_SWAP   =  3   /* byte-swapped */
};

#endif  /* UTpar_h_ */
