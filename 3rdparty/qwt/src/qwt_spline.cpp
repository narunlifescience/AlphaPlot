/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#include "qwt_spline.h"
#include "qwt_math.h"

class QwtSpline::PrivateData
{
public:
    PrivateData():
        size(0),
        buffered(false)
    {
        a = b = c = NULL;
        xbuffer = ybuffer = x = y = NULL;
    }

    // coefficient vectors
    double *a;
    double *b;
    double *c;
    double *d;

    // values
    double *x;
    double *y;
    double *xbuffer;
    double *ybuffer;
    int size;

    //flags
    bool buffered;
};

//! CTOR
QwtSpline::QwtSpline()
{
    d_data = new PrivateData;
}

//! DTOR
QwtSpline::~QwtSpline()
{
    cleanup();
    delete d_data;
}


/*!
  \brief Advise recalc() to buffer the tabulated function or switch off
         internal buffering

  By default, QwtSpline maintains an internal copy of the
  tabulated function given as *x and *y arguments
  of QwtSpline::recalc(). 

  If QwtSpline::copyValues() is called with zero argument,
  subsequent calls to QwtSpline::recalc() will not buffer these values
  anymore and just store the pointers instead. The QwtSpline::value()
  function will then silently assume that these pointers remained valid
  and that the contents of the arrays have not been changed since
  the last QwtSpline::recalc().

  If called with no or nonzero argument,
  any following QwtSpline::recalc() calls will use the internal buffer. 

  \param tf if nonzero, the function table will be buffered
  \warning copyValues() resets all the contents of QwtSpline.
           A subsequent recalc() will be necessary.
*/
void QwtSpline::copyValues(bool tf)
{
    cleanup();
    d_data->buffered = tf;
}

/*!
  Calculate the interpolated function value corresponding 
  to a given argument x.
*/
double QwtSpline::value(double x) const
{
    if (!d_data->a)
        return 0.0;

    const int i = lookup(x);

    const double delta = x - d_data->x[i];
    return( ( ( ( d_data->a[i] * delta) + d_data->b[i] ) 
        * delta + d_data->c[i] ) * delta + d_data->y[i] );
}

//! Determine the function table index corresponding to a value x
int QwtSpline::lookup(double x) const
{
    int i1, i2, i3;
    
    if (x <= d_data->x[0])
       i1 = 0;
    else if (x >= d_data->x[d_data->size - 2])
       i1 = d_data->size -2;
    else
    {
        i1 = 0;
        i2 = d_data->size -2;
        i3 = 0;

        while ( i2 - i1 > 1 )
        {
            i3 = i1 + ((i2 - i1) >> 1);

            if (d_data->x[i3] > x)
               i2 = i3;
            else
               i1 = i3;

        }
    }
    return i1;
}


/*!
  \brief re-calculate the spline coefficients

  Depending on the value of \a periodic, this function
  will determine the coefficients for a natural or a periodic
  spline and store them internally. By default, it also buffers the
  values of x and y, which are needed for the
  interpolation (See QwtSpline::value()). In order to save memory,
  this last behaviour may be changed with the QwtSpline::copyValues() function.
  
  \param x
  \param y points
  \param n number of points
  \param periodic if true, calculate periodic spline
  \return true if successful
  \warning The sequence of x (but not y) values has to be strictly monotone
           increasing, which means <code>x[0] < x[1] < .... < x[n-1]</code>.
       If this is not the case, the function will return false
*/
bool QwtSpline::recalc(double *x, double *y, int n, bool periodic)
{
    cleanup();

    if (n <= 2)
        return false;

    d_data->size = n;

    if (d_data->buffered)
    {
        d_data->xbuffer = new double[n];
        d_data->ybuffer = new double[n];

        for (int i = 0; i < n; i++)
        {
            d_data->xbuffer[i] = x[i];
            d_data->ybuffer[i] = y[i];
        }
        d_data->x = d_data->xbuffer;
        d_data->y = d_data->ybuffer;
    }
    else
    {
        d_data->x = x;
        d_data->y = y;
    }
    
    d_data->a = new double[n-1];
    d_data->b = new double[n-1];
    d_data->c = new double[n-1];

    bool ok;
    if(periodic)
       ok =  buildPerSpline();
    else
       ok =  buildNatSpline();

    if (!ok) 
        cleanup();

    return ok;
}

/*!
  \brief Determines the coefficients for a natural spline
  \return true if successful
*/
bool QwtSpline::buildNatSpline()
{
    int i;
    
    double *d = new double[d_data->size-1];
    double *h = new double[d_data->size-1];
    double *s = new double[d_data->size];

    //
    //  set up tridiagonal equation system; use coefficient
    //  vectors as temporary buffers
    for (i = 0; i < d_data->size - 1; i++) 
    {
        h[i] = d_data->x[i+1] - d_data->x[i];
        if (h[i] <= 0)
        {
            delete[] h;
            delete[] s;
            delete[] d;
            return false;
        }
    }
    
    double dy1 = (d_data->y[1] - d_data->y[0]) / h[0];
    for (i = 1; i < d_data->size - 1; i++)
    {
        d_data->b[i] = d_data->c[i] = h[i];
        d_data->a[i] = 2.0 * (h[i-1] + h[i]);

        const double dy2 = (d_data->y[i+1] - d_data->y[i]) / h[i];
        d[i] = 6.0 * ( dy1 - dy2);
        dy1 = dy2;
    }

    //
    // solve it
    //
    
    // L-U Factorization
    for(i = 1; i < d_data->size - 2;i++)
    {
        d_data->c[i] /= d_data->a[i];
        d_data->a[i+1] -= d_data->b[i] * d_data->c[i]; 
    }

    // forward elimination
    s[1] = d[1];
    for(i=2;i<d_data->size - 1;i++)
       s[i] = d[i] - d_data->c[i-1] * s[i-1];
    
    // backward elimination
    s[d_data->size - 2] = - s[d_data->size - 2] / d_data->a[d_data->size - 2];
    for (i= d_data->size -3; i > 0; i--)
       s[i] = - (s[i] + d_data->b[i] * s[i+1]) / d_data->a[i];

    //
    // Finally, determine the spline coefficients
    //
    s[d_data->size - 1] = s[0] = 0.0;
    for (i = 0; i < d_data->size - 1; i++)
    {
        d_data->a[i] = ( s[i+1] - s[i] ) / ( 6.0 * h[i]);
        d_data->b[i] = 0.5 * s[i];
        d_data->c[i] = ( d_data->y[i+1] - d_data->y[i] ) 
            / h[i] - (s[i+1] + 2.0 * s[i] ) * h[i] / 6.0; 
    }

    delete[] d;
    delete[] s;
    delete[] h;

    return true;
}

/*!
  \brief Determines the coefficients for a periodic spline
  \return true if successful
*/
bool QwtSpline::buildPerSpline()
{
    int i;
    
    double *d = new double[d_data->size-1];
    double *h = new double[d_data->size-1];
    double *s = new double[d_data->size];
    
    //
    //  setup equation system; use coefficient
    //  vectors as temporary buffers
    //
    for (i=0; i<d_data->size - 1; i++)
    {
        h[i] = d_data->x[i+1] - d_data->x[i];
        if (h[i] <= 0.0)
        {
            delete[] h;
            delete[] s;
            delete[] d;
            return false;
        }
    }
    
    const int imax = d_data->size - 2;
    double htmp = h[imax];
    double dy1 = (d_data->y[0] - d_data->y[imax]) / htmp;
    for (i=0; i <= imax; i++)
    {
        d_data->b[i] = d_data->c[i] = h[i];
        d_data->a[i] = 2.0 * (htmp + h[i]);
        const double dy2 = (d_data->y[i+1] - d_data->y[i]) / h[i];
        d[i] = 6.0 * ( dy1 - dy2);
        dy1 = dy2;
        htmp = h[i];
    }

    //
    // solve it
    //
    
    // L-U Factorization
    d_data->a[0] = sqrt(d_data->a[0]);
    d_data->c[0] = h[imax] / d_data->a[0];
    double sum = 0;

    for(i=0;i<imax-1;i++)
    {
        d_data->b[i] /= d_data->a[i];
        if (i > 0)
           d_data->c[i] = - d_data->c[i-1] * d_data->b[i-1] / d_data->a[i];
        d_data->a[i+1] = sqrt( d_data->a[i+1] - qwtSqr(d_data->b[i]));
        sum += qwtSqr(d_data->c[i]);
    }
    d_data->b[imax-1] = (d_data->b[imax-1] - d_data->c[imax-2] * d_data->b[imax-2]) / d_data->a[imax-1];
    d_data->a[imax] = sqrt(d_data->a[imax] - qwtSqr(d_data->b[imax-1]) - sum);
    

    // forward elimination
    s[0] = d[0] / d_data->a[0];
    sum = 0;
    for(i=1;i<imax;i++)
    {
        s[i] = (d[i] - d_data->b[i-1] * s[i-1]) / d_data->a[i];
        sum += d_data->c[i-1] * s[i-1];
    }
    s[imax] = (d[imax] - d_data->b[imax-1]*s[imax-1] - sum) / d_data->a[imax];
    
    
    // backward elimination
    s[imax] = - s[imax] / d_data->a[imax];
    s[imax-1] = -(s[imax-1] + d_data->b[imax-1] * s[imax]) / d_data->a[imax-1];
    for (i= imax - 2; i >= 0; i--)
       s[i] = - (s[i] + d_data->b[i] * s[i+1] + d_data->c[i] * s[imax]) / d_data->a[i];

    //
    // Finally, determine the spline coefficients
    //
    s[d_data->size-1] = s[0];
    for (i=0;i<d_data->size-1;i++)
    {
        d_data->a[i] = ( s[i+1] - s[i] ) / ( 6.0 * h[i]);
        d_data->b[i] = 0.5 * s[i];
        d_data->c[i] = ( d_data->y[i+1] - d_data->y[i] ) 
            / h[i] - (s[i+1] + 2.0 * s[i] ) * h[i] / 6.0; 
    }

    delete[] d;
    delete[] s;
    delete[] h;

    return true;
}


//! Free allocated memory and set size to 0
void QwtSpline::cleanup()
{
    delete[] d_data->a;
    delete[] d_data->b;
    delete[] d_data->c;
    delete[] d_data->xbuffer;
    delete[] d_data->ybuffer;

    d_data->a = d_data->b = d_data->c = NULL;
    d_data->xbuffer = d_data->ybuffer = d_data->x = d_data->y = NULL;
    d_data->size = 0;
}
