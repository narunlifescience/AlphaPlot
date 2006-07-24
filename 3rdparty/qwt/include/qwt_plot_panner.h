/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#ifndef QWT_PLOT_PANNER_H
#define QWT_PLOT_PANNER_H 1

#include "qwt_global.h"
#include "qwt_panner.h"

class QwtPlotCanvas;
class QwtPlot;

class QWT_EXPORT QwtPlotPanner: public QwtPanner
{
    Q_OBJECT

public:
    explicit QwtPlotPanner(QwtPlotCanvas *);
    virtual ~QwtPlotPanner();

    QwtPlotCanvas *canvas();
    const QwtPlotCanvas *canvas() const;

    QwtPlot *plot();
    const QwtPlot *plot() const;

    void setAxisEnabled(int axis, bool on);
    bool isAxisEnabled(int axis) const;

protected slots:
    virtual void moveCanvas(int dx, int dy);

private:
    class PrivateData;
    PrivateData *d_data;
};

#endif
