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
//Added by qt3to4:
#include <Q3PointArray>

//FIXME: All functionality disabled for now (needs port to Qwt5)

VectorCurve::VectorCurve(VectorStyle style, QwtPlot *parent, const char *name):
	QwtPlotCurve()
{
#if false
	d_style = style;
	pen=QPen(Qt::black, 1, Qt::SolidLine);
	filledArrow=true;
	d_headLength=4;
	d_headAngle=45;
	d_position = Tail;
#endif
}

void VectorCurve::copy(const VectorCurve *vc)
{
#if false
	d_style = vc->d_style;
	filledArrow = vc->filledArrow;
	d_headLength = vc->d_headLength;
	d_headAngle = vc->d_headAngle;
	d_position = vc->d_position;
	pen = vc->pen;
	vectorEnd = (QwtArrayData *)vc->vectorEnd->copy();

	setTitle(vc->title());
#endif
}

void VectorCurve::draw(QPainter *painter,
		const QwtScaleMap &xMap, const QwtScaleMap &yMap, int from, int to)
{
#if false
	if ( !painter || dataSize() <= 0 )
		return;

	if (to < 0)
		to = dataSize() - 1;

	QwtPlotCurve::draw(painter, xMap, yMap, from, to);

	if ( verifyRange(from, to) > 0 )
	{
		painter->save();
		painter->setPen(pen);
		drawVector(painter, xMap, yMap, from, to);
		painter->restore();
	}
#endif
}

void VectorCurve::drawVector(QPainter *painter,
		const QwtScaleMap &xMap, const QwtScaleMap &yMap, int from, int to)
{ 
#if false
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
#endif
}

void VectorCurve::drawArrowHead(QPainter *p, int xs, int ys, int xe, int ye)
{
#if false
	p->save();
	p->translate(xe, ye);
	double t=teta(xs, ys, xe, ye);
	p->rotate(-t);

	double pi=4*atan(-1.0);	
	int d=qRound(d_headLength*tan(pi*(double)d_headAngle/180.0));	

	const Q3PointArray endArray(3);	
	endArray[0] = QPoint(0, 0);
	endArray[1] = QPoint(-d_headLength, d);
	endArray[2] = QPoint(-d_headLength, -d);

	if (filledArrow)
		p->setBrush(QBrush(pen.color(), Qt::SolidPattern));

	QwtPainter::drawPolygon(p,endArray);
	p->restore();
#endif
}

double VectorCurve::teta(int x0, int y0, int x1, int y1)
{		
#if false
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

void VectorCurve::setVectorEnd(const QwtArray<double>&x, const QwtArray<double>&y)
{
#if false
	vectorEnd=new QwtArrayData(x, y);
#endif
}

int VectorCurve::width()
{
#if false
	return pen.width ();
#endif
}

void VectorCurve::setWidth(int w)
{
#if false
	pen.setWidth (w);
#endif
}

QColor VectorCurve::color()
{
#if false
	return pen.color();
#endif
}

void VectorCurve::setColor(const QColor& c)
{
#if false
	if (pen.color() != c)
		pen.setColor(c);
#endif
}

void VectorCurve::setHeadLength(int l)
{
#if false
	if (d_headLength != l)
		d_headLength = l;
#endif
}

void VectorCurve::setHeadAngle(int a)
{
#if false
	if (d_headAngle != a)
		d_headAngle = a;
#endif
}

void VectorCurve::fillArrowHead(bool fill)
{
#if false
	if (filledArrow != fill)
		filledArrow = fill;	
#endif
}

QwtDoubleRect VectorCurve::boundingRect() const
{
#if false
	QwtDoubleRect rect = QwtCurve::boundingRect();
	QwtDoubleRect vrect = vectorEnd->boundingRect();

	if (d_style == XYXY)
	{
		rect.setY1(QMIN(rect.y1(), vrect.y1()));
		rect.setY2(QMAX(rect.y2(), vrect.y2()));
		rect.setX1(QMIN(rect.x1(), vrect.x1()));
		rect.setX2(QMAX(rect.x2(), vrect.x2()));
	}
	else
	{
		const double angle = vectorEnd->x(0);
		double mag = vectorEnd->y(0);
		switch(d_position)
		{
			case Tail:
				rect.setY1(QMIN(rect.y1(), rect.y1()+mag*sin(angle)));
				rect.setY2(QMAX(rect.y2(), rect.y2()+mag*sin(angle)));
				rect.setX1(QMIN(rect.x1(), rect.x1()+mag*cos(angle)));
				rect.setX2(QMAX(rect.x2(), rect.x2()+mag*cos(angle)));
				break;

			case Middle:
				{
					mag *= 0.5;
					rect.setY1(QMIN(rect.y1(), rect.y1() - fabs(mag*sin(angle))));
					rect.setY2(QMAX(rect.y2(), rect.y2() + fabs(mag*sin(angle))));
					rect.setX1(QMIN(rect.x1(), rect.x1() - fabs(mag*cos(angle))));
					rect.setX2(QMAX(rect.x2(), rect.x2() + fabs(mag*cos(angle))));
				}
				break;

			case Head:
				rect.setY1(QMIN(rect.y1(), rect.y1() - mag*sin(angle)));
				rect.setY2(QMAX(rect.y2(), rect.y2() - mag*sin(angle)));
				rect.setX1(QMIN(rect.x1(), rect.x1() - mag*cos(angle)));
				rect.setX2(QMAX(rect.x2(), rect.x2() - mag*cos(angle)));
				break;
		}		
	}
	return rect;
#endif
}

VectorCurve::~VectorCurve()
{
#if false
	delete vectorEnd;
#endif
}
