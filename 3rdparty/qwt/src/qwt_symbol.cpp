/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#include <qpainter.h>
#include <qapplication.h>
#include "qwt_painter.h"
#include "qwt_polygon.h"
#include "qwt_symbol.h"

/*!
  Default Constructor

  The symbol is constructed with gray interior,
  black outline with zero width, no size and style 'None'.
*/
QwtSymbol::QwtSymbol(): 
    d_brush(Qt::gray), 
    d_pen(Qt::black), 
    d_size(0,0),
    d_style(QwtSymbol::None)
{
}

/*!
  \brief Constructor
  \param style Symbol Style
  \param brush brush to fill the interior
  \param pen outline pen 
  \param size size
*/
QwtSymbol::QwtSymbol(QwtSymbol::Style style, const QBrush &brush, 
        const QPen &pen, const QSize &size): 
    d_brush(brush), 
    d_pen(pen), 
    d_size(size),
    d_style(style)
{
}

//! Destructor
QwtSymbol::~QwtSymbol()
{
}

/*!
  \brief Specify the symbol's size

  If the 'h' parameter is left out or less than 0,
  and the 'w' parameter is greater than or equal to 0,
  the symbol size will be set to (w,w).
  \param w width
  \param h height (defaults to -1)
*/
void QwtSymbol::setSize(int w, int h)
{
    if ((w >= 0) && (h < 0)) 
        h = w;
    d_size = QSize(w,h);
}

//! Set the symbol's size
void QwtSymbol::setSize(const QSize &s)
{
    if (s.isValid()) 
        d_size = s;
}

/*!
  \brief Assign a brush

  The brush is used to draw the interior of the symbol.
  \param br brush
*/
void QwtSymbol::setBrush(const QBrush &br)
{
    d_brush = br;
}

/*!
  \brief Assign a pen

  The pen is used to draw the symbol's outline.

  \param pn pen
*/
void QwtSymbol::setPen(const QPen &pn)
{
    d_pen = pn;
}

/*!
  \brief Draw the symbol at a point (x,y).
*/
void QwtSymbol::draw(QPainter *painter, int x, int y) const
{
    draw(painter, QPoint(x, y));
}


/*!
  \brief Draw the symbol into a bounding rectangle.

  This function assumes that the painter has been initialized with
  brush and pen before. This allows a much more performant implementation
  when painting many symbols with the same brush and pen like in curves.

  \param painter Painter
  \param r Bounding rectangle
*/
void QwtSymbol::draw(QPainter *painter, const QRect& r) const
{
    switch(d_style)
    {
        case QwtSymbol::Ellipse:
            QwtPainter::drawEllipse(painter, r);
            break;
        case QwtSymbol::Rect:
            QwtPainter::drawRect(painter, r);
            break;
        case QwtSymbol::Diamond:
        {
            const int w2 = r.width() / 2;
            const int h2 = r.height() / 2;

            QwtPolygon pa(4);
            pa.setPoint(0, r.x() + w2, r.y());
            pa.setPoint(1, r.right(), r.y() + h2);
            pa.setPoint(2, r.x() + w2, r.bottom());
            pa.setPoint(3, r.x(), r.y() + h2);
            QwtPainter::drawPolygon(painter, pa);
            break;
        }
        case QwtSymbol::Cross:
        {
            const int w2 = r.width() / 2;
            const int h2 = r.height() / 2;

            QwtPainter::drawLine(painter, r.x() + w2, r.y(), 
                r.x() + w2, r.bottom());
            QwtPainter::drawLine(painter, r.x(), r.y() + h2, 
                r.right(), r.y() + h2);
            break;
        }
        case QwtSymbol::XCross:
        {
            QwtPainter::drawLine(painter, r.left(), r.top(), 
                r.right(), r.bottom());
            QwtPainter::drawLine(painter, r.left(), r.bottom(), 
                r.right(), r.top());
            break;
        }
        case QwtSymbol::Triangle:
        case QwtSymbol::UTriangle:
        {
            const int w2 = r.width() / 2;

            QwtPolygon pa(3);
            pa.setPoint(0, r.x() + w2, r.y());
            pa.setPoint(1, r.right(), r.bottom());
            pa.setPoint(2, r.x(), r.bottom());
            QwtPainter::drawPolygon(painter, pa);
            break;
        }
        case QwtSymbol::DTriangle:
        {
            const int w2 = r.width() / 2;

            QwtPolygon pa(3);
            pa.setPoint(0, r.x(), r.y());
            pa.setPoint(1, r.right(), r.y());
            pa.setPoint(2, r.x() + w2, r.bottom());
            QwtPainter::drawPolygon(painter, pa);
            break;
        }
        case QwtSymbol::RTriangle:
        {
            const int h2 = r.height() / 2;

            QwtPolygon pa(3);
            pa.setPoint(0, r.x(), r.y());
            pa.setPoint(1, r.right(), r.y() + h2);
            pa.setPoint(2, r.x(), r.bottom());
            QwtPainter::drawPolygon(painter, pa);
            break;
        }
        case QwtSymbol::LTriangle:
        {
            const int h2 = r.height() / 2;

            QwtPolygon pa(3);
            pa.setPoint(0, r.right(), r.y());
            pa.setPoint(1, r.x(), r.y() + h2);
            pa.setPoint(2, r.right(), r.bottom());
            QwtPainter::drawPolygon(painter, pa);
            break;
        }
        default:;
    }
}

/*!
  \brief Draw the symbol at a specified point

  \param painter Painter
  \param pos Center of the symbol
*/
void QwtSymbol::draw(QPainter *painter, const QPoint &pos) const
{
    QRect rect;
    rect.setSize(QwtPainter::metricsMap().screenToLayout(d_size));
    rect.moveCenter(pos);

    painter->setBrush(d_brush);
    painter->setPen(d_pen);
    
    draw(painter, rect);
}

/*!
  \brief Specify the symbol style

  The following styles are defined:<dl>
  <dt>QwtSymbol::None<dd>No Style. The symbol cannot be drawn.
  <dt>QwtSymbol::Ellipse<dd>Ellipse or circle
  <dt>QwtSymbol::Rect<dd>Rectangle
  <dt>QwtSymbol::Diamond<dd>Diamond
  <dt>QwtSymbol::Triangle<dd>Triangle pointing upwards
  <dt>QwtSymbol::DTriangle<dd>Triangle pointing downwards
  <dt>QwtSymbol::UTriangle<dd>Triangle pointing upwards
  <dt>QwtSymbol::LTriangle<dd>Triangle pointing left
  <dt>QwtSymbol::RTriangle<dd>Triangle pointing right
  <dt>QwtSymbol::Cross<dd>Cross
  <dt>QwtSymbol::XCross<dd>Diagonal cross</dl>
  \param s style
*/
void QwtSymbol::setStyle(QwtSymbol::Style s)
{
    d_style = s;
}

//! == operator
bool QwtSymbol::operator==(const QwtSymbol &other) const
{
    return brush() == other.brush() && pen() == other.pen()
            && style() == other.style() && size() == other.size();
}

//! != operator
bool QwtSymbol::operator!=(const QwtSymbol &other) const
{
    return !(*this == other);
}
