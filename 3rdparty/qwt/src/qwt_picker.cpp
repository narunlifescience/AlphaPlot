/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#include <qapplication.h>
#include <qevent.h>
#include <qpainter.h>
#include <qframe.h>
#include <qcursor.h>
#include "qwt_text.h"
#include "qwt_painter.h"
#include "qwt_picker_machine.h"
#include "qwt_picker.h"

/*!
  Constructor

  Creates an picker that is enabled, but where selection flags
  s set to NoSelection, rubberband and cursor label are disabled.
  
  \param parent Parent widget, that will be observed
  \param name Object name
 */

QwtPicker::QwtPicker(QWidget *parent, const char *name):
    QObject(parent, name)
{
    init(parent, NoSelection, NoRubberBand, AlwaysOff);
}

/*!
  Constructor

  \param selectionFlags Or´d value of SelectionType, RectSelectionType and 
                        SelectionMode
  \param rubberBand Rubberband style
  \param cursorLabelMode Cursor label mode
  \param parent Parent widget, that will be observed
  \param name Object name
 */
QwtPicker::QwtPicker(int selectionFlags, RubberBand rubberBand,
        DisplayMode cursorLabelMode, QWidget *parent, const char *name):
    QObject(parent, name)
{
    init(parent, selectionFlags, rubberBand, cursorLabelMode);
}

//! Destructor
QwtPicker::~QwtPicker()
{
    setMouseTracking(FALSE);
    delete d_stateMachine;
}

//! Init the picker, used by the constructors
void QwtPicker::init(QWidget *parent, int selectionFlags, 
    RubberBand rubberBand, DisplayMode cursorLabelMode)
{
    d_rubberBand = rubberBand;
    d_enabled = FALSE;
    d_resizeMode = Stretch;
    d_cursorLabelMode = AlwaysOff;
    d_isActive = FALSE;
    d_labelPosition = QPoint(-1, -1);
    d_mouseTracking = FALSE;

    d_stateMachine = NULL;
    setSelectionFlags(selectionFlags);

    if ( parent )
    {
        if ( parent->focusPolicy() == QWidget::NoFocus )
            parent->setFocusPolicy(QWidget::WheelFocus);

        d_cursorLabelFont = parent->font();
        d_mouseTracking = parent->hasMouseTracking();
        setEnabled(TRUE);
    }
    setCursorLabelMode(cursorLabelMode);
}

/*!
   Set a state machine and delete the previous one
*/
void QwtPicker::setStateMachine(QwtPickerMachine *stateMachine)
{
    if ( d_stateMachine != stateMachine )
    {
        if ( isActive() )
            end(FALSE);

        delete d_stateMachine;
        d_stateMachine = stateMachine;

        if ( d_stateMachine )
            d_stateMachine->reset();
    }
}

/*!
   Create a state machine depending on the selection flags.

   - PointSelection | ClickSelection\n
     QwtPickerClickPointMachine()
   - PointSelection | DragSelection\n
     QwtPickerDragPointMachine()
   - RectSelection | ClickSelection\n
     QwtPickerClickRectMachine()
   - RectSelection | DragSelection\n
     QwtPickerDragRectMachine()
   - PolygonSelection\n
     QwtPickerPolygonMachine()

   \sa setSelectionFlags()
*/
QwtPickerMachine *QwtPicker::stateMachine(int flags) const
{
    if ( flags & PointSelection )
    {
        if ( flags & ClickSelection )
            return new QwtPickerClickPointMachine;
        else
            return new QwtPickerDragPointMachine;
    }
    if ( flags & RectSelection )
    {
        if ( flags & ClickSelection )
            return new QwtPickerClickRectMachine;
        else
            return new QwtPickerDragRectMachine;
    }
    if ( flags & PolygonSelection )
    {
        return new QwtPickerPolygonMachine();
    }
    return NULL;
}

//! Return the parent widget, where the selection happens
QWidget *QwtPicker::parentWidget()
{
    QObject *obj = parent();
    if ( obj && obj->isWidgetType() )
        return (QWidget *)obj;

    return NULL;
}

//! Return the parent widget, where the selection happens
const QWidget *QwtPicker::parentWidget() const
{
    QObject *obj = parent();
    if ( obj && obj->isWidgetType() )
        return (QWidget *)obj;

    return NULL;
}

/*!
  Set the selection flags

  \param flags Or´d value of SelectionType, RectSelectionType and 
               SelectionMode. The default value is NoSelection.

  \sa selectionFlags(), SelectionType, RectSelectionType, SelectionMode
*/

void QwtPicker::setSelectionFlags(int flags)
{
    d_selectionFlags = flags;
    setStateMachine(stateMachine(flags));
}

/*!
  \return Selection flags, an Or´d value of SelectionType, RectSelectionType and
          SelectionMode.
  \sa setSelectionFlags(), SelectionType, RectSelectionType, SelectionMode
*/
int QwtPicker::selectionFlags() const
{
    return d_selectionFlags;
}

/*!
  Set the rubberband style 

  \param rubberBand Rubberband style
         The default value is NoRubberBand.

  \sa rubberBand(), RubberBand, setRubberBandPen()
*/
void QwtPicker::setRubberBand(RubberBand rubberBand)
{
    d_rubberBand = rubberBand;
}

/*!
  \return Rubberband style
  \sa setRubberBand(), RubberBand, rubberBandPen()
*/
QwtPicker::RubberBand QwtPicker::rubberBand() const
{
    return d_rubberBand;
}

/*!
  \brief Set the display mode of the cursor label.

  A cursor label dispays the current position of
  the cursor as a string. The display mode controls
  if the label has to be displayed whenever the observed
  widget has focus and cursor (AlwaysOn), never (AlwaysOff), or
  only when the selection is active (ActiveOnly).
  
  \param mode Cursor label display mode

  \warning In case of AlwaysOn, mouseTracking will be enabled
           for the observed widget.
  \sa cursorLabelMode(), DisplayMode
*/

void QwtPicker::setCursorLabelMode(DisplayMode mode)
{   
    if ( d_cursorLabelMode != mode )
    {
        d_cursorLabelMode = mode;
        setMouseTracking(d_cursorLabelMode == AlwaysOn);
    }
}   

/*!
  \return Cursor label display mode
  \sa setCursorLabelMode(), DisplayMode
*/
QwtPicker::DisplayMode QwtPicker::cursorLabelMode() const
{   
    return d_cursorLabelMode;
}   

/*!
  \brief Set the resize mode.

  The resize mode controls what to do with the selected points of an active
  selection when the observed widget is resized.

  Stretch means the points are scaled according to the new
  size, KeepSize means the points remain unchanged.

  The default mode is Stretch.

  \param mode Resize mode
  \sa resizeMode(), ResizeMode
*/
void QwtPicker::setResizeMode(ResizeMode mode)
{
    d_resizeMode = mode;
}   

/*!
  \return Resize mode
  \sa setResizeMode(), ResizeMode
*/

QwtPicker::ResizeMode QwtPicker::resizeMode() const
{   
    return d_resizeMode;
}

/*!
  \brief En/disable the picker

  When enabled is TRUE an event filter is installed for
  the observed widget, otherwise the event filter is removed.

  \param enabled TRUE or FALSE
  \sa isEnabled(), eventFilter()
*/
void QwtPicker::setEnabled(bool enabled)
{
    if ( d_enabled != enabled )
    {
        QWidget *w = parentWidget();
        if ( !w )
            return;

        d_enabled = enabled;
        drawCursorLabel();

        if ( d_enabled )
            w->installEventFilter(this);
        else
            w->removeEventFilter(this);
    }
}

/*!
  \return TRUE when enabled, FALSE otherwise
  \sa setEnabled, eventFilter()
*/

bool QwtPicker::isEnabled() const
{
    return d_enabled;
}

/*!
  Set the font for the cursor label

  \param font Cursor label font
  \sa cursorLabelFont(), setCursorLabelMode(), setCursorLabelPen()
*/
void QwtPicker::setCursorLabelFont(const QFont &font)
{
    if ( font != d_cursorLabelFont )
    {
        if ( isEnabled() )
            drawCursorLabel(); // erase

        d_cursorLabelFont = font;

        if ( isEnabled() )
            drawCursorLabel(); // repaint
    }
}

/*!
  \return Cursor label font
  \sa setCursorLabelFont(), cursorLabelMode(), cursorLabelPen()
*/

QFont QwtPicker::cursorLabelFont() const
{
    return d_cursorLabelFont;
}

/*!
  Set the pen for the cursor label

  \param pen Cursor label pen
  \sa cursorLabelPen(), setCursorLabelMode(), setCursorLabelFont()
*/
void QwtPicker::setCursorLabelPen(const QPen &pen)
{
    if ( pen != d_cursorLabelPen )
    {
        if ( isEnabled() )
            drawCursorLabel(); // erase

        d_cursorLabelPen = pen;

        if ( isEnabled() )
            drawCursorLabel(); // repaint
    }
}

/*!
  \return Cursor label pen
  \sa setCursorLabelPen(), cursorLabelMode(), cursorLabelFont()
*/
QPen QwtPicker::cursorLabelPen() const
{
    return d_cursorLabelPen;
}

/*!
  Set the pen for the rubberband

  \param pen Rubberband pen
  \sa rubberBandPen(), setRubberBand()
*/
void QwtPicker::setRubberBandPen(const QPen &pen)
{
    if ( pen != d_rubberBandPen )
    {
        drawRubberBand(); // erase
        d_rubberBandPen = pen;
        drawRubberBand(); // repaint
    }
}

/*!
  \return Rubberband pen
  \sa setRubberBandPen(), rubberBand()
*/
QPen QwtPicker::rubberBandPen() const
{
    return d_rubberBandPen;
}

/*!
   \brief Return the label for a position

   In case of HLineRubberBand the label is the value of the
   y position, in case of VLineRubberBand the value of the x position.
   Otherwise the label contains x and y position separated by a ´, ´.

   The format for the string conversion is "%d".

   \param pos Position
   \return Converted position as string
*/

QString QwtPicker::cursorLabel(const QPoint &pos) const
{
    QString label;

    switch(rubberBand())
    {
        case HLineRubberBand:
            label.sprintf("%d", pos.y());
            break;
        case VLineRubberBand:
            label.sprintf("%d", pos.x());
            break;
        default:
            label.sprintf("%d, %d", pos.x(), pos.y());
    }
    return label;
}

/*!
   \brief Draw the rubberband

   The rubberband is painted in Qt::XorROP mode, what means
   the first call paints the second erases the painted rubberband.
   
   \param clipRect Clipping rectangle. If it is now valid the result
                   of pickRect() is used instead.

   \sa pickRect(), QPainter::setRasterOp(), QPainter::setClipRect()
*/
void QwtPicker::drawRubberBand(const QRect &clipRect) const
{
    QWidget *widget = (QWidget *)parentWidget();
    if ( !widget || !isActive() || rubberBand() == NoRubberBand || 
        rubberBandPen().style() == Qt::NoPen )
    {
        return;
    }

    const QColor bg = widget->backgroundColor();

    QPainter painter(widget);
    painter.setClipRect(clipRect.isValid() ? clipRect : pickRect());
    painter.setClipping(TRUE);
    painter.setRasterOp(XorROP);

    QPen pen = d_rubberBandPen;
    pen.setColor(QColor(bg.rgb() ^ pen.color().rgb()));
    painter.setPen(pen);

    drawRubberBand(&painter, pickRect(), d_selection);
}

/*!
   Draw a rubberband , depending on rubberBand() and selectionFlags()

   \param painter Painter, initialized with clip rect, 
                  in Qt::XorROP mode of rubberband pen and background color
                  of the observed widget.
   \param pickRect Rectangle where selection can happen.
   \param pa Current selection

   \sa rubberBand(), RubberBand, selectionFlags()
*/

void QwtPicker::drawRubberBand(QPainter *painter,
    const QRect &pickRect, const QPointArray &pa) const
{
    if ( rubberBand() == NoRubberBand )
        return;

    if ( selectionFlags() & PointSelection )
    {
        if ( pa.count() < 1 )
            return;

        const QPoint pos = pa[0];

        switch(rubberBand())
        {
            case VLineRubberBand:
                QwtPainter::drawLine(painter, pos.x(),
                    pickRect.top(), pos.x(), pickRect.bottom());
                break;

            case HLineRubberBand:
                QwtPainter::drawLine(painter, pickRect.left(), 
                    pos.y(), pickRect.right(), pos.y());
                break;

            case CrossRubberBand:
                QwtPainter::drawLine(painter, pos.x(),
                    pickRect.top(), pos.x(), pickRect.bottom());
                QwtPainter::drawLine(painter, pickRect.left(), 
                    pos.y(), pickRect.right(), pos.y());
                break;
            default:
                break;
        }
    }

    else if ( selectionFlags() & RectSelection )
    {
        if ( pa.count() < 2 )
            return;

        QPoint p1 = pa[0];
        QPoint p2 = pa[int(pa.count() - 1)];

        if ( selectionFlags() & CenterToCorner )
        {
            p1.setX(p1.x() - (p2.x() - p1.x()));
            p1.setY(p1.y() - (p2.y() - p1.y()));
        }
        else if ( selectionFlags() & CenterToRadius )
        {
            const int radius = QMAX(QABS(p2.x() - p1.x()), 
                QABS(p2.y() - p1.y()));
            p2.setX(p1.x() + radius);
            p2.setY(p1.y() + radius);
            p1.setX(p1.x() - radius);
            p1.setY(p1.y() - radius);
        }

        const QRect rect = QRect(p1, p2).normalize();
        switch(rubberBand())
        {
            case EllipseRubberBand:
                QwtPainter::drawEllipse(painter, rect);
                break;

            case RectRubberBand:
            {
                // In case of height == 0 or width == 0 the
                // 2 edges erase each other in XOR mode.

                if ( rect.height() <= 1 )
                {
                    QwtPainter::drawLine(painter, 
                        rect.topLeft(), rect.topRight());
                }
                else if ( rect.width() <= 1 )
                {
                    QwtPainter::drawLine(painter, 
                        rect.topLeft(), rect.bottomLeft());
                }
                else
                    QwtPainter::drawRect(painter, rect);
                break;
            }
            default:
                break;
        }
    }
    else if ( selectionFlags() & PolygonSelection )
    {
        if ( rubberBand() == PolygonRubberBand )
            painter->drawPolyline(pa);
    }
}

/*!
   \brief Draw the cursor label

   The cursor label is painted in Qt::XorROP mode, what means
   the first call paints the second erases the painted rubberband.

   \param clipRect Clipping rectangle. If it is now valid the result
                   of pickRect() is used instead.

   \sa pickRect(), QPainter::setRasterOp(), QPainter::setClipRect()
*/

void QwtPicker::drawCursorLabel(const QRect &clipRect) const
{
    QWidget *widget = (QWidget *)QwtPicker::parentWidget();
    if ( !widget )
        return;

    if ( cursorLabelMode() == AlwaysOff || 
        (cursorLabelMode() == ActiveOnly && !isActive() ) )
    {
        return;
    }

    if ( d_labelPosition.x() < 0 || d_labelPosition.y() < 0 )
        return;

    const QColor bg = widget->backgroundColor();

    QPainter painter(widget);
    painter.setClipRect(clipRect.isValid() ? clipRect : pickRect());
    painter.setClipping(TRUE);
    painter.setRasterOp(XorROP);

    QPen pen = d_cursorLabelPen;
    pen.setColor(QColor((bg.rgb() ^ pen.color().rgb())));

    painter.setPen(pen);
    painter.setFont(d_cursorLabelFont);

    drawCursorLabel(&painter, pickRect(), d_labelPosition, d_selection);
}

/*!
   \brief Draw a cursor label.

   The default position of the label is top-right of the cursor.
   In case of a rubberband the label is placed not to conflict with
   the rubberband. 

   \param painter Painter, initialized with clip rect, 
                  in Qt::XorROP mode of cursor label pen and background color
                  of the observed widget.
   \param pickRect Rectangle where selections can happen.
   \param pos Cursor position
   \param pa Current selection

   \sa rubberBand(), RubberBand, selectionFlags()
*/
void QwtPicker::drawCursorLabel(QPainter *painter, const QRect &pickRect,
        const QPoint &pos, const QPointArray &pa) const
{
    int alignment = 0;
    if ( isActive() && pa.count() > 1 && rubberBand() != NoRubberBand )
    {
        const QPoint last = pa[int(pa.count()) - 2];

        alignment |= (pos.x() >= last.x()) ? Qt::AlignRight : Qt::AlignLeft;
        alignment |= (pos.y() > last.y()) ? Qt::AlignBottom : Qt::AlignTop;
    }
    else
        alignment = Qt::AlignTop | Qt::AlignRight;

    QString label = cursorLabel(pos);
    if ( !label.isEmpty() )
    {
        QwtText *text = QwtText::makeText(label, 0, painter->font(),
            painter->pen().color());

        QRect textRect = text->boundingRect(painter);

        const int margin = 5;

        int x = pos.x();
        if ( alignment & Qt::AlignLeft )
            x -= textRect.width() + margin;
        else if ( alignment & Qt::AlignRight )
            x += margin;

        int y = pos.y();
        if ( alignment & Qt::AlignBottom )
            y += margin;
        else if ( alignment & Qt::AlignTop )
            y -= textRect.height() + margin;
        
        textRect.moveTopLeft(QPoint(x, y));

        int right = QMIN(textRect.right(), pickRect.right() - margin);
        int bottom = QMIN(textRect.bottom(), pickRect.bottom() - margin);
        textRect.moveBottomRight(QPoint(right, bottom));

        int left = QMAX(textRect.left(), pickRect.left() + margin);
        int top = QMAX(textRect.top(), pickRect.top() + margin);
        textRect.moveTopLeft(QPoint(left, top));

        text->draw(painter, textRect);

        delete text;
    }
}

/*!
  \brief Repaint cursor label and rubberband on top of the widget

  Send a QEvent::Paint event to the picker

  \param rect Rectangle which has to be repainted. In case of 
              !rect.isValid() a complete repaint ill be initiated.
*/
void QwtPicker::repaint(const QRect &rect)
{
    QApplication::postEvent(this, new QPaintEvent(rect));
}

/*!
  \brief Event Handler

  Paint events for the observed widgets require to repaint rubberband
  an cursor label too. To delay them until the widget is repainted
  the event filter sends a QEvent::Paint to the picker, that is handled
  here.

  \sa repaint, eventFilter(), drawRubberBand(), drawCursorLabel()
*/
bool QwtPicker::event(QEvent *e)
{
    if ( e->type() == QEvent::Paint )
    {
        const QRect clipRect = ((const QPaintEvent *)e)->rect();

        drawRubberBand(clipRect);
        drawCursorLabel(clipRect);
        return TRUE;
    }

    return QObject::event(e);
}

/*!
  \brief Event filter

  When isEnabled() == TRUE all events of the observed widget are filtered.
  Mouse and keyboard events are translated into widgetMouse- and widgetKey-
  and widgetWheel-events. Paint and Resize events are handled to keep 
  rubberband and cursor label up to date.

  \sa event(), widgetMousePressEvent(), widgetMouseReleaseEvent(),
      widgetMouseDoubleClickEvent(), widgetMouseMoveEvent(),
      widgetWheelEvent(), widgetKeyPressEvent(), widgetKeyReleaseEvent()
*/
bool QwtPicker::eventFilter(QObject *o, QEvent *e)
{
    /*
      Because of the XorROP mode the delayed paint has to
      be completed to have a stable state for processing other
      events. Unfortunately the internally posted paint event
      might be too late in event queue. So we process it first.
     */

    QApplication::sendPostedEvents(this, QEvent::Paint);

    if ( o && o == parentWidget() )
    {
        switch(e->type())
        {
            case QEvent::Paint:
            {
                // We have to wait until the widget is updated
                // before we can repaint cursor label and
                // rubberband. So we post an event to delay the repaint.

                const QPaintEvent *re = (QPaintEvent *)e;
                repaint(re->rect());
                break;
            }
            case QEvent::Resize:
            {
                if ( d_resizeMode == Stretch )
                {
                    // erase
                    drawRubberBand();
                    drawCursorLabel();

                    const QResizeEvent *re = (QResizeEvent *)e;
                    stretchSelection(re->oldSize(), re->size());

                    // repaint
                    drawRubberBand();
                    drawCursorLabel(); 
                }
                break;
            }
            case QEvent::MouseButtonPress:
                widgetMousePressEvent((QMouseEvent *)e);
                break;
            case QEvent::MouseButtonRelease:
                widgetMouseReleaseEvent((QMouseEvent *)e);
                break;
            case QEvent::MouseButtonDblClick:
                widgetMouseDoubleClickEvent((QMouseEvent *)e);
                break;
            case QEvent::MouseMove:
                widgetMouseMoveEvent((QMouseEvent *)e);
                break;
            case QEvent::KeyPress:
                widgetKeyPressEvent((QKeyEvent *)e);
                break;
            case QEvent::KeyRelease:
                widgetKeyReleaseEvent((QKeyEvent *)e);
                break;
            case QEvent::Wheel:
                widgetWheelEvent((QWheelEvent *)e);
                break;
            default:
                break;
        }
    }
    return FALSE;
}

/*!
  Handle a mouse press event for the observed widget.

  Begin and/or end a selection depending on the selection flags.

  \sa QwtPicker, selectionFlags()
  \sa eventFilter(), widgetMouseReleaseEvent(),
      widgetMouseDoubleClickEvent(), widgetMouseMoveEvent(),
      widgetWheelEvent(), widgetKeyPressEvent(), widgetKeyReleaseEvent()
*/
void QwtPicker::widgetMousePressEvent(QMouseEvent *e)
{
    transition(e);
}

/*!
  Handle a mouse move event for the observed widget.

  Move the last point of the selection in case of isActive() == TRUE

  \sa eventFilter(), widgetMousePressEvent(), widgetMouseReleaseEvent(),
      widgetMouseDoubleClickEvent(),
      widgetWheelEvent(), widgetKeyPressEvent(), widgetKeyReleaseEvent()
*/
void QwtPicker::widgetMouseMoveEvent(QMouseEvent *e)
{
    drawCursorLabel(); // erase

    if ( pickRect().contains(e->pos()) )
    {
        d_labelPosition = e->pos();
        drawCursorLabel(); // Paint
    }
    else
        d_labelPosition = QPoint(-1, -1);

    transition(e);
}

/*!
  Handle a mouse relase event for the observed widget.

  End a selection depending on the selection flags.

  \sa QwtPicker, selectionFlags()
  \sa eventFilter(), widgetMousePressEvent(), 
      widgetMouseDoubleClickEvent(), widgetMouseMoveEvent(),
      widgetWheelEvent(), widgetKeyPressEvent(), widgetKeyReleaseEvent()
*/
void QwtPicker::widgetMouseReleaseEvent(QMouseEvent *e)
{
    transition(e);
}

/*!
  Handle mouse double click event for the observed widget.

  Empty implementation, does nothing.

  \sa eventFilter(), widgetMousePressEvent(), widgetMouseReleaseEvent(),
      widgetMouseMoveEvent(),
      widgetWheelEvent(), widgetKeyPressEvent(), widgetKeyReleaseEvent()
*/
void QwtPicker::widgetMouseDoubleClickEvent(QMouseEvent *me)
{
    transition(me);
}
    

/*!
  Handle a wheel event for the observed widget.

  Move the last point of the selection in case of isActive() == TRUE

  \sa eventFilter(), widgetMousePressEvent(), widgetMouseReleaseEvent(),
      widgetMouseDoubleClickEvent(), widgetMouseMoveEvent(),
      widgetKeyPressEvent(), widgetKeyReleaseEvent()
*/
void QwtPicker::widgetWheelEvent(QWheelEvent *e)
{
    drawCursorLabel(); // erase

    if ( pickRect().contains(e->pos()) )
    {
        d_labelPosition = e->pos();
        drawCursorLabel(); // Paint
    }
    else
        d_labelPosition = QPoint(-1, -1);

    transition(e);
}

/*!
  Handle a key press event for the observed widget.

  Selections can be completely done by the keyboard. The arrow keys
  move the cursor, the abort key aborts a selection. All other keys
  are handled by the current state machine.

  \sa QwtPicker, selectionFlags()
  \sa eventFilter(), widgetMousePressEvent(), widgetMouseReleaseEvent(),
      widgetMouseDoubleClickEvent(), widgetMouseMoveEvent(),
      widgetWheelEvent(), widgetKeyReleaseEvent(), stateMachine(),
      QwtEventPattern::KeyPatternCode
*/
void QwtPicker::widgetKeyPressEvent(QKeyEvent *ke)
{
    int dx = 0;
    int dy = 0;

    int offset = 1;
    if ( ke->isAutoRepeat() )
        offset = 5;

    if ( keyMatch(KeyLeft, ke) )
        dx = -offset;
    else if ( keyMatch(KeyRight, ke) )
        dx = offset;
    else if ( keyMatch(KeyUp, ke) )
        dy = -offset;
    else if ( keyMatch(KeyDown, ke) )
        dy = offset;
    else if ( keyMatch(KeyAbort, ke) )
    {
        if ( d_stateMachine )
            d_stateMachine->reset();

        if (isActive())
            end(FALSE);
    }
    else
        transition(ke);

    if ( dx != 0 || dy != 0 )
    {
        const QRect rect = pickRect();
        const QPoint pos = parentWidget()->mapFromGlobal(QCursor::pos());

        int x = pos.x() + dx;
        x = QMAX(rect.left(), x);
        x = QMIN(rect.right(), x);

        int y = pos.y() + dy;
        y = QMAX(rect.top(), y);
        y = QMIN(rect.bottom(), y);

        QCursor::setPos(parentWidget()->mapToGlobal(QPoint(x, y)));
    }
}
 
/*!
  Handle a key release event for the observed widget.

  Passes the event to the state machine.

  \sa eventFilter(), widgetMousePressEvent(), widgetMouseReleaseEvent(),
      widgetMouseDoubleClickEvent(), widgetMouseMoveEvent(),
      widgetWheelEvent(), widgetKeyPressEvent(), stateMachine()
*/
void QwtPicker::widgetKeyReleaseEvent(QKeyEvent *ke)
{
    transition(ke);
}

/*!
  Passes an event to the state machine and executes the resulting 
  commands. Append and Move commands use the current position
  of the cursor (QCursor::pos()).

  \param e Event
*/
void QwtPicker::transition(const QEvent *e)
{
    if ( !d_stateMachine )
        return;

    QValueList<QwtPickerMachine::Command> commandList =
        d_stateMachine->transition(*this, e);

    const QPoint pos = parentWidget()->mapFromGlobal(QCursor::pos());

    for ( uint i = 0; i < commandList.count(); i++ )
    {
        switch(commandList[i])
        {
            case QwtPickerMachine::Begin:
            {
                begin();
                break;
            }
            case QwtPickerMachine::Append:
            {
                append(pos);
                break;
            }
            case QwtPickerMachine::Move:
            {
                move(pos);
                break;
            }
            case QwtPickerMachine::End:
            {
                end();
                break;
            }
        }
    }
}

/*!
  Open a selection setting the state to active

  \sa isActive, end(), append(), move()
*/
void QwtPicker::begin()
{
    drawCursorLabel(); // erase

    d_selection.resize(0);
    d_isActive = TRUE;

    if ( cursorLabelMode() != AlwaysOff )
    {
        if ( d_labelPosition.x() < 0 || d_labelPosition.y() < 0 ) 
        {
            QWidget *w = parentWidget();
            if ( w )
                d_labelPosition = w->mapFromGlobal(QCursor::pos());
        }
    }

    drawCursorLabel(); // repaint
    setMouseTracking(TRUE);
}

/*!
  \brief Close a selection setting the state to inactive.

  The selection is validated and maybe fixed by QwtPicker::accept().

  \param ok If TRUE, complete the selection and emit a selected signal
            otherwise discard the selection.
  \return TRUE if the selection is accepted, FALSE otherwise
  \sa isActive, begin(), append(), move(), selected(), accept()
*/
bool QwtPicker::end(bool ok)
{
    if ( d_isActive )
    {
        setMouseTracking(FALSE);

        drawCursorLabel(); // erase
        drawRubberBand(); // erase

        d_isActive = FALSE;

        drawCursorLabel(); // repaint

        if ( cursorLabelMode() == ActiveOnly )
            d_labelPosition = QPoint(-1, -1);

        if ( ok )
            ok = accept(d_selection);

        if ( ok )
            emit selected(d_selection);
        else
            d_selection.resize(0);
    }
    else
        ok = FALSE;

    return ok;
}

/*!
  Append a point to the selection and update rubberband and cursor label.
  The appended() signal is emitted.

  \param pos Additional point

  \sa isActive, begin(), end(), move(), appended()
*/
void QwtPicker::append(const QPoint &pos)
{
    if ( d_isActive )
    {
        drawRubberBand(); // erase
        drawCursorLabel(); // erase

        const int idx = d_selection.count();
        d_selection.resize(idx + 1);
        d_selection[idx] = pos;

        drawRubberBand(); // repaint
        drawCursorLabel(); // repaint

        emit appended(pos);
    }
}

/*!
  Move the last point of the selection
  The moved() signal is emitted.

  \param pos New position
  \sa isActive, begin(), end(), append()

*/
void QwtPicker::move(const QPoint &pos)
{
    if ( d_isActive )
    {
        const int idx = d_selection.count() - 1;
        if ( idx >= 0 )
        {
            drawRubberBand(); // erase
            d_selection[idx] = pos;
            drawRubberBand(); // repaint

            emit moved(pos);
        }
    }
}

bool QwtPicker::accept(QPointArray &) const
{
    return TRUE;
}

/*!
  A picker is active between begin() and end().
  \return TRUE if the selection is active.
*/
bool QwtPicker::isActive() const 
{
    return d_isActive;
}

//!  Return Selected points
const QPointArray &QwtPicker::selection() const
{
    return d_selection;
}

/*!
  Scale the selection by the ratios of oldSize and newSize
  The changed() signal is emitted.

  \param oldSize Previous size
  \param newSize Current size

  \sa ResizeMode, setResizeMode(), resizeMode()
*/
void QwtPicker::stretchSelection(const QSize &oldSize, const QSize &newSize)
{
    const double xRatio =
        double(newSize.width()) / double(oldSize.width());
    const double yRatio =
        double(newSize.height()) / double(oldSize.height());

    for ( int i = 0; i < int(d_selection.count()); i++ )
    {
        QPoint &p = d_selection[i];
        p.setX(qRound(p.x() * xRatio));
        p.setY(qRound(p.y() * yRatio));

        emit changed(d_selection);
    }
}

/*!
  Set mouse tracking for the observed widget.

  In case of enable is TRUE, the previous value
  is saved, that is restored when enable is FALSE.

  \warning Even when enable is FALSE, mouse tracking might be restored
           to TRUE. When mouseTracking for the observed widget
           has been changed directly by QWidget::setMouseTracking
           while mouse tracking has been set to TRUE, this value can´t
           be restored.
*/

void QwtPicker::setMouseTracking(bool enable)
{
    QWidget *widget = parentWidget();
    if ( !widget )
        return;

    if ( enable )
    {
        d_mouseTracking = widget->hasMouseTracking();
        widget->setMouseTracking(TRUE);
    }
    else
    {
        widget->setMouseTracking(d_mouseTracking);
    }
}

/*!
  Find the area of the observed widget, where selection might happen.

  \return QFrame::contentsRect() if it is a QFrame, QWidget::rect() otherwise.
*/
QRect QwtPicker::pickRect() const
{
    QRect rect;

    const QWidget *widget = parentWidget();
    if ( !widget )
        return rect;

    if ( widget->inherits("QFrame") )
        rect = ((QFrame *)widget)->contentsRect();
    else
        rect = widget->rect();

    return rect;
}

// Local Variables:
// mode: C++
// c-file-style: "stroustrup"
// indent-tabs-mode: nil
// End:
