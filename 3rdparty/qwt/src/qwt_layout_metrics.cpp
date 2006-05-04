/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#include <qapplication.h>
#include <qpainter.h>
#include <qpaintdevicemetrics.h> 
#include <qsimplerichtext.h> 
#if QT_VERSION >= 300
#include <qdesktopwidget.h> 
#endif
#include "qwt_layout_metrics.h"

QwtMetricsMap::QwtMetricsMap()
{
    d_screenToLayoutX = d_screenToLayoutY = 
        d_deviceToLayoutX = d_deviceToLayoutY = 1.0;
}

void QwtMetricsMap::setMetrics(const QPaintDeviceMetrics &layoutMetrics, 
    const QPaintDeviceMetrics &deviceMetrics)
{
    const QPaintDeviceMetrics screenMetrics(QApplication::desktop());

    d_screenToLayoutX = double(layoutMetrics.logicalDpiX()) / 
        double(screenMetrics.logicalDpiX());
    d_screenToLayoutY = double(layoutMetrics.logicalDpiY()) / 
        double(screenMetrics.logicalDpiY());

    d_deviceToLayoutX = double(layoutMetrics.logicalDpiX()) / 
        double(deviceMetrics.logicalDpiX());
    d_deviceToLayoutY = double(layoutMetrics.logicalDpiY()) / 
        double(deviceMetrics.logicalDpiY());
}

#ifndef QT_NO_TRANSFORMATIONS
QPoint QwtMetricsMap::layoutToDevice(const QPoint &point, 
    const QPainter *painter) const
#else
QPoint QwtMetricsMap::layoutToDevice(const QPoint &point, 
    const QPainter *) const
#endif
{
    if ( isIdentity() )
        return point;

    QPoint mappedPoint(point);

#ifndef QT_NO_TRANSFORMATIONS
    if ( painter )
        mappedPoint = painter->worldMatrix().map(mappedPoint);
#endif

    mappedPoint.setX(layoutToDeviceX(mappedPoint.x()));
    mappedPoint.setY(layoutToDeviceY(mappedPoint.y()));

#ifndef QT_NO_TRANSFORMATIONS
    if ( painter )
        mappedPoint = painter->worldMatrix().invert().map(mappedPoint);
#endif

    return mappedPoint;
}

#ifndef QT_NO_TRANSFORMATIONS
QPoint QwtMetricsMap::deviceToLayout(const QPoint &point, 
    const QPainter *painter) const
#else
QPoint QwtMetricsMap::deviceToLayout(const QPoint &point, 
    const QPainter *) const
#endif
{
    if ( isIdentity() )
        return point;

    QPoint mappedPoint(point);

#ifndef QT_NO_TRANSFORMATIONS
    if ( painter )
        mappedPoint = painter->worldMatrix().map(mappedPoint);
#endif

    mappedPoint.setX(deviceToLayoutX(mappedPoint.x()));
    mappedPoint.setY(deviceToLayoutY(mappedPoint.y()));

#ifndef QT_NO_TRANSFORMATIONS
    if ( painter )
        mappedPoint = painter->worldMatrix().invert().map(mappedPoint);
#endif

    return mappedPoint;
}

QPoint QwtMetricsMap::screenToLayout(const QPoint &point) const
{
    if ( d_screenToLayoutX == 1.0 && d_screenToLayoutY == 1.0 )
        return point;

    return QPoint(screenToLayoutX(point.x()), screenToLayoutY(point.y()));
}

#ifndef QT_NO_TRANSFORMATIONS
QRect QwtMetricsMap::layoutToDevice(const QRect &rect, 
    const QPainter *painter) const
#else
QRect QwtMetricsMap::layoutToDevice(const QRect &rect, 
    const QPainter *) const
#endif
{
    if ( isIdentity() )
        return rect;

    QRect mappedRect(rect);
#ifndef QT_NO_TRANSFORMATIONS
    if ( painter )
        mappedRect = translate(painter->worldMatrix(), mappedRect);
#endif

    mappedRect = QRect(
        layoutToDeviceX(mappedRect.x()), 
        layoutToDeviceY(mappedRect.y()),
        layoutToDeviceX(mappedRect.width()), 
        layoutToDeviceY(mappedRect.height())
    );

#ifndef QT_NO_TRANSFORMATIONS
    if ( painter )
        mappedRect = translate(painter->worldMatrix().invert(), mappedRect);
#endif

    return mappedRect;
}

#ifndef QT_NO_TRANSFORMATIONS
QRect QwtMetricsMap::deviceToLayout(const QRect &rect,
    const QPainter *painter) const
#else
QRect QwtMetricsMap::deviceToLayout(const QRect &rect,
    const QPainter *) const
#endif
{
    if ( isIdentity() )
        return rect;

    QRect mappedRect(rect);
#ifndef QT_NO_TRANSFORMATIONS
    if ( painter )
        mappedRect = translate(painter->worldMatrix(), mappedRect);
#endif

    mappedRect = QRect(
        deviceToLayoutX(mappedRect.x()), 
        deviceToLayoutY(mappedRect.y()),
        deviceToLayoutX(mappedRect.width()), 
        deviceToLayoutY(mappedRect.height())
    );

#ifndef QT_NO_TRANSFORMATIONS
    if ( painter )
        mappedRect = translate(painter->worldMatrix().invert(), mappedRect);
#endif

    return mappedRect;
}

QRect QwtMetricsMap::screenToLayout(const QRect &rect) const
{
    if ( d_deviceToLayoutX == 1.0 && d_deviceToLayoutY == 1.0 )
        return rect;

    return QRect(screenToLayoutX(rect.x()), screenToLayoutY(rect.y()),
        screenToLayoutX(rect.width()), screenToLayoutY(rect.height()));
}

#ifndef QT_NO_TRANSFORMATIONS
QPointArray QwtMetricsMap::layoutToDevice(const QPointArray &pa, 
    const QPainter *painter) const
#else
QPointArray QwtMetricsMap::layoutToDevice(const QPointArray &pa, 
    const QPainter *) const
#endif
{
    if ( isIdentity() )
        return pa;
    
    QPointArray mappedPa(pa);

#ifndef QT_NO_TRANSFORMATIONS
    if ( painter )
        mappedPa = translate(painter->worldMatrix(), mappedPa);
#endif

    QWMatrix m;
    m.scale(1.0 / d_deviceToLayoutX, 1.0 / d_deviceToLayoutY);
    mappedPa = translate(m, mappedPa);

#ifndef QT_NO_TRANSFORMATIONS
    if ( painter )
        mappedPa = translate(painter->worldMatrix().invert(), mappedPa);
#endif

    return mappedPa;

}

#ifndef QT_NO_TRANSFORMATIONS
QPointArray QwtMetricsMap::deviceToLayout(const QPointArray &pa, 
    const QPainter *painter) const
#else
QPointArray QwtMetricsMap::deviceToLayout(const QPointArray &pa, 
    const QPainter *) const
#endif
{
    if ( isIdentity() )
        return pa;
    
    QPointArray mappedPa(pa);

#ifndef QT_NO_TRANSFORMATIONS
    if ( painter )
        mappedPa = translate(painter->worldMatrix(), mappedPa);
#endif

    QWMatrix m;
    m.scale(d_deviceToLayoutX, d_deviceToLayoutY);
    mappedPa = translate(m, mappedPa);

#ifndef QT_NO_TRANSFORMATIONS
    if ( painter )
        mappedPa = translate(painter->worldMatrix().invert(), mappedPa);
#endif

    return mappedPa;
}

/*!
  Wrapper for QWMatrix::mapRect. Hides Qt2/3 incompatibilities.
*/

QRect QwtMetricsMap::translate(
    const QWMatrix &m, const QRect &rect) 
{
#if QT_VERSION < 300
    return m.map(rect.normalize());
#else
    return m.mapRect(rect);
#endif
}

/*!
  QPointArray QWMatrix::operator*(const QPointArray &) const.
  Hides Qt2/3 incompatibilities.
*/

QPointArray QwtMetricsMap::translate(
    const QWMatrix &m, const QPointArray &pa) 
{
#if QT_VERSION < 300
    return m.map(pa);
#elif QT_VERSION < 400
    return m * pa;
#else
    return m.map(pa);
#endif
}

QwtLayoutMetrics::QwtLayoutMetrics()
{
}

QwtLayoutMetrics::QwtLayoutMetrics(const QwtMetricsMap &map):
    d_map(map)
{
}

void QwtLayoutMetrics::setMap(const QwtMetricsMap &map)
{
    d_map = map;
}

int QwtLayoutMetrics::heightForWidth(const QString &text,
    int width, int flags, const QFontMetrics &fm) const
{
    const QRect rect = fm.boundingRect(
        0, 0, d_map.layoutToScreenX(width), QCOORD_MAX, flags, text);
    return d_map.screenToLayoutY(rect.height());
}

int QwtLayoutMetrics::heightForWidth(const QString &text,
    int width, int flags, QPainter *painter) const
{
    const QRect rect = painter->boundingRect(
        0, 0, d_map.layoutToDeviceX(width), QCOORD_MAX, flags, text);

    return d_map.deviceToLayoutY(rect.height());
}

QRect QwtLayoutMetrics::boundingRect(const QString &text, 
    int flags, QPainter *painter) const
{
    const QRect rect = painter->boundingRect(
        0, 0, 0, 0, flags, text);

    return d_map.deviceToLayout(rect);
}

QRect QwtLayoutMetrics::boundingRect(const QString &text, 
    int flags, const QFontMetrics &fm) const
{
    QRect rect = fm.boundingRect(
        0, 0, QCOORD_MAX, QCOORD_MAX, flags, text);

    return d_map.screenToLayout(rect);
}

#ifndef QT_NO_RICHTEXT

int QwtLayoutMetrics::heightForWidth(QSimpleRichText &text, int width) const
{
    text.setWidth(d_map.layoutToScreenX(width));
    return d_map.screenToLayoutY(text.height());
}

QRect QwtLayoutMetrics::boundingRect(
    const QSimpleRichText &text, int flags, QPainter *painter) const
{
    const int tw = text.width();

    int w, h;
    if ( painter )
    {
        ((QSimpleRichText &)text).setWidth(painter, QCOORD_MAX);
        w = d_map.deviceToLayoutX(text.widthUsed());
        h = d_map.deviceToLayoutY(text.height());
    }
    else
    {
        ((QSimpleRichText &)text).setWidth(QCOORD_MAX);
        w = d_map.screenToLayoutX(text.widthUsed());
        h = d_map.screenToLayoutY(text.height());
    }

    ((QSimpleRichText &)text).setWidth(tw); // reset width

    int x = 0; 
    int y = 0;
    if (flags & Qt::AlignHCenter)
        x -= w/2;
    else if (flags & Qt::AlignRight)
        x -= w;

    if (flags & Qt::AlignVCenter)
        y -= h/2;
    else if (flags & Qt::AlignBottom)
        y -= h;

    return QRect(x, y, w, h);
}

#endif // !QT_NO_RICHTEXT
