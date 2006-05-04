/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#include "qwt_dimap.h"

QT_STATIC_CONST_IMPL double QwtDiMap::LogMin = 1.0e-150;
QT_STATIC_CONST_IMPL double QwtDiMap::LogMax = 1.0e150;

/*!
  \brief Constructor

  The double and integer intervals are both set to [0,1].
*/
QwtDiMap::QwtDiMap():
    d_x1(0.0),
    d_x2(1.0),
    d_y1(0),
    d_y2(1),
    d_cnv(1.0),
    d_log(FALSE)
{
}


/*!
  \brief Constructor

  Constructs a QwtDiMap instance with initial integer
  and double intervals

  \param i1 first border of integer interval
  \param i2 second border of integer interval
  \param d1 first border of double interval
  \param d2 second border of double interval
  \param logarithmic logarithmic mapping, TRUE or FALSE.
*/ 
QwtDiMap::QwtDiMap(int i1, int i2, double d1, double d2, bool logarithmic)
{
    // setIntRange(i1, i2)  calls newFactor() with uninitialized memory
    d_y1 = i1;
    d_y2 = i2;
    setDblRange(d1, d2, logarithmic);
}

/*!
  \brief Destructor
*/
QwtDiMap::~QwtDiMap()
{
}

/*!
  \return TRUE if a value x lies inside or at the border of the
  map's double range.
  \param x value
*/
bool QwtDiMap::contains(double x) const
{
    return ( (x >= QMIN(d_x1, d_x2)) && (x <= QMAX(d_x1, d_x2)));
}

/*!
  \return TRUE if a value x lies inside or at the border of the
    map's integer range
  \param x value
*/
bool QwtDiMap::contains(int x) const
{
    return ( (x >= QMIN(d_y1, d_y2)) && (x <= QMAX(d_y1, d_y2)));
}

/*!
  \brief Specify the borders of the double interval
  \param d1 first border
  \param d2 second border 
  \param lg logarithmic (TRUE) or linear (FALSE) scaling
*/
void QwtDiMap::setDblRange(double d1, double d2, bool lg)
{
    if (lg)
    {
        d_log = TRUE;
        if (d1 < LogMin) 
           d1 = LogMin;
        else if (d1 > LogMax) 
           d1 = LogMax;
        
        if (d2 < LogMin) 
           d2 = LogMin;
        else if (d2 > LogMax) 
           d2 = LogMax;
        
        d_x1 = log(d1);
        d_x2 = log(d2);
    }
    else
    {
        d_log = FALSE;
        d_x1 = d1;
        d_x2 = d2;
    }
    newFactor();
}

/*!
  \brief Specify the borders of the integer interval
  \param i1 first border
  \param i2 second border
*/
void QwtDiMap::setIntRange(int i1, int i2)
{
    d_y1 = i1;
    d_y2 = i2;
    newFactor();
}

/*!
  \brief Transform an integer value into a double value
  \param y integer value to be transformed
  \return
  <dl>
  <dt>linear mapping:<dd>d1 + (d2 - d1) / (i2 - i1) * (y - i1)
  <dt>logarithmic mapping:<dd>d1 + (d2 - d1) / log(i2 / i1) * log(y / i1)
  </dl>
*/
double QwtDiMap::invTransform(int y) const 
{
    if (d_cnv == 0.0)
       return 0.0;
    else
    {
        if(d_log) 
           return exp(d_x1 + double(y - d_y1) / d_cnv );
        else
           return ( d_x1 + double(y - d_y1) / d_cnv );  
    }
}

/*! 
  \brief  Transform and limit

  The function is similar to QwtDiMap::transform, but limits the input value
  to the nearest border of the map's double interval if it lies outside
  that interval.

  \param x value to be transformed
  \return transformed value
*/
int QwtDiMap::limTransform(double x) const
{
    if (d_log) {
    if (x > LogMax)
        x = LogMax;
    else if (x < LogMin)
        x = LogMin;
    x = log(x);
    }

    if ( x > qwtMax(d_x1, d_x2) )
    x = qwtMax(d_x1, d_x2);
    else if ( x < qwtMin(d_x1, d_x2))
    x = qwtMin(d_x1, d_x2);

    return d_log ? transform(exp(x)) : transform(x);
}

/*!
  \brief Exact transformation
 
  This function is similar to QwtDiMap::transform, but
  makes the integer interval appear to be double. 
  \param x value to be transformed
  \return 
  <dl>
  <dt>linear mapping:<dd>i1 + (i2 - i1) / (d2 - d1) * (x - d1)
  <dt>logarithmic mapping:<dd>i1 + (i2 - i1) / log(d2 / d1) * log(x / d1)
  </dl>
*/
double QwtDiMap::xTransform(double x) const
{
    double rv;
    
    if (d_log)
       rv = double(d_y1) + (log(x) - d_x1) * d_cnv;    
    else
       rv = double(d_y1) + (x - d_x1) * d_cnv;

    return rv;
}


/*!
  \brief Re-calculate the conversion factor.
*/
void QwtDiMap::newFactor()
{
    if (d_x2 != d_x1)
       d_cnv = double(d_y2 - d_y1) / (d_x2 - d_x1); 
    else 
       d_cnv = 0.0;
}
