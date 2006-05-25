/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#include "qwt_math.h"
#include "qwt_interval_data.h"

QwtIntervalData::QwtIntervalData()
{
}

QwtIntervalData::QwtIntervalData(
        const QwtArray<QwtDoubleInterval> &interval, 
        const QwtArray<double> &y):
    d_interval(interval),
    d_y(y)
{
}
    
void QwtIntervalData::setData(
    const QwtArray<QwtDoubleInterval> &interval,
    const QwtArray<double> &y)
{
    d_interval = interval;
    d_y = y;
}

QwtDoubleRect QwtIntervalData::boundingRect() const
{
    double minX, maxX, minY, maxY;
    minX = maxX = minY = maxY = 0.0;

    bool isValid = false;

    const size_t sz = size();
    for ( size_t i = 0; i < sz; i++ )
    {
        const QwtDoubleInterval intv = interval(i);
        if ( !intv.isValid() )
            continue;

        const double yv = y(i);

        if ( !isValid )
        {
            minX = intv.minValue();
            maxX = intv.maxValue();
            minY = maxY = yv;

            isValid = true;
        }
        else
        {
            if ( intv.minValue() < minX )
                minX = intv.minValue();
            if ( intv.maxValue() > maxX )
                maxX = intv.maxValue();

            if ( yv < minY )
                minY = yv;
            if ( yv > maxY )
                maxY = yv;
        }
    }
    if ( !isValid )
        return QwtDoubleRect(1.0, 1.0, -2.0, -2.0); // invalid

    return QwtDoubleRect(minX, minY, maxX - minX, maxY - minY);
}
