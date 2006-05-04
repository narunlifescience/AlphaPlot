/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#ifndef QWT_PICKER
#define QWT_PICKER 1

#include <qobject.h>
#include <qpen.h>
#include <qfont.h>
#include <qrect.h>
#include <qpointarray.h>
#include "qwt_event_pattern.h"
#include "qwt_global.h"

class QWidget;
class QMouseEvent;
class QWheelEvent;
class QKeyEvent;
class QwtPickerMachine;

/*!
  \brief QwtPicker provides selections on a widget

  QwtPicker filters all mouse and keyboard events of a widget
  and translates them into an array of selected points. Depending
  on the QwtPicker::SelectionType the selection might be a single point,
  a rectangle or a polygon. The selection process is supported by 
  optional rubberbands (rubberband selection) and position labels. 

  QwtPicker is useful for widgets where the event handlers
  can´t be overloaded, like for components of composite widgets.
  It offers alternative handlers for mouse and key events.

  \par Example 
  \verbatim #include <qwt_picker.h>

QwtPicker *picker = new QwtPicker(widget);
picker->setCursorLabelMode(QwtPicker::ActiveOnly);
connect(picker, SIGNAL(selected(const QPointArray &)), ...);

// emit the position of clicks on widget
picker->setSelectionFlags(QwtPicker::PointSelection | QwtPicker::ClickSelection);

    ...
    
// now select rectangles
picker->setSelectionFlags(QwtPicker::RectSelection | QwtPicker::DragSelection);
picker->setRubberBand(QwtPicker::RectRubberBand); \endverbatim\n

  The selection process uses the commands begin(), append(), move() and end().
  append() adds a new point to the selection, move() changes the position of 
  the latest point. 

  The commands are initiated from a small state machine (QwtPickerMachine) 
  that translates mouse and key events. There are a couple of predefined 
  state machines for point, rect and polygon selections. The selectionFlags()
  control which one should be used. It is possible to use other machines 
  by overloading stateMachine().

  The picker is active (isActive()), between begin() and end().
  In active state the rubberband is displayed, and the cursor label is visible
  in case of cursorLabelMode is ActiveOnly or AlwaysOn.

  The cursor can be moved using the arrow keys. All selections can be aborted
  using the abort key. (QwtEventPattern::KeyPatternCode)

  \warning In case of QWidget::NoFocus the focus policy of the observed
           widget is set to QWidget::WheelFocus and mouse tracking
           will be manipulated for ClickSelection while the picker is active,
           or if cursorLabelMode() is AlwayOn.
*/

class QWT_EXPORT QwtPicker: public QObject, public QwtEventPattern
{
    Q_OBJECT

    Q_ENUMS(RubberBand)
    Q_ENUMS(DisplayMode)
    Q_ENUMS(ResizeMode)

    Q_PROPERTY(int selectionFlags READ selectionFlags WRITE setSelectionFlags)
    Q_PROPERTY(DisplayMode cursorLabelMode 
        READ cursorLabelMode WRITE setCursorLabelMode)
    Q_PROPERTY(QFont cursorLabelFont 
        READ cursorLabelFont WRITE setCursorLabelFont)
    Q_PROPERTY(RubberBand rubberBand READ rubberBand WRITE setRubberBand)
    Q_PROPERTY(ResizeMode resizeMode READ resizeMode WRITE setResizeMode)
    Q_PROPERTY(bool isEnabled READ isEnabled WRITE setEnabled)

#if QT_VERSION >= 300
    // Unfortunately moc is not aware of #ifdefs. To enable the QPen
    // attributes as properties uncomment the following lines.

    //Q_PROPERTY(QPen cursorLabelPen READ cursorLabelPen WRITE setCursorLabelPen)
    //Q_PROPERTY(QPen rubberBandPen READ rubberBandPen WRITE setRubberBandPen)
#endif

public:
    /*! 
      This enum type describes the type of a selection. It can be or´d
      with QwtPicker::RectSelectionType and QwtPicker::SelectionMode
      and passed to QwtPicker::setSelectionFlags()
      - NoSelection\n
        Selection is disabled. Note this is different to the disabled
        state, as you might have a cursor label.
      - PointSelection\n
        Select a single point.
      - RectSelection\n
        Select a rectangle.
      - PolygonSelection\n
        Select a polygon.

      The default value is NoSelection.
      \sa QwtPicker::setSelectionFlags(), QwtPicker::selectionFlags()
    */

    enum SelectionType
    {
        NoSelection = 0,
        PointSelection = 1,
        RectSelection = 2,
        PolygonSelection = 4
    };

    /*! 
      \brief Selection subtype for RectSelection
      This enum type describes the type of rectangle selections. 
      It can be or´d with QwtPicker::RectSelectionType and 
      QwtPicker::SelectionMode and passed to QwtPicker::setSelectionFlags().
      - CornerToCorner\n
        The first and the second selected point are the corners
        of the rectangle.
      - CenterToCorner\n
        The first point is the center, the second a corner of the
        rectangle.
      - CenterToRadius\n
        The first point is the center of a quadrat, calculated by the maximum 
        of the x- and y-distance.

      The default value is CornerToCorner.
      \sa QwtPicker::setSelectionFlags(), QwtPicker::selectionFlags()
    */
    enum RectSelectionType
    {
        CornerToCorner = 64,
        CenterToCorner = 128,
        CenterToRadius = 256
    };

    /*! 
      Values of this enum type or´d together with a SelectionType value
      identifies which state machine should be used for the selection.

      The default value is ClickSelection.
      \sa stateMachine()
    */
    enum SelectionMode
    {
        ClickSelection = 1024,
        DragSelection = 2048
    };

    /*! 
      Rubberband style
      - NoRubberBand\n
        No rubberband.
      - HLineRubberBand & PointSelection\n
        A horizontal line.
      - VLineRubberBand & PointSelection\n
        A vertical line.
      - CrossRubberBand & PointSelection\n
        A horizontal and a vertical line.
      - RectRubberBand & RectSelection\n
        A rectangle.
      - EllipseRubberBand & RectSelection\n
        An ellipse.
      - PolygonRubberBand &PolygonSelection\n
        A polygon.
      - UserRubberBand\n
        Values >= UserRubberBand can be used to define additional
        rubber bands.

      The default value is NoRubberBand.
      \sa QwtPicker::setRubberBand(), QwtPicker::rubberBand()
    */

    enum RubberBand
    {
        NoRubberBand = 0,

        // Point
        HLineRubberBand,
        VLineRubberBand,
        CrossRubberBand,

        // Rect
        RectRubberBand,
        EllipseRubberBand,

        // Polygon
        PolygonRubberBand,

        UserRubberBand = 100
    };

    /*! 
      - AlwaysOff\n
        Display never.
      - AlwaysOn\n
        Display always.
      - ActiveOnly\n
        Display only when the selection is active.

      \sa QwtPicker::setCursorLabelMode(), QwtPicker::cursorLabelMode(), 
          QwtPicker::isActive()
    */
    enum DisplayMode
    {
        AlwaysOff,
        AlwaysOn,
        ActiveOnly
    };

    /*! 
      Controls what to do with the selected points of an active
         selection when the observed widget is resized.
      - Stretch\n
         All points are scaled according to the new size, 
      - KeepSize\n
         All points remain unchanged.

      The default value is Stretch.
      \sa QwtPicker::setResizeMode(), QwtPicker::resize()
    */

    enum ResizeMode
    {
        Stretch,
        KeepSize
    };

    QwtPicker(QWidget *, const char *name = 0);
    QwtPicker(int selectionFlags, RubberBand rubberBand,
        DisplayMode cursorLabelMode, QWidget *, const char *name = 0);

    virtual ~QwtPicker();

    virtual void setSelectionFlags(int);
    int selectionFlags() const;

    virtual void setRubberBand(RubberBand);
    RubberBand rubberBand() const;

    virtual void setCursorLabelMode(DisplayMode);
    DisplayMode cursorLabelMode() const;

    virtual void setResizeMode(ResizeMode);
    ResizeMode resizeMode() const;

    virtual void setRubberBandPen(const QPen &);
    QPen rubberBandPen() const;

    virtual void setCursorLabelPen(const QPen &);
    QPen cursorLabelPen() const;

    virtual void setCursorLabelFont(const QFont &);
    QFont cursorLabelFont() const;

    bool isEnabled() const;
    virtual void setEnabled(bool);

    bool isActive() const;

    virtual bool eventFilter(QObject *, QEvent *);

    QWidget *parentWidget();
    const QWidget *parentWidget() const;

    virtual QRect pickRect() const;
    const QPointArray &selection() const; 

    virtual bool event(QEvent *);

signals:
    /*!
      A signal emitting the selected points, 
      at the end of a selection.

      \param pa Selected points
    */
    void selected(const QPointArray &pa);

    /*!
      A signal emitted when a point has been appended to the selection

      \param pos Position of the appended point.
      \sa append(). moved()
    */
    void appended(const QPoint &pos);

    /*!
      A signal emitted whenever the last appended point of the 
      selection has been moved.

      \param pos Position of the moved last point of the selection.
      \sa move(), appended()
    */
    void moved(const QPoint &pos);

    /*!
      A signal emitted when the active selection has been changed.
      This might happen when the observed widget is resized.

      \param pa Changed selection
      \sa stretchSelection()
    */
    void changed(const QPointArray &pa);

protected:
    /*!
      \brief Validate and fixup the selection

      Accepts all selections unmodified
    
      \param selection Selection to validate and fixup
      \return TRUE, when accepted, FALSE otherwise
    */
    virtual bool accept(QPointArray &selection) const;

    virtual void transition(const QEvent *);

    virtual void begin();
    virtual void append(const QPoint &);
    virtual void move(const QPoint &);
    virtual bool end(bool ok = TRUE);

    virtual void widgetMousePressEvent(QMouseEvent *);
    virtual void widgetMouseReleaseEvent(QMouseEvent *);
    virtual void widgetMouseDoubleClickEvent(QMouseEvent *); 
    virtual void widgetMouseMoveEvent(QMouseEvent *); 
    virtual void widgetWheelEvent(QWheelEvent *);
    virtual void widgetKeyPressEvent(QKeyEvent *); 
    virtual void widgetKeyReleaseEvent(QKeyEvent *); 

    virtual void drawRubberBand(const QRect &clipRect = QRect()) const;
    virtual void drawRubberBand(QPainter *, 
        const QRect &rect, const QPointArray &) const;

    virtual void drawCursorLabel(const QRect &clipRect = QRect()) const;
    virtual void drawCursorLabel(QPainter *, const QRect &rect, 
        const QPoint &, const QPointArray &) const;

    virtual void stretchSelection(const QSize &oldSize, 
        const QSize &newSize);

    virtual QString cursorLabel(const QPoint &pos) const;

    virtual QwtPickerMachine *stateMachine(int) const;

    void repaint(const QRect &rect = QRect());

private:
    void init(QWidget *, int selectionFlags, RubberBand rubberBand,
        DisplayMode cursorLabelMode);

    void setStateMachine(QwtPickerMachine *);
    void setMouseTracking(bool);

    bool d_enabled;

    QwtPickerMachine *d_stateMachine;

    int d_selectionFlags;
    ResizeMode d_resizeMode;

    RubberBand d_rubberBand;
    QPen d_rubberBandPen;

    DisplayMode d_cursorLabelMode;
    QPen d_cursorLabelPen;
    QFont d_cursorLabelFont;

    QPointArray d_selection;
    bool d_isActive;
    QPoint d_labelPosition;

    bool d_mouseTracking; // used to save previous value
};
            
#endif
