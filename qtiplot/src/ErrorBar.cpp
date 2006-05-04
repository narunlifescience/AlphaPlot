#include "ErrorBar.h"

#include <qwt_painter.h>
#include <qpainter.h>

QwtErrorPlotCurve::QwtErrorPlotCurve(int orientation, QwtPlot *parent, const char *name):
    QwtPlotCurve(parent,name)
{
pen=QPen(Qt::black,2,Qt::SolidLine);
cap=10;
type = orientation;
size=QSize(1,1);
plus=TRUE;
minus=TRUE;
through=FALSE;
}

QwtErrorPlotCurve::QwtErrorPlotCurve(QwtPlot *parent, const char *name):
    QwtPlotCurve(parent,name)
{
pen=QPen(Qt::black,2,Qt::SolidLine);
cap=10;
type = Vertical;
size=QSize(1,1);
plus=TRUE;
minus=TRUE;
through=FALSE;
}

void QwtErrorPlotCurve::copy(const QwtErrorPlotCurve *e)
{
cap = e->cap;
type = e->type;
size = e->size;
plus = e->plus;
minus = e->minus;
through = e->through;
pen = e->pen;
err = e->err.copy();
	
setTitle(e->title());
}

void QwtErrorPlotCurve::draw(QPainter *painter,
    const QwtDiMap &xMap, const QwtDiMap &yMap, int from, int to)
{
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
}

void QwtErrorPlotCurve::drawErrorBars(QPainter *painter,
    const QwtDiMap &xMap, const QwtDiMap &yMap, int from, int to)
{   
int sh=size.height();
int sw=size.width();

 for (int i = from; i <= to; i++)
	{
	const int xi = xMap.transform(x(i));
	const int yi = yMap.transform(y(i));
	
	if (type == Vertical)
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
		else if (type == Horizontal)
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
}

bool QwtErrorPlotCurve::throughSymbol()
{
return through;
}

void QwtErrorPlotCurve::drawThroughSymbol(bool yes)
{
through=yes;
}

bool QwtErrorPlotCurve::minusSide()
{
return minus;
}

void QwtErrorPlotCurve::drawMinusSide(bool yes)
{
minus=yes;
}

bool QwtErrorPlotCurve::plusSide()
{
return plus;
}

void QwtErrorPlotCurve::drawPlusSide(bool yes)
{
plus=yes;
}

QwtArray<double> QwtErrorPlotCurve::errors()
{
return err;
}

void QwtErrorPlotCurve::setErrors(const QwtArray<double>&data)
{
err=data;
}

void QwtErrorPlotCurve::setSymbolSize(const QSize& sz)
{
size=sz;
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

int QwtErrorPlotCurve::capLength()
{
return cap;
}

void QwtErrorPlotCurve::setCapLength(int t)
{
cap=t;
}

int QwtErrorPlotCurve::width()
{
return pen.width ();
}

void QwtErrorPlotCurve::setWidth(int w)
{
pen.setWidth (w);
}

QColor QwtErrorPlotCurve::color()
{
return pen.color();
}

void QwtErrorPlotCurve::setColor(const QColor& c)
{
pen.setColor(c);
}

QwtDoubleRect QwtErrorPlotCurve::boundingRect() const
{
QwtDoubleRect rect = QwtCurve::boundingRect();

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
	
rect.setY1(QMIN(minrect.y1(), maxrect.y1()));
rect.setY2(QMAX(minrect.y2(), maxrect.y2()));
rect.setX1(QMIN(minrect.x1(), maxrect.x1()));
rect.setX2(QMAX(minrect.x2(), maxrect.x2()));

delete erMin;
delete erMax;

return rect;
}
