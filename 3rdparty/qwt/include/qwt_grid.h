/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#ifndef QWT_GRID_H
#define QWT_GRID_H

#include <qpen.h>
#include "qwt_global.h"
#include "qwt_dimap.h"
#include "qwt_scldiv.h"

class QPainter;

/*!
  \brief A class which draws a coordinate grid

  The QwtGrid class can be used to draw a coordinate grid.
  A coordinate grid consists of major and minor vertical
  and horizontal gridlines. The locations of the gridlines
  are determined by the X and Y scale divisions which can
  be assigned with QwtGrid::setXDiv and QwtGrid::setYDiv()
  The draw() member draws the grid within a bounding
  rectangle.
*/

class QWT_EXPORT QwtGrid
{
public:

    QwtGrid();
    virtual ~QwtGrid();

    virtual void draw(QPainter *p, const QRect &r, 
        const QwtDiMap &mx, const QwtDiMap &my) const;

    void enableX(bool tf);
    bool xEnabled() const;

    void enableY(bool tf);
    bool yEnabled() const;

    void enableXMin(bool tf);
    bool xMinEnabled() const;

    void enableYMin(bool tf);
    bool yMinEnabled() const;

    void setXDiv(const QwtScaleDiv &sx);
    const QwtScaleDiv &xScaleDiv() const;

    void setYDiv(const QwtScaleDiv &sy);
    const QwtScaleDiv &yScaleDiv() const;

    void setPen(const QPen &p);

    void setMajPen(const QPen &p);
    const QPen& majPen() const;

    void setMinPen(const QPen &p);
    const QPen& minPen() const;


protected:
    virtual void gridChanged();

private:

    bool d_xEnabled;
    bool d_yEnabled;
    bool d_xMinEnabled;
    bool d_yMinEnabled;

    QwtScaleDiv d_sdx;
    QwtScaleDiv d_sdy;

    QPen d_majPen;
    QPen d_minPen;
};

#endif
