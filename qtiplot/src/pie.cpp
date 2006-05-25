#include "pie.h"
#include "colorBox.h"

#include <qwt_painter.h>
#include <qpainter.h>
#include <qpaintdevicemetrics.h>

//FIXME: All functionality disabled for now (needs port to Qwt5)

QwtPieCurve::QwtPieCurve(QwtPlot *parent, const char *name):
	QwtPlotCurve()
{
#if false
	pieRay=100;
	firstColor=0;
	setPen(QPen(QColor(Qt::black),1,Qt::SolidLine));
	setBrush(QBrush(Qt::black,Qt::SolidPattern));
#endif
}

void QwtPieCurve::draw(QPainter *painter,
		const QwtScaleMap &xMap, const QwtScaleMap &yMap, int from, int to)
{
#if false
	if ( !painter || dataSize() <= 0 )
		return;

	if (to < 0)
		to = dataSize() - 1;

	if ( verifyRange(from, to) > 0 )
		drawPie(painter, xMap, yMap, from, to);
#endif
}

void QwtPieCurve::drawPie(QPainter *painter,
		const QwtScaleMap &xMap, const QwtScaleMap &yMap, int from, int to)
{   	
#if false
	int i, d = pieRay*2;
	QwtPlot *plot = (QwtPlot *)parentPlot();
	QRect rect = plot->rect();
	int x_center = rect.x() + rect.width()/2;
	int y_center = rect.y() + rect.height()/2;

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
#endif
}

QColor QwtPieCurve::color(int i)
{
#if false
	int index=(firstColor+i)%16;
	return ColorBox::color(index);
#endif
}

void QwtPieCurve::setBrushStyle(const Qt::BrushStyle& style)
{
#if false
	QBrush br = QwtPlotCurve::brush();
	if (br.style() == style)
		return;

	br.setStyle(style);
	setBrush(br);
#endif
}

