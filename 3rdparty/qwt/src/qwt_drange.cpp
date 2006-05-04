/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#include "qwt_drange.h"
#include "qwt_math.h"

static double MinRelStep = 1.0e-10;
static double DefaultRelStep = 1.0e-2;
static double MinEps = 1.0e-10;

/*!
  The range is initialized to [0.0, 100.0], the
  step size to 1.0, and the value to 0.0.
*/
QwtDblRange::QwtDblRange():
    d_minValue(0.0),
    d_maxValue(100.0),
    d_step(1.0),
    d_pageSize(1),
    d_isValid(FALSE),
    d_value(0.0),
    d_exactValue(0.0),
    d_exactPrevValue(0.0),
    d_prevValue(0.0),
    d_periodic(FALSE)
{
}

//! Destroys the QwtDblRange
QwtDblRange::~QwtDblRange()
{
}

//! Set the value to be valid/invalid
void QwtDblRange::setValid(bool isValid)
{
    if ( isValid != d_isValid )
    {
        d_isValid = isValid;
        valueChange();
    }
}

//! Indicates if the value is valid
bool QwtDblRange::isValid() const
{
    return d_isValid;
}

/*!
  \brief No docs
  
  Description
  \param x ???
  \param align
  \todo Documentation
*/
void QwtDblRange::setNewValue(double x, int align)
{
    double vmin,vmax;
    
    d_prevValue = d_value;

    vmin = qwtMin(d_minValue, d_maxValue);
    vmax = qwtMax(d_minValue, d_maxValue);

    // 
    // Range check
    //
    if (x < vmin)
    {
        if ((d_periodic) && (vmin != vmax))
           d_value = x + ceil( (vmin - x) / (vmax - vmin ) ) 
              * (vmax - vmin);
        else
           d_value = vmin;
    }
    else if (x > vmax)
    {
        if ((d_periodic) && (vmin != vmax))
           d_value = x - ceil( ( x - vmax) / (vmax - vmin )) 
              * (vmax - vmin);
        else
           d_value = vmax;
    }
    else
       d_value = x;

    d_exactPrevValue = d_exactValue;
    d_exactValue = d_value;
    
    // align to grid
    if (align)
    {
        if (d_step != 0.0)
           d_value = d_minValue +
             floor ((d_value - d_minValue) / d_step + 0.5) * d_step;
        else
           d_value = d_minValue;
        
        // correct rounding error at the border
        if (fabs(d_value - d_maxValue) < MinEps * qwtAbs(d_step))
           d_value = d_maxValue;

        // correct rounding error if value = 0
        if (fabs(d_value) < MinEps * qwtAbs(d_step))
           d_value = 0.0;
    }

    if (!d_isValid || d_prevValue != d_value)
    {
        d_isValid = TRUE;
        valueChange();
    }
}

/*!
  \brief  Adjust the value to the closest point in the step raster.
  \param x value
  \warning The value is clipped when it lies outside the range.
  When the range is QwtDblRange::periodic, it will
  be mapped to a point in the interval such that
  \verbatim new value := x + n * (max. value - min. value)\endverbatim
  with an integer number n.
*/
void QwtDblRange::fitValue(double x)
{
    setNewValue(x,1);
}


/*!
  \brief Set a new value without adjusting to the step raster
  \param x new value
  \warning The value is clipped when it lies outside the range.
  When the range is QwtDblRange::periodic, it will
  be mapped to a point in the interval such that
  \verbatim new value := x + n * (max. value - min. value)\endverbatim
  with an integer number n.
*/
void QwtDblRange::setValue(double x)
{
    setNewValue(x,0);
}

/*!
  \brief Specify  range and step size

  \param vmin   lower boundary of the interval
  \param vmax   higher boundary of the interval
  \param vstep  step width
  \param pageSize  page size in steps
  \warning
  \li A change of the range changes the value if it lies outside the
      new range. The current value
      will *not* be adjusted to the new step raster.
  \li vmax < vmin is allowed.
  \li If the step size is left out or set to zero, it will be
      set to 1/100 of the interval length.
  \li If the step size has an absurd value, it will be corrected
      to a better one.
*/
void QwtDblRange::setRange(double vmin, double vmax, double vstep, int pageSize)
{
    int rchg = ((d_maxValue != vmax) || (d_minValue != vmin));
    
    if (rchg) 
    {
        d_minValue = vmin;
        d_maxValue = vmax;
    }
    
    //
    // look if the step width has an acceptable 
    // value or otherwise change it.
    //
    setStep(vstep);

    //
    // limit page size
    //
    d_pageSize = qwtLim(pageSize,0, 
        int(qwtAbs((d_maxValue - d_minValue) / d_step))); 
    
    // 
    // If the value lies out of the range, it 
    // will be changed. Note that it will not be adjusted to 
    // the new step width.
    setNewValue(d_value,0);
    
    // call notifier after the step width has been 
    // adjusted.
    if (rchg)
       rangeChange();
}

/*!
  \brief Change the step raster     
  \param vstep new step width
  \warning The value will \e not be adjusted to the new step raster.
*/
void QwtDblRange::setStep(double vstep)
{
    double intv = d_maxValue - d_minValue;
    
    double newStep;
    if (vstep == 0.0)
       newStep = intv * DefaultRelStep;
    else
    {
        if ((intv > 0) && (vstep < 0) || (intv < 0) && (vstep > 0))
           newStep = -vstep;
        else
           newStep = vstep;
        
        if ( fabs(newStep) < fabs(MinRelStep * intv) )
           newStep = MinRelStep * intv;
    }
    
    if (newStep != d_step)
    {
        d_step = newStep;
        stepChange();
    }
}


/*!
  \brief Make the range periodic

  When the range is periodic, the value will be set to a point
  inside the interval such that

  \verbatim point = value + n * width \endverbatim

  if the user tries to set a new value which is outside the range.
  If the range is nonperiodic (the default), values outside the
  range will be clipped.

  \param tf TRUE for a periodic range
*/
void QwtDblRange::setPeriodic(bool tf)
{
    d_periodic = tf;
}

/*!
  \brief Increment the value by a specified number of steps
  \param nSteps Number of steps to increment
  \warning As a result of this operation, the new value will always be
       adjusted to the step raster.
*/
void QwtDblRange::incValue(int nSteps)
{
    if ( isValid() )
        setNewValue(d_value + double(nSteps) * d_step,1);
}

/*!
  \brief Increment the value by a specified number of pages
  \param nPages Number of pages to increment.
        A negative number decrements the value.
  \warning The Page size is specified in the constructor.
*/
void QwtDblRange::incPages(int nPages)
{
    if ( isValid() )
        setNewValue(d_value + double(nPages) * double(d_pageSize) * d_step,1);
}

/*!
  \brief Notify a change of value

  This virtual function is called whenever the value changes.
  The default implementation does nothing.
*/
void QwtDblRange::valueChange()
{
}


/*!
  \brief Notify a change of the range

  This virtual function is called whenever the range changes.
  The default implementation does nothing.
*/
void QwtDblRange::rangeChange()
{
}


/*!
  \brief Notify a change of the step size

  This virtual function is called whenever the step size changes.
  The default implementation does nothing.
*/
void QwtDblRange::stepChange()
{
}

/*!
  \return the step size
  \sa QwtDblRange::setStep, QwtDblRange::setRange
*/
double QwtDblRange::step() const
{
    return qwtAbs(d_step);
}

/*!
  \brief Returns the value of the second border of the range

  maxValue returns the value which has been specified
  as the second parameter in  QwtDblRange::setRange.
    
  \sa QwtDblRange::setRange()
*/  
double QwtDblRange::maxValue() const
{   
    return d_maxValue;
} 
    
/*!
  \brief Returns the value at the first border of the range
    
  minValue returns the value which has been specified
  as the first parameter in  setRange().
    
  \sa QwtDblRange::setRange()
*/
double QwtDblRange::minValue() const 
{
    return d_minValue; 
}   

/*!
  \brief Returns TRUE if the range is periodic
  \sa QwtDblRange::setPeriodic()
*/
bool QwtDblRange::periodic() const 
{ 
    return d_periodic; 
}

//! Returns the page size in steps.
int QwtDblRange::pageSize() const 
{ 
    return d_pageSize; 
}

//! Returns the current value.
double QwtDblRange::value() const 
{ 
    return d_value; 
}

/*!
  \brief Returns the exact value

  The exact value is the value which QwtDblRange::value would return
  if the value were not adjusted to the step raster. It differs from
  the current value only if QwtDblRange::fitValue or
  QwtDblRange::incValue have been used before. This function
  is intended for internal use in derived classes.
*/
double QwtDblRange::exactValue() const 
{ 
    return d_exactValue; 
}

//! Returns the exact previous value
double QwtDblRange::exactPrevValue() const 
{ 
    return d_exactPrevValue; 
}

//! Returns the previous value
double QwtDblRange::prevValue() const 
{ 
    return d_prevValue; 
}
