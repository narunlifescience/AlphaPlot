/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

// vim: expandtab

#ifndef QWT_MARKER_H
#define QWT_MARKER_H

#include <qpen.h>
#include <qfont.h>
#include <qstring.h>
#include "qwt_global.h"
#include "qwt_symbol.h"
#include "qwt_text.h"

class QRect;

/*!
  \brief A class for drawing markers

  A marker can be a horizontal line, a vertical line,
  a symbol, a label or any combination of them, which can
  be drawn around a center point inside a bounding rectangle.

  The QwtMarker::setSymbol() member assigns a symbol to the marker.
  The symbol is drawn at the specified point.

  With QwtMarker::setLabel(), a label can be assigned to the marker.
  The QwtMarker::setLabelAlignment() member specifies where the label is
  drawn. All the Align*-constants in Qt::AlignmentFlags (see Qt documentation)
  are valid. The interpretation of the alignment depends on the marker's
  line style. The alignment refers to the center point of
  the marker, which means, for example, that the label would be printed
  left above the center point if the alignment was set to AlignLeft|AlignTop.
*/

class QWT_EXPORT QwtMarker
{
public:

    /*!
        Line styles.
        \sa QwtMarker::setLineStyle, QwtMarker::lineStyle
    */
    enum LineStyle {NoLine, HLine, VLine, Cross};
   
    QwtMarker();
    virtual ~QwtMarker();

    QwtMarker(const QwtMarker&);
    QwtMarker &operator=(const QwtMarker&);
    
    void setLineStyle(LineStyle st);
    LineStyle lineStyle() const;

    void setSymbol(const QwtSymbol &s);
    const QwtSymbol &symbol() const;

    void setLabel(const QString &text, const QFont &font = QFont(),
       const QColor &color = QColor(), const QPen &pen = QPen(Qt::NoPen),
       const QBrush &brush = QBrush(Qt::NoBrush));

    void setLabelText(const QString &text);
    const QString label() const;

    void setLabelAlignment(int align);
    int labelAlignment() const;

    void setLabelColor(const QColor &c);
    const QColor labelColor() const;

    void setLabelPen(const QPen &p);
    const QPen labelPen() const;

    void setLinePen(const QPen &p);
    const QPen &linePen() const;

    void setFont(const QFont &f);
    const QFont font() const;
    
    virtual void draw(QPainter *p, int x, int y, const QRect &r);
    
protected:
    virtual void markerChanged();
 
private:
    QwtText *d_label;
    int d_align;
    QPen d_pen;
    QwtSymbol d_sym;
    LineStyle d_style;
};

#endif

// Local Variables:
// mode: C++
// c-file-style: "stroustrup"
// indent-tabs-mode: nil
// End:
