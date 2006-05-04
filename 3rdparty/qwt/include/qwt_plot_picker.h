/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

// vim: expandtab

#ifndef QWT_PLOT_PICKER
#define QWT_PLOT_PICKER

#include "qwt_double_rect.h"
#include "qwt_array.h"
#include "qwt_plot_canvas.h"
#include "qwt_picker.h"

class QwtPlot;

/*!
  \brief QwtPlotPicker provides selections on a plot canvas

  QwtPlotPicker is a QwtPicker tailored for selections on
  a plot canvas. It is set to a x-Axis and y-Axis and
  translates all pixel coordinates into this coodinate system.

  \warning Calling QwtPlot::setAxisScale() while QwtPlot::autoReplot() is FALSE
           leaves the axis in an 'intermediate' state.
           In this case, to prevent buggy behaviour, your must call
       QwtPlot::replot() before calling QwtPlotPicker::scaleRect(),
       QwtPlotZoomer::scaleRect(), QwtPlotPicker::QwtPlotPicker() or
       QwtPlotZoomer::QwtPlotZoomer().
           This quirk will be removed in a future release.
*/

class QWT_EXPORT QwtPlotPicker: public QwtPicker
{
    Q_OBJECT

public:
    QwtPlotPicker(QwtPlotCanvas *, const char *name = 0);

    QwtPlotPicker(int xAxis, int yAxis,
        QwtPlotCanvas *, const char *name = 0);

    QwtPlotPicker(int xAxis, int yAxis, int selectionFlags, 
        RubberBand rubberBand, DisplayMode cursorLabelMode, 
        QwtPlotCanvas *, const char *name = 0);

    virtual void setAxis(int xAxis, int yAxis);

    int xAxis() const;
    int yAxis() const;

    QwtPlot *plot();
    const QwtPlot *plot() const;
    
    QwtPlotCanvas *canvas();
    const QwtPlotCanvas *canvas() const;

signals:

    /*!
      A signal emitted in case of selectionFlags() & PointSelection.
      \param pos Selected point
    */
    void selected(const QwtDoublePoint &pos);

    /*!
      A signal emitted in case of selectionFlags() & RectSelection.
      \param rect Selected rectangle
    */
    void selected(const QwtDoubleRect &rect);

    /*!
      A signal emitting the selected points,
      at the end of a selection.

      \param pa Selected points
    */
    void selected(const QwtArray<QwtDoublePoint> &pa);

    /*!
      A signal emitted when a point has been appended to the selection

      \param pos Position of the appended point.
      \sa append(). moved()
    */
    void appended(const QwtDoublePoint &pos);

    /*!
      A signal emitted whenever the last appended point of the
      selection has been moved.

      \param pos Position of the moved last point of the selection.
      \sa move(), appended() 
    */
    void moved(const QwtDoublePoint &pos);

protected:
    QwtDoubleRect scaleRect() const;

    QwtDoubleRect invTransform(const QRect &) const;
    QRect transform(const QwtDoubleRect &) const;

    QwtDoublePoint invTransform(const QPoint &) const;
    QPoint transform(const QwtDoublePoint &) const;

    virtual QString cursorLabel(const QPoint &) const;
    virtual QString cursorLabel(const QwtDoublePoint &) const;

    virtual void move(const QPoint &);
    virtual void append(const QPoint &);
    virtual bool end(bool ok = TRUE);

private:
    int d_xAxis;
    int d_yAxis;
};
            
#endif

// Local Variables:
// mode: C++
// c-file-style: "stroustrup"
// indent-tabs-mode: nil
// End:
