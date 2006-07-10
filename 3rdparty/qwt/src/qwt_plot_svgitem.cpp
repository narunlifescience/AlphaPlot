/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#include <qglobal.h>

#include <qpainter.h>
#if QT_VERSION >= 0x040100
#include <qsvgrenderer.h>
#else
#include <qbuffer.h>
#include <qpicture.h>
#endif
#if QT_VERSION < 0x040000
#include <qpaintdevicemetrics.h>
#endif
#include <qwt_scale_map.h>
#include <qwt_legend.h>
#include <qwt_legend_item.h>
#include "qwt_plot_svgitem.h"

class QwtPlotSvgItem::PrivateData
{
public:
    PrivateData()
    {
    }

    QwtDoubleRect boundingRect;
#if QT_VERSION >= 0x040100
    QSvgRenderer renderer;
#else
    QPicture picture;
#endif
};

QwtPlotSvgItem::QwtPlotSvgItem(const QString& title):
    QwtPlotItem(QwtText(title))
{
    init();
}

QwtPlotSvgItem::QwtPlotSvgItem(const QwtText& title):
    QwtPlotItem(title)
{
    init();
}

QwtPlotSvgItem::~QwtPlotSvgItem()
{
    delete d_data;
}

void QwtPlotSvgItem::init()
{
    d_data = new PrivateData();

    setItemAttribute(QwtPlotItem::AutoScale, true);
    setItemAttribute(QwtPlotItem::Legend, false);

    setZ(8.0);
}

int QwtPlotSvgItem::rtti() const
{
    return QwtPlotItem::Rtti_PlotSVG;
}

bool QwtPlotSvgItem::loadFile(const QwtDoubleRect &rect, 
    const QString &fileName)
{
    d_data->boundingRect = rect;
#if QT_VERSION >= 0x040100
    const bool ok = d_data->renderer.load(fileName);
#else
    const bool ok = d_data->picture.load(fileName, "svg");
#endif
    itemChanged();
    return ok;
}

bool QwtPlotSvgItem::loadData(const QwtDoubleRect &rect, 
    const QByteArray &data)
{
    d_data->boundingRect = rect;
#if QT_VERSION >= 0x040100
    const bool ok = d_data->renderer.load(data);
#else
#if QT_VERSION >= 0x040000
    QBuffer buffer(&(QByteArray&)data);
#else
    QBuffer buffer(data);
#endif
    const bool ok = d_data->picture.load(&buffer, "svg");
#endif
    itemChanged();
    return ok;
}

QwtDoubleRect QwtPlotSvgItem::boundingRect() const
{
    return d_data->boundingRect;
}

#if QT_VERSION >= 0x040100
const QSvgRenderer &QwtPlotSvgItem::renderer() const
{
    return d_data->renderer;
}

QSvgRenderer &QwtPlotSvgItem::renderer()
{
    return d_data->renderer;
}
#endif

void QwtPlotSvgItem::draw(QPainter *painter,
    const QwtScaleMap &xMap, const QwtScaleMap &yMap,
    const QRect &canvasRect) const
{
    const QwtDoubleRect cRect = invTransform(xMap, yMap, canvasRect);
    const QwtDoubleRect bRect = boundingRect();
    if ( bRect.isValid() && cRect.isValid() )
    {
        QwtDoubleRect area = bRect;
        if ( bRect.contains(cRect) )
            area = cRect;

        render(painter, viewBox(area),
            transform(xMap, yMap, area) );
    }
}

void QwtPlotSvgItem::render(QPainter *painter,
        const QRect &viewBox, const QRect &rect) const
{
    if ( !viewBox.isValid() )
        return;

#if QT_VERSION >= 0x040100
    const QSize paintSize(painter->window().width(),
        painter->window().height());
    if ( !paintSize.isValid() )
        return;

    const double xRatio =
        double(paintSize.width()) / viewBox.width();
    const double yRatio =
        double(paintSize.height()) / viewBox.height();

    const double dx = rect.left() / xRatio + 1.0;
    const double dy = rect.top() / yRatio + 1.0;

    const double mx = double(rect.width()) / paintSize.width();
    const double my = double(rect.height()) / paintSize.height();

    painter->save();

    painter->translate(dx, dy);
    painter->scale(mx, my);

    d_data->renderer.setViewBox(viewBox);
    d_data->renderer.render(painter);

    painter->restore();
#else
    const double mx = double(rect.width()) / viewBox.width();
    const double my = double(rect.height()) / viewBox.height();
    const double dx = rect.x() - mx * viewBox.x();
    const double dy = rect.y() - my * viewBox.y();

    painter->save();

    painter->translate(dx, dy);
    painter->scale(mx, my);
    
    d_data->picture.play(painter);

    painter->restore();
#endif
}

QRect QwtPlotSvgItem::viewBox(const QwtDoubleRect &area) const
{
#if QT_VERSION >= 0x040100
    const QSize sz = d_data->renderer.defaultSize();
#else
#if QT_VERSION > 0x040000
    const QSize sz(d_data->picture.width(), 
        d_data->picture.height());
#else
    QPaintDeviceMetrics metrics(&d_data->picture);
    const QSize sz(metrics.width(), metrics.height());
#endif
#endif
    const QwtDoubleRect br = boundingRect();

    if ( !area.isValid() || !br.isValid() || sz.isNull() )
        return QRect();

    QwtScaleMap xMap;
    xMap.setScaleInterval(br.left(), br.right());
    xMap.setPaintInterval(0, sz.width());

    QwtScaleMap yMap;
    yMap.setScaleInterval(br.top(), br.bottom());
    yMap.setPaintInterval(sz.height(), 0);

    return transform(xMap, yMap, area);
}
