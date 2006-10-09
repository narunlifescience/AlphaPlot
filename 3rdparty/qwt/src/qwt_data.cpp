/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#include "qwt_math.h"
#include "qwt_data.h"

QwtData::QwtData()
{
}

QwtData::~QwtData()
{
}

/*!
  Returns the bounding rectangle of the data. If there is
  no bounding rect, like for empty data the rectangle is invalid:
  QwtDoubleRect::isValid() == false

  \warning This is an slow implementation iterating over all points. 
           It is intended to be overloaded by derived classes. In case of
           auto scaling boundingRect() is called for every replot, so it 
           might be worth to implement a cache, or use x(0), x(size() - 1)
           for ordered data ...
*/
QwtDoubleRect QwtData::boundingRect() const
{
    const size_t sz = size();

    if ( sz <= 0 )
        return QwtDoubleRect(1.0, 1.0, -2.0, -2.0); // invalid

    double minX, maxX, minY, maxY;
    minX = maxX = x(0);
    minY = maxY = y(0);

    for ( size_t i = 1; i < sz; i++ )
    {
        const double xv = x(i);
        if ( xv < minX )
            minX = xv;
        if ( xv > maxX )
            maxX = xv;

        const double yv = y(i);
        if ( yv < minY )
            minY = yv;
        if ( yv > maxY )
            maxY = yv;
    }
    return QwtDoubleRect(minX, minY, maxX - minX, maxY - minY);
}

#if QT_VERSION >= 0x040000
QwtPolygonFData::QwtPolygonFData(const QPolygonF &data):
#else
QwtPolygonFData::QwtPolygonFData(const QwtArray<QwtDoublePoint> &data):
#endif
    d_data(data)
{
}

//! Assignment 
QwtPolygonFData& QwtPolygonFData::operator=(
    const QwtPolygonFData &data)
{
    if (this != &data)
    {
        d_data = data.d_data;
    }
    return *this;
}

size_t QwtPolygonFData::size() const 
{ 
    return d_data.size(); 
}

double QwtPolygonFData::x(size_t i) const 
{ 
    return d_data[int(i)].x(); 
}

double QwtPolygonFData::y(size_t i) const 
{ 
    return d_data[int(i)].y(); 
}

#if QT_VERSION >= 0x040000
const QPolygonF &QwtPolygonFData::data() const
#else
const QwtArray<QwtDoublePoint> &QwtPolygonFData::data() const
#endif
{
    return d_data;
}

QwtData *QwtPolygonFData::copy() const 
{ 
    return new QwtPolygonFData(d_data); 
}

/*!
  Constructor
  
  \sa QwtCurve::setData and QwtPlot::setCurveData.
*/
QwtArrayData::QwtArrayData(
        const QwtArray<double> &x, const QwtArray<double> &y): 
    d_x(x), 
    d_y(y)
{
}

/*!
  Constructor
  
  \sa QwtCurve::setData and QwtPlot::setCurveData.
*/
QwtArrayData::QwtArrayData(const double *x, const double *y, size_t size)
{
#if QT_VERSION >= 0x040000
    d_x.resize(size);
    qMemCopy(d_x.data(), x, size * sizeof(double));

    d_y.resize(size);
    qMemCopy(d_y.data(), y, size * sizeof(double));
#else
    d_x.detach();
    d_x.duplicate(x, size);

    d_y.detach();
    d_y.duplicate(y, size);
#endif
}

//! Assignment 
QwtArrayData& QwtArrayData::operator=(const QwtArrayData &data)
{
    if (this != &data)
    {
        d_x = data.d_x;
        d_y = data.d_y;
    }
    return *this;
}

size_t QwtArrayData::size() const 
{ 
    return qwtMin(d_x.size(), d_y.size()); 
}

double QwtArrayData::x(size_t i) const 
{ 
    return d_x[int(i)]; 
}

double QwtArrayData::y(size_t i) const 
{ 
    return d_y[int(i)]; 
}

const QwtArray<double> &QwtArrayData::xData() const
{
    return d_x;
}

const QwtArray<double> &QwtArrayData::yData() const
{
    return d_y;
}

QwtData *QwtArrayData::copy() const 
{ 
    return new QwtArrayData(d_x, d_y); 
}

/*!
  Returns the bounding rectangle of the data. If there is
  no bounding rect, like for empty data the rectangle is invalid:
  QwtDoubleRect::isValid() == false
*/
QwtDoubleRect QwtArrayData::boundingRect() const
{
    const size_t sz = size();

    if ( sz <= 0 )
        return QwtDoubleRect(1.0, 1.0, -2.0, -2.0); // invalid

    double minX, maxX, minY, maxY;
    QwtArray<double>::ConstIterator xIt = d_x.begin();
    QwtArray<double>::ConstIterator yIt = d_y.begin();
    QwtArray<double>::ConstIterator end = d_x.begin() + sz;
    minX = maxX = *xIt++;
    minY = maxY = *yIt++;

    while ( xIt < end )
    {
        const double xv = *xIt++;
        if ( xv < minX )
            minX = xv;
        if ( xv > maxX )
            maxX = xv;

        const double yv = *yIt++;
        if ( yv < minY )
            minY = yv;
        if ( yv > maxY )
            maxY = yv;
    }
    return QwtDoubleRect(minX, minY, maxX - minX, maxY - minY);
}

QwtCPointerData::QwtCPointerData(const double *x, const double *y,
                                 size_t size):
    d_x(x), d_y(y), d_size(size)
{
}

//! Assignment 
QwtCPointerData& QwtCPointerData::operator=(const QwtCPointerData &data)
{
    if (this != &data)
    {
        d_x = data.d_x;
        d_y = data.d_y;
        d_size = data.d_size;
    }
    return *this;
}

size_t QwtCPointerData::size() const 
{   
    return d_size; 
}

double QwtCPointerData::x(size_t i) const 
{ 
    return d_x[int(i)]; 
}

double QwtCPointerData::y(size_t i) const 
{ 
    return d_y[int(i)]; 
}

const double *QwtCPointerData::xData() const
{
    return d_x;
}

const double *QwtCPointerData::yData() const
{
    return d_y;
}

QwtData *QwtCPointerData::copy() const 
{
    return new QwtCPointerData(d_x, d_y, d_size);
}

/*!
  Returns the bounding rectangle of the data. If there is
  no bounding rect, like for empty data the rectangle is invalid:
  QwtDoubleRect::isValid() == false
*/
QwtDoubleRect QwtCPointerData::boundingRect() const
{
    const size_t sz = size();

    if ( sz <= 0 )
        return QwtDoubleRect(1.0, 1.0, -2.0, -2.0); // invalid

    double minX, maxX, minY, maxY;
    const double *xIt = d_x;
    const double *yIt = d_y;
    const double *end = d_x + sz;
    minX = maxX = *xIt++;
    minY = maxY = *yIt++;

    while ( xIt < end )
    {
        const double xv = *xIt++;
        if ( xv < minX )
            minX = xv;
        if ( xv > maxX )
            maxX = xv;

        const double yv = *yIt++;
        if ( yv < minY )
            minY = yv;
        if ( yv > maxY )
            maxY = yv;
    }
    return QwtDoubleRect(minX, minY, maxX - minX, maxY - minY);
}
