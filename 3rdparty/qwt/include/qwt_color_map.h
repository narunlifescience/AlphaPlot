/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#ifndef QWT_COLOR_MAP_H
#define QWT_COLOR_MAP_H

#include <qglobal.h>
#include <qcolor.h>
#if QT_VERSION < 0x040000
#include <qvaluevector.h>
#else
#include <qvector.h>
#endif
#include "qwt_double_interval.h"

/*!
  QwtColorMap is can be used to map values into colors. 
  It is useful for displaying spectrograms.

  Each color map is optimized to return colors for only one of the
  following image formats:
  - QImage::Format_Indexed8\n
  - QImage::Format_ARGB32\n

  \sa QwtPlotSpectrogram, QwtScaleWidget
*/

class QWT_EXPORT QwtColorMap
{
public:
    /*! 
        - RGB\n
        The map is intended to map into QRgb values.
        - Indexed\n
        The map is intended to map into 8 bit values, that
        are indices into the color table.

        \sa rgb(), colorIndex(), colorTable()
    */

    enum Format
    {
        RGB,
        Indexed
    };

    QwtColorMap(Format = QwtColorMap::RGB );
    virtual ~QwtColorMap();

    inline Format format() const;

    //! Clone the color map
    virtual QwtColorMap *copy() const = 0;

    //!  Map a value of a given interval into a rgb value.
    virtual QRgb rgb(const QwtDoubleInterval &, double value) const = 0;

    //!  Map a value of a given interval into a color index
    virtual unsigned char colorIndex(
        const QwtDoubleInterval &, double value) const = 0;

    QColor color(const QwtDoubleInterval &, double value) const;
#if QT_VERSION < 0x040000
    virtual QValueVector<QRgb> colorTable(const QwtDoubleInterval &) const;
#else
    virtual QVector<QRgb> colorTable(const QwtDoubleInterval &) const;
#endif

private:
    Format d_format;
};


/*!
  QwtLinearColorMap builds a color map from 2 colors
*/
class QWT_EXPORT QwtLinearColorMap: public QwtColorMap
{
public:
    enum Mode
    {
        FixedColors,
        ScaledColors
    };

    QwtLinearColorMap(QwtColorMap::Format = QwtColorMap::RGB);
    QwtLinearColorMap( const QColor &from, const QColor &to,
        QwtColorMap::Format = QwtColorMap::RGB);

    QwtLinearColorMap(const QwtLinearColorMap &);

    virtual ~QwtLinearColorMap();

    QwtLinearColorMap &operator=(const QwtLinearColorMap &);

    virtual QwtColorMap *copy() const;

    void setMode(Mode);
    Mode mode() const;

    void setColorInterval(const QColor &color1, const QColor &color2);
    void addColorStop(double value, const QColor&);

    QColor color1() const;
    QColor color2() const;

    virtual QRgb rgb(const QwtDoubleInterval &, double value) const;
    virtual unsigned char colorIndex(
        const QwtDoubleInterval &, double value) const;

    class ColorStops;

private:
    class PrivateData;
    PrivateData *d_data;
};

/*!
   Map a value into a color

   \param interval Valid interval for values
   \param value Value

   \return Color corresponding to value

   \warning This method is slow for Indexed color maps. If it is
            necessary to map many values, its better to get the
            color table once and find the color using colorIndex().
*/
inline QColor QwtColorMap::color(
    const QwtDoubleInterval &interval, double value) const
{
    if ( d_format == RGB )
    {
        return QColor( rgb(interval, value) );
    }
    else
    {
        const unsigned int index = colorIndex(interval, value);
        return colorTable(interval)[index]; // slow
    }
}

/*!
   \return Intended format of the color map
   \sa Format
*/
inline QwtColorMap::Format QwtColorMap::format() const
{
    return d_format;
}

#endif
