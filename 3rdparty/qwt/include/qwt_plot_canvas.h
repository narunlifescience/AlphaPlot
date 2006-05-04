/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

// vim: expandtab

#ifndef QWT_PLOT_CANVAS_H
#define QWT_PLOT_CANVAS_H

#include <qframe.h>
#include <qpen.h>
#include "qwt_global.h"
#include "qwt.h"

class QwtPlot;
class QPixmap;

/*!
  \brief Canvas of a QwtPlot. 

  \sa  QwtPlot 
*/

class QWT_EXPORT QwtPlotCanvas : public QFrame
{
    Q_OBJECT
    friend class QwtPlot;

public:
    /*!
      \brief Focus indicator

      - NoFocusIndicator\n
        Don´t paint a focus indicator
      - CanvasFocusIndicator\n
        The focus is related to the complete canvas.
        Paint the focus indicator using paintFocus()
      - ItemFocusIndicator\n
        The focus is related to an item (curve, point, ...) on
        the canvas. It is up to the application to display a
        focus indication using f.e. highlighting.

      \sa setFocusIndicator(), focusIndicator(), paintFocus()
    */

    enum FocusIndicator
    {
        NoFocusIndicator,
        CanvasFocusIndicator,
        ItemFocusIndicator
    };

    void setFocusIndicator(FocusIndicator);
    FocusIndicator focusIndicator() const;

    void setCacheMode(bool on);
    bool cacheMode() const;
    const QPixmap *cache() const;
    void invalidateCache();

protected:
    QwtPlotCanvas(QwtPlot *);
    virtual ~QwtPlotCanvas();

    QPixmap *cache();

    virtual void frameChanged();
    virtual void drawContents(QPainter *);
    virtual void drawFocusIndicator(QPainter *, const QRect &);

    void drawCanvas(QPainter *painter = NULL);

private:    
    FocusIndicator d_focusIndicator;
    bool d_cacheMode;
    QPixmap *d_cache;

#ifndef QWT_NO_COMPAT
public:
    void enableOutline(bool tf);
    bool outlineEnabled() const;

    void setOutlinePen(const QPen &p);
    const QPen& outlinePen() const;

    void setOutlineStyle(Qwt::Shape os);
    Qwt::Shape outlineStyle() const;
    
signals:
    /*!
      A signal which is emitted when the mouse is pressed in the canvas.

      \warning Outlining functionality is obsolete: use QwtPlotPicker or
      QwtPlotZoomer.
      
      \param e Mouse event object
    */
    void mousePressed(const QMouseEvent &e);
    /*!
      A signal which is emitted when a mouse button has been released in the
      canvas.

      \warning Outlining functionality is obsolete: use QwtPlotPicker or
      QwtPlotZoomer.

      \param e Mouse event object
    */
    void mouseReleased(const QMouseEvent &e);
    /*!
      A signal which is emitted when the mouse is moved in the canvas.

      \warning Outlining functionality is obsolete: use QwtPlotPicker or
      QwtPlotZoomer.

      \param e Mouse event object
    */
    void mouseMoved(const QMouseEvent &e);

protected:
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *);

private:    
    bool d_outlineEnabled;
    bool d_outlineActive;
    bool d_mousePressed;

    void drawOutline(QPainter &p);

    Qwt::Shape d_outline;
    QPen d_pen;
    QPoint d_entryPoint;
    QPoint d_lastPoint;
#else
private:
    // needed because of mocs #ifdef ignorance
    void mousePressed(const QMouseEvent &);
    void mouseReleased(const QMouseEvent &);
    void mouseMoved(const QMouseEvent &);
#endif // !QWT_NO_COMPAT

};

#endif

// Local Variables:
// mode: C++
// c-file-style: "stroustrup"
// indent-tabs-mode: nil
// End:
