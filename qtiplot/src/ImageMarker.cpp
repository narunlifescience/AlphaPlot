/***************************************************************************
    File                 : ImageMarker.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Ion Vasilief, Knut Franke
    Email                : ion_vasilief@yahoo.fr, knut.franke@gmx.de
    Description          : Draw images on a QwtPlot.
                           
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *  This program is free software; you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation; either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the Free Software           *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor,                    *
 *   Boston, MA  02110-1301  USA                                           *
 *                                                                         *
 ***************************************************************************/

#include "ImageMarker.h"
#include <QPainter>

ImageMarker::ImageMarker(const QPixmap& p):
	d_pic(p),
	d_size(p.size())
{
}

void ImageMarker::draw (QPainter *p, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &) const
{
	p->drawPixmap(QRect(QPoint(xMap.transform(xValue()),yMap.transform(yValue())), d_size), d_pic);
}

QRect ImageMarker::rect() const
{
	return QRect(QPoint(xAxisMap().transform(xValue()), yAxisMap().transform(yValue())), d_size);
}

QPoint ImageMarker::origin() const
{
	return QPoint(xAxisMap().transform(xValue()), yAxisMap().transform(yValue()));
}

void ImageMarker::setOrigin(QPoint p)
{
	setXValue(xAxisMap().invTransform(p.x()));
	setYValue(yAxisMap().invTransform(p.y()));
	itemChanged();
}

void ImageMarker::setRect(int x, int y, int w, int h)
{
	setXValue(xAxisMap().invTransform(x));
	setYValue(yAxisMap().invTransform(y));
	d_size = QSize(qAbs(w), qAbs(h));
	itemChanged();
}

void ImageMarker::setBoundingRect(const QwtDoubleRect& r)
{
	setXValue(r.left());
	setYValue(r.top());
	int w = xAxisMap().transform(r.right()) - xAxisMap().transform(r.left());
	int h = yAxisMap().transform(r.bottom()) - yAxisMap().transform(r.top());
	d_size = QSize(qAbs(w), qAbs(h));
	itemChanged();
}

QwtDoubleRect ImageMarker::boundingRect() const
{
	QRect qr = rect();
	double left = xAxisMap().invTransform(qr.left());
	double right = xAxisMap().invTransform(qr.right());
	double top = yAxisMap().invTransform(qr.top());
	double bottom = yAxisMap().invTransform(qr.bottom());
	return QwtDoubleRect(left, top, qAbs(right-left), qAbs(bottom-top));
}
