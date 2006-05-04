/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#ifndef QWT_ARRBTN_H
#define QWT_ARRBTN_H

#include <qpushbutton.h>
#include "qwt_global.h"

/*!
  \brief Arrow Button

  A push button with one or more filled triangles on its front.
  An Arrow button can have 1 to 3 arrows in a row, pointing
  up, down, left or right.
*/
class QWT_EXPORT QwtArrowButton : public QPushButton
{
public:
    QwtArrowButton (int num, Qt::ArrowType, QWidget *parent,
          const char *name = 0);

    Qt::ArrowType arrowType() const;
    int num() const;

    virtual QSize sizeHint() const;
    virtual QSize minimumSizeHint() const;
    virtual QSizePolicy sizePolicy() const;

protected:
    virtual void drawButtonLabel(QPainter *p);
    virtual void drawArrow(QPainter *, 
        const QRect &, Qt::ArrowType) const;
    virtual QRect labelRect() const;
    virtual QSize arrowSize(Qt::ArrowType,
        const QSize &boundingSize) const;

    virtual void keyPressEvent(QKeyEvent *);

private:

    const int d_num;
    const Qt::ArrowType d_arrowType;
};

#endif
