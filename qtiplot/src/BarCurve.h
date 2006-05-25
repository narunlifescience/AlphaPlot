#ifndef BARCURVE_H
#define BARCURVE_H

#include <qwt_plot.h>
#include <qwt_plot_curve.h>

class QwtBarCurve: public QwtPlotCurve
{
public:
	enum BarStyle{Vertical = 0, Horizontal=1};
	QwtBarCurve(QwtPlot *parent, const char *name=0);
	QwtBarCurve(BarStyle style, QwtPlot *parent, const char *name=0);

	void copy(const QwtBarCurve *b);

	virtual void draw(QPainter *painter,const QwtScaleMap &xMap, 
		const QwtScaleMap &yMap, int from, int to);

	virtual QwtDoubleRect boundingRect() const;

	BarStyle orientation(){return bar_style;};

	void setGap (int gap);  
	int gap(){return bar_gap;};

	void setOffset(int offset);
	int offset(){return bar_offset;};
	
private:
	int bar_gap, bar_offset;
	BarStyle bar_style;
};

#endif 

