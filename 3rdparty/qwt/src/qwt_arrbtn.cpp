/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#include <qpainter.h>
#include <qstyle.h>
#include <qevent.h>
#include "qwt_math.h"
#include "qwt_arrbtn.h"

#if 1
static const int MaxNum = 3;
static const int Margin = 2;
static const int Spacing = 1;
#endif

/*!
  \param num Number of arrows
  \param arrowType see Qt::ArowType in the Qt docs.
  \param parent Parent widget
  \param name Object name
*/
QwtArrowButton::QwtArrowButton(int num, Qt::ArrowType arrowType, 
        QWidget *parent, const char *name): 
    QPushButton (parent, name),
    d_num(qwtLim(num,1,MaxNum)),
    d_arrowType(arrowType)
{
    // We don't need a pixmap, but setting one toggles
    // QPushButton in pixmap mode what removes text
    // related properties from thy style engines.

    setPixmap(QPixmap()); 
    setAutoRepeat(TRUE);
    setAutoDefault(FALSE);
}

/*!
  \brief The direction of the arrows
*/
Qt::ArrowType QwtArrowButton::arrowType() const 
{ 
    return d_arrowType; 
}

/*!
  \brief The number of arrows
*/
int QwtArrowButton::num() const 
{ 
    return d_num; 
}

/*!
  \return the bounding rect for the label
*/
QRect QwtArrowButton::labelRect() const
{
    QRect r =
#if QT_VERSION < 300
        style().buttonRect(rect().x(), rect().y(),
        rect().width(), rect().height());
#else
        style().subRect(QStyle::SR_PushButtonContents, this);
#endif

    r.setRect(r.x() + Margin, r.y() + Margin, 
        r.width() - 2 * Margin, r.height() - 2 * Margin);

    if ( isDown() )
    {
        int ph, pv;
#if QT_VERSION < 300
        style().getButtonShift(ph, pv);
#else
        ph = style().pixelMetric(
            QStyle::PM_ButtonShiftHorizontal, this);
        pv = style().pixelMetric(
            QStyle::PM_ButtonShiftVertical, this);
#endif
        r.moveBy(ph, pv);
    }

    return r;
}

/*!
  \brief Draw the button label
  \sa The Qt Manual on QPushButton
*/
void QwtArrowButton::drawButtonLabel(QPainter *p)
{
    const QRect r = labelRect();

    QSize boundingSize = labelRect().size();
    if ( d_arrowType == Qt::UpArrow || d_arrowType == Qt::DownArrow )
        boundingSize.transpose();
        
    const int w = (boundingSize.width() - (MaxNum - 1) * Spacing) / MaxNum;

    QSize arrow = arrowSize(Qt::RightArrow, 
        QSize(w, boundingSize.height()));

    if ( d_arrowType == Qt::UpArrow || d_arrowType == Qt::DownArrow )
        arrow.transpose();

    QRect contentsSize; // aligned rect where to paint all arrows
    if ( d_arrowType == Qt::LeftArrow || d_arrowType == Qt::RightArrow )
    {
        contentsSize.setWidth(d_num * arrow.width() 
            + (d_num - 1) * Spacing);
        contentsSize.setHeight(arrow.height());
    }
    else
    {
        contentsSize.setWidth(arrow.width());
        contentsSize.setHeight(d_num * arrow.height() 
            + (d_num - 1) * Spacing);
    }

    QRect arrowRect(contentsSize);
    arrowRect.moveCenter(r.center());
    arrowRect.setSize(arrow);

    p->save();
    for (int i = 0; i < d_num; i++)
    {
        drawArrow(p, arrowRect, d_arrowType);

        if ( d_arrowType == Qt::LeftArrow || d_arrowType == Qt::RightArrow )
            arrowRect.moveBy(arrow.width() + Spacing, 0);
        else
            arrowRect.moveBy(0, arrow.height() + Spacing);
    }
    p->restore();

#if QT_VERSION >= 300
    if ( hasFocus() )
    {
        const QRect focusRect =  
            style().subRect(QStyle::SR_PushButtonFocusRect, this);
        style().drawPrimitive(QStyle::PE_FocusRect, p,
            focusRect, colorGroup());
    }
#endif
}

/*!
    Draw an arrow int a bounding rect

    \param p Painter
    \param r Rectangle where to paint the arrow
    \param arrowType Arrow type
*/
void QwtArrowButton::drawArrow(QPainter *p, 
    const QRect &r, Qt::ArrowType arrowType) const 
{
    QPointArray pa(3);

    switch(arrowType)
    {
        case Qt::UpArrow:
            pa.setPoint(0, r.bottomLeft());
            pa.setPoint(1, r.bottomRight());
            pa.setPoint(2, r.center().x(), r.top());
            break;
        case Qt::DownArrow:
            pa.setPoint(0, r.topLeft());
            pa.setPoint(1, r.topRight());
            pa.setPoint(2, r.center().x(), r.bottom());
            break;
        case Qt::RightArrow:
            pa.setPoint(0, r.topLeft());
            pa.setPoint(1, r.bottomLeft());
            pa.setPoint(2, r.right(), r.center().y());
            break;
        case Qt::LeftArrow:
            pa.setPoint(0, r.topRight());
            pa.setPoint(1, r.bottomRight());
            pa.setPoint(2, r.left(), r.center().y());
            break;
    }

    p->setPen(colorGroup().buttonText());
    p->setBrush(colorGroup().brush(QColorGroup::ButtonText));
    p->drawPolygon(pa);
}


/*! 
  \return Fixed/Expanding for Left/Right, Expanding/Fixed for Up/Down
*/
QSizePolicy QwtArrowButton::sizePolicy() const
{
    QSizePolicy policy;
    if ( d_arrowType == Qt::LeftArrow || d_arrowType == Qt::RightArrow )
        policy = QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    else
        policy = QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    return policy;
}

/*!
  \return a size hint
*/
QSize QwtArrowButton::sizeHint() const
{
    return minimumSizeHint();
}

/*!
  \brief Return a minimum size hint
*/
QSize QwtArrowButton::minimumSizeHint() const
{
    const QSize asz = arrowSize(Qt::RightArrow, QSize()); 

    QSize sz(
        2 * Margin + (MaxNum - 1) * Spacing + MaxNum * asz.width(),
        2 * Margin + asz.height()
    );

    if ( d_arrowType == Qt::UpArrow || d_arrowType == Qt::DownArrow )
        sz.transpose();

#if QT_VERSION < 300
    int bm = style().buttonMargin() - 1;
    sz += QSize(2 * bm, 2 * bm);
#else
    sz = style().sizeFromContents(QStyle::CT_PushButton, this, sz);
#endif
    return sz;
}

/*!
   Calculate the size for a arrow that fits into a rect of a given size

   \param arrowType Arrow type
   \param boundingSize Bounding size
   \return Size of the arrow
*/
QSize QwtArrowButton::arrowSize(Qt::ArrowType arrowType,
    const QSize &boundingSize) const
{
    QSize bs = boundingSize;
    if ( arrowType == Qt::UpArrow || arrowType == Qt::DownArrow )
        bs.transpose();
        
    const int MinLen = 2;
    const QSize sz = bs.expandedTo(
        QSize(MinLen, 2 * MinLen - 1)); // minimum

    int w = sz.width();
    int h = 2 * w - 1;

    if ( h > sz.height() )
    {
        h = sz.height();
        w = (h + 1) / 2;
    }

    QSize arrSize(w, h);
    if ( arrowType == Qt::UpArrow || arrowType == Qt::DownArrow )
        arrSize.transpose();

    return arrSize;
}

/*!
  \brief autoRepeat for the space keys
*/
void QwtArrowButton::keyPressEvent(QKeyEvent *e)
{
    if ( e->isAutoRepeat() && e->key() == Qt::Key_Space )
        emit clicked();

    QPushButton::keyPressEvent(e);
}
