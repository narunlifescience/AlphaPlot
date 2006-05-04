/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

// vim: expandtab

#include <math.h>
#include "qwt_plot.h"
#include "qwt_plot_canvas.h"
#include "qwt_plot_zoomer.h"

/*!
  \brief Create a zoomer for a plot canvas.

  The zoomer is set to those x- and y-axis of the parent plot of the
  canvas that are enabled. If both or no x-axis are enabled, the picker
  is set to QwtPlot::xBottom. If both or no y-axis are
  enabled, it is set to QwtPlot::yLeft.

  The selectionFlags() are set to 
  QwtPicker::RectSelection & QwtPicker::ClickSelection, the
  cursor label mode to QwtPicker::ActiveOnly.

  \param canvas Plot canvas to observe, also the parent object
  \param name Object name

  \warning Calling QwtPlot::setAxisScale() while QwtPlot::autoReplot() is FALSE
           leaves the axis in an 'intermediate' state.
           In this case, to prevent buggy behaviour, you must call
           QwtPlot::replot() before calling QwtPlotZoomer().
           This quirk will be removed in a future release.

  \sa QwtPlot::autoReplot(), QwtPlot::replot(), QwtPlotPicker::setZoomBase()
*/
QwtPlotZoomer::QwtPlotZoomer(QwtPlotCanvas *canvas, const char *name):
    QwtPlotPicker(canvas, name)
{
    if ( canvas )
        init();
}

/*!
  \brief Create a zoomer for a plot canvas.

  The selectionFlags() are set to 
  QwtPicker::RectSelection & QwtPicker::ClickSelection, the
  cursor label mode to QwtPicker::ActiveOnly. 

  \param xAxis X axis of the zoomer
  \param yAxis Y axis of the zoomer
  \param canvas Plot canvas to observe, also the parent object
  \param name Object name

  \warning Calling QwtPlot::setAxisScale() while QwtPlot::autoReplot() is FALSE
           leaves the axis in an 'intermediate' state.
           In this case, to prevent buggy behaviour, you must call
           QwtPlot::replot() before calling QwtPlotZoomer().
           This quirk will be removed in a future release.

  \sa QwtPlot::autoReplot(), QwtPlot::replot(), QwtPlotPicker::setZoomBase()
*/

QwtPlotZoomer::QwtPlotZoomer(int xAxis, int yAxis,
        QwtPlotCanvas *canvas, const char *name):
    QwtPlotPicker(xAxis, yAxis, canvas, name)
{
    if ( canvas )
    {
        init();
    }
}

/*!
  Create a zoomer for a plot canvas.

  \param xAxis X axis of the zoomer
  \param yAxis Y axis of the zoomer
  \param selectionFlags Or´d value of QwtPicker::RectSelectionType and
                        QwtPicker::SelectionMode. 
                        QwtPicker::RectSelection will be auto added.
  \param cursorLabelMode Cursor label mode
  \param canvas Plot canvas to observe, also the parent object
  \param name Object name

  \sa QwtPicker, QwtPicker::setSelectionFlags(), QwtPicker::setRubberBand(),
      QwtPicker::setCursorLabelMode

  \warning Calling QwtPlot::setAxisScale() while QwtPlot::autoReplot() is FALSE
           leaves the axis in an 'intermediate' state.
           In this case, to prevent buggy behaviour, you must call
           QwtPlot::replot() before calling QwtPlotZoomer().
           This quirk will be removed in a future release.

  \sa QwtPlot::autoReplot(), QwtPlot::replot(), QwtPlotPicker::setZoomBase()
*/

QwtPlotZoomer::QwtPlotZoomer(int xAxis, int yAxis, int selectionFlags,
        DisplayMode cursorLabelMode, QwtPlotCanvas *canvas, const char *name):
    QwtPlotPicker(xAxis, yAxis,canvas, name)
{
    if ( canvas )
    {
        init(selectionFlags, cursorLabelMode);
    }
}

//! Init the zoomer, used by the constructors
void QwtPlotZoomer::init(int selectionFlags, DisplayMode cursorLabelMode)
{
    d_maxStackDepth = -1;

    setSelectionFlags(selectionFlags);
    setCursorLabelMode(cursorLabelMode);
    setRubberBand(RectRubberBand);

    setZoomBase(scaleRect());
}

/*!
  \brief Limit the number of recursive zoom operations to depth.

  A value of -1 set the depth to unlimited, 0 disables zooming.
  If the current zoom rectangle is below depth, the plot is unzoomed.

  \param depth Maximum for the stack depth
  \sa maxStackDepth()
  \note depth doesn´t include the zoom base, so zoomStack().count() might be
              maxStackDepth() + 1.
*/
void QwtPlotZoomer::setMaxStackDepth(int depth)
{
    d_maxStackDepth = depth;

    if ( depth >= 0 )
    {
        // unzoom if the current depth is below d_maxStackDepth

        const int zoomOut = 
            d_zoomStack.count() - 1 - depth; // -1 for the zoom base

        if ( zoomOut > 0 )
        {
            zoom(-zoomOut);
            for ( uint i = d_zoomStack.count() - 1; i > d_zoomRectIndex; i-- )
                (void)d_zoomStack.pop(); // remove trailing rects
        }
    }
}

/*!
  \return Maximal depth of the zoom stack.
  \sa setMaxStackDepth()
*/
int QwtPlotZoomer::maxStackDepth() const
{
    return d_maxStackDepth;
}

/*!
  Return the zoom stack. zoomStack()[0] is the zoom base,
  zoomStack()[1] the first zoomed rectangle.
*/
const QValueStack<QwtDoubleRect> &QwtPlotZoomer::zoomStack() const
{
    return d_zoomStack;
}

/*!
  Return the zoom stack. zoomStack()[0] is the zoom base,
  zoomStack()[1] the first zoomed rectangle.
*/
QValueStack<QwtDoubleRect> &QwtPlotZoomer::zoomStack() 
{
    return d_zoomStack;
}

/*!
  \return Initial rectangle of the zoomer
  \sa setZoomBase(), zoomRect()
*/
QwtDoubleRect QwtPlotZoomer::zoomBase() const
{
    return d_zoomStack[0];
}

/*!
  Reinitialized the zoom stack with scaleRect() as base.

  \sa zoomBase(), scaleRect()

  \warning Calling QwtPlot::setAxisScale() while QwtPlot::autoReplot() is FALSE
           leaves the axis in an 'intermediate' state.
           In this case, to prevent buggy behaviour, you must call
           QwtPlot::replot() before calling QwtPlotZoomer::setZoomBase().
           This quirk will be removed in a future release.

  \sa QwtPlot::autoReplot(), QwtPlot::replot().
*/
void QwtPlotZoomer::setZoomBase()
{
    const QwtPlot *plt = plot();
    if ( !plt )
        return;

    d_zoomStack.clear();
    d_zoomStack.push(scaleRect());
    d_zoomRectIndex = 0;

    rescale();
}

/*!
  \brief Set the initial size of the zoomer.

  base is united with the current scaleRect() and the zoom stack is
  reinitalized with it as zoom base. plot is zoomed to scaleRect().
  
  \param base Zoom base
  
  \sa zoomBase(), scaleRect()
*/
void QwtPlotZoomer::setZoomBase(const QwtDoubleRect &base)
{
    const QwtPlot *plt = plot();
    if ( !plt )
        return;

    const QwtDoubleRect sRect = scaleRect();
    const QwtDoubleRect bRect = base | sRect;

    d_zoomStack.clear();
    d_zoomStack.push(bRect);
    d_zoomRectIndex = 0;

    if ( base != sRect )
    {
        d_zoomStack.push(sRect);
        d_zoomRectIndex++;
    }

    rescale();
}

/*! 
  Rectangle at the current position on the zoom stack. 

  \sa QwtPlotZoomer::zoomRectIndex(), QwtPlotZoomer::scaleRect().
*/
QwtDoubleRect QwtPlotZoomer::zoomRect() const
{
    return d_zoomStack[d_zoomRectIndex];
}

/*! 
  \return Index of current position of zoom stack.
*/
uint QwtPlotZoomer::zoomRectIndex() const
{
    return d_zoomRectIndex;
}

/*!
  \brief Zoom in

  Clears all rectangles above the current position of the
  zoom stack and pushs the intersection of zoomRect() and 
  the normalized rect on it.

  \note If the maximal stack depth is reached, zoom is ignored.
*/

void QwtPlotZoomer::zoom(const QwtDoubleRect &rect)
{   
    if ( d_maxStackDepth >= 0 && int(d_zoomRectIndex) >= d_maxStackDepth )
        return;

    const QwtDoubleRect zoomRect = d_zoomStack[0] & rect.normalize();
    if ( zoomRect != d_zoomStack[d_zoomRectIndex] )
    {
        for ( uint i = d_zoomStack.count() - 1; i > d_zoomRectIndex; i-- )
            (void)d_zoomStack.pop();

        d_zoomStack.push(zoomRect);
        d_zoomRectIndex++;

        rescale();
    }
}

/*!
  \brief Zoom in or out

  Activate a rectangle on the zoom stack with an offset relative
  to the current position. Negative values of offest will zoom out,
  positive zoom in. A value of 0 zooms out to the zoom base.

  \param offset Offset relative to the current position of the zoom stack.
  \sa zoomRectIndex()
*/
void QwtPlotZoomer::zoom(int offset)
{
    if ( offset == 0 )
        d_zoomRectIndex = 0;
    else
    {
        int newIndex = d_zoomRectIndex + offset;
        newIndex = QMAX(0, newIndex);
        newIndex = QMIN(int(d_zoomStack.count() - 1), newIndex);

        d_zoomRectIndex = uint(newIndex);
    }

    rescale();
}

/*! 
  Adjust the observed plot to zoomRect()

  \note Initiates QwtPlot::replot
*/

void QwtPlotZoomer::rescale()
{
    QwtPlot *plt = plot();
    if ( !plt )
        return;

    const QwtDoubleRect &rect = d_zoomStack[d_zoomRectIndex];
    if ( rect != scaleRect() )
    {
        plt->setAxisScale(xAxis(), rect.x1(), rect.x2());
        plt->setAxisScale(yAxis(), rect.y1(), rect.y2());

        plt->replot();
    }
}

/*!
  Reinitialize the axes, and set the zoom base to their scales.

  \param xAxis X axis 
  \param yAxis Y axis
*/

void QwtPlotZoomer::setAxis(int xAxis, int yAxis)
{
    if ( xAxis != QwtPlotPicker::xAxis() || yAxis != QwtPlotPicker::yAxis() )
    {
        QwtPlotPicker::setAxis(xAxis, yAxis);
        setZoomBase(scaleRect());
    }
}

/*!
   Qt::MidButton zooms out one position on the zoom stack,
   Qt::RightButton to the zoom base.

   Changes the current position on the stack, but doesn´t pop
   any rectangle.

   \note The mouse events can be changed, using
         QwtEventPattern::setMousePattern: 2, 1
*/
void QwtPlotZoomer::widgetMouseReleaseEvent(QMouseEvent *me)
{
    if ( mouseMatch(MouseSelect2, me) )
        zoom(0);
    else if ( mouseMatch(MouseSelect3, me) )
        zoom(-1);
    else if ( mouseMatch(MouseSelect6, me) )
        zoom(+1);
    else 
        QwtPlotPicker::widgetMouseReleaseEvent(me);
}

/*!
   Qt::Key_Plus zooms out, Qt::Key_Minus zooms in one position on the 
   zoom stack, Qt::Key_Escape zooms out to the zoom base.

   Changes the current position on the stack, but doesn´t pop
   any rectangle.

   \note The keys codes can be changed, using
         QwtEventPattern::setKeyPattern: 3, 4, 5
*/

void QwtPlotZoomer::widgetKeyPressEvent(QKeyEvent *ke)
{
    if ( !isActive() )
    {
        if ( keyMatch(KeyUndo, ke) )
            zoom(-1);
        else if ( keyMatch(KeyRedo, ke) )
            zoom(+1);
        else if ( keyMatch(KeyHome, ke) )
            zoom(0);
    }

    QwtPlotPicker::widgetKeyPressEvent(ke);
}

/*!
  Move the current zoom rectangle.

  \param dx X offset
  \param dy Y offset

  \note The changed rectangle is limited by the zoom base
*/
void QwtPlotZoomer::moveBy(double dx, double dy)
{
    const QwtDoubleRect &rect = d_zoomStack[d_zoomRectIndex];
    move(rect.x1() + dx, rect.y1() + dy);
}

/*!
  Move the the current zoom rectangle.

  \param x X value
  \param y value

  \sa QwtDoubleRect::move
  \note The changed rectangle is limited by the zoom base
*/
void QwtPlotZoomer::move(double x, double y)
{
    x = QMAX(x, zoomBase().x1());
    x = QMIN(x, zoomBase().x2() - zoomRect().width());

    y = QMAX(y, zoomBase().y1());
    y = QMIN(y, zoomBase().y2() - zoomRect().height());

    if ( x != zoomRect().x1() || y != zoomRect().y1() )
    {
        d_zoomStack[d_zoomRectIndex].move(x, y);
        rescale();
    }
}

/*!
  \brief Check and correct a selected rectangle

  Reject rectangles with a hight or width < 2, otherwise
  expand the selected rectangle to a minimum size of 11x11
  and accept it.
  
  \return TRUE If rect is accepted, or has been changed
          to a accepted rectangle. 
*/

bool QwtPlotZoomer::accept(QPointArray &pa) const
{
    if ( pa.count() < 2 )
        return FALSE;

    QRect rect = QRect(pa[0], pa[int(pa.count() - 1)]).normalize();

    const int minSize = 2;
    if (rect.width() < minSize && rect.height() < minSize )
        return FALSE; 

    const int minZoomSize = 11;

    const QPoint center = rect.center();
    rect.setSize(rect.size().expandedTo(QSize(minZoomSize, minZoomSize)));
    rect.moveCenter(center);

    pa.resize(2);
    pa[0] = rect.topLeft();
    pa[1] = rect.bottomRight();

    return TRUE;
}

/*!
  \brief Limit zooming by a minimum rectangle

  Tries to stop zooming, when the precision of the
  axes label string conversion leads to identical values.

  \return minimum rectangle
*/
QwtDoubleSize QwtPlotZoomer::minZoomSize() const
{
    char f;
    int fw, xprecision, yprecision;

    plot()->axisLabelFormat(xAxis(), f, xprecision, fw);
    plot()->axisLabelFormat(yAxis(), f, yprecision, fw);

    return QwtDoubleSize(
        d_zoomStack[0].width() / pow(10.0, xprecision - 1),
        d_zoomStack[0].height() / pow(10.0, yprecision - 1)
    );
}

/*! 
  Rejects selections, when the stack depth is too deep, or
  the zoomed rectangle is minZoomSize().

  \sa minZoomSize(), maxStackDepth()
*/
void QwtPlotZoomer::begin()
{
    if ( d_maxStackDepth >= 0 )
    {
        if ( d_zoomRectIndex >= uint(d_maxStackDepth) )
            return;
    }

    const QwtDoubleSize minSize = minZoomSize();
    if ( minSize.isValid() )
    {
        const QwtDoubleSize sz = 
            d_zoomStack[d_zoomRectIndex].size() * 0.9999;

        if ( minSize.width() >= sz.width() &&
            minSize.height() >= sz.height() )
        {
            return;
        }
    }

    QwtPlotPicker::begin();
}

/*!
  Expand the selected rectangle to minZoomSize() and zoom in
  if accepted.

  \note The zoomed signal is emitted.
  \sa QwtPlotZoomer::accept()a, QwtPlotZoomer::minZoomSize()
*/
bool QwtPlotZoomer::end(bool ok)
{
    ok = QwtPlotPicker::end(ok);
    if (!ok)
        return FALSE;

    QwtPlot *plot = QwtPlotZoomer::plot();
    if ( !plot )
        return FALSE;

    const QPointArray &pa = selection();
    if ( pa.count() < 2 )
        return FALSE;

    QRect rect = QRect(pa[0], pa[int(pa.count() - 1)]).normalize();

    QwtDoubleRect zoomRect = invTransform(rect).normalize();

    const QwtDoublePoint center = zoomRect.center();
    zoomRect.setSize(zoomRect.size().expandedTo(minZoomSize()));
    zoomRect.moveCenter(center);

    zoom(zoomRect);
    emit zoomed(zoomRect);

    return TRUE;
}

/*!
  Set the selection flags
  
  \param flags Or´d value of QwtPicker::RectSelectionType and
               QwtPicker::SelectionMode. The default value is 
               QwtPicker::RectSelection & QwtPicker::ClickSelection.

  \sa selectionFlags(), SelectionType, RectSelectionType, SelectionMode
  \note QwtPicker::RectSelection will be auto added.
*/

void QwtPlotZoomer::setSelectionFlags(int flags)
{
    // we accept only rects
    flags &= ~(PointSelection | PolygonSelection);
    flags |= RectSelection;

    QwtPlotPicker::setSelectionFlags(flags);
}

// Local Variables:
// mode: C++
// c-file-style: "stroustrup"
// indent-tabs-mode: nil
// End:

