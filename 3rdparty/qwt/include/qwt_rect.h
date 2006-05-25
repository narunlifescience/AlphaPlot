/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#ifndef QWT_RECT_H
#define QWT_RECT_H

#include <qrect.h>
#if QT_VERSION < 0x040000
#include <qpointarray.h>
#else
#include <qpolygon.h>
#endif
#include "qwt_global.h"

/*!
  \brief Some extensions for QRect
*/

class QWT_EXPORT QwtRect : public QRect
{
public:
    QwtRect();
    QwtRect(const QRect &r);

#if QT_VERSION < 0x040000
    QPointArray clip(const QPointArray &) const;
#else
    QPolygon clip(const QPolygon &) const;
#endif

private:
    enum Edge { Left, Top, Right, Bottom, NEdges };

#if QT_VERSION < 0x040000
    void clipEdge(Edge, const QPointArray &, QPointArray &) const;
#else
    void clipEdge(Edge, const QPolygon &, QPolygon &) const;
#endif
    bool insideEdge(const QPoint &, Edge edge) const;
    QPoint intersectEdge(const QPoint &p1, 
        const QPoint &p2, Edge edge) const;
};

#endif
