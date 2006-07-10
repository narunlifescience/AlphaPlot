/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#include <qpainter.h>
#include <qpixmap.h>
#include <qbitarray.h>
#include "qwt_global.h"
#include "qwt_legend.h"
#include "qwt_legend_item.h"
#include "qwt_data.h"
#include "qwt_rect.h"
#include "qwt_scale_map.h"
#include "qwt_double_rect.h"
#include "qwt_math.h"
#include "qwt_painter.h"
#include "qwt_plot.h"
#include "qwt_plot_canvas.h"
#include "qwt_spline.h"
#include "qwt_symbol.h"
#include "qwt_plot_curve.h"

#if QT_VERSION >= 0x040000

#include <qevent.h>

class QwtPlotCurvePaintHelper: public QObject
{
public:
    QwtPlotCurvePaintHelper(const QwtPlotCurve *curve, int from, int to):
        _curve(curve),
        _from(from),
        _to(to)
    {
    }

    virtual bool eventFilter(QObject *, QEvent *event)
    {
        if ( event->type() == QEvent::Paint )
        {
            _curve->draw(_from, _to);
            return true;
        }
        return false;
    }
private:
    const QwtPlotCurve *_curve;
    int _from;
    int _to;
};

#endif // QT_VERSION >= 0x040000

class QwtPlotCurve::PrivateData
{
public:
    class PixelMatrix: private QBitArray
    {
    public:
        PixelMatrix(const QRect& rect):
            QBitArray(rect.width() * rect.height()),
            _rect(rect)
        {
            fill(false);
        }

        inline bool testPixel(const QPoint& pos)
        {
            if ( !_rect.contains(pos) )
                return false;

            const int idx = _rect.width() * (pos.y() - _rect.y()) + 
                (pos.x() - _rect.x());

            const bool marked = testBit(idx);
            if ( !marked )
                setBit(idx, true);

            return !marked;
        }

    private:
        QRect _rect;
    };

    PrivateData()
    {
        pen = QPen(Qt::black, 0);
        reference = 0.0;
        splineSize = 250;
        attributes = Auto;
        paintAttributes = 0;
        style = QwtPlotCurve::Lines;
    }

    QwtPlotCurve::CurveStyle style;
    double reference;

    QwtSymbol sym;

    QPen pen;
    QBrush brush;

    int attributes;
    int splineSize;
    int paintAttributes;
};

static int qwtChkMono(const double *array, int size)
{
    if (size < 2)
        return 0;

    int rv = qwtSign(array[1] - array[0]);
    for (int i = 1; i < size - 1; i++)
    {
        if ( qwtSign(array[i+1] - array[i]) != rv )
        {
            rv = 0;
            break;
        }
    }
    return rv;
}

static void qwtTwistArray(double *array, int size)
{
    const int s2 = size / 2;

    for (int i=0; i < s2; i++)
    {
        const int itmp = size - 1 - i;
        const double dtmp = array[i];
        array[i] = array[itmp];
        array[itmp] = dtmp;
    }
}

/*!
  \brief Ctor
*/
QwtPlotCurve::QwtPlotCurve():
    QwtPlotItem(QwtText())
{
    init();
}

/*!
  \brief Ctor
  \param title title of the curve   
*/
QwtPlotCurve::QwtPlotCurve(const QwtText &title):
    QwtPlotItem(title)
{
    init();
}

/*!
  \brief Ctor
  \param title title of the curve   
*/
QwtPlotCurve::QwtPlotCurve(const QString &title):
    QwtPlotItem(QwtText(title))
{
    init();
}

//! Dtor
QwtPlotCurve::~QwtPlotCurve()
{
    delete d_xy;
    delete d_data;
}

/*!
  \brief Initialize data members
*/
void QwtPlotCurve::init()
{
    setItemAttribute(QwtPlotItem::Legend);
    setItemAttribute(QwtPlotItem::AutoScale);

    d_data = new PrivateData;
    d_xy = new QwtDoublePointData(QwtArray<QwtDoublePoint>());

    setZ(20.0);
}

int QwtPlotCurve::rtti() const
{
    return QwtPlotItem::Rtti_PlotCurve;
}

/*!
  \brief Specify an attribute how to draw the curve

  The attributes can be used to modify the drawing algorithm.

  The following attributes are defined:<dl>
  <dt>QwtPlotCurve::PaintFiltered</dt>
  <dd>Tries to reduce the data that has to be painted, by sorting out
      duplicates, or paintings outside the visible area. Might have a
      notable impact on curves with many close points. 
      Only a couple of very basic filtering algos are implemented.</dd>
  <dt>QwtPlotCurve::ClipPolygons</dt>
  <dd>Clip polygons before painting them.
  </dl>

  The default is, that no paint attributes are enabled.

  \param attribute Paint attribute
  \param on On/Off
  /sa QwtPlotCurve::testPaintAttribute()
*/
void QwtPlotCurve::setPaintAttribute(PaintAttribute attribute, bool on)
{
    if ( on )
        d_data->paintAttributes |= attribute;
    else
        d_data->paintAttributes &= ~attribute;
}

/*!
    \brief Return the current paint attributes
    \sa QwtPlotCurve::setPaintAttribute
*/
bool QwtPlotCurve::testPaintAttribute(PaintAttribute attribute) const
{
    return (d_data->paintAttributes & attribute);
}

/*!
  \brief Set the curve's drawing style

  Valid styles are:
  <dl>
  <dt>QwtPlotCurve::NoCurve</dt>
  <dd>Don't draw a curve. Note: This doesn't affect the symbol. </dd>
  <dt>QwtPlotCurve::Lines</dt>
  <dd>Connect the points with straight lines.</dd>
  <dt>QwtPlotCurve::Sticks</dt>
  <dd>Draw vertical sticks from a baseline which is defined by setBaseline().</dd>
  <dt>QwtPlotCurve::Steps</dt>
  <dd>Connect the points with a step function. The step function
      is drawn from the left to the right or vice versa,
      depending on the 'Inverted' option.</dd>
  <dt>QwtPlotCurves::Dots</dt>
  <dd>Draw dots at the locations of the data points. Note:
      This is different from a dotted line (see setPen()).</dd>
  <dt>QwtPlotCurve::Spline</dt>
  <dd>Interpolate the points with a spline. The spline
      type can be specified with setCurveAttribute(),
      the size of the spline (= number of interpolated points)
      can be specified with setSplineSize().</dd>
  <dt>QwtPlotCurve::UserCurve ...</dt>
  <dd>Styles >= QwtPlotCurve::UserCurve are reserved for derived
      classes of QwtPlotCurve that overload QwtPlotCurve::draw() with
      additional application specific curve types.</dd>
  </dl>
  \sa QwtPlotCurve::style()
*/
void QwtPlotCurve::setStyle(CurveStyle style)
{
    if ( style != d_data->style )
    {
        d_data->style = style;
        itemChanged();
    }
}

/*!
    \fn CurveStyle QwtPlotCurve::style() const
    \brief Return the current style
    \sa QwtPlotCurve::setStyle
*/
QwtPlotCurve::CurveStyle QwtPlotCurve::style() const 
{ 
    return d_data->style; 
}

/*!
  \brief Assign a symbol
  \param s symbol
  \sa QwtSymbol
*/
void QwtPlotCurve::setSymbol(const QwtSymbol &s )
{
    d_data->sym = s;
    itemChanged();
}

/*!
    \brief Return the current symbol
    \sa QwtPlotCurve::setSymbol
*/
const QwtSymbol &QwtPlotCurve::symbol() const 
{ 
    return d_data->sym; 
}

/*!
  \brief Assign a pen
  \param p New pen
*/
void QwtPlotCurve::setPen(const QPen &p)
{
    if ( p != d_data->pen )
    {
        d_data->pen = p;
        itemChanged();
    }
}

/*!
    \brief Return the pen used to draw the lines
    \sa QwtPlotCurve::setPen
*/
const QPen& QwtPlotCurve::pen() const 
{ 
    return d_data->pen; 
}

/*!
  \brief Assign a brush. 
         In case of brush.style() != QBrush::NoBrush 
         and style() != QwtPlotCurve::Sticks
         the area between the curve and the baseline will be filled.
         In case !brush.color().isValid() the area will be filled by
         pen.color(). The fill algorithm simply connects the first and the
         last curve point to the baseline. So the curve data has to be sorted 
         (ascending or descending). 
  \param brush New brush
    \sa QwtPlotCurve::brush, QwtPlotCurve::setBaseline, QwtPlotCurve::baseline
*/
void QwtPlotCurve::setBrush(const QBrush &brush)
{
    if ( brush != d_data->brush )
    {
        d_data->brush = brush;
        itemChanged();
    }
}

/*!
  \brief Return the brush used to fill the area between lines and the baseline

  \sa QwtPlotCurve::setBrush, QwtPlotCurve::setBaseline, QwtPlotCurve::baseline
*/
const QBrush& QwtPlotCurve::brush() const 
{
    return d_data->brush;
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
void QwtPlotCurve::setData(const double *xData, const double *yData, int size)
{
    delete d_xy;
    d_xy = new QwtArrayData(xData, yData, size);
    itemChanged();
}

/*!
  \brief Initialize data with x- and y-arrays (explicitly shared)

  \param xData x data
  \param yData y data

  \sa QwtData::setData.
*/
void QwtPlotCurve::setData(const QwtArray<double> &xData, 
    const QwtArray<double> &yData)
{
    delete d_xy;
    d_xy = new QwtArrayData(xData, yData);
    itemChanged();
}

/*!
  Initialize data with an array of points (explicitly shared).

  \param data Data

  \sa QwtDoublePointData::setData.
*/
void QwtPlotCurve::setData(const QwtArray<QwtDoublePoint> &data)
{
    delete d_xy;
    d_xy = new QwtDoublePointData(data);
    itemChanged();
}

/*!
  Initialize data with a pointer to QwtData.

  \param data Data

  \sa QwtData::copy.
*/
void QwtPlotCurve::setData(const QwtData &data)
{
    delete d_xy;
    d_xy = data.copy();
    itemChanged();
}

/*!
  \brief Initialize the data by pointing to memory blocks which are not managed
  by QwtPlotCurve.

  setRawData is provided for efficiency. It is important to keep the pointers
  during the lifetime of the underlying QwtCPointerData class.

  \param xData pointer to x data
  \param yData pointer to y data
  \param size size of x and y

  \sa QwtCPointerData::setData.
*/
void QwtPlotCurve::setRawData(const double *xData, const double *yData, int size)
{
    delete d_xy;
    d_xy = new QwtCPointerData(xData, yData, size);
    itemChanged();
}

/*!
  Returns the bounding rectangle of the curve data. If there is
  no bounding rect, like for empty data the rectangle is invalid:
  QwtDoubleRect.isValid() == false
*/

QwtDoubleRect QwtPlotCurve::boundingRect() const
{
    if ( d_xy == NULL )
        return QwtDoubleRect(1.0, 1.0, -2.0, -2.0); // invalid

    return d_xy->boundingRect();
}


/*!
  \brief Checks if a range of indices is valid and corrects it if necessary
  \param i1 Index 1
  \param i2 Index 2
*/
int QwtPlotCurve::verifyRange(int &i1, int &i2) const
{
    int size = dataSize();

    if (size < 1) return 0;

    i1 = qwtLim(i1, 0, size-1);
    i2 = qwtLim(i2, 0, size-1);

    if ( i1 > i2 )
        qSwap(i1, i2);

    return (i2 - i1 + 1);
}

void QwtPlotCurve::draw(QPainter *painter,
    const QwtScaleMap &xMap, const QwtScaleMap &yMap,
    const QRect &) const
{
    draw(painter, xMap, yMap, 0, -1);
}

/*!
  \brief Draw a set of points of a curve.

  When observing an measurement while it is running, new points have to be
  added to an existing curve. drawCurve can be used to display them avoiding
  a complete redraw of the canvas.

  Setting plot()->canvas()->setAttribute(Qt::WA_PaintOutsidePaintEvent, true);
  will result in faster painting, if the paint engine of the canvas widget
  supports this feature. 

  \param from Index of the first point to be painted
  \param to Index of the last point to be painted. If to < 0 the
         curve will be painted to its last point.

  \sa QwtCurve::draw
*/
void QwtPlotCurve::draw(int from, int to) const
{
    if ( !plot() )
        return;

    QwtPlotCanvas *canvas = plot()->canvas();

    bool directPaint = true;

#if QT_VERSION >= 0x040000
    if ( !canvas->testAttribute(Qt::WA_WState_InPaintEvent) &&
        !canvas->testAttribute(Qt::WA_PaintOutsidePaintEvent) )
    {
        /*
          We save curve and range in helper and call repaint.
          The helper filters the Paint event, to repeat
          the QwtPlotCurve::draw, but now from inside the paint
          event.
         */

        QwtPlotCurvePaintHelper helper(this, from, to);
        canvas->installEventFilter(&helper);
        canvas->repaint();

        return;
    }
#endif

    const QwtScaleMap xMap = plot()->canvasMap(xAxis());
    const QwtScaleMap yMap = plot()->canvasMap(yAxis());

    if ( canvas->testPaintAttribute(QwtPlotCanvas::PaintCached) &&
        canvas->paintCache() && !canvas->paintCache()->isNull() )
    {
        QPainter cachePainter((QPixmap *)canvas->paintCache());
        cachePainter.translate(-canvas->contentsRect().x(),
            -canvas->contentsRect().y());

        draw(&cachePainter, xMap, yMap, from, to);
    }

    if ( directPaint )
    {
        QPainter painter(canvas);

        painter.setClipping(true);
        painter.setClipRect(canvas->contentsRect());

        draw(&painter, xMap, yMap, from, to);

        return;
    }

#if QT_VERSION >= 0x040000
    if ( canvas->testPaintAttribute(QwtPlotCanvas::PaintCached) &&
        canvas->paintCache() )
    {
        /*
          The cache is up to date. We flush it via repaint to the
          canvas. This works flicker free but is much ( > 10x )
          slower than direct painting.
         */

        const bool noBG = canvas->testAttribute(Qt::WA_NoBackground);
        if ( !noBG )
            canvas->setAttribute(Qt::WA_NoBackground, true);

        canvas->repaint(canvas->contentsRect());

        if ( !noBG )
            canvas->setAttribute(Qt::WA_NoBackground, false);

        return;
    }
#endif

    // Ok, we give up 
    canvas->repaint(canvas->contentsRect());
}

/*!
  \brief Draw an interval of the curve
  \param painter Painter
  \param xMap maps x-values into pixel coordinates.
  \param yMap maps y-values into pixel coordinates.
  \param from index of the first point to be painted
  \param to index of the last point to be painted. If to < 0 the 
         curve will be painted to its last point.

  \sa QwtPlotCurve::drawCurve, QwtPlotCurve::drawDots,
      QwtPlotCurve::drawLines, QwtPlotCurve::drawSpline,
      QwtPlotCurve::drawSteps, QwtPlotCurve::drawSticks
*/
void QwtPlotCurve::draw(QPainter *painter,
    const QwtScaleMap &xMap, const QwtScaleMap &yMap, 
    int from, int to) const
{
    if ( !painter || dataSize() <= 0 )
        return;

    if (to < 0)
        to = dataSize() - 1;

    if ( verifyRange(from, to) > 0 )
    {
        painter->save();
        painter->setPen(d_data->pen);

        /*
          Qt 4.0.0 is slow when drawing lines, but it´s even 
          slower when the painter has a brush. So we don't
          set the brush before we need it.
         */

        drawCurve(painter, d_data->style, xMap, yMap, from, to);
        painter->restore();

        if (d_data->sym.style() != QwtSymbol::None)
        {
            painter->save();
            drawSymbols(painter, d_data->sym, xMap, yMap, from, to);
            painter->restore();
        }
    }
}

/*!
  \brief Draw the line part (without symbols) of a curve interval. 
  \param painter Painter
  \param style curve style, see QwtPlotCurve::CurveStyle
  \param xMap x map
  \param yMap y map
  \param from index of the first point to be painted
  \param to index of the last point to be painted
  \sa QwtPlotCurve::draw, QwtPlotCurve::drawDots, QwtPlotCurve::drawLines,
      QwtPlotCurve::drawSpline, QwtPlotCurve::drawSteps, QwtPlotCurve::drawSticks
*/

void QwtPlotCurve::drawCurve(QPainter *painter, int style,
    const QwtScaleMap &xMap, const QwtScaleMap &yMap, 
    int from, int to) const
{
    switch (style)
    {
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
        case NoCurve:
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
  \sa QwtPlotCurve::draw, QwtPlotCurve::drawLines, QwtPlotCurve::drawDots, 
      QwtPlotCurve::drawSpline, QwtPlotCurve::drawSteps, QwtPlotCurve::drawSticks
*/
void QwtPlotCurve::drawLines(QPainter *painter,
    const QwtScaleMap &xMap, const QwtScaleMap &yMap, 
    int from, int to) const
{
    const int size = to - from + 1;
    if ( size <= 0 )
        return;

    QwtPolygon polyline(size);

    if ( d_data->paintAttributes & PaintFiltered )
    {
        QPoint pp( xMap.transform(x(from)), yMap.transform(y(from)) );
        polyline.setPoint(0, pp);

        int count = 1;
        for (int i = from + 1; i <= to; i++)
        {
            const QPoint pi(xMap.transform(x(i)), yMap.transform(y(i)));
            if ( pi != pp )
            {
                polyline.setPoint(count, pi);
                count++;

                pp = pi;
            }
        }
        if ( count != size )
            polyline.resize(count);
    }
    else
    {
        for (int i = from; i <= to; i++)
        {
            int xi = xMap.transform(x(i));
            int yi = yMap.transform(y(i));

            polyline.setPoint(i - from, xi, yi);
        }
    }

    if ( d_data->paintAttributes & ClipPolygons )
    {
        QwtRect r = painter->window();
        polyline = r.clip(polyline);
    }

    QwtPainter::drawPolyline(painter, polyline);

    if ( d_data->brush.style() != Qt::NoBrush )
        fillCurve(painter, xMap, yMap, polyline);
}

/*!
  \brief Draw sticks
  \param painter Painter
  \param xMap x map
  \param yMap y map
  \param from index of the first point to be painted
  \param to index of the last point to be painted
  \sa QwtPlotCurve::draw, QwtPlotCurve::drawCurve, QwtPlotCurve::drawDots, 
      QwtPlotCurve::drawLines, QwtPlotCurve::drawSpline, QwtPlotCurve::drawSteps
*/
void QwtPlotCurve::drawSticks(QPainter *painter,
    const QwtScaleMap &xMap, const QwtScaleMap &yMap, 
    int from, int to) const
{
    int x0 = xMap.transform(d_data->reference);
    int y0 = yMap.transform(d_data->reference);

    for (int i = from; i <= to; i++)
    {
        const int xi = xMap.transform(x(i));
        const int yi = yMap.transform(y(i));

        if (d_data->attributes & Xfy)
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
  \sa QwtPlotCurve::drawPolyline, QwtPlotCurve::drawLine, 
      QwtPlotCurve::drawLines, QwtPlotCurve::drawSpline, QwtPlotCurve::drawSteps
      QwtPlotCurve::drawPolyline, QwtPlotCurve::drawPolygon
*/
void QwtPlotCurve::drawDots(QPainter *painter,
    const QwtScaleMap &xMap, const QwtScaleMap &yMap, 
    int from, int to) const
{
    const QRect window = painter->window();
    if ( window.isEmpty() )
        return;

    const bool doFill = d_data->brush.style() != Qt::NoBrush;

    QwtPolygon polyline;
    if ( doFill )
        polyline.resize(to - from + 1);

    if ( to > from && d_data->paintAttributes & PaintFiltered )
    {
        int count = 0;

        if ( doFill )   
        {
            QPoint pp( xMap.transform(x(from)), yMap.transform(y(from)) );
            polyline.setPoint(0, pp);

            int count = 1;
            for (int i = from + 1; i <= to; i++)
            {
                const QPoint pi(xMap.transform(x(i)), yMap.transform(y(i)));
                if ( pi != pp )
                {
                    polyline.setPoint(count, pi);
                    count++;

                    pp = pi;
                }
            }
        }
        else
        {
            // if we don't need to fill, we can sort out
            // duplicates independent from the order

            PrivateData::PixelMatrix pixelMatrix(window);

            for (int i = from; i <= to; i++)
            {
                const QPoint p( xMap.transform(x(i)),
                    yMap.transform(y(i)) );

                if ( pixelMatrix.testPixel(p) )
                {
                    polyline[count] = p;
                    count++;
                }
            }
        }
        if ( int(polyline.size()) != count )
            polyline.resize(count);
    }
    else
    {
        for (int i = from; i <= to; i++)
        {
            const int xi = xMap.transform(x(i));
            const int yi = yMap.transform(y(i));
            QwtPainter::drawPoint(painter, xi, yi);

            if ( doFill )
                polyline.setPoint(i - from, xi, yi);
        }
    }

    if ( d_data->paintAttributes & ClipPolygons )
    {
        const QwtRect r = painter->window();
        polyline = r.clip(polyline);
    }

    if ( doFill )
        fillCurve(painter, xMap, yMap, polyline);
}

/*!
  \brief Draw step function
  \param painter Painter
  \param xMap x map
  \param yMap y map
  \param from index of the first point to be painted
  \param to index of the last point to be painted
  \sa QwtPlotCurve::draw, QwtPlotCurve::drawCurve, QwtPlotCurve::drawDots, 
      QwtPlotCurve::drawLines, QwtPlotCurve::drawSpline, QwtPlotCurve::drawSticks
*/
void QwtPlotCurve::drawSteps(QPainter *painter,
    const QwtScaleMap &xMap, const QwtScaleMap &yMap, 
    int from, int to) const
{
    QwtPolygon polyline(2 * (to - from) + 1);

    bool inverted = d_data->attributes & Yfx;
    if ( d_data->attributes & Inverted )
        inverted = !inverted;

    int i,ip;
    for (i = from, ip = 0; i <= to; i++, ip += 2)
    {
        const int xi = xMap.transform(x(i));
        const int yi = yMap.transform(y(i));

        if ( ip > 0 )
        {
            if (inverted)
                polyline.setPoint(ip - 1, polyline[ip-2].x(), yi);
            else
                polyline.setPoint(ip - 1, xi, polyline[ip-2].y());
        }

        polyline.setPoint(ip, xi, yi);
    }

    if ( d_data->paintAttributes & ClipPolygons )
    {
        const QwtRect r = painter->window();
        polyline = r.clip(polyline);
    }

    QwtPainter::drawPolyline(painter, polyline);

    if ( d_data->brush.style() != Qt::NoBrush )
        fillCurve(painter, xMap, yMap, polyline);
}

/*!
  \brief Draw a spline
  \param painter Painter
  \param xMap x map
  \param yMap y map
  \sa QwtPlotCurve::draw, QwtPlotCurve::drawCurve, QwtPlotCurve::drawDots,
      QwtPlotCurve::drawLines, QwtPlotCurve::drawSteps, QwtPlotCurve::drawSticks
*/
void QwtPlotCurve::drawSpline(QPainter *painter,
    const QwtScaleMap &xMap, const QwtScaleMap &yMap) const
{
    int i;

    const int size = dataSize();
    double *txval = new double[size];
    double *tyval = new double[size];

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
    if (! (d_data->attributes & (Yfx|Xfy|Parametric)))
    {
        if (qwtChkMono(txval, size))
        {
            stype = Yfx;
        }
        else if(qwtChkMono(tyval, size))
        {
            stype = Xfy;
        }
        else
        {
            stype = Parametric;
            if ( (d_data->attributes & Periodic) ||
                ( (x(0) == x(size-1))
                && (y(0) == y(size-1))))
            {
                stype |= Periodic;
            }
        }
    }
    else
    {
        stype = d_data->attributes;
    }

    bool ok = false;
    QwtPolygon polyline(d_data->splineSize);

    if (stype & Parametric)
    {
        //
        // setup parameter vector
        //
        double *param = new double[size];
        param[0] = 0.0;
        for (i=1; i<size; i++)
        {
            const double delta = sqrt( qwtSqr(txval[i] - txval[i-1])
                          + qwtSqr( tyval[i] - tyval[i-1]));
            param[i] = param[i-1] + qwtMax(delta, 1.0);
        }

        //
        // setup splines
        QwtSpline spx, spy;
        ok = spx.recalc(param, txval, size, stype & Periodic);
        if (ok)
            ok = spy.recalc(param, tyval, size, stype & Periodic);

        if ( ok )
        {
            // fill point array
            const double delta = 
                param[size - 1] / double(d_data->splineSize-1);
            for (i = 0; i < d_data->splineSize; i++)
            {
                const double dtmp = delta * double(i);
                polyline.setPoint(i, qRound(spx.value(dtmp)), 
                    qRound(spy.value(dtmp)) );
            }
        }
        delete[] param;
    }
    else if (stype & Xfy)
    {
        if (tyval[size-1] < tyval[0])
        {
            qwtTwistArray(txval, size);
            qwtTwistArray(tyval, size);
        }

        // 1. Calculate spline coefficients
        QwtSpline spx;
        ok = spx.recalc(tyval, txval, size, stype & Periodic);
        if ( ok )
        {
            const double ymin = qwtGetMin(tyval, size);
            const double ymax = qwtGetMax(tyval, size);
            const double delta = (ymax - ymin) / double(d_data->splineSize - 1);

            for (i = 0; i < d_data->splineSize; i++)
            {
                const double dtmp = ymin + delta * double(i);
                polyline.setPoint(i, 
                    qRound(spx.value(dtmp)), qRound(dtmp + 0.5));
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
        QwtSpline spy;
        ok = spy.recalc(txval, tyval, size, stype & Periodic);
        if ( ok )
        {
            const double xmin = qwtGetMin(txval, size);
            const double xmax = qwtGetMax(txval, size);
            const double delta = (xmax - xmin) / double(d_data->splineSize - 1);

            for (i = 0; i < d_data->splineSize; i++)
            {
                double dtmp = xmin + delta * double(i);
                polyline.setPoint(i, 
                    qRound(dtmp), qRound(spy.value(dtmp)));
            }
        }
    }

    delete[] txval;
    delete[] tyval;

    if ( ok )
    {
        if ( d_data->paintAttributes & ClipPolygons )
        {
            const QwtRect r = painter->window();
            polyline = r.clip(polyline);
        }

        QwtPainter::drawPolyline(painter, polyline);

        if ( d_data->brush.style() != Qt::NoBrush )
            fillCurve(painter, xMap, yMap, polyline);
    }
    else
        drawLines(painter, xMap, yMap, 0, size - 1);
}

/*!
  \brief Specify an attribute for the drawing style  

  The attributes can be used to modify the drawing style.
  The following attributes are defined:<dl>
  <dt>QwtPlotCurve::Auto</dt>
  <dd>The default setting. For QwtPlotCurve::spline,
      this means that the type of the spline is
      determined automatically, depending on the data.
      For all other styles, this means that y is
      regarded as a function of x.</dd>
  <dt>QwtPlotCurve::Yfx</dt>
  <dd>Draws y as a function of x (the default). The
      baseline is interpreted as a horizontal line
      with y = baseline().</dd>
  <dt>QwtPlotCurve::Xfy</dt>
  <dd>Draws x as a function of y. The baseline is
      interpreted as a vertical line with x = baseline().</dd>
  <dt>QwtPlotCurve::Parametric</dt>
  <dd>For QwtPlotCurve::Spline only. Draws a parametric spline.</dd>
  <dt>QwtPlotCurve::Periodic</dt>
  <dd>For QwtPlotCurve::Spline only. Draws a periodic spline.</dd>
  <dt>QwtPlotCurve::Inverted</dt>
  <dd>For QwtPlotCurve::Steps only. Draws a step function
      from the right to the left.</dd></dl>

  \param attribute Curve attribute
  \param on On/Off
  /sa QwtPlotCurve::testCurveAttribute()
*/
void QwtPlotCurve::setCurveAttribute(CurveAttribute attribute, bool on)
{
    if ( bool(d_data->attributes & attribute) == on )
        return;

    if ( on )
        d_data->attributes |= attribute;
    else
        d_data->attributes &= ~attribute;

    itemChanged();
}

/*!
    \brief Return the current curve attributes
    \sa QwtPlotCurve::setCurveAttribute
*/
bool QwtPlotCurve::testCurveAttribute(CurveAttribute attribute) const 
{ 
    return d_data->attributes & attribute;
}

/*!
  \brief Change the number of interpolated points
  \param s new size
  \warning The default is 250 points.
*/
void QwtPlotCurve::setSplineSize(int s)
{
    d_data->splineSize = qwtMax(s, 10);
    itemChanged();
}

/*!
    \fn int QwtPlotCurve::splineSize() const
    \brief Return the spline size
    \sa QwtPlotCurve::setSplineSize
*/

int QwtPlotCurve::splineSize() const 
{ 
    return d_data->splineSize; 
}

/*! 
  Fill the area between the polygon and the baseline with 
  the curve brush

  \param painter Painter
  \param xMap x map
  \param yMap y map
  \param pa Polygon

  \sa QwtPlotCurve::setBrush()
*/

void QwtPlotCurve::fillCurve(QPainter *painter,
    const QwtScaleMap &xMap, const QwtScaleMap &yMap,
    QwtPolygon &pa) const
{
    if ( d_data->brush.style() == Qt::NoBrush )
        return;

    closePolyline(xMap, yMap, pa);
    if ( pa.count() <= 2 ) // a line can't be filled
        return;

    QBrush b = d_data->brush;
    if ( !b.color().isValid() )
        b.setColor(d_data->pen.color());

    painter->save();

    painter->setPen(QPen(Qt::NoPen));
    painter->setBrush(b);

    QwtPainter::drawPolygon(painter, pa);

    painter->restore();
}

/*!
  \brief Complete a polygon to be a closed polygon 
         including the area between the original polygon
         and the baseline.
  \param xMap X map
  \param yMap Y map
  \param pa Polygon to be completed
*/

void QwtPlotCurve::closePolyline(
    const QwtScaleMap &xMap, const QwtScaleMap &yMap,
    QwtPolygon &pa) const
{
    const int sz = pa.size();
    if ( sz < 2 )
        return;

    pa.resize(sz + 2);

    if ( d_data->attributes & QwtPlotCurve::Xfy )
    {
        pa.setPoint(sz,
            xMap.transform(d_data->reference), pa.point(sz - 1).y());
        pa.setPoint(sz + 1,
            xMap.transform(d_data->reference), pa.point(0).y());
    }
    else
    {
        pa.setPoint(sz,
            pa.point(sz - 1).x(), yMap.transform(d_data->reference));
        pa.setPoint(pa.size() - 1,
            pa.point(0).x(), yMap.transform(d_data->reference));
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
void QwtPlotCurve::drawSymbols(QPainter *painter, const QwtSymbol &symbol,
    const QwtScaleMap &xMap, const QwtScaleMap &yMap, 
    int from, int to) const
{
    painter->setBrush(symbol.brush());
    painter->setPen(symbol.pen());

    QRect rect;
    rect.setSize(QwtPainter::metricsMap().screenToLayout(symbol.size()));

    if ( to > from && d_data->paintAttributes & PaintFiltered )
    {
        const QRect window = painter->window();
        if ( window.isEmpty() )
            return;

        PrivateData::PixelMatrix pixelMatrix(window);

        for (int i = from; i <= to; i++)
        {
            const QPoint pi( xMap.transform(x(i)),
                yMap.transform(y(i)) );

            if ( pixelMatrix.testPixel(pi) )
            {
                rect.moveCenter(pi);
                symbol.draw(painter, rect);
            }
        }
    }
    else
    {
        for (int i = from; i <= to; i++)
        {
            const int xi = xMap.transform(x(i));
            const int yi = yMap.transform(y(i));

            rect.moveCenter(QPoint(xi, yi));
            symbol.draw(painter, rect);
        }
    }
}

/*!
  \brief Set the value of the baseline

  The baseline is needed for filling the curve with a brush or
  the QwtPlotCurve::Sticks drawing style. 
  The default value is 0.0. The interpretation
  of the baseline depends on the style options. With QwtPlotCurve::Yfx,
  the baseline is interpreted as a horizontal line at y = baseline(),
  with QwtPlotCurve::Yfy, it is interpreted as a vertical line at
  x = baseline().
  \param reference baseline
  \sa QwtPlotCurve::setBrush(), QwtPlotCurve::setStyle(), QwtPlotCurve::setCurveAttribute()
*/
void QwtPlotCurve::setBaseline(double reference)
{
    if ( d_data->reference != reference )
    {
        d_data->reference = reference;
        itemChanged();
    }
}

/*!
    \brief Return the value of the baseline
    \sa QwtPlotCurve::setBaseline
*/
double QwtPlotCurve::baseline() const 
{ 
    return d_data->reference; 
}

/*!
  Return the size of the data arrays
*/
int QwtPlotCurve::dataSize() const
{
    return d_xy->size();
}

int QwtPlotCurve::closestPoint(const QPoint &pos, double *dist) const
{
    if ( plot() == NULL || dataSize() <= 0 )
        return -1;

    const QwtScaleMap xMap = plot()->canvasMap(xAxis());
    const QwtScaleMap yMap = plot()->canvasMap(yAxis());

    int index = -1;
    double dmin = 1.0e10;

    for (int i=0; i < dataSize(); i++)
    {
        const double cx = xMap.xTransform(x(i)) - pos.x();
        const double cy = yMap.xTransform(y(i)) - pos.y();

        const double f = qwtSqr(cx) + qwtSqr(cy);
        if (f < dmin)
        {
            index = i;
            dmin = f;
        }
    }
    if ( dist )
        *dist = sqrt(dmin);

    return index;
}

void QwtPlotCurve::updateLegend(QwtLegend *legend) const
{
    if ( !legend )
        return;

    QwtPlotItem::updateLegend(legend);

    QWidget *widget = legend->find(this);
    if ( !widget || !widget->inherits("QwtLegendItem") )
        return;

    QwtLegendItem *legendItem = (QwtLegendItem *)widget;

#if QT_VERSION < 0x040000
    const bool doUpdate = legendItem->isUpdatesEnabled();
#else
    const bool doUpdate = legendItem->updatesEnabled();
#endif
    legendItem->setUpdatesEnabled(false);

    const int policy = legend->displayPolicy();

    if (policy == QwtLegend::Fixed)
    {
        int mode = legend->identifierMode();

        if (mode & QwtLegendItem::ShowLine)
            legendItem->setCurvePen(pen());

        if (mode & QwtLegendItem::ShowSymbol)
            legendItem->setSymbol(symbol());

        if (mode & QwtLegendItem::ShowText)
            legendItem->setText(title());
        else
            legendItem->setText(QwtText());

        legendItem->setIdentifierMode(mode);
    }
    else if (policy == QwtLegend::Auto)
    {
        int mode = 0;

        if (QwtPlotCurve::NoCurve != style())
        {
            legendItem->setCurvePen(pen());
            mode |= QwtLegendItem::ShowLine;
        }
        if (QwtSymbol::None != symbol().style())
        {
            legendItem->setSymbol(symbol());
            mode |= QwtLegendItem::ShowSymbol;
        }
        if ( !title().isEmpty() )
        {
            legendItem->setText(title());
            mode |= QwtLegendItem::ShowText;
        }
        else
        {
            legendItem->setText(QwtText());
        }
        legendItem->setIdentifierMode(mode);
    }

    legendItem->setUpdatesEnabled(doUpdate);
    legendItem->update();
}
