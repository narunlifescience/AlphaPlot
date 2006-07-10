/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2003   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

// vim: expandtab

#ifndef QWT_TEXT_H
#define QWT_TEXT_H

#include <qstring.h>
#include <qsize.h>
#include <qfont.h>
#include "qwt_global.h"

class QColor;
class QPen;
class QBrush;
class QRect;
class QPainter;
class QwtTextEngine;

class QWT_EXPORT QwtText
{
public:
    enum TextFormat
    {
        AutoText = 0,
        
        PlainText,
        RichText,

        MathMLText,
        TeXText
    };

    enum PaintAttributes
    {
        PaintUsingPainter = 0,

        PaintUsingTextFont = 1,
        PaintUsingTextColor = 2,

        PaintBackground = 4
    };

    enum LayoutAttributes
    {
        DefaultLayout = 0,
        MinimumLayout = 1
    };

    QwtText(const QString & = QString::null, 
        TextFormat textFormat = AutoText);
    QwtText(const QwtText &);
    ~QwtText();

    QwtText &operator=(const QwtText &);

    int operator==(const QwtText &) const;
    int operator!=(const QwtText &) const;

    void setText(const QString &, 
        QwtText::TextFormat textFormat = AutoText);
    QString text() const;

    inline bool isNull() const { return text().isNull(); }
    inline bool isEmpty() const { return text().isEmpty(); }

    void setFont(const QFont &);
    QFont font() const;

    QFont usedFont(const QFont &) const;

    void setFlags(int flags);
    int flags() const;

    void setColor(const QColor &);
    QColor color() const;

    QColor usedColor(const QColor &) const;

    void setBackgroundPen(const QPen &);
    QPen backgroundPen() const;

    void setBackgroundBrush(const QBrush &);
    QBrush backgroundBrush() const;

    void setPaintAttributes(int);
    int paintAttributes() const;

    void setLayoutAttributes(int);
    int layoutAttributes() const;

    int heightForWidth(int width, const QFont & = QFont()) const;
    QSize textSize(const QFont & = QFont()) const;

    void draw(QPainter *painter, const QRect &rect) const;

private:
    const QwtTextEngine *textEngine(const QString &text,
        QwtText::TextFormat textFormat = AutoText) const;

    class PrivateData;
    PrivateData *d_data;

    class LayoutCache;
    LayoutCache *d_layoutCache;

    class EngineDict;
};

#endif
