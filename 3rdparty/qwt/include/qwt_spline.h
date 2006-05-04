/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#ifndef QWT_SPLINE_H
#define QWT_SPLINE_H

#include "qwt_array.h"
#include "qwt_global.h"

/*!
  \brief A class for spline interpolation

  The QwtSpline class is used for cubical spline interpolation.
  Two types of splines, natural and periodic, are supported.
  
  \par Usage:
  <ol>
  <li>First call QwtSpline::recalc() to determine the spline coefficients 
      for a tabulated function y(x).
  <li>After the coefficients have been set up, the interpolated
      function value for an argument x can be determined by calling 
      QwtSpline::value().
  </ol>
  In order to save storage space, QwtSpline can be advised
  not to buffer the contents of x and y.
  This means that the arrays have to remain valid and unchanged
  for the interpolation to work properly. This can be achieved
  by calling QwtSpline::copyValues().

  \par Example:
  \code
#include<qwt_spline.h>
#include<iostream.h>

QwtSpline s;
double x[30], y[30], xInter[300], yInter[300];
int i;

for(i=0;i<30;i++)               // fill up x[] and y[]
cin >> x[i] >> y[i];

if (s.recalc(x,y,30,0) == 0)    // build natural spline
{
   for(i=0;i<300;i++)          // interpolate
   {
     xInter[i] = x[0] + double(i) * (x[29] - x[0]) / 299.0;
     yInter[i] = s.value( xInter[i] );
   }

   do_something(xInter, yInter, 300);
}
else
  cerr << "Uhhh...\n";
  \endcode
*/

class QWT_EXPORT QwtSpline
{
public:
    QwtSpline();
    ~QwtSpline();

    double value(double x) const;
    int recalc(double *x, double *y, int n, int periodic = 0);
    int recalc(const QwtArray<double> &x, const QwtArray<double> &y,
        int periodic = 0);
    void copyValues(int tf = 1);

private:
    int buildPerSpline();
    int buildNatSpline();
    int lookup(double x) const;
    void cleanup();

    // coefficient vectors
    double *d_a;
    double *d_b;
    double *d_c;
    double *d_d;

    // values
    double *d_x;
    double *d_y;
    double *d_xbuffer;
    double *d_ybuffer;
    int d_size;

    //flags
    int d_buffered;
};





#endif





