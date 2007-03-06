#include <qapplication.h>
#include <qpainter.h>
#include <qpen.h>

#include <qwt_array.h>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_marker.h>

class QwtErrorPlotCurve: public QwtPlotCurve
{
public:
    QwtErrorPlotCurve::QwtErrorPlotCurve(
	QwtPlot *parent, const char *name = 0):
	QwtPlotCurve(parent,name) {};
    virtual void draw(QPainter *painter,
		      const QwtScaleMap &xMap, const QwtScaleMap &yMap,
		      int from = 0, int to = -1);
private:
    void QwtErrorPlotCurve::drawErrorBars(
	QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap,
	int from, int to);

};

void QwtErrorPlotCurve::draw(QPainter *painter,
			     const QwtScaleMap &xMap, const QwtScaleMap &yMap,
			     int from, int to)
{
    if ( !painter || dataSize() <= 0 )
	return;
    
    if (to < 0)
	to = dataSize() - 1;
    
    if ( verifyRange(from, to) > 0 ) {
	painter->save();
	painter->setPen(QPen(Qt::red,2,Qt::SolidLine));
	painter->setBrush(QBrush(Qt::red));
	drawErrorBars(painter, xMap, yMap, from, to);
	painter->restore();
    }
}

void QwtErrorPlotCurve::drawErrorBars(
    QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap,
    int from, int to)
{
    for (int i = from; i <= to; i++) {
	const int xi = xMap.transform(x(i));
	const int yh = yMap.transform(y(i)+0.5);
	const int yl = yMap.transform(y(i)-0.5);
	painter->drawLine(xi,yh,xi,yl); // +/- 0.5  vertical error bars
    }
}

int main(int argc, char **argv)
{
    QApplication a(argc, argv);
    QwtPlot plot;
    a.setMainWidget(&plot);
    plot.show();
    QwtArray<double> x(100), y(100);
    for (unsigned int i = 0; i<x.size(); i++) {
	x[i] = 0.1*i;
	y[i] = sin(x[i]);
    }
	
    QwtErrorPlotCurve *errors=new QwtErrorPlotCurve(&plot,0);
    errors->setData(x,y);
    plot.insertCurve(errors);
    
    plot.replot();

    return a.exec();
}

