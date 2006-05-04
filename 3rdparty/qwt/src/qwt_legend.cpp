/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

// vim: expandtab

#include <qapplication.h> 
#include <qpainter.h>
#include <qbitmap.h>
#include <qstyle.h>
#include "qwt_text.h"
#include "qwt_legend.h"
#include "qwt_painter.h"
#include "qwt_dyngrid_layout.h"

static const int IdentifierWidth = 8;
static const int Margin = 2;

//! Create an item with mode = ShowLine|ShowText and no pen
QwtLegendItem::QwtLegendItem():
    d_identifierMode(ShowLine | ShowText),
    d_curvePen(Qt::NoPen)
{
}

/*! 
  Constructor with mode = ShowLine|ShowText
  \param symbol Symbol
  \param curvePen Curve pen
*/
QwtLegendItem::QwtLegendItem(const QwtSymbol &symbol, const QPen &curvePen):
    d_identifierMode(ShowLine | ShowText),
    d_symbol(symbol),
    d_curvePen(curvePen)
{
}

//! Destructor
QwtLegendItem::~QwtLegendItem()
{
}

/*! 
  Set identifier mode.
  Default is ShowLine | ShowText.
  \param mode Or'd values of IdentifierMode

  \sa QwtLegendItem::identifierMode()
*/
void QwtLegendItem::setIdentifierMode(int mode) 
{
    if ( mode != d_identifierMode )
    {
        d_identifierMode = mode;
        updateItem();
    }
}

/*!
  Or'd values of IdentifierMode.
  \sa QwtLegendButton::setIdentifierMode(), QwtLegendItem::IdentifierMode
*/
int QwtLegendItem::identifierMode() const 
{ 
    return d_identifierMode; 
}

/*! 
  Set curve symbol.
  \param symbol Symbol

  \sa QwtLegendItem::symbol()
*/
void QwtLegendItem::setSymbol(const QwtSymbol &symbol) 
{
    if ( symbol != d_symbol )
    {
        d_symbol = symbol;
        updateItem();
    }
}
    
/*!
  \return The curve symbol.
  \sa QwtLegendButton::setSymbol()
*/
const QwtSymbol& QwtLegendItem::symbol() const 
{ 
    return d_symbol; 
}
    

/*! 
  Set curve pen.
  \param pen Curve pen

  \sa QwtLegendItem::curvePen()
*/
void QwtLegendItem::setCurvePen(const QPen &pen) 
{
    if ( pen != d_curvePen )
    {
        d_curvePen = pen;
        updateItem();
    }
}

/*!
  \return The curve pen.
  \sa QwtLegendButton::setCurvePen()
*/
const QPen& QwtLegendItem::curvePen() const 
{ 
    return d_curvePen; 
}

//! Update the item
void QwtLegendItem::updateItem()
{
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

    if ( (d_identifierMode & ShowLine ) && (d_curvePen.style() != Qt::NoPen) )
    {
        painter->save();
        painter->setPen(d_curvePen);
        QwtPainter::drawLine(painter, rect.left(), rect.center().y(), 
            rect.right(), rect.center().y());
        painter->restore();
    }

    if ( (d_identifierMode & ShowSymbol) 
        && (d_symbol.style() != QwtSymbol::None) )
    {
        QSize symbolSize = 
            QwtPainter::metricsMap().screenToLayout(d_symbol.size());

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
        painter->setBrush(d_symbol.brush());
        painter->setPen(d_symbol.pen());
        d_symbol.draw(painter, symbolRect);
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
    const QwtMetricsMap &map = QwtPainter::metricsMap();

    const int margin = map.screenToLayoutX(Margin);

    const QRect identifierRect(rect.x() + margin, rect.y(), 
        map.screenToLayoutX(IdentifierWidth), rect.height());
    drawIdentifier(painter, identifierRect);

    // Label

    QwtText *txt = titleText();
    if ( txt )
    {
        QRect titleRect = rect;
        titleRect.setX(identifierRect.right() + 2 * margin);
     
        txt->draw(painter, titleRect);
        delete txt;
    }
}


/*!
  \param parent Parent widget
  \param name Widget name
*/
QwtLegendButton::QwtLegendButton(QWidget *parent, const char *name): 
    QwtPushButton(parent, name)
{
    init();
}
    
/*!
  \param symbol Curve symbol
  \param curvePen Curve pen
  \param text Button text
  \param parent Parent widget
  \param name Widget name
*/
QwtLegendButton::QwtLegendButton(
        const QwtSymbol &symbol, const QPen &curvePen, const QString &text,
        QWidget *parent, const char *name): 
    QwtPushButton(text, parent, name),
    QwtLegendItem(symbol, curvePen)
{
    init();
}

void QwtLegendButton::init()
{
    setFlat(TRUE);
    setAlignment(Qt::AlignLeft | Qt::AlignVCenter | 
        Qt::ExpandTabs | Qt::WordBreak);
    setIndent(2 * Margin);
    updateIconset();
}

void QwtLegendButton::updateItem()
{
    updateIconset();
}

/*! 
  Update the iconset according to the current identifier properties
*/
void QwtLegendButton::updateIconset()
{
    const QFontMetrics fm(font());

    QPixmap pm(IdentifierWidth, fm.height());
    pm.fill(this, 0, 0);

    QPainter p(&pm);
    drawIdentifier(&p, QRect(0, 0, pm.width(), pm.height()) );
    p.end();

    pm.setMask(pm.createHeuristicMask());

    setIconSet(QIconSet(pm));
}

/*! 
  Set the title of the button
  \param title New title

  \sa QwtLegendButton::title
*/
void QwtLegendButton::setTitle(const QString &title)
{
    setText(title);
}

/*! 
  \return The button title
  \sa QwtLegendButton::setTitle. 
*/
QString QwtLegendButton::title() const
{
    return text();
}

/*! 
  \return The item title
  \sa QwtLegendButton::setTitle. 
*/
QwtText *QwtLegendButton::titleText() const
{
    return QwtText::makeText(text(), usedTextFormat(),
        alignment(), font());
}

/*!
  \param parent Parent widget
  \param name Widget name
*/
QwtLegendLabel::QwtLegendLabel(QWidget *parent, const char *name):
    QLabel(parent, name)
{
    init();
}

/*!
  \param symbol Curve symbol
  \param curvePen Curve pen
  \param text Label text
  \param parent Parent widget
  \param name Widget name
*/
QwtLegendLabel::QwtLegendLabel(const QwtSymbol &symbol, 
        const QPen &curvePen, const QString &text, 
        QWidget *parent, const char *name):
    QLabel(text, parent, name),
    QwtLegendItem(symbol, curvePen)
{
    init();
}

void QwtLegendLabel::init()
{
    setAlignment(Qt::AlignLeft | Qt::AlignVCenter | 
        Qt::ExpandTabs | Qt::WordBreak);
    setIndent(Margin + IdentifierWidth + 2 * Margin);
    setMargin(Margin);
}

/*! 
  Set the title of the label
  \param title New title

  \sa QwtLegendLabel::title
*/
void QwtLegendLabel::setTitle(const QString &title)
{
    setText(title);
}

/*! 
  \return The label title
  \sa QwtLegendLabel::setTitle. 
*/
QString QwtLegendLabel::title() const
{
    return text();
}

/*! 
  \return The item title
  \sa QwtLegendLabel::setTitle.
*/
QwtText *QwtLegendLabel::titleText() const
{
    return QwtText::makeText(text(), textFormat(), 
        alignment(), font());
}

/*!
  \param painter Painter
*/
void QwtLegendLabel::drawContents(QPainter *painter)
{
    QLabel::drawContents(painter);

    QRect rect = contentsRect();
    rect.setX(rect.x() + Margin);
    rect.setWidth(IdentifierWidth);

    drawIdentifier(painter, rect);
}

void QwtLegendLabel::updateItem()
{
    update();
}

/*!
  \param parent Parent widget
  \param name Widget name
*/
QwtLegend::QwtLegend(QWidget *parent, const char *name): 
    QScrollView(parent, name),
    d_readOnly(FALSE),
    d_displayPolicy(QwtLegend::Auto),
    d_identifierMode(QwtLegendButton::ShowLine
             | QwtLegendButton::ShowSymbol
             | QwtLegendButton::ShowText)
{
    setFrameStyle(NoFrame);
    setResizePolicy(Manual);

    viewport()->setBackgroundMode(NoBackground); // Avoid flicker

    d_contentsWidget = new QWidget(viewport());
    d_contentsWidget->installEventFilter(this);

    QwtDynGridLayout *layout = new QwtDynGridLayout(d_contentsWidget);
    layout->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    layout->setAutoAdd(TRUE);

    addChild(d_contentsWidget);
}

/*!
  A mode telling QwtPlot to insert read only items
  \sa isReadOnly
*/
void QwtLegend::setReadOnly(bool readOnly)
{
    d_readOnly = readOnly;
}

/*! 
  A mode telling QwtPlot to insert read only items
  \sa setReadOnly
*/
bool QwtLegend::isReadOnly() const
{
    return d_readOnly;
}

/*!
  Set the legend display policy to:

  \param policy Legend display policy
  \param mode Identifier mode (or'd ShowLine, ShowSymbol, ShowText)

  \sa QwtLegend::displayPolicy, QwtLegend::LegendDisplayPolicy
*/
void QwtLegend::setDisplayPolicy(LegendDisplayPolicy policy, int mode)
{
    d_displayPolicy = policy;
    if (-1 != mode)
       d_identifierMode = mode;
}

/*! 
  \return the legend display policy.
  Default is LegendDisplayPolicy::Auto.
  \sa QwtLegend::setDisplayPolicy, QwtLegend::LegendDisplayPolicy
*/ 

QwtLegend::LegendDisplayPolicy QwtLegend::displayPolicy() const 
{ 
    return d_displayPolicy; 
}

/*!
  \return the IdentifierMode to be used in combination with
  LegendDisplayPolicy::Fixed.

  Default is ShowLine | ShowSymbol | ShowText.
*/

int QwtLegend::identifierMode() const
{
    return d_identifierMode;
}

/*! 
  The contents widget is the only child of the viewport() and
  the parent widget of all legend items.
*/
QWidget *QwtLegend::contentsWidget() 
{ 
    return d_contentsWidget; 
}

/*!  
  The contents widget is the only child of the viewport() and
  the parent widget of all legend items.
*/

const QWidget *QwtLegend::contentsWidget() const 
{ 
    return d_contentsWidget; 
}

/*!
  Insert a new item for a specific key.
  \param item New legend item
  \param key Unique key. Key must be >= 0.
  \note The parent of item will be changed to QwtLegend::contentsWidget()
  \note In case of key < 0, nothing will be inserted.
*/
void QwtLegend::insertItem(QWidget *item, long key)
{
    if ( item == NULL || key < 0 )
        return;

    if ( item->parent() != d_contentsWidget )
        item->reparent(d_contentsWidget, QPoint(0, 0));

    item->show();

    if ( d_items.count() > d_items.size() - 5 )
        d_items.resize(d_items.count() + 5);

    d_items.insert(key, item);

    layoutContents();

    QWidget *w = 0;

    if ( d_contentsWidget->layout() )
    {
        // set tab focus chain

        QLayoutIterator layoutIterator = 
            d_contentsWidget->layout()->iterator();

        for ( QLayoutItem *item = layoutIterator.current();
            item != 0; item = ++layoutIterator)
        {
            if ( w && item->widget() )
                QWidget::setTabOrder(w, item->widget());

            w = item->widget();
        }
    }
}

//! Find the item for a given key.
QWidget *QwtLegend::findItem(long key)
{
    return d_items.find(key);
}

//! Find the item for a given key.
const QWidget *QwtLegend::findItem(long key) const 
{ 
    return d_items.find(key); 
}
    
//! Find the item for a given key and remove it from the item list.
QWidget *QwtLegend::takeItem(long key) 
{ 
    return d_items.take(key); 
}

/*!
  Return the key of an legend item.
  \param item Legend item
  \return key of the item, or -1 if the item can't be found.
*/
long QwtLegend::key(const QWidget *item) const
{
    QWidgetIntDictIt it(d_items);
    for ( const QWidget *w = it.toFirst(); w != 0; w = ++it)
    {
        if ( w == item )
            return it.currentKey();
    }
    return -1;
}

//! Remove all items.
void QwtLegend::clear()
{
    // We can't delete the items while we are running
    // through the iterator. So we collect them in
    // a list first.

    QValueList<QWidget *> clearList;
    
    QWidgetIntDictIt it(d_items);
    for ( QWidget *item = it.toFirst(); item != 0; item = ++it)
        clearList += item;

    for ( uint i = 0; i < clearList.count(); i++ )
        delete clearList[i];

#if QT_VERSION < 232
    // In Qt 2.3.0 the ChildRemoved events are not sent, before the
    // first show of the legend. Thus the deleted items are not cleared
    // from the list in QwtLegend::eventFilter. In most cases
    // the following clear is useless, but is is safe to do so.
    
    d_items.clear();
#endif
}

//! Return an item iterator.
QWidgetIntDictIt QwtLegend::itemIterator() const
{
    return QWidgetIntDictIt(d_items);
}

//! Return a size hint.
QSize QwtLegend::sizeHint() const
{
    QSize hint = d_contentsWidget->sizeHint();
    hint += QSize(2 * frameWidth(), 2 * frameWidth());
    
    return hint;
}

/*!
  \return The preferred height, for the width w.
*/
int QwtLegend::heightForWidth(int w) const
{
    w -= 2 * frameWidth();

    int h = d_contentsWidget->heightForWidth(w);
    if ( h <= 0 ) // not implemented, we try the layout
    {
        QLayout *l = d_contentsWidget->layout();
        if ( l && l->hasHeightForWidth() )
        {
            h = l->heightForWidth(w);
            h += 2 * frameWidth();
        }
    }

    return h;
}

/*!
  Adjust contents widget and item layout to the size of the viewport().
*/
void QwtLegend::layoutContents()
{
    const QSize visibleSize = viewport()->size();

    const QLayout *l = d_contentsWidget->layout();
    if ( l && l->inherits("QwtDynGridLayout") )
    {
        const QwtDynGridLayout *tl = (const QwtDynGridLayout *)l;

        const int minW = int(tl->maxItemWidth()) + 2 * tl->margin();

        int w = QMAX(visibleSize.width(), minW);
        int h = QMAX(tl->heightForWidth(w), visibleSize.height());

        const int vpWidth = viewportSize(w, h).width();

        if ( w > vpWidth )
        {
            w = QMAX(vpWidth, minW);
            h = QMAX(tl->heightForWidth(w), visibleSize.height());
        }

        d_contentsWidget->resize(w, h);
        resizeContents(w, h);
    }
}

/*
  Filter QEvent::ChildRemoved, and QEvent::LayoutHint for
  QwtLegend::contentsWidget().
*/

//! Event filter
bool QwtLegend::eventFilter(QObject *o, QEvent *e)
{
    if ( o == d_contentsWidget )
    {
        switch(e->type())
        {
            case QEvent::ChildRemoved:
            {   
                const QChildEvent *ce = (const QChildEvent *)e;
                if ( ce->child()->isWidgetType() )
                    (void)takeItem(key((QWidget *)ce->child()));
                break;
            }
            case QEvent::LayoutHint:
            {
                layoutContents();
                break;
            }
            default:
                break;
        }
    }
    
    return QScrollView::eventFilter(o, e);
}

/*!
  Resize the viewport() and post a QEvent::LayoutHint to
  QwtLegend::contentsWidget() to update the layout.
*/
void QwtLegend::viewportResizeEvent(QResizeEvent *e)
{
    QScrollView::viewportResizeEvent(e);

    // It's not safe to update the layout now, because
    // we are in an internal update of the scrollview framework.
    // So we delay the update by posting a LayoutHint.

    QApplication::postEvent(d_contentsWidget, new QEvent(QEvent::LayoutHint));
}
