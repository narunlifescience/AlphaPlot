/***************************************************************************
    File                 : ImageMarker.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Image marker (extension to QwtPlotMarker)
                           
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
#include <qwt_plot.h>
#include <qwt_plot_canvas.h>

ImageMarker::ImageMarker(const QPixmap& p):
    d_pic(p),
	d_pos(QPoint(0,0)),
	d_picSize(p.size())
{
}

void ImageMarker::draw (QPainter *p, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &r) const
{
	const int x0 = xMap.transform(d_rect.left());
	const int y0 = yMap.transform(d_rect.top());
	const int x1 = xMap.transform(d_rect.right());
	const int y1 = yMap.transform(d_rect.bottom());

	p->drawPixmap(QRect(QPoint(x0, y0), QPoint(x1, y1)), d_pic);
}

QSize ImageMarker::size()
{
	const QwtScaleMap &xMap = plot()->canvasMap(xAxis());
	const QwtScaleMap &yMap = plot()->canvasMap(yAxis());

	QPoint topLeft = QPoint(xMap.transform(d_rect.left()), yMap.transform(d_rect.top()));
	QPoint bottomRight = QPoint(xMap.transform(d_rect.right()), yMap.transform(d_rect.bottom()));

	return QRect(topLeft, bottomRight).size();
}

void ImageMarker::setSize(const QSize& size)
{
	d_picSize = size;

	const QwtScaleMap &xMap = plot()->canvasMap(xAxis());
	const QwtScaleMap &yMap = plot()->canvasMap(yAxis());

	int x = xMap.transform(d_rect.left());
	int y = yMap.transform(d_rect.top());

	d_rect.setRight(xMap.invTransform(x + size.width()));
	d_rect.setBottom(yMap.invTransform(y + size.height()));
}

void ImageMarker::setBoundingRect(const QwtDoubleRect& rect)
{
	if (d_rect == rect)
		return;

	d_rect = rect;

	const QwtScaleMap &xMap = plot()->canvasMap(xAxis());
	const QwtScaleMap &yMap = plot()->canvasMap(yAxis());

	d_pos = QPoint(xMap.transform(rect.left()), yMap.transform(rect.top()));
}

void ImageMarker::setOrigin(const QPoint& p)
{
	d_pos = p;

	const QwtScaleMap &xMap = plot()->canvasMap(xAxis());
	const QwtScaleMap &yMap = plot()->canvasMap(yAxis());

	d_rect.moveTo(xMap.invTransform(p.x()), yMap.invTransform(p.y()));
}

QPoint ImageMarker::getOrigin()
{
	const QwtScaleMap &xMap = plot()->canvasMap(xAxis());
	const QwtScaleMap &yMap = plot()->canvasMap(yAxis());

	return QPoint(xMap.transform(d_rect.left()), yMap.transform(d_rect.top()));
};

QRect ImageMarker::rect()
{
	return QRect(getOrigin(), size());
}

QwtDoubleRect ImageMarker::boundingRect() const
{
	return d_rect;
}

void ImageMarker::updateOrigin()
{
	const QwtScaleMap &xMap = plot()->canvasMap(xAxis());
	const QwtScaleMap &yMap = plot()->canvasMap(yAxis());

	d_rect.moveTo(xMap.invTransform(d_pos.x()), yMap.invTransform(d_pos.y()));

	d_rect.setRight(xMap.invTransform(d_pos.x() + d_picSize.width()));
	d_rect.setBottom(yMap.invTransform(d_pos.y() + d_picSize.height()));
}
