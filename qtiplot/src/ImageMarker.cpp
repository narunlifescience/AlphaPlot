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
#include <QImageReader>

ImageMarker::ImageMarker(const QString& fn):
	d_pos(QPoint(0,0)),
	d_x_right(0),
    d_y_bottom(0)
{
	QList<QByteArray> lst = QImageReader::supportedImageFormats();
	for (int i=0; i<(int)lst.count(); i++){
		if (fn.contains("." + lst[i])){
			d_pic.load(fn, lst[i], QPixmap::Auto);
			d_size = d_pic.size();
			d_file_name = fn;
			break;
		}
	}
}

void ImageMarker::draw (QPainter *p, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &) const
{
    const int x0 = xMap.transform(xValue());
	const int y0 = yMap.transform(yValue());
	const int x1 = xMap.transform(d_x_right);
	const int y1 = yMap.transform(d_y_bottom);

	p->drawPixmap(QRect(x0, y0, abs(x1 - x0), abs(y1 - y0)), d_pic);
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

    setXValue(plot()->invTransform(xAxis(), p.x()));
    setYValue(plot()->invTransform(yAxis(), p.y()));

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

void ImageMarker::setBoundingRect(double left, double top, double right, double bottom)
{
    if (xValue() == left && yValue() == top && d_x_right == right && d_y_bottom == bottom)
        return;

    setXValue(left);
    setYValue(top);
    d_x_right = right;
    d_y_bottom = bottom;

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

    setXValue(plot()->invTransform(xAxis(), d_pos.x()));
    d_x_right = plot()->invTransform(xAxis(), d_pos.x() + d_size.width());

    setYValue(plot()->invTransform(yAxis(), d_pos.y()));
    d_y_bottom = plot()->invTransform(yAxis(), d_pos.y() + d_size.height());
}

QwtDoubleRect ImageMarker::boundingRect() const
{
    return QwtDoubleRect(xValue(), yValue(), qAbs(d_x_right - xValue()), qAbs(d_y_bottom - yValue()));
}

QRect ImageMarker::rect() const
{
    const QwtScaleMap &xMap = plot()->canvasMap(xAxis());
    const QwtScaleMap &yMap = plot()->canvasMap(yAxis());

    const int x0 = xMap.transform(xValue());
    const int y0 = yMap.transform(yValue());
    const int x1 = xMap.transform(d_x_right);
    const int y1 = yMap.transform(d_y_bottom);

    return QRect(x0, y0, abs(x1 - x0), abs(y1 - y0));
}
