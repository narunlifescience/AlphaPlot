/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

// vim: expandtab

#include <math.h>
#include <qpen.h>
#include <qpainter.h>
#include <qfontmetrics.h>
#include "qwt_painter.h"
#include "qwt_scale_div.h"
#include "qwt_scale_map.h"
#include "qwt_round_scale_draw.h"

class QwtRoundScaleDraw::PrivateData
{
public:
    PrivateData():
        center(50, 50),
        radius(50),
        minAngle(-135 * 16),
        maxAngle(135 * 16)
    {
    }

    QPoint center;
    int radius; 

    int minAngle;
    int maxAngle;
};

/*!
  \brief Constructor

  The range of the scale is initialized to [0, 100],
  The center is set to (50, 50) with a radius of 50.
  The angle range is set to [-135, 135].
*/
QwtRoundScaleDraw::QwtRoundScaleDraw()
{
    d_data = new QwtRoundScaleDraw::PrivateData;

    setRadius(50);
    scaleMap().setPaintInterval(d_data->minAngle, d_data->maxAngle);
}

//! Copy constructor
QwtRoundScaleDraw::QwtRoundScaleDraw(const QwtRoundScaleDraw &other):
    QwtAbstractScaleDraw(other)
{
    d_data = new QwtRoundScaleDraw::PrivateData(*other.d_data);
}


//! Destructor
QwtRoundScaleDraw::~QwtRoundScaleDraw()
{
    delete d_data;
}

//! Assignment operator
QwtRoundScaleDraw &QwtRoundScaleDraw::operator=(const QwtRoundScaleDraw &other)
{
    *(QwtAbstractScaleDraw*)this = (const QwtAbstractScaleDraw &)other;
    *d_data = *other.d_data;
    return *this;
}

/*!
  Change of radius the scale

  Radius is the radius of the backbone without ticks and labels.

  \param radius New Radius
  \sa moveCenter
*/
void QwtRoundScaleDraw::setRadius(int radius)
{
    d_data->radius = radius;
}

/*!
  Get the radius 

  Radius is the radius of the backbone without ticks and labels.

  \sa setRadius(), extent()
*/
int QwtRoundScaleDraw::radius() const
{
    return d_data->radius;
}

/*!
   Move the center of the scale draw, leaving the radius unchanged

   \param center New center
   \sa setRadius
*/
void QwtRoundScaleDraw::moveCenter(const QPoint &center)
{
    d_data->center = center;
}

//! Get the center of the scale
QPoint QwtRoundScaleDraw::center() const
{
    return d_data->center;
}

/*!
  \brief Adjust the baseline circle segment for round scales.

  The baseline will be drawn from min(angle1,angle2) to max(angle1, angle2).
  The default setting is [ -135, 135 ].
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
void QwtRoundScaleDraw::setAngleRange(double angle1, double angle2)
{
    angle1 = qwtLim(angle1, -360.0, 360.0);
    angle2 = qwtLim(angle2, -360.0, 360.0);

    int amin = qRound(qwtMin(angle1, angle2) * 16.0);
    int amax = qRound(qwtMax(angle1, angle2) * 16.0); 
 
    if (amin == amax)
    {
        amin -= 1;
        amax += 1;
    }
 
    d_data->minAngle = amin;
    d_data->maxAngle = amax;
    scaleMap().setPaintInterval(d_data->minAngle, d_data->maxAngle);
}

/*!
   Draws the label for a major scale tick

   \param painter Painter
   \param value Value

   \sa drawTick(), drawBackbone()
*/
void QwtRoundScaleDraw::drawLabel(QPainter *painter, double value) const
{
    const QwtText label = tickLabel(painter->font(), value);
    if ( label.isEmpty() )
        return; 

    const int tval = map().transform(value);
    if ((tval > d_data->minAngle + 359 * 16)
        || (tval < d_data->minAngle - 359 * 16))
    {
       return; 
    }

    const double arc = tval / 16.0 / 360.0 * 2 * M_PI;

    QRect r( QPoint(0, 0), label.textSize(painter->font()) );
    r.moveCenter(labelCenter(painter->font(), arc, label));

    label.draw(painter, r);
}

/*!
   Draw a tick
   
   \param painter Painter
   \param value Value of the tick
   \param len Lenght of the tick

   \sa drawBackbone(), drawLabel() 
*/
void QwtRoundScaleDraw::drawTick(QPainter *painter, double value, int len) const
{
    if ( len <= 0 )
        return;

    const int tval = map().transform(value);

    const int cx = d_data->center.x();
    const int cy = d_data->center.y();
    const int radius = d_data->radius;

    if ((tval <= d_data->minAngle + 359 * 16)
        || (tval >= d_data->minAngle - 359 * 16))
    {
        const double arc = double(tval) / 16.0 * M_PI / 180.0;

        const double sinArc = sin(arc);
        const double cosArc = cos(arc);

        const int x1 = qRound( cx + radius * sinArc );
        const int x2 = qRound( cx + (radius + len) * sinArc );
        const int y1 = qRound( cy - radius * cosArc );
        const int y2 = qRound( cy - (radius + len) * cosArc );

        QwtPainter::drawLine(painter, x1, y1, x2, y2);
    }
}

/*!
   Draws the baseline of the scale
   \param painter Painter

   \sa drawTick(), drawLabel()
*/
void QwtRoundScaleDraw::drawBackbone(QPainter *painter) const
{
    const int a1 = qRound(qwtMin(map().p1(), map().p2()) - 90 * 16);
    const int a2 = qRound(qwtMax(map().p1(), map().p2()) - 90 * 16);

    const int radius = d_data->radius;
    const int x = d_data->center.x() - radius;
    const int y = d_data->center.y() - radius;

    painter->drawArc(x, y, 2 * radius, 2 * radius,
        -a2, a2 - a1 + 1);           // counterclockwise
}

/*!
   Calculate the extent of the scale

   The extent is the distcance between the baseline to the outermost
   pixel of the scale draw. radius() + extent() is an upper limit 
   for the radius of the bounding circle.

   \param pen Pen that is used for painting backbone and ticks
   \param font Font used for painting the labels

   \sa setMinimumExtent(), minimumExtent()
   \warning The implemented algo is not too smart and
            calculates only an upper limit, that might be a
            few pixels too large
*/
int QwtRoundScaleDraw::extent(const QPen &pen, const QFont &font) const
{
    int d = 0;

    if ( hasComponent(QwtAbstractScaleDraw::Labels) )
    {
        const QwtScaleDiv &sd = scaleDiv();
        const QwtTickList &ticks = sd.ticks(QwtScaleDiv::MajorTick);
        for (uint i = 0; i < (uint)ticks.count(); i++)
        {
            const double v = ticks[i];
            if ( sd.contains(v) )
            {
                const QRect r = labelRect(font, v);
                if ( !r.isEmpty() )
                {
                    int dx = r.center().x() - d_data->center.x();
                    int dy = r.center().y() - d_data->center.y();

                    const int dist1 = 
                        qRound(sqrt((double)(dx * dx + dy * dy)));

                    dx = r.width() / 2 + r.width() % 2;
                    dy = r.height() / 2 + r.height() % 2;

                    const int dist2 = 
                        qRound(sqrt((double)(dx * dx + dy * dy)));

                    int dist = dist1 + dist2;
                    if ( dist > d )
                        d = dist;
                }
            }
        }
        d -= d_data->radius;
    }

    if ( d == 0 )
    {
        if ( hasComponent(QwtAbstractScaleDraw::Ticks) )
        {
            d += majTickLength();
        }

        if ( hasComponent(QwtAbstractScaleDraw::Backbone) )
        {
            const int pw = qwtMax( 1, pen.width() );  // penwidth can be zero
            d += pw;
        }

    }
    d = qwtMax(d, minimumExtent());

    return d;
}

/*!
  Find the size of the label. 

  \param font Font
  \param value Value
*/
QSize QwtRoundScaleDraw::labelSize(
    const QFont &font, double value) const
{   
    return tickLabel(font, value).textSize(font);
}

/*!
  Calculate the bounding rect for a tick label

  \param font Font, that will be used to paint the scale draw 
  \param value Value of the tick label

  \sa labelSize(), labelCenter()
*/
QRect QwtRoundScaleDraw::labelRect(const QFont &font, double value) const
{
    const QwtText label = tickLabel(font, value);
    if ( label.isEmpty() )
        return QRect(); 

    const int tval = map().transform(value);
    if ((tval > d_data->minAngle + 359 * 16)
        || (tval < d_data->minAngle - 359 * 16))
    {
       return QRect();
    }

    const double arc = tval / 16.0 / 360.0 * 2 * M_PI;

    QRect r( QPoint(0, 0), label.textSize(font) );
    r.moveCenter(labelCenter(font, arc, label));

    return r;
}

/*!
  Find the position, where to move the center of a label 
*/
QPoint QwtRoundScaleDraw::labelCenter( 
    const QFont &font, double arc, const QwtText& label) const
{   
    const QFont fnt = label.usedFont(font);

    QFontMetrics fm(fnt);
    const int fmh = fm.ascent() - 2;

    double radius = d_data->radius + spacing();
    if ( hasComponent(QwtAbstractScaleDraw::Ticks) )
        radius += majTickLength();

    // First we find the point on a circle enlarged
    // by half of the font height.

    double xOffset = ( radius + fmh / 2 ) * sin(arc);
    double yOffset = ( radius + fmh / 2 ) * cos(arc);

    if ( qRound(xOffset) != 0 )
    {
        // The centered label might cut the circle
        // with distance: d_data->radius + d_data->majLen + d_data->vpad
        // We align the label to the circle by moving
        // the x-coordinate, because we have only
        // horizontal labels here.

        const int brw = label.textSize(font).width();

        const double circleX = radius * sin(arc);
        if ( xOffset < 0 )
            xOffset = circleX - brw / 2; // left
        else
            xOffset = circleX + brw / 2; // right
    }
    const int x = d_data->center.x() + qRound(xOffset);
    const int y = d_data->center.y() - qRound(yOffset);

    return QPoint(x, y);
}
