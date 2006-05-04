/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

// vim: expandtab

#include <qpainter.h>
#include "qwt_painter.h"
#include "qwt_marker.h"
#include "qwt_math.h"

static const int LabelDist = 2;

//! Sets alignment to Qt::AlignCenter, and style to NoLine
QwtMarker::QwtMarker()
{
    // QwtMarker.draw() assumes that d_label->alignment() == Qt::AlignCenter. 
    d_label = QwtText::makeText(QString::null, Qt::AlignCenter, QFont());
    d_align = Qt::AlignCenter;
    d_style = NoLine;
}

//! Destructor
QwtMarker::~QwtMarker()
{
    delete d_label;
}

/*!
  \brief Copy constructor
  \param m Marker
*/
QwtMarker::QwtMarker(const QwtMarker &m)
{
    d_label = m.d_label->clone();
    d_align = m.d_align;
    d_pen = m.d_pen;
    d_sym = m.d_sym;
    d_style = m.d_style;
}

/*!
  \brief Assignment operator
  \param m Marker
*/
QwtMarker& QwtMarker::operator=(const QwtMarker &m)
{
    if (this != &m)
    {
        d_label = m.d_label->clone();
        d_align = m.d_align;
        d_pen = m.d_pen;
        d_sym = m.d_sym;
        d_style = m.d_style;
    }

    return *this;
}

/*!
  \brief Draw the marker
  \param p Painter
  \param x X position
  \param y Y position
  \param r Bounding rectangle
*/
void QwtMarker::draw(QPainter *p, int x, int y, const QRect &r)
{
    // QwtMarker.draw() assumes that d_label->alignment() == Qt::AlignCenter. 

    // draw lines
    if (d_style != NoLine)
    {
        p->setPen(d_pen);
        if ((d_style == HLine) || (d_style == Cross))
            QwtPainter::drawLine(p, r.left(), y, r.right(), y);
        if ((d_style == VLine)||(d_style == Cross))
            QwtPainter::drawLine(p, x, r.top(), x, r.bottom());
    }

    // draw symbol
    QSize sSym(0, 0);
    if (d_sym.style() != QwtSymbol::None)
    {
        sSym = d_sym.size();
        d_sym.draw(p, x, y);
    }

    // draw label
    if (!d_label->text().isEmpty())
    {
        int xlw = qwtMax(int(d_pen.width()), 1);
        int ylw = xlw;
        int xlw1;
        int ylw1;

        const int xLabelDist = 
            QwtPainter::metricsMap().screenToLayoutX(LabelDist);
        const int yLabelDist = 
            QwtPainter::metricsMap().screenToLayoutY(LabelDist);

        if ((d_style == VLine) || (d_style == HLine))
        {
            xlw1 = (xlw + 1) / 2 + xLabelDist;
            xlw = xlw / 2 + xLabelDist;
            ylw1 = (ylw + 1) / 2 + yLabelDist;
            ylw = ylw / 2 + yLabelDist;
        }
        else 
        {
            xlw1 = qwtMax((xlw + 1) / 2, (sSym.width() + 1) / 2) + xLabelDist;
            xlw = qwtMax(xlw / 2, (sSym.width() + 1) / 2) + xLabelDist;
            ylw1 = qwtMax((ylw + 1) / 2, (sSym.height() + 1) / 2) + yLabelDist;
            ylw = qwtMax(ylw / 2, (sSym. height() + 1) / 2) + yLabelDist;
        }

        // tr is offset with respect to (0, 0) obeying the immutable flags 
        // Qt::AlignCenter. Hence: 
        // tr.x() == -tr.width()/2
        // tr.y() == -tr.height()/2
        QRect tr = d_label->boundingRect(p);

        int dx = x;
        int dy = y;

        if (d_style == VLine)
        {
            if (d_align & (int) Qt::AlignTop)
                dy = r.top() + yLabelDist - tr.y();
            else if (d_align & (int) Qt::AlignBottom)
                dy = r.bottom() - yLabelDist + tr.y();
            else
                dy = r.top() + r.height() / 2;
        }
        else
        {
            if (d_align & (int) Qt::AlignTop)
                dy += tr.y() - ylw1;
            else if (d_align & (int) Qt::AlignBottom)
                dy -= tr.y() - ylw1;
        }


        if (d_style == HLine)
        {
            if (d_align & (int) Qt::AlignLeft)
                dx = r.left() + xLabelDist - tr.x();
            else if (d_align & (int) Qt::AlignRight)
                dx = r.right() - xLabelDist + tr.x();
            else
                dx = r.left() + r.width() / 2;
        }
        else
        {
            if (d_align & (int) Qt::AlignLeft)
                dx += tr.x() - xlw1;
            else if (d_align & (int) Qt::AlignRight)
                dx -= tr.x() - xlw1;
        }

        tr.moveBy(dx, dy);
        d_label->draw(p, tr);
    }
}

/*!
  \brief Specify the font for the label
  \param f New font
  \sa QwtMarker::font()
*/
void QwtMarker::setFont(const QFont &f)
{
    if ( f == d_label->font() )
        return;
    
    d_label->setFont(f);
    markerChanged();
}

/*!
  \return the font used for the label
  \sa QwtMarker::setFont()
*/
const QFont QwtMarker::font() const 
{ 
    return d_label->font(); 
}


/*!
  \brief Set the line style
  \param st Line style. Can be one of QwtMarker::NoLine,
    QwtMarker::HLine, QwtMarker::VLine or QwtMarker::Cross
  \sa QwtMarker::lineStyle()
*/
void QwtMarker::setLineStyle(QwtMarker::LineStyle st)
{
    if ( st != d_style )
    {
        d_style = st;
        markerChanged();
    }
}

/*!
  \return the line style
  \sa For a description of line styles, see QwtMarker::setLineStyle()
*/
QwtMarker::LineStyle QwtMarker::lineStyle() const 
{ 
    return d_style; 
}

/*!
  \brief Assign a symbol
  \param s New symbol 
  \sa QwtSymbol, QwtMarker::symbol()
*/
void QwtMarker::setSymbol(const QwtSymbol &s)
{
    d_sym = s;
    markerChanged();
}

/*!
  \return the symbol
  \sa QwtMarker::setSymbol(), QwtSymbol
*/
const QwtSymbol &QwtMarker::symbol() const 
{ 
    return d_sym; 
}

/*!
  \brief Set the label text
  \param text label text 
  \sa QwtMarker::label()
*/
void QwtMarker::setLabelText(const QString &text)
{
    setLabel(text, d_label->font(), d_label->color(), 
        d_label->rectPen(), d_label->rectBrush());
}

/*!
  \brief Set the label
  \param text Contents of the label text
  \param font Font of the label text
  \param color Color of the label text
  \param pen Pen to draw the enclosing rectangle
  \param brush Brush to draw the enclosing rectangle
  \sa QwtMarker::label()
*/
void QwtMarker::setLabel(const QString &text, const QFont &font,
    const QColor &color, const QPen &pen, const QBrush &brush)
{
    if ( text == d_label->text()
         && font == d_label->font()
         && color == d_label->color()
         && pen == d_label->rectPen()
         && brush == d_label->rectBrush() )
        return;
    
    QwtText *label = QwtText::makeText(
        text, d_label->alignment(), font, color, pen, brush);

    delete d_label;
    d_label = label;

    markerChanged();
}

/*!
  \return the label
  \sa QwtMarker::setLabel()
*/
const QString QwtMarker::label() const 
{ 
    return d_label->text(); 
}

/*!
  \brief Set the alignment of the label

  The alignment determines where the label is drawn relative to
  the marker's position.

  \param align Alignment. A combination of AlignTop, AlignBottom,
    AlignLeft, AlignRight, AlignCenter, AlgnHCenter,
    AlignVCenter.  
  \sa QwtMarker::labelAlignment()
*/
void QwtMarker::setLabelAlignment(int align)
{
    if ( align == d_align )
        return;
    
    d_align = align;
    markerChanged();
}

/*!
  \return the label alignment
  \sa QwtMarker::setLabelAlignment()
*/
int QwtMarker::labelAlignment() const 
{ 
    return d_align; 
}

/*!
  \brief Specify a pen for the line.
  \param p New pen
  \sa QwtMarker::linePen()
*/
void QwtMarker::setLinePen(const QPen &p)
{
    if ( p != d_pen )
    {
        d_pen = p;
        markerChanged();
    }
}

/*!
  \return the line pen
  \sa QwtMarker::setLinePen()
*/
const QPen &QwtMarker::linePen() const 
{ 
    return d_pen; 
}

/*!
  \brief Specify a color for the label
  \param color Color of the label text
  \sa QwtMarker::labelColor()
*/
void QwtMarker::setLabelColor(const QColor &color)
{
    if ( color == d_label->color() )
        return;
    
    d_label->setColor(color);
    markerChanged();
}

/*!
  \return the label's color
  \sa QwtMarker::setLabelPen()
*/
const QColor QwtMarker::labelColor() const 
{ 
    return d_label->color(); 
}

/*!
  \brief Specify a pen for the label
  \param p New pen
  \sa QwtMarker::labelPen()
  \warning This function is deprecated. Use QwtMarker::setLabelColor.
*/
void QwtMarker::setLabelPen(const QPen &p)
{
    if ( p == QPen(d_label->color()) )
        return;
    
    d_label->setColor(p.color());
    markerChanged();
}

/*!
  \return the label's pen
  \sa QwtMarker::setLabelPen()
  \warning This function is deprecated. Use QwtMarker::labelColor.
*/
const QPen QwtMarker::labelPen() const 
{ 
    return QPen(d_label->color()); 
}

/*!
  \brief Notify a change of parameters

  This virtual function is called when a marker property
  has been changed and redrawing is advisable. It can be
  reimplemented by derived classes. The default implementation
  does nothing.
*/
void QwtMarker::markerChanged() 
{
}

// Local Variables:
// mode: C++
// c-file-style: "stroustrup"
// indent-tabs-mode: nil
// End:

