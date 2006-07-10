/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#ifndef QWT_SCALE_MAP_H
#define QWT_SCALE_MAP_H

#include "qwt_global.h"
#include "qwt_math.h"

/*!
   Operations for linear or logarithmic (base 10) transformations
*/
class QWT_EXPORT QwtScaleTransformation
{
public:
    QwtScaleTransformation();
    QwtScaleTransformation(
        double (*xForm)(double x, double s1, double s2, 
            double p1, double p2, void *),
        double (*invXForm)(double x, double p1, double p2, 
            double s1, double s2, void *)
    );

    static double linearXForm(double x, double s1, double s2,
        double p1, double p2, void *);

    static double log10XForm(double x, double s1, double s2,
        double p1, double p2, void *);
    static double log10InvXForm(double x, double p1, double p2,
        double s1, double s2, void *);

    double (*xForm)(double x, double s1, double s2, 
        double p1, double p2, void *);
    double (*invXForm)(double y, double p1, double p2, 
        double s1, double s2, void *);

    inline void setData(void *);
    inline void *data() const;

private:
    void *d_data;  // data, passed to xForm/invXForm
};

/*!
   \brief A scale map

   QwtScaleMap offers transformations from a scale 
   into a paint interval and vice versa.
*/
class QWT_EXPORT QwtScaleMap
{
public:
    QwtScaleMap();
    QwtScaleMap(int i1, int i2, double d1, double d2);
    ~QwtScaleMap();

    void setTransformation(bool logarithmic);
    void setTransformation(const QwtScaleTransformation& );
    const QwtScaleTransformation &transformation() const;

    void setPaintInterval(int p1, int p2);
    void setPaintXInterval(double p1, double p2);
    void setScaleInterval(double s1, double s2);

    int transform(double x) const;
    double invTransform(double i) const;

    double xTransform(double x) const;

    inline double p1() const;
    inline double p2() const;

    inline double s1() const;
    inline double s2() const;

    inline double pDist() const;
    inline double sDist() const;

    QT_STATIC_CONST double LogMin;
    QT_STATIC_CONST double LogMax;

private:
    void newFactor();   

    double d_s1, d_s2;     // scale interval boundaries
    double d_p1, d_p2;     // paint device interval boundaries

    double d_cnv;       // conversion factor

    QwtScaleTransformation d_transformation;
};

/*!
   Add data, that will passed to xForm/invXForm
   \warning The data has to be deleted by the application
*/
inline void QwtScaleTransformation::setData(void *data)
{
    d_data = data;
}

//! Get the data, that is passed to xForm/invXForm
inline void *QwtScaleTransformation::data() const
{
    return d_data;
}

/*!
    \return First border of the scale interval
*/
inline double QwtScaleMap::s1() const 
{
    return d_s1;
}

/*!
    \return Second border of the scale interval
*/
inline double QwtScaleMap::s2() const 
{
    return d_s2;
}

/*!
    \return First border of the paint interval
*/
inline double QwtScaleMap::p1() const 
{
    return d_p1;
}

/*!
    \return Second border of the paint interval
*/
inline double QwtScaleMap::p2() const 
{
    return d_p2;
}

inline double QwtScaleMap::pDist() const
{
    return qwtAbs(d_p2 - d_p1);
}

inline double QwtScaleMap::sDist() const
{
    return qwtAbs(d_s2 - d_s1);
}

/*!
  Transform a point related to the scale interval into an point 
  related to the interval of the paint device
*/
inline double QwtScaleMap::xTransform(double x) const
{
    // try to inline code from QwtScaleTransformation::linearXForm,
    // QwtScaleTransformation::logXForm. 

    if ( d_transformation.xForm == QwtScaleTransformation::linearXForm )
        return d_p1 + (x - d_s1) * d_cnv;

    if ( d_transformation.xForm == QwtScaleTransformation::log10XForm )
        return d_p1 + log(x / d_s1) * d_cnv;

    return (*d_transformation.xForm)(x, d_s1, d_s2, 
        d_p1, d_p2, d_transformation.data() );
}

/*!
  \brief Transform an paint device value into a value in the
         interval of the scale.
*/
inline double QwtScaleMap::invTransform(double y) const
{
    return (*d_transformation.invXForm)(y, d_p1, d_p2, 
        d_s1, d_s2, d_transformation.data() );
}

/*!
  Transform a point related to the scale interval into an point 
  related to the interval of the paint device and round it to
  an integer. (In Qt <= 3.x paint devices are integer based. )

  \sa QwtScaleMap::xTransform
*/
inline int QwtScaleMap::transform(double x) const
{
    return qRound(xTransform(x));
}

#endif
