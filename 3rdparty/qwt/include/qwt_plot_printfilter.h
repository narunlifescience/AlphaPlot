/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#ifndef QWT_PLOT_PRINTFILTER_H
#define QWT_PLOT_PRINTFILTER_H

#include <qcolor.h>
#include <qfont.h>
#include <qintdict.h>
#include <qvaluelist.h>
#include "qwt_global.h"

class QwtPlot;
class QwtPlotPrintFilterCache;

/*!
  \brief A base class for plot print filters

  QwtPlotPrintFilter can be used to customize QwtPlot::print.
*/

class QWT_EXPORT QwtPlotPrintFilter
{
public:
    //! Print options
    enum Options 
    {
        PrintMargin = 1,
        PrintTitle = 2,
        PrintLegend = 4,
        PrintGrid = 8,
        PrintCanvasBackground = 16,
        PrintWidgetBackground = 32,

        PrintAll = ~PrintWidgetBackground
    }; 

    //! Print items
    enum Item
    {
        Title,
        Legend,
        Curve,
        CurveSymbol,
        Marker,
        MarkerSymbol,
        MajorGrid,
        MinorGrid,
        CanvasBackground,
        AxisScale,
        AxisTitle,
        WidgetBackground
    };

    QwtPlotPrintFilter();
    virtual ~QwtPlotPrintFilter(); 

    virtual QColor color(const QColor &, Item item, int id = -1) const;
    virtual QFont font(const QFont &, Item item, int id = -1) const;

/*!
    \brief Set plot print options
    \param options Or'd QwtPlotPrintFilter::Options values
    
    \sa QwtPlotPrintFilter::options()
*/

    void setOptions(int options) { d_options = options; }

/*!
    \brief Get plot print options
    \sa QwtPlotPrintFilter::setOptions()
*/

    int options() const { return d_options; }

    virtual void apply(QwtPlot *) const;
    virtual void reset(QwtPlot *) const;

private:
    int d_options;
    QwtPlotPrintFilterCache *d_cache;
};

#endif
