/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#include "qwt_scldiv.h"
#include "qwt_math.h"

static const double step_eps = 1.0e-3;
static const double border_eps = 1.0e-10;

static bool qwtLimRange(double &val, double v1, double v2, 
    double eps_rel = 0.0, double eps_abs = 0.0)
{
    bool rv = TRUE;
    double vmin = qwtMin(v1, v2);
    double vmax = qwtMax(v1, v2);
    double delta_min = qwtMax(qwtAbs(eps_rel * vmin), qwtAbs(eps_abs));
    double delta_max = qwtMax(qwtAbs(eps_rel * vmax), qwtAbs(eps_abs));
    
    if (val < vmin) 
    {
        if (val < vmin - delta_min) 
            rv = FALSE;
        val = vmin;
    }
    else if (val > vmax)
    {
        if (val > vmax + delta_max) 
            rv = FALSE;
        val = vmax;
    }
    return rv;
}


//! Construct a QwtScaleDiv instance.
QwtScaleDiv::QwtScaleDiv()
{
    d_lBound = 0.0;
    d_hBound = 0.0;
    d_majStep = 0.0;
    d_log = FALSE;
}

/*!
  \brief Build a scale width major and minor divisions
  If no fixed step width is specified or if it is set to 0, the
  major step width will be calculated automatically according to the
  the value of maxMajSteps. The maxMajSteps parameter has no effect
  if a fixed step size is specified. The minor step width is always
  calculated automatically.

  If the step width is to be calculated automatically, the algorithm
  tries to find reasonable values fitting into the scheme {1,2,5}*10^n
  with an integer number n for linear scales.

  For logarithmic scales, there are three different cases
  <ol>
  <li>If the major step width is one decade, the minor marks
      will fit into one of the schemes {1,2,...9}, {2,4,6,8}, {2,5} or {5},
      depending on the maxMinSteps parameter.
  <li>If the major step size spans
      more than one decade, the minor step size will be {1,2,5}*10^n decades
      with a natural number n.
  <li>If the whole range is less than one decade, a linear scale
      division will be built.
  </ol>

  \param x1 first boundary value
  \param x2 second boundary value
  \param maxMajSteps max. number of major step intervals
  \param maxMinSteps max. number of minor step intervals
  \param log logarithmic division (TRUE/FALSE)
  \param step  fixed major step width. Defaults to 0.0.
  \param ascend if TRUE, sort in ascending order from min(x1, x2)
                to max(x1, x2). If FALSE, sort in the direction
        from x1 to x2. Defaults to TRUE.
  \return True if the arrays have been allocated successfully.
  \warning For logarithmic scales, the step width is measured in decades.
*/
bool QwtScaleDiv::rebuild(double x1, double x2, 
    int maxMajSteps, int maxMinSteps, bool log, double step, bool ascend)
{
  int rv;

  d_lBound = qwtMin(x1, x2);
  d_hBound = qwtMax(x1, x2);
  d_log = log;
  
  if (d_log)
      rv = buildLogDiv(maxMajSteps,maxMinSteps,step);
  else
      rv = buildLinDiv(maxMajSteps, maxMinSteps, step);

  if ((!ascend) && (x2 < x1))
  {
      d_lBound = x1;
      d_hBound = x2;
      qwtTwistArray(d_majMarks.data(), d_majMarks.size());
      qwtTwistArray(d_minMarks.data(), d_minMarks.size());
  }

  return rv;
}

/*!
  \brief Build a linear scale division in ascending order

  If the 'step' parameter is set to 0.0, this function
  cal[culates the step width automatically according to
  the value of 'maxSteps'. MaxSteps must be greater than or
  equal to 2. It will be guessed if an invalid value is specified.

  The maximum possible number of steps is   limited to 10000.
  The maxSteps parameter has no effect if a fixed step width is
  specified.

  \param maxSteps max. number of step intervals
  \param step -- fixed step width
  \return TRUE if array has been successfully resized
  \warning This function uses the data members d_lBound and d_hBound
  and assumes that d_hBound > d_lBound.
*/
bool QwtScaleDiv::buildLinDiv(int maxMajSteps, int maxMinSteps, double step)
{
    int nMaj, nMin, minSize, i0,i,k;
    double val, mval;
    double firstTick, lastTick;
    double minStep;
    QwtArray<double> buffer;
    bool rv = TRUE;

    // parameter range check
    maxMajSteps = qwtMax(1, maxMajSteps);
    maxMinSteps = qwtMax(0, maxMinSteps);
    step = qwtAbs(step);
    
    // detach arrays
    d_majMarks.duplicate(0,0);
    d_minMarks.duplicate(0,0);

    if (d_lBound == d_hBound) return TRUE;
    
    //
    // Set up major divisions
    //
    if (step == 0.0)
       d_majStep = qwtCeil125(qwtAbs(d_hBound - d_lBound) * 0.999999
                  / double(maxMajSteps));
    else
       d_majStep = step;

    if (d_majStep == 0.0) return TRUE;

    firstTick = ceil( (d_lBound - step_eps * d_majStep) / d_majStep) * d_majStep;
    lastTick = floor( (d_hBound + step_eps * d_majStep) / d_majStep) * d_majStep;

    nMaj = qwtMin(10000, int(floor ((lastTick - firstTick) / d_majStep + 0.5)) + 1);

    if ((rv = d_majMarks.resize(nMaj)))
       qwtLinSpace(d_majMarks.data(), d_majMarks.size(), firstTick, lastTick);
    else
       return FALSE;

    //
    // Set up minor divisions
    //
    if (maxMinSteps < 1) // no minor divs
       return TRUE;

    minStep = qwtCeil125( d_majStep  /  double(maxMinSteps) );

    if (minStep == 0.0) return TRUE;
    
    nMin = qwtAbs(int(floor(d_majStep / minStep + 0.5))) - 1; // # minor steps per interval

    // Do the minor steps fit into the interval?
    if ( qwtAbs(double(nMin +  1) * minStep - d_majStep) 
        >  step_eps * d_majStep)
    {
        nMin = 1;
        minStep = d_majStep * 0.5;
    }
    
    // Are there minor ticks below the first major tick?
    if (d_majMarks[0] > d_lBound )
       i0 = -1; 
    else
       i0 = 0;

    // resize buffer to the maximum possible number of minor ticks
    rv = buffer.resize(nMin * (nMaj + 1));
       
    // calculate minor ticks
    if (rv)
    {
        minSize = 0;
        for (i = i0; i < (int)d_majMarks.size(); i++)
        {
            if (i >= 0)
               val = d_majMarks[i];
            else
               val = d_majMarks[0] - d_majStep;

            for (k=0; k< nMin; k++)
            {
                mval = (val += minStep);
                if (qwtLimRange(mval, d_lBound, d_hBound, border_eps))
                {
                    buffer[minSize] = mval;
                    minSize++;
                }
            }
        }
        d_minMarks.duplicate(buffer.data(), minSize);
    }

    return rv;
}

/*!
  \brief Build a logarithmic scale division
  \return True if memory has been successfully allocated
  \warning This function uses the data members d_lBound and d_hBound
  and assumes that d_hBound > d_lBound.
*/
bool QwtScaleDiv::buildLogDiv(int maxMajSteps, int maxMinSteps, double majStep)
{
    double firstTick, lastTick;
    double lFirst, lLast;
    double val, sval, minStep, minFactor;
    int nMaj, nMin, minSize, i, k, k0, kstep, kmax, i0;
    int rv = TRUE;
    double width;
    
    QwtArray<double> buffer;
    

    // Parameter range check
    maxMajSteps = qwtMax(1, qwtAbs(maxMajSteps));
    maxMinSteps = qwtMax(0, qwtAbs(maxMinSteps));
    majStep = qwtAbs(majStep);

    // boundary check
    qwtLimRange(d_hBound, LOG_MIN, LOG_MAX);
    qwtLimRange(d_lBound, LOG_MIN, LOG_MAX);
    
    // detach arrays
    d_majMarks.duplicate(0,0);
    d_minMarks.duplicate(0,0);

    if (d_lBound == d_hBound) return TRUE;

    // scale width in decades
    width = log10(d_hBound) - log10(d_lBound);

    // scale width is less than one decade -> build linear scale
    if (width < 1.0) 
    {
        rv = buildLinDiv(maxMajSteps, maxMinSteps, 0.0);
        // convert step width to decades
        if (d_majStep > 0)
           d_majStep = log10(d_majStep);

        return rv;
    }

    //
    //  Set up major scale divisions
    //
    if (majStep == 0.0)
       d_majStep = qwtCeil125( width * 0.999999 / double(maxMajSteps));
    else
       d_majStep = majStep;

    // major step must be >= 1 decade
    d_majStep = qwtMax(d_majStep, 1.0);
    

    lFirst = ceil((log10(d_lBound) - step_eps * d_majStep) / d_majStep) * d_majStep;
    lLast = floor((log10(d_hBound) + step_eps * d_majStep) / d_majStep) * d_majStep;

    firstTick = pow(10.0, lFirst);
    lastTick = pow(10.0, lLast);
    
    nMaj = qwtMin(10000, int(floor (qwtAbs(lLast - lFirst) / d_majStep + 0.5)) + 1);

    if (d_majMarks.resize(nMaj))
       qwtLogSpace(d_majMarks.data(), d_majMarks.size(), firstTick, lastTick);
    else
       return FALSE;


    //
    // Set up minor scale divisions
    //

    if ((d_majMarks.size() < 1) || (maxMinSteps < 1)) return TRUE; // no minor marks

    if (d_majStep < 1.1)            // major step width is one decade
    {
        if (maxMinSteps >= 8)
        {
            k0 = 2;
            kmax = 9;
            kstep = 1;
            minSize = (d_majMarks.size() + 1) * 8;
        }
        else if (maxMinSteps >= 4)
        {
            k0 = 2;
            kmax = 8;
            kstep = 2;
            minSize = (d_majMarks.size() + 1) * 4;
        }
        else if (maxMinSteps >= 2)
        {
            k0 = 2;
            kmax = 5;
            kstep = 3;
            minSize = (d_majMarks.size() + 1) * 2;
        }
        else
        {
            k0 = 5;
            kmax = 5;
            kstep = 1;
            minSize = (d_majMarks.size() + 1);
        }
        
        // resize buffer to the max. possible number of minor marks
        buffer.resize(minSize);

        // Are there minor ticks below the first major tick?
        if ( d_lBound < firstTick )
            i0 = -1;
        else
           i0 = 0;
        
        minSize = 0;
        for (i = i0; i< (int)d_majMarks.size(); i++)
        {
            if (i >= 0)
               val = d_majMarks[i];
            else
               val = d_majMarks[0] / pow(10.0, d_majStep);
            
            for (k=k0; k<= kmax; k+=kstep)
            {
            sval = val * double(k);
            if (qwtLimRange(sval, d_lBound, d_hBound, border_eps))
            {
                buffer[minSize] = sval;
                minSize++;
            }
            }
        }

        // copy values into the minMarks array
        d_minMarks.duplicate(buffer.data(), minSize);

    }
    else                // major step > one decade
    {
        
        // substep width in decades, at least one decade
        minStep = qwtCeil125( (d_majStep - step_eps * (d_majStep / double(maxMinSteps)))
                 /  double(maxMinSteps) );
        minStep = qwtMax(1.0, minStep);

        // # subticks per interval
        nMin = int(floor (d_majStep / minStep + 0.5)) - 1;

        // Do the minor steps fit into the interval?
        if ( qwtAbs( double(nMin + 1) * minStep - d_majStep)  >  step_eps * d_majStep)
            nMin = 0;

        if (nMin < 1) return TRUE;      // no subticks

        // resize buffer to max. possible number of subticks
        buffer.resize((d_majMarks.size() + 1) * nMin );
        
        // substep factor = 10^substeps
        minFactor = qwtMax(pow(10.0, minStep), 10.0);

        // Are there minor ticks below the first major tick?
        if ( d_lBound < firstTick )
            i0 = -1;
        else
           i0 = 0;
        
        minSize = 0;
        for (i = i0; i< (int)d_majMarks.size(); i++)
        {
            if (i >= 0)
               val = d_majMarks[i];
            else
               val = firstTick / pow(10.0, d_majStep);
            
            for (k=0; k< nMin; k++)
            {
                sval = (val *= minFactor);
                if (qwtLimRange(sval, d_lBound, d_hBound, border_eps))
                {
                    buffer[minSize] = sval;
                    minSize++;
                }
            }
        }
        d_minMarks.duplicate(buffer.data(), minSize);
    }
    
    return rv;
}

/*!
  \brief Equality operator
  \return TRUE if this instance is equal to s
*/
int QwtScaleDiv::operator==(const QwtScaleDiv &s) const
{
    if (d_lBound != s.d_lBound) return 0;
    if (d_hBound != s.d_hBound) return 0;
    if (d_log != s.d_log) return 0;
    if (d_majStep != s.d_majStep) return 0;
    if (d_majMarks != s.d_majMarks) return 0;
    return (d_minMarks == s.d_minMarks);
}

/*!
  \brief Inequality
  \return TRUE if this instance is not equal to s
*/
int QwtScaleDiv::operator!=(const QwtScaleDiv &s) const
{
    return (!(*this == s));
}

//! Detach the shared data and set everything to zero. 
void QwtScaleDiv::reset()
{
    // detach arrays
    d_majMarks.duplicate(0,0);
    d_minMarks.duplicate(0,0);

    d_lBound = 0.0;
    d_hBound = 0.0;
    d_majStep = 0.0;
    d_log = FALSE;
}
