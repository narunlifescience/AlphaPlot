/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *
 *
 *Modifications added by Ion Vasilief to function: void QwtPlot::updateAxes()
 *****************************************************************************/

// vim: expandtab
#include <qapplication.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qfocusdata.h>
#include <qevent.h>
#include "qwt_plot.h"
#include "qwt_plot_layout.h"
#include "qwt_plot_dict.h"
#include "qwt_rect.h"
#include "qwt_scale.h"
#include "qwt_scale.h"
#include "qwt_legend.h"
#include "qwt_dyngrid_layout.h"
#include "qwt_plot_canvas.h"
#include "qwt_math.h"
#include "qwt_paint_buffer.h"

/*!
  \brief Constructor
  \param parent Parent widget
  \param name Widget name
 */

QwtPlot::QwtPlot(QWidget *parent, const char *name) :
    QFrame(parent, name, Qt::WRepaintNoErase|Qt::WResizeNoErase)
{
    initPlot();
}


/*!
  \brief Constructor
  \param title Title text
  \param parent Parent widget
  \param name Widget name
 */
QwtPlot::QwtPlot(const QString &title, QWidget *parent, const char *name) :
    QFrame(parent, name, Qt::WRepaintNoErase|Qt::WResizeNoErase)
{
    initPlot(title);
}

//! Destructor
QwtPlot::~QwtPlot()
{
    delete d_layout;
    delete d_curves;
    delete d_markers;
    delete d_grid;
}

/*!
  \brief Initializes a QwtPlot instance
  \param title Title text
 */
void QwtPlot::initPlot(const QString &title)
{
    d_layout = new QwtPlotLayout;

    d_curves = new QwtCurveDict;
    d_markers = new QwtMarkerDict;

    d_autoReplot = FALSE;

    d_lblTitle = new QLabel(title, this);
    d_lblTitle->setFont(QFont(fontInfo().family(), 14, QFont::Bold));
    d_lblTitle->setAlignment(Qt::AlignCenter|Qt::WordBreak|Qt::ExpandTabs);

    d_legend = new QwtLegend(this);
    d_autoLegend = FALSE;

    d_scale[yLeft] = new QwtScale(QwtScale::Left, this, "yLeft");
    d_scale[yRight] = new QwtScale(QwtScale::Right, this, "yRight");
    d_scale[xTop] = new QwtScale(QwtScale::Top, this, "xTop");
    d_scale[xBottom] = new QwtScale(QwtScale::Bottom, this, "xBottom");

    initAxes();

    d_grid = new QwtPlotGrid(this);
    d_grid->setPen(QPen(Qt::black, 0, Qt::DotLine));
    d_grid->enableXMin(FALSE);
    d_grid->enableYMin(FALSE);
    d_grid->setAxis(xBottom, yLeft);

    d_canvas = new QwtPlotCanvas(this);
    d_canvas->setFrameStyle(QFrame::Panel|QFrame::Sunken);
    d_canvas->setLineWidth(2);
    d_canvas->setMidLineWidth(0);

#ifndef QWT_NO_COMPAT
    connect(d_canvas, SIGNAL(mousePressed(const QMouseEvent &)),
        this, SIGNAL(plotMousePressed(const QMouseEvent &)));
    connect(d_canvas, SIGNAL(mouseMoved(const QMouseEvent &)),
        this, SIGNAL(plotMouseMoved(const QMouseEvent &)));
    connect(d_canvas, SIGNAL(mouseReleased(const QMouseEvent &)),
        this, SIGNAL(plotMouseReleased(const QMouseEvent &)));
#endif

    updateTabOrder();

    QSizePolicy sp;
    sp.setHorData( QSizePolicy::MinimumExpanding );
    sp.setVerData( QSizePolicy::MinimumExpanding );
    setSizePolicy(sp);
}

//! Initialize axes
void QwtPlot::initAxes()
{
    int axis;

    QFont fscl(fontInfo().family(), 10);
    QFont fttl(fontInfo().family(), 12, QFont::Bold);

    for(axis = 0; axis < axisCnt; axis++)
    {
        d_scale[axis]->setFont(fscl);
        d_scale[axis]->setTitleFont(fttl);
        d_scale[axis]->setBaselineDist(2);
    }

    d_axisEnabled[yLeft] = TRUE;
    d_axisEnabled[yRight] = FALSE;
    d_axisEnabled[xBottom] = TRUE;
    d_axisEnabled[xTop] = FALSE;

    for (axis=0; axis < axisCnt; axis++)
    {
        d_as[axis].adjust(0.0,1000.0,TRUE);
        d_scale[axis]->setScaleDiv(d_as[axis].scaleDiv());
    }
}

/*!
  \brief Adds handling of QEvent::LayoutHint
*/
bool QwtPlot::event(QEvent *e)
{
    bool ok = QFrame::event(e);
    switch(e->type())
    {
#if 0
        case QEvent::ChildInserted:
        case QEvent::ChildRemoved:
#endif
        case QEvent::LayoutHint:
            updateLayout();
            break;
        default:;
    }
    return ok;
}

/*!
  \brief Replots the plot if QwtPlot::autoReplot() is \c TRUE.
*/

void QwtPlot::autoRefresh()
{
    if (d_autoReplot)
        replot();
}

/*!
  \brief Set or reset the autoReplot option
  If the autoReplot option is set, the plot will be
  updated implicitly by manipulating member functions.
  Since this may be time-consuming, it is recommended
  to leave this option switched off and call replot()
  explicitly if necessary.

  The autoReplot option is set to FALSE by default, which
  means that the user has to call replot() in order to make
  changes visible.
  \param tf \c TRUE or \c FALSE. Defaults to \c TRUE.
  \sa replot()
*/
void QwtPlot::setAutoReplot(bool tf)
{
    d_autoReplot = tf;
}

/*!
    \return TRUE if the autoReplot option is set.
*/
bool QwtPlot::autoReplot() const
{
    return d_autoReplot; 
}

/*!
  \brief Change the plot's title
  \param t new title
*/
void QwtPlot::setTitle(const QString &t)
{
    d_lblTitle->setText(t);
}

/*!
  \return the plot's title
*/

QString QwtPlot::title() const
{
    return d_lblTitle->text();
}


/*!
  \brief Change the title font
  \param f new title font
*/
void QwtPlot::setTitleFont(const QFont &f)
{
    d_lblTitle->setFont(f);
}

/*!
  \return the plot's title font
*/
QFont QwtPlot::titleFont() const
{
    return d_lblTitle->font();
}

/*!
  \return the plot's layout
*/
QwtPlotLayout *QwtPlot::plotLayout()
{
    return d_layout;
}

/*!
  \return the plot's layout
*/
const QwtPlotLayout *QwtPlot::plotLayout() const
{
    return d_layout;
}

/*!
  \return the plot's titel label.
*/
QLabel *QwtPlot::titleLabel()
{
    return d_lblTitle;
}

/*!
  \return the plot's titel label.
*/
const QLabel *QwtPlot::titleLabel() const
{
    return d_lblTitle;
}

/*!
  \return the plot's legend
  \sa insertLegendItem(), updateLegendItem(), printLegendItem()
*/
QwtLegend *QwtPlot::legend()
{ 
    return d_legend;
}   

/*!
  \return the plot's legend
  \sa insertLegendItem(), updateLegendItem(), printLegendItem()
*/
const QwtLegend *QwtPlot::legend() const
{ 
    return d_legend;
}   


/*!
  \return the plot's canvas
*/
QwtPlotCanvas *QwtPlot::canvas()
{ 
    return d_canvas;
}   

/*!
  \return the plot's canvas
*/
const QwtPlotCanvas *QwtPlot::canvas() const
{ 
    return d_canvas;
}

/*!  
  Return sizeHint
  \sa QwtPlot::minimumSizeHint()
*/

QSize QwtPlot::sizeHint() const
{
    int dw = 0;
    int dh = 0;
    for ( int axis = 0; axis < axisCnt; axis++ )
    {
        if ( d_axisEnabled[axis] )
        {   
            const int niceDist = 40;
            const QwtScale *scale = d_scale[axis];
            const int majCnt = scale->scaleDraw()->scaleDiv().majCnt();

            if ( axis == yLeft || axis == yRight )
            {
                int hDiff = (majCnt - 1) * niceDist 
                    - scale->minimumSizeHint().height();
                if ( hDiff > dh )
                    dh = hDiff;
            }
            else
            {
                int wDiff = (majCnt - 1) * niceDist 
                    - scale->minimumSizeHint().width();
                if ( wDiff > dw )
                    dw = wDiff;
            }
        }
    }
    return minimumSizeHint() + QSize(dw, dh);
}

/*!
  \brief Return a minimum size hint
*/
QSize QwtPlot::minimumSizeHint() const
{
    QSize hint = d_layout->minimumSizeHint(this);
    hint += QSize(2 * frameWidth(), 2 * frameWidth());

    return hint;
}

//! Resize and update internal layout
void QwtPlot::resizeEvent(QResizeEvent *e)
{
    QFrame::resizeEvent(e);
    updateLayout();
}

/*!
  \brief Redraw the plot

  If the autoReplot option is not set (which is the default)
  or if any curves are attached to raw data, the plot has to
  be refreshed explicitly in order to make changes visible.

  \sa setAutoReplot()
  \warning Calls canvas()->repaint, take care of infinite recursions
*/
void QwtPlot::replot()
{
    bool doAutoReplot = autoReplot();
    setAutoReplot(FALSE);

    updateAxes();
	// Force the scales to resize to prevent that
    // the scales and canvas are drawn with different maps.
    QApplication::sendPostedEvents(this, QEvent::LayoutHint);

    d_canvas->invalidateCache();
    d_canvas->repaint(d_canvas->contentsRect(), FALSE);

    setAutoReplot(doAutoReplot);
}

/*!
  \brief Adjust plot content to its current size.
  \sa QwtPlot::resizeEvent
*/
void QwtPlot::updateLayout()
{
    d_layout->activate(this, contentsRect());

    //
    // resize and show the visible widgets
    //
    if (!d_lblTitle->text().isEmpty())
    {
        d_lblTitle->setGeometry(d_layout->titleRect());
        if (!d_lblTitle->isVisible())
            d_lblTitle->show();
    }
    else
        d_lblTitle->hide();

    for (int axis = 0; axis < axisCnt; axis++ )
    {
        if (d_axisEnabled[axis])
        {
            d_scale[axis]->setGeometry(d_layout->scaleRect(axis));

            if ( axis == xBottom || axis == xTop )
            {
                QRegion r(d_layout->scaleRect(axis));
                if ( d_axisEnabled[yLeft] )
                    r = r.subtract(QRegion(d_layout->scaleRect(yLeft)));
                if ( d_axisEnabled[yRight] )
                    r = r.subtract(QRegion(d_layout->scaleRect(yRight)));
                r.translate(-d_layout->scaleRect(axis).x(), 
                    -d_layout->scaleRect(axis).y());

                d_scale[axis]->setMask(r);
            }
            if (!d_scale[axis]->isVisible())
                d_scale[axis]->show();
        }
        else
            d_scale[axis]->hide();
    }

    if (d_legend->itemCount() > 0)
    {
        d_legend->setGeometry(d_layout->legendRect());
        d_legend->show();
    }
    else
        d_legend->hide();

    d_canvas->setGeometry(d_layout->canvasRect());
}

//! Rebuild the scales and maps
void QwtPlot::updateAxes()
{
    int i;
    bool resetDone[axisCnt];
    for (i = 0; i < axisCnt; i++)
        resetDone[i] = FALSE;

    //
    //  Adjust autoscalers
    //

    QwtPlotCurveIterator itc = curveIterator();
    for (const QwtPlotCurve *c = itc.toFirst(); c != 0; c = ++itc )
    {
        const int xAxis = c->xAxis();
        const int yAxis = c->yAxis();

        if ( d_as[xAxis].autoScale() || d_as[yAxis].autoScale() )
        {
            const QwtDoubleRect rect = c->boundingRect();
            if ( rect.isValid() )
            {
                if ( d_as[xAxis].autoScale() )
                {
                    if ( !resetDone[xAxis] )
                    {
                        d_as[xAxis].reset();
                        resetDone[xAxis] = TRUE;
                    }
                    d_as[xAxis].adjust(rect.x1(), rect.x2());
                }

                if ( d_as[yAxis].autoScale() )
                {
                    if ( !resetDone[yAxis] )
                    {
                        d_as[yAxis].reset();
                        resetDone[yAxis] = TRUE;
                    }
                    d_as[yAxis].adjust(rect.y1(), rect.y2());
                }
            }
        }
    }

    //
    // Adjust scales
    //

	/*******************************************************/
	//modifications added by Ion Vasilief in order to synchronize oposite axes

	d_scale[xBottom]->setScaleDiv(d_as[xBottom].scaleDiv());
	d_scale[xTop]->setScaleDiv(d_as[xBottom].scaleDiv());
	d_scale[yLeft]->setScaleDiv(d_as[yLeft].scaleDiv());
	d_scale[yRight]->setScaleDiv(d_as[yLeft].scaleDiv());

	int startDist, endDist;
    d_scale[yLeft]->minBorderDist(startDist, endDist);
	d_scale[yLeft]->setBorderDist(startDist, endDist);
    d_scale[yRight]->setBorderDist(startDist, endDist);
	
	d_scale[xBottom]->minBorderDist(startDist, endDist);
	d_scale[xBottom]->setBorderDist(startDist, endDist);
    d_scale[xTop]->setBorderDist(startDist, endDist);
	/*******************************************************/

    d_grid->setXDiv(d_as[d_grid->xAxis()].scaleDiv());
    d_grid->setYDiv(d_as[d_grid->yAxis()].scaleDiv());
}

//! Update the focus tab order

void QwtPlot::updateTabOrder()
{
    // Depending on the position of the legend the 
    // tab order will be changed that the canvas is
    // next to the last legend item, or directly before
    // the first one. The following code seems much too
    // complicated but is there a better implementation ?

    if ( d_canvas->focusPolicy() == QWidget::NoFocus || focusData() == NULL )
        return;

    // move the cursor to the canvas

    for ( int i = 0; i < focusData()->count(); i++ )
    {
        if ( focusData()->next() == d_canvas )
            break;
    }

    const bool canvasFirst = d_layout->legendPosition() == QwtPlot::Bottom ||
        d_layout->legendPosition() == QwtPlot::Right;

    for ( int j = 0; j < focusData()->count(); j++ )
    {
        QWidget *w = canvasFirst ? focusData()->next() : focusData()->prev();

        if ( w->focusPolicy() != QWidget::NoFocus 
            && w->parent() && w->parent() == d_legend->contentsWidget() )
        {
            if ( canvasFirst )
            {
                do // go back to last non legend item
                {
                    w = focusData()->prev(); // before the first legend item
                } while ( w->focusPolicy() == QWidget::NoFocus );
            }

            if ( w != d_canvas )
                setTabOrder(w, d_canvas);
            break;
        }
    }
}

//! drawContents
// \sa QFrame::drawContents

void QwtPlot::drawContents( QPainter * )
{
    // We must erase the region that is not
    // occupied by our children
    QRegion cr( contentsRect() );
    cr = cr.subtract( childrenRegion() );
    erase( cr );
}

/*! 
  Redraw the canvas.
  \param painter Painter used for drawing

  \warning drawCanvas calls drawCanvasItems what is also used
           for printing. Applications that like to add individual
           plot items better overload QwtPlot::drawCanvasItems
  \sa QwtPlot::drawCanvasItems
*/

void QwtPlot::drawCanvas(QPainter *painter)
{
    QwtArray<QwtDiMap> map(axisCnt);
    for ( int axis = 0; axis < axisCnt; axis++ )
        map[axis] = canvasMap(axis);

    drawCanvasItems(painter, 
        d_canvas->contentsRect(), map, QwtPlotPrintFilter());
}

/*! 
  Redraw the canvas items.
  \param painter Painter used for drawing
  \param rect Bounding rectangle where to paint
  \param map QwtPlot::axisCnt maps, mapping between plot and paint device coordinates
  \param pfilter Plot print filter
*/

void QwtPlot::drawCanvasItems(QPainter *painter, const QRect &rect, 
        const QwtArray<QwtDiMap> &map, const QwtPlotPrintFilter &pfilter) const
{
    //
    // draw grid
    //
    if ( pfilter.options() & QwtPlotPrintFilter::PrintGrid )
    {
        if ( d_grid->enabled() )
        {
            d_grid->draw(painter, rect, 
                map[d_grid->xAxis()], map[d_grid->yAxis()]);
        }
    }

    //
    //  draw curves
    //
    QwtPlotCurveIterator itc = curveIterator();
    for (QwtPlotCurve *curve = itc.toFirst(); curve != 0; curve = ++itc )
    {
        if ( curve->enabled() )
        {
            curve->draw(painter, 
                map[curve->xAxis()], map[curve->yAxis()]);
        }
    }

    //
    // draw markers
    //
    QwtPlotMarkerIterator itm = markerIterator();
    for (QwtPlotMarker *marker = itm.toFirst(); marker != 0; marker = ++itm )
    {
        if ( marker->enabled() )
        {
            marker->draw(painter,
                map[marker->xAxis()].transform(marker->xValue()),
                map[marker->yAxis()].transform(marker->yValue()),
                rect);
        }
    }
}

/*!
  \brief Draw a set of points of a curve.
  When observing an measurement while it is running, new points have to be
  added to an existing curve. drawCurve can be used to display them avoiding
  a complete redraw of the canvas.

  \param key curve key
  \param from index of the first point to be painted
  \param to index of the last point to be painted. If to < 0 the
         curve will be painted to its last point.
  \sa QwtCurve::draw
*/
void QwtPlot::drawCurve(long key, int from, int to)
{
    QwtPlotCurve *curve = d_curves->find(key);
    if ( !curve )
        return;

    QPainter p(canvas());

    p.setClipping(TRUE);
    p.setClipRect(canvas()->contentsRect());

    curve->draw(&p,
        canvasMap(curve->xAxis()), canvasMap(curve->yAxis()),
        from, to);

    if ( canvas()->cacheMode() && canvas()->cache())
    {
        QPainter cachePainter(canvas()->cache());
        cachePainter.translate(-canvas()->contentsRect().x(),
            -canvas()->contentsRect().y());

        curve->draw(&cachePainter,
            canvasMap(curve->xAxis()), canvasMap(curve->yAxis()),
            from, to);
    }
}

/*!
  \param axis Axis
  \return Map for the axis on the canvas. With this map pixel coordinates can
          translated to plot coordinates and vice versa.
  \sa QwtDiMap, QwtPlot::transform, QwtPlot::invTransform
  
*/
QwtDiMap QwtPlot::canvasMap(int axis) const
{
    QwtDiMap map;
    if ( !d_canvas )
        return map;

    const QwtScaleDiv &sd = d_as[axis].scaleDiv();
    map.setDblRange(sd.lBound(), sd.hBound(), sd.logScale());

    if ( axisEnabled(axis) )
    {
        const QwtScale *s = d_scale[axis];
        if ( axis == yLeft || axis == yRight )
        {
            int y = s->y() + s->startBorderDist() - d_canvas->y();
            int h = s->height() - s->startBorderDist() - s->endBorderDist();
            map.setIntRange(y + h - 1, y);
        }
        else
        {
            int x = s->x() + s->startBorderDist() - d_canvas->x();
            int w = s->width() - s->startBorderDist() - s->endBorderDist();
            map.setIntRange(x, x + w - 1);
        }
    }
    else
    {
        const int margin = plotLayout()->canvasMargin(axis);

        const QRect &canvasRect = d_canvas->contentsRect();
        if ( axis == yLeft || axis == yRight )
        {
            map.setIntRange(canvasRect.bottom() - margin, 
                canvasRect.top() + margin);
        }
        else
        {
            map.setIntRange(canvasRect.left() + margin, 
                canvasRect.right() - margin);
        }
    }
    return map;
}

/*!
  Change the margin of the plot. The margin is the space
  around all components.

  \param margin new margin
  \sa QwtPlotLayout::setMargin(), QwtPlot::margin(), QwtPlot::plotLayout()
*/
void QwtPlot::setMargin(int margin)
{
    if ( margin < 0 )
        margin = 0;

    if ( margin != d_layout->margin() )
    {
        d_layout->setMargin(margin);
        updateLayout();
    }
}

/*!
    \return margin
    \sa QwtPlot::setMargin(), QwtPlotLayout::margin(), QwtPlot::plotLayout()
*/
int QwtPlot::margin() const
{
    return d_layout->margin();
}

/*!
  \brief Change the background of the plotting area
  
  Sets c to QColorGroup::Background of all colorgroups of 
  the palette of the canvas. Using canvas()->setPalette()
  is a more powerful way to set these colors.
  \param c new background color
*/
void QwtPlot::setCanvasBackground(const QColor &c)
{
    QPalette p = d_canvas->palette();

    for ( int i = 0; i < QPalette::NColorGroups; i++ )
        p.setColor((QPalette::ColorGroup)i, QColorGroup::Background, c);

    canvas()->setPalette(p);
}

/*!
  Nothing else than: canvas()->palette().color(
        QPalette::Normal, QColorGroup::Background);
  
  \return the background color of the plotting area.
*/
const QColor & QwtPlot::canvasBackground() const
{
    return canvas()->palette().color(
        QPalette::Normal, QColorGroup::Background);
}

/*!
  \brief Change the border width of the plotting area
  Nothing else than canvas()->setLineWidth(w), 
  left for compatibility only.
  \param w new border width
*/
void QwtPlot::setCanvasLineWidth(int w)
{
    canvas()->setLineWidth(w);
}
 
/*! 
  Nothing else than: canvas()->lineWidth(), 
  left for compatibility only.
  \return the border width of the plotting area
*/
int QwtPlot::canvasLineWidth() const
{ 
    return canvas()->lineWidth();
}

#ifndef QWT_NO_COMPAT

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
  \sa setOutlineStyle()
*/
void QwtPlot::enableOutline(bool tf)
{
    d_canvas->enableOutline(tf);
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
  <dt>Qwt::Cross
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
  \sa enableOutline()
*/
void QwtPlot::setOutlineStyle(Qwt::Shape os)
{
    d_canvas->setOutlineStyle(os);
}

/*!
  \brief Specify a pen for the outline

  \warning Outlining functionality is obsolete: use QwtPlotPicker or
  QwtPlotZoomer.

  \param pn new pen
*/
void QwtPlot::setOutlinePen(const QPen &pn)
{
    d_canvas->setOutlinePen(pn);
}

/*!
  \return \c TRUE if the outline feature is enabled

  \warning Outlining functionality is obsolete: use QwtPlotPicker or
  QwtPlotZoomer.
*/
bool QwtPlot::outlineEnabled() const
{
     return d_canvas->outlineEnabled();
}

/*!
  \return the pen used to draw outlines

  \warning Outlining functionality is obsolete: use QwtPlotPicker or
  QwtPlotZoomer.
*/
const QPen & QwtPlot::outlinePen() const
{
    return d_canvas->outlinePen();
}

/*!
  \return the outline style

  \warning Outlining functionality is obsolete: use QwtPlotPicker or
  QwtPlotZoomer.

  \sa setOutlineStyle()
*/
Qwt::Shape QwtPlot::outlineStyle() const
{
    return d_canvas->outlineStyle();
}

#endif // ! QWT_NO_COMPAT

/*!
  \return \c TRUE if the specified axis exists, otherwise \c FALSE
  \param axis axis index
 */
bool QwtPlot::axisValid(int axis)
{
    return ((axis >= QwtPlot::yLeft) && (axis < QwtPlot::axisCnt));
}

/*!
  Called internally when the legend has been clicked on.
  Emits a legendClicked() signal.
*/

void QwtPlot::lgdClicked()
{
    if ( sender()->isWidgetType() )
    {
        long key = d_legend->key((QWidget *)sender());
        if ( key >= 0 )
            emit legendClicked(key);
    }
}

//! Remove all curves and markers
void QwtPlot::clear()
{
    d_legend->clear();
    d_curves->clear();
    d_markers->clear();
}


//! Remove all curves
void QwtPlot::removeCurves()
{
    d_curves->clear();
    d_legend->clear();
    autoRefresh();
}

//! Remove all markers
void QwtPlot::removeMarkers()
{
    d_markers->clear();
    autoRefresh();
}

/*!
  \brief Set or reset the autoLegend option
  If the autoLegend option is set, a item will be added
  to the legend whenever a curve is inserted.

  The autoLegend option is set to FALSE by default, which
  means that the user has to call enableLegend.
  \param tf \c TRUE or \c FALSE. Defaults to \c FALSE.
  \sa QwtPlot::enableLegend()
*/
void QwtPlot::setAutoLegend(bool tf)
{
    d_autoLegend = tf;
}

/*!
    \return TRUE if the autoLegend option is set.
*/
bool QwtPlot::autoLegend() const
{
    return d_autoLegend;
}


/*!
  \brief Enable or disable the legend
  \param enable \c TRUE (enabled) or \c FALSE (disabled)
  \param curveKey Key of a existing curve.
                  If curveKey < 0 the legends for all
                  curves will be updated.
  \sa QwtPlot::setAutoLegend()
  \sa QwtPlot::setLegendPosition()
*/
void QwtPlot::enableLegend(bool enable, long curveKey)
{
    bool isUpdateEnabled = d_legend->isUpdatesEnabled();
    d_legend->setUpdatesEnabled(FALSE);

    if ( curveKey < 0 ) // legends for all curves
    {
        if ( enable )
        {
            if ( d_legend->itemCount() < d_curves->count() )
            {
                // not all curves have a legend

                d_legend->clear();

                QwtPlotCurveIterator itc = curveIterator();
                for ( const QwtPlotCurve *curve = itc.toFirst();
                    curve != 0; curve = ++itc )
                {
                    insertLegendItem(itc.currentKey());
                }
            }
        }
        else
        {
            d_legend->clear();
        }
    }
    else
    {
        QWidget *legendItem = d_legend->findItem(curveKey);
        if ( enable )
        {
            if ( d_curves->find(curveKey) && !legendItem )
                insertLegendItem(curveKey);
        }
        else
            delete legendItem;
    }

    d_legend->setUpdatesEnabled(isUpdateEnabled);
    updateLayout();
}

/*!
  \param curveKey Curve key.
  \return \c TRUE if legend is enabled, otherwise \c FALSE
*/

bool QwtPlot::legendEnabled(long curveKey) const
{
    return d_legend->findItem(curveKey) != 0;
}

/*!
  Specify the position of the legend within the widget.
  If the position legend is \c QwtPlot::Left or \c QwtPlot::Right
  the legend will be organized in one column from top to down. 
  Otherwise the legend items will be placed be placed in a table 
  with a best fit number of columns from left to right.
       
  \param pos The legend's position. Valid values are \c QwtPlot::Left,
           \c QwtPlot::Right, \c QwtPlot::Top, \c QwtPlot::Bottom.
  \param ratio Ratio between legend and the bounding rect
               of title, canvas and axes. The legend will be shrinked
               if it would need more space than the given ratio.
               The ratio is limited to ]0.0 .. 1.0]. In case of <= 0.0
               it will be reset to the default ratio.
               The default vertical/horizontal ratio is 0.33/0.5.

  \sa QwtPlot::legendPosition(), QwtPlotLayout::setLegendPosition()
*/
void QwtPlot::setLegendPosition(QwtPlot::Position pos, double ratio)
{
    if (pos != d_layout->legendPosition())
    {
        d_layout->setLegendPosition(pos, ratio);

        QLayout *l = d_legend->contentsWidget()->layout();
        if ( l && l->inherits("QwtDynGridLayout") )
        {
            QwtDynGridLayout *tl = (QwtDynGridLayout *)l;
            if ( d_layout->legendPosition() == QwtPlot::Top ||
                d_layout->legendPosition() == QwtPlot::Bottom )
            {
                tl->setMaxCols(0); // unlimited
            }
            else
                tl->setMaxCols(1); // 1 column: align vertical
        }

        updateLayout();
        updateTabOrder();
    }
}

/*!
  Specify the position of the legend within the widget.
  If the position legend is \c QwtPlot::Left or \c QwtPlot::Right
  the legend will be organized in one column from top to down. 
  Otherwise the legend items will be placed be placed in a table 
  with a best fit number of columns from left to right.
       
  \param pos The legend's position. Valid values are \c QwtPlot::Left,
           \c QwtPlot::Right, \c QwtPlot::Top, \c QwtPlot::Bottom.

  \sa QwtPlot::legendPosition(), QwtPlotLayout::setLegendPosition()
*/
void QwtPlot::setLegendPosition(QwtPlot::Position pos)
{
    setLegendPosition(pos, 0.0);
}

/*!
    \return position of the legend
    \sa QwtPlot::setLegendPosition(), QwtPlotLayout::legendPosition()
*/
QwtPlot::Position QwtPlot::legendPosition() const
{
    return d_layout->legendPosition();
}

#ifndef QWT_NO_COMPAT

/*!
  Specify the position of the legend within the widget.
  If the position legend is \c QwtPlot::Left or \c QwtPlot::Right
  the legend will be organized in one column from top to down. 
  Otherwise the legend items will be placed be placed in a table 
  with a best fit number of columns from left to right.
       
  \param pos The legend's position. Valid values are \c QwtPlot::Left,
           \c QwtPlot::Right, \c QwtPlot::Top, \c QwtPlot::Bottom.
  \param ratio Ratio between legend and the bounding rect
               of title, canvas and axes. The legend will be shrinked
               if it would need more space than the given ratio.
               The ratio is limited to ]0.0 .. 1.0]. In case of <= 0.0
               it will be reset to the default ratio.
               The default vertical/horizontal ratio is 0.33/0.5.

  \sa QwtPlot::legendPosition(), QwtPlotLayout::setLegendPosition()
  \warning This function is deprecated. Use QwtPlot::setLegendPosition().
*/
void QwtPlot::setLegendPos(int pos, double ratio)
{
    setLegendPosition(QwtPlot::Position(pos), ratio);
}

/*!
  \return position of the legend
  \sa QwtPlot::setLegendPosition(), QwtPlotLayout::legendPosition()
  \warning This function is deprecated. Use QwtPlot::legendPosition().  
*/
int QwtPlot::legendPos() const
{
    return d_layout->legendPosition();
}

#endif // !QWT_NO_COMPAT

/*!
  \brief Change the font of the legend items
  \param f new font
*/
void QwtPlot::setLegendFont(const QFont &f)
{
    d_legend->setFont(f);
    if (d_legend->isVisible())
        updateLayout();
}

/*!
  \brief Change the legend's frame style
  \param st Frame Style. See Qt manual on QFrame.
*/
void QwtPlot::setLegendFrameStyle(int st)
{
    d_legend->setFrameStyle(st);
    updateLayout();
}

/*!
  \return the frame style of the legend
*/
int QwtPlot::legendFrameStyle() const
{
    return d_legend->frameStyle();
}

/*!
  \return the font of the legend items
*/
const QFont QwtPlot::legendFont() const
{
    return d_legend->font();
}

/*!
  Set the identifier display policy of the legend.
  \param policy new policy.
  \param mode new mode.
  \sa QwtLegend::setDisplayPolicy, QwtLegend::LegendDisplayPolicy
*/
void QwtPlot::setLegendDisplayPolicy(
    QwtLegend::LegendDisplayPolicy policy, int mode)
{
    d_legend->setDisplayPolicy(policy, mode);
    
    for (QwtPlotCurveIterator iter=curveIterator(); iter.current(); ++iter)
        updateLegendItem(iter.currentKey());
}

// Local Variables:
// mode: C++
// c-file-style: "stroustrup"
// indent-tabs-mode: nil
// End:
