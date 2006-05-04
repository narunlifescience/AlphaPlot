/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#ifndef QWT_DOUBLE_RECT_H
#define QWT_DOUBLE_RECT_H 1

#include <qpoint.h>
#include <qsize.h>
#include <qrect.h>
#include "qwt_global.h"

/*!
  The QwtDoublePoint class defines a point in double coordinates
*/

class QWT_EXPORT QwtDoublePoint
{
public:
    QwtDoublePoint();
    QwtDoublePoint(double x, double y);
    QwtDoublePoint(const QPoint &);

    inline bool isNull()    const;

    inline double x() const;
    inline double y() const;
    inline void setX(double x);
    inline void setY(double y);

    inline double &rx();
    inline double &ry();

    bool operator==(const QwtDoublePoint &) const;
    bool operator!=(const QwtDoublePoint &) const;

    const QwtDoublePoint operator-() const;
    const QwtDoublePoint operator+(const QwtDoublePoint &) const;
    const QwtDoublePoint operator-(const QwtDoublePoint &) const;
    const QwtDoublePoint operator*(double) const;
    const QwtDoublePoint operator/(double) const;

    QwtDoublePoint &operator+=(const QwtDoublePoint &);
    QwtDoublePoint &operator-=(const QwtDoublePoint &);
    QwtDoublePoint &operator*=(double);
    QwtDoublePoint &operator/=(double);

private:
    double d_x;
    double d_y;
};

/*!
  The QwtDoubleSize class defines a size in double coordinates
*/

class QWT_EXPORT QwtDoubleSize
{
public:
    QwtDoubleSize();
    QwtDoubleSize(double width, double height);
    QwtDoubleSize(const QSize &);

    inline bool isNull() const;
    inline bool isEmpty() const;
    inline bool isValid() const;

    inline double width() const;
    inline double height() const;
    inline void setWidth( double w );
    inline void setHeight( double h );
    void transpose();

    QwtDoubleSize expandedTo(const QwtDoubleSize &) const;
    QwtDoubleSize boundedTo(const QwtDoubleSize &) const;

    inline double &rwidth();
    inline double &rheight();

    bool operator==(const QwtDoubleSize &) const;
    bool operator!=(const QwtDoubleSize &) const;

    const QwtDoubleSize operator-() const;
    const QwtDoubleSize operator+(const QwtDoubleSize &) const;
    const QwtDoubleSize operator-(const QwtDoubleSize &) const;
    const QwtDoubleSize operator*(double) const;
    const QwtDoubleSize operator/(double) const;

    QwtDoubleSize &operator+=(const QwtDoubleSize &);
    QwtDoubleSize &operator-=(const QwtDoubleSize &);
    QwtDoubleSize &operator*=(double c);
    QwtDoubleSize &operator/=(double c);

private:
    double d_width;
    double d_height;
};

/*!
  The QwtDoubleRect class defines a size in double coordinates.
*/

class QWT_EXPORT QwtDoubleRect  
{
public:
    QwtDoubleRect();
    QwtDoubleRect(double x1, double x2, double y1, double y2);
    QwtDoubleRect(double x, double y, const QwtDoubleSize &);

    inline bool isNull()    const;
    inline bool isEmpty()   const;
    inline bool isValid()   const;

    QwtDoubleRect normalize() const;

    inline double x1()  const;
    inline double x2()  const;
    inline double y1()  const;
    inline double y2()  const;

    inline double &rx1();
    inline double &rx2();
    inline double &ry1();
    inline double &ry2();

    inline void setX1(double);
    inline void setX2(double);
    inline void setY1(double);
    inline void setY2(double);

    QwtDoublePoint center()  const;

    void moveX(double x);
    void moveY(double y );
    void move(double x, double y);
    void moveBy(double dx, double dy);
    void moveCenter(const QwtDoublePoint &);
    void moveCenter(double dx, double dy);

    void setRect(double x1, double x2, double y1, double y2);

    inline double width()   const;
    inline double height()  const;
    inline QwtDoubleSize size() const;

    inline void setWidth(double w );
    inline void setHeight(double h );
    void setSize(const QwtDoubleSize &);

    QwtDoubleRect  operator|(const QwtDoubleRect &r) const;
    QwtDoubleRect  operator&(const QwtDoubleRect &r) const;
    QwtDoubleRect &operator|=(const QwtDoubleRect &r);
    QwtDoubleRect &operator&=(const QwtDoubleRect &r);
    bool operator==( const QwtDoubleRect &) const;
    bool operator!=( const QwtDoubleRect &) const;

    bool contains(const QwtDoublePoint &p, bool proper = FALSE) const;
    bool contains(double x, double y, bool proper = FALSE) const; 
    bool contains(const QwtDoubleRect &r, bool proper=FALSE) const;

    QwtDoubleRect unite(const QwtDoubleRect &) const;
    QwtDoubleRect intersect(const QwtDoubleRect &) const;
    bool intersects(const QwtDoubleRect &) const;

private:
    double d_x1;
    double d_x2;
    double d_y1;
    double d_y2;
};

/*! 
  Returns TRUE if both the x value and the y value are 0; 
  otherwise returns FALSE. 
*/
inline bool QwtDoublePoint::isNull() const
{ 
    return d_x == 0.0 && d_y == 0.0; 
}

//! Returns the x coordinate of the point. 
inline double QwtDoublePoint::x() const
{ 
    return d_x; 
}

//! Returns the y coordinate of the point. 
inline double QwtDoublePoint::y() const
{   
    return d_y; 
}

//! Sets the x coordinate of the point to x. 
inline void QwtDoublePoint::setX(double x)
{ 
    d_x = x; 
}

//! Sets the y coordinate of the point to x. 
inline void QwtDoublePoint::setY(double y)
{ 
    d_y = y; 
}

//! Returns a reference to the x coordinate of the point. 
inline double &QwtDoublePoint::rx()
{ 
    return d_x; 
}

//! Returns a reference to the y coordinate of the point. 
inline double &QwtDoublePoint::ry()
{ 
    return d_y; 
}

/*!
  Returns TRUE if the width is 0 and the height is 0; 
  otherwise returns FALSE.
*/
inline bool QwtDoubleSize::isNull() const
{ 
    return d_width == 0.0 && d_height == 0.0; 
}

/*! 
  Returns TRUE if the width is <= 0 or the height is <= 0, 
  otherwise FALSE. 
*/
inline bool QwtDoubleSize::isEmpty() const
{ 
    return d_width <= 0.0 || d_height <= 0.0; 
}

/*!
  Returns TRUE if the width is equal to or greater than 0 and the height 
  is equal to or greater than 0; otherwise returns FALSE.
*/
inline bool QwtDoubleSize::isValid() const
{ 
    return d_width >= 0.0 && d_height >= 0.0; 
}

//! Returns the width. 
inline double QwtDoubleSize::width() const
{ 
    return d_width; 
}

//! Returns the height. 
inline double QwtDoubleSize::height() const
{ 
    return d_height; 
}

//! Sets the width to width. 
inline void QwtDoubleSize::setWidth(double width)
{ 
    d_width = width; 
}

//! Sets the height to height. 
inline void QwtDoubleSize::setHeight(double height)
{ 
    d_height = height; 
}

//! Returns a reference to the width. 
inline double &QwtDoubleSize::rwidth()
{ 
    return d_width; 
}

//! Returns a reference to the height. 
inline double &QwtDoubleSize::rheight()
{   
    return d_height; 
}

/*!
  Returns TRUE if the rectangle is a null rectangle; 
  otherwise returns FALSE. 

  A rect is null when x1() == x2() && y1() == y2().
*/
inline bool QwtDoubleRect::isNull() const
{ 
    return d_x2 == d_x1 && d_y2 == d_y1;
}

/*!
  Returns TRUE if the rectangle is a empty rectangle; 
  otherwise returns FALSE. 

  A rect is null when x1() >= x2() && y1() >= y2().
*/
inline bool QwtDoubleRect::isEmpty() const
{ 
    return d_x1 >= d_x2 || d_y1 >= d_y2; 
}

/*!
  Returns TRUE if the rectangle is a valid rectangle; 
  otherwise returns FALSE. 

  A rect is valid when x1() <= x2() && y1() <= y2().
*/
inline bool QwtDoubleRect::isValid() const
{ 
    return d_x1 <= d_x2 && d_y1 <= d_y2; 
}

//! Returns x1
inline double QwtDoubleRect::x1() const
{ 
    return d_x1; 
}

//! Returns x2
inline double QwtDoubleRect::x2() const
{ 
    return d_x2; 
}

//! Returns y1
inline double QwtDoubleRect::y1() const
{ 
    return d_y1; 
}

//! Returns y2
inline double QwtDoubleRect::y2() const
{ 
    return d_y2; 
}

//! Returns a reference to x1
inline double &QwtDoubleRect::rx1()
{ 
    return d_x1; 
}

//! Returns a reference to x2
inline double &QwtDoubleRect::rx2()
{ 
    return d_x2; 
}

//! Returns a reference to y1
inline double &QwtDoubleRect::ry1()
{ 
    return d_y1; 
}

//! Returns a reference to y2
inline double &QwtDoubleRect::ry2()
{ 
    return d_y2; 
}

//! Set x1  
inline void QwtDoubleRect::setX1(double x1)
{ 
    d_x1 = x1;
}

//! Set x2  
inline void QwtDoubleRect::setX2(double x2)
{ 
    d_x2 = x2;
}

//! Set y1  
inline void QwtDoubleRect::setY1(double y1)
{ 
    d_y1 = y1;
}

//! Set y2  
inline void QwtDoubleRect::setY2(double y2)
{ 
    d_y2 = y2;
}

//! Returns the width
inline double QwtDoubleRect::width() const
{ 
    return  d_x2 - d_x1; 
}

//! Returns the height
inline double QwtDoubleRect::height() const
{ 
    return  d_y2 - d_y1; 
}

//! Returns the size
inline QwtDoubleSize QwtDoubleRect::size() const
{ 
    return QwtDoubleSize(width(), height());
}

//! Set the width, by x2 = x1 + w;
inline void QwtDoubleRect::setWidth(double w)
{
    d_x2 = d_x1 + w;
}

//! Set the height, by y2 = y1 + h;
inline void QwtDoubleRect::setHeight(double h)
{
    d_y2 = d_y1 + h;
}

#endif // QWT_DOUBLE_RECT_H
