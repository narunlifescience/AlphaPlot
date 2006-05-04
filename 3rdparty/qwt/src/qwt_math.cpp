/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

// vim: expandtab

#include "qwt_math.h"

/*!
  \brief Find the smallest value in an array
  \param array Pointer to an array
  \param size Array size
*/
double qwtGetMin(double *array, int size)
{
    if (size <= 0)
       return 0.0;

    double rv = array[0];
    for (int i = 1; i < size; i++)
       rv = qwtMin(rv, array[i]);

    return rv;
}


/*!
  \brief Find the largest value in an array
  \param array Pointer to an array
  \param size Array size
*/
double qwtGetMax(double *array, int size)
{
    if (size <= 0)
       return 0.0;
    
    double rv = array[0];
    for (int i = 1; i < size; i++)
       rv = qwtMax(rv, array[i]);

    return rv;
}


/*!
  \brief Find the smallest value out of {1,2,5}*10^n with an integer number n
  which is greater than or equal to x

  \param x Input value
*/
double qwtCeil125( double x)
{
    if (x == 0.0) 
        return 0.0;

    const double sign = (x > 0) ? 1.0 : -1.0;
    const double lx = log10(fabs(x));
    const double p10 = floor(lx);

    double fr = pow(10.0, lx - p10);
    if (fr <=1.0)
       fr = 1.0;
    else if (fr <= 2.0)
       fr = 2.0;
    else if (fr <= 5.0)
       fr = 5.0;
    else
       fr = 10.0;

    return sign * fr * pow(10.0, p10);
} 


/*!
  \brief Find the largest value out of {1,2,5}*10^n with an integer number n
  which is smaller than or equal to x
  
  \param x Input value
*/
double qwtFloor125( double x)
{
    if (x == 0.0) 
        return 0.0;
    
    double sign = (x > 0) ? 1.0 : -1.0;
    const double lx = log10(fabs(x));
    const double p10 = floor(lx);

    double fr = pow(10.0, lx - p10);
    if (fr >= 10.0)
       fr = 10.0;
    else if (fr >= 5.0)
       fr = 5.0;
    else if (fr >= 2.0)
       fr = 2.0;
    else
       fr = 1.0;

    return sign * fr * pow(10.0, p10);
} 


/*!
  \brief  Checks if an array is a strictly monotonic sequence
  \param array Pointer to an array 
  \param size Size of the array
  \return
  <dl>
  <dt>0<dd>sequence is not strictly monotonic
  <dt>1<dd>sequence is strictly monotonically increasing
  <dt>-1<dd>sequence is strictly monotonically decreasing
  </dl>
*/
int qwtChkMono(double *array, int size)
{
    if (size < 2) 
        return 0;
    
    int rv = qwtSign(array[1] - array[0]);
    for (int i = 1; i < size - 1; i++)
    {
        if ( qwtSign(array[i+1] - array[i]) != rv )
        {
            rv = 0;
            break;
        }
    }
    return rv;
}

/*!
  \brief Invert the order of array elements
  \param array Pointer to an array 
  \param size Size of the array
*/
void qwtTwistArray(double *array, int size)
{
    const int s2 = size / 2;
    
    for (int i=0; i < s2; i++)
    {
        const int itmp = size - 1 - i;
        const double dtmp = array[i];
        array[i] = array[itmp];
        array[itmp] = dtmp;
    }
}

/*!
  \brief Create an array of equally spaced values
  \param array Where to put the values
  \param size Size of the array
  \param xmin Value associated with index 0
  \param xmax Value associated with index (size-1)
*/
void qwtLinSpace(double *array, int size, double xmin, double xmax)
{
    if (size <= 0)
        return;

    const int imax = size -1;

    array[0] = xmin;
    array[imax] = xmax;

    const double step = (xmax - xmin) / double(imax);
    const double tiny = 1e-6;

    for (int i = 1; i < imax; i++)
    {
       array[i] = xmin + double(i) * step;
       if (fabs(array[i]) < tiny*fabs(step))
          array[i] = step*floor(array[i]/step + tiny/2);
    }
}

/*!
  \brief Create an array of logarithmically equally spaced values
  \param array  Where to put the values
  \param size   Size of the array
  \param xmin Value associated with index 0
  \param xmax   Value associated with index (size-1)
*/
void qwtLogSpace(double *array, int size, double xmin, double xmax)
{
    if ((xmin <= 0.0) || (xmax <= 0.0) || (size <= 0))
       return;

    const int imax = size -1;

    array[0] = xmin;
    array[imax] = xmax;

    const double lxmin = log(xmin);
    const double lxmax = log(xmax);
    const double lstep = (lxmax - lxmin) / double(imax);

    for (int i = 1; i < imax; i++)
       array[i] = exp(lxmin + double(i) * lstep);
}
