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
#include <qdrawutil.h>
#include <qstyle.h>
#include <qpen.h>
#if QT_VERSION >= 0x040000
#include <qevent.h>
#include <qstyleoption.h>
#endif
#include "qwt_painter.h"
#include "qwt_symbol.h"
#include "qwt_legend_item.h"

static const int IdentifierWidth = 8;
static const int Margin = 2;
static const int ButtonFrame = 2;

static QSize buttonShift(const QwtLegendItem *w)
{
#if QT_VERSION < 0x040000
    const int ph = w->style().pixelMetric(
        QStyle::PM_ButtonShiftHorizontal, w);
    const int pv = w->style().pixelMetric(
        QStyle::PM_ButtonShiftVertical, w);
#else
    QStyleOption option;
    option.init(w);

    const int ph = w->style()->pixelMetric(
        QStyle::PM_ButtonShiftHorizontal, &option, w);
    const int pv = w->style()->pixelMetric(
        QStyle::PM_ButtonShiftVertical, &option, w);
#endif
    return QSize(ph, pv);
}

class QwtLegendItem::PrivateData
{
public:
    PrivateData():
        itemMode(QwtLegend::ReadOnlyItem),
        isDown(false),
        identifierMode(QwtLegendItem::ShowLine | QwtLegendItem::ShowText),
        curvePen(Qt::NoPen)
    {
    }

    QwtLegend::LegendItemMode itemMode;
    bool isDown;

    int identifierMode;
    QwtSymbol symbol;
    QPen curvePen;
};

/*!
  \param parent Parent widget
*/
QwtLegendItem::QwtLegendItem(QWidget *parent):
    QwtTextLabel(parent)
{
    d_data = new PrivateData;
    init(QwtText());
}

/*!
  \param symbol Curve symbol
  \param curvePen Curve pen
  \param text Label text
  \param parent Parent widget
*/
QwtLegendItem::QwtLegendItem(const QwtSymbol &symbol, 
        const QPen &curvePen, const QwtText &text, 
        QWidget *parent):
    QwtTextLabel(parent)
{
    d_data = new PrivateData;

    d_data->symbol = symbol;
    d_data->curvePen = curvePen;

    init(text);
}

void QwtLegendItem::init(const QwtText &text)
{
    setIndent(Margin + IdentifierWidth + 2 * Margin);
    setMargin(Margin);
    setText(text);
}

QwtLegendItem::~QwtLegendItem()
{
    delete d_data;
    d_data = NULL;
}

void QwtLegendItem::setText(const QwtText &text)
{
    const int flags = Qt::AlignLeft | Qt::AlignVCenter
#if QT_VERSION < 0x040000
        | Qt::WordBreak | Qt::ExpandTabs;
#else
        | Qt::TextExpandTabs | Qt::TextWordWrap;
#endif

    QwtText txt = text;
    txt.setRenderFlags(flags);

    QwtTextLabel::setText(txt);
}

void QwtLegendItem::setItemMode(QwtLegend::LegendItemMode mode) 
{ 
    d_data->itemMode = mode; 
    d_data->isDown = false; 

#if QT_VERSION >= 0x040000
    using namespace Qt;
#endif
    setFocusPolicy(mode != QwtLegend::ReadOnlyItem ? TabFocus : NoFocus);
    setMargin(Margin + ButtonFrame);

    updateGeometry();
}

QwtLegend::LegendItemMode QwtLegendItem::itemMode() const 
{ 
    return d_data->itemMode; 
}

/*!
  Set identifier mode.
  Default is ShowLine | ShowText.
  \param mode Or'd values of IdentifierMode

  \sa QwtLegendItem::identifierMode()
*/
void QwtLegendItem::setIdentifierMode(int mode)
{
    if ( mode != d_data->identifierMode )
    {
        d_data->identifierMode = mode;
        update();
    }
}

/*!
  Or'd values of IdentifierMode.
  \sa QwtLegendItem::setIdentifierMode(), QwtLegendItem::IdentifierMode
*/
int QwtLegendItem::identifierMode() const 
{ 
    return d_data->identifierMode; 
}

/*! 
  Set curve symbol.
  \param symbol Symbol

  \sa QwtLegendItem::symbol()
*/
void QwtLegendItem::setSymbol(const QwtSymbol &symbol) 
{
    if ( symbol != d_data->symbol )
    {
        d_data->symbol = symbol;
        update();
    }
}
    
/*!
  \return The curve symbol.
  \sa QwtLegendItem::setSymbol()
*/
const QwtSymbol& QwtLegendItem::symbol() const 
{ 
    return d_data->symbol; 
}
    

/*! 
  Set curve pen.
  \param pen Curve pen

  \sa QwtLegendItem::curvePen()
*/
void QwtLegendItem::setCurvePen(const QPen &pen) 
{
    if ( pen != d_data->curvePen )
    {
        d_data->curvePen = pen;
        update();
    }
}

/*!
  \return The curve pen.
  \sa QwtLegendItem::setCurvePen()
*/
const QPen& QwtLegendItem::curvePen() const 
{ 
    return d_data->curvePen; 
}

/*! 
  Paint the identifier to a given rect.
  \param painter Painter
  \param rect Rect where to paint
*/
void QwtLegendItem::drawIdentifier(
    QPainter *painter, const QRect &rect) const
{
    if ( rect.isEmpty() )
        return;

    if ( (d_data->identifierMode & ShowLine ) && (d_data->curvePen.style() != Qt::NoPen) )
    {
        painter->save();
        painter->setPen(d_data->curvePen);
        QwtPainter::drawLine(painter, rect.left(), rect.center().y(), 
            rect.right(), rect.center().y());
        painter->restore();
    }

    if ( (d_data->identifierMode & ShowSymbol) 
        && (d_data->symbol.style() != QwtSymbol::None) )
    {
        QSize symbolSize = 
            QwtPainter::metricsMap().screenToLayout(d_data->symbol.size());

        // scale the symbol size down if it doesn't fit into rect.

        if ( rect.width() < symbolSize.width() )
        {
            const double ratio = 
                double(symbolSize.width()) / double(rect.width());
            symbolSize.setWidth(rect.width());
            symbolSize.setHeight(qRound(symbolSize.height() / ratio));
        }
        if ( rect.height() < symbolSize.height() )
        {
            const double ratio = 
                double(symbolSize.width()) / double(rect.width());
            symbolSize.setHeight(rect.height());
            symbolSize.setWidth(qRound(symbolSize.width() / ratio));
        }

        QRect symbolRect;
        symbolRect.setSize(symbolSize);
        symbolRect.moveCenter(rect.center());

        painter->save();
        painter->setBrush(d_data->symbol.brush());
        painter->setPen(d_data->symbol.pen());
        d_data->symbol.draw(painter, symbolRect);
        painter->restore();
    }
}

/*!
  Draw the legend item to a given rect.
  \param painter Painter
  \param rect Rect where to paint the button
*/

void QwtLegendItem::drawItem(QPainter *painter, const QRect &rect) const
{
    painter->save();

    const QwtMetricsMap &map = QwtPainter::metricsMap();

    const int margin = map.screenToLayoutX(Margin);

    const QRect identifierRect(rect.x() + margin, rect.y(), 
        map.screenToLayoutX(IdentifierWidth), rect.height());
    drawIdentifier(painter, identifierRect);

    // Label

    QRect titleRect = rect;
    titleRect.setX(identifierRect.right() + 2 * margin);
     
    text().draw(painter, titleRect);

    painter->restore();
}

void QwtLegendItem::paintEvent(QPaintEvent *e)
{
    const QRect cr = contentsRect();

    QPainter painter(this);
    painter.setClipRegion(e->region());

    if ( d_data->isDown )
    {
        qDrawWinButton(&painter, 0, 0, width(), height(), 
#if QT_VERSION < 0x040000
            colorGroup(), 
#else
            palette(),
#endif
            true);
    }

    painter.save();

    if ( d_data->isDown )
    {
        const QSize shiftSize = buttonShift(this);
        painter.translate(shiftSize.width(), shiftSize.height());
    }

    painter.setClipRect(cr);

    drawContents(&painter);

    QRect rect = cr;
    rect.setX(rect.x() + Margin);
    if ( d_data->itemMode != QwtLegend::ReadOnlyItem )
        rect.setX(rect.x() + ButtonFrame);

    rect.setWidth(IdentifierWidth);

    drawIdentifier(&painter, rect);

    painter.restore();
}

void QwtLegendItem::mousePressEvent(QMouseEvent *e)
{
    if ( e->button() != Qt::LeftButton )
        return;

    switch(d_data->itemMode)
    {
        case QwtLegend::ClickableItem:
        {
            setDown(true);
            break;
        }
        case QwtLegend::CheckableItem:
        {
            setDown(!isDown());
            break;
        }
        default:;
    }
}

void QwtLegendItem::mouseReleaseEvent(QMouseEvent *e)
{
    if ( !e->button() == Qt::LeftButton )
        return;

    if ( d_data->itemMode == QwtLegend::ClickableItem )
        setDown(false);
}

void QwtLegendItem::keyPressEvent(QKeyEvent *e)
{
    if ( e->key() != Qt::Key_Space || e->isAutoRepeat() )
        return;

    switch(d_data->itemMode)
    {
        case QwtLegend::ClickableItem:
        {
            setDown(true);
            break;
        }
        case QwtLegend::CheckableItem:
        {
            setDown(!isDown());
            break;
        }
        default:;
    }
}

void QwtLegendItem::keyReleaseEvent(QKeyEvent *e)
{
    if ( e->key() != Qt::Key_Space || e->isAutoRepeat() )
        return;

    if ( d_data->itemMode == QwtLegend::ClickableItem )
        setDown(false);
}

void QwtLegendItem::setChecked(bool on)
{
    if ( d_data->itemMode == QwtLegend::CheckableItem )
    {
        const bool isBlocked = signalsBlocked();
        blockSignals(true);

        setDown(on);

        blockSignals(isBlocked);
    }
}

bool QwtLegendItem::isChecked() const
{
    return d_data->itemMode == QwtLegend::CheckableItem && isDown();
}

void QwtLegendItem::setDown(bool down)
{
    if ( down == d_data->isDown )
        return;

    d_data->isDown = down;
    update();

    if ( d_data->itemMode == QwtLegend::ClickableItem )
    {
        if ( d_data->isDown )
            emit pressed();
        else
        {
            emit released();
            emit clicked();
        }
    }

    if ( d_data->itemMode == QwtLegend::CheckableItem )
        emit checked(d_data->isDown);
}

bool QwtLegendItem::isDown() const
{
    return d_data->isDown;
}

QSize QwtLegendItem::sizeHint() const
{
    QSize sz = QwtTextLabel::sizeHint();
    if ( d_data->itemMode != QwtLegend::ReadOnlyItem )
        sz += buttonShift(this);

    return sz;
}

void QwtLegendItem::drawText(QPainter *painter, const QRect &rect)
{
    QwtTextLabel::drawText(painter, rect);
}
