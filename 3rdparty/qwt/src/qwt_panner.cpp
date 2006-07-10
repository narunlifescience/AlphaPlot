/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

// vim: expandtab

#include <qpainter.h>
#include <qpixmap.h>
#include <qevent.h>
#include <qframe.h>
#include <qcursor.h>
#include "qwt_panner.h"

class QwtPanner::PrivateData
{
public:
    PrivateData():
        isEnabled(false),
        button(Qt::MidButton),
        buttonState(Qt::NoButton)
    {
    }
        
    bool isEnabled;
    int button;
    int buttonState;

    QPoint initialPos;
    QPoint pos;

    QPixmap pixmap;
    QCursor cursor;
};

QwtPanner::QwtPanner(QWidget *parent):
    QWidget(parent)
{
    d_data = new PrivateData();

    if ( parent )
        d_data->cursor = parent->cursor();

#if QT_VERSION >= 0x040000
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_PaintOnScreen);
    setFocusPolicy(Qt::NoFocus);
#else
    setBackgroundMode(Qt::NoBackground);
    setFocusPolicy(QWidget::NoFocus);
#endif
    hide();

    setEnabled(true);
}

QwtPanner::~QwtPanner()
{
    delete d_data;
}

void QwtPanner::setMouseButton(int button, int buttonState)
{
    d_data->button = button;
    d_data->buttonState = buttonState;
}

void QwtPanner::setCursor(const QCursor &cursor)
{
    d_data->cursor = cursor;
}

const QCursor QwtPanner::cursor() const
{
    return d_data->cursor;
}

void QwtPanner::setEnabled(bool on)
{
    if ( d_data->isEnabled != on )
    {
        d_data->isEnabled = on;

        QWidget *w = parentWidget();
        if ( w )
        {
            if ( d_data->isEnabled )
            {
                w->installEventFilter(this);
            }
            else
            {
                w->removeEventFilter(this);
                hide();
            }
        }
    }
}

bool QwtPanner::isEnabled() const
{
    return d_data->isEnabled;
}

void QwtPanner::paintEvent(QPaintEvent *pe)
{
    QPixmap pm(size());

    QPainter painter(&pm);

    const QColor bg =
#if QT_VERSION < 0x040000
        parentWidget()->palette().color(
            QPalette::Normal, QColorGroup::Background);
#else
        parentWidget()->palette().color(
            QPalette::Normal, QPalette::Background);
#endif

    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(bg));
    painter.drawRect(0, 0, pm.width(), pm.height());

    int dx = d_data->pos.x() - d_data->initialPos.x();
    int dy = d_data->pos.y() - d_data->initialPos.y();

    QRect r(0, 0, d_data->pixmap.width(), d_data->pixmap.height());
    r.moveCenter(QPoint(r.center().x() + dx, r.center().y() + dy));

    painter.drawPixmap(r, d_data->pixmap);
    painter.end();

    painter.begin(this);
    painter.setClipRegion(pe->region());
    painter.drawPixmap(0, 0, pm);
}

bool QwtPanner::eventFilter(QObject *o, QEvent *e)
{
    if ( o == NULL || o != parentWidget() )
        return false;

    switch(e->type())
    {
        case QEvent::MouseButtonPress:
        {
            const QMouseEvent *me = (QMouseEvent *)e;
    
            if ( me->button() == d_data->button )
            {
#if QT_VERSION < 0x040000
                if ( (me->state() & Qt::KeyButtonMask) ==
                    (d_data->buttonState & Qt::KeyButtonMask) )
#else
                if ( (me->modifiers() & Qt::KeyboardModifierMask) ==
                    (int)(d_data->buttonState & Qt::KeyboardModifierMask) )
#endif
                {
                    parentWidget()->setCursor(d_data->cursor);

                    d_data->initialPos = d_data->pos = me->pos();

                    QRect cr = parentWidget()->rect();
                    if ( parentWidget()->inherits("QFrame") )
                    {
                        const QFrame* frame = (QFrame*)parentWidget();
                        cr = frame->contentsRect();
                    }
                    setGeometry(cr);
                    d_data->pixmap = QPixmap::grabWidget(parentWidget(),
                        cr.x(), cr.y(), cr.width(), cr.height());
                    show();
                }
            }
            break;
        }
        case QEvent::MouseMove:
        {
            const QMouseEvent *me = (QMouseEvent *)e;
            if ( isVisible() && rect().contains(me->pos()) )
            {
                d_data->pos = me->pos();
                update();
            }
            break;
        }
        case QEvent::MouseButtonRelease:
        {
            const QMouseEvent *me = (QMouseEvent *)e;
            if ( isVisible() )
            {
                hide();
                parentWidget()->unsetCursor();

                d_data->pixmap = QPixmap();
                d_data->pos = me->pos();
                if ( d_data->pos != d_data->initialPos )
                {
                    emit panned(d_data->pos.x() - d_data->initialPos.x(), 
                        d_data->pos.y() - d_data->initialPos.y());
                }
            }
        }
        default:;
    }

    return false;
}
