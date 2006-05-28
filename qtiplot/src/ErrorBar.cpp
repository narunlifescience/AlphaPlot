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
#include <qpainter.h>

//FIXME: All functionality disabled for now (needs port to Qwt5)


QwtErrorPlotCurve::QwtErrorPlotCurve(int orientation, QwtPlot *parent, const char *name):
    QwtPlotCurve()
{
#if false
pen=QPen(Qt::black,2,Qt::SolidLine);
cap=10;
type = orientation;
size=QSize(1,1);
plus=TRUE;
minus=TRUE;
through=FALSE;
#endif
}

QwtErrorPlotCurve::QwtErrorPlotCurve(QwtPlot *parent, const char *name):
    QwtPlotCurve()
{
#if false
pen=QPen(Qt::black,2,Qt::SolidLine);
cap=10;
type = Qt::Vertical;
size=QSize(1,1);
plus=TRUE;
minus=TRUE;
through=FALSE;
#endif
}

void QwtErrorPlotCurve::copy(const QwtErrorPlotCurve *e)
{
#if false
cap = e->cap;
type = e->type;
size = e->size;
plus = e->plus;
minus = e->minus;
through = e->through;
pen = e->pen;
err = e->err.copy();
	
setTitle(e->title());
#endif
}

void QwtErrorPlotCurve::draw(QPainter *painter,
    const QwtScaleMap &xMap, const QwtScaleMap &yMap, int from, int to)
{
#if false
    if ( !painter || dataSize() <= 0 )
        return;

    if (to < 0)
        to = dataSize() - 1;

    if ( verifyRange(from, to) > 0 )
    {
        painter->save();
        painter->setPen(pen);
		drawErrorBars(painter, xMap, yMap, from, to);
        painter->restore();
    }
#endif
}

void QwtErrorPlotCurve::drawErrorBars(QPainter *painter,
    const QwtScaleMap &xMap, const QwtScaleMap &yMap, int from, int to)
{   
#if false
int sh=size.height();
int sw=size.width();

 for (int i = from; i <= to; i++)
	{
	const int xi = xMap.transform(x(i));
	const int yi = yMap.transform(y(i));
	
	if (type == Qt::Vertical)
			{
			const int yh = yMap.transform(y(i)+err[i]);
			const int yl = yMap.transform(y(i)-err[i]);
			const int yhl=yi-qRound(0.5*sh)-pen.width();
			const int ylh=yi+qRound(0.5*sh)+pen.width();

			if (abs(yl-yh)>sh)
				{
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
			}
		else if (type == Qt::Horizontal)
			{
			const int xp = xMap.transform(x(i)+err[i]);
			const int xm = xMap.transform(x(i)-err[i]);

			const int xpm = xi+qRound(0.5*sw)+pen.width();
			const int xmp = xi-qRound(0.5*sw)-pen.width();


			if (abs(xp-xm)>sw)
			{
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
#endif
}

bool QwtErrorPlotCurve::throughSymbol()
{
#if false
return through;
#endif
}

void QwtErrorPlotCurve::drawThroughSymbol(bool yes)
{
#if false
through=yes;
#endif
}

bool QwtErrorPlotCurve::minusSide()
{
#if false
return minus;
#endif
}

void QwtErrorPlotCurve::drawMinusSide(bool yes)
{
#if false
minus=yes;
#endif
}

bool QwtErrorPlotCurve::plusSide()
{
#if false
return plus;
#endif
}

void QwtErrorPlotCurve::drawPlusSide(bool yes)
{
#if false
plus=yes;
#endif
}

QwtArray<double> QwtErrorPlotCurve::errors()
{
#if false
return err;
#endif
}

void QwtErrorPlotCurve::setErrors(const QwtArray<double>&data)
{
#if false
err=data;
#endif
}

void QwtErrorPlotCurve::setSymbolSize(const QSize& sz)
{
#if false
size=sz;
#endif
}

bool QwtErrorPlotCurve::xErrors()
{
#if false
bool x = false;
if (type == Qt::Horizontal)
	x = true;

return x;
#endif
}

void QwtErrorPlotCurve::setXErrors(bool yes)
{
#if false
if (yes) 
	type = Qt::Horizontal;
else 
	type = Qt::Vertical;
#endif
}

int QwtErrorPlotCurve::capLength()
{
#if false
return cap;
#endif
}

void QwtErrorPlotCurve::setCapLength(int t)
{
#if false
cap=t;
#endif
}

int QwtErrorPlotCurve::width()
{
#if false
return pen.width ();
#endif
}

void QwtErrorPlotCurve::setWidth(int w)
{
#if false
pen.setWidth (w);
#endif
}

QColor QwtErrorPlotCurve::color()
{
#if false
return pen.color();
#endif
}

void QwtErrorPlotCurve::setColor(const QColor& c)
{
#if false
pen.setColor(c);
#endif
}

QwtDoubleRect QwtErrorPlotCurve::boundingRect() const
{
#if false
QwtDoubleRect rect = QwtCurve::boundingRect();

int size = dataSize();

QwtArray <double> X(size), Y(size), min(size), max(size);
for (int i=0; i<size; i++)
	{
	X[i]=x(i);
	Y[i]=y(i);
	if (type == Qt::Vertical)
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
if (type == Qt::Vertical)
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
	
rect.setY1(QMIN(minrect.y1(), maxrect.y1()));
rect.setY2(QMAX(minrect.y2(), maxrect.y2()));
rect.setX1(QMIN(minrect.x1(), maxrect.x1()));
rect.setX2(QMAX(minrect.x2(), maxrect.x2()));

delete erMin;
delete erMax;

return rect;
#endif
}
