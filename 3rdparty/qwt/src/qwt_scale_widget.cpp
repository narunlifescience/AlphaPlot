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
#include <qevent.h>
#include "qwt_painter.h"
#include "qwt_scale_widget.h"
#include "qwt_math.h"
#include "qwt_paint_buffer.h"
#include "qwt_scale_div.h"
#include "qwt_text.h"

class QwtScaleWidget::PrivateData
{
public:
    PrivateData():
        scaleDraw(NULL)
    {
    }

    ~PrivateData()
    {
        delete scaleDraw;
    }

    QwtScaleDraw *scaleDraw;

    int borderDist[2];
    int minBorderDist[2];
    int scaleLength;
    int baseDist;

    int titleOffset;
    int titleDist;
    QwtText title;
};

/*!
  \brief Create a scale with the position QwtScaleWidget::Left
  \param parent Parent widget

  \warning Workaround of a designer problem
*/
QwtScaleWidget::QwtScaleWidget(QWidget *parent):
    QWidget(parent)
{
    initScale(QwtScaleDraw::LeftScale);
}

/*!
  \brief Constructor
  \param align Alignment. 
  \param parent Parent widget
*/
QwtScaleWidget::QwtScaleWidget(
        QwtScaleDraw::Alignment align, QWidget *parent):
    QWidget(parent)
{
    initScale(align);
}

//! Destructor
QwtScaleWidget::~QwtScaleWidget()
{
    delete d_data;
}

//! Initialize the scale
void QwtScaleWidget::initScale(QwtScaleDraw::Alignment align)
{
    d_data = new PrivateData;

#if QT_VERSION < 0x040000
    setWFlags(Qt::WNoAutoErase);
#endif 

    d_data->borderDist[0] = 0;
    d_data->borderDist[1] = 0;
    d_data->minBorderDist[0] = 0;
    d_data->minBorderDist[1] = 0;
    d_data->baseDist = 4;
    d_data->titleOffset = 0;
    d_data->titleDist = 2;

    d_data->scaleDraw = new QwtScaleDraw;
    d_data->scaleDraw->setAlignment(align);
    d_data->scaleDraw->setLength(10);

    const int flags = Qt::AlignHCenter
#if QT_VERSION < 0x040000
        | Qt::WordBreak | Qt::ExpandTabs;
#else
        | Qt::TextExpandTabs | Qt::TextWordWrap;
#endif
    d_data->title.setFlags(flags); 
    d_data->title.setFont(font()); 

    QSizePolicy policy(QSizePolicy::MinimumExpanding,
        QSizePolicy::Fixed);
    if ( d_data->scaleDraw->orientation() == Qt::Vertical )
        policy.transpose();

    setSizePolicy(policy);
    
#if QT_VERSION >= 0x040000
    setAttribute(Qt::WA_WState_OwnSizePolicy, false);
#else
    clearWState( WState_OwnSizePolicy );
#endif

}

void QwtScaleWidget::setTitle(const QString &title)
{
    if ( d_data->title.text() != title )
    {
        d_data->title.setText(title);
        layoutScale();
    }
}

/*!
  \brief Give title new text contents
  \param title New title
  \sa QwtScaleWidget::title
  \warning The title flags are interpreted in
               direction of the label, AlignTop, AlignBottom can't be set
               as the title will always be aligned to the scale.
*/
void QwtScaleWidget::setTitle(const QwtText &title)
{
    QwtText t = title;
    const int flags = title.flags() & ~(Qt::AlignTop | Qt::AlignBottom);
    t.setFlags(flags);

    if (t != d_data->title)
    {
        d_data->title = t;
        layoutScale();
    }
}

/*!
  Change the alignment

  \param alignment New alignment
  \sa QwtScaleWidget::alignment
*/
void QwtScaleWidget::setAlignment(QwtScaleDraw::Alignment alignment)
{
#if QT_VERSION >= 0x040000
    if ( !testAttribute(Qt::WA_WState_OwnSizePolicy) )
#else
    if ( !testWState( WState_OwnSizePolicy ) )
#endif
    {
        QSizePolicy policy(QSizePolicy::MinimumExpanding,
            QSizePolicy::Fixed);
        if ( d_data->scaleDraw->orientation() == Qt::Vertical )
            policy.transpose();
        setSizePolicy(policy);

#if QT_VERSION >= 0x040000
        setAttribute(Qt::WA_WState_OwnSizePolicy, false);
#else
        clearWState( WState_OwnSizePolicy );
#endif
    }

    if (d_data->scaleDraw)
        d_data->scaleDraw->setAlignment(alignment);
    layoutScale();
}

        
/*! 
    \return position 
    \sa QwtScaleWidget::setPosition
*/
QwtScaleDraw::Alignment QwtScaleWidget::alignment() const 
{
    if (!scaleDraw())
        return QwtScaleDraw::LeftScale;

    return scaleDraw()->alignment();
}

/*!
  Specify distances of the scale's endpoints from the
  widget's borders. The actual borders will never be less
  than minimum border distance.
  \param dist1 Left or top Distance
  \param dist2 Right or bottom distance
  \sa QwtScaleWidget::borderDist
*/
void QwtScaleWidget::setBorderDist(int dist1, int dist2)
{
    if ( dist1 != d_data->borderDist[0] || dist2 != d_data->borderDist[1] )
    {
        d_data->borderDist[0] = dist1;
        d_data->borderDist[1] = dist2;
        layoutScale();
    }
}

/*!
  \brief Specify the border distance of the scale's base line.
  \param bd border distance of the baseline
  \sa QwtScaleWidget::baseLineDist
*/
void QwtScaleWidget::setBaselineDist(int bd)
{
    bd = qwtMax( 0, bd );
    if ( bd != d_data->baseDist )
    {
        d_data->baseDist = bd;
        layoutScale();
    }
}

/*!
  \brief Specify the distance between scale and title
  \param td Title distance
  \sa QwtScaleWidget::titleDist
*/
void QwtScaleWidget::setTitleDist(int td)
{
    td = qwtMax( 0, td );
    if ( td != d_data->titleDist )
    {
        d_data->titleDist = td;
        layoutScale();
    }
}

/*!
  \brief Change the alignment for the labels.

  \sa QwtScaleDraw::setLabelAlignment(), QwtScaleWidget::setLabelRotation()
*/
#if QT_VERSION < 0x040000
void QwtScaleWidget::setLabelAlignment(int alignment)
#else
void QwtScaleWidget::setLabelAlignment(Qt::Alignment alignment)
#endif
{
    d_data->scaleDraw->setLabelAlignment(alignment);
    layoutScale();
}

/*!
  \brief Change the rotation for the labels.
  See QwtScaleDraw::setLabelRotation().
  \sa QwtScaleDraw::setLabelRotation(), QwtScaleWidget::setLabelFlags()
*/
void QwtScaleWidget::setLabelRotation(double rotation)
{
    d_data->scaleDraw->setLabelRotation(rotation);
    layoutScale();
}

/*!
  \brief Set a scale draw
  sd has to be created with new and will be deleted in
  QwtScaleWidget::~QwtScale or the next call of QwtScaleWidget::setScaleDraw.
*/
void QwtScaleWidget::setScaleDraw(QwtScaleDraw *sd)
{
    if ( sd == NULL || sd == d_data->scaleDraw )
        return;

    if ( d_data->scaleDraw )
        *sd = *d_data->scaleDraw;

    delete d_data->scaleDraw;
    d_data->scaleDraw = sd;

    layoutScale();
}

/*! 
    scaleDraw of this scale
    \sa QwtScaleDraw::setScaleDraw
*/
const QwtScaleDraw *QwtScaleWidget::scaleDraw() const 
{ 
    return d_data->scaleDraw; 
}

/*! 
    scaleDraw of this scale
    \sa QwtScaleDraw::setScaleDraw
*/
QwtScaleDraw *QwtScaleWidget::scaleDraw() 
{ 
    return d_data->scaleDraw; 
}

/*! 
    \return title 
    \sa QwtScaleWidget::setTitle
*/
QwtText QwtScaleWidget::title() const 
{
    return d_data->title;
}

/*! 
    \return start border distance 
    \sa QwtScaleWidget::setBorderDist
*/
int QwtScaleWidget::startBorderDist() const 
{ 
    return d_data->borderDist[0]; 
}  

/*! 
    \return end border distance 
    \sa QwtScaleWidget::setBorderDist
*/
int QwtScaleWidget::endBorderDist() const 
{ 
    return d_data->borderDist[1]; 
}

/*! 
    \return base line distance 
    \sa QwtScaleWidget::setBaselineDist
*/
int QwtScaleWidget::baseLineDist() const 
{ 
    return d_data->baseDist; 
}

/*! 
    \return distance between scale and title
    \sa QwtScaleWidget::setBaselineDist
*/
int QwtScaleWidget::titleDist() const 
{ 
    return d_data->titleDist; 
}

/*!
  \brief paintEvent
*/
void QwtScaleWidget::paintEvent(QPaintEvent *e)
{
    const QRect &ur = e->rect();
    if ( ur.isValid() )
    {
#if QT_VERSION < 0x040000
        QwtPaintBuffer paintBuffer(this, ur);
        draw(paintBuffer.painter());
#else
        QPainter painter(this);
        draw(&painter);
#endif
    }
}

/*!
  \brief draw the scale
*/
void QwtScaleWidget::draw(QPainter *p) const
{
#if QT_VERSION < 0x040000
    d_data->scaleDraw->draw(p, colorGroup());
#else
    d_data->scaleDraw->draw(p, palette());
#endif

    QRect r = rect();
    if ( d_data->scaleDraw->orientation() == Qt::Horizontal )
    {
        r.setLeft(r.left() + d_data->borderDist[0]);
        r.setWidth(r.width() - d_data->borderDist[1]);
    }
    else
    {
        r.setTop(r.top() + d_data->borderDist[1]);
        r.setHeight(r.height() - d_data->borderDist[0]);
    }

    switch(d_data->scaleDraw->alignment())
    {
        case QwtScaleDraw::BottomScale:
            r.setTop( r.top() + d_data->titleOffset );
            break;

        case QwtScaleDraw::LeftScale:
            r.setRight( r.right() - d_data->titleOffset );
            break;

        case QwtScaleDraw::RightScale:
            r.setLeft( r.left() + d_data->titleOffset );
            break;

        case QwtScaleDraw::TopScale:
        default:
            r.setBottom( r.bottom() - d_data->titleOffset );
            break;
    }

    drawTitle(p, d_data->scaleDraw->alignment(), r);
}

/*!
  \brief resizeEvent
*/
void QwtScaleWidget::resizeEvent(QResizeEvent *)
{
    layoutScale(false);
}

//! Recalculate the scale's geometry and layout based on
//  the current rect and fonts.
//  \param update_geometry   notify the layout system and call update
//         to redraw the scale

void QwtScaleWidget::layoutScale( bool update_geometry )
{
    int bd0, bd1;
    getBorderDistHint(bd0, bd1);
    if ( d_data->borderDist[0] > bd0 )
        bd0 = d_data->borderDist[0];
    if ( d_data->borderDist[1] > bd1 )
        bd1 = d_data->borderDist[1];

    const QRect r = rect();
    int x, y, length;

    if ( d_data->scaleDraw->orientation() == Qt::Vertical )
    {
        y = r.top() + bd0;
        length = r.height() - (bd0 + bd1);

        if ( d_data->scaleDraw->alignment() == QwtScaleDraw::LeftScale )
            x = r.right() - d_data->baseDist;
        else
            x = r.left() + d_data->baseDist;
    }
    else
    {
        x = r.left() + bd0; 
        length = r.width() - (bd0 + bd1);

        if ( d_data->scaleDraw->alignment() == QwtScaleDraw::BottomScale )
            y = r.top() + d_data->baseDist;
        else
            y = r.bottom() - d_data->baseDist;
    }

    d_data->scaleDraw->move(x, y);
    d_data->scaleDraw->setLength(length);

    d_data->titleOffset = d_data->baseDist + d_data->titleDist +
        d_data->scaleDraw->extent(QPen(), font());;

    if ( update_geometry )
    {
      updateGeometry();
      update();
    }
}

/*!
  Rotate and paint a title according to its position into a given rectangle.
  \param painter Painter
  \param align Alignment
  \param rect Bounding rectangle
*/

void QwtScaleWidget::drawTitle(QPainter *painter,
    QwtScaleDraw::Alignment align, const QRect &rect) const
{
    QRect r;
    double angle;
    int flags = d_data->title.flags() & 
        ~(Qt::AlignTop | Qt::AlignBottom | Qt::AlignVCenter);

    switch(align)
    {
        case QwtScaleDraw::LeftScale:
            flags |= Qt::AlignTop;
            angle = -90.0;
            r.setRect(rect.left(), rect.bottom(), rect.height(), rect.width());
            break;
        case QwtScaleDraw::RightScale:
            flags |= Qt::AlignTop;
            angle = 90.0;
            r.setRect(rect.right(), rect.top(), rect.height(), rect.width());
            break;
        case QwtScaleDraw::TopScale:
            flags |= Qt::AlignTop;
            angle = 0.0;
            r = rect;
            break;
        case QwtScaleDraw::BottomScale:
        default:
            flags |= Qt::AlignBottom;
            angle = 0.0;
            r = rect;
            break;
    }

    painter->save();
    painter->setFont(font());
#if QT_VERSION < 0x040000
    painter->setPen(colorGroup().color(QColorGroup::Text));
#else
    painter->setPen(palette().color(QPalette::Text));
#endif

    painter->translate(r.x(), r.y());
    if (angle != 0.0)
        painter->rotate(angle);


    QwtText title = d_data->title;
    title.setFlags(flags);
    title.draw(painter, QRect(0, 0, r.width(), r.height()));

    painter->restore();
}

/*!
  \brief Notify a change of the scale

  This virtual function can be overloaded by derived
  classes. The default implementation updates the geometry
  and repaints the widget.
*/

void QwtScaleWidget::scaleChange()
{
    layoutScale();
}

/*!
  \return a size hint
*/
QSize QwtScaleWidget::sizeHint() const
{
    return minimumSizeHint();
}

/*!
  \return a minimum size hint
*/
QSize QwtScaleWidget::minimumSizeHint() const
{
    const Qt::Orientation o = d_data->scaleDraw->orientation();

    // Border Distance cannot be less than the scale borderDistHint
    // Note, the borderDistHint is already included in minHeight/minWidth
    int length = 0;
    int mbd1, mbd2;
    getBorderDistHint(mbd1, mbd2);
    length += qwtMax( 0, d_data->borderDist[0] - mbd1 );
    length += qwtMax( 0, d_data->borderDist[1] - mbd2 );
    length += d_data->scaleDraw->minLength(QPen(), font());

    int dim = dimForLength(length, font());
    if ( length < dim )
    {
        // compensate for long titles
        length = dim;
        dim = dimForLength(length, font());
    }

    QSize size(length + 2, dim);
    if ( o == Qt::Vertical )
        size.transpose();

    return size;
}

/*!
  \brief Find the height of the title for a given width.
  \param width Width
  \return height Height
 */

int QwtScaleWidget::titleHeightForWidth(int width) const
{
    return d_data->title.heightForWidth(width, font());
}

/*!
  \brief Find the minimum dimension for a given length.
         dim is the height, length the width seen in
         direction of the title.
  \param length width for horizontal, height for vertical scales
  \param scaleFont Font of the scale
  \return height for horizontal, width for vertical scales
*/

int QwtScaleWidget::dimForLength(int length, const QFont &scaleFont) const
{
    int dim = d_data->baseDist;
    dim += d_data->scaleDraw->extent(QPen(), scaleFont);

    if ( !d_data->title.isEmpty() )
    {
        dim += titleHeightForWidth(length) + d_data->titleDist;
    }
    return dim;
}

/*!
  \brief Calculate a hint for the border distances.

  This member function calculates the distance
  of the scale's endpoints from the widget borders which
  is required for the mark labels to fit into the widget.
  The maximum of this distance an the minimum border distance
  is returned.

  \warning
  <ul> <li>The minimum border distance depends on the font.</ul>
  \sa setMinBorderDist(), getMinBorderDist(), setBorderDist()
*/
void QwtScaleWidget::getBorderDistHint(int &start, int &end) const
{
    d_data->scaleDraw->getBorderDistHint(font(), start, end);

    if ( start < d_data->minBorderDist[0] )
        start = d_data->minBorderDist[0];

    if ( end < d_data->minBorderDist[1] )
        end = d_data->minBorderDist[1];
}

/*!
  Set a minimum value for the distances of the scale's endpoints from 
  the widget borders. This is useful to avoid that the scales
  are "jumping", when the tick labels or their positions change 
  often.

  \sa getMinBorderDist(), getBorderDistHint()
*/
void QwtScaleWidget::setMinBorderDist(int start, int end)
{
    d_data->minBorderDist[0] = start;
    d_data->minBorderDist[1] = end;
}

/*!
  Get the minimum value for the distances of the scale's endpoints from 
  the widget borders.

  \sa setMinBorderDist(), getBorderDistHint()
*/
void QwtScaleWidget::getMinBorderDist(int &start, int &end) const
{
    start = d_data->minBorderDist[0];
    end = d_data->minBorderDist[1];
}

#if QT_VERSION < 0x040000

/*!
  \brief Notify a change of the font

  This virtual function may be overloaded by derived widgets.
  The default implementation resizes the scale and repaints
  the widget.
  \param oldFont Previous font
*/
void QwtScaleWidget::fontChange(const QFont &oldFont)
{
    QWidget::fontChange( oldFont );
    layoutScale();
}

#endif

/*!
  \brief Assign a scale division

  The scale division determines where to set the tick marks.

  \param transformation Transformation, needed to translate between
                        scale and pixal values
  \param scaleDiv Scale Division
  \sa For more information about scale divisions, see QwtScaleDiv.
*/
void QwtScaleWidget::setScaleDiv(
    const QwtScaleTransformation& transformation,
    const QwtScaleDiv &scaleDiv)
{
    if (d_data->scaleDraw->scaleDiv() != scaleDiv)
    {
        d_data->scaleDraw->setTransformation(transformation);
        d_data->scaleDraw->setScaleDiv(scaleDiv);
        layoutScale();
    }
}
