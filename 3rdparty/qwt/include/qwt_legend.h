/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

// vim: expandtab

#ifndef QWT_LEGEND_H
#define QWT_LEGEND_H

#include <qpen.h>
#include <qscrollview.h>
#include <qlabel.h>
#include "qwt_global.h"
#include "qwt_symbol.h"
#include "qwt_push_button.h"

#if QT_VERSION < 0x040000
#include <qwidgetintdict.h>
#else
#include "qwt_plot_dict.h"
class QWT_EXPORT QWidgetIntDict: public QwtSeqDict<QWidget>
{
public:
    QWidgetIntDict() {}
};
typedef QIntDictIterator<QWidget> QWidgetIntDictIt;
#endif

class QPainter;
class QwtText;

/*!
  A legend item
  \sa QwtLegend, QwtCurve
*/
class QwtLegendItem
{
public:
    /*!
       \brief Identifier mode

       Default is ShowLine | ShowText
       \sa QwtLegendItem::identifierMode, QwtLegendItem::setIdentifierMode
     */

    enum IdentifierMode
    {
        NoIdentifier = 0,
        ShowLine = 1,
        ShowSymbol = 2,
        ShowText = 4
    };

    QwtLegendItem();
    QwtLegendItem(const QwtSymbol &, const QPen &);

    virtual ~QwtLegendItem();

    void setIdentifierMode(int);
    int identifierMode() const;

    void setSymbol(const QwtSymbol &);
    const QwtSymbol& symbol() const;

    void setCurvePen(const QPen &);
    const QPen& curvePen() const;

    virtual void drawIdentifier(QPainter *, const QRect &) const;
    virtual void drawItem(QPainter *p, const QRect &) const; 
    
    virtual void setTitle(const QString &) = 0;
    virtual QString title() const = 0;

protected:
    virtual QwtText *titleText() const = 0;
    virtual void updateItem();

private:
    int d_identifierMode; 
    QwtSymbol d_symbol;
    QPen d_curvePen;
};

/*!
  \brief A legend button

  QwtLegendButton represents a curve on a legend. 
  It displays an curve identifier with an explaining text. 
  The identifier might be a combination of curve symbol and line.
  
  \sa QwtLegend, QwtCurve
*/

class QWT_EXPORT QwtLegendButton: public QwtPushButton, public QwtLegendItem
{
    Q_OBJECT
public:
    QwtLegendButton(QWidget *parent = 0, const char *name = 0);
    QwtLegendButton(const QwtSymbol &, const QPen &,
        const QString &, QWidget *parent = 0, const char *name = 0);

    virtual void setTitle(const QString &);
    virtual QString title() const;

protected:
    virtual QwtText *titleText() const;
    virtual void updateItem();
    virtual void updateIconset();

private:
    void init();
};

/*!
  \brief A legend label

  QwtLegendLabel represents a curve on a legend.
  It displays an curve identifier with an explaining text.
  The identifier might be a combination of curve symbol and line.

  \sa QwtLegend, QwtCurve
*/
class QWT_EXPORT QwtLegendLabel: public QLabel, public QwtLegendItem
{
    Q_OBJECT
public:
    QwtLegendLabel(QWidget *parent = 0, const char *name = 0);
    QwtLegendLabel(const QwtSymbol &, const QPen &,
        const QString &, QWidget *parent = 0, const char *name = 0);

    virtual void setTitle(const QString &);
    virtual QString title() const;

protected:
    virtual QwtText *titleText() const;
    virtual void drawContents(QPainter *);
    virtual void updateItem();

private:
    void init();
};

/*!
  \brief The legend widget

  The QwtLegend widget is a tabular arrangement of legend items. Legend
  items might be any type of widget, but in general they will be
  a QwtLegendButton.

  \sa QwtLegendButton, QwtPlot
*/

class QWT_EXPORT QwtLegend : public QScrollView
{
    Q_OBJECT

    Q_PROPERTY( bool readOnly READ isReadOnly WRITE setReadOnly )

public:

    /*!
      \brief Display policy

       - None\n
         the client code is responsible how to display of each legend item.
         The Qwt library will not interfere.

       - Fixed\n
         all legend items are displayed with the QwtLegendItem::IdentifierMode
         to be passed in 'mode'.

       - Auto\n
         each legend item is displayed with a mode that is a bitwise or of
         - QwtLegendItem::ShowLine (if its curve is drawn with a line) and
         - QwtLegendItem::ShowSymbol (if its curve is drawn with symbols) and
         - QwtLegendItem::ShowText (if the has a title).

       Default is LegendDisplayPolicy::Auto.
       \sa QwtLegend::setDisplayPolicy, QwtLegend::displayPolicy,                          QwtLegendItem::IdentifierMode
     */

    enum LegendDisplayPolicy
    {
        None = 0,
        Fixed = 1,
        Auto = 2
    };

    QwtLegend(QWidget *parent = 0, const char *name = 0);
    
    void setReadOnly(bool readOnly);
    bool isReadOnly() const;

    void setDisplayPolicy(LegendDisplayPolicy policy, int mode = -1);
    LegendDisplayPolicy displayPolicy() const;

    int identifierMode() const;

    QWidget *contentsWidget();
    const QWidget *contentsWidget() const;

    void insertItem(QWidget *, long key);
    QWidget *findItem(long key);
    const QWidget *findItem(long key) const;
    QWidget *takeItem(long key);

    virtual QWidgetIntDictIt itemIterator() const;

    long key(const QWidget *) const;

    void clear();
    
    bool isEmpty() const;
    uint itemCount() const;

    virtual bool eventFilter(QObject *, QEvent *);

    virtual QSize sizeHint() const;
    virtual int heightForWidth(int w) const;

protected:
    virtual void viewportResizeEvent(QResizeEvent *);
    virtual void layoutContents();

private:
    bool d_readOnly;
    LegendDisplayPolicy d_displayPolicy;
    int d_identifierMode;
    QWidgetIntDict d_items;
    QWidget *d_contentsWidget;
};

//! Return TRUE, if there are no legend items.
inline bool QwtLegend::isEmpty() const 
{ 
    return d_items.count() == 0; 
}

//! Return the number of legend items.
inline uint QwtLegend::itemCount() const 
{ 
    return d_items.count(); 
}

#endif // QWT_LEGEND_H
