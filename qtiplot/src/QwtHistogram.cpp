/***************************************************************************
    File                 : QwtHistogram.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : Histogram class
                           
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
#include "QwtHistogram.h"
#include <QPainter>

QwtHistogram::QwtHistogram(Table *t, const QString& xColName, const char *name, int startRow, int endRow):
	QwtBarCurve(QwtBarCurve::Vertical, t, xColName, name, startRow, endRow)
{}

void QwtHistogram::copy(const QwtHistogram *h)
{
	QwtBarCurve::copy((const QwtBarCurve *)h);

	d_autoBin = h->d_autoBin;
	d_binSize = h->d_binSize;
	d_begin = h->d_begin;
	d_end = h->d_end;
}

void QwtHistogram::draw(QPainter *painter,
		const QwtScaleMap &xMap, const QwtScaleMap &yMap, int from, int to) const
{
	if ( !painter || dataSize() <= 0 )
		return;

	if (to < 0)
		to = dataSize() - 1;


	painter->save();
	painter->setPen(QwtPlotCurve::pen());
	painter->setBrush(QwtPlotCurve::brush());

	const int ref= yMap.transform(baseline());
	const int dx=abs(xMap.transform(x(from+1)) - xMap.transform(x(from)));
	const int bar_width=int(dx*(1-gap()*0.01));
	const int half_width = int(0.5*(dx-bar_width));
	const int xOffset = int(0.01*offset()*bar_width);

	for (int i=from; i<=to; i++)
	{
		const int px1 = xMap.transform(x(i));
		const int py1 = yMap.transform(y(i));
		painter->drawRect(px1+half_width+xOffset,py1,bar_width+1,(ref-py1+1));
	}

	painter->restore();
}

QwtDoubleRect QwtHistogram::boundingRect() const
{
	QwtDoubleRect rect = QwtPlotCurve::boundingRect();
	rect.setLeft(rect.left()-x(1));
	rect.setRight(rect.right()+x(dataSize()-1));
	rect.setTop(0);
	rect.setBottom(1.2*rect.bottom());
	return rect;
}

void QwtHistogram::setBinning(bool autoBin, double begin, double end, double size)
{
	d_autoBin = autoBin;
	d_binSize = size;
	d_begin = begin;
	d_end = end;
}
