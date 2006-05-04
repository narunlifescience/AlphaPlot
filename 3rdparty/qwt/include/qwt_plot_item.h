/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#ifndef QWT_PLOT_ITEM
#define QWT_PLOT_ITEM

#include "qwt_global.h"

class QwtPlot;

/*!
  \brief Base class for items on the plot canvas
*/

class QWT_EXPORT QwtPlotItem
{
public:
    QwtPlotItem(QwtPlot *parent, bool nbl = TRUE);
    virtual ~QwtPlotItem();

    void reparent(QwtPlot *plot);

    //! Return parent plot
    QwtPlot *parentPlot() { return d_parent; }

    //! Return parent plot
    const QwtPlot *parentPlot() const { return d_parent; }
    
    virtual void setEnabled(bool);
    bool enabled() const;

    virtual void itemChanged();

private:
    bool d_enabled;
    QwtPlot *d_parent;
};
            

/*!
  \brief Base class for items on the plot canvas,
  that are attached to x and y axes.
*/
class QWT_EXPORT QwtPlotMappedItem : public QwtPlotItem 
{
public:
    QwtPlotMappedItem(QwtPlot *parent, bool nbl = TRUE);
    
    void setAxis(int xAxis, int yAxis);

    void setXAxis(int axis);
    int xAxis() const;

    void setYAxis(int axis);
    int yAxis() const;

private:
    int d_xAxis;
    int d_yAxis;
};

#endif
