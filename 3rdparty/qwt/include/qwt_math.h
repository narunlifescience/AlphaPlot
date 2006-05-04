/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

/*!
  \file qwt_math.h
  \brief A set of mathematical routines
*/

#ifndef QWT_MATH_H
#define QWT_MATH_H

#include <math.h>
#include <qpoint.h>
#include "qwt_global.h"


#ifndef LOG10_2
#define LOG10_2     0.30102999566398119802  /* log10(2) */
#endif

#ifndef LOG10_3
#define LOG10_3     0.47712125471966243540  /* log10(3) */
#endif

#ifndef LOG10_5
#define LOG10_5     0.69897000433601885749  /* log10(5) */
#endif

#ifndef M_2PI
#define M_2PI       6.28318530717958623200  /* 2 pi */
#endif

#ifndef LOG_MIN
//! Mininum value for logarithmic scales
#define LOG_MIN 1.0e-100
#endif

#ifndef LOG_MAX
//! Maximum value for logarithmic scales
#define LOG_MAX 1.0e100
#endif

#ifndef M_E
#define M_E            2.7182818284590452354   /* e */
#endif

#ifndef M_LOG2E
#define M_LOG2E 1.4426950408889634074   /* log_2 e */
#endif

#ifndef M_LOG2E
#define M_LOG10E    0.43429448190325182765  /* log_10 e */
#endif

#ifndef M_LN2
#define M_LN2       0.69314718055994530942  /* log_e 2 */
#endif

#ifndef M_LN10
#define M_LN10         2.30258509299404568402  /* log_e 10 */
#endif

#ifndef M_PI
#define M_PI        3.14159265358979323846  /* pi */
#endif

#ifndef M_PI_2
#define M_PI_2      1.57079632679489661923  /* pi/2 */
#endif

#ifndef M_PI_4
#define M_PI_4      0.78539816339744830962  /* pi/4 */
#endif

#ifndef M_1_PI
#define M_1_PI      0.31830988618379067154  /* 1/pi */
#endif

#ifndef M_2_PI
#define M_2_PI      0.63661977236758134308  /* 2/pi */
#endif

#ifndef M_2_SQRTPI
#define M_2_SQRTPI  1.12837916709551257390  /* 2/sqrt(pi) */
#endif

#ifndef M_SQRT2
#define M_SQRT2 1.41421356237309504880  /* sqrt(2) */
#endif

#ifndef M_SQRT1_2
#define M_SQRT1_2   0.70710678118654752440  /* 1/sqrt(2) */
#endif

QWT_EXPORT double qwtCeil125(double x);
QWT_EXPORT double qwtFloor125(double x);
QWT_EXPORT double qwtGetMin (double *array, int size);
QWT_EXPORT double qwtGetMax( double *array, int size);
QWT_EXPORT void qwtTwistArray(double *array, int size);
QWT_EXPORT int qwtChkMono(double *array, int size);
QWT_EXPORT void qwtLinSpace(double *array, int size, double xmin, double xmax);
QWT_EXPORT void qwtLogSpace(double *array, int size, double xmin, double xmax);


#define qwtMax QMAX
#define qwtMin QMIN
#define qwtAbs QABS
#define qwtInt qRound

//! Return the sign 
template <class T>
inline int qwtSign(const T& x)
{
    if (x > T(0))
       return 1;
    else if (x < T(0))
       return (-1);
    else
       return 0;
}            

//! Return the square of a number
template <class T>
inline T qwtSqr(const T&x)
{
    return x*x;
}

/*!
  \brief Copy an array into another
  \param dest Destination
  \param src Source
  \param n Number of elements  
*/
template <class T>
void qwtCopyArray(T *dest, T *src, int n) 
{
    int i;
    for (i=0; i<n;i++ )
       dest[i] = src[i];
}

/*!
  \brief Shift an array by a specified number of positions 
  \param arr Pointer to an array
  \param size Number of elements
  \param di Number of positions. A positive number shifts right,  
        a negative number shifts left.
*/
template <class T>
void qwtShiftArray(T *arr, int size, int di)
{
    int i, delta;
    T* buffer = 0;
    
    delta = qwtAbs(di);

    if ((delta > 0) && (delta < size))
    {
    if ((buffer = new T[delta]))
    {
        if (di < 0)         // shift left
        {
        qwtCopyArray(buffer, arr, delta);
        qwtCopyArray(&arr[0], &arr[delta], size - delta);
        qwtCopyArray(&arr[size - delta], buffer, delta);
        
        }
        else            // shift right
        {
        qwtCopyArray(buffer, &arr[size - delta], delta);
        for ( i = size-delta-1; i >= 0; i-- )
           arr[i + delta] = arr[i];
        qwtCopyArray(arr, buffer, delta);
        }
    }
    }
    
    if (buffer != 0) delete[] buffer;
}

//! Swap two values 
template <class T>
void qwtSwap( T &x1, T& x2)
{
    T tmp;
    tmp = x1;
    x1 = x2;
    x2 = tmp;
}


/*!
  \brief Sort two values in ascending order
  \param x1 First input value
  \param x2 Second input value
  \param xmax Greater value
  \param xmin Smaller value
*/
template <class T>
void qwtSort(const T& x1, const T& x2, T& xmin, T& xmax)
{
    T buffer;
    
    if (x2 < x1)
    {
    buffer = x1;
    xmin = x2;
    xmax = buffer;
    }
    else
    {
    xmin = x1;
    xmax = x2;
    }
}

//! Sort two values in ascending order 
template <class T>
void qwtSort(T& x1, T& x2)
{
    T buffer;
    
    if (x2 < x1)
    {
    buffer = x1;
    x1 = x2;
    x2 = buffer;
    }
}

/*!
  \brief Limit a value to fit into a specified interval
  \param x Input value
  \param x1 First interval boundary
  \param x2 Second interval boundary  
*/
template <class T>
T qwtLim(const T& x, const T& x1, const T& x2)
{
    T rv;
    T xmin, xmax;
    
    xmin = qwtMin(x1, x2);
    xmax = qwtMax(x1, x2);

    if ( x < xmin )
       rv = xmin;
    else if ( x > xmax )
       rv = xmax;
    else
       rv = x;

    return rv;
}

inline QPoint qwtPolar2Pos(const QPoint &center,
    double radius, double angle)
{
    const double x = center.x() + radius * cos(angle);
    const double y = center.y() - radius * sin(angle);

    return QPoint(qRound(x), qRound(y));
}

inline QPoint qwtDegree2Pos(const QPoint &center,
    double radius, double angle)
{
    return qwtPolar2Pos(center, radius, angle / 180.0 * M_PI);
}

#endif
