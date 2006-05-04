/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#include <qpainter.h>
#include "qwt_global.h"
#include "qwt_curve.h"
#include "qwt_data.h"
#include "qwt_dimap.h"
#include "qwt_double_rect.h"
#include "qwt_math.h"
#include "qwt_painter.h"

/*!
  \brief Initialize data members
*/
void QwtCurve::init(const QString &title)
{
    d_pen = QPen(Qt::black, 1);
    d_ref = 0.0;
    d_splineSize = 250;
    d_options = Auto;
    d_title = title;
    d_style = Lines;
    d_data = new QwtDoublePointData(QwtArray<QwtDoublePoint>());
}

/*!
  \brief Copy the contents of a curve into another curve
*/
void QwtCurve::copy(const QwtCurve &c)
{
    d_ref = c.d_ref;
    d_sym = c.d_sym;
    d_pen = c.d_pen;
    d_title = c.d_title;
    d_style = c.d_style;

    d_splineSize = c.d_splineSize;
    d_options = c.d_options;

    delete d_data;
    d_data = c.d_data->copy();
}

//! Dtor
QwtCurve::~QwtCurve()
{
    delete d_data;
}

/*!
  \brief Copy Constructor
*/
QwtCurve::QwtCurve(const QwtCurve &c)
{
    init(c.d_title);
    copy(c);
}

/*!
  \brief Copy Assignment
*/
const QwtCurve& QwtCurve::operator=(const QwtCurve &c)
{
    if (this != &c)
    {
        copy(c);
        curveChanged();
    }

    return *this;
}

/*!
  \brief Set the curve's drawing style

  Valid styles are:
  <dl>
  <dt>QwtCurve::NoCurve</dt>
  <dd>Don't draw a curve. Note: This doesn't affect the symbol. </dd>
  <dt>QwtCurve::Lines</dt>
  <dd>Connect the points with straight lines.</dd>
  <dt>QwtCurve::Sticks</dt>
  <dd>Draw vertical sticks from a baseline which is defined by setBaseline().</dd>
  <dt>QwtCurve::Steps</dt>
  <dd>Connect the points with a step function. The step function
      is drawn from the left to the right or vice versa,
      depending on the 'Inverted' option.</dd>
  <dt>QwtCurves::Dots</dt>
  <dd>Draw dots at the locations of the data points. Note:
      This is different from a dotted line (see setPen()).</dd>
  <dt>QwtCurve::Spline</dt>
  <dd>Interpolate the points with a spline. The spline
      type can be specified with setOptions(),
      the size of the spline (= number of interpolated points)
      can be specified with setSplineSize().</dd>
  <dt>QwtCurve::UserCurve ...</dt>
  <dd>Styles >= QwtCurve::UserCurve are reserved for derived
      classes of QwtCurve that overload QwtCurve::draw() with
      additional application specific curve types.</dd>
  </dl>
  \sa QwtCurve::style()
*/
void QwtCurve::setStyle(int style, int options)
{
    d_options = options;
    d_style = style;
    curveChanged();
}

/*!
    \fn CurveStyle QwtCurve::style() const
    \brief Return the current style
    \sa QwtCurve::setStyle
*/
int QwtCurve::style() const 
{ 
    return d_style; 
}

/*!
  \brief Assign a symbol
  \param s symbol
  \sa QwtSymbol
*/
void QwtCurve::setSymbol(const QwtSymbol &s )
{
    d_sym = s;
    curveChanged();
}

/*!
    \brief Return the current symbol
    \sa QwtCurve::setSymbol
*/
const QwtSymbol &QwtCurve::symbol() const 
{ 
    return d_sym; 
}


/*!
  \brief Assign a pen
  \param p New pen
*/
void QwtCurve::setPen(const QPen &p)
{
    if ( p != d_pen )
    {
        d_pen = p;
        curveChanged();
    }
}

/*!
    \brief Return the pen used to draw the lines
    \sa QwtCurve::setPen
*/
const QPen& QwtCurve::pen() const 
{ 
    return d_pen; 
}

/*!
  \brief Assign a brush. 
         In case of brush.style() != QBrush::NoBrush 
         and style() != QwtCurve::Sticks
         the area between the curve and the baseline will be filled.
         In case !brush.color().isValid() the area will be filled by
         pen.color(). The fill algorithm simply connects the first and the
         last curve point to the baseline. So the curve data has to be sorted 
         (ascending or descending). 
  \param brush New brush
    \sa QwtCurve::brush, QwtCurve::setBaseline, QwtCurve::baseline
*/
void QwtCurve::setBrush(const QBrush &brush)
{
    if ( brush != d_brush )
    {
        d_brush = brush;
        curveChanged();
    }
}

/*!
  \brief Return the brush used to fill the area between lines and the baseline

  \sa QwtCurve::setBrush, QwtCurve::setBaseline, QwtCurve::baseline
*/
const QBrush& QwtCurve::brush() const 
{
    return d_brush;
}


/*!
  \brief Set data by copying x- and y-values from specified memory blocks
  Contrary to \b QwtPlot::setCurveRawData, this function makes a 'deep copy' of
  the data.

  \param xData pointer to x values
  \param yData pointer to y values
  \param size size of xData and yData

  \sa QwData::setData.
*/
void QwtCurve::setData(const double *xData, const double *yData, int size)
{
    delete d_data;
    d_data = new QwtArrayData(xData, yData, size);
    curveChanged();
}

/*!
  \brief Initialize data with x- and y-arrays (explicitly shared)

  \param xData x data
  \param yData y data

  \sa QwtData::setData.
*/
void QwtCurve::setData(const QwtArray<double> &xData, 
    const QwtArray<double> &yData)
{
    delete d_data;
    d_data = new QwtArrayData(xData, yData);
    curveChanged();
}

/*!
  Initialize data with an array of points (explicitly shared).

  \param data Data

  \sa QwtDoublePointData::setData.
*/
void QwtCurve::setData(const QwtArray<QwtDoublePoint> &data)
{
    delete d_data;
    d_data = new QwtDoublePointData(data);
    curveChanged();
}

/*!
  Initialize data with a pointer to QwtData.

  \param data Data

  \sa QwtData::copy.
*/
void QwtCurve::setData(const QwtData &data)
{
    delete d_data;
    d_data = data.copy();
    curveChanged();
}

/*!
  \brief Initialize the data by pointing to memory blocks which are not managed
  by QwtCurve.

  setRawData is provided for efficiency. It is important to keep the pointers
  during the lifetime of the underlying QwtCPointerData class.

  \param xData pointer to x data
  \param yData pointer to y data
  \param size size of x and y

  \sa QwtCPointerData::setData.
*/
void QwtCurve::setRawData(const double *xData, const double *yData, int size)
{
    delete d_data;
    d_data = new QwtCPointerData(xData, yData, size);
    curveChanged();
}

/*!
  \brief Assign a title to a curve
  \param title new title
*/
void QwtCurve::setTitle(const QString &title)
{
    d_title = title;
    curveChanged();
}

/*!
    \brief Return the title.
    \sa QwtCurve::setTitle
*/
const QString &QwtCurve::title() const 
{ 
    return d_title; 
}

/*!
  Returns the bounding rectangle of the curve data. If there is
  no bounding rect, like for empty data the rectangle is invalid:
  QwtDoubleRect.isValid() == FALSE
*/

QwtDoubleRect QwtCurve::boundingRect() const
{
    if ( d_data == NULL )
        return QwtDoubleRect(1.0, -1.0, 1.0, -1.0); // invalid

    return d_data->boundingRect();
}

/*!
  \brief Ctor
  \param title title of the curve   
*/
QwtCurve::QwtCurve(const QString &title)
{
    init(title);
}

/*!
  \brief Checks if a range of indices is valid and corrects it if necessary
  \param i1 Index 1
  \param i2 Index 2
*/
int QwtCurve::verifyRange(int &i1, int &i2)
{
    int size = dataSize();

    if (size < 1) return 0;

    i1 = qwtLim(i1, 0, size-1);
    i2 = qwtLim(i2, 0, size-1);
    qwtSort(i1, i2, i1, i2);

    return (i2 - i1 + 1);
}

/*!
  \brief Draw an intervall of the curve
  \param painter Painter
  \param xMap maps x-values into pixel coordinates.
  \param yMap maps y-values into pixel coordinates.
  \param from index of the first point to be painted
  \param to index of the last point to be painted. If to < 0 the 
         curve will be painted to its last point.

  \sa QwtCurve::drawCurve, QwtCurve::drawDots,
      QwtCurve::drawLines, QwtCurve::drawSpline,
      QwtCurve::drawSteps, QwtCurve::drawSticks
*/
void QwtCurve::draw(QPainter *painter,
    const QwtDiMap &xMap, const QwtDiMap &yMap, int from, int to)
{
    if ( !painter || dataSize() <= 0 )
        return;

    if (to < 0)
        to = dataSize() - 1;

    if ( verifyRange(from, to) > 0 )
    {
        painter->save();
        painter->setPen(d_pen);

        QBrush b = d_brush;
        if ( b.style() != Qt::NoBrush && !b.color().isValid() )
            b.setColor(d_pen.color());

        painter->setBrush(b);

        drawCurve(painter, d_style, xMap, yMap, from, to);
        painter->restore();

        if (d_sym.style() != QwtSymbol::None)
        {
            painter->save();
            drawSymbols(painter, d_sym, xMap, yMap, from, to);
            painter->restore();
        }
    }
}

/*!
  \brief Draw the line part (without symbols) of a curve interval. 
  \param painter Painter
  \param style curve style, see QwtCurve::CurveStyle
  \param xMap x map
  \param yMap y map
  \param from index of the first point to be painted
  \param to index of the last point to be painted
  \sa QwtCurve::draw, QwtCurve::drawDots, QwtCurve::drawLines,
      QwtCurve::drawSpline, QwtCurve::drawSteps, QwtCurve::drawSticks
*/

void QwtCurve::drawCurve(QPainter *painter, int style,
    const QwtDiMap &xMap, const QwtDiMap &yMap, int from, int to)
{
    switch (style)
    {
        case NoCurve:
            break;
        case Lines:
            drawLines(painter, xMap, yMap, from, to);
            break;
        case Sticks:
            drawSticks(painter, xMap, yMap, from, to);
            break;
        case Steps:
            drawSteps(painter, xMap, yMap, from, to);
            break;
        case Spline:
            if ( from > 0 || to < dataSize() - 1 )
                drawLines(painter, xMap, yMap, from, to);
            else
                drawSpline(painter, xMap, yMap);
            break;
        case Dots:
            drawDots(painter, xMap, yMap, from, to);
            break;
        default:
            break;
    }
}

/*!
  \brief Draw lines
  \param painter Painter
  \param xMap x map
  \param yMap y map
  \param from index of the first point to be painted
  \param to index of the last point to be painted
  \sa QwtCurve::draw, QwtCurve::drawLines, QwtCurve::drawDots, 
      QwtCurve::drawSpline, QwtCurve::drawSteps, QwtCurve::drawSticks
*/
void QwtCurve::drawLines(QPainter *painter,
    const QwtDiMap &xMap, const QwtDiMap &yMap, int from, int to)
{
    QPointArray polyline(to - from + 1);
    for (int i = from; i <= to; i++)
    {
        int xi = xMap.transform(x(i));
        int yi = yMap.transform(y(i));

        polyline.setPoint(i - from, xi, yi);
    }

    QwtPainter::drawPolyline(painter, polyline);

    if ( painter->brush().style() != Qt::NoBrush )
    {
        closePolyline(xMap, yMap, polyline);
        painter->setPen(QPen(Qt::NoPen));
        QwtPainter::drawPolygon(painter, polyline);
    }
}

/*!
  \brief Draw sticks
  \param painter Painter
  \param xMap x map
  \param yMap y map
  \param from index of the first point to be painted
  \param to index of the last point to be painted
  \sa QwtCurve::draw, QwtCurve::drawCurve, QwtCurve::drawDots, 
      QwtCurve::drawLines, QwtCurve::drawSpline, QwtCurve::drawSteps
*/
void QwtCurve::drawSticks(QPainter *painter,
    const QwtDiMap &xMap, const QwtDiMap &yMap, int from, int to)
{
    int x0 = xMap.transform(d_ref);
    int y0 = yMap.transform(d_ref);

    for (int i = from; i <= to; i++)
    {
        int xi = xMap.transform(x(i));
        int yi = yMap.transform(y(i));

        if (d_options & Xfy)
            QwtPainter::drawLine(painter, x0, yi, xi, yi);
        else
            QwtPainter::drawLine(painter, xi, y0, xi, yi);
    }
}

/*!
  \brief Draw dots
  \param painter Painter
  \param xMap x map
  \param yMap y map
  \param from index of the first point to be painted
  \param to index of the last point to be painted
  \sa QwtCurve::drawPolyline, QwtCurve::drawLine, 
      QwtCurve::drawLines, QwtCurve::drawSpline, QwtCurve::drawSteps
      QwtCurve::drawPolyline, QwtCurve::drawPolygon
*/
void QwtCurve::drawDots(QPainter *painter,
    const QwtDiMap &xMap, const QwtDiMap &yMap, int from, int to)
{
    const bool doFill = painter->brush().style() != Qt::NoBrush;

    QPointArray polyline;
    if ( doFill )
        polyline.resize(to - from + 1);

    for (int i = from; i <= to; i++)
    {
        int xi = xMap.transform(x(i));
        int yi = yMap.transform(y(i));
        QwtPainter::drawPoint(painter, xi, yi);

        if ( doFill )
            polyline.setPoint(i - from, xi, yi);
    }

    if ( doFill )
    {
        closePolyline(xMap, yMap, polyline);
        painter->setPen(QPen(Qt::NoPen));
        QwtPainter::drawPolygon(painter, polyline);
    }
}

/*!
  \brief Draw step function
  \param painter Painter
  \param xMap x map
  \param yMap y map
  \param from index of the first point to be painted
  \param to index of the last point to be painted
  \sa QwtCurve::draw, QwtCurve::drawCurve, QwtCurve::drawDots, 
      QwtCurve::drawLines, QwtCurve::drawSpline, QwtCurve::drawSticks
*/
void QwtCurve::drawSteps(QPainter *painter,
    const QwtDiMap &xMap, const QwtDiMap &yMap, int from, int to)
{
    QPointArray polyline(2 * (to - from) + 1);

    bool inverted = d_options & Yfx;
    if ( d_options & Inverted )
        inverted = !inverted;

    int i,ip;
    for (i = from, ip = 0; i <= to; i++, ip += 2)
    {
        int xi = xMap.transform(x(i));
        int yi = yMap.transform(y(i));

        if ( ip > 0 )
        {
            if (inverted)
                polyline.setPoint(ip - 1, polyline[ip-2].x(), yi);
            else
                polyline.setPoint(ip - 1, xi, polyline[ip-2].y());
        }

        polyline.setPoint(ip, xi, yi);
    }

    QwtPainter::drawPolyline(painter, polyline);

    if ( painter->brush().style() != Qt::NoBrush )
    {
        closePolyline(xMap, yMap, polyline);
        painter->setPen(QPen(Qt::NoPen));
        QwtPainter::drawPolygon(painter, polyline);
    }
}

/*!
  \brief Draw a spline
  \param painter Painter
  \param xMap x map
  \param yMap y map
  \sa QwtCurve::draw, QwtCurve::drawCurve, QwtCurve::drawDots,
      QwtCurve::drawLines, QwtCurve::drawSteps, QwtCurve::drawSticks
*/
void QwtCurve::drawSpline(QPainter *painter,
    const QwtDiMap &xMap, const QwtDiMap &yMap)
{
    register int i;

    int size = dataSize();
    double *txval = new double[size];
    double *tyval = new double[size];


    if ( !txval || !tyval )
    {
        if (txval) delete[] txval;
        if (tyval) delete[] tyval;
        return;
    }

    QPointArray polyline(d_splineSize);

    //
    // Transform x and y values to window coordinates
    // to avoid a distinction between linear and
    // logarithmic scales.
    //
    for (i=0;i<size;i++)
    {
        txval[i] = xMap.xTransform(x(i));
        tyval[i] = yMap.xTransform(y(i));
    }

    int stype;
    if (! (d_options & (Yfx|Xfy|Parametric)))
    {
        if (qwtChkMono(txval, size))
        {
            stype = Yfx;
        }
        else
        {
            if(qwtChkMono(tyval, size))
            {
                stype = Xfy;
            }
            else
            {
                stype = Parametric;
                if ( (d_options & Periodic) ||
                    ( (x(0) == x(size-1))
                    && (y(0) == y(size-1))))
                {
                    stype |= Periodic;
                }
            }
        }
    }
    else
    {
        stype = d_options;
    }

    if (stype & Parametric)
    {
        double *param = new double[size];
        if (param)
        {
            //
            // setup parameter vector
            //
            param[0] = 0.0;
            for (i=1; i<size; i++)
            {
                double delta = sqrt( qwtSqr(txval[i] - txval[i-1])
                              + qwtSqr( tyval[i] - tyval[i-1]));
                param[i] = param[i-1] + qwtMax(delta, 1.0);
            }

            //
            // setup splines
            int rc = d_spx.recalc(param, txval, size, stype & Periodic);
            if (!rc)
                rc = d_spy.recalc(param, tyval, size, stype & Periodic);

            if (rc)
            {
                drawLines(painter, xMap, yMap, 0, size - 1);
            }
            else
            {
                // fill point array
                double delta = param[size - 1] / double(d_splineSize-1);
                for (i=0;i<d_splineSize;i++)
                {
                    double dtmp = delta * double(i);
                    polyline.setPoint(i, int(floor (d_spx.value(dtmp) + 0.5)),
                                  int(floor (d_spy.value(dtmp) + 0.5)));
                }
            }

            delete[] param;
        }
    }
    else if (stype & Xfy)
    {
        if (tyval[size-1] < tyval[0])
        {
            qwtTwistArray(txval, size);
            qwtTwistArray(tyval, size);
        }

        // 1. Calculate spline coefficients
        int rc = d_spx.recalc(tyval, txval, size, stype & Periodic);
        if (rc)                         // an error occurred
        {
            drawLines(painter, xMap, yMap, 0, size - 1);
        }
        else                            // Spline OK
        {
            double ymin = qwtGetMin(tyval, size);
            double ymax = qwtGetMax(tyval, size);
            double delta = (ymax - ymin) / double(d_splineSize - 1);

            for (i=0;i<d_splineSize;i++)
            {
                double dtmp = ymin + delta * double(i);
                polyline.setPoint(i, int(floor(d_spx.value(dtmp) + 0.5)),
                              int(floor(dtmp + 0.5)));
            }
        }
    }
    else
    {
        if (txval[size-1] < txval[0])
        {
            qwtTwistArray(tyval, size);
            qwtTwistArray(txval, size);
        }


        // 1. Calculate spline coefficients
        int rc = d_spy.recalc(txval, tyval, size, stype & Periodic);
        if (rc)                         // error
        {
            drawLines(painter, xMap, yMap, 0, size - 1);
        }
        else                            // Spline OK
        {
            double xmin = qwtGetMin(txval, size);
            double xmax = qwtGetMax(txval, size);
            double delta = (xmax - xmin) / double(d_splineSize - 1);

            for (i=0;i<d_splineSize;i++)
            {
                double dtmp = xmin + delta * double(i);
                polyline.setPoint(i, int(floor (dtmp + 0.5)),
                              int(floor(d_spy.value(dtmp) + 0.5)));
            }
        }
    }

    delete[] txval;
    delete[] tyval;

    QwtPainter::drawPolyline(painter, polyline);

    if ( painter->brush().style() != Qt::NoBrush )
    {
        closePolyline(xMap, yMap, polyline);
        painter->setPen(QPen(Qt::NoPen));
        QwtPainter::drawPolygon(painter, polyline);
    }
}

/*!
  \brief Specify options for the drawing style  

  The options can be used to modify the drawing style.
  Options can be or-combined.
  The following options are defined:<dl>
  <dt>QwtCurve::Auto</dt>
  <dd>The default setting. For QwtCurve::spline,
      this means that the type of the spline is
      determined automatically, depending on the data.
      For all other styles, this means that y is
      regarded as a function of x.</dd>
  <dt>QwtCurve::Yfx</dt>
  <dd>Draws y as a function of x (the default). The
      baseline is interpreted as a horizontal line
      with y = baseline().</dd>
  <dt>QwtCurve::Xfy</dt>
  <dd>Draws x as a function of y. The baseline is
      interpreted as a vertical line with x = baseline().</dd>
  <dt>QwtCurve::Parametric</dt>
  <dd>For QwtCurve::Spline only. Draws a parametric spline.</dd>
  <dt>QwtCurve::Periodic</dt>
  <dd>For QwtCurve::Spline only. Draws a periodic spline.</dd>
  <dt>QwtCurve::Inverted</dt>
  <dd>For QwtCurve::Steps only. Draws a step function
      from the right to the left.</dd></dl>

  \param opt new options
  /sa QwtCurve::options()
*/
void QwtCurve::setOptions(int opt)
{
    d_options = opt;
    curveChanged();
}

/*!
    \brief Return the current style options
    \sa QwtCurve::setOptions
*/
int QwtCurve::options() const 
{ 
    return d_options; 
}

/*!
  \brief Change the number of interpolated points
  \param s new size
  \warning The default is 250 points.
*/
void QwtCurve::setSplineSize(int s)
{
    d_splineSize = qwtMax(s, 10);
    curveChanged();
}

/*!
    \fn int QwtCurve::splineSize() const
    \brief Return the spline size
    \sa QwtCurve::setSplineSize
*/

int QwtCurve::splineSize() const 
{ 
    return d_splineSize; 
}

/*!
  \brief Complete a polygon to be a closed polygon 
         including the area between the original polygon
         and the baseline.
  \param xMap X map
  \param yMap Y map
  \param pa Polygon to be completed
*/

void QwtCurve::closePolyline(const QwtDiMap &xMap, const QwtDiMap &yMap,
    QPointArray &pa) const
{
    const int sz = pa.size();
    if ( sz < 2 )
        return;

    pa.resize(sz + 2);

    if ( d_options & QwtCurve::Xfy )
    {
        pa.setPoint(sz,
            xMap.transform(d_ref), pa.point(sz - 1).y());
        pa.setPoint(sz + 1,
            xMap.transform(d_ref), pa.point(0).y());
    }
    else
    {
        pa.setPoint(sz,
            pa.point(sz - 1).x(), yMap.transform(d_ref));
        pa.setPoint(pa.size() - 1,
            pa.point(0).x(), yMap.transform(d_ref));
    }
}

/*!
  \brief Draw symbols
  \param painter Painter
  \param symbol Curve symbol
  \param xMap x map
  \param yMap y map
  \param from index of the first point to be painted
  \param to index of the last point to be painted
*/
void QwtCurve::drawSymbols(QPainter *painter, QwtSymbol &symbol,
    const QwtDiMap &xMap, const QwtDiMap &yMap, int from, int to)
{
    painter->setBrush(symbol.brush());
    painter->setPen(symbol.pen());

    QRect rect;
    rect.setSize(QwtPainter::metricsMap().screenToLayout(symbol.size()));

    for (int i = from; i <= to; i++)
    {
        const int xi = xMap.transform(x(i));
        const int yi = yMap.transform(y(i));

        rect.moveCenter(QPoint(xi, yi));
        symbol.draw(painter, rect);
    }
}

/*!
  \brief Set the value of the baseline

  The baseline is needed for filling the curve with a brush or
  the QwtCurve::Sticks drawing style. 
  The default value is 0.0. The interpretation
  of the baseline depends on the style options. With QwtCurve::Yfx,
  the baseline is interpreted as a horizontal line at y = baseline(),
  with QwtCurve::Yfy, it is interpreted as a vertical line at
  x = baseline().
  \param ref baseline
  \sa QwtCurve::setBrush(), QwtCurve::setStyle(), QwtCurve::setOptions()
*/
void QwtCurve::setBaseline(double ref)
{
    d_ref = ref;
    curveChanged();
}

/*!
    \brief Return the value of the baseline
    \sa QwtCurve::setBaseline
*/
double QwtCurve::baseline() const 
{ 
    return d_ref; 
}

/*!
  Return the size of the data arrays
*/
int QwtCurve::dataSize() const
{
    return d_data->size();
}

/*!
    \brief Notify a change of attributes.
    This virtual function is called when an attribute of the curve
    has changed. It can be redefined by derived classes.
    The default implementation does nothing.
*/

void QwtCurve::curveChanged() 
{
}
