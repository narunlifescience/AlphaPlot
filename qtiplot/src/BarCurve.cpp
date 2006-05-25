#include "BarCurve.h"
#include <qpainter.h>

//FIXME: All functionality disabled for now (needs port to Qwt5)

QwtBarCurve::QwtBarCurve(QwtPlot *parent, const char *name):
	QwtPlotCurve()
{
#if false
	bar_offset=0;
	bar_gap=0;
	bar_style=Vertical;

	setPen(QPen(Qt::black,1,Qt::SolidLine));
	setBrush(QBrush(Qt::red));
#endif
}

QwtBarCurve::QwtBarCurve(BarStyle style, QwtPlot *parent, const char *name):
	QwtPlotCurve()
{
#if false
	bar_offset=0;
	bar_gap=0;
	bar_style=style;

	setPen(QPen(Qt::black,1,Qt::SolidLine));
	setBrush(QBrush(Qt::red));
#endif
}

void QwtBarCurve::copy(const QwtBarCurve *b)
{
#if false
	bar_gap = b->bar_gap;
	bar_offset = b->bar_offset;
	bar_style = b->bar_style;

	setTitle(b->title());
#endif
}

void QwtBarCurve::draw(QPainter *painter,
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

		int dx,dy,ref,bar_width;

		if (bar_style == Vertical)
			ref= yMap.transform(1e-100); //smalest positive value for log scales
		else
			ref= xMap.transform(1e-100);	

		int i;
		if (bar_style == Vertical)
		{
			dx = abs(xMap.transform(x(from+1))-xMap.transform(x(from)));
			for (i=from+2; i<to; i++)
			{
				int min = abs(xMap.transform(x(i+1))-xMap.transform(x(i)));
				if (min <= dx)
					dx=min;
			}
			bar_width=int(dx*(1-bar_gap*0.01));
		}
		else
		{
			dy = abs(yMap.transform(y(from+1))-yMap.transform(y(from)));
			for (i=from+2; i<to; i++)
			{
				int min = abs(yMap.transform(y(i+1))-yMap.transform(y(i)));
				if (min <= dy)
					dy=min;
			}
			bar_width=int(dy*(1-bar_gap*0.01));
		}

		const int half_width = int((0.5-bar_offset*0.01)*bar_width);
		const int bw1 = bar_width+1;
		for (i=from; i<=to; i++)
		{
			const int px = xMap.transform(x(i));
			const int py = yMap.transform(y(i));

			if (bar_style == Vertical)
			{
				if (y(i) < 0)
					painter->drawRect(px-half_width, ref, bw1, (py-ref));
				else
					painter->drawRect(px-half_width, py, bw1, (ref-py+1));	
			}
			else
			{
				if (x(i) < 0)
					painter->drawRect(px, py-half_width, (ref-px), bw1);
				else
					painter->drawRect(ref, py-half_width,(px-ref), bw1);
			}	
		}
		painter->restore();
	}
#endif
}

QwtDoubleRect QwtBarCurve::boundingRect() const
{
#if false
	QwtDoubleRect rect = QwtCurve::boundingRect();
	double n= (double)dataSize();

	if (bar_style == Vertical)
	{	
		double dx=(rect.x2()-rect.x1())/n;
		rect.setX1(rect.x1()-dx);
		rect.setX2(rect.x2()+dx);
	}
	else
	{	
		double dy=(rect.y2()-rect.y1())/n;
		rect.setY1(rect.y1()-dy);
		rect.setY2(rect.y2()+dy);
	}

	return rect;
#endif
}

void QwtBarCurve::setGap (int gap)   
{
#if false
	if (bar_gap == gap)
		return;

	bar_gap =gap;
#endif
}

void QwtBarCurve::setOffset(int offset)   
{
#if false
	if (bar_offset == offset)
		return;

	bar_offset = offset;
#endif
}
