/*------------ Telecommunications & Signal Processing Lab --------------
                         McGill University

Routine:
  FIpar.h

Description:
  Declarations for TSP filter coefficient files

Author / revision:
  P. Kabal  Copyright (C) 2015
  $Revision: 1.9 $  $Date: 2015/03/18 14:38:59 $

----------------------------------------------------------------------*/

#ifndef FIpar_h_
#define FIpar_h_

/* Filter types */
enum {
  FI_UNDEF  = 0,  /* undefined filter file identifier */
  FI_FIR    = 1,  /* FIR filter, direct form */
  FI_IIR    = 2,  /* IIR filter, cascade of biquad sections */
  FI_ALL    = 3,  /* All-pole filter, direct form */
  FI_WIN    = 4,  /* Window coefficients, direct form */
  FI_CAS    = 5   /* Cascade analog biquad sections */
};

#endif  /* FIpar_h_ */
