/*------------- Telecommunications & Signal Processing Lab -------------
                           McGill University

Routine:
  void PQWOME (const double f[], double W2[], int N)
  void PQenThresh (const double f[], double Et[], int N)
  void PQexIndex (const double f[], double s[], int N);
  void PQintNoise (const double f[], double EIN[], int N)
  void PQtConst (double t100, double tmin, double Fs, const double f[],
	         double a[], double b[], int N)

Purpose:
  Table generation routines PQevalAudio

Description:
  These routines generate tables used in PQevalAudio.
    PQWOME - Outer and middle ear weighting (squared) at given frequencies
    PQenThresh - Energy thresholds at given frequencies
    PQexIndex - Excitation index at given frequencies
    PQintNoise - Internal noise energy at given frequencies
    PQtConst - Generate coefficients for frequency dependent time constants

Parameters:
  PQWOME
   -> const double f[]
      Array of N frequency values
  <-  double W2[]
      Array of N squared weight values
   -> int N
      Number of array elements
  PQenThresh
   -> const double f[]
      Array of N frequency values
  <-  double Et[]
      Array of N energy theshold values
   -> int N
      Number of array elements
  PQexIndex
   -> const double f[]
      Array of N frequency values
  <-  double s[]
      Array of N excitation index (energy) values
   -> int N
      Number of array elements
  PQintNoise
   -> const double f[]
      Array of N frequency values
  <-  double EIN[]
      Array of N internal noise energies
   -> int N
      Number of array elements
  PQtConst
   -> double t100
      Time constant at 100 Hz (s)
   -> double tmin
      Minimum time constant (s)
   -> double Fs
      Sampling frequency (Hz)
   -> const double f[]
      Frequency array
  <-  double a[]
      Difference equation coefficients (N values)
  <-  double b[]
      Difference equation coefficients, b[k] = (1 - a[k]). b can be NULL.
   -> int N
      Number of frequency values and coefficient values

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.9 $  $Date: 2003/05/13 01:12:06 $

----------------------------------------------------------------------*/

#include <math.h>

#include "PQevalAudio.h"

#define DBPI(x)		(pow (10., x / 10.))	/* Inverse dB (power) */
#define SQRV(x)		((x) * (x))
#define PI		3.14159265358979323846

/*-------------------------------*/
/* Outer & middle ear weighting */

void
PQWOME (const double f[], double W2[], int N)

{
  int k;
  double fkHz, AdB;

  for (k = 0; k < N; ++k) {
    if (f[k] != 0) {
      fkHz = f[k] / 1000.;
      AdB = -2.184 * pow (fkHz, -0.8) + 6.5 * exp(-0.6 * SQRV (fkHz - 3.3))
	    -0.001 * pow (fkHz, 3.6);
      W2[k] = DBPI (AdB);
    }
    else {
      W2[k] = 0;
    }
  }

  return;
}

/* Energy threshold */

void
PQenThresh (const double f[], double Et[], int N)

{
  int m;
  double EtdB;

  for (m = 0; m < N; ++m) {
    EtdB = 3.64 * pow (f[m] / 1000., -0.8);
    Et[m] = DBPI (EtdB);
  }

  return;
}

/* Excitation index */

void
PQexIndex (const double f[], double s[], int N)

{
  int m;
  double sdB;

  for (m = 0; m < N; ++m) {
    sdB = -2 - 2.05 * atan (f[m]/4000.) - 0.75 * atan (SQRV(f[m]/1600.));
    s[m] = DBPI (sdB);
  }

  return;
}

/* Internal noise */

void
PQintNoise (const double f[], double EIN[], int N)

{
  int m;
  double INdB;

  for (m = 0; m < N; ++m) {
    INdB = 1.456 * pow (f[m] / 1000., -0.8);
    EIN[m] = DBPI (INdB);
  }

  return;
}

/* Frequency dependent time constants */

void
PQtConst (double t100, double tmin, double Fs, const double f[],
	  double a[], double b[], int N)

{
  int m;
  double t;

  for (m = 0; m < N; ++m) {
    t = tmin + (100 / f[m]) * (t100 - tmin);
    a[m] = exp (-1. / (Fs * t));
    if (b != NULL)
      b[m] = 1. - a[m];
  }

  return;
}
