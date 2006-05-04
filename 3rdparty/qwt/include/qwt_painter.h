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
#include <qpointarray.h>
#include <qrect.h>
#include "qwt_global.h"
#include "qwt_layout_metrics.h"

class QPainter;

#if defined(Q_WS_X11)
// Warning: QCOORD_MIN, QCOORD_MAX are wrong on X11.
#define QWT_COORD_MAX 16384
#define QWT_COORD_MIN (-QWT_COORD_MAX - 1)
#else
#define QWT_COORD_MAX QCOORD_MAX
#define QWT_COORD_MIN QCOORD_MIN
#endif

/*!
  \brief A collection of QPainter workarounds

  1) Clipping to coordinate system limits

  On X11 pixel coordinates are stored in shorts. Qt 
  produces overruns when mapping QCOORDS to shorts. 

  2) Scaling to device metrics

  QPainter scales fonts, line and fill patterns to the metrics
  of the paint device. Other values like the geometries of rects, points
  remain device independend. To enable a device independent widget 
  implementation, QwtPainter adds scaling of these geometries.
  (Unfortunately QPainter::scale scales both types of paintings,
   so the objects of the first type would be scaled twice).

  3) Hide some Qt2/3 incompatibilities
*/

class QWT_EXPORT QwtPainter
{
public:
    /*!
      Indicates a method to work around a bug in the drawing of text using the
      XorROP raster operator on the X Window system. Has no effect on other
      platforms.

      \sa  QwtPainter::setTextXorRopMode
     */
    enum TextXorRopMode {
        XorRopTextNormal = 0,
        XorRopTextKeepFont = 1,
        XorRopTextKeepColor = 2
    };

    static void setMetricsMap(const QPaintDevice *layout,
        const QPaintDevice *device);
    static void setMetricsMap(const QwtMetricsMap &);
    static void resetMetricsMap();
    static const QwtMetricsMap &metricsMap();

    static void setDeviceClipping(bool);
    static bool deviceClipping();

    static void setClipRect(QPainter *, const QRect &);

    static void drawText(QPainter *, int x, int y, 
        const QString &, int len = -1);
    static void drawText(QPainter *, const QPoint &, 
        const QString &, int len = -1);
    static void drawText(QPainter *, int x, int y, int w, int h, 
        int flags, const QString &, int len = -1);
    static void drawText(QPainter *, const QRect &, 
        int flags, const QString &, int len = -1);

#ifndef QT_NO_RICHTEXT
    static void drawSimpleRichText(QPainter *, const QRect &,
        int flags, QSimpleRichText &);
#endif

    static void drawRect(QPainter *, int x, int y, int w, int h);
    static void drawRect(QPainter *, const QRect &rect);
    static void fillRect(QPainter *, const QRect &, const QBrush &); 

    static void drawEllipse(QPainter *, const QRect &);

    static void drawLine(QPainter *, int x1, int y1, int x2, int y2);
    static void drawLine(QPainter *, const QPoint &p1, const QPoint &p2);
    static void drawPolygon(QPainter *, const QPointArray &pa);
    static void drawPolyline(QPainter *, const QPointArray &pa);
    static void drawPoint(QPainter *, int x, int y);

    static void drawRoundFrame(QPainter *, const QRect &,
        int width, const QColorGroup &cg, bool sunken);

    static QPointArray clip(const QPointArray &);

    static int textXorRopMode();
    static int setTextXorRopMode(TextXorRopMode mode);

private:
    static const QRect &deviceClipRect();
    static void drawColoredArc(QPainter *, const QRect &,
        int peak, int arc, int intervall, const QColor &c1, const QColor &c2);

    static bool d_deviceClipping;
    static QwtMetricsMap d_metricsMap;
    static int d_textXorRopMode;
};

#endif

// Local Variables:
// mode: C++
// c-file-style: "stroustrup"
// indent-tabs-mode: nil
// End:
