/***************************************************************************
    File                 : VectorCurve.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Vector curve class
                           
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
#include "VectorCurve.h"

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_painter.h>
#include <qwt_double_rect.h>
#include <qpainter.h>

VectorCurve::VectorCurve(VectorStyle style, QwtPlot *parent, const char *name):
    QwtPlotCurve(name)
{
d_style = style;
pen=QPen(Qt::black, 1, Qt::SolidLine);
filledArrow=true;
d_headLength=4;
d_headAngle=45;
d_position = Tail;
}

void VectorCurve::copy(const VectorCurve *vc)
{
d_style = vc->d_style;
filledArrow = vc->filledArrow;
d_headLength = vc->d_headLength;
d_headAngle = vc->d_headAngle;
d_position = vc->d_position;
pen = vc->pen;
vectorEnd = (QwtArrayData *)vc->vectorEnd->copy();
	
setTitle(vc->title());
}

void VectorCurve::draw(QPainter *painter,
    const QwtScaleMap &xMap, const QwtScaleMap &yMap, int from, int to) const
{
    if ( !painter || dataSize() <= 0 )
        return;
	
    if (to < 0)
        to = dataSize() - 1;
	
	QwtPlotCurve::draw(painter, xMap, yMap, from, to);

    painter->save();
    painter->setPen(pen);
    drawVector(painter, xMap, yMap, from, to);
    painter->restore();
}

void VectorCurve::drawVector(QPainter *painter,
    const QwtScaleMap &xMap, const QwtScaleMap &yMap, int from, int to) const
{ 
if (d_style == XYAM)
{
 for (int i = from; i <= to; i++)
	{
	const double x0 = x(i);
	const double y0 = y(i);
	const double angle = vectorEnd->x(i);
	const double mag = vectorEnd->y(i);

	int xs, ys, xe, ye;
	switch(d_position)
		{
		case Tail:
			xs = xMap.transform(x0);
			ys = yMap.transform(y0);
			xe = xMap.transform(x0 + mag*cos(angle));
			ye = yMap.transform(y0 + mag*sin(angle));
		break;

		case Middle:
			{
			double dxh = 0.5*mag*cos(angle);
			double dyh = 0.5*mag*sin(angle);
			xs = xMap.transform(x0 - dxh);
			ys = yMap.transform(y0 - dyh);
			xe = xMap.transform(x0 + dxh);
			ye = yMap.transform(y0 + dyh);
			}
		break;

		case Head:
			xs = xMap.transform(x0 - mag*cos(angle));
			ys = yMap.transform(y0 - mag*sin(angle));
			xe = xMap.transform(x0);
			ye = yMap.transform(y0);
		break;
		}		
	QwtPainter::drawLine(painter, xs, ys, xe, ye);
	drawArrowHead(painter, xs, ys, xe, ye);
	}
}
else
	{
	for (int i = from; i <= to; i++)
		{
		const int xs = xMap.transform(x(i));
		const int ys = yMap.transform(y(i));			
		const int xe = xMap.transform(vectorEnd->x(i));
		const int ye = yMap.transform(vectorEnd->y(i));	
		QwtPainter::drawLine(painter, xs, ys, xe, ye);
		drawArrowHead(painter, xs, ys, xe, ye);
		}
	}
}

void VectorCurve::drawArrowHead(QPainter *p, int xs, int ys, int xe, int ye) const
{
p->save();
p->translate(xe, ye);
double t=teta(xs, ys, xe, ye);
p->rotate(-t);
	
double pi=4*atan(-1.0);	
int d=qRound(d_headLength*tan(pi*(double)d_headAngle/180.0));	
	
QPolygon endArray(3);	
endArray[0] = QPoint(0, 0);
endArray[1] = QPoint(-d_headLength, d);
endArray[2] = QPoint(-d_headLength, -d);

if (filledArrow)
	p->setBrush(QBrush(pen.color(), Qt::SolidPattern));

QwtPainter::drawPolygon(p,endArray);
p->restore();
}

double VectorCurve::teta(int x0, int y0, int x1, int y1) const
{		
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

void VectorCurve::setVectorEnd(const QwtArray<double>&x, const QwtArray<double>&y)
{
vectorEnd=new QwtArrayData(x, y);
}

int VectorCurve::width()
{
return pen.width ();
}

void VectorCurve::setWidth(int w)
{
pen.setWidth (w);
}

QColor VectorCurve::color()
{
return pen.color();
}

void VectorCurve::setColor(const QColor& c)
{
if (pen.color() != c)
	pen.setColor(c);
}

void VectorCurve::setHeadLength(int l)
{
if (d_headLength != l)
	d_headLength = l;
}
	
void VectorCurve::setHeadAngle(int a)
{
if (d_headAngle != a)
	d_headAngle = a;
}
		
void VectorCurve::fillArrowHead(bool fill)
{
if (filledArrow != fill)
	filledArrow = fill;	
}

QwtDoubleRect VectorCurve::boundingRect() const
{
QwtDoubleRect rect = QwtPlotCurve::boundingRect();
QwtDoubleRect vrect = vectorEnd->boundingRect();
	
if (d_style == XYXY)
	{
	rect.setTop(QMIN(rect.top(), vrect.top()));
	rect.setBottom(QMAX(rect.bottom(), vrect.bottom()));
	rect.setLeft(QMIN(rect.left(), vrect.left()));
	rect.setRight(QMAX(rect.right(), vrect.right()));
	}
else
	{
	const double angle = vectorEnd->x(0);
	double mag = vectorEnd->y(0);
	switch(d_position)
		{
		case Tail:
			rect.setTop(QMIN(rect.top(), rect.top()+mag*sin(angle)));
			rect.setBottom(QMAX(rect.bottom(), rect.bottom()+mag*sin(angle)));
			rect.setLeft(QMIN(rect.left(), rect.left()+mag*cos(angle)));
			rect.setRight(QMAX(rect.right(), rect.right()+mag*cos(angle)));
		break;

		case Middle:
			{
			mag *= 0.5;
			rect.setTop(QMIN(rect.top(), rect.top() - fabs(mag*sin(angle))));
			rect.setBottom(QMAX(rect.bottom(), rect.bottom() + fabs(mag*sin(angle))));
			rect.setLeft(QMIN(rect.left(), rect.left() - fabs(mag*cos(angle))));
			rect.setRight(QMAX(rect.right(), rect.right() + fabs(mag*cos(angle))));
			}
		break;

		case Head:
			rect.setTop(QMIN(rect.top(), rect.top() - mag*sin(angle)));
			rect.setBottom(QMAX(rect.bottom(), rect.bottom() - mag*sin(angle)));
			rect.setLeft(QMIN(rect.left(), rect.left() - mag*cos(angle)));
			rect.setRight(QMAX(rect.right(), rect.right() - mag*cos(angle)));
		break;
		}		
	}
return rect;
}

VectorCurve::~VectorCurve()
{
delete vectorEnd;
}
