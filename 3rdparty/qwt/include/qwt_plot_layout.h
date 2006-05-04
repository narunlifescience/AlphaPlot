/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#ifndef QWT_PLOT_LAYOUT_H
#define QWT_PLOT_LAYOUT_H

#include <qlayout.h>
#include "qwt_global.h"
#include "qwt_plot.h"

class QwtPlotLayoutData;

/*!
  \brief Layout class for QwtPlot.

  Organizes the geometry for the different QwtPlot components.
*/

class QWT_EXPORT QwtPlotLayout
{
public:
    enum Options
    {
        AlignScales = 1,
        IgnoreScrollbars = 2,
        IgnoreFrames = 4,
        IgnoreMargin = 8,
        IgnoreLegend = 16
    };

    QwtPlotLayout();
    virtual ~QwtPlotLayout();

    void setMargin(int);
    int margin() const;

    void setCanvasMargin(int margin, int axis = -1);
    int canvasMargin(int axis) const;

    void setAlignCanvasToScales(bool);
    bool alignCanvasToScales() const;

    void setSpacing(int);
    int spacing() const;

    void setLegendPosition(QwtPlot::Position pos, double ratio);
    void setLegendPosition(QwtPlot::Position pos);
    QwtPlot::Position legendPosition() const;

    void setLegendRatio(double ratio);
    double legendRatio() const;

#ifndef QWT_NO_COMPAT    
    // The following two functions are deprecated
    void setLegendPos(int pos, double ratio = 0.0);
    int legendPos() const;
#endif

    virtual QSize minimumSizeHint(const QwtPlot *) const;    

    virtual void activate(const QwtPlot *, 
        const QRect &rect, int options = 0);

    virtual void invalidate();

    const QRect &titleRect() const;
    const QRect &legendRect() const;
    const QRect &scaleRect(int axis) const;
    const QRect &canvasRect() const;

protected:

    QRect layoutLegend(int options, const QRect &) const;
    QRect alignLegend(const QRect &canvasRect, 
        const QRect &legendRect) const;

    void expandLineBreaks(int options, const QRect &rect, 
        int &dimTitle, int dimAxes[QwtPlot::axisCnt]) const;

    void alignScales(int options, QRect &canvasRect,
        QRect scaleRect[QwtPlot::axisCnt]) const;

    QRect d_titleRect;
    QRect d_legendRect;
    QRect d_scaleRect[QwtPlot::axisCnt];
    QRect d_canvasRect;

    QwtPlotLayoutData *d_layoutData;

private:
    QwtPlot::Position d_legendPos;
    double d_legendRatio;
    unsigned int d_margin;
    unsigned int d_spacing;
    unsigned int d_canvasMargin[QwtPlot::axisCnt];
    bool d_alignCanvasToScales;
};

#endif
