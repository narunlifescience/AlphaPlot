/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#include <qpointarray.h>
#include "qwt_rect.h"

//! Constructor

QwtRect::QwtRect(): 
    QRect() 
{
}

//! Copy constructor
QwtRect::QwtRect(const QRect &r): 
    QRect(r) 
{
}

QRect QwtRect::cutTop(int h, int distTop, int distBottom)
{
    QRect rv;
    rv.setTop(top() + distTop);
    rv.setHeight(h);
    setTop(rv.bottom() + distBottom + 1);
    rv.setLeft(left());
    rv.setRight(right());
    return rv;
}

QRect QwtRect::cutBottom(int h, int distTop, int distBottom)
{
    QRect rv;
    setBottom(bottom() - h - distBottom - distTop);
    rv.setTop(bottom() + 1 + distTop);
    rv.setHeight(h);
    rv.setLeft(left());
    rv.setRight(right());
    return rv;
}

QRect QwtRect::cutLeft(int w, int distLeft, int distRight)
{
    QRect rv;
    rv.setLeft(left() + distLeft);
    rv.setWidth(w);
    setLeft(rv.right() + distRight + 1);
    rv.setTop(top());
    rv.setBottom(bottom());
    return rv;
}

QRect QwtRect::cutRight(int w, int distLeft, int distRight)
{
    QRect rv;
    setRight(right() - w - distRight - distLeft);
    rv.setLeft(right() + 1 + distLeft);
    rv.setWidth(w);
    rv.setTop(top());
    rv.setBottom(bottom());
    return rv;
}

const QwtRect& QwtRect::cutMargin(int mLeft, int mRight, int mTop, int mBottom)
{
    setHeight(height() - mTop - mBottom);
    setWidth(width() - mLeft - mRight);
    moveBy(mLeft, mTop);
    return *this;
}

inline void addPoint(QPointArray &pa, uint pos, const QPoint &point)
{
    if ( pa.size() <= pos ) 
        pa.resize(pos + 5);

    pa.setPoint(pos, point);
}

//! Sutherland-Hodgman polygon clipping

QPointArray QwtRect::clip(const QPointArray &pa) const
{
    if ( contains( pa.boundingRect() ) )
        return pa;

    QPointArray cpa(pa.size());

    for ( uint edge = 0; edge < NEdges; edge++ ) 
    {
        const QPointArray rpa = (edge == 0) ? pa : cpa.copy();
        clipEdge((Edge)edge, rpa, cpa);
    }

    return cpa;
}

bool QwtRect::insideEdge(const QPoint &p, Edge edge) const
{
    switch(edge) 
    {
        case Left:
            return p.x() > left();
        case Top:
            return p.y() > top();
        case Right:
            return p.x() < right();
        case Bottom:
            return p.y() < bottom();
        default:
            break;
    }

    return FALSE;
}

QPoint QwtRect::intersectEdge(const QPoint &p1, 
    const QPoint &p2, Edge edge ) const
{
    int x=0, y=0;
    double m = 0;

    const double dy = p2.y() - p1.y();
    const double dx = p2.x() - p1.x();

    switch ( edge ) 
    {
        case Left:
            x = left();
            m = double(QABS(p1.x() - x)) / QABS(dx);
            y = p1.y() + int(dy * m);
            break;
        case Top:
            y = top();
            m = double(QABS(p1.y() - y)) / QABS(dy);
            x = p1.x() + int(dx * m);
            break;
        case Right:
            x = right();
            m = double(QABS(p1.x() - x)) / QABS(dx);
            y = p1.y() + int(dy * m);
            break;
        case Bottom:
            y = bottom();
            m = double(QABS(p1.y() - y)) / QABS(dy);
            x = p1.x() + int(dx * m);
            break;
        default:
            break;
    }

    return QPoint(x,y);
}

void QwtRect::clipEdge(Edge edge, const QPointArray &pa, QPointArray &cpa) const
{
    if ( pa.count() == 0 )
    {
        cpa.resize(0);
        return;
    }

    unsigned int count = 0;

    QPoint p1 = pa.point(0);
    if ( insideEdge(p1, edge) )
        addPoint(cpa, count++, p1);

    const uint nPoints = pa.size();
    for ( uint i = 1; i < nPoints; i++ )
    {
        const QPoint p2 = pa.point(i);
        if ( insideEdge(p2, edge) )
        {
            if ( insideEdge(p1, edge) )
                addPoint(cpa, count++, p2);
            else
            {
                addPoint(cpa, count++, intersectEdge(p1, p2, edge));
                addPoint(cpa, count++, p2);
            }
        }
        else
        {
            if ( insideEdge(p1, edge) )
                addPoint(cpa, count++, intersectEdge(p1, p2, edge));
        }
        p1 = p2;
    }
    cpa.resize(count);
}
