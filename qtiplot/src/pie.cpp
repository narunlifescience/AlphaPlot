/***************************************************************************
    File                 : pie.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : Pie plot class
                           
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
#include "pie.h"
#include "colorBox.h"

#include <qwt_painter.h>
#include <QPainter>

QwtPieCurve::QwtPieCurve(const char *name):
    QwtPlotCurve(name)
{
pieRay=100;
firstColor=0;
setPen(QPen(QColor(Qt::black),1,Qt::SolidLine));
setBrush(QBrush(Qt::black,Qt::SolidPattern));
}

void QwtPieCurve::draw(QPainter *painter,
    const QwtScaleMap &xMap, const QwtScaleMap &yMap, int from, int to) const
{
    if ( !painter || dataSize() <= 0 )
        return;

    if (to < 0)
        to = dataSize() - 1;

    drawPie(painter, xMap, yMap, from, to);
}

void QwtPieCurve::drawPie(QPainter *painter,
    const QwtScaleMap &xMap, const QwtScaleMap &yMap, int from, int to) const
{   	
	int i, d = pieRay*2;
	QwtPlot *plot = (QwtPlot *)this->plot();
	QRect rect = plot->rect();
	int x_center = rect.x() + rect.width()/2;
	int y_center = rect.y() + rect.height()/2;

// FIXME: fix the printing code
/*
	if (painter->device()->isExtDev()) 
		{//draw on printer
		QPaintDeviceMetrics pdmFrom(plot);
		QPaintDeviceMetrics pdmTo(painter->device());
		
		double dx = (double)pdmTo.width()/(double)pdmFrom.width();
		double dy = (double)pdmTo.height()/(double)pdmFrom.height();

		x_center = int(x_center*dx);
		y_center = int(y_center*dy);
		d = int(d*QMIN(dx, dy));
		}
*/
	QRect pieRect;
	pieRect.setX(x_center - d/2);
	pieRect.setY(y_center - d/2);
	pieRect.setHeight(d);
	pieRect.setWidth(d);	
				
	double sum=0.0;
	for (i = from; i <= to; i++)
		{
		const double yi = y(i);
		sum+=yi;
		}
	
	int angle = (int)(5760 * 0.75);
	painter->save();
	for (i = from; i <= to; i++)
		{
		const double yi = y(i);			
		const int value = (int)(yi/sum*5760);
			
		painter->setPen(QwtPlotCurve::pen());
        painter->setBrush(QBrush(color(i), QwtPlotCurve::brush().style()));
		painter->drawPie(pieRect, -angle, -value);

		angle += value;
		}
	painter->restore();
}

QColor QwtPieCurve::color(int i) const
{
int index=(firstColor+i)%16;
return ColorBox::color(index);
}

void QwtPieCurve::setBrushStyle(const Qt::BrushStyle& style)
{
QBrush br = QwtPlotCurve::brush();
if (br.style() == style)
	return;

br.setStyle(style);
setBrush(br);
}
