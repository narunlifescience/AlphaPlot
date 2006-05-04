/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

// vim: expandtab

#include <qlabel.h>
#include <qpainter.h>
#include <qpaintdevicemetrics.h>
#include "qwt_painter.h"
#include "qwt_legend.h"
#include "qwt_plot.h"
#include "qwt_plot_layout.h"
#include "qwt_plot_dict.h"
#include "qwt_rect.h"
#include "qwt_dyngrid_layout.h"
#include "qwt_scale.h"
#include "qwt_text.h"
#include "qwt_math.h"

/*!
  \brief Print the plot to a \c QPaintDevice (\c QPrinter)
  This function prints the contents of a QwtPlot instance to
  \c QPaintDevice object. The size is derived from its device
  metrics.

  \param paintDev device to paint on, often a printer
  \param pfilter print filter
  \sa QwtPlot::print
  \sa QwtPlotPrintFilter

  \attention Using a \c QPrinter in \c QPrinter::HighResolution mode may
  expose a bug in the \c QFontDatabase class in Qt-X11 and Qt-Embedded.

  \attention The problem concerns the font metrics: the height and width of
  a font should measure 8 times more pixels on a 600 dpi paint device than
  on a 75 dpi paint device.
  High resolution print tests on a Mandrake-8.2 system with a 75
  dpi screen show (test program available on request):
  - Qt finds more fonts with the X Font Server enabled than disabled (use
    qtconfig). Enabling or disabling the font server has no consequences for
    the print quality.
  - Qt-3.0.7 finds significantly more fonts than its successors and prints
    well.
  - Qt-3.1.2 finds less fonts than Qt-3.0.7 with the X Font Server enabled
    and prints well.
  - Qt-3.2.0 finds less fonts than Qt-3.0.7 and printing only works for rich
    text. All plain text is printed at 75 dpi instead of 600 dpi.  Some
    standard X Window fonts as "Helvetica [Adobe]" do not print well as
    rich text.
  - Qt-3.2.1 finds a few more fonts than Qt-3.2.0 and some standard
    non-scalable X Window fonts as "Helvetica [Adobe]" do not print well
    as rich and plain text.
  - Qt-3.2.2 still has problems with the same fonts as Qt-3.2.1, but there
    is improvement (text printed with non-scalable X Windows fonts is barely
    readable and looks ugly, because Qt gets the font metrics wrong).
  - Qt-3.2.3 prints well (but finds less fonts than Qt-3.0.7).

 \attention Recommendations:
  - If you can, upgrade Qt-3.2.x to Qt-3.2.3.
  - If you cannot, patches for Qt-3.2.1 and Qt-3.2.2 are available on request.
  - Do not use Qt-3.2.0.
  - Do not use non-scalable fonts for printing, especially if you print to
    postscript files that are to be included in other documents.
*/

void QwtPlot::print(QPaintDevice &paintDev,
   const QwtPlotPrintFilter &pfilter) const
{
    QPaintDeviceMetrics mpr(&paintDev);

    QRect rect(0, 0, mpr.width(), mpr.height());
    double aspect = double(rect.width())/double(rect.height());
    if ((aspect < 1.0))
        rect.setHeight(int(aspect*rect.width()));

    QPainter p(&paintDev);
    print(&p, rect, pfilter);
}

/*!
  \brief Paint the plot into a given rectangle.
  Paint the contents of a QwtPlot instance into a given rectangle.

  \param painter Painter
  \param plotRect Bounding rectangle
  \param pfilter Print filter
  \sa QwtPlotPrintFilter
*/
void QwtPlot::print(QPainter *painter, const QRect &plotRect,
        const QwtPlotPrintFilter &pfilter) const
{
    int axis;

    if ( painter == 0 || !painter->isActive() ||
            !plotRect.isValid() || size().isNull() )
       return;

    painter->save();

    // All paint operations need to be scaled according to
    // the paint device metrics. 

    QwtPainter::setMetricsMap(this, painter->device());

#if QT_VERSION < 300 
    if ( painter->device()->isExtDev() )
    {
        QPaintDeviceMetrics metrics(painter->device());
        if ( metrics.logicalDpiX() == 72 && metrics.logicalDpiY() == 72 )
        {
            // In Qt 2.x QPrinter returns hardcoded wrong metrics.
            // So scaling won´t work: we reset to screen resolution

            QwtPainter::setMetricsMap(this, this);
        }
    }
#endif

    const QwtMetricsMap &metricsMap = QwtPainter::metricsMap();

    // It is almost impossible to integrate into the Qt layout
    // framework, when using different fonts for printing
    // and screen. To avoid writing different and Qt unconform
    // layout engines we change the widget attributes, print and 
    // reset the widget attributes again. This way we produce a lot of
    // useless layout events ...

    pfilter.apply((QwtPlot *)this);

    int baseLineDists[QwtPlot::axisCnt];
    if ( !(pfilter.options() & QwtPlotPrintFilter::PrintCanvasBackground) )
    {
        // In case of no background we set the backbone of
        // the scale on the frame of the canvas.

        for (axis = 0; axis < QwtPlot::axisCnt; axis++ )
        {
            if ( d_scale[axis] )
            {
                baseLineDists[axis] = d_scale[axis]->baseLineDist();
                d_scale[axis]->setBaselineDist(0);
            }
        }
    }
    // Calculate the layout for the print.

    int layoutOptions = QwtPlotLayout::IgnoreScrollbars 
        | QwtPlotLayout::IgnoreFrames;
    if ( !(pfilter.options() & QwtPlotPrintFilter::PrintMargin) )
        layoutOptions |= QwtPlotLayout::IgnoreMargin;
    if ( !(pfilter.options() & QwtPlotPrintFilter::PrintLegend) )
        layoutOptions |= QwtPlotLayout::IgnoreLegend;

    d_layout->activate(this, QwtPainter::metricsMap().deviceToLayout(plotRect), 
        layoutOptions);

    if ((pfilter.options() & QwtPlotPrintFilter::PrintTitle)
        && (!d_lblTitle->text().isEmpty()))
    {
        printTitle(painter, d_layout->titleRect());
    }

    if ( (pfilter.options() & QwtPlotPrintFilter::PrintLegend)
        && !d_legend->isEmpty() )
    {
        printLegend(painter, d_layout->legendRect());
    }

    for ( axis = 0; axis < QwtPlot::axisCnt; axis++ )
    {
        if (d_scale[axis])
        {
            int baseDist = d_scale[axis]->baseLineDist();

            int startDist, endDist;
            d_scale[axis]->minBorderDist(startDist, endDist);

            printScale(painter, axis, startDist, endDist,
                baseDist, d_layout->scaleRect(axis));
        }
    }

    const QRect canvasRect = metricsMap.layoutToDevice(d_layout->canvasRect());

    // When using QwtPainter all sizes where computed in pixel
    // coordinates and scaled by QwtPainter later. This limits
    // the precision to screen resolution. A much better solution
    // is to scale the maps and print in unlimited resolution.

    QwtArray<QwtDiMap> map(axisCnt);
    for (axis = 0; axis < axisCnt; axis++)
    {
        const QwtScaleDiv &scaleDiv = d_as[axis].scaleDiv();
        map[axis].setDblRange(scaleDiv.lBound(),
            scaleDiv.hBound(), scaleDiv.logScale());

        double from, to;
        if ( axisEnabled(axis) )
        {
            const int sDist = d_scale[axis]->startBorderDist();
            const int eDist = d_scale[axis]->endBorderDist();
            const QRect &scaleRect = d_layout->scaleRect(axis);

            if ( axis == xTop || axis == xBottom )
            {
                from = metricsMap.layoutToDeviceX(scaleRect.left() + sDist);
                to = metricsMap.layoutToDeviceX(scaleRect.right() - eDist);
            }
            else
            {
                from = metricsMap.layoutToDeviceY(scaleRect.bottom() - sDist);
                to = metricsMap.layoutToDeviceY(scaleRect.top() + eDist);
            }
        }
        else
        {
            const int margin = plotLayout()->canvasMargin(axis);

            const QRect &canvasRect = plotLayout()->canvasRect();
            if ( axis == yLeft || axis == yRight )
            {
                from = metricsMap.layoutToDeviceX(canvasRect.bottom() - margin);
                to = metricsMap.layoutToDeviceX(canvasRect.top() + margin);
            }
            else
            {
                from = metricsMap.layoutToDeviceY(canvasRect.left() + margin);
                to = metricsMap.layoutToDeviceY(canvasRect.right() - margin);
            }
        }
        map[axis].setIntRange( qwtInt(from), qwtInt(to));
    }


    // The maps are already scaled. 
    QwtPainter::setMetricsMap(painter->device(), painter->device());

    printCanvas(painter, canvasRect, map, pfilter);

    QwtPainter::resetMetricsMap();

    d_layout->invalidate();

    // reset all widgets with their original attributes.
    if ( !(pfilter.options() & QwtPlotPrintFilter::PrintCanvasBackground) )
    {
        // restore the previous base line dists

        for (axis = 0; axis < QwtPlot::axisCnt; axis++ )
        {
            if ( d_scale[axis] )
                d_scale[axis]->setBaselineDist(baseLineDists[axis]);
        }
    }

    pfilter.reset((QwtPlot *)this);

    painter->restore();
}

/*!
  Print the title into a given rectangle.

  \param painter Painter
  \param rect Bounding rectangle
*/

void QwtPlot::printTitle(QPainter *painter, const QRect &rect) const
{
    QwtText *text = QwtText::makeText(
        d_lblTitle->text(), d_lblTitle->textFormat(),
        d_lblTitle->alignment(), d_lblTitle->font(),
        d_lblTitle->palette().color(
            QPalette::Active, QColorGroup::Foreground)); 

    text->draw(painter, rect);
    delete text;
}

/*!
  Print the legend into a given rectangle.

  \param painter Painter
  \param rect Bounding rectangle
*/

void QwtPlot::printLegend(QPainter *painter, const QRect &rect) const
{
    if ( !d_legend || d_legend->isEmpty() )
        return;

    QLayout *l = d_legend->contentsWidget()->layout();
    if ( l == 0 || !l->inherits("QwtDynGridLayout") )
        return;

    QwtDynGridLayout *legendLayout = (QwtDynGridLayout *)l;

    uint numCols = legendLayout->columnsForWidth(rect.width());
    QValueList<QRect> itemRects = 
        legendLayout->layoutItems(rect, numCols);

    int index = 0;

    QLayoutIterator layoutIterator = legendLayout->iterator();
    for ( QLayoutItem *item = layoutIterator.current(); 
        item != 0; item = ++layoutIterator)
    {
        QWidget *w = item->widget();
        if ( w )
        {
            painter->save();
            painter->setClipping(TRUE);
            QwtPainter::setClipRect(painter, itemRects[index]);

            printLegendItem(painter, w, itemRects[index]);

            index++;
            painter->restore();
        }
    }
}

/*!
  Print the legend item into a given rectangle.

  \param painter Painter
  \param w Widget representing a legend item
  \param rect Bounding rectangle
*/

void QwtPlot::printLegendItem(QPainter *painter, 
    const QWidget *w, const QRect &rect) const
{
    const QwtLegendItem *item = NULL;

    if ( w->inherits("QwtLegendButton") )
        item = (QwtLegendButton *)w;

    if ( w->inherits("QwtLegendLabel") )
        item = (QwtLegendLabel *)w;

    if ( item )
    {
        painter->setFont(w->font());
        item->drawItem(painter, rect);
    }
}

/*!
  \brief Paint a scale into a given rectangle.
  Paint the scale into a given rectangle.

  \param painter Painter
  \param axis Axis
  \param startDist Start border distance
  \param endDist End border distance
  \param baseDist Base distance
  \param rect Bounding rectangle
*/

void QwtPlot::printScale(QPainter *painter,
    int axis, int startDist, int endDist, int baseDist, 
    const QRect &rect) const
{
    if (!d_axisEnabled[axis])
        return;

    QwtScaleDraw::Orientation o;
    int x, y, w;

    switch(axis)
    {
        case yLeft:
        {
            x = rect.right() - baseDist;
            y = rect.y() + startDist;
            w = rect.height() - startDist - endDist;
            o = QwtScaleDraw::Left;
            break;
        }
        case yRight:
        {
            x = rect.left() + baseDist;
            y = rect.y() + startDist;
            w = rect.height() - startDist - endDist;
            o = QwtScaleDraw::Right;
            break;
        }
        case xTop:
        {
            x = rect.left() + startDist;
            y = rect.bottom() - baseDist;
            w = rect.width() - startDist - endDist;
            o = QwtScaleDraw::Top;
            break;
        }
        case xBottom:
        {
            x = rect.left() + startDist;
            y = rect.top() + baseDist;
            w = rect.width() - startDist - endDist;
            o = QwtScaleDraw::Bottom;
            break;
        }
        default:
            return;
    }

    const QwtScale *scale = d_scale[axis];
    scale->drawTitle(painter, o, rect);

    painter->save();
    QPen pen(painter->pen());
    pen.setColor(
        scale->palette().color(QPalette::Active, QColorGroup::Foreground));
    painter->setPen(pen);
    painter->setFont(scale->font());

    QwtScaleDraw *sd = (QwtScaleDraw *)scale->scaleDraw();
    int xSd = sd->x();
    int ySd = sd->y();
    int lengthSd = sd->length();

    sd->setGeometry(x, y, w, o);
    sd->draw(painter);
    sd->setGeometry(xSd, ySd, lengthSd, o); // reset previous values
    painter->restore();
}

/*!
  Print the canvas into a given rectangle.

  \param painter Painter
  \param map Maps mapping between plot and paint device coordinates
  \param canvasRect Bounding rectangle
  \param pfilter Print filter
  \sa QwtPlotPrintFilter
*/

void QwtPlot::printCanvas(QPainter *painter, const QRect &canvasRect,
    const QwtArray<QwtDiMap> &map, const QwtPlotPrintFilter &pfilter) const
{
    if ( pfilter.options() & QwtPlotPrintFilter::PrintCanvasBackground )
    {
        QwtPainter::fillRect(painter, QRect(canvasRect.x(), canvasRect.y(), 
            canvasRect.width() - 1, canvasRect.height() - 1),
            canvasBackground());
    }
    else
        QwtPainter::drawRect(painter, canvasRect.x() - 1, canvasRect.y() - 1,
            canvasRect.width() + 1, canvasRect.height() + 1);

    painter->setClipping(TRUE);
    QwtPainter::setClipRect(painter, canvasRect);

    drawCanvasItems(painter, canvasRect, map, pfilter);
}

// Local Variables:
// mode: C++
// c-file-style: "stroustrup"
// indent-tabs-mode: nil
// End:
