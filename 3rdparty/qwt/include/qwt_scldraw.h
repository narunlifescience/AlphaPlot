/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *
 *modifications made by Ion Vasilief:
 *the folowing functions have been declared virtual: 
 QRect labelBoundingRect(const QFontMetrics &, double val) const;
 void drawTick(QPainter *p, double val, int len) const;
 virtual void drawBackbone(QPainter *p) const;
 void drawLabel(QPainter *p, double val) const;
 QWMatrix labelWorldMatrix(const QFontMetrics &, const QPoint &, int alignment, double rotation, const QString &) const;
 *****************************************************************************/

#ifndef QWT_SCLDRAW_H
#define QWT_SCLDRAW_H

#include <qstring.h>
#include <qfont.h>
#include <qrect.h>
#include <qcolor.h>
#include "qwt_global.h"
#include "qwt_dimap.h"
#include "qwt_scldiv.h"

class QPainter;

/*!
  \brief A class for drawing scales

  QwtScaleDraw can be used to draw linear or logarithmic scales.
  A scale has an origin,
  an orientation and a length, which all can be specified with
  QwtScaleDraw::setGeometry(). The labels can be rotated and aligned
  to the ticks using QwtScaleDraw::setLabelRotation(), 
  QwtScaleDraw::setLabelAlignment().

  After a scale division has been specified as a QwtScaleDiv object
  using QwtScaleDraw::setScale(const QwtScaleDiv &s),
  or determined internally using QwtScaleDraw::setScale(double vmin, double vmax, int maxMajIntv, int maxMinIntv, double step = 0.0, int logarithmic = 0),
  the scale can be drawn with the QwtScaleDraw::draw() member.
*/
class QWT_EXPORT QwtScaleDraw: public QwtDiMap
{
public:
    /*! 
        Orientation/Position of the scale draw
        \sa QwtScaleDraw::setGeometry
     */
    enum Orientation { Bottom, Top, Left, Right, Round };
    enum Option { None = 0, Backbone = 1 };
 
    QwtScaleDraw();
    virtual ~QwtScaleDraw();
    
    void setScale(const QwtScaleDiv &s);
    void setScale(double vmin, double vmax, int maxMajIntv, int maxMinIntv,
                  double step = 0.0, int logarithmic = 0);

    void setOptions(int opt);
    int options() const;

    void setGeometry(int xorigin, int yorigin, int length, Orientation o);
    int x() const;
    int y() const;
    int length() const;
    Orientation orientation() const;

    void setAngleRange(double angle1, double angle2);
    void setLabelFormat(char f, int prec, int fieldwidth = 0);
    void setLabelAlignment(int align);
    void setLabelRotation(double rotation);
    void setTickLength(unsigned int minLen, 
        unsigned int medLen, unsigned int majLen);

    void setMargin(uint hMargin, uint vMargin);

    /*! \return scale division */
    const QwtScaleDiv& scaleDiv() const { return d_scldiv; }
    int maxWidth(const QPen &pen, const QFontMetrics &fm) const;
    int maxHeight(const QPen &pen, const QFontMetrics &fm) const;

    void minBorderDist(const QFontMetrics &fm, int &start, int &end) const;
    int minLabelDist(const QFontMetrics &fm) const;

    int minHeight(const QPen &pen, const QFontMetrics &fm) const; 
    int minWidth(const QPen &pen, const QFontMetrics &fm) const; 
    void labelFormat(char &f, int &prec, int &fieldwidth) const;
    int labelAlignment() const;
    double labelRotation() const;
    void tickLength(unsigned int &minLen, 
        unsigned int &medLen, unsigned int &majLen) const;
    unsigned int majTickLength() const;
        
    virtual void draw(QPainter *p) const;
    virtual QString label(double) const;

protected:
    virtual void labelPlacement( const QFontMetrics &, double val,
        QPoint &pos, int &alignment, double &rotation) const;
    virtual int maxLabelHeight(const QFontMetrics &) const;
    virtual int maxLabelWidth(const QFontMetrics &) const;

	/*The following functions have been declared virtual by Ion Vasilief ****************/
    virtual QRect labelBoundingRect(
        const QFontMetrics &, double val) const;

    virtual void drawTick(QPainter *p, double val, int len) const;
    virtual void drawBackbone(QPainter *p) const;
    virtual void drawLabel(QPainter *p, double val) const;
    virtual QWMatrix labelWorldMatrix(const QFontMetrics &,
        const QPoint &, int alignment, double rotation, 
        const QString &) const;
        
private:
    int d_options;

    QwtScaleDiv d_scldiv;
    Orientation d_orient;
        
    int d_xorg;
    int d_yorg;
    int d_len;
        
    unsigned int d_hpad;
    unsigned int d_vpad;
        
    unsigned int d_medLen;
    unsigned int d_majLen;
    unsigned int d_minLen;

    int d_minAngle;
    int d_maxAngle;

    double d_xCenter;
    double d_yCenter;
    double d_radius;

    char d_fmt;
    int d_prec;
    int d_fieldwidth;

    int d_labelAlignment;
    double d_labelRotation;

    // d_formatBuffer must be able to hold:
    // - a percent sign a '%'
    // - a two digit positive fieldwidth
    // - a point
    // - a two digit positive precision
    // - a format character ('e', 'f', 'g', 'E', 'F, 'G')
    // - a terminating '/0' character 
    char d_formatBuffer[8];
};

#endif
