/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#include "qwt_double_rect.h"

//! Constructs a point with coordinates (0.0, 0.0) (isNull() returns TRUE). 

QwtDoublePoint::QwtDoublePoint():
    d_x(0.0),
    d_y(0.0)
{
}

//! Constructs a point with x value xpos and y value ypos.

QwtDoublePoint::QwtDoublePoint(double x, double y ):
    d_x(x),
    d_y(y)
{
}

//! Copy constructor
QwtDoublePoint::QwtDoublePoint(const QPoint &p):
    d_x(double(p.x())),
    d_y(double(p.y()))
{
}

//! Returns TRUE if point and other are equal; otherwise returns FALSE. 
bool QwtDoublePoint::operator==(const QwtDoublePoint &other) const
{
    return (d_x == other.d_x) && (d_y == other.d_y);
}

//! Returns TRUE if point and other are not equal; otherwise returns FALSE. 
bool QwtDoublePoint::operator!=(const QwtDoublePoint &other) const
{
    return !operator==(other);
}

/*! 
  Returns the point formed by changing the sign of both components, 
  equivalent to QDoublePoint(-x(), -y());
*/

const QwtDoublePoint QwtDoublePoint::operator-() const
{
    return QwtDoublePoint(-d_x, -d_y);
}

/*! 
  Returns the point formed by adding both components by 
  the components of other. Each component is added separately.
*/

const QwtDoublePoint QwtDoublePoint::operator+(
    const QwtDoublePoint &other) const
{
    return QwtDoublePoint(d_x + other.d_x, d_y + other.d_y);
}

/*! 
  Returns the point formed by subtracting both components by 
  the components of other. Each component is subtracted separately.
*/

const QwtDoublePoint QwtDoublePoint::operator-(
    const QwtDoublePoint &other) const
{
    return QwtDoublePoint(d_x - other.d_x, d_y - other.d_y);
}

//! Returns the point formed by multiplying both components by c. 

const QwtDoublePoint QwtDoublePoint::operator*(double c) const
{
    return QwtDoublePoint(d_x * c, d_y * c);
}

//! Returns the point formed by dividing both components by c.

const QwtDoublePoint QwtDoublePoint::operator/(double c) const
{
    return QwtDoublePoint(d_x / c, d_y / c);
}

//! Adds point other to this point and returns a reference to this point.
  
QwtDoublePoint &QwtDoublePoint::operator+=(const QwtDoublePoint &other)
{
    d_x += other.d_x;
    d_y += other.d_y;
    return *this;
}

//! Subtracts point other from this point and returns a reference to this point. 

QwtDoublePoint &QwtDoublePoint::operator-=(const QwtDoublePoint &other)
{
    d_x -= other.d_x;
    d_y -= other.d_y;
    return *this;
}

//! Multiplies this point's x and y by c, and returns a reference to this point.

QwtDoublePoint &QwtDoublePoint::operator*=(double c)
{
    d_x *= c;
    d_y *= c;
    return *this;
}

//! Divides both x and y by c, and returns a reference to this point.

QwtDoublePoint &QwtDoublePoint::operator/=(double c)
{
    d_x /= c;
    d_y /= c;
    return *this;
}

//! Constructs a size with a width and height of 0.0. 

QwtDoubleSize::QwtDoubleSize():
    d_width(0.0),
    d_height(0.0)
{   
}   

//! Constructs a size with a width w and height h.

QwtDoubleSize::QwtDoubleSize( double w, double h ):
    d_width(w),
    d_height(h)
{   
}   

//! Copy constructor

QwtDoubleSize::QwtDoubleSize(const QSize &sz):
    d_width(double(sz.width())),
    d_height(double(sz.height()))
{   
}   

//! Swaps the values of width and height. 

void QwtDoubleSize::transpose()
{   
    double tmp = d_width;
    d_width = d_height;
    d_height = tmp;
}

/*! 
  Returns a size with the maximum width and height 
  of this size and other.
*/

QwtDoubleSize QwtDoubleSize::expandedTo(
    const QwtDoubleSize &other) const
{   
    return QwtDoubleSize(
        QMAX(d_width, other.d_width),
        QMAX(d_height, other.d_height)
    );  
}   

/*!
  Returns a size with the minimum width and height of 
  this size and other.
*/

QwtDoubleSize QwtDoubleSize::boundedTo(
    const QwtDoubleSize &other) const
{   
    return QwtDoubleSize(
        QMIN(d_width, other.d_width),
        QMIN(d_height, other.d_height)
    );  
}   

//! Returns TRUE if the size is equal to other; otherwise returns FALSE. 

bool QwtDoubleSize::operator==(const QwtDoubleSize &other) const
{ 
    return d_width == other.d_width && d_height == other.d_height;
}   

//! Returns TRUE if the size is not equal to other; otherwise returns FALSE. 

bool QwtDoubleSize::operator!=(const QwtDoubleSize &other) const
{ 
    return !operator==(other);
}   

/*! 
  Returns the size formed by changing the sign of both components,
  equivalent to QwtDoubleSize(-width(), -height());
*/

const QwtDoubleSize QwtDoubleSize::operator-() const
{
    return QwtDoubleSize(-d_width, -d_height);
}

/*! 
  Returns the size formed by adding both components by
  the components of other. Each component is added separately.
*/

const QwtDoubleSize QwtDoubleSize::operator+(
    const QwtDoubleSize &other) const
{   
    return QwtDoubleSize(d_width + other.d_width,
        d_height + other.d_height); 
}       

/*! 
  Returns the size formed by subtracting both components by
  the components of other. Each component is subtracted separately.
*/  

const QwtDoubleSize QwtDoubleSize::operator-(
    const QwtDoubleSize &other) const
{   
    return QwtDoubleSize(d_width - other.d_width,
        d_height - other.d_height); 
}       

//! Returns the size formed by multiplying both components by c.

const QwtDoubleSize QwtDoubleSize::operator*(double c) const
{ 
    return QwtDoubleSize(d_width * c, d_height * c);
}   

//! Returns the size formed by dividing both components by c.

const QwtDoubleSize QwtDoubleSize::operator/(double c) const
{ 
    return QwtDoubleSize(d_width / c, d_height / c);
}   

//! Adds size other to this size and returns a reference to this size.

QwtDoubleSize &QwtDoubleSize::operator+=(const QwtDoubleSize &other)
{   
    d_width += other.d_width; 
    d_height += other.d_height;
    return *this;
}

//! Subtracts size other from this size and returns a reference to this size.

QwtDoubleSize &QwtDoubleSize::operator-=(const QwtDoubleSize &other)
{   
    d_width -= other.d_width; 
    d_height -= other.d_height;
    return *this;
}

/* 
  Multiplies this size's width and height by c, 
  and returns a reference to this size.
*/

QwtDoubleSize &QwtDoubleSize::operator*=(double c)
{   
    d_width *= c; 
    d_height *= c;
    return *this;
}

/* 
  Devides this size's width and height by c, 
  and returns a reference to this size.
*/

QwtDoubleSize &QwtDoubleSize::operator/=(double c)
{
    d_width /= c;
    d_height /= c;
    return *this;
}   

//! Constructs an rectangle with all components set to 0.0 
QwtDoubleRect::QwtDoubleRect():
    d_x1(0.0),
    d_x2(0.0),
    d_y1(0.0),
    d_y2(0.0)
{
}

/*! 
  Constructs an rectangle with x1 to x2 as x-range and,
  y1 to y2 as y-range.
*/

QwtDoubleRect::QwtDoubleRect(double x1, double x2,
        double y1, double y2):
    d_x1(x1),
    d_x2(x2),
    d_y1(y1),
    d_y2(y2)
{
}

/*! 
  Constructs an rectangle with x1 to x1 + size.width() as x-range and,
  y1 to y1 + size.height() as y-range.
*/

QwtDoubleRect::QwtDoubleRect(double x, double y, const QwtDoubleSize &size):
    d_x1(x),
    d_x2(x + size.width()),
    d_y1(y),
    d_y2(y + size.height())
{
}

/*! 
  Set the x-range from x1 to x2 and the y-range from y1 to y2.
*/
void QwtDoubleRect::setRect(double x1, double x2, double y1, double y2)
{
    d_x1 = x1;
    d_x2 = x2;
    d_y1 = y1;
    d_y2 = y2;
}

/*!
  Sets the size of the rectangle to size. 
  Changes x2 and y2 only.
*/

void QwtDoubleRect::setSize(const QwtDoubleSize &size)
{
    setWidth(size.width());
    setHeight(size.height());
}

/*!
  Returns a normalized rectangle, i.e. a rectangle that has a non-negative 
  width and height. 

  It swaps x1 and x2 if x1() > x2(), and swaps y1 and y2 if y1() > y2(). 
*/

QwtDoubleRect QwtDoubleRect::normalize() const
{
    QwtDoubleRect r;
    if ( d_x2 < d_x1 ) 
    {
        r.d_x1 = d_x2;
        r.d_x2 = d_x1;
    } 
    else 
    {
        r.d_x1 = d_x1;
        r.d_x2 = d_x2; 
    }
    if ( d_y2 < d_y1 ) 
    { 
        r.d_y1 = d_y2; 
        r.d_y2 = d_y1;
    } 
    else 
    {
        r.d_y1 = d_y1;
        r.d_y2 = d_y2;
    }
    return r;
}

/*!
  Returns the bounding rectangle of this rectangle and rectangle other. 
  r.unite(s) is equivalent to r|s. 
*/

QwtDoubleRect QwtDoubleRect::unite(const QwtDoubleRect &other) const
{
    return *this | other;
}

/*!
  Returns the intersection of this rectangle and rectangle other. 
  r.intersect(s) is equivalent to r&s. 
*/

QwtDoubleRect QwtDoubleRect::intersect(const QwtDoubleRect &other) const
{
    return *this & other;
}

/*!
  Returns TRUE if this rectangle intersects with rectangle other; 
  otherwise returns FALSE. 
*/

bool QwtDoubleRect::intersects(const QwtDoubleRect &other) const
{
    return ( QMAX(d_x1, other.d_x1) <= QMIN(d_x2, other.d_x2) ) &&
         ( QMAX(d_y1, other.d_y1 ) <= QMIN(d_y2, other.d_y2) );
}

//! Returns TRUE if this rect and other are equal; otherwise returns FALSE. 

bool QwtDoubleRect::operator==(const QwtDoubleRect &other) const
{
    return d_x1 == other.d_x1 && d_x2 == other.d_x2 && 
        d_y1 == other.d_y1 && d_y2 == other.d_y2;
}

//! Returns TRUE if this rect and other are different; otherwise returns FALSE. 

bool QwtDoubleRect::operator!=(const QwtDoubleRect &other) const
{
    return !operator==(other);
}

/*!
  Returns the bounding rectangle of this rectangle and rectangle other. 
  The bounding rectangle of a nonempty rectangle and an empty or 
  invalid rectangle is defined to be the nonempty rectangle. 
*/

QwtDoubleRect QwtDoubleRect::operator|(const QwtDoubleRect &other) const
{
    if ( !isValid() ) 
        return other;

    if ( !other.isValid() ) 
        return *this;
        
    return QwtDoubleRect(QMIN(d_x1, other.d_x1), QMAX(d_x2, other.d_x2),
        QMIN(d_y1, other.d_y1), QMAX(d_y2, other.d_y2) );
}

/*!
  Returns the intersection of this rectangle and rectangle other. 
  Returns an empty rectangle if there is no intersection. 
*/

QwtDoubleRect QwtDoubleRect::operator&(const QwtDoubleRect &other) const
{
    return QwtDoubleRect(QMAX(d_x1, other.d_x1), QMIN(d_x2, other.d_x2),
        QMAX(d_y1, other.d_y1), QMIN(d_y2, other.d_y2));
}

//! Unites this rectangle with rectangle other. 

QwtDoubleRect &QwtDoubleRect::operator|=(const QwtDoubleRect &other)
{
    *this = *this | other;
    return *this;
}

//! Intersects this rectangle with rectangle other. 

QwtDoubleRect &QwtDoubleRect::operator&=(const QwtDoubleRect &other)
{
    *this = *this & other;
    return *this;
}

//! Returns the center point of the rectangle. 

QwtDoublePoint QwtDoubleRect::center() const
{
    return QwtDoublePoint(d_x1 + (d_x2 - d_x1) / 2.0, 
        d_y1 + (d_y2 - d_y1) / 2.0);
}

/*!
  Returns TRUE if the point (x, y) is inside or on the edge of the rectangle; 
  otherwise returns FALSE. 

  If proper is TRUE, this function returns TRUE only if p is inside 
  (not on the edge). 
*/

bool QwtDoubleRect::contains(double x, double y, bool proper) const
{
    if ( proper )
        return x > d_x1 && x < d_x2 && y > d_y1 && y < d_y2;
    else
        return x >= d_x1 && x <= d_x2 && y >= d_y1 && y <= d_y2;
}

/*!
  Returns TRUE if the point p is inside or on the edge of the rectangle; 
  otherwise returns FALSE. 

  If proper is TRUE, this function returns TRUE only if p is inside 
  (not on the edge). 
*/

bool QwtDoubleRect::contains(const QwtDoublePoint &p, bool proper) const
{
    return contains(p.x(), p.y(), proper);
}

/*!
  Returns TRUE if the rectangle other is inside this rectangle; 
  otherwise returns FALSE. 

  If proper is TRUE, this function returns TRUE only if other is entirely 
  inside (not on the edge). 
*/

bool QwtDoubleRect::contains(const QwtDoubleRect &other, bool proper) const
{
    return contains(other.d_x1, other.d_y1, proper) && 
        contains(other.d_x2, other.d_y2, proper);
}

//! moves x1() to x, leaving the size unchanged

void QwtDoubleRect::moveX(double x)
{
    const double w = width();
    d_x1 = x;
    d_x2 = d_x1 + w;
}

//! moves y1() to y, leaving the size unchanged

void QwtDoubleRect::moveY(double y)
{
    const double h = height();
    d_y1 = y;
    d_y2 = d_y1 + h;
}

//! moves x1() to x and y1() to y, leaving the size unchanged

void QwtDoubleRect::move(double x, double y)
{
    moveX(x);
    moveY(y);
}

//! moves x1() by dx and y1() by dy. leaving the size unchanged

void QwtDoubleRect::moveBy(double dx, double dy)
{
    d_x1 += dx;
    d_x2 += dx;
    d_y1 += dy;
    d_y2 += dy;
}

//! moves the center to pos, leaving the size unchanged

void QwtDoubleRect::moveCenter(const QwtDoublePoint &pos)
{
    moveCenter(pos.x(), pos.y());
}

//! moves the center to (x, y), leaving the size unchanged

void QwtDoubleRect::moveCenter(double x, double y)
{
    move(x - width() / 2.0, y - height() / 2.0);
}
