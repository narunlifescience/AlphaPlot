/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

// vim: expandtab

#include <stdio.h>
#include <qpainter.h>
#include <qpaintdevice.h>
#include "qwt_math.h"
#include "qwt_painter.h"
#include "qwt_scldraw.h"
#include "qwt_layout_metrics.h"

static const double step_eps = 1.0e-6;

/*!
  \brief Constructor

  The range of the scale is initialized to [0, 100],
  the angle range is set to [-135, 135], the geometry
  is initialized such that the origin is at (0,0), the
  length is 100, and the orientation is QwtScaleDraw::Bottom.
*/
QwtScaleDraw::QwtScaleDraw():
    d_options(Backbone),
    d_hpad(4),
    d_vpad(4),
    d_medLen(6),
    d_majLen(8),
    d_minLen(4),
    d_minAngle(-135 * 16),
    d_maxAngle(135 * 16),
    d_fmt('g'),
    d_prec(4),
    d_fieldwidth(0),
    d_labelAlignment(0),
    d_labelRotation(0.0)
{
    // snprintf is C99 and therefore not portable :-(
    // fieldwidth and precision must be in the range 0, 1, .., 99.
    sprintf(d_formatBuffer, "%%%d.%d%c", d_fieldwidth, d_prec, d_fmt);
    setGeometry(0,0,100,Bottom);
    setScale(0,100,0,0,10);
}

//! Destructor
QwtScaleDraw::~QwtScaleDraw()
{
}

//! Change the scale draw options
void QwtScaleDraw::setOptions(int opt)
{
    d_options = opt;
}

/*!
  \return scale draw options
*/
int QwtScaleDraw::options() const
{
    return d_options;
}

/*!
  \brief Adjust the range of the scale

  If step == 0.0, the step width is calculated automatically
  dependent on the maximal number of scale ticks.
  \param x1 Value at the left/low endpoint of the scale
  \param x2 Value at the right/high endpoint of the scale
  \param maxMajIntv Max. number of major step intervals
  \param maxMinIntv Max. number of minor step intervals
  \param step Step size (default : 0.0)
  \param logscale Logarithmic scale (default : 0)
*/
void QwtScaleDraw::setScale(double x1, double x2, int maxMajIntv,
                            int maxMinIntv, double step, int logscale)
{
    d_scldiv.rebuild( x1, x2, maxMajIntv, maxMinIntv, logscale, step, FALSE );
    setDblRange( d_scldiv.lBound(), d_scldiv.hBound(), d_scldiv.logScale());
}


/*!
  \brief Change the scale division
  \param sd new scale division
*/
void QwtScaleDraw::setScale(const QwtScaleDiv &sd)
{
    d_scldiv = sd;
    setDblRange(d_scldiv.lBound(),d_scldiv.hBound(),d_scldiv.logScale());
}

/*!
  \brief Draw the scale
  \param p the painter
*/
void QwtScaleDraw::draw(QPainter *p) const
{
    uint i;

    for (i=0; i< d_scldiv.majCnt(); i++)
    {
        const double val = d_scldiv.majMark(i);
        drawTick(p, val, d_majLen);
        drawLabel(p, val);
    }

    if (d_scldiv.logScale())
    {
        for (i=0; i< d_scldiv.minCnt(); i++)
            drawTick(p, d_scldiv.minMark(i), d_minLen);
    }
    else
    {
        const int kmax = d_scldiv.majCnt() - 1;
        if (kmax > 0) 
        {
            double majTick = d_scldiv.majMark(0);
            double hval = majTick - 0.5 * d_scldiv.majStep();

            int k = 0;
            for (i=0; i< d_scldiv.minCnt(); i++)
            {
                const double val = d_scldiv.minMark(i);
                if  (val > majTick)
                {
                    if (k < kmax)
                    {
                        k++;
                        majTick = d_scldiv.majMark(k);
                    }
                    else
                    {
                        majTick += d_scldiv.majMark(kmax) + d_scldiv.majStep();
                    }
                    hval = majTick - 0.5 * d_scldiv.majStep();

                }
                if (qwtAbs(val-hval) < step_eps * d_scldiv.majStep())
                    drawTick(p, val, d_medLen);
                else
                    drawTick(p, val, d_minLen);
            }
        }
    }

    if ( options() & Backbone )
        drawBackbone(p);
}


//! Draws a single scale tick
void QwtScaleDraw::drawTick(QPainter *p, double val, int len) const
{
   if ( len <= 0 )
        return;

    const int tval = transform(val);

    switch(d_orient)
    {
        case Left:
            QwtPainter::drawLine(p, d_xorg, tval, d_xorg - len, tval);
            break;

        case Right:
            QwtPainter::drawLine(p, d_xorg, tval, d_xorg + len, tval);
            break;

        case Bottom: 
            QwtPainter::drawLine(p, tval, d_yorg, tval, d_yorg + len);
            break;

        case Top:
            QwtPainter::drawLine(p, tval, d_yorg, tval, d_yorg - len);
            break;

        case Round:
            if ((tval <= d_minAngle + 359 * 16) 
                || (tval >= d_minAngle - 359 * 16))
            {
                const double arc = double(tval) / 16.0 * M_PI / 180.0;
                const int x1 = qwtInt(d_xCenter + sin(arc) * d_radius);
                const int x2 = qwtInt(d_xCenter + sin(arc) 
                    * (d_radius + double(len)));
                const int y1 = qwtInt(d_yCenter - cos(arc) * d_radius);
                const int y2 = qwtInt(d_yCenter - cos(arc) 
                    * (d_radius + double(len)));

                QwtPainter::drawLine(p, x1, y1, x2, y2);
            }
            break;
    }
}

//! Draws the number label for a major scale tick
void QwtScaleDraw::drawLabel(QPainter *p, double val) const
{
    QPoint pos;
    int alignment;
    double rotation;
    labelPlacement(QFontMetrics(p->font()), val, pos, alignment, rotation);

    if ( alignment )
    {
        const QString txt = label(val);
        if ( !txt.isEmpty() )
        {
            QWMatrix m = labelWorldMatrix(QFontMetrics(p->font()),
                pos, alignment, rotation, txt);

            p->save();
#ifndef QT_NO_TRANSFORMATIONS
            p->setWorldMatrix(m, TRUE);
#else
            p->translate(m.dx(), m.dy());
#endif
            QwtPainter::drawText(p, 0, 0, txt);
            p->restore();
        }
    }
}

//! Find position, alignment and rotation of the label
void QwtScaleDraw::labelPlacement( const QFontMetrics &fm, double val, 
    QPoint &pos, int &alignment, double &rotation) const
{
    // correct rounding errors if val = 0
    if ((!d_scldiv.logScale()) 
        && (qwtAbs(val) < qwtAbs(step_eps * d_scldiv.majStep())))
    {
       val = 0.0;
    }
    
    const int tval = transform(val);

    int x = 0;
    int y = 0;
    int align = 0;

    switch(d_orient)
    {
        case Right:
        {
            x = d_xorg + d_majLen + d_hpad + 1;
            y = tval;
            align = d_labelAlignment;
            if ( align == 0 )
                align = Qt::AlignRight | Qt::AlignVCenter;
            break;
        }
        case Left:
        {
            x = d_xorg - d_majLen - d_hpad - 1;
            y = tval;
            align = d_labelAlignment;
            if ( align == 0 )
                align = Qt::AlignLeft | Qt::AlignVCenter;
            break;
        }
        case Bottom:
        {
            x = tval;
            y = d_yorg + d_majLen + d_vpad + 1;
            align = d_labelAlignment;
            if ( align == 0 )
                align = Qt::AlignHCenter | Qt::AlignBottom;
            break;
        }
        case Top:
        {
            x = tval;
            y = d_yorg - d_majLen - d_vpad - 1;
            align = d_labelAlignment;
            if ( align == 0 )
                align = Qt::AlignHCenter | Qt::AlignTop;
            break;
        }
        case Round:
        {
            if ((tval > d_minAngle + 359 * 16) 
                || (tval < d_minAngle - 359 * 16))
            {
               break;
            }
            
            const int fmh = fm.ascent() - 2; 
            const double arc = tval / 16.0 / 360.0 * 2 * M_PI;
            const double radius = d_radius + d_majLen + d_vpad;

            // First we find the point on a circle enlarged
            // by half of the font height.

            double xOffset = ( radius + fmh / 2 ) * sin(arc); 
            double yOffset = ( radius + fmh / 2 ) * cos(arc);

            if ( qwtInt(xOffset) != 0 ) 
            {
                // The centered label might cut the circle 
                // with distance: d_radius + d_majLen + d_vpad
                // We align the label to the circle by moving
                // the x-coordinate, because we have only
                // horizontal labels here.

                const int fmw = fm.width(label(val));

                const double circleX = radius * sin(arc); 
                if ( xOffset < 0 )
                    xOffset = circleX - fmw / 2; // left
                else
                    xOffset = circleX + fmw / 2; // right
            }
            x = qwtInt(d_xCenter + xOffset);
            y = qwtInt(d_yCenter - yOffset);
            align = Qt::AlignHCenter | Qt::AlignVCenter;

            break;
        }
    }

    pos = QPoint(x, y);
    alignment = align;
    rotation = d_labelRotation;
}

//! Return the world matrix for painting the label 

QWMatrix QwtScaleDraw::labelWorldMatrix(const QFontMetrics &fm,
    const QPoint &pos, int alignment, 
#ifdef QT_NO_TRANSFORMATIONS
    double,
#else
    double rotation, 
#endif
    const QString &txt) const
{
    const int w = fm.boundingRect(0, 0, 
        QCOORD_MAX, QCOORD_MAX, 0, txt).width() - 2;
    const int h = fm.ascent() - 2;
    
    int x, y;
    if ( alignment & Qt::AlignLeft )
        x = -w;
    else if ( alignment & Qt::AlignRight )
        x = 0 - w % 2;
    else // Qt::AlignHCenter
        x = -(w / 2);
        
    if ( alignment & Qt::AlignTop )
        y = 0;
    else if ( alignment & Qt::AlignBottom )
        y = h - 1;
    else // Qt::AlignVCenter
        y = h / 2;
    
    QWMatrix m;
    m.translate(pos.x(), pos.y());
#ifndef QT_NO_TRANSFORMATIONS
    m.rotate(rotation);
#endif
    m.translate(x, y);

    return m;
}

//! Draws the baseline of the scale
void QwtScaleDraw::drawBackbone(QPainter *p) const
{   
	const int bw2 = p->pen().width() / 2;
    
    switch(d_orient)
    {
        case Left:
            QwtPainter::drawLine(p, d_xorg - bw2, 
                d_yorg, d_xorg - bw2, d_yorg + d_len - 1);
            break;
        case Right:
            QwtPainter::drawLine(p, d_xorg + bw2, 
                d_yorg, d_xorg + bw2, d_yorg + d_len - 1);
            break;
        case Top:
            QwtPainter::drawLine(p, d_xorg, d_yorg - bw2, 
                d_xorg + d_len - 1, d_yorg - bw2);
            break;
        case Bottom:
            QwtPainter::drawLine(p, d_xorg, d_yorg + bw2, 
                d_xorg + d_len - 1, d_yorg + bw2);
            break;
        case Round:
        {
            const int a1 = qwtMin(i1(), i2()) - 90 * 16; 
            const int a2 = qwtMax(i1(), i2()) - 90 * 16; 
            
            p->drawArc(d_xorg, d_yorg, d_len, d_len,
                -a2, a2 - a1 + 1);           // counterclockwise
            break;
        }
    }
}


/*!
  \brief Specify the geometry of the scale

  The parameters xorigin, yorigin and length have different meanings,
  dependent on the
  orientation:
  <dl>
  <dt>QwtScaleDraw::Left
  <dd>The origin is the topmost point of the
      baseline. The baseline is a vertical line with the
      specified length. Scale marks and labels are drawn
      at the left of the baseline.
  <dt>QwtScaleDraw::Right
  <dd>The origin is the topmost point of the
      baseline. The baseline is a vertical line with the
      specified length. Scale marks and labels are drawn
      at the right of the baseline.
  <dt>QwtScaleDraw::Top
  <dd>The origin is the leftmost point of the
      baseline. The baseline is a horizontal line with the
      specified length. Scale marks and labels are drawn
      above the baseline.
  <dt>QwtScaleDraw::Bottom
  <dd>The origin is the leftmost point of the
      baseline. The baseline is a horizontal line with the
      specified length. Scale marks and labels are drawn
      below the baseline.
  <dt>QwtScaleDraw::Round
  <dd>The origin is the top left corner of the
      bounding rectangle of the baseline circle. The baseline
      is the segment of a circle with a diameter of the specified length.
      Scale marks and labels are drawn outside the baseline
      circle.</dl>

  \param xorigin x coordinate of the origin
  \param yorigin y coordinate of the origin
  \param length length or diameter of the scale, excluding border distance
  \param o The orientation
*/
void QwtScaleDraw::setGeometry(int xorigin, int yorigin, 
    int length, Orientation o)
{
    static int minLen = 10;

    d_xorg = xorigin;
    d_yorg = yorigin;
    d_radius = double(length) * 0.5;
    d_xCenter = double(xorigin) + double(length) * 0.5;
    d_yCenter = double(yorigin) + double(length) * 0.5;
    
    if (length > minLen)
       d_len = length;
    else
       d_len = minLen;
    
    d_orient = o;
    
    switch(d_orient)
    {
        case Left:
        case Right:
            setIntRange(d_yorg + d_len - 1, d_yorg);
            break;
        case Round:
            setIntRange(d_minAngle, d_maxAngle);
            break;
        case Top:
        case Bottom:
            setIntRange(d_xorg, d_xorg + d_len - 1);
            break;
    }
}

/*!
  \param pen pen
  \param fm font metrics
  \return the maximum width of the scale 
*/
int QwtScaleDraw::maxWidth(const QPen &pen, const QFontMetrics &fm) const
{
    int w = 0;

    switch (d_orient)
    {
        case Left:
        case Right:
            w += pen.width() + d_majLen + d_hpad + maxLabelWidth(fm);
            break;
        case Round:
            w += pen.width() + d_majLen + d_vpad + maxLabelWidth(fm);
            break;
        case Top:
        case Bottom:
            w = d_len + maxLabelWidth(fm);
            break;
    }
    return w;
}

/*!
  \param pen pen
  \param fm font metrics
  \return the maximum height of the scale
*/
int QwtScaleDraw::maxHeight(const QPen &pen, const QFontMetrics &fm) const 
{
    int h = 0;
    
    switch (d_orient)
    {
        case Top:
        case Bottom:
            h = pen.width() + d_vpad + d_majLen + maxLabelHeight(fm);
            break;
        case Left:
        case Right:
            h = d_len + maxLabelHeight(fm);
            break;
        case Round:
            h = d_vpad + d_majLen;
            if ( maxLabelHeight(fm) > 0 )
                h += fm.ascent() - 2;
            break;
    }
    
    return h;
}

/*!
  Rotate all labels. 

  \param rotation Angle in degrees. When changing the label rotation,
                  the label alignment might be adjusted too.
  \sa setLabelAlignment(), labelRotation(), labelAlignment().
  \warning Rotation of labels is not implemented for round scales.

*/
void QwtScaleDraw::setLabelRotation(double rotation)
{
    d_labelRotation = rotation;
}

/*!
  \return the label rotation
  \sa setLabelRotation(), labelAlignment()
*/
double QwtScaleDraw::labelRotation() const
{
    return d_labelRotation;
}

/*!
  Labels are aligned to the point ticklength + margin away from the backbone. 
  The alignment is relative to the orientation of the label text.
  In case of an alignment of 0 the label will be aligned 
  depending on the orientation of the scale:

      QwtScaleDraw::Top: Qt::AlignHCenter | Qt::AlignTop\n
      QwtScaleDraw::Bottom: Qt::AlignHCenter | Qt::AlignBottom\n
      QwtScaleDraw::Left: Qt::AlignLeft | Qt::AlignVCenter\n
      QwtScaleDraw::Right: Qt::AlignRight | Qt::AlignVCenter\n

  Changing the alignment is often necessary for rotated labels.

  \param alignment Or'd Qt::AlignmentFlags <see qnamespace.h>
  \warning Alignment of labels is not implemented for round scales.
  \sa QwtScaleDraw::setLabelRotation(), QwtScaleDraw::labelRotation(), 
      QwtScaleDraw::labelAlignment()
*/
  
void QwtScaleDraw::setLabelAlignment(int alignment)
{
    d_labelAlignment = alignment;
}

/*!
  \return the label alignment
  \sa setLabelAlignment(), labelRotation()
*/
int QwtScaleDraw::labelAlignment() const
{
    return d_labelAlignment;
}

/*!
  \brief Adjust the baseline circle segment for round scales.

  The baseline will be drawn from min(angle1,angle2) to max(angle1, angle2).
  The settings have no effect if the scale orientation is not set to
  QwtScaleDraw::Round. The default setting is [ -135, 135 ].
  An angle of 0 degrees corresponds to the 12 o'clock position,
  and positive angles count in a clockwise direction.
  \param angle1
  \param angle2 boundaries of the angle interval in degrees.
  \warning <ul>
  <li>The angle range is limited to [-360, 360] degrees. Angles exceeding
      this range will be clipped.
  <li>For angles more than 359 degrees above or below min(angle1, angle2),
      scale marks will not be drawn.
  <li>If you need a counterclockwise scale, use QwtScaleDiv::setRange
  </ul>
*/
void QwtScaleDraw::setAngleRange(double angle1, double angle2)
{
    angle1 = qwtLim(angle1, -360.0, 360.0);
    angle2 = qwtLim(angle2, -360.0, 360.0);

    int amin = int(floor (qwtMin(angle1, angle2) * 16.0 + 0.5));
    int amax = int(floor (qwtMax(angle1, angle2) * 16.0 + 0.5));
    
    if (amin == amax)
    {
        amin -= 1;
        amax += 1;
    }
    
    d_minAngle = amin;
    d_maxAngle = amax;
    setIntRange(d_minAngle, d_maxAngle);
}

/*!
  \brief Set the number format for the major scale labels

  Format character, precision and fieldwidth have the same meaning as for
  sprintf().
  \param f format character 'e', 'f', 'g' 
  \param prec
    - for 'e', 'f': the number of digits after the radix character (point)
    - for 'g': the maximum number of significant digits
  \param fieldwidth fieldwidth

  \note precision and fieldwidth must be in the range 0, 1, .., 99.  Invalid
  input is discarded.

  \sa labelFormat()
*/
void QwtScaleDraw::setLabelFormat(char f, int prec, int fieldwidth)
{
    switch(f)
    {
        case 'e':
        case 'f':
        case 'g':
            d_fmt = f;
            break;
        default:
            break;
    }
    if ((prec > -1) && (prec < 99))
        d_prec = prec;
    if ((fieldwidth > -1) && (fieldwidth < 99))
        d_fieldwidth = fieldwidth;

    sprintf(d_formatBuffer, "%%%d.%d%c", d_fieldwidth, d_prec, d_fmt);
}

/*!
  \brief Return the number format for the major scale labels

  Format character, precision and fieldwidth have the same meaning as for
  sprintf().
  \param f format character 'e', 'f' or 'g' 
  \param prec
    - for 'e', 'f': the number of digits after the radix character (point)
    - for 'g': the maximum number of significant digits
  \param fieldwidth fieldwidth

  \sa setLabelFormat()
*/
void QwtScaleDraw::labelFormat(char &f, int &prec, int &fieldwidth) const
{
    f = d_fmt;
    prec = d_prec;
    fieldwidth = d_fieldwidth;
}

/*!
    Set the margins of the ticks

    \param hMargin Horizontal margin
    \param vMargin Vertical margin
*/
void QwtScaleDraw::setMargin(uint hMargin, uint vMargin)
{
    d_hpad = hMargin;
    d_vpad = vMargin;
}

/*!
    Set the length of the ticks
*/
void QwtScaleDraw::setTickLength(unsigned int minLen, 
    unsigned int medLen, unsigned int majLen)
{
    const unsigned int maxTickLen = 1000;

    d_minLen = QMIN(minLen, maxTickLen);
    d_medLen = QMIN(medLen, maxTickLen);
    d_majLen = QMIN(majLen, maxTickLen);
}

/*!
    Return the length of the ticks
    \sa QwtScaleDraw::majTickLength()
*/
void QwtScaleDraw::tickLength(unsigned int &minLen,
        unsigned int &medLen, unsigned int &majLen) const
{
    minLen = d_minLen;
    medLen = d_medLen;
    majLen = d_majLen;
}

/*!
    Return the length of the major ticks
    \sa QwtScaleDraw::tickLength()
*/
unsigned int QwtScaleDraw::majTickLength() const
{
    return d_majLen;
}

/*!
  \param fm QFontMetrics
  \return the maximum width of a label
*/
int QwtScaleDraw::maxLabelWidth(const QFontMetrics &fm) const
{
    int maxWidth = 0;

    for (uint i = 0; i < d_scldiv.majCnt(); i++)
    {
        double val = d_scldiv.majMark(i);

        // correct rounding errors if val = 0

        if ((!d_scldiv.logScale()) 
            && (qwtAbs(val) < step_eps * qwtAbs(d_scldiv.majStep())))
        {
            val = 0.0;
        }

        const int w = labelBoundingRect(fm, val).width();
        if ( w > maxWidth )
            maxWidth = w;
    }

    return maxWidth;
}

/*!
  \param fm QFontMetrics
  \return the maximum width of a label
*/
int QwtScaleDraw::maxLabelHeight(const QFontMetrics &fm) const
{
    int maxHeight = 0;

    for (uint i = 0; i < d_scldiv.majCnt(); i++)
    {
        double val = d_scldiv.majMark(i);

        // correct rounding errors if val = 0

        if ((!d_scldiv.logScale()) 
            && (qwtAbs(val) < step_eps * qwtAbs(d_scldiv.majStep())))
        {
            val = 0.0;
        }

        const int h = labelBoundingRect(fm, val).height();
        if ( h > maxHeight )
            maxHeight = h;
    }

    return maxHeight;
}

/*!
  Find the bounding rect for the label. The coordinates of
  the rect are relative to margin + ticklength from the backbone
  in direction of the tick.
*/
QRect QwtScaleDraw::labelBoundingRect(
    const QFontMetrics &fm, double val) const
{
    QString zeroString;
    if ( d_fieldwidth > 0 )
        zeroString.fill('0', d_fieldwidth);

    const QString lbl = label(val);

    const QString &txt = fm.width(zeroString) > fm.width(lbl) 
        ? zeroString : lbl;
    if ( txt.isEmpty() )
        return QRect(0, 0, 0, 0);

    QRect br;

    QPoint pos;
    int alignment;
    double rotation;

    labelPlacement(fm, val, pos, alignment, rotation);
    if ( alignment )
    {
        // Don't use fm.boundingRect(txt), it cuts off pixels.
        const int w = fm.boundingRect(0, 0, 
            QCOORD_MAX, QCOORD_MAX, 0, txt).width();
        const int h = -(fm.ascent() - 2);

        QWMatrix m = labelWorldMatrix(fm, pos, alignment, rotation, txt);
        br = QwtMetricsMap::translate(m, QRect(0, 0, w, h));
        br.moveBy(-pos.x(), -pos.y());
    }

    return br;
}

/*!
  \brief Determine the minimum border distance

  This member function returns the minimum space
  needed to draw the mark labels at the scale's endpoints.

  \param fm QFontMetrics
  \param start start border distance
  \param end end border distance
*/
void QwtScaleDraw::minBorderDist(const QFontMetrics &fm,
    int &start, int &end ) const
{
    start = 0;
    end = 0;

    if ( d_scldiv.majCnt() > 0 )
    {
        const QRect labelRectMin = labelBoundingRect(fm, d_scldiv.majMark(0));
        const QRect labelRectMax = labelBoundingRect(fm, 
            d_scldiv.majMark(d_scldiv.majCnt() - 1));

        switch (d_orient)
        {
            case Left:
            case Right:
                end = -labelRectMin.y();
                start = labelRectMax.height() + labelRectMax.y();
                break;
            case Top:
            case Bottom:
                start = -labelRectMin.x();
                end = labelRectMax.width() + labelRectMax.x();
                break;
            case Round:
                start = labelRectMin.width();
                end = labelRectMax.width();
                break;
        }
    }
}

/*!
  Determine the minimum distance between two labels, that is necessairy
  that the texts don't overlap.

  \param fm QFontMetrics
  \return the maximum width of a label
  \warning Not implemented for round scales
*/

int QwtScaleDraw::minLabelDist(const QFontMetrics &fm) const
{
    if ( d_orient == Round ) // no implementation
        return 0;

    if (0 == d_scldiv.majCnt())
    {
        return 0;
    }

    const bool vertical = (d_orient == Left || d_orient == Right);

    QRect bRect1;
    QRect bRect2 = labelBoundingRect(fm, d_scldiv.majMark(0));
    if ( vertical )
    {
        bRect2.setRect(-bRect2.bottom(), 0, bRect2.height(), bRect2.width());
    }
    int maxDist = 0;

    for (uint i = 1; i < d_scldiv.majCnt(); i++ )
    {
        bRect1 = bRect2;
        bRect2 = labelBoundingRect(fm, d_scldiv.majMark(i));
        if ( vertical )
        {
            bRect2.setRect(-bRect2.bottom(), 0,
                bRect2.height(), bRect2.width());
        }

        int dist = fm.leading(); // space between the labels
        if ( bRect1.right() > 0 )
            dist += bRect1.right();
        if ( bRect2.left() < 0 )
            dist += -bRect2.left();

        if ( dist > maxDist )
            maxDist = dist;
    }

    double angle = d_labelRotation / 180.0 * M_PI;
    if ( vertical )
        angle += M_PI / 2;

    if ( sin(angle) == 0.0 )
        return maxDist;

    const int fmHeight = fm.ascent() - 2; 

    // The distance we need until there is
    // the height of the label font. This height is needed
    // for the neighbour labal.

    int labelDist = (int)(fmHeight / sin(angle) * cos(angle));
    if ( labelDist < 0 )
        labelDist = -labelDist;

    // The cast above floored labelDist. We want to ceil.
    labelDist++; 

    // For text orientations close to the scale orientation 

    if ( labelDist > maxDist )
        labelDist = maxDist;

    // For text orientations close to the opposite of the 
    // scale orientation

    if ( labelDist < fmHeight )
        labelDist = fmHeight;

    return labelDist;
}

/*!
  \param pen pen
  \param fm font metrics
  \return the minimum height required to draw the scale
          including the minimum border distance
*/
int QwtScaleDraw::minHeight( const QPen &pen, const QFontMetrics &fm ) const
{
    const int pw = QMAX( 1, pen.width() );  // penwidth can be zero

    int h = 0;
    switch ( d_orient )
    {
        case Left:
        case Right:
        {
            int bottomDist, topDist;
            minBorderDist(fm, bottomDist, topDist);

            h = bottomDist + topDist;
            if ( d_scldiv.majCnt() >= 2 ) 
                h += minLabelDist(fm) * (d_scldiv.majCnt() - 1);

            int th = 2 * (d_scldiv.majCnt() + d_scldiv.minCnt()) * pw;
            if ( th > h )
                h = th;
            break;
        }
        case Round:
            // compute the radial thickness
            h = pw + d_vpad + d_majLen + maxLabelWidth(fm);
            break;
        case Top:
        case Bottom:
            h = pw + d_vpad + d_majLen + maxLabelHeight(fm);
            break;
    }
    return h;
}

/*!
  \param pen pen
  \param fm font metrics
  \return the minimum width required to draw the scale
          including the minimum border distance
*/
int QwtScaleDraw::minWidth( const QPen &pen, const QFontMetrics &fm ) const
{
    const int pw = QMAX( 1, pen.width() );  // penwidth can be zero

    int w = 0;

    switch(d_orient)
    {
        case Left:
        case Right:
        {
            w = pw + d_hpad + d_majLen + maxLabelWidth(fm);
            break;
        }
        case Round:
        {   
            w = pw + d_vpad + d_majLen + maxLabelWidth(fm);
            break;
        }
        case Top:
        case Bottom:
        {
            int leftDist, rightDist;
            minBorderDist(fm, leftDist, rightDist);

            w = leftDist + rightDist + 
                minLabelDist(fm) * (d_scldiv.majCnt() - 1);

            int tw = 2 * (d_scldiv.majCnt() + d_scldiv.minCnt()) * pw;
            if ( tw > w )
                w = tw;

            break;
        }
    }
    return w;
}

/*!
  \brief Convert a value into its representing label using the labelFormat. 
  \param value Value
  \return Label string.
  \sa QwtScaleDraw::setLabelFormat()
*/

QString QwtScaleDraw::label(double value) const
{
#if 1
    if ( value == -0 )
        value = 0;
#endif

    // snprintf is C99 and therefore not portable :-(
    // sprintf is safe, when buffer is large enough for
    // a minus sign, 99 digits, a radix symbol,
    // an exponent character, a minus or plus sign, 3 digits,
    // and a '\0'.
    char buffer[128];
    sprintf(buffer, d_formatBuffer, value);

    return QString::fromLatin1(buffer);
}

//! Return x origin
int QwtScaleDraw::x() const
{
    return d_xorg;
}

//! Return y origin
int QwtScaleDraw::y() const
{
    return d_yorg;
}

//! Return length
int QwtScaleDraw::length() const
{
    return d_len;
}

//! Return scale orientation 
QwtScaleDraw::Orientation QwtScaleDraw::orientation() const 
{ 
    return d_orient; 
}

// Local Variables:
// mode: C++
// c-file-style: "stroustrup"
// indent-tabs-mode: nil
// End:
