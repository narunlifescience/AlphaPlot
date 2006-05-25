#include "Histogram.h"
#include <qpainter.h>

//FIXME: All functionality disabled for now (needs port to Qwt5)

QwtHistogram::QwtHistogram(QwtPlot *parent, const char *name):
	QwtBarCurve( parent, name )
{}

void QwtHistogram::copy(const QwtHistogram *h)
{
	QwtBarCurve::copy((const QwtBarCurve *)h);

	d_autoBin = h->d_autoBin;
	d_binSize = h->d_binSize;
	d_begin = h->d_begin;
	d_end = h->d_end;
}

void QwtHistogram::draw(QPainter *painter,
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
		painter->setPen(QwtPlotCurve::pen());
		painter->setBrush(QwtPlotCurve::brush());

		const int ref= yMap.transform(baseline());
		const int dx=abs(xMap.transform(x(from+1)) - xMap.transform(x(from)));
		const int bar_width=int(dx*(1-gap()*0.01));
		const int half_width = int(0.5*(dx-bar_width));
		const int xOffset = int(0.01*offset()*bar_width);

		for (int i=from; i<=to; i++)
		{
			const int px1 = xMap.transform(x(i));
			const int py1 = yMap.transform(y(i));
			painter->drawRect(px1+half_width+xOffset,py1,bar_width+1,(ref-py1+1));
		}

		painter->restore();
	}
#endif
}

QwtDoubleRect QwtHistogram::boundingRect() const
{
#if false
	QwtDoubleRect rect = QwtCurve::boundingRect();
	rect.setX1(rect.x1()-x(1));
	rect.setX2(rect.x2()+x(dataSize()-1));
	rect.setY1(0);
	rect.setY2(1.2*rect.y2());
	return rect;
#endif
}

void QwtHistogram::setBinning(bool autoBin, double begin, double end, double size)
{
#if false
	d_autoBin = autoBin;
	d_binSize = size;
	d_begin = begin;
	d_end = end;
#endif
}

