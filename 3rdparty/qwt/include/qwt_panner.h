/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#ifndef QWT_PANNER_H
#define QWT_PANNER_H 1

#include <qnamespace.h>
#include <qwidget.h>
#include "qwt_global.h"

class QCursor;

class QWT_EXPORT QwtPanner: public QWidget
{
    Q_OBJECT

public:
    QwtPanner(QWidget* parent);
    virtual ~QwtPanner();

    void setEnabled(bool);
    bool isEnabled() const;

    void setMouseButton(int button, int buttonState = Qt::NoButton);
    void setCursor(const QCursor &);
    const QCursor cursor() const;

protected:
    virtual bool eventFilter(QObject *, QEvent *);
    virtual void paintEvent(QPaintEvent *);

signals:
    void panned(int dx, int dy);

private:
    class PrivateData;
    PrivateData *d_data;
};

#endif
