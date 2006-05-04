#ifndef LINEMARKER_H
#define LINEMARKER_H

#include <qwt_plot_classes.h>
#include <qwt_marker.h>
	
class LineMarker: public QwtPlotMarker
{
public:
    LineMarker(QwtPlot *);
    virtual void draw(QPainter *p, int x, int y, const QRect &);

	void setStartPoint(const QPoint& p);
	void setEndPoint(const QPoint& p);
	QPoint startPoint();
	QPoint endPoint();
	double dist(int x, int y);
	double teta();
	double length();

	void setColor(const QColor& c);
	QColor color();

	void setWidth(int w);
	int width();

	void setStyle(Qt::PenStyle s);
	Qt::PenStyle style();

	void setStartArrow(bool on);
	bool getStartArrow();
	void setEndArrow(bool on);
	bool getEndArrow();
	
	int headLength(){return d_headLength;};
	void setHeadLength(int l);
	
	int headAngle(){return d_headAngle;};
	void setHeadAngle(int a);
	
	bool filledArrowHead(){return filledArrow;};
	void fillArrowHead(bool fill);
	
	static QwtDiMap mapCanvasToDevice(QPainter *p, QwtPlot *plot, int axis) ;
		
private:
	QPoint start,end;
	QPen pen;
	bool startArrow,endArrow, filledArrow;
	int d_headAngle, d_headLength;
};
#endif
