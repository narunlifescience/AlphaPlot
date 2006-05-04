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
#include "qwt_scale.h"
#include "qwt_math.h"
#include "qwt_paint_buffer.h"
#include "qwt_text.h"

/*!
  \brief Create a scale with the position QwtScale::Left
  \param parent
  \param name passed to QWidget's CTOR

  \warning Workaround of a designer problem
*/
QwtScale::QwtScale(QWidget *parent, const char *name):
    QWidget(parent, name, Qt::WRepaintNoErase|Qt::WResizeNoErase)
{
    initScale(QwtScale::Left);
}

/*!
  \brief Constructor
  \param pos Position. Must be one of
             <code>QwtScale::Left, QwtScale::Right, QwtScale::Bottom,
             </code>or \c QwtScale::Top
  \param parent
  \param name passed to QWidget's CTOR
*/
QwtScale::QwtScale(QwtScale::Position pos, QWidget *parent, const char *name):
    QWidget(parent,name, Qt::WRepaintNoErase|Qt::WResizeNoErase)
{
    initScale(pos);
}

//! Destructor
QwtScale::~QwtScale()
{
    delete d_scaleDraw;
    delete d_title;
}

//! Initialize the scale
void QwtScale::initScale(QwtScale::Position pos)
{
    QwtScaleDraw::Orientation sdo;
    d_borderDist[0] = 0;
    d_borderDist[1] = 0;
    d_baseDist = 4;
    d_titleOffset = 0;
    d_titleDist = 2;

    switch(pos)
    {
        case QwtScale::Left:
            sdo = QwtScaleDraw::Left;
            break;
        case QwtScale::Right:
            sdo = QwtScaleDraw::Right;
            break;
        case QwtScale::Bottom:
            sdo = QwtScaleDraw::Bottom;
            break;
        case QwtScale::Top:
            sdo = QwtScaleDraw::Top;
            break;
        default:
            sdo = QwtScaleDraw::Left;
    }

    d_scaleDraw = new QwtScaleDraw;
    d_scaleDraw->setGeometry(0, 0, 10, sdo);

    d_title = new QwtPlainText( QString::null, font(), 
                Qt::AlignHCenter | Qt::WordBreak | Qt::ExpandTabs);
}

/*!
  \brief Give title new text contents
  \param text New text
  \sa QwtScale::title
*/
void QwtScale::setTitle(const QString &text)
{
    if (text == title())
        return;

    QwtText *title = QwtText::makeText(text, d_title->alignment(),
        d_title->font(), d_title->color());

    delete d_title;
    d_title = title;

    layoutScale();
}

/*!
  \brief Assign a position
  \param pos new position
  \sa QwtScale::position
*/
void QwtScale::setPosition(Position pos)
{
    QwtScaleDraw::Orientation sdo;
    switch(pos)
    {
        case QwtScale::Left:
            sdo = QwtScaleDraw::Left;
            break;
        case QwtScale::Right:
            sdo = QwtScaleDraw::Right;
            break;
        case QwtScale::Bottom:
            sdo = QwtScaleDraw::Bottom;
            break;
        case QwtScale::Top:
            sdo = QwtScaleDraw::Top;
            break;
        default:
            sdo = QwtScaleDraw::Left;
    }
    
    if (d_scaleDraw)
        d_scaleDraw->setGeometry(0,0,10,sdo);
    layoutScale();
}

        
/*! 
    \return position 
    \sa QwtScale::setPosition
*/
QwtScale::Position QwtScale::position() const 
{
    if (!scaleDraw())
        return Left;
    
    QwtScaleDraw::Orientation ori = scaleDraw()->orientation();
    switch(ori)
    {
        case QwtScaleDraw::Left:
            return QwtScale::Left;
        case QwtScaleDraw::Right:
            return QwtScale::Right;
        case QwtScaleDraw::Bottom:
            return QwtScale::Bottom;
        case QwtScaleDraw::Top:
            return QwtScale::Top;
        default:
            return QwtScale::Left;
    }
}

/*!
  Specify distances of the scale's endpoints from the
  widget's borders. The actual borders will never be less
  than minimum border distance.
  \param dist1 Left or top Distance
  \param dist2 Right or bottom distance
  \sa QwtScale::borderDist
*/
void QwtScale::setBorderDist(int dist1, int dist2)
{
    if ( dist1 != d_borderDist[0] || dist2 != d_borderDist[1] )
    {
        d_borderDist[0] = dist1;
        d_borderDist[1] = dist2;
        layoutScale();
    }
}

/*!
  \brief Specify the border distance of the scale's base line.
  \param bd border distance of the baseline
  \sa QwtScale::baseLineDist
*/
void QwtScale::setBaselineDist(int bd)
{
    bd = qwtMax( 0, bd );
    if ( bd != d_baseDist )
    {
        d_baseDist = bd;
        layoutScale();
    }
}

/*!
  \brief Specify the distance between scale and title
  \param td Title distance
  \sa QwtScale::titleDist
*/
void QwtScale::setTitleDist(int td)
{
    td = qwtMax( 0, td );
    if ( td != d_titleDist )
    {
        d_titleDist = td;
        layoutScale();
    }
}


/*!
  \brief Change the title font
  \param f new font
  \sa QwtScale::title
*/
void QwtScale::setTitleFont(const QFont &f)
{
    if (f == d_title->font())
        return;

    d_title->setFont(f);
    layoutScale();
}

/*!
  \brief Change the title color
  \param c new title color
  \sa QwtScale::titleColor
*/
void QwtScale::setTitleColor(const QColor &c)
{
    d_title->setColor(c);
    update();
}

/*!
  \brief Change the title alignment
  \param flags see Qt::AlignmentFlags. flags are interpreted in
               direction of the label, AlignTop, AlignBottom can't be set
               as the title will always be aligned to the scale.
  \sa QwtScale::titleAlignment, Qt::AlignmentFlags

*/
void QwtScale::setTitleAlignment(int flags)
{
    flags &= ~(Qt::AlignTop | Qt::AlignBottom);
    d_title->setAlignment(flags);
    layoutScale(); // Qt::SingleLine, Qt::ExpandTabs might affect layout
}

/*!
  \brief Set the number format for the major scale labels

  Format character and precision have the same meaning as for the
  QString class.
  \param f  format character
  \param prec precision
  \param fieldwidth minimum fieldwidth
  \sa QwtScale::labelFormat, QString::sprintf in the Qt manual
*/

void QwtScale::setLabelFormat(char f, int prec, int fieldwidth)
{
    d_scaleDraw->setLabelFormat(f, prec, fieldwidth);
    layoutScale();
}

/*!
  \brief Change the alignment for the labels.

  See QwtScaleDraw::setLabelAlignment().
  \sa QwtScaleDraw::setLabelAlignment(), QwtScale::setLabelRotation()
*/

void QwtScale::setLabelAlignment(int align)
{
    d_scaleDraw->setLabelAlignment(align);
    layoutScale();
}

/*!
  \brief Change the rotation for the labels.
  See QwtScaleDraw::setLabelRotation().
  \sa QwtScaleDraw::setLabelRotation(), QwtScale::setLabelAlignment()
*/
void QwtScale::setLabelRotation(double rotation)
{
    d_scaleDraw->setLabelRotation(rotation);
    layoutScale();
}

/*!
  \brief Set a scale draw
  sd has to be created with new and will be deleted in
  QwtScale::~QwtScale or the next call of QwtScale::setScaleDraw.
*/
void QwtScale::setScaleDraw(QwtScaleDraw *sd)
{
    if ( sd == NULL || sd == d_scaleDraw )
        return;

    if ( d_scaleDraw )
        *sd = *d_scaleDraw;

    delete d_scaleDraw;
    d_scaleDraw = sd;

    layoutScale();
}

/*!
  Format character and precision have the same meaning as for the
  QString class.
  \retval f format character
  \retval prec precision
  \retval fieldwidth minimum fieldwidth
  \return the number format for the major scale labels
  \sa QwtScale::setLabelFormat, QString::sprintf in the Qt manual
*/

void QwtScale::labelFormat(char &f, int &prec, int &fieldwidth) const
{
    d_scaleDraw->labelFormat(f, prec, fieldwidth);
}

/*! 
    scaleDraw of this scale
    \sa QwtScaleDraw::setScaleDraw
*/
const QwtScaleDraw *QwtScale::scaleDraw() const 
{ 
    return d_scaleDraw; 
}

/*! 
    \return alignment
    \sa QwtScale::setTitleAlignment, Qt::AlignmentFlags
*/
int QwtScale::titleAlignment() const 
{ 
    return d_title->alignment();
}

/*! 
    \return title font
    \sa QwtScale::setTitleFont
*/
const QFont QwtScale::titleFont() const 
{
    return d_title->font();
}

/*! 
    \return title color
    \sa QwtScale::setTitleColor
*/
const QColor QwtScale::titleColor() const 
{
    return d_title->color();
}

/*! 
    \return title 
    \sa QwtScale::setTitle
*/
const QString QwtScale::title() const 
{
    return d_title->text();
}

/*! 
    \return start border distance 
    \sa QwtScale::setBorderDist
*/
int QwtScale::startBorderDist() const 
{ 
    return d_borderDist[0]; 
}  

/*! 
    \return end border distance 
    \sa QwtScale::setBorderDist
*/
int QwtScale::endBorderDist() const 
{ 
    return d_borderDist[1]; 
}

/*! 
    \return base line distance 
    \sa QwtScale::setBaselineDist
*/
int QwtScale::baseLineDist() const 
{ 
    return d_baseDist; 
}

/*! 
    \return distance between scale and title
    \sa QwtScale::setBaselineDist
*/
int QwtScale::titleDist() const 
{ 
    return d_titleDist; 
}

/*!
  \brief paintEvent
*/
void QwtScale::paintEvent(QPaintEvent *e)
{
    const QRect &ur = e->rect();
    if ( ur.isValid() )
    {
        QwtPaintBuffer paintBuffer(this, ur);
        draw(paintBuffer.painter());
    }
}

/*!
  \brief draw the scale
*/
void QwtScale::draw(QPainter *p) const
{
    d_scaleDraw->draw(p);

    QRect r = rect();

    switch(d_scaleDraw->orientation())
    {
        case QwtScaleDraw::Bottom:
            r.setTop( r.top() + d_titleOffset );
            break;

        case QwtScaleDraw::Left:
            r.setRight( r.right() - d_titleOffset );
            break;

        case QwtScaleDraw::Right:
            r.setLeft( r.left() + d_titleOffset );
            break;

        case QwtScaleDraw::Top:
        default:
            r.setBottom( r.bottom() - d_titleOffset );
            break;
    }

    drawTitle(p, d_scaleDraw->orientation(), r);
}

/*!
  \brief resizeEvent
*/
void QwtScale::resizeEvent(QResizeEvent *)
{
    layoutScale(FALSE);
}

//! Recalculate the scale's geometry and layout based on
//  the current rect and fonts.
//  \param update_geometry   notify the layout system and call update
//         to redraw the scale

void QwtScale::layoutScale( bool update_geometry )
{
    QRect r = this->rect();

    // Borders cannot be less than the minBorderDist

    int bd0, bd1;
    minBorderDist(bd0, bd1);
    if ( d_borderDist[0] > bd0 )
        bd0 = d_borderDist[0];
    if ( d_borderDist[1] > bd1 )
        bd1 = d_borderDist[1];

    const QFontMetrics fm(font());
    switch(d_scaleDraw->orientation())
    {
    case QwtScaleDraw::Bottom:
        d_scaleDraw->setGeometry(r.x() + bd0, r.y() + d_baseDist,
            r.width() - bd0 - bd1, QwtScaleDraw::Bottom);

        d_titleOffset = d_titleDist + d_baseDist +
            d_scaleDraw->maxHeight(QPen(), fm);
        break;

    case QwtScaleDraw::Top:
        d_scaleDraw->setGeometry(r.x() + bd0, r.bottom() - d_baseDist,
               r.width() - bd0 - bd1, QwtScaleDraw::Top);
        d_titleOffset =  d_titleDist + d_baseDist +
                           d_scaleDraw->maxHeight(QPen(), fm);
        break;

    case QwtScaleDraw::Left:
        d_scaleDraw->setGeometry(r.right() - d_baseDist, r.y() + bd0,
                            r.height() - bd0 - bd1, QwtScaleDraw::Left);
        d_titleOffset = d_baseDist + d_titleDist +
                        d_scaleDraw->maxWidth(QPen(), fm);
        break;

    case QwtScaleDraw::Right:
        d_scaleDraw->setGeometry(r.x() + d_baseDist, r.y() + bd0,
                            r.height() - bd0 - bd1, QwtScaleDraw::Right);
        d_titleOffset = d_baseDist + d_titleDist +
                          d_scaleDraw->maxWidth(QPen(), fm);
        break;
    default:
        break;
    }
    if ( update_geometry )
    {
      updateGeometry();
      update();
    }
}

/*!
  Rotate and paint a title according to its position into a given rectangle.
  \param painter Painter
  \param o Orientation
  \param rect Bounding rectangle
*/

void QwtScale::drawTitle(QPainter *painter,
    QwtScaleDraw::Orientation o, const QRect &rect) const
{
    QRect r;
    double angle;
    int align = d_title->alignment() & ~(Qt::AlignTop | Qt::AlignBottom);

    switch(o)
    {
#ifndef QT_NO_TRANSFORMATIONS
        case QwtScaleDraw::Left:
            align |= Qt::AlignTop;
            angle = -90.0;
            r.setRect(rect.left(), rect.bottom(), rect.height(), rect.width());
            break;
        case QwtScaleDraw::Right:
            align |= Qt::AlignTop;
            angle = 90.0;
            r.setRect(rect.right(), rect.top(), rect.height(), rect.width());
            break;
#else
        case QwtScaleDraw::Left:
        case QwtScaleDraw::Right:
            // swallow vertical titles
            return;
#endif
        case QwtScaleDraw::Top:
            align |= Qt::AlignTop;
            angle = 0.0;
            r = rect;
            break;
        case QwtScaleDraw::Bottom:
        default:
            align |= Qt::AlignBottom;
            angle = 0.0;
            r = rect;
            break;
    }

    d_title->setAlignment(align);

    painter->save();
    painter->translate(r.x(), r.y());
#ifndef QT_NO_TRANSFORMATIONS
    if (angle != 0.0)
        painter->rotate(angle);
#endif
    d_title->draw(painter, QRect(0, 0, r.width(), r.height()));

    // Users do not expect to see AlignTop or AlignBottom
    d_title->setAlignment(align & ~(Qt::AlignTop | Qt::AlignBottom));
    painter->restore();
}

/*!
  \brief Notify a change of the scale

  This virtual function can be overloaded by derived
  classes. The default implementation updates the geometry
  and repaints the widget.
*/

void QwtScale::scaleChange()
{
    layoutScale();
}

/*!
  \return Fixed/MinimumExpanding for vertical,
          MinimumExpanding/Fixed for horizontal scales.
*/
QSizePolicy QwtScale::sizePolicy() const
{
    QSizePolicy sp;
    if ( d_scaleDraw->orientation() == QwtScaleDraw::Left ||
        d_scaleDraw->orientation() == QwtScaleDraw::Right )
    {
        sp.setHorData( QSizePolicy::Fixed );
        sp.setVerData( QSizePolicy::MinimumExpanding );
    }
    else
    {
        sp.setHorData( QSizePolicy::MinimumExpanding );
        sp.setVerData( QSizePolicy::Fixed );
    }
    return sp;
}

/*!
  \return a size hint
*/
QSize QwtScale::sizeHint() const
{
    return minimumSizeHint();
}

/*!
  \return a minimum size hint
*/
QSize QwtScale::minimumSizeHint() const
{
    Qt::Orientation o = Qt::Horizontal;
    if ( d_scaleDraw->orientation() == QwtScaleDraw::Left ||
        d_scaleDraw->orientation() == QwtScaleDraw::Right )
    {
        o = Qt::Vertical;
    }

    // Border Distance cannot be less than the scale minBorderDist
    // Note, the minBorderDist is already included in minHeight/minWidth
    int length = 0;
    int mbd1, mbd2;
    minBorderDist(mbd1, mbd2);
    length += qwtMax( 0, d_borderDist[0] - mbd1 );
    length += qwtMax( 0, d_borderDist[1] - mbd2 );

    const QFontMetrics fm(font());
    if ( o == Qt::Vertical )
        length += d_scaleDraw->minHeight(QPen(), fm);
    else
        length += d_scaleDraw->minWidth(QPen(), fm);

    int dim = dimForLength(length, fm);
    if ( length < dim )
    {
        // compensate for long titles
        length = dim;
        dim = dimForLength(length, fm);
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

int QwtScale::titleHeightForWidth(int width) const
{
    return d_title->heightForWidth(width);
}

/*!
  \brief Find the minimum dimension for a given length.
         dim is the height, length the width seen in
         direction of the title.
  \param length width for horizontal, height for vertical scales
  \param scaleFontMetrics Font metrics of the scale
  \return height for horizontal, width for vertical scales
*/

int QwtScale::dimForLength(int length,
    const QFontMetrics &scaleFontMetrics) const
{
    int dim = d_baseDist;
    if ( d_scaleDraw->orientation() == QwtScaleDraw::Left ||
        d_scaleDraw->orientation() == QwtScaleDraw::Right )
    {
        dim += d_scaleDraw->minWidth(QPen(), scaleFontMetrics);
#ifdef QT_NO_TRANSFORMATIONS
        return dim;
#endif
    }
    else
        dim += d_scaleDraw->minHeight(QPen(), scaleFontMetrics);

    if ( !d_title->text().isEmpty() )
    {
        dim += titleHeightForWidth(length) + d_titleDist;
    }
    return dim;
}

/*!
  \brief Determine the minimum Border distances.

  This member function returns the minimum distance
  of the scale's endpoints from the widget borders which
  is required for the mark labels to fit into the widget.
  \warning
  <ul> <li>The minimum border distance depends on the font.</ul>
  \sa QwtScale::setBorderDist()
*/
void QwtScale::minBorderDist(int &start, int &end) const
{
    const QFontMetrics fm(font());
    d_scaleDraw->minBorderDist(fm, start, end);
}

/*!
  \brief Notify a change of the font

  This virtual function may be overloaded by derived widgets.
  The default implementation resizes the scale and repaints
  the widget.
  \param oldFont Previous font
*/
void QwtScale::fontChange(const QFont &oldFont)
{
    QWidget::fontChange( oldFont );
    layoutScale();
}

/*!
  \brief Assign a scale division

  The scale division determines where to set the tick
  marks.
  \param sd Scale Division
  \sa For more information about scale divisions, see QwtScaleDiv.
*/
void QwtScale::setScaleDiv(const QwtScaleDiv &sd)
{
    if (d_scaleDraw->scaleDiv() != sd)
    {
        d_scaleDraw->setScale(sd);
        layoutScale();
    }
}

// Local Variables:
// mode: C++
// c-file-style: "stroustrup"
// indent-tabs-mode: nil
// End:
