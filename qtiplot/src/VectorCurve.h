#ifndef VECTORCURVE_H
#define VECTORCURVE_H

#include <qwt_plot_curve.h>
#include <qwt_plot.h>

class QwtPlot;
class QwtPlotCurve;

class VectorCurve: public QwtPlotCurve
{
public:
	enum VectorStyle{XYXY, XYAM};

	VectorCurve(VectorStyle style, QwtPlot *parent, const char *name=0);
	~VectorCurve();

	enum Position{Tail, Middle, Head};

	void copy(const VectorCurve *vc);

	QwtDoubleRect boundingRect() const;

	virtual void draw(QPainter *painter,const QwtScaleMap &xMap, 
		const QwtScaleMap &yMap, int from, int to);

	virtual void drawVector(QPainter *painter, const QwtScaleMap &xMap, 
		const QwtScaleMap &yMap, int from, int to);
	
	void drawArrowHead(QPainter *p, int xs, int ys, int xe, int ye);
	double teta(int x0, int y0, int x1, int y1);

	void setVectorEnd(const QwtArray<double>&x, const QwtArray<double>&y);

	int width();
	void setWidth(int w);

	QColor color();
	void setColor(const QColor& c);
	
	int headLength(){return d_headLength;};
	void setHeadLength(int l);
	
	int headAngle(){return d_headAngle;};
	void setHeadAngle(int a);
	
	bool filledArrowHead(){return filledArrow;};
	void fillArrowHead(bool fill);

	int position(){return d_position;};
	void setPosition(int pos){d_position = pos;};
	
	int vectorStyle(){return d_style;};
	void setVectorStyle(int style){d_style = style;};

protected:
	QwtArrayData *vectorEnd;
	QPen pen;
	bool filledArrow;
	int d_style, d_headLength, d_headAngle, d_position;
};

#endif

