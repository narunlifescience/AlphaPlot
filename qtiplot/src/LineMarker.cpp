#include "LineMarker.h"

#include <qpainter.h>
#include <qpaintdevicemetrics.h>

#include <qwt_plot.h>
#include <qwt_scale.h>
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
    QwtPlotMarker(plot)
{
endArrow=true;
filledArrow=true;
d_headAngle=45;
d_headLength=4;
}

void LineMarker::draw(QPainter *p, int, int, const QRect &rect)
{	
	int x0,y0,x1,y1;
	if ( p->device()->isExtDev() )
		{	
		QwtPlot *plot = (QwtPlot *)parentPlot();
		const QwtDiMap xMap = plot->canvasMap(QwtPlot::xBottom);
		const QwtDiMap yMap = plot->canvasMap(QwtPlot::yLeft);
	
		double r0_x=xMap.invTransform(start.x());
		double r0_y=yMap.invTransform(start.y());	
		double r1_x=xMap.invTransform(end.x());
		double r1_y=yMap.invTransform(end.y());
			
		QwtDiMap map=mapCanvasToDevice(p, plot, QwtPlot::xBottom);
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
	QBrush brush=QBrush(pen.color(), QBrush::SolidPattern);
	QwtPainter::drawLine(p,x0,y0,x1,y1);
	p->restore();
		
	if (endArrow)
		{
		p->save();
		p->translate(x1,y1);
		double t=teta();
		p->rotate(-t);

		const QPointArray endArray(3);	
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

		const QPointArray startArray(3);	
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

double LineMarker::teta()
{	
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
}

double LineMarker::length()
{	
int x0,y0,x1,y1;
x0=start.x();
y0=start.y();
x1=end.x();
y1=end.y();
		
double l=sqrt(double((x1-x0)*(x1-x0)+(y1-y0)*(y1-y0)));	
return fabs(l);
}

double LineMarker::dist(int x, int y)
{
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

void LineMarker::setStartPoint(const QPoint& p)
{
if (start == p)
	return;

start=p;
}

void LineMarker::setEndPoint(const QPoint& p)
{
if (end == p)
	return;

end=p;
}

QPoint LineMarker::startPoint()
{
return start;
}

QPoint LineMarker::endPoint()
{
return end;
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

QwtDiMap LineMarker::mapCanvasToDevice(QPainter *p, QwtPlot *plot, int axis) 
{
QwtPlotLayout *pl=plot->plotLayout ();
	
QPaintDeviceMetrics pdmFrom(plot);
QPaintDeviceMetrics pdmTo(p->device());	
	
QwtMetricsMap metricsMap;
metricsMap.setMetrics(pdmFrom, pdmTo);

QwtDiMap map;
double from=0.0, to=0.0;		
if (plot->axisEnabled(axis))
	{
	const QwtScaleDiv *scaleDiv =plot->axisScale(axis);
	map.setDblRange(scaleDiv->lBound(),
				scaleDiv->hBound(), scaleDiv->logScale());
		
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
		 QwtDiMap yMap = plot->canvasMap(QwtPlot::yLeft);
		 map.setDblRange(yMap.d1(),yMap.d2(),yMap.logarithmic());
			 
         from = metricsMap.layoutToDeviceY(canvasRect.bottom() - margin);
         to = metricsMap.layoutToDeviceY(canvasRect.top() + margin);
         }
    else
        {
		QwtDiMap xMap = plot->canvasMap(QwtPlot::xBottom);
		map.setDblRange(xMap.d1(),xMap.d2(),xMap.logarithmic());
			
        from = metricsMap.layoutToDeviceX(canvasRect.left() + margin);
        to = metricsMap.layoutToDeviceX(canvasRect.right() - margin);
        }
	}      	
map.setIntRange(qwtInt(from), qwtInt(to));	
return map;	
}
