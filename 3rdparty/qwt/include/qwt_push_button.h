/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#ifndef QWT_PUSH_BUTTON_H
#define QWT_PUSH_BUTTON_H

#include <qpushbutton.h>
#include "qwt_global.h"

/*!
  A push button that allows alignment of button labels, 
  and use of rich texts, as provided by QLabel.

  \warning The layout of the text label might be broken for a couple
           of styles. At least the KDE 3.3 Keramik style is known.
*/

class QWT_EXPORT QwtPushButton: public QPushButton
{
    Q_OBJECT

    Q_PROPERTY(TextFormat textFormat READ textFormat WRITE setTextFormat)
    Q_PROPERTY(Alignment alignment READ alignment WRITE setAlignment)
    Q_PROPERTY(int indent READ indent WRITE setIndent)

public:
    QwtPushButton(QWidget * = NULL, const char * name = NULL);
    QwtPushButton(const QString &text, 
        QWidget * = NULL, const char *name = NULL);
    QwtPushButton(const QIconSet &, const QString &, 
        QWidget * = NULL, const char *name = NULL);

    // The second technology preview release of Qt-4 needs Qt::TextFormat
    // but moc accepts only TextFormat
    TextFormat textFormat() const;
    void setTextFormat(TextFormat);

    virtual Qt::TextFormat usedTextFormat() const;

    int alignment() const;
    virtual void setAlignment(int alignment);

    void setIndent(int);
    int indent() const;

    virtual QSize sizeHint() const;
    virtual int heightForWidth(int) const;

protected:
    virtual void drawButtonLabel(QPainter *);

private:
    void init();

    Qt::TextFormat d_textFormat;
    int d_alignment;
    int d_indent;
};

#endif
