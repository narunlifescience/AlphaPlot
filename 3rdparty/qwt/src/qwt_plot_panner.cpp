/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

// vim: expandtab

#include "qwt_scale_div.h"
#include "qwt_plot.h"
#include "qwt_plot_canvas.h"
#include "qwt_plot_panner.h"

class QwtPlotPanner::PrivateData
{
public:
    PrivateData()
    {
        for ( int axis = 0; axis < QwtPlot::axisCnt; axis++ )
            isAxisEnabled[axis] = true;
    }

    bool isAxisEnabled[QwtPlot::axisCnt];
};

QwtPlotPanner::QwtPlotPanner(QwtPlotCanvas *canvas):
    QwtPanner(canvas)
{
    d_data = new PrivateData();

    connect(this, SIGNAL(panned(int, int)),
        SLOT(moveCanvas(int, int)));
}

QwtPlotPanner::~QwtPlotPanner()
{
    delete d_data;
}

void QwtPlotPanner::setAxisEnabled(int axis, bool on)
{
    if ( axis >= 0 && axis <= QwtPlot::axisCnt )
        d_data->isAxisEnabled[axis] = on;
}

bool QwtPlotPanner::isAxisEnabled(int axis) const
{
    if ( axis >= 0 && axis <= QwtPlot::axisCnt )
        return d_data->isAxisEnabled[axis];

    return true;
}

//! Return observed plot canvas
QwtPlotCanvas *QwtPlotPanner::canvas()
{
    QWidget *w = parentWidget();
    if ( w && w->inherits("QwtPlotCanvas") )
        return (QwtPlotCanvas *)w;

    return NULL;
}

//! Return Observed plot canvas
const QwtPlotCanvas *QwtPlotPanner::canvas() const
{
    return ((QwtPlotPanner *)this)->canvas();
}

//! Return plot widget, containing the observed plot canvas
QwtPlot *QwtPlotPanner::plot()
{
    QObject *w = canvas();
    if ( w )
    {
        w = w->parent();
        if ( w && w->inherits("QwtPlot") )
            return (QwtPlot *)w;
    }

    return NULL;
}

//! Return plot widget, containing the observed plot canvas
const QwtPlot *QwtPlotPanner::plot() const
{
    return ((QwtPlotPanner *)this)->plot();
}

void QwtPlotPanner::moveCanvas(int dx, int dy)
{
    if ( dx == 0 && dy == 0 )
        return;

    QwtPlot *plot = QwtPlotPanner::plot();
    if ( plot == NULL )
        return;
    
    const bool doReplot = plot->autoReplot();
    plot->setAutoReplot(false);

    for ( int axis = 0; axis < QwtPlot::axisCnt; axis++ )
    {
        if ( !d_data->isAxisEnabled[axis] )
            continue;

        const QwtScaleMap map = plot->canvasMap(axis);

        const int i1 = map.transform(plot->axisScaleDiv(axis)->lBound());
        const int i2 = map.transform(plot->axisScaleDiv(axis)->hBound());

        double d1, d2;
        if ( axis == QwtPlot::xBottom || axis == QwtPlot::xTop )
        {
            d1 = map.invTransform(i1 - dx);
            d2 = map.invTransform(i2 - dx);
        }
        else
        {
            d1 = map.invTransform(i1 - dy);
            d2 = map.invTransform(i2 - dy);
        }

        plot->setAxisScale(axis, d1, d2);
    }

    plot->setAutoReplot(doReplot);

    if ( doReplot )
        plot->replot();
}

