/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2003   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

// vim: expandtab

#include <qpainter.h>
#include <qpalette.h>
#include <qstylesheet.h>
#include "qwt_painter.h"
#include "qwt_text.h"

Qt::TextFormat QwtText::d_defaultFormat = Qt::AutoText;

/*!
  \brief Constructor

  \param text Text
  \param font Font
  \param align Alignment flags
  \param color Color
  \param pen Pen
  \param brush Brush
*/

QwtText::QwtText(const QString &text, const QFont &font, 
        int align, const QColor &color, const QPen &pen, const QBrush &brush):
    d_align(align),
    d_text(text),
    d_font(font),
    d_color(color),
    d_fm(font),
    d_rectPen(pen),
    d_rectBrush(brush)
{
}

//! Destructor
QwtText::~QwtText() 
{
}

/*!
  Factory function for QwtText objects. Creates a text according
  to QwtText::defaultFormat()

  \param text Contents of the text
  \param align Or'd values of Qt::AlignmentFlags for drawing the text
  \param font Font of the text
  \param color Color of the text
  \param pen Pen to draw the enclosing rectangle
  \param brush Brush to draw the enclosing rectangle 
  \return Pointer to a new QwtPlainText or QwtRichText object
  \sa QwtText::defaultFormat, QwtText::setDefaultFormat
*/
QwtText *QwtText::makeText(const QString &text, 
    int align, const QFont &font, const QColor &color,
    const QPen &pen, const QBrush &brush)
{
    return makeText(text, d_defaultFormat, align, font, color, pen, brush);
}

/*!
  Factory function for QwtText objects.

  \param text Contents of the text
  \param format Format of the text
  \param align Or'd values of Qt::AlignmentFlags for drawing the text
  \param font Font of the text
  \param color Color of the text
  \param pen Pen to draw the enclosing rectangle
  \param brush Brush to draw the enclosing rectangle 
  \return Pointer to a new QwtPlainText or QwtRichText object
*/

QwtText *QwtText::makeText(const QString &text, Qt::TextFormat format,
    int align, const QFont &font, const QColor &color,
    const QPen &pen, const QBrush &brush)
{
#ifndef QT_NO_RICHTEXT
    if (format == Qt::RichText || ((format == Qt::AutoText)
            && QStyleSheet::mightBeRichText(text)))
    {
        return new QwtRichText(text, font, align, color, pen, brush);
    }
    else
#endif
    {
        return new QwtPlainText(text, font, align, color, pen, brush);
    }
}

//!  Set the default format for the QwtText factory
void QwtText::setDefaultFormat(Qt::TextFormat format)
{
    d_defaultFormat = format;
}

//! Return default format for the QwtText factory
Qt::TextFormat QwtText::defaultFormat()
{
    return d_defaultFormat;
}

//! Set the text.
void QwtText::setText(const QString &text) 
{ 
    d_text = text; 
}

//! Return the text.
QString QwtText::text() const 
{ 
    return d_text; 
}

//! Set the font.
void QwtText::setFont(const QFont &font) 
{
    d_font = font; 
    d_fm = QFontMetrics(font);
}

//! Return the font.
QFont QwtText::font() const 
{ 
    return d_font; 
}

//! Return the font metrics.
QFontMetrics QwtText::fontMetrics() const 
{ 
    return d_fm; 
}

/*!
  Set the alignment flags.
  \param align Or'd Qt::Alignment flags
*/
void QwtText::setAlignment(int align) 
{ 
    d_align = align; 
}

//! Get the alignment flags.
int QwtText::alignment() const 
{ 
    return d_align; 
}

//! Set the color.
void QwtText::setColor(const QColor &color) 
{ 
    d_color = color; 
}

//! Return the color.
QColor QwtText::color() const 
{ 
    return d_color; 
}

//! Set the rectangle pen.
void QwtText::setRectPen(const QPen &pen) 
{ 
    d_rectPen = pen; 
}

//! Return the rectangle pen.
QPen QwtText::rectPen() const 
{ 
    return d_rectPen; 
}

//! Set the rectangle brush.
void QwtText::setRectBrush(const QBrush &brush) 
{ 
    d_rectBrush = brush; 
}

//! Return the rectangle brush.
QBrush QwtText::rectBrush() const 
{ 
    return d_rectBrush; 
}

/*!
  \brief Constructor

  \param text Contents of the text
  \param font Font of the text
  \param align Or'd values of Qt::AlignmentFlags for drawing the text
  \param color Color of the text
  \param pen Pen to draw the enclosing rectangle
  \param brush Brush to draw the enclosing rectangle 
*/

QwtPlainText::QwtPlainText(const QString &text, const QFont &font,
        int align, const QColor &color, const QPen &pen, const QBrush &brush):
    QwtText(text, font, align, color, pen, brush) 
{
}

/*!
  \brief Constructor
  \param text Text
  \param font Font
  \param color Color
*/

QwtPlainText::QwtPlainText(const QString &text, 
        const QFont &font, const QColor &color):
    QwtText(text, font, 
        Qt::AlignCenter | Qt::WordBreak | Qt::ExpandTabs, color) 
{
}

//! Create a clone
QwtText *QwtPlainText::clone() const
{
    return new QwtPlainText(
        text(), font(), alignment(), color(), rectPen(), rectBrush());
}

/*!
  \brief Find the height for a given width.
  \param width Width
  \return height Height 
*/
int QwtPlainText::heightForWidth(int width) const
{
    const QwtLayoutMetrics metrics(QwtPainter::metricsMap());
    return metrics.heightForWidth(text(), width, alignment(), fontMetrics());
}

/*!
  \brief Draw the text in a clipping rectangle.
  \param painter Painter
  \param rect Clipping rectangle
*/
void QwtPlainText::draw(QPainter *painter, const QRect &rect) const
{
    painter->save();
    painter->setPen(rectPen());
    painter->setBrush(rectBrush());
    QwtPainter::drawRect(painter, rect);
    painter->restore();

    painter->save();
    painter->setFont(font());
    painter->setPen(color());
    QwtPainter::drawText(painter, rect, alignment(), text());
    painter->restore();
}

QRect QwtPlainText::boundingRect(QPainter *painter) const
{
    const QwtLayoutMetrics metrics(QwtPainter::metricsMap());
    
    if (painter)
    {
        painter->save();
        painter->setFont(font());
        const QRect rect = metrics.boundingRect(text(), 
            alignment(), painter);
        painter->restore();
        return rect;
    }

    return metrics.boundingRect(text(), alignment(), fontMetrics());
}

#ifndef QT_NO_RICHTEXT

/*!
  \brief Constructor

  \param text Contents of the text
  \param font Font of the text
  \param align Or'd values of Qt::AlignmentFlags for drawing the text
  \param color Color of the text
  \param pen Pen to draw the enclosing rectangle
  \param brush Brush to draw the enclosing rectangle 
*/

QwtRichText::QwtRichText(const QString &text, const QFont &font,
        int align, const QColor &color, const QPen &pen, const QBrush &brush):
    QwtText(text, font, align, color, pen, brush),
    d_doc(new QSimpleRichText(text, font))
{ 
    setText(text); 
}

/*!
  \brief Constructor
  \param text Text
  \param font Font
  \param color Color
*/

QwtRichText::QwtRichText(const QString &text, const QFont &font,
        const QColor &color):
    QwtText(text, font,
        Qt::AlignCenter | Qt::WordBreak | Qt::ExpandTabs, color),
    d_doc(new QSimpleRichText(text, font))
{ 
    setText(text); 
}

//! Destructor
QwtRichText::~QwtRichText() 
{ 
    delete d_doc; 
}

//! Create a clone
QwtText *QwtRichText::clone() const
{
    return new QwtRichText(
        text(), font(), alignment(), color(), rectPen(), rectBrush());
}

//! Set the text
void QwtRichText::setText(const QString &text)
{
    QwtText::setText(text);

    delete d_doc;
    d_doc = new QSimpleRichText(taggedText(text, alignment()), font());
}

//! Set the font
void QwtRichText::setFont(const QFont &font)
{
#if QT_VERSION >= 300
    d_doc->setDefaultFont(font);
#endif
    QwtText::setFont(font);
}

//! Set the alignment
void QwtRichText::setAlignment(int align)
{
    QwtText::setAlignment(align);

    delete d_doc;
    d_doc = new QSimpleRichText(taggedText(text(), align), font());
}

/*!
  \brief Find the height for a given width
  \param width Width
  \return height Height 
*/
int QwtRichText::heightForWidth(int width) const
{
#if QT_VERSION < 300
    const QFont defaultFont = QFont::defaultFont();
    QFont::setDefaultFont(font());
#endif

    const QwtLayoutMetrics metrics(QwtPainter::metricsMap());
    const int height = metrics.heightForWidth(*d_doc, width);

#if QT_VERSION < 300
    QFont::setDefaultFont(defaultFont);
#endif

    return height;
}

/*!
  \brief Draw the text in a clipping rectangle
  \param painter Painter
  \param rect Clipping rectangle
*/
void QwtRichText::draw(QPainter *painter, const QRect &rect) const
{
    painter->save();
    painter->setPen(rectPen());
    painter->setBrush(rectBrush());
    QwtPainter::drawRect(painter, rect);
    painter->restore();

    painter->save();

    painter->setPen(color());
#if QT_VERSION < 300
    const QFont defaultFont = QFont::defaultFont();
    QFont::setDefaultFont(font());
#else
    painter->setFont(font());
#endif

    QwtPainter::drawSimpleRichText(painter, rect, alignment(), *d_doc);

#if QT_VERSION < 300
    QFont::setDefaultFont(defaultFont);
#endif
    painter->restore();
}

QRect QwtRichText::boundingRect(QPainter *painter) const
{
#if QT_VERSION < 300
    const QFont defaultFont = QFont::defaultFont();
    QFont::setDefaultFont(font());
#endif

    const QwtLayoutMetrics metrics(QwtPainter::metricsMap());
    const QRect rect = metrics.boundingRect(*d_doc, alignment(), painter);

#if QT_VERSION < 300
    QFont::setDefaultFont(defaultFont);
#endif

    return rect;
}

//! Wrap text into <div align=...> </div> tags according align
QString QwtRichText::taggedText(const QString &text, int align) const
{
    QString rich = text;

    // By default QwtSimpleRichText is Qt::AlignLeft
#if QT_VERSION >= 300
    if (align & Qt::AlignJustify) 
    {
        rich.prepend(QString::fromLatin1("<div align=\"justify\">"));
        rich.append(QString::fromLatin1("</div>"));
    } else
#endif
    if (align & Qt::AlignRight)
    {
        rich.prepend(QString::fromLatin1("<div align=\"right\">"));
        rich.append(QString::fromLatin1("</div>"));
    }
    else if (align & Qt::AlignHCenter) 
    {
        rich.prepend(QString::fromLatin1("<div align=\"center\">"));
        rich.append(QString::fromLatin1("</div>"));
    }

    return rich;
}

#endif

// Local Variables:
// mode: C++
// c-file-style: "stroustrup"
// indent-tabs-mode: nil
// End:
