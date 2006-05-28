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
#include <q3paintdevicemetrics.h>

#include <qwt_plot.h>
#include <qwt_scale_widget.h>
#include <qwt_painter.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_layout.h>
//Added by qt3to4:
#include <Q3PointArray>

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

#ifndef M_PI
#define M_PI	3.141592653589793238462643 
#endif


//FIXME: All functionality disabled for now (needs port to Qwt5)

LineMarker::LineMarker(QwtPlot *plot):
	QwtPlotMarker()
{
#if false
	endArrow=true;
	filledArrow=true;
	d_headAngle=45;
	d_headLength=4;
#endif
}

void LineMarker::draw(QPainter *p, int, int, const QRect &rect)
{	
#if false
	int x0,y0,x1,y1;
	if ( p->device()->isExtDev() )
	{	
		QwtPlot *plot = (QwtPlot *)parentPlot();
		const QwtScaleMap xMap = plot->canvasMap(QwtPlot::xBottom);
		const QwtScaleMap yMap = plot->canvasMap(QwtPlot::yLeft);

		double r0_x=xMap.invTransform(start.x());
		double r0_y=yMap.invTransform(start.y());	
		double r1_x=xMap.invTransform(end.x());
		double r1_y=yMap.invTransform(end.y());

		QwtScaleMap map=mapCanvasToDevice(p, plot, QwtPlot::xBottom);
		x0=map.transform(r0_x);
		x1=map.transform(r1_x);

		map=mapCanvasToDevice(p, plot, QwtPlot::yLeft);		
		y0=map.transform(r0_y);
		y1=map.transform(r1_y);
	}		
	else	
	{
		int clw = parentPlot()->canvas()->lineWidth();
		x0=start.x() + rect.x() - clw;
		y0=start.y() + rect.y() - clw;
		x1=end.x() + rect.x() - clw;
		y1=end.y() + rect.y() - clw;
	}

	p->save();
	p->setPen(pen);
	QBrush brush=QBrush(pen.color(), Qt::SolidPattern);
	QwtPainter::drawLine(p,x0,y0,x1,y1);
	p->restore();

	if (endArrow)
	{
		p->save();
		p->translate(x1,y1);
		double t=teta();
		p->rotate(-t);

		const Q3PointArray endArray(3);	
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
		double t=teta();
		p->rotate(-t);

		const Q3PointArray startArray(3);	
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
#endif
}

double LineMarker::teta()
{	
#if false
	int x0,y0,x1,y1;
	x0=start.x();
	y0=start.y();
	x1=end.x();
	y1=end.y();

	double t,pi=4*atan(-1.0);

	if (x1==x0)
	{
		if (y0>y1)
			t=90;
		else
			t=270;
	}
	else
	{
		t=atan2((y1-y0)*1.0,(x1-x0)*1.0)*180/pi;
		if (t<0)
			t=360+t;
	}
	return t;
#endif
}

double LineMarker::length()
{	
#if false
	int x0,y0,x1,y1;
	x0=start.x();
	y0=start.y();
	x1=end.x();
	y1=end.y();

	double l=sqrt(double((x1-x0)*(x1-x0)+(y1-y0)*(y1-y0)));	
	return fabs(l);
#endif
}

double LineMarker::dist(int x, int y)
{
#if false
	int x0,y0,x1,y1;		
	x0=start.x();
	y0=start.y();
	x1=end.x();
	y1=end.y();

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
#endif
}

QColor LineMarker::color()
{
#if false
	return pen.color();
#endif
}

int LineMarker::width()
{
#if false
	return pen.width ();
#endif
}

bool LineMarker::getStartArrow()
{
#if false
	return startArrow;
#endif
}

void LineMarker::setStartArrow(bool on)
{
#if false
	startArrow=on;
#endif
}

bool LineMarker::getEndArrow()
{
#if false
	return endArrow;
#endif
}

void LineMarker::setEndArrow(bool on)
{
#if false
	endArrow=on;
#endif
}

void LineMarker::setWidth(int w)
{
#if false
	pen.setWidth (w);
#endif
}

Qt::PenStyle LineMarker::style()
{
#if false
	return pen.style ();
#endif
}

void LineMarker::setStyle(Qt::PenStyle style)
{
#if false
	pen.setStyle(style);
#endif
}

void LineMarker::setColor(const QColor& c)
{
#if false
	pen.setColor(c);
#endif
}

void LineMarker::setStartPoint(const QPoint& p)
{
#if false
	if (start == p)
		return;

	start=p;
#endif
}

void LineMarker::setEndPoint(const QPoint& p)
{
#if false
	if (end == p)
		return;

	end=p;
#endif
}

QPoint LineMarker::startPoint()
{
#if false
	return start;
#endif
}

QPoint LineMarker::endPoint()
{
#if false
	return end;
#endif
}

void LineMarker::setHeadLength(int l)
{
#if false
	if (d_headLength == l)
		return;
	d_headLength=l;
#endif
}

void LineMarker::setHeadAngle(int a)
{
#if false
	if (d_headAngle == a)
		return;
	d_headAngle=a;
#endif
}

void LineMarker::fillArrowHead(bool fill)
{
#if false
	if (filledArrow == fill)
		return;
	filledArrow=fill;
#endif
}

QwtScaleMap LineMarker::mapCanvasToDevice(QPainter *p, QwtPlot *plot, int axis) 
{
#if false
	QwtPlotLayout *pl=plot->plotLayout ();

	Q3PaintDeviceMetrics pdmFrom(plot);
	Q3PaintDeviceMetrics pdmTo(p->device());	

	QwtMetricsMap metricsMap;
	metricsMap.setMetrics(pdmFrom, pdmTo);

	QwtScaleMap map;
	double from=0.0, to=0.0;		
	if (plot->axisEnabled(axis))
	{
		const QwtScaleDiv *scaleDiv =plot->axisScale(axis);
		map.setScaleInterval(scaleDiv->lBound(),
				scaleDiv->hBound());
		// FIXME: QwtScaleDiv doesn't habe logScale() method anymore
		//X map.setTransformation(scaleDiv->logScale());

		const QwtScale *scale =plot->axis(axis);
		const int sDist = scale->startBorderDist();
		const int eDist = scale->endBorderDist();
		const QRect &scaleRect = pl->scaleRect(axis);		
		if (axis==QwtPlot::xBottom)
		{	
			from= metricsMap.layoutToDeviceX(scaleRect.left() + sDist);
			to = metricsMap.layoutToDeviceX(scaleRect.right()  - eDist);
		}
		else if (axis==QwtPlot::yLeft)
		{
			from = metricsMap.layoutToDeviceY(scaleRect.bottom() - sDist);
			to = metricsMap.layoutToDeviceY(scaleRect.top() + eDist);
		}
	}
	else
	{
		const int margin = pl->canvasMargin(axis);
		const QRect &canvasRect = pl->canvasRect();
		if ( axis ==QwtPlot::yLeft)
		{
			QwtScaleMap yMap = plot->canvasMap(QwtPlot::yLeft);
			map.setScaleInterval(yMap.s1(),yMap.s2());
			map.setTransformation(yMap.transformation());

			from = metricsMap.layoutToDeviceY(canvasRect.bottom() - margin);
			to = metricsMap.layoutToDeviceY(canvasRect.top() + margin);
		}
		else
		{
			QwtScaleMap xMap = plot->canvasMap(QwtPlot::xBottom);
			map.setScaleInterval(xMap.s1(),xMap.s2());
			map.setTransformation(xMap.transformation());

			from = metricsMap.layoutToDeviceX(canvasRect.left() + margin);
			to = metricsMap.layoutToDeviceX(canvasRect.right() - margin);
		}
	}      	
	map.setPaintInterval(qwtInt(from), qwtInt(to));	
	return map;	
#endif
}

