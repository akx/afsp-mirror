/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void AOsetFIopt (const struct AO_FIpar *FI, int NsampND, int RAccess)

Purpose:
  Set input file options

Description:
  This routine sets the input file options in the audio file options structure
  from the input file parameter structure.

  Calling this routine as
    AOsetFIopt (FI, 0, 0)
  will leave NsampND and RAccess at their default settings.

Parameters:
  <-  void AOsetFIopt
   -> const struct AO_FIpar *FI
      Input file parameters
   -> int NsampND
      Flag to determine if number of samples must be known
      0 - Nsamp must be known
      1 - Nsamp=AF_NSAMP_UNDEF allowed
   -> int RAccess
      Flag to determine if the file must be random access
      0 - File need not be random access, but an error will occur when an
          attempt is made to reposition the file
      1 - File must be random access

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.9 $  $Date: 2017/06/13 12:28:53 $

-------------------------------------------------------------------------*/

#include <AO.h>


void
AOsetFIopt (const struct AO_FIpar *FI, int NsampND, int RAccess)

{
  AFopt.NsampND = NsampND;
  AFopt.RAccess = RAccess;
  AFopt.FtypeI = FI->Ftype;
  AFopt.InputPar = FI->InputPar;

  return;
}
