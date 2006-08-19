/***************************************************************************
    File                 : LineMarker.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
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

#include <qpainter.h>
#include <qpaintdevice.h>
#include <QPolygon>

#include <qwt_plot.h>
#include <qwt_scale_widget.h>
#include <qwt_painter.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_layout.h>

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

#ifndef M_PI
#define M_PI	3.141592653589793238462643 
#endif

LineMarker::LineMarker(QwtPlot *plot):
		endArrow(true),
		filledArrow(true),
		d_headAngle(45),
		d_headLength(4),
		d_plot(plot)
{
}

void LineMarker::draw(QPainter *p, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &r) const
{	
	const int x0 = xMap.transform(d_rect.left());
	const int y0 = yMap.transform(d_rect.top());
	const int x1 = xMap.transform(d_rect.right());
	const int y1 = yMap.transform(d_rect.bottom());
	
	p->save();
	p->setPen(pen);
	QBrush brush=QBrush(pen.color(), Qt::SolidPattern);
	QwtPainter::drawLine(p,x0,y0,x1,y1);
	p->restore();
		
	if (endArrow)
		{
		p->save();
		p->translate(x1,y1);
		const double t = teta(x0, y0, x1, y1);
		p->rotate(-t);

		QPolygon endArray(3);	
		endArray[0] = QPoint(0,0);
			
		int d=(int)floor(d_headLength*tan(M_PI*d_headAngle/180.0)+0.5);				
		endArray[1] = QPoint(-d_headLength,d);
		endArray[2] = QPoint(-d_headLength,-d);

		p->setPen(QPen(pen.color(),pen.width(),Qt::SolidLine));
		if (filledArrow)
			p->setBrush(brush);

		QwtPainter::drawPolygon(p,endArray);
		p->restore();
		}

	if (startArrow)
		{
		p->save();
		p->translate(x0,y0);
		const double t = teta(x0, y0, x1, y1);
		p->rotate(-t);

		QPolygon startArray(3);	
		startArray[0] = QPoint(0,0);
			
		int d=(int)floor(d_headLength*tan(M_PI*d_headAngle/180.0)+0.5);
		startArray[1] = QPoint(d_headLength,d);
		startArray[2] = QPoint(d_headLength,-d);

		p->setPen(QPen(pen.color(), pen.width(), Qt::SolidLine));
		if (filledArrow)
			p->setBrush(brush);
		QwtPainter::drawPolygon(p,startArray);
		p->restore();
		}
}

double LineMarker::teta(int xs, int ys, int xe, int ye) const
{			
double t,pi=4*atan(-1.0);

if (xe == xs)
	{
	if (ys>ye)
		t=90;
	else
		t=270;
	}
else
	{
	t=atan2((ye-ys)*1.0,(xe-xs)*1.0)*180/pi;
	if (t<0)
		t=360+t;
	}
return t;
}

double LineMarker::length()
{
const QwtScaleMap &xMap = d_plot->canvasMap(xAxis());
const QwtScaleMap &yMap = d_plot->canvasMap(yAxis());

const int x0 = xMap.transform(d_rect.left());
const int y0 = yMap.transform(d_rect.top());
const int x1 = xMap.transform(d_rect.right());
const int y1 = yMap.transform(d_rect.bottom());
		
double l=sqrt(double((x1-x0)*(x1-x0)+(y1-y0)*(y1-y0)));	
return fabs(l);
}

double LineMarker::dist(int x, int y)
{
const QwtScaleMap &xMap = d_plot->canvasMap(xAxis());
const QwtScaleMap &yMap = d_plot->canvasMap(yAxis());

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

QColor LineMarker::color()
{
return pen.color();
}

int LineMarker::width()
{
return pen.width ();
}

bool LineMarker::getStartArrow()
{
return startArrow;
}

void LineMarker::setStartArrow(bool on)
{
startArrow=on;
}

bool LineMarker::getEndArrow()
{
return endArrow;
}

void LineMarker::setEndArrow(bool on)
{
endArrow=on;
}

void LineMarker::setWidth(int w)
{
pen.setWidth (w);
}

Qt::PenStyle LineMarker::style()
{
return pen.style ();
}

void LineMarker::setStyle(Qt::PenStyle style)
{
pen.setStyle(style);
}

void LineMarker::setColor(const QColor& c)
{
pen.setColor(c);
}

void LineMarker::setHeadLength(int l)
{
if (d_headLength == l)
	return;
d_headLength=l;
}

void LineMarker::setHeadAngle(int a)
{
if (d_headAngle == a)
	return;
d_headAngle=a;
}

void LineMarker::fillArrowHead(bool fill)
{
if (filledArrow == fill)
	return;
filledArrow=fill;
}

void LineMarker::setStartPoint(const QPoint& p)
{
if (d_start == p)
	return;

d_start = p;

const QwtScaleMap &xMap = d_plot->canvasMap(xAxis());
const QwtScaleMap &yMap = d_plot->canvasMap(yAxis());

d_rect.setLeft(xMap.invTransform(p.x()));
d_rect.setTop(yMap.invTransform(p.y()));
}

void LineMarker::setEndPoint(const QPoint& p)
{
if (d_end == p)
	return;

d_end = p;

const QwtScaleMap &xMap = d_plot->canvasMap(xAxis());
const QwtScaleMap &yMap = d_plot->canvasMap(yAxis());

d_rect.setRight(xMap.invTransform(p.x()));
d_rect.setBottom(yMap.invTransform(p.y()));
}

QPoint LineMarker::startPoint()
{
const QwtScaleMap &xMap = d_plot->canvasMap(xAxis());
const QwtScaleMap &yMap = d_plot->canvasMap(yAxis());

return QPoint(xMap.transform(d_rect.left()), yMap.transform(d_rect.top()));
}

QwtDoublePoint LineMarker::coordStartPoint()
{
return QwtDoublePoint(d_rect.left(), d_rect.top());
}

void LineMarker::setCoordStartPoint(const QwtDoublePoint& p)
{
if (QwtDoublePoint(d_rect.left(), d_rect.top()) == p)
	return;

d_rect.setLeft(p.x());
d_rect.setTop(p.y());

const QwtScaleMap &xMap = d_plot->canvasMap(xAxis());
const QwtScaleMap &yMap = d_plot->canvasMap(yAxis());

d_start = QPoint(xMap.transform(p.x()), yMap.transform(p.y()));
}

QPoint LineMarker::endPoint()
{
const QwtScaleMap &xMap = d_plot->canvasMap(xAxis());
const QwtScaleMap &yMap = d_plot->canvasMap(yAxis());

return QPoint(xMap.transform(d_rect.right()), yMap.transform(d_rect.bottom()));
}

void LineMarker::setCoordEndPoint(const QwtDoublePoint& p)
{
if (QwtDoublePoint(d_rect.right(), d_rect.bottom()) == p)
	return;

d_rect.setRight(p.x());
d_rect.setBottom(p.y());

const QwtScaleMap &xMap = d_plot->canvasMap(xAxis());
const QwtScaleMap &yMap = d_plot->canvasMap(yAxis());

d_end = QPoint(xMap.transform(p.x()), yMap.transform(p.y()));
}

QwtDoublePoint LineMarker::coordEndPoint()
{
return QwtDoublePoint(d_rect.right(), d_rect.bottom());
}

void LineMarker::updateBoundingRect()
{
const QwtScaleMap &xMap = d_plot->canvasMap(xAxis());
const QwtScaleMap &yMap = d_plot->canvasMap(yAxis());

d_rect.setLeft(xMap.invTransform(d_start.x()));
d_rect.setTop(yMap.invTransform(d_start.y()));

d_rect.setRight(xMap.invTransform(d_end.x()));
d_rect.setBottom(yMap.invTransform(d_end.y()));
}
