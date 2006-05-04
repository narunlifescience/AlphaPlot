/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

// vim: expandtab

#ifndef QWT_SCALE_H
#define QWT_SCALE_H

#include <qwidget.h>
#include <qfont.h>
#include <qcolor.h>
#include <qstring.h>

#include "qwt_global.h"
#include "qwt_scldiv.h"
#include "qwt_scldraw.h"

class QPainter;
class QwtText;

/*!
  \brief A Widget which contains a scale

  This Widget can be used to decorate composite widgets with
  a scale.
*/

class QWT_EXPORT QwtScale : public QWidget
{
    Q_OBJECT 
    Q_ENUMS(Position)
    Q_PROPERTY( Position position READ position WRITE setPosition )
    Q_PROPERTY( QString title READ title WRITE setTitle )
    Q_PROPERTY( QFont titleFont READ titleFont WRITE setTitleFont )
    Q_PROPERTY( QColor titleColor READ titleColor WRITE setTitleColor )

public:
    /*! 
        Position of the scale
        \sa QwtScale::QwtScale
     */
    
    enum Position { Left, Right, Top, Bottom };

    QwtScale(QWidget *parent = 0, const char *name = 0);
    QwtScale(Position, QWidget *parent = 0, const char *name = 0);
    virtual ~QwtScale();

    void setTitle(const QString &title);
    const QString title() const;

    void setTitleColor(const QColor &c);
    const QColor titleColor() const;

    void setTitleFont(const QFont &f);
    const QFont titleFont() const;

    void setTitleAlignment(int flags);
    int titleAlignment() const;

    void setBorderDist(int start, int end);
    int startBorderDist() const;
    int endBorderDist() const;
    void minBorderDist(int &start, int &end) const;

    void setBaselineDist(int bd);
    int baseLineDist() const;

    void setTitleDist(int td);
    int titleDist() const;

    void setScaleDiv(const QwtScaleDiv &sd);
    void setScaleDraw(QwtScaleDraw *);
    const QwtScaleDraw *scaleDraw() const;

    void setLabelFormat(char f, int prec, int fieldWidth);
    void labelFormat(char &f, int &prec, int &fieldWidth) const;

    void setLabelAlignment(int align);
    void setLabelRotation(double rotation);

    virtual QSize sizeHint() const;
    virtual QSize minimumSizeHint() const;
    virtual QSizePolicy sizePolicy() const;

    int titleHeightForWidth(int width) const;
    int dimForLength(int length, const QFontMetrics &scaleFontMetrics ) const;

    void drawTitle(QPainter *painter, QwtScaleDraw::Orientation o,
        const QRect &rect) const;
        
    void setPosition(Position o);
    Position position() const;

protected:
    void draw(QPainter *p) const;
    void paintEvent(QPaintEvent *e);
    void resizeEvent(QResizeEvent *e);
    void scaleChange();
    void fontChange(const QFont &oldfont);
    void layoutScale( bool update = TRUE );

private:
    void initScale(Position);

    QwtScaleDraw *d_scaleDraw;

    int d_borderDist[2];
    int d_scaleLength;
    int d_baseDist;

    int d_titleOffset;
    int d_titleDist;
    QwtText *d_title;
};

#endif
