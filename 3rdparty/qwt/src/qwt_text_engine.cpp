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
#include <qpixmap.h>
#include <qimage.h>
#include <qmap.h>
#include <qwidget.h>
#include "qwt_math.h"
#include "qwt_painter.h"
#include "qwt_text_engine.h"

#if QT_VERSION < 0x040000

#include <qsimplerichtext.h>
#include <qstylesheet.h>

class QwtRichTextDocument: public QSimpleRichText
{
public:
    QwtRichTextDocument(const QString &text, const QFont &font):
        QSimpleRichText(text, font)
    {
    }
};

#else // QT_VERSION >= 0x040000

#define USE_LABEL 1
#ifdef USE_LABEL
#include <qlabel.h>
#else
#include <qtextobject.h>
#endif
#include <qtextdocument.h>
#include <qabstracttextdocumentlayout.h>

class QwtRichTextDocument: public QTextDocument
{
public:
    QwtRichTextDocument(const QString &text, const QFont &font)
    {
        setUndoRedoEnabled(false);
        setDefaultFont(font);
        setHtml(text);

        // make sure we have a document layout
        (void)documentLayout();
    }
};

#endif

class QwtPlainTextEngine::PrivateData
{
public:
    int effectiveAscent(const QFont &font) const
    {
        const QString fontKey = font.key();

        QMap<QString, int>::const_iterator it = 
            d_ascentCache.find(fontKey);
        if ( it == d_ascentCache.end() )
        {
            int ascent = findAscent(font);
            it = d_ascentCache.insert(fontKey, ascent);
        }

        return (*it);
    }

private:
    int findAscent(const QFont &font) const
    {
        static const QString dummy("E");
        static const QColor white(Qt::white);

        const QFontMetrics fm(font);
        QPixmap pm(fm.width(dummy), fm.height()); 
        pm.fill(white);

        QPainter p(&pm);
        p.setFont(font);  
        p.drawText(0, 0,  pm.width(), pm.height(), 0, dummy);
        p.end();

    #if QT_VERSION < 0x040000
        const QImage img = pm.convertToImage();
    #else
        const QImage img = pm.toImage();
    #endif

        int row = 0;
        for ( row = 0; row < img.height(); row++ )
        {   
            const QRgb *line = (const QRgb *)img.scanLine(row);

            const int w = pm.width();
            for ( int col = 0; col < w; col++ )
            {   
                if ( line[col] != white.rgb() )
                    return fm.ascent() - row + 1;
            }
        }

        return fm.ascent();
    }   

    mutable QMap<QString, int> d_ascentCache;
};


QwtTextEngine::QwtTextEngine()
{
}

QwtTextEngine::~QwtTextEngine()
{
}

void QwtTextEngine::textMargins(const QFont &, const QString &,
    int &left, int &right, int &top, int &bottom)
{
    left = right = top = bottom = 0;
}

QwtPlainTextEngine::QwtPlainTextEngine()
{
    d_data = new PrivateData;
}

QwtPlainTextEngine::~QwtPlainTextEngine()
{
    delete d_data;
}

int QwtPlainTextEngine::heightForWidth(const QFont& font, int flags,
        const QString& text, int width) const
{
    const QFontMetrics fm(font);
    const QRect rect = fm.boundingRect(
        0, 0, width, QWIDGETSIZE_MAX, flags, text);

    return rect.height();
}

QSize QwtPlainTextEngine::textSize(const QFont &font,
    int flags, const QString& text) const
{
    const QFontMetrics fm(font);
    const QRect rect = fm.boundingRect(
        0, 0, QWIDGETSIZE_MAX, QWIDGETSIZE_MAX, flags, text);

    return rect.size();
}

void QwtPlainTextEngine::textMargins(const QFont &font, const QString &,
    int &left, int &right, int &top, int &bottom)
{
    left = right = top = 0;

    const QFontMetrics fm(font);
    top = fm.ascent() - d_data->effectiveAscent(font);
    bottom = fm.descent() + 1;
}

void QwtPlainTextEngine::draw(QPainter *painter, const QRect &rect,
    int flags, const QString& text) const
{
    QwtPainter::drawText(painter, rect, flags, text);
}

bool QwtPlainTextEngine::mightRender(const QString &) const
{
    return true;
}


#ifndef QT_NO_RICHTEXT

QwtRichTextEngine::QwtRichTextEngine()
{
}

int QwtRichTextEngine::heightForWidth(const QFont& font, int flags,
        const QString& text, int width) const
{
    QwtRichTextDocument doc(taggedText(text, flags), font);

#if QT_VERSION < 0x040000
    doc.setWidth(width);
    const int h = doc.height();
#else
    doc.setPageSize(QSize(width, QWIDGETSIZE_MAX));
    const int h = qRound(doc.documentLayout()->documentSize().height());
#endif
    return h;
}

QSize QwtRichTextEngine::textSize(const QFont &font,
    int flags, const QString& text) const
{
    QwtRichTextDocument doc(taggedText(text, flags), font);

#if QT_VERSION < 0x040000
    doc.setWidth(QWIDGETSIZE_MAX);

    int w = doc.widthUsed();
    int h = doc.height();
#else
#if USE_LABEL 
    /*
      Unfortunately offering the bounding rect calculation in the
      API of QTextDocument has been forgotten in Qt <= 4.1.x. It
      is planned to come with Qt 4.2.x.
      In the meantime we need a hack with a temporary QLabel,
      to reengineer the internal calculations.
     */

    static int off = 0;
    static QLabel *label = NULL;
    if ( label == NULL )
    {
        label = new QLabel;
        label->hide();

        const char *s = "XXXXX";
        label->setText(s);
        int w1 = label->sizeHint().width();
        const QFontMetrics fm(label->font());
        int w2 = fm.width(s);
        off = w1 - w2;
    }
    label->setFont(doc.defaultFont());
    label->setText(text);

    int w = qwtMax(label->sizeHint().width() - off, 0);
    doc.setPageSize(QSize(w, QWIDGETSIZE_MAX));
    
    int h = qRound(doc.documentLayout()->documentSize().height());
#else
    QTextLayout *layout = doc.begin().layout();
    layout->beginLayout();
    for(qreal y = 0;;)  
    {
        QTextLine line = layout->createLine();
        if (!line.isValid())
            break;
        line.setPosition(QPointF(0, y));
        y += line.height();
    }
    layout->endLayout();

    int w = qRound(layout->maximumWidth());
    int h = qRound(layout->boundingRect().height());

    h += QFontMetrics(font).descent() + 4;
    w += 2 * 4;
#endif
#endif

    return QSize(w, h);
}

void QwtRichTextEngine::draw(QPainter *painter, const QRect &rect,
    int flags, const QString& text) const
{
    QwtRichTextDocument doc(taggedText(text, flags), painter->font());
    QwtPainter::drawSimpleRichText(painter, rect, flags, doc);
}

//! Wrap text into <div align=...> </div> tags according align
QString QwtRichTextEngine::taggedText(const QString &text, int flags) const
{
    QString richText = text;

    // By default QSimpleRichText is Qt::AlignLeft
    if (flags & Qt::AlignJustify)
    {
        richText.prepend(QString::fromLatin1("<div align=\"justify\">"));
        richText.append(QString::fromLatin1("</div>"));
    }
    else if (flags & Qt::AlignRight)
    {
        richText.prepend(QString::fromLatin1("<div align=\"right\">"));
        richText.append(QString::fromLatin1("</div>"));
    }
    else if (flags & Qt::AlignHCenter)
    {
        richText.prepend(QString::fromLatin1("<div align=\"center\">"));
        richText.append(QString::fromLatin1("</div>"));
    }

    return richText;
}

bool QwtRichTextEngine::mightRender(const QString &text) const
{
#if QT_VERSION < 0x040000
    return QStyleSheet::mightBeRichText(text);
#else
    return Qt::mightBeRichText(text);
#endif
}

#endif // !QT_NO_RICHTEXT
