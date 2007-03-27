/***************************************************************************
    File                 : ImageMarker.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Ion Vasilief, Knut Franke
    Email (use @ for *)  : ion_vasilief*yahoo.fr, knut.franke*gmx.de
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
	d_pos(QPoint(0,0)),
	d_size(p.size())
{
}

void ImageMarker::draw (QPainter *p, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &) const
{
	p->drawPixmap(transform(xMap, yMap, d_rect), d_pic);
}

void ImageMarker::setSize(const QSize& size)
{
d_size = size;
updateBoundingRect();
}

void ImageMarker::setOrigin(const QPoint& p)
{
d_pos = p;

if (!plot())
	return;

d_rect.moveTo(plot()->invTransform(xAxis(), p.x()), plot()->invTransform(yAxis(), p.y()));
d_size = size();
updateBoundingRect();
}

void ImageMarker::setRect(int x, int y, int w, int h)
{
if (d_pos == QPoint(x, y) && d_size == QSize(w, h))
	return;

d_pos = QPoint(x, y);
d_size = QSize(w, h);
updateBoundingRect();
}

void ImageMarker::setBoundingRect(const QwtDoubleRect& rect)
{
if (d_rect == rect)
	return;

d_rect = rect;

if (!plot())
	return;

plot()->updateLayout();

QRect r = this->rect();
d_pos = r.topLeft();
d_size = r.size();
}

void ImageMarker::updateBoundingRect()
{
if (!plot())
	return;

d_rect = invTransform(plot()->canvasMap(xAxis()), plot()->canvasMap(yAxis()), QRect(d_pos, d_size));
}

QwtDoubleRect ImageMarker::boundingRect() const
{
	QRect qr = rect();
	double left = plot()->canvasMap(xAxis()).invTransform(qr.left());
	double right = plot()->canvasMap(xAxis()).invTransform(qr.right());
	double top = plot()->canvasMap(yAxis()).invTransform(qr.top());
	double bottom = plot()->canvasMap(yAxis()).invTransform(qr.bottom());
	return QwtDoubleRect(left, top, qAbs(right-left), qAbs(bottom-top));
}
