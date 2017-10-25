/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int AFdRdAlaw (AFILE *AFp, double Dbuff[], int Nreq)
  int AFdRdMulaw (AFILE *AFp, double Dbuff[], int Nreq)
  int AFdRdMulawR (AFILE *AFp, double Dbuff[], int Nreq)

Purpose:
  Read 8-bit A-law data from an audio file (return double values)
  Read 8-bit mu-law data from an audio file (return double values)
  Read bit-reversed 8-bit mu-law data from an audio file (return double values)

Description:
  These routines read a specified number of 8-bit A-law, mu-law, or bit-reversed
  mu-law samples from an audio file.  The data in the file is converted to
  double format on output.

Parameters:
  <-  int AFdRdAlaw
      Number of data values transferred from the file.  On reaching the end of
      the file, this value may be less than Nreq.
   -> AFILE *AFp
      Audio file pointer for an audio file opened by AFopnRead
  <-  double Dbuff[]
      Array of doubles to receive the samples
   -> int Nreq
      Number of samples requested.  Nreq may be zero.

  <-  int AFdRdMulaw
      Number of data values transferred from the file.  On reaching the end of
      the file, this value may be less than Nreq.
   -> AFILE *AFp
      Audio file pointer for an audio file opened by AFopnRead
  <-  double Dbuff[]
      Array of doubles to receive the samples
   -> int Nreq
      Number of samples requested.  Nreq may be zero.

  <-  int AFdRdMulawR
      Number of data values transferred from the file.  On reaching the end of
      the file, this value may be less than Nreq.
   -> AFILE *AFp
      Audio file pointer for an audio file opened by AFopnRead
  <-  double Dbuff[]
      Array of doubles to receive the samples
   -> int Nreq
      Number of samples requested.  Nreq may be zero.

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.11 $  $Date: 2017/09/07 16:41:32 $

-------------------------------------------------------------------------*/

#include <AFpar.h>
#include <libtsp/AFdataio.h>
#include <libtsp/UTtypes.h>

#define LW1   FDL_ALAW8     /* Also FDL_MULAW8 and FDL_MULAWR8 */
/* NBBUF defined in AFdataio.h */

/* ITU-T Recommendation G.711
   A-law data is stored in sign-magnitude alternate-bit-complemented format.
   The sign bit is 1 for positive data.  After complementing bits, the byte is
   in sign-segment-mantissa format.
   A-law data   complement  segment step-size       value
   0 000 xxxx   0 101 xyxy     5       32     -1008, ... , -528
   0 001 xxxx   0 100 xyxy     4       16      -504, ... , -264
   0 010 xxxx   0 111 xyxy     7      128     -4032, ... , -2112
   0 011 xxxx   0 110 xyxy     6       64     -2016, ... , -1056
   0 100 xxxx   0 001 xyxy     1        2       -63, ... , -33
   0 101 xxxx   0 000 xyxy     1        2       -31, ... , -1
   0 110 xxxx   0 011 xyxy     3        8      -252, ... , -132
   0 111 xxxx   0 010 xyxy     2        4      -126, ... , -66
   1 000 xxxx   1 101 xyxy     5       32       528, ... , 1008
   1 001 xxxx   1 100 xyxy     4       16       264, ... , 504
   1 010 xxxx   1 111 xyxy     7      128      2112, ... , 4032
   1 011 xxxx   1 110 xyxy     6       64      1056, ... , 2016
   1 100 xxxx   1 001 xyxy     1        2        33, ... , 63
   1 101 xxxx   1 000 xyxy     1        2         1, ... , 31
   1 110 xxxx   1 011 xyxy     3        8       132, ... , 252
   1 111 xxxx   1 010 xyxy     2        4        66, ... , 126

   Mu-law data is stored in sign-magnitude bit-complemented format.  The sign
   bit is 0 for positive data.  After complementing the bits, the byte is in
   sign-segment-mantissa format.
   mu-law data  complement  segment step-size       value
   0 000 xxxx   1 111 yyyy     7      256     -8031, ... , -4191
   0 001 xxxx   1 110 yyyy     6      128     -3999, ... , -2079
   0 010 xxxx   1 101 yyyy     5       64     -1983, ... , -1023
   0 011 xxxx   1 100 yyyy     4       32      -975, ... , -495
   0 100 xxxx   1 011 yyyy     3       16      -471, ... , -231
   0 101 xxxx   1 010 yyyy     2        8      -219, ... , -99
   0 110 xxxx   1 001 yyyy     1        4       -93, ... , -33
   0 111 xxxx   1 000 yyyy     0        2       -30, ... ,  0
   1 000 xxxx   0 111 yyyy     7      256      8031, ... , 4191
   1 001 xxxx   0 110 yyyy     6      128      3999, ... , 2079
   1 010 xxxx   0 101 yyyy     5       64      1983, ... , 1023
   1 011 xxxx   0 100 yyyy     4       32       975, ... , 495
   1 100 xxxx   0 011 yyyy     3       16       471, ... , 231
   1 101 xxxx   0 010 yyyy     2        8       219, ... , 99
   1 110 xxxx   0 001 yyyy     1        4        93, ... , 33
   1 111 xxxx   0 000 yyyy     0        2        30, ... , 0

   Mu-law with a bit-reversed code is the same as standard mu-law, but with the
   code words bit reversed.  For instance code 0 100 1101 when bit reversed
   becomes 1011 001 0, with the sign-bit now in the least significant position
   in the byte.
 */

/* The decoding of A-law, mu-law, and bit-reversed mu-law use a table lookup.
   This table maps an 8-bit code to an output value.  The table takes care of
   complementing bits and bit-reversal as appropriate to the type of input code.
   The source code for the different cases differs only in which table is used.
*/

int
AFdRdAlaw (AFILE *AFp, double Dbuff[], int Nreq)

{
  int is, N, i, Nr;
  UT_uint1_t Buf[NBBUF/LW1];

  /*
  The following table implements the conversion from A-law, with the output
  data scaled by 8, including the necessary bit complementing operation.  The
  output values range from -32256 to +32256.
  */
  const double Atab[256] = {
     -5504,  -5248,  -6016,  -5760,  -4480,  -4224,  -4992,  -4736,
     -7552,  -7296,  -8064,  -7808,  -6528,  -6272,  -7040,  -6784,
     -2752,  -2624,  -3008,  -2880,  -2240,  -2112,  -2496,  -2368,
     -3776,  -3648,  -4032,  -3904,  -3264,  -3136,  -3520,  -3392,
    -22016, -20992, -24064, -23040, -17920, -16896, -19968, -18944,
    -30208, -29184, -32256, -31232, -26112, -25088, -28160, -27136,
    -11008, -10496, -12032, -11520,  -8960,  -8448,  -9984,  -9472,
    -15104, -14592, -16128, -15616, -13056, -12544, -14080, -13568,
      -344,   -328,   -376,   -360,   -280,   -264,   -312,   -296,
      -472,   -456,   -504,   -488,   -408,   -392,   -440,   -424,
       -88,    -72,   -120,   -104,    -24,     -8,    -56,    -40,
      -216,   -200,   -248,   -232,   -152,   -136,   -184,   -168,
     -1376,  -1312,  -1504,  -1440,  -1120,  -1056,  -1248,  -1184,
     -1888,  -1824,  -2016,  -1952,  -1632,  -1568,  -1760,  -1696,
      -688,   -656,   -752,   -720,   -560,   -528,   -624,   -592,
      -944,   -912,  -1008,   -976,   -816,   -784,   -880,   -848,
      5504,   5248,   6016,   5760,   4480,   4224,   4992,   4736,
      7552,   7296,   8064,   7808,   6528,   6272,   7040,   6784,
      2752,   2624,   3008,   2880,   2240,   2112,   2496,   2368,
      3776,   3648,   4032,   3904,   3264,   3136,   3520,   3392,
     22016,  20992,  24064,  23040,  17920,  16896,  19968,  18944,
     30208,  29184,  32256,  31232,  26112,  25088,  28160,  27136,
     11008,  10496,  12032,  11520,   8960,   8448,   9984,   9472,
     15104,  14592,  16128,  15616,  13056,  12544,  14080,  13568,
       344,    328,    376,    360,    280,    264,    312,    296,
       472,    456,    504,    488,    408,    392,    440,    424,
        88,     72,    120,    104,     24,      8,     56,     40,
       216,    200,    248,    232,    152,    136,    184,    168,
      1376,   1312,   1504,   1440,   1120,   1056,   1248,   1184,
      1888,   1824,   2016,   1952,   1632,   1568,   1760,   1696,
       688,    656,    752,    720,    560,    528,    624,    592,
       944,    912,   1008,    976,    816,    784,    880,    848
  };


  for (is = 0; is < Nreq; ) {

    /* Read data from the audio file */
    N = MINV (NBBUF / LW1, Nreq - is);
    Nr = FREAD (Buf, LW1, N, AFp->fp);

    /* Scale */
    for (i = 0; i < Nr; ++i, ++is)
      Dbuff[is] = AFp->ScaleF * Atab[Buf[i]];

    if (Nr < N)
      break;
  }

  return is;
}

int
AFdRdMulaw (AFILE *AFp, double Dbuff[], int Nreq)

{
  int is, N, i, Nr;
  UT_uint1_t Buf[NBBUF/LW1];

  /*
  The following table implements the conversion from mu-law, with the output
  data scaled by 4 and including the necessary bit complementing operation.  The
  output values range from -32124 to +32124.
  */
  const double Mutab[256] = {
    -32124, -31100, -30076, -29052, -28028, -27004, -25980, -24956,
    -23932, -22908, -21884, -20860, -19836, -18812, -17788, -16764,
    -15996, -15484, -14972, -14460, -13948, -13436, -12924, -12412,
    -11900, -11388, -10876, -10364,  -9852,  -9340,  -8828,  -8316,
     -7932,  -7676,  -7420,  -7164,  -6908,  -6652,  -6396,  -6140,
     -5884,  -5628,  -5372,  -5116,  -4860,  -4604,  -4348,  -4092,
     -3900,  -3772,  -3644,  -3516,  -3388,  -3260,  -3132,  -3004,
     -2876,  -2748,  -2620,  -2492,  -2364,  -2236,  -2108,  -1980,
     -1884,  -1820,  -1756,  -1692,  -1628,  -1564,  -1500,  -1436,
     -1372,  -1308,  -1244,  -1180,  -1116,  -1052,   -988,   -924,
      -876,   -844,   -812,   -780,   -748,   -716,   -684,   -652,
      -620,   -588,   -556,   -524,   -492,   -460,   -428,   -396,
      -372,   -356,   -340,   -324,   -308,   -292,   -276,   -260,
      -244,   -228,   -212,   -196,   -180,   -164,   -148,   -132,
      -120,   -112,   -104,    -96,    -88,    -80,    -72,    -64,
       -56,    -48,    -40,    -32,    -24,    -16,     -8,      0,
     32124,  31100,  30076,  29052,  28028,  27004,  25980,  24956,
     23932,  22908,  21884,  20860,  19836,  18812,  17788,  16764,
     15996,  15484,  14972,  14460,  13948,  13436,  12924,  12412,
     11900,  11388,  10876,  10364,   9852,   9340,   8828,   8316,
      7932,   7676,   7420,   7164,   6908,   6652,   6396,   6140,
      5884,   5628,   5372,   5116,   4860,   4604,   4348,   4092,
      3900,   3772,   3644,   3516,   3388,   3260,   3132,   3004,
      2876,   2748,   2620,   2492,   2364,   2236,   2108,   1980,
      1884,   1820,   1756,   1692,   1628,   1564,   1500,   1436,
      1372,   1308,   1244,   1180,   1116,   1052,    988,    924,
       876,    844,    812,    780,    748,    716,    684,    652,
       620,    588,    556,    524,    492,    460,    428,    396,
       372,    356,    340,    324,    308,    292,    276,    260,
       244,    228,    212,    196,    180,    164,    148,    132,
       120,    112,    104,     96,     88,     80,     72,     64,
        56,     48,     40,     32,     24,     16,      8,      0
  };

  for (is = 0; is < Nreq; ) {

    /* Read data from the audio file */
    N = MINV (NBBUF / LW1, Nreq - is);
    Nr = FREAD (Buf, LW1, N, AFp->fp);

    /* Convert and scale */
    for (i = 0; i < Nr; ++i, ++is)
      Dbuff[is] = AFp->ScaleF * Mutab[Buf[i]];

    if (Nr < N)
      break;
  }

  return is;
}

int
AFdRdMulawR (AFILE *AFp, double Dbuff[], int Nreq)

{
  int is, N, i, Nr;
  UT_uint1_t Buf[NBBUF/LW1];

  /*
  The following table implements the conversion from my-law, with the output
  data scaled by 4 and including the necessary bit reversal and bit
  complementing operations.  The output values range from -32124 to +32124.
  */
  const double MuRtab[256] = {
    -32124,  32124,  -1884,   1884,  -7932,   7932,   -372,    372,
    -15996,  15996,   -876,    876,  -3900,   3900,   -120,    120,
    -23932,  23932,  -1372,   1372,  -5884,   5884,   -244,    244,
    -11900,  11900,   -620,    620,  -2876,   2876,    -56,     56,
    -28028,  28028,  -1628,   1628,  -6908,   6908,   -308,    308,
    -13948,  13948,   -748,    748,  -3388,   3388,    -88,     88,
    -19836,  19836,  -1116,   1116,  -4860,   4860,   -180,    180,
     -9852,   9852,   -492,    492,  -2364,   2364,    -24,     24,
    -30076,  30076,  -1756,   1756,  -7420,   7420,   -340,    340,
    -14972,  14972,   -812,    812,  -3644,   3644,   -104,    104,
    -21884,  21884,  -1244,   1244,  -5372,   5372,   -212,    212,
    -10876,  10876,   -556,    556,  -2620,   2620,    -40,     40,
    -25980,  25980,  -1500,   1500,  -6396,   6396,   -276,    276,
    -12924,  12924,   -684,    684,  -3132,   3132,    -72,     72,
    -17788,  17788,   -988,    988,  -4348,   4348,   -148,    148,
     -8828,   8828,   -428,    428,  -2108,   2108,     -8,      8,
    -31100,  31100,  -1820,   1820,  -7676,   7676,   -356,    356,
    -15484,  15484,   -844,    844,  -3772,   3772,   -112,    112,
    -22908,  22908,  -1308,   1308,  -5628,   5628,   -228,    228,
    -11388,  11388,   -588,    588,  -2748,   2748,    -48,     48,
    -27004,  27004,  -1564,   1564,  -6652,   6652,   -292,    292,
    -13436,  13436,   -716,    716,  -3260,   3260,    -80,     80,
    -18812,  18812,  -1052,   1052,  -4604,   4604,   -164,    164,
     -9340,   9340,   -460,    460,  -2236,   2236,    -16,     16,
    -29052,  29052,  -1692,   1692,  -7164,   7164,   -324,    324,
    -14460,  14460,   -780,    780,  -3516,   3516,    -96,     96,
    -20860,  20860,  -1180,   1180,  -5116,   5116,   -196,    196,
    -10364,  10364,   -524,    524,  -2492,   2492,    -32,     32,
    -24956,  24956,  -1436,   1436,  -6140,   6140,   -260,    260,
    -12412,  12412,   -652,    652,  -3004,   3004,    -64,     64,
    -16764,  16764,   -924,    924,  -4092,   4092,   -132,    132,
     -8316,   8316,   -396,    396,  -1980,   1980,     -0,      0
  };

  for (is = 0; is < Nreq; ) {

    /* Read data from the audio file */
    N = MINV (NBBUF / LW1, Nreq - is);
    Nr = FREAD (Buf, LW1, N, AFp->fp);

    /* Convert and scale */
    for (i = 0; i < Nr; ++i, ++is)
      Dbuff[is] = AFp->ScaleF * MuRtab[Buf[i]];

    if (Nr < N)
      break;
  }

  return is;
}
