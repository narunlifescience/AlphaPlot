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
#include "qwt.h"

/*!
  Calculate the interpolated function value corresponding 
  to a given argument x.
*/
double QwtSpline::value(double x) const
{
    if (!d_a)
        return 0.0;

    const int i = lookup(x);

    const double delta = x - d_x[i];
    return( ( ( ( d_a[i] * delta) + d_b[i] ) 
        * delta + d_c[i] ) * delta + d_y[i] );
}

//! CTOR
QwtSpline::QwtSpline()
{
    d_a = d_b = d_c = 0;
    d_xbuffer = d_ybuffer = d_x = d_y = 0;
    d_size = 0;
    d_buffered = 0;
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
void QwtSpline::copyValues(int tf)
{
    cleanup();
    d_buffered = tf;
}

//! DTOR
QwtSpline::~QwtSpline()
{
    cleanup();
}

//! Determine the function table index corresponding to a value x
int QwtSpline::lookup(double x) const
{
    int i1, i2, i3;
    
    if (x <= d_x[0])
       i1 = 0;
    else if (x >= d_x[d_size - 2])
       i1 = d_size -2;
    else
    {
        i1 = 0;
        i2 = d_size -2;
        i3 = 0;

        while ( i2 - i1 > 1 )
        {
            i3 = i1 + ((i2 - i1) >> 1);

            if (d_x[i3] > x)
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
  \param periodic if non-zero, calculate periodic spline
  \return <dl>
          <dt>0<dd>successful
      <dt>Qwt::ErrMono<dd>Sequence of x values is not strictly monotone
                          increasing
          <dt>Qwt::ErrNoMem<dd>Couldn't allocate memory
      </dl>
  \warning The sequence of x (but not y) values has to be strictly monotone
           increasing, which means <code>x[0] < x[1] < .... < x[n-1]</code>.
       If this is not the case, the function will return an error.
*/
int QwtSpline::recalc(double *x, double *y, int n, int periodic)
{
    int i, rv = 0;

    cleanup();

    if (n > 2)
    {
        d_size = n;

        if (d_buffered)
        {
            d_xbuffer = new double[n-1];
            d_ybuffer = new double[n-1];

            if ((!d_xbuffer) || (!d_ybuffer))
            {
                cleanup();
                return Qwt::ErrNoMem;
            }
            else
            {
                for (i=0;i<n;i++)
                {
                    d_xbuffer[i] = x[i];
                    d_ybuffer[i] = y[i];
                }
                d_x = d_xbuffer;
                d_y = d_ybuffer;
            }
        }
        else
        {
            d_x = x;
            d_y = y;
        }
        
        d_a = new double[n-1];
        d_b = new double[n-1];
        d_c = new double[n-1];

        if ( (!d_a) || (!d_b) || (!d_c) )
        {
            cleanup();
            return Qwt::ErrMono;
        }

        if(periodic)
           rv =  buildPerSpline();
        else
           rv =  buildNatSpline();

        if (rv) cleanup();
    }

    return rv;
}

/*!
  \brief re-calculate the spline coefficients

  Depending on the value of \a periodic, this function
  will determine the coefficients for a natural or a periodic
  spline and store them internally. It also buffers the
  values of x and y, which are needed for the
  interpolation (See QwtSpline::value()).  
  \param x QwtArray<double> of points 
  \param y QwtArray<double> of points
  \param periodic if non-zero, calculate periodic spline
  \return <dl>
          <dt>0<dd>successful
      <dt>Qwt::ErrMono<dd>Sequence of x values is not strictly monotone
                          increasing
          <dt>Qwt::ErrNoMem<dd>Couldn't allocate memory
      </dl>
  \warning The sequence of x (but not y) values has to be strictly monotone
           increasing, which means <code>x[0] < x[1] < .... < x[n-1]</code>.
       If this is not the case, the function will return an error.
*/
int QwtSpline::recalc(const QwtArray<double> &x, const QwtArray<double> &y,
    int periodic)
{
    int n = QMIN(x.size(), y.size());
    d_buffered = TRUE;

    return recalc(x.data(), y.data(), n, periodic);
}

/*!
  \brief Determines the coefficients for a natural spline
  \return <dl>
          <dt>0<dd>successful
      <dt>Qwt::ErrMono<dd>Sequence of x values is not strictly monotone
                          increasing
          <dt>Qwt::ErrNoMem<dd>Couldn't allocate memory
      </dl>
*/
int QwtSpline::buildNatSpline()
{
    int i;
    double dy1, dy2;
    
    double *d = new double[d_size-1];
    double *h = new double[d_size-1];
    double *s = new double[d_size];

    if ( (!d) || (!h) || (!s) )
    {
        cleanup();
        if (h) delete[] h;
        if (s) delete[] s;
        if (d) delete[] d;
        return Qwt::ErrNoMem;
    }

    //
    //  set up tridiagonal equation system; use coefficient
    //  vectors as temporary buffers
    for (i=0; i<d_size - 1; i++) 
    {
        h[i] = d_x[i+1] - d_x[i];
        if (h[i] <= 0)
        {
            delete[] h;
            delete[] s;
            delete[] d;
            return Qwt::ErrMono;
        }
    }
    
    dy1 = (d_y[1] - d_y[0]) / h[0];
    for (i = 1; i < d_size - 1; i++)
    {
        d_b[i] = d_c[i] = h[i];
        d_a[i] = 2.0 * (h[i-1] + h[i]);

        dy2 = (d_y[i+1] - d_y[i]) / h[i];
        d[i] = 6.0 * ( dy1 - dy2);
        dy1 = dy2;
    }

    //
    // solve it
    //
    
    // L-U Factorization
    for(i = 1; i < d_size - 2;i++)
    {
        d_c[i] /= d_a[i];
        d_a[i+1] -= d_b[i] * d_c[i]; 
    }

    // forward elimination
    s[1] = d[1];
    for(i=2;i<d_size - 1;i++)
       s[i] = d[i] - d_c[i-1] * s[i-1];
    
    // backward elimination
    s[d_size - 2] = - s[d_size - 2] / d_a[d_size - 2];
    for (i= d_size -3; i > 0; i--)
       s[i] = - (s[i] + d_b[i] * s[i+1]) / d_a[i];

    //
    // Finally, determine the spline coefficients
    //
    s[d_size - 1] = s[0] = 0.0;
    for (i = 0; i < d_size - 1; i++)
    {
        d_a[i] = ( s[i+1] - s[i] ) / ( 6.0 * h[i]);
        d_b[i] = 0.5 * s[i];
        d_c[i] = ( d_y[i+1] - d_y[i] ) 
            / h[i] - (s[i+1] + 2.0 * s[i] ) * h[i] / 6.0; 
    }

    delete[] d;
    delete[] s;
    delete[] h;

    return 0;
    
}

/*!
  \brief Determines the coefficients for a periodic spline
  \return <dl>
          <dt>0<dd>successful
      <dt>Qwt::ErrMono<dd>Sequence of x values is not strictly monotone
                          increasing
          <dt>Qwt::ErrNoMem<dd>Couldn't allocate memory
      </dl>
*/
int QwtSpline::buildPerSpline()
{
    int i,imax;
    double sum;
    double dy1, dy2,htmp;
    
    double *d = new double[d_size-1];
    double *h = new double[d_size-1];
    double *s = new double[d_size];
    
    if ( (!d) || (!h) || (!s) )
    {
        cleanup();
        if (h) delete[] h;
        if (s) delete[] s;
        if (d) delete[] d;
        return Qwt::ErrNoMem;
    }

    //
    //  setup equation system; use coefficient
    //  vectors as temporary buffers
    //
    for (i=0; i<d_size - 1; i++)
    {
        h[i] = d_x[i+1] - d_x[i];
        if (h[i] <= 0.0)
        {
            delete[] h;
            delete[] s;
            delete[] d;
            return Qwt::ErrMono;
        }
    }
    
    imax = d_size - 2;
    htmp = h[imax];
    dy1 = (d_y[0] - d_y[imax]) / htmp;
    for (i=0; i <= imax; i++)
    {
        d_b[i] = d_c[i] = h[i];
        d_a[i] = 2.0 * (htmp + h[i]);
        dy2 = (d_y[i+1] - d_y[i]) / h[i];
        d[i] = 6.0 * ( dy1 - dy2);
        dy1 = dy2;
        htmp = h[i];
    }

    //
    // solve it
    //
    
    // L-U Factorization
    d_a[0] = sqrt(d_a[0]);
    d_c[0] = h[imax] / d_a[0];
    sum = 0;

    for(i=0;i<imax-1;i++)
    {
        d_b[i] /= d_a[i];
        if (i > 0)
           d_c[i] = - d_c[i-1] * d_b[i-1] / d_a[i];
        d_a[i+1] = sqrt( d_a[i+1] - qwtSqr(d_b[i]));
        sum += qwtSqr(d_c[i]);
    }
    d_b[imax-1] = (d_b[imax-1] - d_c[imax-2] * d_b[imax-2]) / d_a[imax-1];
    d_a[imax] = sqrt(d_a[imax] - qwtSqr(d_b[imax-1]) - sum);
    

    // forward elimination
    s[0] = d[0] / d_a[0];
    sum = 0;
    for(i=1;i<imax;i++)
    {
        s[i] = (d[i] - d_b[i-1] * s[i-1]) / d_a[i];
        sum += d_c[i-1] * s[i-1];
    }
    s[imax] = (d[imax] - d_b[imax-1]*s[imax-1] - sum) / d_a[imax];
    
    
    // backward elimination
    s[imax] = - s[imax] / d_a[imax];
    s[imax-1] = -(s[imax-1] + d_b[imax-1] * s[imax]) / d_a[imax-1];
    for (i= imax - 2; i >= 0; i--)
       s[i] = - (s[i] + d_b[i] * s[i+1] + d_c[i] * s[imax]) / d_a[i];

    //
    // Finally, determine the spline coefficients
    //
    s[d_size-1] = s[0];
    for (i=0;i<d_size-1;i++)
    {
        d_a[i] = ( s[i+1] - s[i] ) / ( 6.0 * h[i]);
        d_b[i] = 0.5 * s[i];
        d_c[i] = ( d_y[i+1] - d_y[i] ) 
            / h[i] - (s[i+1] + 2.0 * s[i] ) * h[i] / 6.0; 
    }

    delete[] d;
    delete[] s;
    delete[] h;

    return 0;
}


//! Free allocated memory and set size to 0
void QwtSpline::cleanup()
{
    if (d_a) delete[] d_a;
    if (d_b) delete[] d_b;
    if (d_c) delete[] d_c;
    if (d_xbuffer) delete[] d_xbuffer;
    if (d_ybuffer) delete[] d_ybuffer;
    d_a = d_b = d_c = 0;
    d_xbuffer = d_ybuffer = d_x = d_y = 0;
    d_size = 0;
}
