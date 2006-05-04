/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#ifndef QWT_PLOT_CLASSES_H
#define QWT_PLOT_CLASSES_H

#include "qwt_global.h"
#include "qwt_plot_item.h"
#include "qwt_marker.h"
#include "qwt_curve.h"
#include "qwt_grid.h"

/*!
  \brief Curve class for QwtPlot
*/

class QWT_EXPORT QwtPlotCurve: public QwtCurve, public QwtPlotMappedItem
{
public:
    //! Constructor
    QwtPlotCurve(QwtPlot* parent, const QString &title = QString::null): 
        QwtCurve(title), 
        QwtPlotMappedItem(parent) 
    {}

    //! Assignment
    const QwtPlotCurve& operator=(const QwtPlotCurve &c)
    {
        QwtPlotMappedItem::operator=((const QwtPlotMappedItem &)c);
        QwtCurve::operator=(c);
        return *this;
    } 

    //! Calls itemChanged()
    virtual void curveChanged() { itemChanged(); }
};

/*!
  \brief Grid class for QwtPlot
*/
class QWT_EXPORT QwtPlotGrid: public QwtGrid, public QwtPlotMappedItem
{
public:
    //! Constructor
    QwtPlotGrid(QwtPlot *parent): 
        QwtPlotMappedItem(parent) {}

    //! Calls itemChanged()
    virtual void gridChanged() { itemChanged(); }
};


/*!
  \brief Marker class for QwtPlot
*/
class QWT_EXPORT QwtPlotMarker: public QwtMarker, public QwtPlotMappedItem 
{
public:
    //! Constructor
    QwtPlotMarker(QwtPlot *parent): 
        QwtPlotMappedItem(parent),
        d_xValue(0.0),
        d_yValue(0.0)
    {}

    //! Return x Value
    double xValue() const { return d_xValue; }

    //! Return y Value
    double yValue() const { return d_yValue; }

    //! Set X Value
    void setXValue(double val) { d_xValue = val; markerChanged(); }

    //! Set Y Value
    void setYValue(double val) { d_yValue = val; markerChanged(); }

    //! Calls itemChanged()
    virtual void markerChanged() { itemChanged(); }

private:
    double d_xValue;
    double d_yValue;
};

#endif
