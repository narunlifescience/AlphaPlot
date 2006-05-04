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
#include "qwt_plot.h"
#include "qwt_plot_dict.h"
#include "qwt_legend.h"
#include "qwt_scale.h"
#include "qwt_plot_printfilter.h"

class QwtPlotPrintFilterCache
{
    friend class QwtPlotPrintFilter;

protected:
    QwtPlotPrintFilterCache()
    {
        legendFonts.setAutoDelete(TRUE);
        curveColors.setAutoDelete(TRUE);
        curveSymbolBrushColors.setAutoDelete(TRUE);
        curveSymbolPenColors.setAutoDelete(TRUE);
        markerFonts.setAutoDelete(TRUE);
        markerLabelColors.setAutoDelete(TRUE);
        markerLineColors.setAutoDelete(TRUE);
        markerSymbolBrushColors.setAutoDelete(TRUE);
        markerSymbolPenColors.setAutoDelete(TRUE);
    }

    QColor titleColor;
    QFont titleFont;

    QColor scaleColor[4];
    QFont scaleFont[4];
    QColor scaleTitleColor[4];
    QFont scaleTitleFont[4];

    QIntDict<QFont> legendFonts;

    QColor widgetBackground;
    QColor canvasBackground;
    QColor gridColors[2];

    QIntDict<QColor> curveColors;
    QIntDict<QColor> curveSymbolBrushColors;
    QIntDict<QColor> curveSymbolPenColors;

    QIntDict<QFont> markerFonts;
    QIntDict<QColor> markerLabelColors;
    QIntDict<QColor> markerLineColors;
    QIntDict<QColor> markerSymbolBrushColors;
    QIntDict<QColor> markerSymbolPenColors;
};

/*!
  Sets filter options to QwtPlotPrintFilter::PrintAll
*/  

QwtPlotPrintFilter::QwtPlotPrintFilter():
    d_options(PrintAll),
    d_cache(0)
{
}

//! Destructor
QwtPlotPrintFilter::~QwtPlotPrintFilter()
{
    delete d_cache;
}

/*!
  \brief Modifies a color for printing
  \param c Color to be modified
  \param item Type of item where the color belongs
  \param id Optional id of the item (curveId/markerId)
  \return Modified color.

  In case of !(QwtPlotPrintFilter::options() & PrintBackground) 
  MajorGrid is modified to Qt::darkGray, MinorGrid to Qt::gray. 
  All other colors are returned unmodified.
*/

QColor QwtPlotPrintFilter::color(const QColor &c, Item item, int) const
{
    if ( !(options() & PrintCanvasBackground))
    {
        switch(item)
        {
            case MajorGrid:
                return Qt::darkGray;
            case MinorGrid:
                return Qt::gray;
            default:;
        }
    }
    return c;
}

/*!
  \brief Modifies a font for printing
  \param f Font to be modified
  \param item Type of item where the font belongs
  \param id Optional id of the item (curveId/markerId)

  All fonts are returned unmodified
*/

QFont QwtPlotPrintFilter::font(const QFont &f, Item, int) const
{
    return f;
}

/*! 
  Change color and fonts of a plot
  \sa QwtPlotPrintFilter::apply
*/
void QwtPlotPrintFilter::apply(QwtPlot *plot) const
{
    QwtPlotPrintFilter *that = (QwtPlotPrintFilter *)this;

    delete that->d_cache;
    that->d_cache = new QwtPlotPrintFilterCache;

    QwtPlotPrintFilterCache &cache = *that->d_cache;

    if ( plot->d_lblTitle )
    {
        QPalette palette = plot->d_lblTitle->palette();
        cache.titleColor = palette.color(
            QPalette::Active, QColorGroup::Foreground);
        palette.setColor(QPalette::Active, QColorGroup::Foreground,
                         color(cache.titleColor, Title));
        plot->d_lblTitle->setPalette(palette);

        cache.titleFont = plot->d_lblTitle->font();
        plot->d_lblTitle->setFont(font(cache.titleFont, Title));
    }
    if ( plot->d_legend )
    {
        QIntDictIterator<QWidget> it = plot->d_legend->itemIterator();
        for ( QWidget *w = it.toFirst(); w != 0; w = ++it)
        {
            const int key = it.currentKey();

            cache.legendFonts.insert(it.currentKey(), new QFont(w->font()));
            w->setFont(font(w->font(), Legend, key));

            if ( w->inherits("QwtLegendButton") )
            {
                QwtLegendButton *btn = (QwtLegendButton *)w;

                QwtSymbol symbol = btn->symbol();
                QPen pen = symbol.pen();
                QBrush brush = symbol.brush();

                pen.setColor(color(pen.color(), CurveSymbol, key));
                brush.setColor(color(brush.color(), CurveSymbol, key));

                symbol.setPen(pen);
                symbol.setBrush(brush);
                btn->setSymbol(symbol);

                pen = btn->curvePen();
                pen.setColor(color(pen.color(), Curve, key));
                btn->setCurvePen(pen);
            }
        }
    }
    for ( int axis = 0; axis < QwtPlot::axisCnt; axis++ )
    {
        QwtScale *scale = plot->d_scale[axis];
        if ( scale )
        {
            cache.scaleColor[axis] = scale->palette().color(
                QPalette::Active, QColorGroup::Foreground);
            QPalette palette = scale->palette();
            palette.setColor(QPalette::Active, QColorGroup::Foreground,
                             color(cache.scaleColor[axis], AxisScale, axis));
            scale->setPalette(palette);

            cache.scaleFont[axis] = scale->font();
            scale->setFont(font(cache.scaleFont[axis], AxisScale, axis));

            cache.scaleTitleColor[axis] = scale->titleColor();
            scale->setTitleColor(
                color(cache.scaleTitleColor[axis], AxisTitle, axis));

            cache.scaleTitleFont[axis] = scale->titleFont();
            scale->setTitleFont(
                font(cache.scaleTitleFont[axis], AxisTitle, axis));

            int startDist, endDist;
            scale->minBorderDist(startDist, endDist);
            scale->setBorderDist(startDist, endDist);
        }
    }

    cache.widgetBackground = plot->backgroundColor();
    plot->setBackgroundColor(color(cache.widgetBackground, WidgetBackground));

    cache.canvasBackground = plot->canvasBackground();
    plot->setCanvasBackground(color(cache.canvasBackground, CanvasBackground));

    QPen pen = plot->d_grid->majPen();
    cache.gridColors[0] = pen.color();
    pen.setColor(color(pen.color(), MajorGrid));
    plot->d_grid->setMajPen(pen);

    pen = plot->d_grid->minPen();
    cache.gridColors[1] = pen.color();
    pen.setColor(color(pen.color(), MinorGrid));
    plot->d_grid->setMinPen(pen);
    
    QIntDictIterator<QwtPlotCurve> itc(*plot->d_curves);
    for (QwtPlotCurve *c = itc.toFirst(); c != 0; c = ++itc )
    {
        const int key = itc.currentKey();

        QwtSymbol symbol = c->symbol();

        QPen pen = symbol.pen();
        cache.curveSymbolPenColors.insert(key, new QColor(pen.color()));
        pen.setColor(color(pen.color(), CurveSymbol, key));
        symbol.setPen(pen);

        QBrush brush = symbol.brush();
        cache.curveSymbolBrushColors.insert(key, new QColor(brush.color()));
        brush.setColor(color(brush.color(), CurveSymbol, key));
        symbol.setBrush(brush);

        c->setSymbol(symbol);

        pen = c->pen();
        cache.curveColors.insert(key, new QColor(pen.color()));
        pen.setColor(color(pen.color(), Curve, key));
        c->setPen(pen);
    }

    QIntDictIterator<QwtPlotMarker> itm(*plot->d_markers);
    for (QwtPlotMarker *m = itm.toFirst(); m != 0; m = ++itm )
    {
        const int key = itm.currentKey();

        cache.markerFonts.insert(key, new QFont(m->font()));
        m->setFont(font(m->font(), Marker, key));

        QPen pen = m->labelPen();
        cache.markerLabelColors.insert(key, new QColor(pen.color()));
        pen.setColor(color(pen.color(), Marker, key));
        m->setLabelPen(pen);
        
        pen = m->linePen();
        cache.markerLineColors.insert(key, new QColor(pen.color()));
        pen.setColor(color(pen.color(), Marker, key));
        m->setLinePen(pen);

        QwtSymbol symbol = m->symbol();

        pen = symbol.pen();
        cache.markerSymbolPenColors.insert(key, new QColor(pen.color()));
        pen.setColor(color(pen.color(), MarkerSymbol, key));
        symbol.setPen(pen);

        QBrush brush = symbol.brush();
        cache.markerSymbolBrushColors.insert(key, new QColor(brush.color()));
        brush.setColor(color(brush.color(), MarkerSymbol, key));
        symbol.setBrush(brush);

        m->setSymbol(symbol);
    }
}

/*! 
   Reset color and fonts of a plot
   \sa QwtPlotPrintFilter::apply
*/
void QwtPlotPrintFilter::reset(QwtPlot *plot) const
{
    if ( d_cache == 0 )
        return;

    QFont *font;
    QColor *color;

    if ( plot->d_lblTitle )
    {
        QPalette palette = plot->d_lblTitle->palette();
        palette.setColor(
            QPalette::Active, QColorGroup::Foreground, d_cache->titleColor);
        plot->d_lblTitle->setPalette(palette);

        plot->d_lblTitle->setFont(d_cache->titleFont);
    }

    if ( plot->d_legend )
    {
        QIntDictIterator<QWidget> it = plot->d_legend->itemIterator();
        for ( QWidget *w = it.toFirst(); w != 0; w = ++it)
        {
            const int key = it.currentKey();

            font = d_cache->legendFonts.find(key);
            if ( font )
                w->setFont(*font);

            if ( w->inherits("QwtLegendButton") )
            {
                QwtLegendButton *btn = (QwtLegendButton *)w;

                QwtSymbol symbol = btn->symbol();
                color = d_cache->curveSymbolPenColors.find(key);
                if ( color )
                {
                    QPen pen = symbol.pen();
                    pen.setColor(*color);
                    symbol.setPen(pen);
                }

                color = d_cache->curveSymbolBrushColors.find(key);
                if ( color )
                {
                    QBrush brush = symbol.brush();
                    brush.setColor(*color);
                    symbol.setBrush(brush);
                }
                btn->setSymbol(symbol);

                color = d_cache->curveColors.find(key);
                if ( color )
                {
                    QPen pen = btn->curvePen();
                    pen.setColor(*color);
                    btn->setCurvePen(pen);
                }
            }
        }
    }
    for ( int axis = 0; axis < QwtPlot::axisCnt; axis++ )
    {
        QwtScale *scale = plot->d_scale[axis];
        if ( scale )
        {
            QPalette palette = scale->palette();
            palette.setColor(QPalette::Active, QColorGroup::Foreground,
                             d_cache->scaleColor[axis]);
            scale->setPalette(palette);
            scale->setFont(d_cache->scaleFont[axis]);

            scale->setTitleColor(d_cache->scaleTitleColor[axis]);
            scale->setTitleFont(d_cache->scaleTitleFont[axis]);

            int startDist, endDist;
            scale->minBorderDist(startDist, endDist);
            scale->setBorderDist(startDist, endDist);
        }
    }

    plot->setBackgroundColor(d_cache->widgetBackground);
    plot->setCanvasBackground(d_cache->canvasBackground);

    QPen pen = plot->d_grid->majPen();
    pen.setColor(d_cache->gridColors[0]);
    plot->d_grid->setMajPen(pen);

    pen = plot->d_grid->minPen();
    pen.setColor(d_cache->gridColors[1]);
    plot->d_grid->setMinPen(pen);
    
    QIntDictIterator<QwtPlotCurve> itc(*plot->d_curves);
    for (QwtPlotCurve *c = itc.toFirst(); c != 0; c = ++itc )
    {
        const int key = itc.currentKey();

        QwtSymbol symbol = c->symbol();

        color = d_cache->curveSymbolPenColors.find(key);
        if ( color )
        {
            QPen pen = symbol.pen();
            pen.setColor(*color);
            symbol.setPen(pen);
        }

        color = d_cache->curveSymbolBrushColors.find(key);
        if ( color )
        {
            QBrush brush = symbol.brush();
            brush.setColor(*color);
            symbol.setBrush(brush);
        }
        c->setSymbol(symbol);

        color = d_cache->curveColors.find(key);
        if ( color )
        {
            QPen pen = c->pen();
            pen.setColor(*color);
            c->setPen(pen);
        }
    }

    QIntDictIterator<QwtPlotMarker> itm(*plot->d_markers);
    for (QwtPlotMarker *m = itm.toFirst(); m != 0; m = ++itm )
    {
        const int key = itm.currentKey();

        font = d_cache->markerFonts.find(key);
        if ( font )
            m->setFont(*font);

        color = d_cache->markerLabelColors.find(key);
        if ( color )
        {
            QPen pen = m->labelPen();
            pen.setColor(*color);
            m->setLabelPen(pen);
        }

        color = d_cache->markerLineColors.find(key);
        if ( color )
        {
            QPen pen = m->linePen();
            pen.setColor(*color);
            m->setLinePen(pen);
        }
        
        QwtSymbol symbol = m->symbol();

        color = d_cache->markerSymbolPenColors.find(key);
        if ( color )
        {
            QPen pen = symbol.pen();
            pen.setColor(*color);
            symbol.setPen(pen);
        }

        color = d_cache->markerSymbolBrushColors.find(key);
        if ( color )
        {
            QBrush brush = symbol.brush();
            brush.setColor(*color);
            symbol.setBrush(brush);
        }

        m->setSymbol(symbol);

    }

    QwtPlotPrintFilter *that = (QwtPlotPrintFilter *)this;
    delete that->d_cache;
    that->d_cache = 0;
}

// Local Variables:
// mode: C++
// c-file-style: "stroustrup"
// indent-tabs-mode: nil
// End:
