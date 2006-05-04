/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

// vim: expandtab

#include <qlayout.h>
#include <qlineedit.h>
#include <qevent.h>
#include "qwt_counter.h"
#include "qwt_arrbtn.h"

/*!
  The default number of buttons is set to 2. The default increments are:
  \li Button 1: 1 step
  \li Button 2: 10 steps
  \li Button 3: 100 steps

  \param parent
  \param name Forwarded to QWidget's ctor.
 */
QwtCounter::QwtCounter(QWidget *parent, const char *name ):
    QWidget(parent,name), 
    d_blockKeys(FALSE),
    d_keyPressed(FALSE)
{
    d_increment[Button1] = 1;
    d_increment[Button2] = 10;
    d_increment[Button3] = 100;

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setAutoAdd(TRUE);

    int i;
    for(i = ButtonCnt - 1; i >= 0; i--)
    {
        QwtArrowButton *btn =
            new QwtArrowButton(i+1, Qt::DownArrow,this);
        btn->setFocusPolicy(QWidget::StrongFocus);
        btn->installEventFilter(this);

        connect(btn, SIGNAL(released()), SLOT(btnReleased()));
        connect(btn, SIGNAL(clicked()), SLOT(btnClicked()));

        d_buttonDown[i] = btn;
    }

    d_valueEdit = new QLineEdit(this);
    d_valueEdit->setReadOnly(TRUE);
    d_valueEdit->setFocusPolicy(QWidget::NoFocus);

    layout->setStretchFactor(d_valueEdit, 10);

    for(i = 0; i < ButtonCnt; i++)
    {
        QwtArrowButton *btn =
            new QwtArrowButton(i+1, Qt::UpArrow, this);
        btn->setFocusPolicy(QWidget::StrongFocus);
        btn->installEventFilter(this);

        connect(btn, SIGNAL(released()), SLOT(btnReleased()));
        connect(btn, SIGNAL(clicked()), SLOT(btnClicked()));
    
        d_buttonUp[i] = btn;
    }

    setNumButtons(2);
    setRange(0.0,1.0,0.001);
    setValue(0.0);
}

//! Keep track of key press and release events
bool QwtCounter::eventFilter(QObject *object, QEvent *e)
{
    if ( object->inherits("QPushButton") )
    {
        if ( e->type() == QEvent::KeyPress )
        {
            if ( !((QKeyEvent *)e)->isAutoRepeat() )
                d_keyPressed = TRUE;
        }

        if ( e->type() == QEvent::KeyRelease )
        {
            if ( !((QKeyEvent *)e)->isAutoRepeat() )
            {
                d_keyPressed = FALSE;

                // Unblock key events. They might be blocked
                // to interrupt auto repeat, when we changed
                // the focus to a different button.

                d_blockKeys = FALSE;
            }
        }
    }

    return QWidget::eventFilter(object, e);
}

/*!
  Specify the number of steps by which the value
  is incremented or decremented when a specified button
  is pushed.

  \param btn One of \c QwtCounter::Button1, \c QwtCounter::Button2,
             \c QwtCounter::Button3
  \param nSteps Number of steps
*/
void QwtCounter::setIncSteps(QwtCounter::Button btn, int nSteps)
{
    if (( btn >= 0) && (btn < ButtonCnt))
       d_increment[btn] = nSteps;
}

/*!
  \return the number of steps by which a specified button increments the value
  or 0 if the button is invalid.
  \param btn One of \c QwtCounter::Button1, \c QwtCounter::Button2,
  \c QwtCounter::Button3
*/
int QwtCounter::incSteps(QwtCounter::Button btn) const
{
    if (( btn >= 0) && (btn < ButtonCnt))
       return d_increment[btn];

    return 0;
}

/*!
  \brief Set a new value
  \param v new value
  Calls QwtDblRange::setValue and does all visual updates.
  \sa QwtDblRange::setValue
*/

void QwtCounter::setValue(double v)
{
    QwtDblRange::setValue(v);

    showNum(value());
    updateButtons();
}

/*!
  \brief Notify a change of value
*/
void QwtCounter::valueChange()
{
    if ( isValid() )
        showNum(value());
    else
        d_valueEdit->setText(QString::null);

    updateButtons();

    if ( isValid() )
        emit valueChanged(value());
}

/*!
  \brief Update buttons according to the current value

  When the QwtCounter under- or over-flows, the focus is set to the smallest
  up- or down-button and counting is disabled.

  Counting is re-enabled on a button release event (mouse or space bar).
*/
void QwtCounter::updateButtons()
{
    if ( isValid() )
    {
        // 1. save enabled state of the smallest down- and up-button
        // 2. change enabled state on under- or over-flow
        // 3. switch focus if the enabled state has changed

        int oldEnabledDown = d_buttonDown[0]->isEnabled();
        int oldEnabledUp = d_buttonUp[0]->isEnabled();

        for ( int i = 0; i < ButtonCnt; i++ )
        {
            d_buttonDown[i]->setEnabled(value() > minValue());
            d_buttonUp[i]->setEnabled(value() < maxValue());
        }

        QPushButton *focusButton = NULL;
        if (oldEnabledDown && !d_buttonDown[0]->isEnabled())
            focusButton = d_buttonUp[0];

        if (oldEnabledUp && !d_buttonUp[0]->isEnabled())
            focusButton = d_buttonDown[0];

        if ( focusButton )
        {
            focusButton->setFocus();
            if ( d_keyPressed )
            {
                // Stop auto repeat until the key has been released 
                d_blockKeys = TRUE;
            }
        }
    }
    else
    {
        for ( int i = 0; i < ButtonCnt; i++ )
        {
            d_buttonDown[i]->setEnabled(FALSE);
            d_buttonUp[i]->setEnabled(FALSE);
        }
    }
}

/*!
  \brief Specify the number of buttons on each side of the label
  \param n Number of buttons
*/
void QwtCounter::setNumButtons(int n)
{
    if ( n<0 || n>ButtonCnt )
        return;

    for ( int i = 0; i < ButtonCnt; i++ )
    {
        if ( i < n )
        {
            d_buttonDown[i]->show();
            d_buttonUp[i]->show();
        }
        else
        {
            d_buttonDown[i]->hide();
            d_buttonUp[i]->hide();
        }
    }

    d_nButtons = n;
}

/*!
    \return The number of buttons on each side of the widget.
*/
int QwtCounter::numButtons() const 
{ 
    return d_nButtons; 
}

//!  Display number string
void QwtCounter::showNum(double d)
{
    QString v;
    v.setNum(d);

    d_valueEdit->setText(v);
    d_valueEdit->setCursorPosition(0);
}

//!  Button clicked
void QwtCounter::btnClicked()
{
    if (d_blockKeys)
    {
        // When we set the focus to a different button,
        // auto repeat has been stopped until the key will
        // be released

        return;
    }

    for ( int i = 0; i < ButtonCnt; i++ )
    {
        if ( d_buttonUp[i] == sender() )
            incValue(d_increment[i]);

        if ( d_buttonDown[i] == sender() )
            incValue(-d_increment[i]);
    }
}

//!  Button released
void QwtCounter::btnReleased()
{
    emit buttonReleased(value());
}

/*!
  \brief Notify change of range

  This function updates the enabled property of
  all buttons contained in QwtCounter.
*/
void QwtCounter::rangeChange()
{
    updateButtons();
}

/*!
  \brief Notify change of font

  This function updates the fonts of all widgets
  contained in QwtCounter.
  \param f new font
*/
void QwtCounter::fontChange(const QFont &f)
{
    QWidget::fontChange( f );
    d_valueEdit->setFont(font());
}

//! A size hint
QSize QwtCounter::sizeHint() const
{
    QString tmp;
    QFontMetrics fm(d_valueEdit->font());

    int w = fm.width(tmp.setNum(minValue()));
    int w1 = fm.width(tmp.setNum(maxValue()));
    if ( w1 > w )
        w = w1;
    w1 = fm.width(tmp.setNum(minValue() + step()));
    if ( w1 > w )
        w = w1;
    w1 = fm.width(tmp.setNum(maxValue() - step()));
    if ( w1 > w )
        w = w1;

    // QLineEdit::minimumSizeHint is for one char. Subtracting
    // the size for the char we get all the margins, frames ...

#if QT_VERSION < 300
    w += d_valueEdit->minimumSizeHint().width() - fm.maxWidth();
#else
    w += 2 * d_valueEdit->frameWidth() + 
        d_valueEdit->fontMetrics().minRightBearing() + 3;
#endif

    // Now we replace default sizeHint contribution of d_valueEdit by
    // what we really need.

    w += QWidget::sizeHint().width() - d_valueEdit->sizeHint().width();

    return QSize(w, QWidget::sizeHint().height());
}

//! Preferred/Fixed
QSizePolicy QwtCounter::sizePolicy() const
{
    QSizePolicy sp;
    sp.setHorData( QSizePolicy::Preferred );
    sp.setVerData( QSizePolicy::Fixed );
    return sp;
}
