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

#include <qfont.h>
#include <qfontmetrics.h>
#include <qnamespace.h>
#include <qpen.h>
#include <qbrush.h>
#include <qsimplerichtext.h>
#include <qstring.h>

#include "qwt_global.h"

/*!
  \brief A virtual class to draw text

  QwtText is an interface class for drawing plain and rich text. It provides
  also an interface for text classes that understand other text description
  languages such as LaTeX or LyX.
  
  \warning Rich text in Qt-2.3.x does not understand sub- and super-scripts.
  Qt-3.x.x does a better job in rendering mathematics.
 */
class QWT_EXPORT QwtText
{
public:
    virtual ~QwtText();

    static QwtText *makeText(const QString &, Qt::TextFormat,
        int align, const QFont &, const QColor &color = QColor(),
        const QPen &pen = QPen(Qt::NoPen),
        const QBrush &brush = QBrush(Qt::NoBrush));

    static QwtText *makeText(const QString &, 
        int align, const QFont &, const QColor &color = QColor(),
        const QPen &pen = QPen(Qt::NoPen),
        const QBrush &brush = QBrush(Qt::NoBrush));

    static void setDefaultFormat(Qt::TextFormat);
    static Qt::TextFormat defaultFormat();

    virtual void setText(const QString &);
    QString text() const;

    inline bool isNull() const { return text().isNull(); }
    inline bool isEmpty() const { return text().isEmpty(); }

    virtual void setFont(const QFont &);
    QFont font() const;
    QFontMetrics fontMetrics() const;

    virtual void setAlignment(int align);
    int alignment() const;

    virtual void setColor(const QColor &);
    QColor color() const;

    virtual void setRectPen(const QPen &);
    QPen rectPen() const;

    virtual void setRectBrush(const QBrush &);
    QBrush rectBrush() const;

    /*!
      \brief Find the height for a given width
      \param width Width
      \return height Height 
     */
    virtual int heightForWidth(int width) const = 0;

    /*!
      \brief Returns the bounding rectangle.

      The bounding rectangle is calculated with respect to (0, 0).
      The left and top border depend on the alignment flags:
      - the left border is at -width, -width/2 or 0 for Qt::AlignRight,
        Qt::AlignHCenter or Qt::AlignLeft respectively
      - the top border is at 0, -height/2 or -height for Qt::AlignTop,
        Qt::AlignVCenter or Qt::AlignBottom respectively

      Use for instance QRect::moveBy(x,y), before drawing the text into
      the rectangle.
     */
    virtual QRect boundingRect(QPainter *painter = 0) const = 0;

    /*!
      \brief Draw the text in a clipping rectangle
      \param painter Painter
      \param rect Clipping rectangle
     */ 
    virtual void draw(QPainter *painter, const QRect &rect) const = 0;

    /*!
      Create a clone 
    */
    virtual QwtText *clone() const = 0;

protected:
    QwtText(const QString &text, const QFont &, int align, const QColor &,
        const QPen &pen = QPen(Qt::NoPen),
        const QBrush &brush = QBrush(Qt::NoBrush));

private:
    int d_align;
    QString d_text;
    QFont d_font;
    QColor d_color;
    QFontMetrics d_fm;
    QPen d_rectPen;
    QBrush d_rectBrush;

    static Qt::TextFormat d_defaultFormat;
};


/*!
  \brief Plain text drawing support
 */
class QWT_EXPORT QwtPlainText: public QwtText
{
public:
    QwtPlainText(const QString &text, const QFont &,
        int align = Qt::AlignCenter | Qt::WordBreak | Qt::ExpandTabs,
        const QColor &color = QColor(), const QPen &pen = QPen(Qt::NoPen),
        const QBrush &brush = QBrush(Qt::NoBrush));

    QwtPlainText(const QString &text, const QFont &font,
                 const QColor &color);

    virtual QwtText *clone() const;

    virtual int heightForWidth(int width) const;
    virtual QRect boundingRect(QPainter *painter = 0) const;

    virtual void draw(QPainter *painter, const QRect &rect) const;
};


#ifndef QT_NO_RICHTEXT

/*!
  \brief Rich text drawing support
 */
class QWT_EXPORT QwtRichText: public QwtText
{
public:
    QwtRichText(const QString &text, const QFont &font,
        int align = Qt::AlignCenter | Qt::WordBreak | Qt::ExpandTabs,
        const QColor &color = QColor(),
        const QPen &pen = QPen(Qt::NoPen),
        const QBrush &brush = QBrush(Qt::NoBrush));

    QwtRichText(const QString &, const QFont &, const QColor &);

    virtual ~QwtRichText();

    virtual QwtText *clone() const;

    virtual void setText(const QString &text);
    virtual void setFont(const QFont &font);
    virtual void setAlignment(int align);

    virtual int heightForWidth(int width) const;
    virtual QRect boundingRect(QPainter *painter = 0) const;

    virtual void draw(QPainter *painter, const QRect &rect) const;

private:
    QString taggedText(const QString &text, int alignment) const;

    QSimpleRichText *d_doc;
};

#endif

#endif

// Local Variables:
// mode: C++
// c-file-style: "stroustrup"
// End:
