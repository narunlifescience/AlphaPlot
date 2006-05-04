/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

// vim: expandtab

#ifndef QWT_PLOT_ZOOMER
#define QWT_PLOT_ZOOMER

#include <qvaluestack.h>
#include "qwt_double_rect.h"
#include "qwt_plot_picker.h"

#if defined(QWT_TEMPLATEDLL)
// MOC_SKIP_BEGIN
template class QWT_EXPORT QValueStack<QwtDoubleRect>;
// MOC_SKIP_END
#endif

/*!
  \brief QwtPlotZoomer provides stacked zooming for a plot widget

  QwtPlotZoomer offers rubberband selections on the plot canvas, 
  translating the selected rectangles into plot coordinates and
  adjusting the axes to them. Zooming can repeated as often as 
  possible, limited only by maxStackDepth() or minZoomSize(). 
  Each rectangle is pushed on a stack. 

  Zoom rectangles can be selected depending on selectionFlags() using the
  mouse or keyboard (QwtEventPattern, QwtPickerMachine).
  QwtEventPattern::MouseSelect3/QwtEventPattern::KeyUndo, 
  or QwtEventPattern::MouseSelect6/QwtEventPattern::KeyRedo 
  walk up and down the zoom stack.
  QwtEventPattern::MouseSelect2 or QwtEventPattern::KeyHome unzoom to
  the initial size. 
  
  QwtPlotZoomer is tailored for plots with one x and y axis, but it is
  allowed to attach a second QwtPlotZoomer for the other axes, even when
  one or both of them are disabled. In most situations it is 
  recommended to disable rubber band and cursor label for one of the zoomers.

  \note The realtime example includes an derived zoomer class that adds 
        scrollbars to the plot canvas.

  \warning Calling QwtPlot::setAxisScale() while QwtPlot::autoReplot() is FALSE
           leaves the axis in an 'intermediate' state.
           In this case, to prevent buggy behaviour, your must call
       QwtPlot::replot() before calling QwtPlotPicker::scaleRect(),
       QwtPlotZoomer::scaleRect(), QwtPlotPicker::QwtPlotPicker() or
       QwtPlotZoomer::QwtPlotZoomer().
           This quirk will be removed in a future release.
*/

class QWT_EXPORT QwtPlotZoomer: public QwtPlotPicker
{
    Q_OBJECT
public:
    QwtPlotZoomer(QwtPlotCanvas *, const char *name = 0);
    QwtPlotZoomer(int xAxis, int yAxis, 
        QwtPlotCanvas *, const char *name = 0);
    QwtPlotZoomer(int xAxis, int yAxis, int selectionFlags,
        DisplayMode cursorLabelMode, QwtPlotCanvas *, const char *name = 0);

    virtual void setZoomBase();
    virtual void setZoomBase(const QwtDoubleRect &);

    QwtDoubleRect zoomBase() const;
    QwtDoubleRect zoomRect() const;

    virtual void setAxis(int xAxis, int yAxis);

    void setMaxStackDepth(int);
    int maxStackDepth() const;

    const QValueStack<QwtDoubleRect> &zoomStack() const;
    uint zoomRectIndex() const;

    virtual void setSelectionFlags(int);

public slots:
    void moveBy(double x, double y);
    virtual void move(double x, double y);

    virtual void zoom(const QwtDoubleRect &);
    virtual void zoom(int up);

signals:
    /*!
      A signal emitting the zoomRect(), when the plot has been 
      zoomed in or out.

      \param rect Current zoom rectangle.
      \warning zoomed() is only emitted after selections in end(), but never
               in zoom().
    */

    void zoomed(const QwtDoubleRect &rect);

protected:
    QValueStack<QwtDoubleRect> &zoomStack();

    virtual void rescale();

    virtual QwtDoubleSize minZoomSize() const;

    virtual void widgetMouseReleaseEvent(QMouseEvent *);
    virtual void widgetKeyPressEvent(QKeyEvent *);

    virtual void begin();
    virtual bool end(bool ok = TRUE);
    virtual bool accept(QPointArray &) const;

private:
    void init(int selectionFlags = RectSelection & ClickSelection, 
        DisplayMode cursorLabelMode = ActiveOnly);

    uint d_zoomRectIndex;
    QValueStack<QwtDoubleRect> d_zoomStack;

    int d_maxStackDepth;
};
            
#endif

// Local Variables:
// mode: C++
// c-file-style: "stroustrup"
// indent-tabs-mode: nil
// End:
