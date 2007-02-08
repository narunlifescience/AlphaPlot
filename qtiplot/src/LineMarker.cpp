/***************************************************************************
    File                 : LineMarker.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief
    Email                : ion_vasilief@yahoo.fr
    Description          : Line marker (extension to QwtPlotMarker)
                           
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
#include "LineMarker.h"

#include <QPainter>

#include <qwt_plot.h>
#include <qwt_painter.h>

#ifndef M_PI
#define M_PI 3.141592653589793238462643;
#endif

LineMarker::LineMarker():
		d_end_arrow(true),
		d_fill_head(true),
		d_head_angle(45),
		d_head_length(4),
		d_rect(0, 0, 1, 1)
{
}

void LineMarker::draw(QPainter *p, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &r) const
{	
	const int x0 = xMap.transform(d_rect.left());
	const int y0 = yMap.transform(d_rect.top());
	const int x1 = xMap.transform(d_rect.right());
	const int y1 = yMap.transform(d_rect.bottom());
	
	p->save();
	QPen pen = linePen();
	p->setPen(pen);

	QBrush brush = QBrush(pen.color(), Qt::SolidPattern);
	QwtPainter::drawLine(p,x0,y0,x1,y1);
	p->restore();
		
	if (d_end_arrow)
		{
		p->save();
		p->translate(x1,y1);
		const double t = theta(x0, y0, x1, y1);
		p->rotate(-t);

		QPolygon endArray(3);	
		endArray[0] = QPoint(0,0);
			
		int d=(int)floor(d_head_length*tan(M_PI*d_head_angle/180.0)+0.5);				
		endArray[1] = QPoint(-d_head_length,d);
		endArray[2] = QPoint(-d_head_length,-d);

		p->setPen(QPen(pen.color(), pen.width(), Qt::SolidLine));
		if (d_fill_head)
			p->setBrush(brush);

		QwtPainter::drawPolygon(p,endArray);
		p->restore();
		}

	if (d_start_arrow)
		{
		p->save();
		p->translate(x0,y0);
		const double t = theta(x0, y0, x1, y1);
		p->rotate(-t);

		QPolygon startArray(3);	
		startArray[0] = QPoint(0,0);
			
		int d=(int)floor(d_head_length*tan(M_PI*d_head_angle/180.0)+0.5);
		startArray[1] = QPoint(d_head_length,d);
		startArray[2] = QPoint(d_head_length,-d);

		p->setPen(QPen(pen.color(), pen.width(), Qt::SolidLine));
		if (d_fill_head)
			p->setBrush(brush);
		QwtPainter::drawPolygon(p,startArray);
		p->restore();
		}
}

double LineMarker::theta(int xs, int ys, int xe, int ye) const
{
double t, pi = 4.0*atan(-1.0);
if (xe == xs)
	{
	if (ys > ye)
		t = 90;
	else
		t = 270;
	}
else
	{
	t = atan2((ye-ys)*1.0,(xe-xs)*1.0)*45/atan(-1.0);
	if (t<0)
		t = 360+t;
	}
return t;
}

double LineMarker::length()
{
if (!plot())
	return -1.0;

const QwtScaleMap &xMap = plot()->canvasMap(xAxis());
const QwtScaleMap &yMap = plot()->canvasMap(yAxis());

const int x0 = xMap.transform(d_rect.left());
const int y0 = yMap.transform(d_rect.top());
const int x1 = xMap.transform(d_rect.right());
const int y1 = yMap.transform(d_rect.bottom());
		
double l=sqrt(double((x1-x0)*(x1-x0)+(y1-y0)*(y1-y0)));	
return fabs(l);
}

double LineMarker::dist(int x, int y)
{
if (!plot())
	return -1.0;

const QwtScaleMap &xMap = plot()->canvasMap(xAxis());
const QwtScaleMap &yMap = plot()->canvasMap(yAxis());

const int x0 = xMap.transform(d_rect.left());
const int y0 = yMap.transform(d_rect.top());
const int x1 = xMap.transform(d_rect.right());
const int y1 = yMap.transform(d_rect.bottom());

int xmin=QMIN(x0,x1);
int xmax=QMAX(x0,x1);
int ymin=QMIN(y0,y1);
int ymax=QMAX(y0,y1);
	
if ( (x>xmax || x<xmin || xmin==xmax) && (ymax<y || ymin>y || ymin==ymax))
	//return the shortest distance to one of the ends
	return QMIN(sqrt(double((x-x0)*(x-x0)+(y-y0)*(y-y0))),
				sqrt(double((x-x1)*(x-x1)+(y-y1)*(y-y1))));
	
double d;
if (x0==x1)
	d=abs(x-x0);
else
	{
	double a=(double)(y1-y0)/(double)(x1-x0);
	double b=y0-a*x0;
	d=(a*x-y+b)/sqrt(a*a+1);
	}	
return fabs(d);
}

void LineMarker::setColor(const QColor& c)
{
if (linePen().color() == c)
	return;

QPen pen = linePen();
pen.setColor(c);
setLinePen(pen);
}

void LineMarker::setWidth(int w)
{
if (linePen().width() == w)
	return;

QPen pen = linePen();
pen.setWidth (w);
setLinePen(pen);
}

void LineMarker::setStyle(Qt::PenStyle style)
{
if (linePen().style() == style)
	return;

QPen pen = linePen();
pen.setStyle(style);
setLinePen(pen);
}

void LineMarker::setHeadLength(int l)
{
if (d_head_length == l)
	return;

d_head_length=l;
}

void LineMarker::setHeadAngle(int a)
{
if (d_head_angle == a)
	return;

d_head_angle=a;
}

void LineMarker::fillArrowHead(bool fill)
{
if (d_fill_head == fill)
	return;

d_fill_head=fill;
}

void LineMarker::setStartPoint(const QPoint& p)
{
if (d_start == p)
	return;

d_start = p;

if (!plot())
	return;

d_rect.setLeft(plot()->invTransform(xAxis(), p.x()));
d_rect.setTop(plot()->invTransform(yAxis(), p.y()));
}

void LineMarker::setEndPoint(const QPoint& p)
{
if (d_end == p)
	return;

d_end = p;

if (!plot())
	return;

d_rect.setRight(plot()->invTransform(xAxis(), p.x()));
d_rect.setBottom(plot()->invTransform(yAxis(), p.y()));
}

QPoint LineMarker::startPoint()
{
if (!plot())
	return QPoint();

return QPoint(plot()->transform(xAxis(), d_rect.left()), 
			  plot()->transform(yAxis(), d_rect.top()));
}

QwtDoublePoint LineMarker::startPointCoord()
{
return QwtDoublePoint(d_rect.left(), d_rect.top());
}

void LineMarker::setStartPoint(double x, double y)
{
if (d_rect.left() == x && d_rect.top() == y)
	return;

d_rect.setLeft(x);
d_rect.setTop(y);

if (!plot())
	return;

d_start = QPoint(plot()->transform(xAxis(), x), plot()->transform(yAxis(), y));
}

QPoint LineMarker::endPoint()
{
if (!plot())
	return QPoint();

return QPoint(plot()->transform(xAxis(), d_rect.right()), 
			  plot()->transform(yAxis(), d_rect.bottom()));
}

void LineMarker::setEndPoint(double x, double y)
{
if (d_rect.right() == x && d_rect.bottom() == y)
	return;

d_rect.setRight(x);
d_rect.setBottom(y);

if (!plot())
	return;

d_end = QPoint(plot()->transform(xAxis(), x), plot()->transform(yAxis(), y));
}

QwtDoublePoint LineMarker::endPointCoord()
{
return QwtDoublePoint(d_rect.right(), d_rect.bottom());
}

QwtDoubleRect LineMarker::boundingRect() const
{
return d_rect;
}

void LineMarker::updateBoundingRect()
{
const QwtScaleMap &xMap = plot()->canvasMap(xAxis());
const QwtScaleMap &yMap = plot()->canvasMap(yAxis());

d_rect.setLeft(xMap.invTransform(d_start.x()));
d_rect.setTop(yMap.invTransform(d_start.y()));
d_rect.setRight(xMap.invTransform(d_end.x()));
d_rect.setBottom(yMap.invTransform(d_end.y()));
}
