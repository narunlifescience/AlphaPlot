/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#ifndef QWT_DYNGRID_LAYOUT_H
#define QWT_DYNGRID_LAYOUT_H

#include <qlayout.h>
#include <qsize.h>
#include "qwt_array.h"
#include <qvaluelist.h>
#include "qwt_global.h"

class QwtDynGridLayoutPrivate;

/*!
  \brief The QwtDynGridLayout class lays out widgets in a grid,
         adjusting the number of columns and rows to the current size.
         
  QwtDynGridLayout takes the space it gets, divides it up into rows and 
  columns, and puts each of the widgets it manages into the correct cell(s). 
  It lays out as many number of columns as possible (limited by maxCols()).
*/

class QWT_EXPORT QwtDynGridLayout : public QLayout
{
    Q_OBJECT
public:
    QwtDynGridLayout(QWidget *, int margin=0, 
        int space=-1, const char *name = 0);
    QwtDynGridLayout(QLayout *, int space=-1, const char * name = 0 );
    QwtDynGridLayout(int space=-1, const char * name=0 );

    virtual ~QwtDynGridLayout();

    virtual void invalidate();

    void setMaxCols(uint maxCols);
    uint maxCols() const;

    uint numRows () const; 
    uint numCols () const;

    virtual void addItem(QLayoutItem *);
    virtual QLayoutIterator iterator();

    virtual int maxItemWidth() const;

    virtual void setGeometry(const QRect &rect);

    virtual bool hasHeightForWidth() const;
    virtual int heightForWidth(int) const;

    virtual QSize sizeHint() const;

    void setExpanding(QSizePolicy::ExpandData);
    virtual QSizePolicy::ExpandData expanding() const;

    virtual bool isEmpty() const;
    uint itemCount() const;

    virtual uint columnsForWidth(int width) const;

    QValueList<QRect> layoutItems(const QRect &, uint numCols) const;

protected:

    void layoutGrid(uint numCols,
            QwtArray<int>& rowHeight, QwtArray<int>& colWidth) const;
    void stretchGrid(const QRect &rect, uint numCols, 
        QwtArray<int>& rowHeight, QwtArray<int>& colWidth) const;


private:
    void init();
    int maxRowWidth(int numCols) const;
    void updateLayoutCache();

    uint d_maxCols;
    uint d_numRows;
    uint d_numCols;
    
    QSizePolicy::ExpandData d_expanding;
    QwtDynGridLayoutPrivate *d_layoutData;
};

#endif

// Local Variables:
// mode: C++
// c-file-style: "stroustrup"
// indent-tabs-mode: nil
// End:
