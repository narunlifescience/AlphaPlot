/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#ifndef QWT_PAINTER_H
#define QWT_PAINTER_H

#include <qpoint.h>
#include <qrect.h>
#include "qwt_global.h"
#include "qwt_layout_metrics.h"

class QPainter;
class QBrush;
class QColor;
class QWidget;

#if QT_VERSION < 0x040000
class QColorGroup;
class QSimpleRichText;
#else
class QPalette;
class QTextDocument;
#endif

#if defined(Q_WS_X11)
// Warning: QCOORD_MIN, QCOORD_MAX are wrong on X11.
#define QWT_COORD_MAX 16384
#define QWT_COORD_MIN (-QWT_COORD_MAX - 1)
#else
#define QWT_COORD_MAX 2147483647
#define QWT_COORD_MIN -QWT_COORD_MAX - 1
#endif

/*!
  \brief A collection of QPainter workarounds

  1) Clipping to coordinate system limits (Qt3 only)

  On X11 pixel coordinates are stored in shorts. Qt 
  produces overruns when mapping QCOORDS to shorts. 

  2) Scaling to device metrics

  QPainter scales fonts, line and fill patterns to the metrics
  of the paint device. Other values like the geometries of rects, points
  remain device independend. To enable a device independent widget 
  implementation, QwtPainter adds scaling of these geometries.
  (Unfortunately QPainter::scale scales both types of paintings,
   so the objects of the first type would be scaled twice).
*/

class QWT_EXPORT QwtPainter
{
public:
    static void setMetricsMap(const QPaintDevice *layout,
        const QPaintDevice *device);
    static void setMetricsMap(const QwtMetricsMap &);
    static void resetMetricsMap();
    static const QwtMetricsMap &metricsMap();

    static void setDeviceClipping(bool);
    static bool deviceClipping();

    static void setClipRect(QPainter *, const QRect &);

    static void drawText(QPainter *, int x, int y, 
        const QString &);
    static void drawText(QPainter *, const QPoint &, 
        const QString &);
    static void drawText(QPainter *, int x, int y, int w, int h, 
        int flags, const QString &);
    static void drawText(QPainter *, const QRect &, 
        int flags, const QString &);

#ifndef QT_NO_RICHTEXT
#if QT_VERSION < 0x040000
    static void drawSimpleRichText(QPainter *, const QRect &,
        int flags, QSimpleRichText &);
#else
    static void drawSimpleRichText(QPainter *, const QRect &,
        int flags, QTextDocument &);
#endif
#endif

    static void drawRect(QPainter *, int x, int y, int w, int h);
    static void drawRect(QPainter *, const QRect &rect);
    static void fillRect(QPainter *, const QRect &, const QBrush &); 

    static void drawEllipse(QPainter *, const QRect &);

    static void drawLine(QPainter *, int x1, int y1, int x2, int y2);
    static void drawLine(QPainter *, const QPoint &p1, const QPoint &p2);
#if QT_VERSION < 0x040000
    static void drawPolygon(QPainter *, const QPointArray &pa);
    static void drawPolyline(QPainter *, const QPointArray &pa);
#else
    static void drawPolygon(QPainter *, const QPolygon &pa);
    static void drawPolyline(QPainter *, const QPolygon &pa);
#endif
    static void drawPoint(QPainter *, int x, int y);

#if QT_VERSION < 0x040000
    static void drawRoundFrame(QPainter *, const QRect &,
        int width, const QColorGroup &cg, bool sunken);
#else
    static void drawRoundFrame(QPainter *, const QRect &,
        int width, const QPalette &, bool sunken);
#endif
    static void drawFocusRect(QPainter *, QWidget *);
    static void drawFocusRect(QPainter *, QWidget *, const QRect &);

#if QT_VERSION < 0x040000
    static QPointArray clip(const QPointArray &);
#else
    static QPolygon clip(const QPolygon &);
#endif

private:
    static void drawColoredArc(QPainter *, const QRect &,
        int peak, int arc, int intervall, const QColor &c1, const QColor &c2);

    static const QRect &deviceClipRect();
    static bool d_deviceClipping;
    static QwtMetricsMap d_metricsMap;
};

//!  Wrapper for QPainter::drawLine()
inline void QwtPainter::drawLine(QPainter *painter,
    const QPoint &p1, const QPoint &p2)
{
    drawLine(painter, p1.x(), p1.y(), p2.x(), p2.y());
}

#endif
