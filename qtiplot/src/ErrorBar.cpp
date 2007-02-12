/***************************************************************************
    File                 : ErrorBar.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Error bars curve
                           
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
#include "ErrorBar.h"

#include <qwt_painter.h>
#include <QPainter>

QwtErrorPlotCurve::QwtErrorPlotCurve(int orientation, const char *name):
	QwtPlotCurve(name)
{
	cap=10;
	type = orientation;
	size=QSize(1,1);
	plus=TRUE;
	minus=TRUE;
	through=FALSE;
	d_xOffset = 0;
	d_yOffset = 0;
}

QwtErrorPlotCurve::QwtErrorPlotCurve(const char *name):
	QwtPlotCurve(name)
{
	cap=10;
	type = Vertical;
	size=QSize(1,1);
	plus=TRUE;
	minus=TRUE;
	through=FALSE;
	d_xOffset = 0;
	d_yOffset = 0;
}

void QwtErrorPlotCurve::copy(const QwtErrorPlotCurve *e)
{
	cap = e->cap;
	type = e->type;
	size = e->size;
	plus = e->plus;
	minus = e->minus;
	through = e->through;
	setPen(e->pen());
	err = e->err;

	d_xOffset = e->xDataOffset();
	d_yOffset = e->yDataOffset();
}

void QwtErrorPlotCurve::draw(QPainter *painter,
		const QwtScaleMap &xMap, const QwtScaleMap &yMap, int from, int to) const
{
	if ( !painter || dataSize() <= 0 )
		return;

	if (to < 0)
		to = dataSize() - 1;

  painter->save();
  painter->setPen(pen());
  drawErrorBars(painter, xMap, yMap, from, to);
  painter->restore();
}

void QwtErrorPlotCurve::drawErrorBars(QPainter *painter,
		const QwtScaleMap &xMap, const QwtScaleMap &yMap, int from, int to) const
{   
	int sh=size.height();
	int sw=size.width();

	for (int i = from; i <= to; i++)
	{
		const int xi = xMap.transform(x(i) + d_xOffset);
		const int yi = yMap.transform(y(i) + d_yOffset);

		if (type == Vertical)
		{
			const int yh = yMap.transform(y(i)+err[i]);
			const int yl = yMap.transform(y(i)-err[i]);
			const int yhl = yi - sh/2;
			const int ylh = yi + sh/2;

			if (plus)
				{
				QwtPainter::drawLine(painter, xi, yhl, xi, yh);
				QwtPainter::drawLine(painter, xi-cap/2, yh, xi+cap/2, yh);
				}
			if (minus)
				{
				QwtPainter::drawLine(painter, xi, ylh, xi, yl);
				QwtPainter::drawLine(painter, xi-cap/2, yl, xi+cap/2, yl);
				}
			if (through)
				QwtPainter::drawLine(painter, xi, yhl, xi, ylh);
		}
		else if (type == Horizontal)
		{
			const int xp = xMap.transform(x(i)+err[i]);
			const int xm = xMap.transform(x(i)-err[i]);
  			const int xpm = xi + sw/2;
  	        const int xmp = xi - sw/2;
			
			if (plus)
				{
				QwtPainter::drawLine(painter, xp, yi, xpm, yi);
				QwtPainter::drawLine(painter, xp, yi-cap/2, xp, yi+cap/2);
				}
			if (minus)
				{
				QwtPainter::drawLine(painter, xm, yi, xmp, yi);
				QwtPainter::drawLine(painter, xm, yi-cap/2, xm, yi+cap/2);
				}
			if (through)
				QwtPainter::drawLine(painter, xmp, yi, xpm, yi);
		}
	}
}

double QwtErrorPlotCurve::errorValue(int i)
{
	if (i >= 0 && i < dataSize())
		return err[i];
	else 
		return 0.0;
}

bool QwtErrorPlotCurve::xErrors()
{
	bool x = false;
	if (type == Horizontal)
		x = true;

	return x;
}

void QwtErrorPlotCurve::setXErrors(bool yes)
{
	if (yes) 
		type = Horizontal;
	else 
		type = Vertical;
}

void QwtErrorPlotCurve::setWidth(int w)
{
	QPen p = pen();
 	p.setWidth (w);
  	setPen(p);
}

void QwtErrorPlotCurve::setColor(const QColor& c)
{
	QPen p = pen();
  	p.setColor(c);
  	setPen(p);
}

QwtDoubleRect QwtErrorPlotCurve::boundingRect() const
{
	QwtDoubleRect rect = QwtPlotCurve::boundingRect();

	int size = dataSize();

	QwtArray <double> X(size), Y(size), min(size), max(size);
	for (int i=0; i<size; i++)
	{
		X[i]=x(i);
		Y[i]=y(i);
		if (type == Vertical)
		{
			min[i] = y(i) - err[i];
			max[i] = y(i) + err[i];
		}
		else
		{
			min[i] = x(i) - err[i];
			max[i] = x(i) + err[i];
		}
	}

	QwtArrayData *erMin, *erMax;
	if (type == Vertical)
	{
		erMin=new QwtArrayData(X, min);
		erMax=new QwtArrayData(X, max);
	}
	else
	{
		erMin=new QwtArrayData(min, Y);
		erMax=new QwtArrayData(max, Y);
	}

	QwtDoubleRect minrect = erMin->boundingRect();
	QwtDoubleRect maxrect = erMax->boundingRect();

	rect.setTop(QMIN(minrect.top(), maxrect.top()));
	rect.setBottom(QMAX(minrect.bottom(), maxrect.bottom()));
	rect.setLeft(QMIN(minrect.left(), maxrect.left()));
	rect.setRight(QMAX(minrect.right(), maxrect.right()));

	delete erMin;
	delete erMax;

	return rect;
}
