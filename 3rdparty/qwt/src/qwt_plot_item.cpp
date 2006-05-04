/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#include "qwt_plot.h"
#include "qwt_plot_item.h"

//! Constructor
QwtPlotItem::QwtPlotItem(QwtPlot *parent, bool nbl)
{
    d_parent = parent;
    d_enabled = nbl;
}

//! Destroy the QwtPlotItem
QwtPlotItem::~QwtPlotItem()
{
}

//! reparent the item
void QwtPlotItem::reparent(QwtPlot *parent)
{
    d_parent = parent;
}

/*! 
    enables the item
    \sa QwtPlotItem::setEnabled
*/
void QwtPlotItem::setEnabled(bool tf) 
{ 
    if ( tf != d_enabled )
    {
        d_enabled = tf; 
        itemChanged(); 
    }
}

/*! 
    \return TRUE if enabled
    \sa QwtPlotItem::setEnabled
*/
bool QwtPlotItem::enabled()  const
{ 
    return d_enabled; 
}

//! Call QwtPlot::autoRefresh for the parent plot
void QwtPlotItem::itemChanged()
{
    if ( d_parent )
        d_parent->autoRefresh();
}

//! Constructor

QwtPlotMappedItem::QwtPlotMappedItem(QwtPlot *parent, bool nbl): 
    QwtPlotItem(parent, nbl)
{
    d_xAxis = QwtPlot::xBottom;
    d_yAxis = QwtPlot::yLeft;
}

//!  Set x and y axis
void QwtPlotMappedItem::setAxis(int xAxis, int yAxis)
{
    if (xAxis == QwtPlot::xBottom || xAxis == QwtPlot::xTop )
       d_xAxis = xAxis;

    if (yAxis == QwtPlot::yLeft || yAxis == QwtPlot::yRight )
       d_yAxis = yAxis;

    itemChanged();    
}

//!  Set y axis
void QwtPlotMappedItem::setXAxis(int axis)
{
    if (axis == QwtPlot::xBottom || axis == QwtPlot::xTop )
    {
       d_xAxis = axis;
       itemChanged();    
    }
}

//!  Set y axis
void QwtPlotMappedItem::setYAxis(int axis)
{
    if (axis == QwtPlot::yLeft || axis == QwtPlot::yRight )
    {
       d_yAxis = axis;
       itemChanged();   
    }
}

//! Return xAxis
int QwtPlotMappedItem::xAxis() const 
{ 
    return d_xAxis; 
}

//! Return yAxis
int QwtPlotMappedItem::yAxis() const 
{ 
    return d_yAxis; 
}
