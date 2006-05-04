/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#include "qwt_math.h"
#include "qwt_autoscl.h"


static const double MinEps=1.0e-10;

//! Ctor
QwtAutoScale::QwtAutoScale ()
{
    d_autoScale = TRUE;
    d_scaleOpt = None;
    
    d_minValue = 0.0;
    d_maxValue = 0.0;
    d_scaleMin = 0.0;
    d_scaleMax = 0.0;
    d_loMargin = 0.0;
    d_hiMargin = 0.0;
    d_step = 0.0;
    d_maxMajor = 8;
    d_maxMinor = 5;
    d_reset = 1;
    d_autoRebuild = TRUE;
}


//! Dtor
QwtAutoScale::~QwtAutoScale ()
{
}

/*!
    \return \c TRUE if auto-scaling is active
    \sa QwtAutoScale::setAutoScale()
*/
bool QwtAutoScale::autoScale() const 
{   
    return d_autoScale; 
}

/*!
    \return the margin at the lower end of the scale
    \sa QwtAutoScale::setMargins()
*/
double QwtAutoScale::loMargin() const 
{ 
    return d_loMargin; 
}

/*!
    \return the margin at the upper end of the scale
    \sa QwtAutoScale::setMargins()
*/
double QwtAutoScale::hiMargin() const 
{ 
    return d_hiMargin; 
}

/*!
    \return the maximum number of major tickmarks
    \sa QwtAutoScale::setMaxMajor()
*/
int QwtAutoScale::maxMajor() const 
{ 
    return d_maxMajor; 
}

/*!
    \return the maximum number of minor scale ticks
    \sa QwtAutoScale::setMaxMinor()
*/
int QwtAutoScale::maxMinor() const 
{ 
    return d_maxMinor; 
}


/*!
  \brief Adjust the scale to include a given array of input values.

  This member function extends the boundaries of the scale
  and re-calculates the step size if necessary in order
  to include all values in the array. If the reset
  parameter has nonzero value, the previous state will
  be cleared.
  
  \param x Array of input values
  \param num Array size
  \param reset If != 0 reset the scale's contents
*/
void QwtAutoScale::adjust(double *x, int num, int reset)
{
    if (d_reset || reset)
       d_minValue = d_maxValue = x[0];

    for (int i = 0; i < num; i++)
    {
        if (x[i] > d_maxValue)
           d_maxValue = x[i];
        if (x[i] < d_minValue)
           d_minValue = x[i];
    }
    d_reset = 0;

    if (d_autoRebuild) 
        build();
}


/*!
  \brief Adjust the scale to include a given array of input values.

  This member function extends the boundaries of the scale
  and re-calculates the step size if necessary in order
  to include all values in the array. If the reset
  parameter has nonzero value, the previous state will
  be cleared.
  
  \param x QwtArray<double> of input values
  \param reset If != 0 reset the scale's contents
*/
void QwtAutoScale::adjust(const QwtArray<double> &x, int reset)
{
    adjust(x.data(), x.size(), reset);
}


/*!
  \brief Adjust the scale to include a specified interval

  This member function extends the boundaries of the scale
  and re-calculates the step size if necessary in order
  to include a specified interval. If the reset
  parameter has nonzero value, the previous state will
  be cleared.

  \param vmin lower border of the specified interval
  \param vmax upper border of the specified interval
  \param reset if nonzero, reset the scale. Defaults to 0.
*/
void QwtAutoScale::adjust(double vmin, double vmax, int reset)
{ 
    double mxv = qwtMax(vmin,vmax);
    double mnv = qwtMin(vmin,vmax);
    
    if (d_reset || reset)
    {
        d_minValue = mnv;
        d_maxValue = mxv;
    }
    else
    {
        if (d_minValue > mnv) 
           d_minValue = mnv;
        if (d_maxValue < mxv)
           d_maxValue = mxv;
    }
    d_reset = 0;

    if (d_autoRebuild) 
        build();
}

/*!
  \brief Re-build the scale
*/
void QwtAutoScale::build() 
{
    if (d_reset) 
        return;
    
    if (d_autoScale)
    {
        if (d_scaleOpt & Logarithmic) 
            buildLogScale(); 
        else 
            buildLinScale();
    }
    else
    {
        double start, stop, step;
        if (d_scaleOpt & Inverted)
        {
            start = d_scaleMax;
            stop = d_scaleMin;
            step = -d_step;
        }
        else
        {
            start = d_scaleMin;
            stop = d_scaleMax;
            step = d_step;
        }

        d_scldiv.rebuild(start, stop, d_maxMajor, d_maxMinor,
            bool(d_scaleOpt & Logarithmic), step, FALSE); 
    }
}


/*!
  \brief Build a linear scale
*/
void QwtAutoScale::buildLinScale ()
{
    double delta;
    const double ticks = double (d_maxMajor);

    //
    // If in Autoscale Mode, adjust minval and maxval according to
    // the active scale options, and add the margins
    //
    if (!d_autoScale) 
        return;
    
    double minval = d_minValue;    // scale boundaries are based on the
    double maxval = d_maxValue;    // data.

    //
    // add / subtract margins
    //
    if (d_loMargin > 0.0)
       minval -= d_loMargin;
    if (d_hiMargin > 0.0)
       maxval += d_hiMargin;

    //
    //  Correct minval / maxval according to the scale options
    //
    if (d_scaleOpt & Symmetric)
    {
        delta = qwtMax(qwtAbs(d_ref - maxval), qwtAbs(d_ref - minval));
        maxval = d_ref + delta;
        minval = d_ref - delta; 
    }
    else if (d_scaleOpt & IncludeRef)
    {
        if (maxval < d_ref) 
           maxval = d_ref;
        else if (minval > d_ref) 
           minval = d_ref;
    }
    
    //
    // first approximation of d_scaleMin and d_scaleMax
    //
    setRange(minval, maxval);
    delta = d_scaleMax - d_scaleMin;


    // dec := maximal power of ten which fits into the interval
    //   [d_scaleMin,d_scaleMax]
    const double dec = pow (10.0, floor (log10 (delta)));

    //
    //  The following magic line calculates the step size such that
    //      - The number of subintervals will not exceed the maximum
    //        as specified by the user
    //      - The step size fits {1,2,5}*10^n with a natural number n  
    // 
    double step = qwtCeil125(delta * 0.999999 / dec / ticks) * dec;

    //
    //  determine he final values of scaleMin and scaleMax
    //
    if (! (d_scaleOpt & Floating) )
    {
       // adjust of d_scaleMin and d_scaleMax such that both are integer
       // multiples of the step size.
       d_scaleMin = step * floor ((d_scaleMin + MinEps * step) / step);
       d_scaleMax = step * ceil ((d_scaleMax - MinEps * step) / step);
    }

    if (d_scaleOpt & Inverted)
    {
        step = -step;
        d_scldiv.rebuild(d_scaleMax, d_scaleMin, d_maxMajor, d_maxMinor,
             FALSE, step, FALSE);
    }
    else
    {
        d_scldiv.rebuild(d_scaleMin, d_scaleMax, d_maxMajor, d_maxMinor,
             FALSE, step, TRUE);
    }
}


/*!
  \brief build a logarithmic scale
*/
void QwtAutoScale::buildLogScale ()
{
    if (!d_autoScale) 
        return;

    double minval = d_minValue; // the calculation of scale divisions 
    double maxval = d_maxValue; // is based on the input data.

    if (d_loMargin > 0.0)
        minval /= pow(10.0, d_loMargin);
    if (d_hiMargin > 0.0)
        maxval *= pow(10.0, d_hiMargin);

    if (d_scaleOpt & Symmetric)
    {
        const double delta = qwtMax(maxval / d_lref,  d_lref / minval); 
        maxval = d_lref * delta;
        minval = d_lref / delta;    
    }
    else if (d_scaleOpt & IncludeRef)
    {
        if (maxval < d_lref) 
            maxval = d_lref;
        else if (minval > d_lref) 
            minval = d_lref;
    }

    const double ticks = (d_maxMajor > 0) ? double(d_maxMajor) : 1;

    setRange(minval, maxval);

    // decades included in the interval
    const double decades = qwtAbs(log10 (d_scaleMax / d_scaleMin));

    // calculate step size in decades

    double step;
    if ((decades > 1.0) && (decades > ticks))
    {
        double ipart;
        // One interval contains more than one decade.
        // The number of decades in an interval is adjusted
        // to be a multiple of 2,3,5, or 10.
        double fpart = modf (log10 (ceil (decades * 0.999999 / ticks)), &ipart);
        if (fpart < MinEps)
           fpart = 1.0;
        else if ((fpart - LOG10_2) < MinEps)
           fpart = 2.0;
        else if ((fpart - LOG10_3) < MinEps)
           fpart = 3.0;
        else if ((fpart - LOG10_5) < MinEps)
           fpart = 5.0;
        else
           fpart = 10.0;

        step = pow (10.0, ipart) * fpart;

    }
    else                // The minimal step size is one decade.
    {
        step = 1.0;
    }
    
    if (!(d_scaleOpt & Floating))
    {
        d_scaleMin = pow (10.0, step * 
            floor ((log10(d_scaleMin) + MinEps * step) / step));
        d_scaleMax = pow (10.0, step * 
            ceil ((log10(d_scaleMax) - MinEps * step) / step));
    }

    if (d_scaleOpt & Inverted)
    {
        step = -step;
        d_scldiv.rebuild(d_scaleMax, d_scaleMin, d_maxMajor, d_maxMinor, TRUE,
             step, FALSE);
    }
    else
    {
        d_scldiv.rebuild(d_scaleMin, d_scaleMax, d_maxMajor, d_maxMinor,
             TRUE, step, TRUE);
    }
}

/*!
  \brief Set or reset specified scale options
  \param opt or-combined  scale options
  \param tf If \c TRUE, set the specified options.
  If \c FALSE, reset these options.
  \sa QwtAutoScale::setOptions()
*/
void QwtAutoScale::changeOptions(int opt, bool tf)
{
    if (tf)
       d_scaleOpt |= opt;
    else
       d_scaleOpt &= (~opt);
    build();
}


/*!
  \brief Set the interval boundaries to zero and clear the scale division

  This member function resets an AutoScale object 
  to its initial state. It is needed to clean up
  the scale before or
  after subsequent adjust() calls.
  The boundaries of the scale are set to zero
  and the scale division is cleared.

  \warning A reset doesn't affect the margins.
*/
void QwtAutoScale::reset()
{
    d_reset = TRUE;
    d_scldiv.reset();
    d_minValue = 0;
    d_maxValue = 0;
    d_step = 0;
}


/*!
  \brief Enable auto-scaling

  This function is used to switch back to auto-scaling mode
  if the scale has been frozen temporarily (see setScale()).

  When auto-scaling is reactivated, the scale will be rebuild, which
  means that
  \li if adjust or setMaxIntv have been called in between, the scale
      will be adjusted to the new conditions.
  \li if none of these functions and no reset has been called, the old state
      will be restored.
  \li if only reset has been called in between, nothing will happen.

  \sa QwtAutoScale::setScale()
*/
void QwtAutoScale::setAutoScale()
{
    d_autoScale = TRUE;
    build();
}

/*!
  \brief Specify margins at the scale's endpoints
  \param mlo minimum distance between the scale's lower boundary and the
             smallest enclosed value
  \param mhi minimum distance between the scale's upper boundary and the
             greatest enclosed value

  Margins can be used to leave a minimum amount of space between
  the enclosed intervals and the boundaries of the scale.

  \warning
  \li With logarithmic scales, the margins are measured in decades.
  \li The margins will not be changed by any other member function.
      You should remember this when you call reset()
      or change from a linear to a logarithmic scale.
*/
void QwtAutoScale::setMargins(double mlo, double mhi)
{
    d_loMargin = qwtMax(mlo,0.0);
    d_hiMargin = qwtMax(mhi,0.0);
    build();
}


/*!
  \brief Specify the maximum number of major intervals
  \param mx maximum number of subintervals

  The auto-scaler places the major ticks at reasonable
  points, such that the number of major tick intervals does not exceed
  the specified maximum number.
*/
void QwtAutoScale::setMaxMajor(int mx)
{
    d_maxMajor = qwtMax(mx,1);
    d_maxMajor = qwtMin(mx, 10000);
    build();
}

/*!
  \brief Specify the maximum number of minor subdivisions within major scale
         intervals
  \param mx maximum number of minor ticks
*/
void QwtAutoScale::setMaxMinor(int mx)
{
    d_maxMinor = qwtMin(qwtMax(mx,0), 100);
    build();
}


/*!
  \brief Specify a user-defined range

  \param x1 user-defined lower boundary
  \param x2 user-defined upper boundary

  \warning
  \li if x1 > x2, the lower boundary will be set to x2 and the upper boundary
      to x1.
  \li if x1 == x2, the auto-scaler sets the boundaries to (-0.5, 0.5) for
      linear scales and to (LOG_MIN, LOG_MAX) for logarithmic scales.

  \sa QwtAutoScale::setScale(), LOG_MIN, LOG_MAX.
*/
void QwtAutoScale::setRange(double x1, double x2)
{
    double minval = qwtMin(x1, x2);
    double maxval = qwtMax(x1, x2);

    if (d_scaleOpt & Logarithmic)
    {
        minval = qwtMin(qwtMax(minval, LOG_MIN), LOG_MAX);
        maxval = qwtMin(qwtMax(maxval, LOG_MIN), LOG_MAX);
    }
    
    double delta = maxval - minval;
    
    if (delta <= 0.0)       // all values are equal
    {               
        if (minval > 0)
        {
            d_scaleMin = minval * 0.5;
            d_scaleMax = maxval * 1.5;
        }
        else if (minval < 0)
        {
            d_scaleMin = minval * 1.5;
            d_scaleMax = maxval * 0.5;
        }
        else              // all values are zero
        {           
            d_scaleMin = -0.5;
            d_scaleMax = 0.5;
        }

        delta = d_scaleMax - d_scaleMin;
    }
    else            // the normal case
    {               
        d_scaleMin = minval;
        d_scaleMax = maxval;
    }
}

/*!
  \brief Specify a user-defined scale and switch off auto-scaling

  \param xmin user-defined lower boundary
  \param xmax user-defined upper boundary
  \param step user-defined fixed major step size

  A fixed scale may be used for different purposes, e.g.
  zooming. If the step argument is left out or less or equal
  to zero, the auto-scaler will calculate the major step size
  size according to the maxMajor setting.

  The fixed-scale mode can switched off using
  setAutoScale(), which restores the
  previous values.

  \warning
  \li if xmin > xmax, the lower boundary will be set to xmax and the upper
      boundary to xmin.
  \li if xmin == xmax, the auto-scaler sets the boundaries to (-0.5, 0.5)
      for linear scales and to (LOG_MIN, LOG_MAX) for logarithmic scales.
  \li Options and margins have no effect while auto-scaling is switched off.

  \sa QwtAutoScale::setMaxMajor(), QwtAutoScale::setAutoScale(),
      LOG_MIN, LOG_MAX.
*/
void QwtAutoScale::setScale(double xmin, double xmax, double step)
{
    // turn auto-scaling off and set the
    // scale limits to the desired values
    setRange(xmin,xmax);
    d_autoScale = FALSE;
    d_step = step;

    build(); // rebuild the scale
}


/*!
  \brief Reset scale options and set specified options
  \param opt Combined set of options

  The behaviour of the auto-scaling algorithm can be changed
  with the following options:
  <dl>
  <dt>QwtAutoScale::None
  <dd>Switch all options off.
  <dt>QwtAutoscale::IncludeRef
  <dd>Build a scale which includes the reference value.
  <dt>QwtAutoScale::Symmetric
  <dd>Build a scale which is symmetric to the reference value.
  <dt>QwtAutoScale::Logarithmic
  <dd>Build a logarithmic scale.
  <dt>QwtAutoScale::Floating
  <dd>The endpoints of the scale are supposed to be equal the outmost included
  values plus the specified margins (see setMargins()). If this option is
  *not* set, the endpoints of the scale will be integer multiples of the step
  size.
  <dt>QwtAutoScale::Inverted
  <dd>Turn the scale upside down.
  </dl>

  \warning
  \li If the type of scale division is changed from logarithmic to linear
      or vice versa, the margins will not be transformed. Note that
      the margins count in decades if the scale is logarithmic.
  \li If a linear scale contains negative values, switching to a
      logarithmic scale will cut them off and set the lower scale
      boundary to its lowest possible value of 1.0E-100. This effect
      is reversible if you
      switch back to a linear scale.
  \li The options have no effect while auto-scaling is turned off
      (see setScale())
      
  Example:\code
#include "../include/qwt_autoscl.h>

void main() 
{
    QwtAutoScale as;

    // build linear scale with default settings
    as.adjust(17.45, 87344.0);

    //...

    // change to logarithmic scale with floating ends
    as.setOptions(QwtAutoScale::Floating | QwtAutoscale::Logarithmic);

    //...

    // change to linear, zero-symmetric scale
    as.setOptions(QwtAutoScale::ZeroSymmetric);

    //...
}\endcode

  \sa QwtAutoScale::changeOptions() for a description of the possible options.
*/
void QwtAutoScale::setOptions(int opt)
{
    d_scaleOpt = opt;
    build();
}


/*!
  \brief Specify a reference point
  \param r new reference value

  The reference point is needed if the auto-scaler options IncludeRef or
  Symmetric are active. Its default value is 0 for linear scales and 1 for
  logarithmic scales.

  \warning The reference value for logarithmic scales is limited to
  ( LOG_MIN / 2 <= reference <= LOG_MAX /2 ). If the specified
  value is less than LOG_MIN, it will be set to 1.0 for logarithmic scales.

  \sa LOG_MIN, LOG_MAX.
*/
void QwtAutoScale::setReference(double r)
{
    d_ref = r;
    
    if (r > LOG_MIN / 2) 
        d_lref = qwtMin(r, LOG_MAX / 2);
    else
       d_lref = 1.0;

    build();
}

/*!
    \return the reference value
    \sa QwtAutoScale::setOptions(), QwtAutoScale::option()
*/
double QwtAutoScale::reference() const 
{ 
    return d_ref; 
}


/*!
  \brief Returns TRUE if the specified option is set.
  \param opt Option
  \sa QwtAutoScale::setOptions()
*/
bool QwtAutoScale::option(int opt) const
{
    return bool(d_scaleOpt & opt);
}

/*!
    \return options
    \sa QwtAutoScale::setOptions(), QwtAutoScale::option()
*/

int QwtAutoScale::options() const 
{ 
    return d_scaleOpt; 
}

/*!
    The scale division consists of two boundary values,
    an array of major tickmarks and an array of minor
    tickmarks.
    \return a const reference to the scale division
    \sa QwtScaleDiv
*/

const QwtScaleDiv &QwtAutoScale::scaleDiv() const 
{ 
    return d_scldiv; 
}

/*!
    If true, rebuild scale automatically with call to 'adjust'
    \sa QwtAutoScale::autoRebuild()
*/

void QwtAutoScale::setAutoRebuild(bool tf) 
{ 
    d_autoRebuild = tf; 
}

/*!
    \return If true, rebuild scale automatically with call to 'adjust'
    \sa QwtAutoScale::setAutoRebuild()
*/
bool QwtAutoScale::autoRebuild() const
{
    return d_autoRebuild;
}
