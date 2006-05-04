/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

// vim: expandtab

#include <qpainter.h>
#include <qstyle.h>
#include <qevent.h>
#include "qwt_painter.h"
#include "qwt_math.h"
#include "qwt_plot.h"
#include "qwt_paint_buffer.h"
#include "qwt_plot_canvas.h"

static const int dim = 5000;

//! Sets a cross cursor, and an invisible red outline

QwtPlotCanvas::QwtPlotCanvas(QwtPlot *plot):
    QFrame(plot, "canvas", Qt::WRepaintNoErase|Qt::WResizeNoErase),
    d_focusIndicator(CanvasFocusIndicator),
    d_cacheMode(TRUE),
    d_cache(NULL)
#ifndef QWT_NO_COMPAT
    ,d_outlineEnabled(FALSE),
    d_outlineActive(FALSE),
    d_mousePressed(FALSE),
    d_outline(Qwt::Rect),
    d_pen(Qt::red)
#endif
{
    setCursor(Qt::crossCursor);
}

//! Destructor
QwtPlotCanvas::~QwtPlotCanvas()
{
    delete d_cache;
}

/*!
  \brief En/Disable caching

  When cache mode is enabled the canvas contents are copied to
  a pixmap that is used for trivial repaints. Such repaints happen
  when a plot gets unhidden, deiconified or changes the focus.

  The win of caching depends on the costs of QwtPlot::drawCanvas. In
  case of plots with huge data it might be significant. The price of
  caching is wasting memory for the cache, what is a pixmap in size
  of contentsRect(). In case of QwtPaintBuffer::isEnabled() updating 
  the cache produces no performance overhead as it reuses the 
  temporary paintbuffer of the double buffering.  
  Otherwise canvas updates have to painted twice to widget and cache.

  \param on Enable caching, when TRUE

  \sa cacheMode(), drawCanvas(), drawContents(), cache()
*/
void QwtPlotCanvas::setCacheMode(bool on)
{
    if ( d_cacheMode != on )
    {
        d_cacheMode = on;
        if (!d_cacheMode )
        {
            delete d_cache;
            d_cache = NULL;
        }
    }
}

/*!
  \return Cache mode
  \sa setCacheMode
*/
bool QwtPlotCanvas::cacheMode() const
{
    return d_cacheMode;
}

//! Return the paint cache, might be null
QPixmap *QwtPlotCanvas::cache()
{
    return d_cache;
}

//! Return the paint cache, might be null
const QPixmap *QwtPlotCanvas::cache() const
{
    return d_cache;
}

//! Invalidate the internal paint cache
void QwtPlotCanvas::invalidateCache()
{
    if ( d_cache )
        d_cache->resize(0, 0);
}

/*!
  Set the focus indicator

  \sa FocusIndicator, focusIndicator
*/
void QwtPlotCanvas::setFocusIndicator(FocusIndicator focusIndicator)
{
    d_focusIndicator = focusIndicator;
}

/*!
  \return Focus indicator
  
  \sa FocusIndicator, setFocusIndicator
*/
QwtPlotCanvas::FocusIndicator QwtPlotCanvas::focusIndicator() const
{
    return d_focusIndicator;
}

//! Requires layout updates of the parent plot
void QwtPlotCanvas::frameChanged()
{
    QFrame::frameChanged();

    // frame changes change the size of the contents rect, what
    // is related to the axes. So we have to update the layout.

    ((QwtPlot *)parent())->updateLayout();
}

//! Redraw the canvas, and focus rect
void QwtPlotCanvas::drawContents(QPainter *painter)
{
    if ( cacheMode() && d_cache 
        && d_cache->size() == contentsRect().size() )
    {
        painter->drawPixmap(contentsRect().topLeft(), *d_cache);
    }
    else
        drawCanvas(painter);

#ifndef QWT_NO_COMPAT
    if ( d_outlineActive )
        drawOutline(*painter); // redraw outline
#endif

    if ( hasFocus() && focusIndicator() == CanvasFocusIndicator )
    {
        const int margin = 1;
        QRect focusRect = contentsRect();
        focusRect.setRect(focusRect.x() + margin, focusRect.y() + margin,
            focusRect.width() - 2 * margin, focusRect.height() - 2 * margin);

        drawFocusIndicator(painter, focusRect);
    }
}

/*!
  Draw the the canvas

  Paints all plot items to the contentsRect(), using QwtPlot::drawCanvas
  and updates the cache.

  \sa QwtPlot::drawCanvas, setCacheMode(), cacheMode()
*/

void QwtPlotCanvas::drawCanvas(QPainter *painter)
{
    if ( !contentsRect().isValid() )
        return;

    QRect clipRect = contentsRect();
    if ( !cacheMode() || !QwtPaintBuffer::isEnabled() )
    {
        // If we don´t need the paint buffer as cache we can
        // use the clip for painting to the buffer too. 

        if ( painter && !painter->clipRegion().isNull() )
            clipRect = painter->clipRegion().boundingRect();
    }

    QwtPaintBuffer paintBuffer(this, clipRect, painter);
    ((QwtPlot *)parent())->drawCanvas(paintBuffer.painter());

    if ( cacheMode() )
    {
        if ( d_cache == NULL )
        {
            d_cache = new QPixmap(contentsRect().size());
#if QT_VERSION >= 300
#ifdef Q_WS_X11
            if ( d_cache->x11Screen() != x11Screen() )
                d_cache->x11SetScreen(x11Screen());
#endif
#endif
        }
        else
            d_cache->resize(contentsRect().size());

        if ( QwtPaintBuffer::isEnabled() )
            *d_cache = paintBuffer.buffer();
        else
        {
            d_cache->fill(this, 0, 0);
            QPainter cachePainter(d_cache);
            cachePainter.translate(-contentsRect().x(),
                -contentsRect().y());
            ((QwtPlot *)parent())->drawCanvas(&cachePainter);
        }
    }
}

//! Draw the focus indication
void QwtPlotCanvas::drawFocusIndicator(QPainter *painter, const QRect &rect)
{
#if QT_VERSION < 300
        style().drawFocusRect(painter, rect, colorGroup());
#else
        style().drawPrimitive(QStyle::PE_FocusRect, painter,
            rect, colorGroup());
#endif
}

#ifndef QWT_NO_COMPAT

//! Mouse event handler
void QwtPlotCanvas::mousePressEvent(QMouseEvent *e)
{
    if (d_outlineActive)
    {
        QPainter p(this);
        drawOutline(p); // Delete active outlines
    }

    d_outlineActive = FALSE;

    //
    // store this point as entry point
    //
    d_lastPoint = e->pos();
    d_entryPoint = e->pos();

    if (d_outlineEnabled)
    {
        QPainter p(this);
        drawOutline(p); // draw new outline
        d_outlineActive = TRUE;
    }

    d_mousePressed = TRUE;

    QMouseEvent m(QEvent::MouseButtonPress, 
        e->pos() - rect().topLeft(), e->button(), e->state());

    emit mousePressed(m);
}

//! Mouse event handler
void QwtPlotCanvas::mouseReleaseEvent(QMouseEvent *e)
{
    if (d_outlineActive)
    {
        QPainter p(this);
        drawOutline(p);
    }

    d_outlineActive = FALSE;
    d_mousePressed = FALSE;

    QMouseEvent m(QEvent::MouseButtonRelease, 
        e->pos() - rect().topLeft(), e->button(), e->state());

    emit mouseReleased(m);
}

//! Mouse event handler
void QwtPlotCanvas::mouseMoveEvent(QMouseEvent *e)
{
    if (d_outlineActive)
    {
        QPainter p(this);
        drawOutline(p);
        d_lastPoint = e->pos();
        drawOutline(p);
    }

    QMouseEvent m(QEvent::MouseMove, 
        e->pos() - rect().topLeft(), e->button(), e->state());

    emit mouseMoved(m);
}

/*!
  \brief Enables or disables outline drawing.

  \warning Outlining functionality is obsolete: use QwtPlotPicker or
  QwtPlotZoomer.

  When the outline feature is enabled, a shape will be drawn
  in the plotting region  when the user presses
  or drags the mouse. It can be used to implement crosshairs,
  mark a selected region, etc.
  \param tf \c TRUE (enabled) or \c FALSE (disabled)
  \warning An outline style has to be specified.
  \sa QwtPlotCanvas::setOutlineStyle()
*/

void QwtPlotCanvas::enableOutline(bool tf)
{

    //
    //  If the mouse is pressed, erase existing outline
    //  or draw new outline if 'tf' changes the 'enabled' state.
    //
    if ((tf != d_outlineEnabled) && d_mousePressed)
    {
        QPainter p(this);
        drawOutline(p);
        d_outlineActive = tf;
    }
    d_outlineEnabled = tf;
}

/*!
  \return \c TRUE if the outline feature is enabled

  \warning Outlining functionality is obsolete: use QwtPlotPicker or
  QwtPlotZoomer.

  \sa QwtPlotCanvas::enableOutline
*/

bool QwtPlotCanvas::outlineEnabled() const 
{ 
    return d_outlineEnabled; 
}

/*!
  \brief Specify the style of the outline

  \warning Outlining functionality is obsolete: use QwtPlotPicker or
  QwtPlotZoomer.

  The outline style determines which kind of shape is drawn
  in the plotting region when the user presses a mouse button
  or drags the mouse. Valid Styles are:
  \param os Outline Style. Valid values are: \c Qwt::HLine, \c Qwt::VLine,
            \c Qwt::Cross, \c Qwt::Rect, \c Qwt::Ellipse
  <dl>
  <dt>Qwt::Cros
  <dd>Cross hairs are drawn across the plotting area
      when the user presses a mouse button. The lines
      intersect at the point where the mouse was pressed
      and move with the mouse pointer.
  <dt>Qwt::HLine, Qwt::VLine
  <dd>A horizontal or vertical line appears when
      the user presses a mouse button. This is useful
      for moving line markers.
  <dt>Qwt::Rect
  <dd>A rectangle is displayed when the user drags
      the mouse. One corner is fixed at the point where
      the mouse was pressed, and the opposite corner moves
      with the mouse pointer. This can be used for selecting
      regions.
  <dt>Qwt::Ellipse
  <dd>Similar to Qwt::Rect, but with an ellipse inside
      a bounding rectangle.
  </dl>
  \sa QwtPlotCanvas::enableOutline(), QwtPlotCanvas::outlineStyle()
*/

void QwtPlotCanvas::setOutlineStyle(Qwt::Shape os)
{
    if (d_outlineActive)
    {
        QPainter p(this); // erase old outline
        drawOutline(p);
    }

    d_outline = os;

    if (d_outlineActive)
    {
        QPainter p(this);
        drawOutline(p); // draw new outline
    }
}

/*!
  \return the outline style

  \warning Outlining functionality is obsolete: use QwtPlotPicker or
  QwtPlotZoomer.

  \sa QwtPlotCanvas::setOutlineStyle()
*/
Qwt::Shape QwtPlotCanvas::outlineStyle() const 
{ 
    return d_outline; 
}

/*!
  \brief Specify a pen for the outline

  \warning Outlining functionality is obsolete: use QwtPlotPicker or
  QwtPlotZoomer.

  \param pen new pen
  \sa QwtPlotCanvas::outlinePen
*/

void QwtPlotCanvas::setOutlinePen(const QPen &pen)
{
    d_pen = pen;
}

/*!
  \return the pen used to draw outlines

  \warning Outlining functionality is obsolete: use QwtPlotPicker or
  QwtPlotZoomer.

  \sa QwtPlotCanvas::setOutlinePen
*/

const QPen& QwtPlotCanvas::outlinePen() const 
{ 
    return d_pen; 
}

/*!
  draw an outline

  \warning Outlining functionality is obsolete: use QwtPlotPicker or
  QwtPlotZoomer.
*/
void QwtPlotCanvas::drawOutline(QPainter &p)
{
    const QRect &r = contentsRect();

    QColor bg = ((QwtPlot *)parent())->canvasBackground();

    QPen pn = d_pen;
    pn.setColor(QColor(bg.rgb() ^ d_pen.color().rgb()));

    p.setPen(pn);
    p.setRasterOp(XorROP);
    p.setClipRect(r);
    p.setClipping(TRUE);

    switch(d_outline)
    {
        case Qwt::VLine:
            QwtPainter::drawLine(&p, d_lastPoint.x(), 
                r.top(), d_lastPoint.x(), r.bottom());
            break;
        
        case Qwt::HLine:
            QwtPainter::drawLine(&p, r.left(), 
                d_lastPoint.y(), r.right(), d_lastPoint.y());
            break;
        
        case Qwt::Cross:
            QwtPainter::drawLine(&p, r.left(), 
                d_lastPoint.y(), r.right(), d_lastPoint.y());
            QwtPainter::drawLine(&p, d_lastPoint.x(), 
                r.top(), d_lastPoint.x(), r.bottom());
            break;

        case Qwt::Rect:
            QwtPainter::drawRect(&p, d_entryPoint.x(), d_entryPoint.y(),
               d_lastPoint.x() - d_entryPoint.x() + 1,
               d_lastPoint.y() - d_entryPoint.y() + 1);
            break;
        
        case Qwt::Ellipse:
            p.drawEllipse(d_entryPoint.x(), d_entryPoint.y(),
               d_lastPoint.x() - d_entryPoint.x() + 1,
               d_lastPoint.y() - d_entryPoint.y() + 1);
            break;

        default:
            break;
    }
}

#endif // !QWT_NO_COMPAT

// Local Variables:
// mode: C++
// c-file-style: "stroustrup"
// indent-tabs-mode: nil
// End:

