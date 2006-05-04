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
#include <qpicture.h>
#include <qsimplerichtext.h>
#include <qstylesheet.h>
#include <qstyle.h>
#include "qwt_text.h"
#include "qwt_push_button.h"

#ifndef QT_NO_PICTURE

class QwtPBPaintFilter: public QPicture
{
    // A helper class that filters the QPushButton paint commands
    // and changes them according the additional features of QwtPushButton 

public:
    QwtPBPaintFilter(const QwtPushButton *);

protected:
    virtual bool cmd(int, QPainter *, QPDevCmdParam *);

private:
    bool isLabel(const QPixmap *) const;
    QRect indentRect(const QRect &) const;

    const QwtPushButton *d_button;
    QRect d_iconRect;
    bool d_inFilter;
};

QwtPBPaintFilter::QwtPBPaintFilter(const QwtPushButton *button):
    d_button(button),
    d_inFilter(FALSE)
{
}

bool QwtPBPaintFilter::cmd(int c, QPainter *painter, QPDevCmdParam *param)
{
    switch(c)
    {
        case PdcDrawTextFormatted:
        case PdcDrawText2Formatted:
        {
            Qt::TextFormat textFormat = d_button->usedTextFormat();
            if ( textFormat == Qt::PlainText )
            {
                param[1].ival = d_button->alignment();

                QRect &r = *((QRect *)param[0].rect);
                r = indentRect(r);

                return QPicture::cmd(c, painter, param);
            }
            if ( textFormat == Qt::RichText )
            {
                if ( !d_inFilter ) // avoid recursive calls
                {
                    d_inFilter = TRUE;
                    QwtRichText richText(*param[2].str, painter->font(), 
                        d_button->alignment(), painter->pen().color());
                    richText.draw(painter, indentRect(*param[0].rect));
                    d_inFilter = FALSE;
                    return TRUE;
                }
            }
            break;
        }
        case PdcDrawPixmap:
        {
            if ( d_inFilter ) // avoid recursive calls
            {
                // There might be pixmaps embedded in rich text.
                // This problem is solved by the d_inFilter guard 
                // above too.
                break;
            }

            if ( isLabel(param[1].pixmap) &&
                !(d_button->alignment() & Qt::AlignCenter))
            {
                const QRect contentsRect =
#if QT_VERSION >= 300
                    d_button->style().subRect( 
                        QStyle::SR_PushButtonContents, d_button);
#else
                    d_button->style().pushButtonContentsRect(
                        (QPushButton *)d_button);
#endif

                QRect pixRect = contentsRect;
                if ( !d_iconRect.isEmpty() )
                    pixRect.setX(d_iconRect.right());

                // Many styles move the label right/down
                // when the button is down.

#if QT_VERSION >= 300
                const QRect &r = *((QRect *)param[0].rect);
#else
                const QRect r(*param[0].point, param[1].pixmap->size());
#endif
                const int offsetY = r.center().y() - pixRect.center().y();
                const int offsetX = d_iconRect.isEmpty() ? offsetY : 0;
                pixRect.moveBy(offsetX, offsetY);

                if ( d_button->indent() > 0 )
                {
                    const int indent = d_button->indent();
                    const int align = d_button->alignment();

                    if ( align & Qt::AlignRight )
                        pixRect.setRight(pixRect.right() - indent);
                    else if ( align & Qt::AlignLeft )
                        pixRect.setLeft(pixRect.left() + indent);

                    if ( align & Qt::AlignTop )
                        pixRect.setTop(pixRect.top() + indent);
                    else if ( align & Qt::AlignBottom )
                        pixRect.setBottom(pixRect.bottom() - indent);
                }

                pixRect &= contentsRect; // clip to contentsRect

                d_inFilter = TRUE;

                d_button->style().drawItem(painter, 
#if QT_VERSION >= 300
                    pixRect,
#else
                    pixRect.x(), pixRect.y(), 
                    pixRect.width(), pixRect.height(),
#endif
                    d_button->alignment(), d_button->colorGroup(), 
                    d_button->isEnabled(), param[1].pixmap, QString::null);

                d_inFilter = FALSE;
                return TRUE;
            }
            else
            {
                // We save the position of the icon. We need it later
                // to align the label pixmap. Hope that there are no styles
                // that paint the pixmap before the icon.
#if QT_VERSION < 300
                d_iconRect = QRect(*param[0].point, param[1].pixmap->size());
#else
                d_iconRect = *param[0].rect;
#endif
            }
            break;
        }
    }
    return QPicture::cmd(c, painter, param);
}

QRect QwtPBPaintFilter::indentRect(const QRect &rect) const
{
    const int indent = d_button->indent();
    if ( indent <= 0 )
        return rect;

    QRect r = rect;
    if ( d_button->alignment() & Qt::AlignRight )
        r.setRight(r.right() - indent);
    else if ( d_button->alignment() & Qt::AlignLeft )
        r.setLeft(r.left() + indent);

    if ( d_button->alignment() & Qt::AlignTop )
        r.setTop(r.top() + indent);
    else if ( d_button->alignment() & Qt::AlignBottom )
        r.setBottom(r.bottom() - indent);

    return r;
}

bool QwtPBPaintFilter::isLabel(const QPixmap *pixmap) const
{
    if ( !d_button->pixmap() || 
        d_button->pixmap()->serialNumber() != pixmap->serialNumber() )
    {
        return FALSE;
    }

    if ( d_button->iconSet() && !d_button->iconSet()->isNull() )
    {
        if ( d_button->iconSet()->pixmap().serialNumber() == 
            pixmap->serialNumber() )
        {
            // Iconset and label are both set, and use the same pixmap.
            // We hope the iconSet is painted first.

            static bool firstPixmap = TRUE;
            firstPixmap = !firstPixmap;

            return !firstPixmap;
        }
    }
    return TRUE;
}

#endif // !QT_NO_PICTURE

/*! 
  Constructs a push button with no text. 
*/
QwtPushButton::QwtPushButton(QWidget *parent, const char *name):
    QPushButton(parent, name)
{
    init();
}

/*! 
  Constructs a push button called name with the parent parent 
  and the text text. 
*/
QwtPushButton::QwtPushButton(const QString &text, 
        QWidget *parent, const char *name):
    QPushButton(text, parent, name)
{
    init();
}

/*!
  Constructs a push button with an icon and a text. 
*/
QwtPushButton::QwtPushButton(const QIconSet &iconSet, const QString &text, 
        QWidget *parent, const char *name):
    QPushButton(iconSet, text, parent, name)
{
    init();
}

//! initialize the button
void QwtPushButton::init()
{
    d_textFormat = Qt::AutoText;
    d_alignment = Qt::AlignCenter | Qt::ExpandTabs | Qt::WordBreak;
    d_indent = 4;
}

/*! 
  \brief Find the text format that is used for painting the button label.

  In case of Qt::AutoText the format is found by evaluating text(), otherwise
  it is textFormat()

  \return Text format that is used for painting
  \sa textFormat(), setTextFormat(), QStyleSheet::mightBeRichText()
*/

Qt::TextFormat QwtPushButton::usedTextFormat() const
{
#ifndef QT_NO_PICTURE
    if ( d_textFormat == Qt::AutoText && QStyleSheet::mightBeRichText(text()) )
        return Qt::RichText;
#endif

    return Qt::PlainText;
}

/*! 
  \return The button label's text format
  \sa QwtPushButton::setTextFormat
*/

Qt::TextFormat QwtPushButton::textFormat() const
{
    return d_textFormat;
}

/*! 
  \brief Sets the button label's text format.
  The default format is AutoText. 

  \param textFormat Text format for the button label text

  \sa QwtPushButton::textFormat()
*/

void QwtPushButton::setTextFormat(TextFormat textFormat)
{
    d_textFormat = textFormat;
}

/*!
  \return Alignment of the button label's contents.
  \sa QwtPushButton::setAlignment
*/

int QwtPushButton::alignment() const
{
    return d_alignment;
}

/*! 
  \brief Sets the alignment of the button label´s contents

  The default format is Qt::AlignCenter | Qt::ExpandTabs | Qt::WordBreak. 

  \param alignment Bitwise OR of Qt::AlignmentFlags and Qt::TextFlags values.

  \sa QwtPushButton::alignment()
*/
void QwtPushButton::setAlignment(int alignment)
{
    d_alignment = alignment;
}

/*!
  \return Indent of the button label's contents.
  \sa QwtPushButton::setIndent
*/

int QwtPushButton::indent() const
{
    return d_indent;
}
    
/*! 
  \brief Sets the button label's contents indent in pixels.

  The default value is 4 pixels.

  \param indent Indent

  \sa QwtPushButton::indent()
*/

void QwtPushButton::setIndent(int indent)
{   
    d_indent = indent;
}

/*!
  Find the preferred height, given the width w.

  \param width Width
  \return preferred height
*/

int QwtPushButton::heightForWidth(int width) const
{
    if ( pixmap() )
        return QPushButton::heightForWidth(width);

    QwtText *txt = QwtText::makeText(text(), usedTextFormat(),
        d_alignment, font());

    int h = sizeHint().height();
    h -= txt->boundingRect().height();
    h += txt->heightForWidth(width);

    delete txt;

    return h;
}

//! Return sizeHint
QSize QwtPushButton::sizeHint() const
{
    QSize hint = QPushButton::sizeHint();

    if ( d_indent > 0 )
    {
        if ( (d_alignment & Qt::AlignLeft)  
            || (d_alignment & Qt::AlignRight) )
        {
            hint.setWidth(hint.width() + d_indent);
        }
        if ( (d_alignment & Qt::AlignTop) 
            || (d_alignment & Qt::AlignBottom) )
        {
            hint.setHeight(hint.height() + d_indent);
        }
    }

    if ( pixmap() )
        return hint;

    const Qt::TextFormat textFormat = usedTextFormat();
    if ( textFormat == Qt::RichText )
    {
        QwtRichText richText(text(), font(), d_alignment);

        const QSize sizeText = fontMetrics().size(Qt::ShowPrefix, text());
        const QSize sizeRichText(richText.boundingRect().size());

        int iconHeight = 0;
        if ( iconSet() && !iconSet()->isNull() )
        {
            iconHeight = iconSet()->pixmap(QIconSet::Small,
                QIconSet::Normal).height();
        }

        const int heightText = QMAX(iconHeight, sizeText.height());
        const int heightRichText = QMAX(iconHeight, sizeRichText.height());

        hint.setWidth(hint.width() - sizeText.width() + sizeRichText.width());
        hint.setHeight(hint.height() - heightText + heightRichText);
    }

    return hint;
}

//! Draws the button text or pixmap. 
void QwtPushButton::drawButtonLabel(QPainter *painter)
{
#ifndef QT_NO_PICTURE
    // Unfortunately QStyle doesn´t offer an API to add
    // the alignment and rich text features. But we don´t want
    // to paint the button label on our own, as we would lose
    // the flexibility of the styles. So we let the style
    // paint the button label to a QPicture first, change
    // the paint commands and replay the manipulated commands
    // to the button.

    QwtPBPaintFilter paintFilter(this);

    QPainter picPainter(&paintFilter);
    picPainter.setFont(painter->font());

#if QT_VERSION >= 300
    // When painting to QPicture the dotted line of the focus rect is
    // set to solid. ( 06.08.2003 )
    // So we don´t set the Style_HasFocus flag and paint the focus rect
    // later directly to the button.

    QStyle::SFlags flags = QStyle::Style_Default;
    if (isEnabled())
        flags |= QStyle::Style_Enabled;
    if (isDown())
        flags |= QStyle::Style_Down;
    if (isOn())
        flags |= QStyle::Style_On;
    if (! isFlat() && ! isDown())
        flags |= QStyle::Style_Raised;
    if (isDefault())
        flags |= QStyle::Style_ButtonDefault;

    style().drawControl(QStyle::CE_PushButtonLabel, &picPainter, this,
            style().subRect(QStyle::SR_PushButtonContents, this),
            colorGroup(), flags);
#else
    // For Qt <= 2.x the focus rect is not painted in
    // drawButtonLabel. So we don´t need a workaround here.

    QPushButton::drawButtonLabel(&picPainter);
#endif

    picPainter.end();

    paintFilter.play(painter); 

#if QT_VERSION >= 300
    if (hasFocus())
    {
        // Paint the focus rect on top of the button label.

        flags |= QStyle::Style_HasFocus;
        style().drawPrimitive(QStyle::PE_FocusRect, painter, 
            style().subRect(QStyle::SR_PushButtonFocusRect, this), 
            colorGroup(), flags);
    }
#endif

#else // QT_NO_PICTURE
    QPushButton::drawButtonLabel(painter);
#endif
}

// Local Variables:
// mode: C++
// c-file-style: "stroustrup"
// indent-tabs-mode: nil
// End:
