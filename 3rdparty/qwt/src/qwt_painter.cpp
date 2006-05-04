/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

// vim: expandtab

#include <qwindowdefs.h>
#include <qrect.h>
#include <qpainter.h>
#include <qpalette.h>
#include <qpaintdevice.h>
#include <qpaintdevicemetrics.h>
#include <qpixmap.h>
#include <qsimplerichtext.h>

#include "qwt_painter.h"
#include "qwt_rect.h"
#include "qwt_math.h"

#if defined(Q_WS_X11)
bool QwtPainter::d_deviceClipping = TRUE;
#else
bool QwtPainter::d_deviceClipping = FALSE;
#endif

QwtMetricsMap QwtPainter::d_metricsMap;

#if defined(QWT_BROKEN_RASTEROP_FONT)
#if QT_VERSION < 300
// QwtPainter::textFontSubstitutionMode does not exist for Qt-2.3.

// Calling qt_use_xft() from Qt-2.3 internals gives a segmentation error,
// because qt_use_xft() requires presumably an open X display.
// Steal qt_use_xft() but check only the environment variable QT_XFT.
#include <stdlib.h>
static int qwt_use_xft (void)
{
    static int  checked_env=0;
    static int  use_xft=0;

    if (!checked_env) {
        char *e = getenv ("QT_XFT");
        if ( e && (*e == '1' ||
                   *e == 'y' || *e == 'Y' ||
                   *e == 't' || *e == 'T' ))
            use_xft = 1;
        else
            use_xft = 0;
    }
    checked_env = 1;

    return use_xft;
}
int QwtPainter::d_textXorRopMode = qwt_use_xft() ? 
    QwtPainter::XorRopTextKeepFont : QwtPainter::XorRopTextNormal;
#else // QT_VERSION >= 300
#if 1
int QwtPainter::d_textXorRopMode = QwtPainter::XorRopTextKeepFont;
#else
int QwtPainter::d_textXorRopMode = QwtPainter::XorRopTextKeepColor;
#endif
#endif // QT_VERSION
#else
int QwtPainter::d_textXorRopMode = QwtPainter::XorRopTextNormal;
#endif

/*!
  En/Disable device clipping. On X11 the default
  for device clipping is enabled, otherwise it is disabled.
  \sa QwtPainter::deviceClipping()
*/
void QwtPainter::setDeviceClipping(bool enable)
{
    d_deviceClipping = enable;
}

/*!
  Returns whether device clipping is enabled. On X11 the default
  is enabled, otherwise it is disabled.
  \sa QwtPainter::setDeviceClipping()
*/

bool QwtPainter::deviceClipping()
{
    return d_deviceClipping;
}

/*!
  Returns rect for device clipping
  \sa QwtPainter::setDeviceClipping()
*/
const QRect &QwtPainter::deviceClipRect()
{
    static QRect clip;

    if ( !clip.isValid() )
    {
        clip.setCoords(QWT_COORD_MIN, QWT_COORD_MIN,
            QWT_COORD_MAX, QWT_COORD_MAX);
    }
    return clip;
}

/*!
  Scale all QwtPainter drawing operations using the ratio
  QwtPaintMetrics(from).logicalDpiX() / QwtPaintMetrics(to).logicalDpiX()
  and QwtPaintMetrics(from).logicalDpiY() / QwtPaintMetrics(to).logicalDpiY()

  \sa QwtPainter::resetScaleMetrics(), QwtPainter::scaleMetricsX,
        QwtPainter::scaleMetricsY()
*/
void QwtPainter::setMetricsMap(const QPaintDevice *layout,
    const QPaintDevice *device)
{
    d_metricsMap.setMetrics(layout, device);
}

/*! 
  Change the metrics map 
  \sa QwtPainter::resetMetricsMap, QwtPainter::metricsMap
*/
void QwtPainter::setMetricsMap(const QwtMetricsMap &map)
{
    d_metricsMap = map;
}

/*! 
   Reset the metrics map to the ratio 1:1
   \sa QwtPainter::setMetricsMap, QwtPainter::resetMetricsMap
*/
void QwtPainter::resetMetricsMap()
{
    d_metricsMap = QwtMetricsMap();
}

/*!
  \return Metrics map
*/
const QwtMetricsMap &QwtPainter::metricsMap()
{
    return d_metricsMap;
}

/*!
    Wrapper for QPainter::setClipRect()
*/
void QwtPainter::setClipRect(QPainter *painter, const QRect &rect)
{
    painter->setClipRect(d_metricsMap.layoutToDevice(rect, painter));
}

/*!
    Wrapper for QPainter::drawRect()
*/
void QwtPainter::drawRect(QPainter *painter, int x, int y, int w, int h) 
{
    drawRect(painter, QRect(x, y, w, h));
}

/*!
    Wrapper for QPainter::drawRect()
*/
void QwtPainter::drawRect(QPainter *painter, const QRect &rect) 
{
    const QRect r = d_metricsMap.layoutToDevice(rect, painter);

    if ( d_deviceClipping && !deviceClipRect().contains(r) )
        return;

    painter->drawRect(r);
}

/*!
    Wrapper for QPainter::fillRect()
*/
void QwtPainter::fillRect(QPainter *painter, 
    const QRect &rect, const QBrush &brush)
{
    const QRect r = d_metricsMap.layoutToDevice(rect, painter);

    if ( d_deviceClipping && !deviceClipRect().contains(r) )
        return;

    painter->fillRect(r, brush);
}

/*!
    Wrapper for QPainter::drawEllipse()
*/
void QwtPainter::drawEllipse(QPainter *painter, const QRect &rect)
{
    const QRect r = d_metricsMap.layoutToDevice(rect, painter);

    if ( d_deviceClipping && !deviceClipRect().contains(rect) )
        return;

    painter->drawEllipse(r);
}

/*!
    Wrapper for QPainter::drawText()
*/
void QwtPainter::drawText(QPainter *painter, int x, int y, 
        const QString &text, int len)
{
    drawText(painter, QPoint(x, y), text, len);
}

/*!
    Wrapper for QPainter::drawText()
*/
void QwtPainter::drawText(QPainter *painter, const QPoint &pos, 
        const QString &text, int len)
{
    const QPoint p = d_metricsMap.layoutToDevice(pos, painter);

    if ( d_deviceClipping && !deviceClipRect().contains(p) )
        return;

#if defined(QWT_BROKEN_RASTEROP_FONT)
    // XorROP text drawing does not work with xft
#if QT_VERSION >= 300
    if (Qt::XorROP == painter->rasterOp()
        && d_textXorRopMode == XorRopTextKeepColor)
    {
        // step 1: disable xft
        extern bool qt_has_xft;
        const bool XftEnabled = qt_has_xft;
        qt_has_xft = FALSE;

        // step 2: substitute the font by a bitmap font
        painter->save();
        QFont font = painter->font();
        font.setStyleStrategy(QFont::PreferBitmap);
        painter->setFont(font);

        painter->drawText(p, text, len);

        // restore state
        qt_has_xft = XftEnabled;
        painter->restore();

    }
    else
#endif
    if (Qt::XorROP == painter->rasterOp()
        && d_textXorRopMode == XorRopTextKeepFont)
    {
        // step 1: create a buffer pixmap, but we have to guess its size.
#if QT_VERSION < 300
        int flags = Qt::AlignLeft;
#else
        int flags = Qt::AlignAuto; // OK for QwtScaleDraw, but ???
#endif 
        QFontMetrics fm = painter->fontMetrics();
        QPixmap pixmap(fm.boundingRect(
            0, 0, QCOORD_MAX, QCOORD_MAX, flags, text, len).size());
        pixmap.fill(QColor(0, 0, 0));

        // step 2: draw the text on the pixmap
        QPainter pmPainter(&pixmap);
        pmPainter.setPen(painter->pen());
        pmPainter.setFont(painter->font());
        pmPainter.drawText(pixmap.rect(), flags, text, len);

        // step 3: draw the pixmap
        painter->drawPixmap(p.x(), p.y() - fm.ascent(), pixmap);
    }
    else
#endif
    {
        painter->drawText(p, text, len);
    }
}

/*!
    Wrapper for QPainter::drawText()
*/
void QwtPainter::drawText(QPainter *painter, int x, int y, int w, int h, 
        int flags, const QString &text, int len)
{
    drawText(painter, QRect(x, y, w, h), flags, text, len);
}

/*!
    Wrapper for QPainter::drawText()
*/
void QwtPainter::drawText(QPainter *painter, const QRect &rect, 
        int flags, const QString &text, int len)
{
#if defined(QWT_BROKEN_RASTEROP_FONT)
    // XorROP text drawing does not work with xft
#if QT_VERSION >= 300
    if (Qt::XorROP == painter->rasterOp()
        && d_textXorRopMode == XorRopTextKeepColor)
    {
        // step 1: disable xft
        extern bool qt_has_xft;
        const bool XftEnabled = qt_has_xft;
        qt_has_xft = FALSE;

        // step 2: substitute the font by a bitmap font
        painter->save();
        QFont font = painter->font();
        font.setStyleStrategy(QFont::PreferBitmap);
        painter->setFont(font);

        painter->drawText(
            d_metricsMap.layoutToDevice(rect, painter), flags, text, len);

        // restore state
        painter->restore();
        qt_has_xft = XftEnabled;

    }
    else
#endif
    if (Qt::XorROP == painter->rasterOp()
        && d_textXorRopMode == XorRopTextKeepFont)
    {
        // step 1: create a buffer pixmap
        QRect target = d_metricsMap.layoutToDevice(rect, painter);
        QPixmap pixmap(target.size());
        pixmap.fill(QColor(0, 0, 0));

        // step 2: draw the text on the pixmap
        QPainter pmPainter(&pixmap);
        pmPainter.setPen(painter->pen());
        pmPainter.setFont(painter->font());
        pmPainter.drawText(pixmap.rect(), flags, text, len);

        // step 3: draw the pixmap
#if QT_VERSION < 300
        painter->drawPixmap(target.x(), target.y(), pixmap);
#else
        painter->drawPixmap(target, pixmap);
#endif
    }
    else
#endif
    {
        painter->drawText(
            d_metricsMap.layoutToDevice(rect, painter), flags, text, len);
    }
}

#ifndef QT_NO_RICHTEXT

/*!
  Wrapper for QSimpleRichText::draw()
*/
void QwtPainter::drawSimpleRichText(QPainter *painter, const QRect &rect,
    int flags, QSimpleRichText &text)
{
    QColorGroup cg;
    cg.setColor(QColorGroup::Text, painter->pen().color());

#if QT_VERSION < 300
    const QFont defaultFont = QFont::defaultFont();
    QFont::setDefaultFont(painter->font());
#endif

    const QRect scaledRect = d_metricsMap.layoutToDevice(rect, painter);

    text.setWidth(painter, scaledRect.width());

    // QSimpleRichText is Qt::AlignTop by default

    int y = scaledRect.y();
    if (flags & Qt::AlignBottom)
        y += (scaledRect.height() - text.height());
    else if (flags & Qt::AlignVCenter)
        y += (scaledRect.height() - text.height())/2;

#if defined(QWT_BROKEN_RASTEROP_FONT)
    // XorROP text drawing does not work with xft
#if QT_VERSION >= 300
    if (Qt::XorROP == painter->rasterOp()
        && d_textXorRopMode == XorRopTextKeepColor)
    {
        // step 1: disable xft
        extern bool qt_has_xft;
        const bool XftEnabled = qt_has_xft;
        qt_has_xft = FALSE;

        // step 2: substitute the font by a bitmap font
        painter->save();
        QFont font = painter->font();
        font.setStyleStrategy(QFont::PreferBitmap);
        painter->setFont(font);

        text.draw(painter, scaledRect.x(), y, scaledRect, cg);

        // restore state
        painter->restore();
        qt_has_xft = XftEnabled;

    }
    else
#endif
    if (Qt::XorROP == painter->rasterOp()
        && d_textXorRopMode == XorRopTextKeepFont)
    {
        // step 1: create a buffer pixmap
        QPixmap pixmap(scaledRect.size());
        pixmap.fill(QColor(0, 0, 0));

        // step 2: draw the text on the pixmap
        QPainter pmPainter(&pixmap);
        pmPainter.setPen(painter->pen());
        pmPainter.setFont(painter->font());
        text.draw(&pmPainter, scaledRect.x(), y, scaledRect, cg);

        // step 3: draw the pixmap
#if QT_VERSION < 300
        painter->drawPixmap(scaledRect.x(), scaledRect.y(), pixmap);
#else
        painter->drawPixmap(scaledRect, pixmap);
#endif
    }
    else
#endif
    {
        text.draw(painter, scaledRect.x(), y, scaledRect, cg);
    }

#if QT_VERSION < 300
    QFont::setDefaultFont(defaultFont);
#endif
}

#endif // !QT_NO_RICHTEXT


/*!
  Wrapper for QPainter::drawLine()
*/
void QwtPainter::drawLine(QPainter *painter, 
    const QPoint &p1, const QPoint &p2) 
{
    QPointArray pa(2);

    if ( d_deviceClipping && 
        !(deviceClipRect().contains(p1) && deviceClipRect().contains(p2)) )
    {
        pa.setPoint(0, p1);
        pa.setPoint(1, p2);
        drawPolyline(painter, pa);
    }
    else
    {
        pa.setPoint(0, d_metricsMap.layoutToDevice(p1));
        pa.setPoint(1, d_metricsMap.layoutToDevice(p2));

#if QT_VERSION >= 0x030200
        if ( painter->device()->isExtDev() )
        {
            // Strange: the postscript driver of QPrinter adds an offset 
            // of 0.5 to the start/endpoint when using drawLine, but not
            // for lines painted with drawLineSegments.

            painter->drawLineSegments(pa);
        }
        else
            painter->drawLine(pa[0], pa[1]);
#else
        painter->drawLine(pa[0], pa[1]);
#endif
    }
}

/*!
  Wrapper for QPainter::drawLine()
*/

void QwtPainter::drawLine(QPainter *painter, int x1, int y1, int x2, int y2)
{
    drawLine(painter, QPoint(x1, y1), QPoint(x2, y2));
}

/*!
  Wrapper for QPainter::drawPolygon()
*/

void QwtPainter::drawPolygon(QPainter *painter, const QPointArray &pa)
{
    QPointArray cpa = d_metricsMap.layoutToDevice(pa);
    if ( d_deviceClipping )
        cpa = clip(cpa);
    painter->drawPolygon(cpa);
}

/*!
    Wrapper for QPainter::drawPolyline()
*/
void QwtPainter::drawPolyline(QPainter *painter, const QPointArray &pa)
{
    QPointArray cpa = d_metricsMap.layoutToDevice(pa);
    if ( d_deviceClipping )
        cpa = clip(cpa);
    painter->drawPolyline(cpa);
}

/*!
    Wrapper for QPainter::drawPoint()
*/

void QwtPainter::drawPoint(QPainter *painter, int x, int y)
{
    const QPoint pos = d_metricsMap.layoutToDevice(QPoint(x, y));

    if ( d_deviceClipping && !deviceClipRect().contains(pos) )
        return;

    painter->drawPoint(pos);
}

//! Clip a point array
QPointArray QwtPainter::clip(const QPointArray &pa)
{
    const QwtRect rect(deviceClipRect());
    return rect.clip(pa);
}

void QwtPainter::drawColoredArc(QPainter *painter, const QRect &rect, 
    int peak, int arc, int intervall, const QColor &c1, const QColor &c2)
{
    int h1, s1, v1;
    int h2, s2, v2;

    c1.hsv(&h1, &s1, &v1);
    c2.hsv(&h2, &s2, &v2);
    
    arc /= 2;
    for ( int angle = -arc; angle < arc; angle += intervall)
    {
        double ratio;
        if ( angle >= 0 )
            ratio = 1.0 - angle / double(arc);
        else
            ratio = 1.0 + angle / double(arc);
            

        const QColor c(h1 + qRound(ratio * (h2 - h1)),
            s1 + qRound(ratio * (s2 - s1)),
            v1 + qRound(ratio * (v2 - v1)),
            QColor::Hsv);

        painter->setPen(QPen(c, painter->pen().width()));
        painter->drawArc(rect, (peak + angle) * 16, intervall * 16);
    }
}

//!  Draw a round frame
void QwtPainter::drawRoundFrame(QPainter *painter, const QRect &rect,
    int width, const QColorGroup &cg, bool sunken)
{
    QColor c0 = cg.mid();
    QColor c1, c2;
    if ( sunken )
    {
        c1 = cg.dark();
        c2 = cg.light();
    }
    else
    {
        c1 = cg.light();
        c2 = cg.dark();
    }

    painter->setPen(QPen(c0, width));
    painter->drawArc(rect, 0, 360 * 16); // full

    const int peak = 150;
    const int intervall = 2;

    if ( c0 != c1 )
        drawColoredArc(painter, rect, peak, 160, intervall, c0, c1);
    if ( c0 != c2 )
        drawColoredArc(painter, rect, peak + 180, 120, intervall, c0, c2);
}

/*!
  Return the method to work around a bug in the drawing of text using the
  XorROP raster operator on the X Window system.

  \sa QwtPainter::textXorRopMode
 */
int QwtPainter::textXorRopMode()
{
    return d_textXorRopMode;
}

/*!
  Selects a method to work around a bug in the drawing of text using the XorROP
  raster operator on the X Window system. Has no effect on other platforms.
 
  \sa QwtPicker

  Possible modes are:
  - XorRopTextNormal; fails on the X Window System with X Free Type enabled.
  - XorRopTextKeepFont; works around the bug while keeping the font, but the
    color of the text is unpredictable.  It is possible to get satisfactory
    results by choosing a suitable combination of font, text color and
    background color.
  - XorRopTextKeepColor; works around the bug while keeping the color, but the
    font of the text is unpredictable (it depends on the version of Qt).
    The font that is used may not at all resemble the requested font.
    This mode does not exist when Qwt has been built for Qt-2.3.

  \param mode Mode to do the XorROP text drawing 
*/
#if defined(QWT_BROKEN_RASTEROP_FONT)

int QwtPainter::setTextXorRopMode(TextXorRopMode mode)
{
    if ((mode == XorRopTextNormal)
        || (mode == XorRopTextKeepFont)
#if QT_VERSION >= 300
        || (mode == XorRopTextKeepColor)
#endif
        )
        d_textXorRopMode = mode;

#if QT_VERSION < 300
    if (!qwt_use_xft())
        d_textXorRopMode = XorRopTextNormal;
#endif

    return d_textXorRopMode;
}

#else

int QwtPainter::setTextXorRopMode(TextXorRopMode)
{
    return d_textXorRopMode;
}

#endif

// Local Variables:
// mode: C++
// c-file-style: "stroustrup"
// indent-tabs-mode: nil
// End:
