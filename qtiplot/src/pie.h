#include <qwt_plot.h>
#include <qwt_plot_curve.h>

class QwtPieCurve: public QwtPlotCurve
{
public:
	QwtPieCurve(QwtPlot *parent, const char *name=0);

	virtual void draw(QPainter *painter,const QwtScaleMap &xMap, 
		const QwtScaleMap &yMap, int from, int to);

	virtual void drawPie(QPainter *painter, const QwtScaleMap &xMap, 
		const QwtScaleMap &yMap, int from, int to);

public slots:
	QColor color(int i);

	int ray(){return pieRay;};
	void setRay(int size){pieRay=size;};

	Qt::BrushStyle pattern(){return QwtPlotCurve::brush().style();};
	void setBrushStyle(const Qt::BrushStyle& style);

	void setFirstColor(int index){firstColor=index;};
	int first(){return firstColor;};
	
private:
	int pieRay,firstColor;
};
