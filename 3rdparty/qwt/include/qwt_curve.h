/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#ifndef QWT_CURVE_H
#define QWT_CURVE_H

#include <qpen.h>
#include <qstring.h>
#include "qwt_global.h"
#include "qwt_array.h"
#include "qwt_data.h"
#include "qwt_spline.h"
#include "qwt_symbol.h"

class QPainter;
class QwtDiMap;

#if defined(QWT_TEMPLATEDLL)
// MOC_SKIP_BEGIN
template class QWT_EXPORT QwtArray<double>;
// MOC_SKIP_END
#endif

/*!
  \brief A class which draws curves

  This class can be used to display data as a curve in the  x-y plane.
  It supports different display styles, spline interpolation and symbols.

  \par Usage
  <dl><dt>A. Assign curve properties</dt>
  <dd>When a curve is created, it is configured to draw black solid lines
  with QwtCurve::Lines and no symbols. You can change this by calling 
  QwtCurve::setPen(), QwtCurve::setStyle() and QwtCurve::setSymbol().</dd>
  <dt>B. Assign or change data.</dt>
  <dd>Data can be set in two ways:<ul>
  <li>QwtCurve::setData() is overloaded to initialize the x and y data by
  copying from different data structures with different kind of copy semantics.
  <li>QwtCurve::setRawData() only stores the pointers and size information
  and is provided for backwards compatibility.  This function is less safe (you
  must not delete the data while they are attached), but has been more
  efficient, and has been more convenient for dynamically changing data.
  Use of QwtCurve::setData() in combination with a problem-specific subclass
  of QwtData is always preferrable.</ul></dd>
  <dt>C. Draw</dt>
  <dd>QwtCurve::draw() maps the data into pixel coordinates and paints them.
  </dd></dl>

  \par Example:
  see examples/curvdemo

  \sa QwtData, QwtSymbol, QwtDiMap
*/
class QWT_EXPORT QwtCurve
{
public:
    /*! 
        Curve styles. 
        \sa QwtCurve::setStyle
    */
    enum CurveStyle
    {
        NoCurve,
        Lines,
        Sticks,
        Steps,
        Dots,
        Spline,
        UserCurve = 100
    };

    /*! 
        Curve options. 
        \sa QwtCurve::setOptions
    */
    enum CurveOption
    {
        Auto = 0,
        Yfx = 1,
        Xfy = 2,
        Parametric = 4,
        Periodic = 8,
        Inverted = 16
    };

    QwtCurve(const QString &title = QString::null);
    QwtCurve(const QwtCurve &c);
    virtual ~QwtCurve();

    const QwtCurve& operator= (const QwtCurve &c);

    void setRawData(const double *x, const double *y, int size);
    void setData(const double *xData, const double *yData, int size);
    void setData(const QwtArray<double> &xData, const QwtArray<double> &yData);
    void setData(const QwtArray<QwtDoublePoint> &data);
    void setData(const QwtData &data);
    
    int dataSize() const;
    inline double x(int i) const;
    inline double y(int i) const;

    virtual QwtDoubleRect boundingRect() const;

    //! boundingRect().x1()
    inline double minXValue() const { return boundingRect().x1(); }
    //! boundingRect().x2()
    inline double maxXValue() const { return boundingRect().x2(); }
    //! boundingRect().y1()
    inline double minYValue() const { return boundingRect().y1(); }
    //! boundingRect().y2()
    inline double maxYValue() const { return boundingRect().y2(); }

    void setOptions(int t);
    int options() const;

    void setTitle(const QString &title);
    const QString &title() const;

    void setPen(const QPen &);
    const QPen &pen() const;

    void setBrush(const QBrush &);
    const QBrush &brush() const;

    void setBaseline(double ref);
    double baseline() const;

    void setStyle(int style, int options = 0);
    int style() const;

    void setSymbol(const QwtSymbol &s);
    const QwtSymbol& symbol() const;

    void setSplineSize(int s);
    int splineSize() const;

    virtual void draw(QPainter *p, const QwtDiMap &xMap, const QwtDiMap &yMap,
        int from = 0, int to = -1);

protected:

    void init(const QString &title);
    void copy(const QwtCurve &c);

    virtual void drawCurve(QPainter *p, int style,
        const QwtDiMap &xMap, const QwtDiMap &yMap,
        int from, int to);

    virtual void drawSymbols(QPainter *p, QwtSymbol &,
        const QwtDiMap &xMap, const QwtDiMap &yMap,
        int from, int to);

    void drawLines(QPainter *p,
        const QwtDiMap &xMap, const QwtDiMap &yMap,
        int from, int to);
    void drawSticks(QPainter *p,
        const QwtDiMap &xMap, const QwtDiMap &yMap,
        int from, int to);
    void drawDots(QPainter *p,
        const QwtDiMap &xMap, const QwtDiMap &yMap,
        int from, int to);
    void drawSteps(QPainter *p,
        const QwtDiMap &xMap, const QwtDiMap &yMap,
        int from, int to);
    void drawSpline(QPainter *p,
        const QwtDiMap &xMap, const QwtDiMap &yMap);

    void closePolyline(const QwtDiMap &, const QwtDiMap &,
        QPointArray &) const;

    virtual void curveChanged();

    int verifyRange(int &i1, int &i2);

protected:
    QwtSpline d_spx;
    QwtSpline d_spy;

private:
    QwtData *d_data;

    int d_style;
    double d_ref;

    QwtSymbol d_sym;

    QPen d_pen;
    QBrush d_brush;
    QString d_title;

    int d_options;
    int d_splineSize;
};

/*!
    \param i index
    \return x-value at position i
*/
inline double QwtCurve::x(int i) const 
{ 
    return d_data->x(i); 
}

/*!
    \param i index
    \return y-value at position i
*/
inline double QwtCurve::y(int i) const 
{ 
    return d_data->y(i); 
}

#endif
