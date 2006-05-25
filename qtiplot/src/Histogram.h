#include "BarCurve.h"

class QwtHistogram: public QwtBarCurve
{
public:
	QwtHistogram(QwtPlot *parent, const char *name=0);

	void copy(const QwtHistogram *h);

	virtual void draw(QPainter *painter,const QwtScaleMap &xMap, 
		const QwtScaleMap &yMap, int from, int to);

	QwtDoubleRect boundingRect() const;

	void setBinning(bool autoBin, double begin, double end, double size);
	bool autoBinning(){return d_autoBin;};
	double begin(){return d_begin;};
	double end(){return d_end;};
	double binSize(){return d_binSize;};

private:
	bool d_autoBin;
	double d_binSize, d_begin, d_end;
};

