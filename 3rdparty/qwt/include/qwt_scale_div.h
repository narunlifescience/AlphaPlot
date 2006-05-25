/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#ifndef QWT_SCALE_DIV_H
#define QWT_SCALE_DIV_H

#include "qwt_global.h"

class QwtDoubleInterval;

#if QT_VERSION < 0x040000

#include <qvaluelist.h>

#if defined(QWT_TEMPLATEDLL)
// MOC_SKIP_BEGIN
template class QWT_EXPORT QValueList<double>;
// MOC_SKIP_END
#endif

typedef QValueList<double> QwtTickList;

#else

#include <qlist.h>

#if defined(QWT_TEMPLATEDLL)

#if defined Q_CC_MSVC  // Q_CC_MSVC_NET
// Some definitions, needed to avoid a MSVC crash
#include <qset.h>
#include <qvector.h>
inline uint qHash(double key) { return uint(key); }
#endif

// MOC_SKIP_BEGIN
template class QWT_EXPORT QList<double>;
// MOC_SKIP_END
#endif

typedef QList<double> QwtTickList;

#endif


/*!
  \brief A class representing a scale division

  A scale division consists of its limits and 3 list
  of tick values qualified as major, medium and minor ticks.

  In most cases scale divisions are calculated by a QwtScaleEngine.

  \sa QwtScaleEngine::subDivideInto, QwtScaleEngine::subDivide
*/

class QWT_EXPORT QwtScaleDiv
{
public:
    enum TickType
    {
        NoTick = -1,

        MinorTick,
        MediumTick,
        MajorTick,

        NTickTypes
    };

    explicit QwtScaleDiv();
    explicit QwtScaleDiv(const QwtDoubleInterval &,
        QwtTickList[NTickTypes]);
    explicit QwtScaleDiv(double lBound, double rBound,
        QwtTickList[NTickTypes]);

    int operator==(const QwtScaleDiv &s) const;
    int operator!=(const QwtScaleDiv &s) const;
    
    inline double lBound() const;
    inline double hBound() const;
    inline double range() const;

    bool contains(double v) const;

    const QwtTickList &ticks(int type) const;

    void invalidate();
    bool isValid() const;
 
    void invert();

private:
    double d_lBound;
    double d_hBound;
    QwtTickList d_ticks[NTickTypes];

    bool d_isValid;
};

/*! 
  \return left bound
  \sa QwtScaleDiv::hBound
*/
inline double QwtScaleDiv::lBound() const 
{ 
    return d_lBound;
}

/*! 
  \return right bound
  \sa QwtScaleDiv::lBound
*/
inline double QwtScaleDiv::hBound() const 
{ 
    return d_hBound;
}

/*! 
  \return hBound() - lBound()
*/
inline double QwtScaleDiv::range() const 
{ 
    return d_hBound - d_lBound;
}
#endif
