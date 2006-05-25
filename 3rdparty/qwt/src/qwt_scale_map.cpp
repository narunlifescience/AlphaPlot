/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#include "qwt_scale_map.h"

QT_STATIC_CONST_IMPL double QwtScaleMap::LogMin = 1.0e-150;
QT_STATIC_CONST_IMPL double QwtScaleMap::LogMax = 1.0e150;

QwtScaleTransformation::QwtScaleTransformation():
    xForm(linearXForm),
    invXForm(linearXForm)
{
}

QwtScaleTransformation::QwtScaleTransformation(
        double (*xf)(double x, double s1, double s2, 
            double p1, double p2, void *),
        double (*invxf)(double y, double p1, double p2, 
            double s1, double s2, void *) ):
    xForm(xf),
    invXForm(invxf)
{
}

/*!
  \brief Transform a value between 2 linear intervals

  \param x value related to the interval [x1, x2]
  \param x1 first border of source interval
  \param x2 first border of source interval
  \param y1 first border of target interval
  \param y2 first border of target interval
  \return 
  <dl>
  <dt>linear mapping:<dd>y1 + (y2 - y1) / (x2 - x1) * (x - x1)
  </dl>
*/

double QwtScaleTransformation::linearXForm(    
    double x, double x1, double x2, double y1, double y2, void *)
{
    const double ratio = (y2 - y1) / (x2 - x1);
    return y1 + (x - x1) * ratio;
}

/*!
  \brief Transform a value from a logarithmic to a linear interval

  \param x value related to the logarithmic interval [s1, s2]
  \param s1 first border of logarithmic interval
  \param s2 first border of logarithmic interval
  \param p1 first border of linear interval
  \param p2 first border of linear interval
  \return 
  <dl>
  <dt>p1 + (p2 - p1) / log(s2 / s1) * log(x / s1)
  </dl>
*/


double QwtScaleTransformation::log10XForm(double x, double s1, double s2, 
    double p1, double p2, void *)
{
    return p1 + (p2 - p1) / log(s2 / s1) * log(x / s1);
}

/*!
  \brief Transform a value from a linear to a logarithmic interval

  \param x value related to the linear interval [p1, p2]
  \param p1 first border of linear interval
  \param p2 first border of linear interval
  \param s1 first border of logarithmic interval
  \param s2 first border of logarithmic interval
  \return 
  <dl>
  <dt>exp((x - p1) / (p2 - p1) * log(s2 / s1)) * s1;
  </dl>
*/

double QwtScaleTransformation::log10InvXForm(double x, double p1, double p2, 
    double s1, double s2, void *)
{
    return exp((x - p1) / (p2 - p1) * log(s2 / s1)) * s1;
}

/*!
  \brief Constructor

  The scale and paint device intervals are both set to [0,1].
*/
QwtScaleMap::QwtScaleMap():
    d_s1(0.0),
    d_s2(1.0),
    d_p1(0.0),
    d_p2(1.0),
    d_cnv(1.0),
    d_transformationData(NULL)
{
}


/*!
  \brief Constructor

  Constructs a QwtScaleMap instance with initial paint device
  and scale intervals

  \param p1 first border of paint device interval
  \param p2 second border of paint device interval
  \param s1 first border of scale interval
  \param s2 second border of scale interval
*/ 
QwtScaleMap::QwtScaleMap(int p1, int p2, double s1, double s2):
    d_p1(p1),
    d_p2(p2),
    d_transformationData(NULL)
{
    setScaleInterval(s1, s2);
}

/*!
  Destructor
*/
QwtScaleMap::~QwtScaleMap()
{
}

/*!
   Initialize the map with a linear/logarithmic transformation
*/
void QwtScaleMap::setTransformation(bool logarithmic)
{
    QwtScaleTransformation transformation;
    if ( logarithmic )
    {
        transformation.xForm = QwtScaleTransformation::log10XForm;
        transformation.invXForm = QwtScaleTransformation::log10InvXForm;
    }
    else
    {
        transformation.xForm = QwtScaleTransformation::linearXForm;
        transformation.invXForm = QwtScaleTransformation::linearXForm;
    }
    setTransformation(transformation);
}

/*!
   Initialize the map with a transformation
*/
void QwtScaleMap::setTransformation(
    const QwtScaleTransformation &transformation)
{
    d_transformation = transformation;
}

//! Get the transformation
const QwtScaleTransformation &QwtScaleMap::transformation() const
{
    return d_transformation;
}

/*!
   Add data, that will passed to the transformations

   \warning The transformationData has to be deleted by the application
*/
void QwtScaleMap::setTransformationData(void *transformationData)
{
    d_transformationData = transformationData;
}

//! Get the data, that is passed to the transformations
void *QwtScaleMap::transformationData() const
{
    return d_transformationData;
}

/*!
  \brief Specify the borders of the scale interval
  \param s1 first border
  \param s2 second border 
  \warning logarithmic scales might be aligned to [LogMin, LogMax]
*/
void QwtScaleMap::setScaleInterval(double s1, double s2)
{
    if (d_transformation.xForm == QwtScaleTransformation::log10XForm)
    {
        if (s1 < LogMin) 
           s1 = LogMin;
        else if (s1 > LogMax) 
           s1 = LogMax;
        
        if (s2 < LogMin) 
           s2 = LogMin;
        else if (s2 > LogMax) 
           s2 = LogMax;
    }

    d_s1 = s1;
    d_s2 = s2;

    newFactor();
}

/*!
  \brief Specify the borders of the paint device interval
  \param p1 first border
  \param p2 second border
*/
void QwtScaleMap::setPaintInterval(int p1, int p2)
{
    d_p1 = p1;
    d_p2 = p2;
    newFactor();
}

/*!
  \brief Specify the borders of the paint device interval
  \param p1 first border
  \param p2 second border
*/
void QwtScaleMap::setPaintXInterval(double p1, double p2)
{
    d_p1 = p1;
    d_p2 = p2;
    newFactor();
}

/*!
  \brief Re-calculate the conversion factor.
*/
void QwtScaleMap::newFactor()
{
    d_cnv = 0.0;
#if 1
    if (d_s2 == d_s1)
        return;
#endif

    if (d_transformation.xForm == QwtScaleTransformation::linearXForm)
        d_cnv = (d_p2 - d_p1) / (d_s2 - d_s1); 
    else if (d_transformation.xForm == QwtScaleTransformation::log10XForm)
        d_cnv = (d_p2 - d_p1) / log(d_s2 / d_s1);
}
