/***************************************************************************
    File                 : ArrowMarker.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief
    Email (use @ for *)  : ion_vasilief*yahoo.fr
    Description          : Arrow marker (extension to QwtPlotMarker)

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
#include "ArrowMarker.h"
#include "LineDialog.h"

#include <QPainter>
#include <QMouseEvent>
#include <QApplication>

#include <qwt_plot.h>
#include <qwt_plot_canvas.h>
#include <qwt_painter.h>

#ifndef M_PI
#define M_PI 3.141592653589793238462643;
#endif

ArrowMarker::ArrowMarker():
		d_end_arrow(true),
		d_fill_head(true),
		d_head_angle(45),
		d_head_length(4),
		d_rect(0, 0, 1, 1),
		d_editable(false),
		d_op(None)
{
}

void ArrowMarker::draw(QPainter *p, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &) const
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

	if (d_editable) {
		p->save();
		p->setPen(QPen(Qt::black,1,Qt::SolidLine));
		QRect handler(QPoint(0,0), QSize(10,10));
		handler.moveCenter(startPoint());
		p->fillRect(handler, QBrush(Qt::black));
		handler.moveCenter(endPoint());
		p->fillRect(handler, QBrush(Qt::black));
		p->restore();
	}
}

double ArrowMarker::theta(int xs, int ys, int xe, int ye) const
{
double t;
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

double ArrowMarker::length()
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

double ArrowMarker::dist(int x, int y)
{
	if (!plot())
		return -1.0;

	const QwtScaleMap &xMap = plot()->canvasMap(xAxis());
	const QwtScaleMap &yMap = plot()->canvasMap(yAxis());

	const int x0 = xMap.transform(d_rect.left());
	const int y0 = yMap.transform(d_rect.top());
	const int x1 = xMap.transform(d_rect.right());
	const int y1 = yMap.transform(d_rect.bottom());

	int xmin=qMin(x0,x1);
	int xmax=qMax(x0,x1);
	int ymin=qMin(y0,y1);
	int ymax=qMax(y0,y1);

	if ( (x>xmax || x<xmin || xmin==xmax) && (ymax<y || ymin>y || ymin==ymax))
		//return the shortest distance to one of the ends
		return qMin(sqrt(double((x-x0)*(x-x0)+(y-y0)*(y-y0))),
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

void ArrowMarker::setColor(const QColor& c)
{
if (linePen().color() == c)
	return;

QPen pen = linePen();
pen.setColor(c);
setLinePen(pen);
}

void ArrowMarker::setWidth(int w)
{
if (linePen().width() == w)
	return;

QPen pen = linePen();
pen.setWidth (w);
setLinePen(pen);
}

void ArrowMarker::setStyle(Qt::PenStyle style)
{
if (linePen().style() == style)
	return;

QPen pen = linePen();
pen.setStyle(style);
setLinePen(pen);
}

void ArrowMarker::setHeadLength(int l)
{
if (d_head_length == l)
	return;

d_head_length=l;
}

void ArrowMarker::setHeadAngle(int a)
{
if (d_head_angle == a)
	return;

d_head_angle=a;
}

void ArrowMarker::fillArrowHead(bool fill)
{
if (d_fill_head == fill)
	return;

d_fill_head=fill;
}

void ArrowMarker::setStartPoint(const QPoint& p)
{
if (d_start == p)
	return;

d_start = p;

if (!plot())
	return;

d_rect.setLeft(plot()->invTransform(xAxis(), p.x()));
d_rect.setTop(plot()->invTransform(yAxis(), p.y()));
}

void ArrowMarker::setEndPoint(const QPoint& p)
{
if (d_end == p)
	return;

d_end = p;

if (!plot())
	return;

d_rect.setRight(plot()->invTransform(xAxis(), p.x()));
d_rect.setBottom(plot()->invTransform(yAxis(), p.y()));
}

QPoint ArrowMarker::startPoint() const
{
    if (!plot())
		return QPoint();

    return QPoint(plot()->transform(xAxis(), d_rect.left()),
                plot()->transform(yAxis(), d_rect.top()));
}

QwtDoublePoint ArrowMarker::startPointCoord()
{
	return QwtDoublePoint(d_rect.left(), d_rect.top());
}

void ArrowMarker::setStartPoint(double x, double y)
{
if (d_rect.left() == x && d_rect.top() == y)
	return;

d_rect.setLeft(x);
d_rect.setTop(y);

if (!plot())
	return;

plot()->updateLayout();
d_start = QPoint(plot()->transform(xAxis(), x), plot()->transform(yAxis(), y));
}

QPoint ArrowMarker::endPoint() const
{
    if (!plot())
		return QPoint();

	return QPoint(plot()->transform(xAxis(), d_rect.right()),
			plot()->transform(yAxis(), d_rect.bottom()));
}

void ArrowMarker::setEndPoint(double x, double y)
{
if (d_rect.right() == x && d_rect.bottom() == y)
	return;

d_rect.setRight(x);
d_rect.setBottom(y);

if (!plot())
	return;

plot()->updateLayout();
d_end = QPoint(plot()->transform(xAxis(), x), plot()->transform(yAxis(), y));
}

QwtDoublePoint ArrowMarker::endPointCoord()
{
return QwtDoublePoint(d_rect.right(), d_rect.bottom());
}

void ArrowMarker::setBoundingRect(double xs, double ys, double xe, double ye)
{
if (d_rect.left() == xs && d_rect.top() == ys &&
	d_rect.right() == xe && d_rect.bottom() == ye)
	return;

d_rect.setLeft(xs);
d_rect.setTop(ys);
d_rect.setRight(xe);
d_rect.setBottom(ye);

if (!plot())
	return;

plot()->updateLayout();
d_start = QPoint(plot()->transform(xAxis(), xs), plot()->transform(yAxis(), ys));
d_end = QPoint(plot()->transform(xAxis(), xe), plot()->transform(yAxis(), ye));
}

QwtDoubleRect ArrowMarker::boundingRect() const
{
	const QwtScaleMap &xMap = plot()->canvasMap(xAxis());
	const QwtScaleMap &yMap = plot()->canvasMap(yAxis());

	const int x0 = xMap.transform(d_rect.left());
	const int y0 = yMap.transform(d_rect.top());
	const int x1 = xMap.transform(d_rect.right());
	const int y1 = yMap.transform(d_rect.bottom());

	return QwtDoubleRect(
			x0<x1 ? d_rect.left() : d_rect.right(),
			y0<y1 ? d_rect.top() : d_rect.bottom(),
			qAbs(d_rect.left() - d_rect.right()),
			qAbs(d_rect.top() - d_rect.bottom()));
}

void ArrowMarker::updateBoundingRect()
{
const QwtScaleMap &xMap = plot()->canvasMap(xAxis());
const QwtScaleMap &yMap = plot()->canvasMap(yAxis());

d_rect.setLeft(xMap.invTransform(d_start.x()));
d_rect.setTop(yMap.invTransform(d_start.y()));
d_rect.setRight(xMap.invTransform(d_end.x()));
d_rect.setBottom(yMap.invTransform(d_end.y()));
}

void ArrowMarker::setEditable(bool yes)
{
	if (yes == d_editable)
		return;
	if (yes) {
		d_editable = true;
		plot()->canvas()->installEventFilter(this);
	} else {
		d_editable = false;
		plot()->canvas()->removeEventFilter(this);
	}
	plot()->replot();
}

bool ArrowMarker::eventFilter(QObject *, QEvent *e)
{
	switch(e->type()) {
		case QEvent::MouseButtonPress:
			{
				const QMouseEvent *me = (const QMouseEvent *)e;
				if (me->button() != Qt::LeftButton)
					return false;
				QRect handler = QRect (QPoint(0,0), QSize(10, 10));
				handler.moveCenter (startPoint());
				if (handler.contains(me->pos()))
				{
					QApplication::setOverrideCursor(QCursor(Qt::SizeAllCursor), true);
					d_op = MoveStart;
					return true;
				}
				handler.moveCenter (endPoint());
				if (handler.contains(me->pos()))
				{
					QApplication::setOverrideCursor(QCursor(Qt::SizeAllCursor), true);
					d_op = MoveEnd;
					return true;
				}
				int d = width()+(int)floor(headLength()*tan(M_PI*headAngle()/180.0)+0.5);
				if (dist(me->pos().x(),me->pos().y()) <= d)
				{
					QApplication::setOverrideCursor(QCursor(Qt::SizeAllCursor), true);
					d_op = MoveBoth;
					d_op_startat = me->pos()-startPoint();
					return true;
				}
				return false;
			}
		case QEvent::MouseMove:
			{
				const QMouseEvent *me = (const QMouseEvent *)e;
				switch(d_op) {
					case MoveStart:
						setStartPoint(me->pos());
						plot()->replot();
						return true;
					case MoveEnd:
						setEndPoint(me->pos());
						plot()->replot();
						return true;
					case MoveBoth:
						setEndPoint(endPoint()+me->pos()-d_op_startat-startPoint());
						setStartPoint(me->pos()-d_op_startat);
						plot()->replot();
						return true;
					default:
						return false;
				}
			}
		case QEvent::MouseButtonRelease:
			{
				const QMouseEvent *me = (const QMouseEvent *)e;
				switch(d_op) {
					case MoveStart:
						setStartPoint(me->pos());
						plot()->replot();
						d_op = None;
						QApplication::restoreOverrideCursor();
						return true;
					case MoveEnd:
						setEndPoint(me->pos());
						plot()->replot();
						d_op = None;
						QApplication::restoreOverrideCursor();
						return true;
					case MoveBoth:
						setXValue(plot()->invTransform(xAxis(), me->pos().x()-d_op_startat.x()));
						setYValue(plot()->invTransform(yAxis(), me->pos().y()-d_op_startat.y()));
						plot()->replot();
						d_op = None;
						QApplication::restoreOverrideCursor();
						return true;
					default:
						d_op = None;
						QApplication::restoreOverrideCursor();
						return false;
				}
			}
		case QEvent::MouseButtonDblClick:
			{
				const QMouseEvent *me = (const QMouseEvent *)e;
				if (me->button() != Qt::LeftButton)
					return false;
				LineDialog *ld = new LineDialog(this, plot()->window(), "lineDialog", true, Qt::Tool);
				ld->setAttribute(Qt::WA_DeleteOnClose);
				ld->exec();
				return true;
			}
		default:
			return false;
	}
}
