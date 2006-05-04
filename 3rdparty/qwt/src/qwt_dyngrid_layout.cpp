/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#include "qwt_dyngrid_layout.h"

#if QT_VERSION >= 300
#include <qptrlist.h>
#else
#include <qlist.h>
#ifndef QPtrList
#define QPtrList QList
#define QPtrListIterator QListIterator
#endif
#endif

class QwtDynGridLayoutPrivate
{
public:
    QwtDynGridLayoutPrivate():
        isDirty(TRUE)
    {
    }

    QPtrList<QLayoutItem> itemList;

    bool isDirty;
    QwtArray<QSize> itemSizeHints;
};

class QwtDynGridLayoutIterator: public QGLayoutIterator
{
public:
    QwtDynGridLayoutIterator(QwtDynGridLayoutPrivate *);

    virtual QLayoutItem *current();
    virtual QLayoutItem *next();
    virtual QLayoutItem *takeCurrent();

private:
    QwtDynGridLayoutPrivate *d_data;
};

QwtDynGridLayoutIterator::QwtDynGridLayoutIterator(
        QwtDynGridLayoutPrivate *data):
    d_data(data)  
{
    (void)d_data->itemList.first();
}

QLayoutItem *QwtDynGridLayoutIterator::current()
{ 
    return d_data->itemList.current();
}

QLayoutItem *QwtDynGridLayoutIterator::next()
{ 
    return d_data->itemList.next();
}

QLayoutItem *QwtDynGridLayoutIterator::takeCurrent()
{ 
    d_data->isDirty = TRUE;
    return d_data->itemList.take(); 
}

/*!
  \param parent Parent widget
  \param margin Margin
  \param space Spacing
  \param name Widget name
*/

QwtDynGridLayout::QwtDynGridLayout(QWidget *parent, 
        int margin, int space, const char *name):
    QLayout(parent, margin, space, name)
{
    init();
}

/*!
  \param parent Parent widget
  \param space Spacing
  \param name Widget name
*/

QwtDynGridLayout::QwtDynGridLayout(QLayout *parent, 
        int space, const char *name):
    QLayout(parent, space, name)
{
    init();
}

/*!
  \param space Spacing
  \param name Widget name
*/

QwtDynGridLayout::QwtDynGridLayout(int space, const char *name):
    QLayout(space, name)
{
    init();
}

/*!
  Initialize the layout with default values.
*/
void QwtDynGridLayout::init()
{
    d_layoutData = new QwtDynGridLayoutPrivate;
    d_maxCols = d_numRows = d_numCols = 0;
    d_expanding = QSizePolicy::NoDirection;

    setSupportsMargin(TRUE);
}

//! Destructor

QwtDynGridLayout::~QwtDynGridLayout()
{
    deleteAllItems();
    delete d_layoutData;
}

void QwtDynGridLayout::invalidate()
{
    d_layoutData->isDirty = TRUE;
    QLayout::invalidate();
}

void QwtDynGridLayout::updateLayoutCache()
{
    d_layoutData->itemSizeHints.resize(itemCount());

    int index = 0;

    QPtrListIterator<QLayoutItem> it(d_layoutData->itemList);
    for (const QLayoutItem *item = it.toFirst(); item != 0;
        item = ++it, index++ )
    {
        d_layoutData->itemSizeHints[int(index)] = item->sizeHint();
    }

    d_layoutData->isDirty = FALSE;
}

/*!
  Limit the number of columns.
  \param maxCols upper limit, 0 means unlimited
  \sa QwtDynGridLayout::maxCols()
*/
  
void QwtDynGridLayout::setMaxCols(uint maxCols)
{
    d_maxCols = maxCols;
}

/*!
  Return the upper limit for the number of columns.
  0 means unlimited, what is the default.
  \sa QwtDynGridLayout::setMaxCols()
*/

uint QwtDynGridLayout::maxCols() const 
{ 
    return d_maxCols; 
}

//! Adds item to the next free position.

void QwtDynGridLayout::addItem(QLayoutItem *item)
{
    d_layoutData->itemList.append(item);
    invalidate();
}

/*! 
  \return TRUE if this layout is empty. 
*/

bool QwtDynGridLayout::isEmpty() const
{
    return d_layoutData->itemList.isEmpty();
}

/*! 
  \return number of layout items
*/

uint QwtDynGridLayout::itemCount() const
{
    return d_layoutData->itemList.count();
}

/*! 
  \return An iterator over the children of this layout.
*/

QLayoutIterator QwtDynGridLayout::iterator()
{       
    return QLayoutIterator( 
        new QwtDynGridLayoutIterator(d_layoutData) );
}

/*!
  Reorganizes columns and rows and resizes managed widgets within 
  the rectangle rect. 
*/

void QwtDynGridLayout::setGeometry(const QRect &rect)
{
    QLayout::setGeometry(rect);

    if ( isEmpty() )
        return;

    d_numCols = columnsForWidth(rect.width());
    d_numRows = itemCount() / d_numCols;
    if ( itemCount() % d_numCols )
        d_numRows++;

    QValueList<QRect> itemGeometries = layoutItems(rect, d_numCols);

    int index;

    QLayoutItem *item;
    QPtrListIterator<QLayoutItem> it(d_layoutData->itemList);
    for ( index = 0, item = it.toFirst(); item != 0; item = ++it )
    {
        QWidget *w = item->widget();
        if ( w )
        {
            w->setGeometry(itemGeometries[index]);
            index++;
        }
    }
}

/*! 
  Calculate the number of columns for a given width. It tries to
  use as many columns as possible (limited by maxCols())

  \param width Available width for all columns
  \sa QwtDynGridLayout::maxCols(), QwtDynGridLayout::setMaxCols()
*/

uint QwtDynGridLayout::columnsForWidth(int width) const
{
    if ( isEmpty() )
        return 0;

    const int maxCols = (d_maxCols > 0) ? d_maxCols : itemCount();
    if ( maxRowWidth(maxCols) <= width )
        return maxCols;

    for (int numCols = 2; numCols <= maxCols; numCols++ )
    {
        const int rowWidth = maxRowWidth(numCols);
        if ( rowWidth > width )
            return numCols - 1;
    }

    return 1; // At least 1 column
}

/*! 
  Calculate the width of a layout for a given number of
  columns.

  \param numCols Given number of columns
  \param itemWidth Array of the width hints for all items
*/
int QwtDynGridLayout::maxRowWidth(int numCols) const
{
    int col;

    QwtArray<int> colWidth(numCols);
    for ( col = 0; col < (int)numCols; col++ )
        colWidth[col] = 0;

    if ( d_layoutData->isDirty )
        ((QwtDynGridLayout*)this)->updateLayoutCache();

    for ( uint index = 0; 
        index < d_layoutData->itemSizeHints.count(); index++ )
    {
        col = index % numCols;
        colWidth[col] = QMAX(colWidth[col], 
            d_layoutData->itemSizeHints[int(index)].width());
    }

    int rowWidth = 2 * margin() + (numCols - 1) * spacing();
    for ( col = 0; col < (int)numCols; col++ )
        rowWidth += colWidth[col];

    return rowWidth;
}

/*!
  \return the maximum width of all layout items
*/

int QwtDynGridLayout::maxItemWidth() const
{
    if ( isEmpty() )
        return 0;

    if ( d_layoutData->isDirty )
        ((QwtDynGridLayout*)this)->updateLayoutCache();

    int w = 0;
    for ( uint i = 0; i < d_layoutData->itemSizeHints.count(); i++ )
    {
        const int itemW = d_layoutData->itemSizeHints[int(i)].width();
        if ( itemW > w )
            w = itemW;
    }

    return w;
}

/*!
  Calculate the geometries of the layout items for a layout
  with numCols columns and a given rect.
  \param rect Rect where to place the items
  \param numCols Number of columns
  \return item geometries
*/

QValueList<QRect> QwtDynGridLayout::layoutItems(const QRect &rect,
    uint numCols) const
{
    QValueList<QRect> itemGeometries;
    if ( numCols == 0 || isEmpty() )
        return itemGeometries;

    uint numRows = itemCount() / numCols;
    if ( numRows % itemCount() )
        numRows++;
 
    QwtArray<int> rowHeight(numRows);
    QwtArray<int> colWidth(numCols);
 
    layoutGrid(numCols, rowHeight, colWidth);

    if ( expanding() != QSizePolicy::NoDirection )
        stretchGrid(rect, numCols, rowHeight, colWidth);

    QwtDynGridLayout *that = (QwtDynGridLayout *)this;
    const int maxCols = d_maxCols;
    that->d_maxCols = numCols;
    const QRect alignedRect = alignmentRect(rect);
    that->d_maxCols = maxCols;

#if QT_VERSION < 300
    const int xOffset = ( expanding() & QSizePolicy::Horizontal ) 
        ? 0 : alignedRect.x();
    const int yOffset = ( expanding() & QSizePolicy::Vertical ) 
        ? 0 : alignedRect.y();
#else
    const int xOffset = ( expanding() & QSizePolicy::Horizontally ) 
        ? 0 : alignedRect.x();
    const int yOffset = ( expanding() & QSizePolicy::Vertically ) 
        ? 0 : alignedRect.y();
#endif

    QwtArray<int> colX(numCols);
    QwtArray<int> rowY(numRows);

    const int xySpace = spacing();

    rowY[0] = yOffset + margin();
    for ( int r = 1; r < (int)numRows; r++ )
        rowY[r] = rowY[r-1] + rowHeight[r-1] + xySpace;

    colX[0] = xOffset + margin();
    for ( int c = 1; c < (int)numCols; c++ )
        colX[c] = colX[c-1] + colWidth[c-1] + xySpace;
    
    int index;
    QLayoutItem *item;

    QPtrListIterator<QLayoutItem> it(d_layoutData->itemList);
    for ( item = it.toFirst(), index = 0; item != 0; item = ++it, index++ )
    {
        const int row = index / numCols;
        const int col = index % numCols;

        QRect itemGeometry(colX[col], rowY[row], 
            colWidth[col], rowHeight[row]);
        itemGeometries.append(itemGeometry);
    }

    return itemGeometries;
}


/*!
  Calculate the dimensions for the columns and rows for a grid
  of numCols columns.
  \param numCols Number of columns.
  \param rowHeight Array where to fill in the calculated row heights.
  \param colWidth Array where to fill in the calculated column widths.
*/

void QwtDynGridLayout::layoutGrid(uint numCols, 
    QwtArray<int>& rowHeight, QwtArray<int>& colWidth) const
{
    if ( numCols <= 0 )
        return;

    if ( d_layoutData->isDirty )
        ((QwtDynGridLayout*)this)->updateLayoutCache();

    for ( uint index = 0; 
        index < d_layoutData->itemSizeHints.count(); index++ )
    {
        const int row = index / numCols;
        const int col = index % numCols;

        const QSize &size = d_layoutData->itemSizeHints[int(index)];

        rowHeight[row] = (col == 0) 
            ? size.height() : QMAX(rowHeight[row], size.height());
        colWidth[col] = (row == 0) 
            ? size.width() : QMAX(colWidth[col], size.width());
    }
}

/*!
  Set whether this layout can make use of more space than sizeHint(). 
  A value of Vertical or Horizontal means that it wants to grow in only 
  one dimension, while BothDirections means that it wants to grow in 
  both dimensions. The default value is NoDirection. 
  \sa QwtDynGridLayout::expanding()
*/

void QwtDynGridLayout::setExpanding(QSizePolicy::ExpandData expanding)
{
    d_expanding = expanding;
}

/*!
  Returns whether this layout can make use of more space than sizeHint(). 
  A value of Vertical or Horizontal means that it wants to grow in only 
  one dimension, while BothDirections means that it wants to grow in 
  both dimensions. 
  \sa QwtDynGridLayout::setExpanding()
*/

QSizePolicy::ExpandData QwtDynGridLayout::expanding() const
{
    return d_expanding;
}

/*!
  \return TRUE: QwtDynGridLayout implements heightForWidth.
  \sa QwtDynGridLayout::heightForWidth()
*/

bool QwtDynGridLayout::hasHeightForWidth() const
{
    return TRUE;
}

/*!
  \return The preferred height for this layout, given the width w. 
  \sa QwtDynGridLayout::hasHeightForWidth()
*/

int QwtDynGridLayout::heightForWidth(int width) const
{
    if ( isEmpty() )
        return 0;

    const uint numCols = columnsForWidth(width);
    uint numRows = itemCount() / numCols;
    if ( itemCount() % numCols )
        numRows++;

    QwtArray<int> rowHeight(numRows);
    QwtArray<int> colWidth(numCols);

    layoutGrid(numCols, rowHeight, colWidth);

    int h = 2 * margin() + (numRows - 1) * spacing();
    for ( int row = 0; row < (int)numRows; row++ )
        h += rowHeight[row];

    return h;
}

/*!
  Stretch columns in case of expanding() & QSizePolicy::Horizontal and
  rows in case of expanding() & QSizePolicy::Vertical to fill the entire
  rect. Rows and columns are stretched with the same factor.
  \sa QwtDynGridLayout::setExpanding(), QwtDynGridLayout::expanding()
*/

void QwtDynGridLayout::stretchGrid(const QRect &rect, 
    uint numCols, QwtArray<int>& rowHeight, QwtArray<int>& colWidth) const
{
    if ( numCols == 0 || isEmpty() )
        return;

#if QT_VERSION < 300
    if ( expanding() & QSizePolicy::Horizontal )
#else
    if ( expanding() & QSizePolicy::Horizontally )
#endif
    {
        int xDelta = rect.width() - 2 * margin() - (numCols - 1) * spacing();
        for ( int col = 0; col < (int)numCols; col++ )
            xDelta -= colWidth[col];

        if ( xDelta > 0 )
        {
            for ( int col = 0; col < (int)numCols; col++ )
            {
                const int space = xDelta / (numCols - col);
                colWidth[col] += space;
                xDelta -= space;
            }
        }
    }

#if QT_VERSION < 300
    if ( expanding() & QSizePolicy::Vertical )
#else
    if ( expanding() & QSizePolicy::Vertically )
#endif
    {
        uint numRows = itemCount() / numCols;
        if ( itemCount() % numCols )
            numRows++;

        int yDelta = rect.height() - 2 * margin() - (numRows - 1) * spacing();
        for ( int row = 0; row < (int)numRows; row++ )
            yDelta -= rowHeight[row];

        if ( yDelta > 0 )
        {
            for ( int row = 0; row < (int)numRows; row++ )
            {
                const int space = yDelta / (numRows - row);
                rowHeight[row] += space;
                yDelta -= space;
            }
        }
    }
}

/*!
   Return the size hint. If maxCols() > 0 it is the size for
   a grid with maxCols() columns, otherwise it is the size for
   a grid with only one row.
   \sa QwtDynGridLayout::maxCols(), QwtDynGridLayout::setMaxCols()
*/

QSize QwtDynGridLayout::sizeHint() const
{
    if ( isEmpty() )
        return QSize();

    const uint numCols = (d_maxCols > 0 ) ? d_maxCols : itemCount();
    uint numRows = itemCount() / numCols;
    if ( itemCount() % numCols )
        numRows++;

    QwtArray<int> rowHeight(numRows);
    QwtArray<int> colWidth(numCols);

    layoutGrid(numCols, rowHeight, colWidth);

    int h = 2 * margin() + (numRows - 1) * spacing();
    for ( int row = 0; row < (int)numRows; row++ )
        h += rowHeight[row];

    int w = 2 * margin() + (numCols - 1) * spacing(); 
    for ( int col = 0; col < (int)numCols; col++ )
        w += colWidth[col];

    return QSize(w, h);
}

/*!
  \return Number of rows of the current layout.
  \sa QwtDynGridLayout::numCols
  \warning The number of rows might change whenever the geometry changes
*/
uint QwtDynGridLayout::numRows() const 
{ 
    return d_numRows; 
}

/*!
  \return Number of columns of the current layout.
  \sa QwtDynGridLayout::numRows
  \warning The number of columns might change whenever the geometry changes
*/
uint QwtDynGridLayout::numCols() const 
{ 
    return d_numCols; 
}

// Local Variables:
// mode: C++
// c-file-style: "stroustrup"
// indent-tabs-mode: nil
// End:

