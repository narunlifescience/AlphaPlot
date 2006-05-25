/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

// vim: expandtab

#ifndef QWT_DATA_H
#define QWT_DATA_H 1

#include "qwt_global.h"
#include "qwt_array.h"
#include "qwt_double_rect.h"

// MOC_SKIP_BEGIN
#if defined(QWT_TEMPLATEDLL)
template class QWT_EXPORT QwtArray<double>;
#endif 
// MOC_SKIP_END

/*!
  \brief QwtData defines an interface to any type of data.

  Classes, derived from QwtData may:
  - store the data in almost any type of container
  - calculate the data on the fly instead of storing it
  - implement data with error bars (this implies subclassing QwtPlot,
    QwtCurve and QwtPlotCurve)

  \warning In the future, "almost any type of container" may be limited to
  containers supporting bi-directional iterators like the vector, array
  and list classes in Qt or in the standard template library.
 */

class QWT_EXPORT QwtData
{
public:
    QwtData();
    virtual ~QwtData(); 
    /*!
      Return a pointer to a copy (virtual copy constructor)
     */
    virtual QwtData *copy() const = 0;

    /*!
      Return the size of the data set
     */
    virtual size_t size() const = 0;
    /*!
      Return the x value of data point i
      \param i Index
      \return x X value of data point i
     */
    virtual double x(size_t i) const = 0;
    /*!
      Return the y value of data point i
      \param i Index
      \return y Y value of data point i
     */
    virtual double y(size_t i) const = 0;

    virtual QwtDoubleRect boundingRect() const;

protected:
    /*!
      Assignment operator (virtualized)
     */
    QwtData &operator=(const QwtData &);
};

/*!
  \brief Data class containing a single QwtArray<QwtDoublePoint> object. 
 */
class QWT_EXPORT QwtDoublePointData: public QwtData
{
public:
    /*!
      Constructor
      
      \sa QwtCurve::setData and QwtPlot::setCurveData.
     */
    QwtDoublePointData(const QwtArray<QwtDoublePoint> &);
    QwtDoublePointData &operator=(const QwtDoublePointData &);
    virtual QwtData *copy() const;

    virtual size_t size() const;
    virtual double x(size_t i) const;
    virtual double y(size_t i) const;

    const QwtArray<QwtDoublePoint> data() const;

private:
    QwtArray<QwtDoublePoint> d_data;
};

/*!
  \brief Data class containing two QwtArray<double> objects.
 */

class QWT_EXPORT QwtArrayData: public QwtData
{
public:
    QwtArrayData(const QwtArray<double> &x, const QwtArray<double> &y);
    QwtArrayData(const double *x, const double *y, size_t size);
    QwtArrayData &operator=(const QwtArrayData &);
    virtual QwtData *copy() const;

    virtual size_t size() const;
    virtual double x(size_t i) const;
    virtual double y(size_t i) const;

    const QwtArray<double> &xData() const;
    const QwtArray<double> &yData() const;

    virtual QwtDoubleRect boundingRect() const;

private:
    QwtArray<double> d_x;
    QwtArray<double> d_y;
};

/*!
  \brief Data class containing two pointers to memory blocks of doubles.
 */
class QWT_EXPORT QwtCPointerData: public QwtData
{
public:
    /*!
      Constructor
      
      \warning The programmer must assure that the memory blocks referenced
      by the pointers remain valid during the lifetime of the QwtPlotCPointer
      object.
      \sa QwtCurve::setRawData and QwtPlot::setCurveRawData.
     */
    QwtCPointerData(const double *x, const double *y, size_t size);
    QwtCPointerData &operator=(const QwtCPointerData &);
    virtual QwtData *copy() const;

    virtual size_t size() const;
    virtual double x(size_t i) const;
    virtual double y(size_t i) const;

    const double *xData() const;
    const double *yData() const;

    virtual QwtDoubleRect boundingRect() const;

private:
    const double *d_x;
    const double *d_y;
    size_t d_size;
};

#endif // !QWT_DATA
