/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2003   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

// vim: expandtab

#ifndef QWT_TEXT_ENGINE_H
#define QWT_TEXT_ENGINE_H 1

#include <qsize.h>
#include "qwt_global.h"

class QFont;
class QRect;
class QString;
class QPainter;

class QWT_EXPORT QwtTextEngine
{
public:
    virtual ~QwtTextEngine();

    /*!
      Find the height for a given width
     */
    virtual int heightForWidth(const QFont &font, int flags, 
        const QString &text, int width) const = 0;

    /*!
      Returns the size, that is needed to render text
     */
    virtual QSize textSize(const QFont &font, int flags,
        const QString &text) const = 0;

    /*!
      \brief Draw the text in a clipping rectangle
      \param painter Painter
      \param rect Clipping rectangle
     */ 
    virtual void draw(QPainter *painter, const QRect &rect,
        int flags, const QString &text) const = 0;

    virtual bool mightRender(const QString &) const = 0;

    /*!
      Return margins around the texts

      The textSize might include margins around the 
      text, like QFontMetrics::descent. In situations
      where texts need to be aligend in detail, knowing
      these margins might improve the layout calculations.
     */
    virtual void textMargins(const QFont &, const QString &,
        int &left, int &right, int &top, int &bottom) const = 0;

protected:
    QwtTextEngine();
};


/*!
  \brief Plain text drawing support
 */
class QWT_EXPORT QwtPlainTextEngine: public QwtTextEngine
{
public:
    QwtPlainTextEngine();
    virtual ~QwtPlainTextEngine();

    virtual int heightForWidth(const QFont &font, int flags, 
        const QString &text, int width) const;

    virtual QSize textSize(const QFont &font, int flags,
        const QString &text) const;

    virtual void draw(QPainter *painter, const QRect &rect,
        int flags, const QString &text) const;

    virtual bool mightRender(const QString &) const;

    virtual void textMargins(const QFont &, const QString &,
        int &left, int &right, int &top, int &bottom) const;

private:
    class PrivateData; 
    PrivateData *d_data;
};


#ifndef QT_NO_RICHTEXT

/*!
  \brief Rich text drawing support
 */
class QWT_EXPORT QwtRichTextEngine: public QwtTextEngine
{
public:
    QwtRichTextEngine();

    virtual int heightForWidth(const QFont &font, int flags, 
        const QString &text, int width) const;

    virtual QSize textSize(const QFont &font, int flags,
        const QString &text) const;

    virtual void draw(QPainter *painter, const QRect &rect,
        int flags, const QString &text) const;

    virtual bool mightRender(const QString &) const;

    virtual void textMargins(const QFont &, const QString &,
        int &left, int &right, int &top, int &bottom) const;
private:
    QString taggedText(const QString &, int flags) const;
};

#endif // !QT_NO_RICHTEXT

#endif
