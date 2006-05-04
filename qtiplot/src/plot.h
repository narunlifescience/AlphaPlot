#ifndef PLOT_H
#define PLOT_H

#include <qobject.h>

#include <qwt_plot.h>

class Plot: public QwtPlot
{	
    Q_OBJECT

public:	
	Plot(QWidget *parent = 0, const char *name = 0);
	
	enum TicksType{In=-1, None = 0, Out=1, Both=2};
	
	QValueList <int> getTicksType(){return ticksType;};
	void setTicksType(int axis, int type);

	int minorTickLength(){return minTickLength;};
	int majorTickLength(){return majTickLength;};
	void setTickLength (int minLength, int majLength);

	void setTicksLineWidth(int width);
	void printFrame(QPainter *painter, const QRect &rect) const;

	QColor frameColor();

	void mousePressEvent ( QMouseEvent * e );
	void mouseReleaseEvent ( QMouseEvent * e );
	void mouseMoveEvent ( QMouseEvent * e );
	
protected:
	void printCanvas(QPainter *painter, const QRect &canvasRect,
   			 const QwtArray<QwtDiMap> &map, const QwtPlotPrintFilter &pfilter) const;

	virtual void drawCanvasItems (QPainter *painter, const QRect &rect,
			const QwtArray< QwtDiMap > &map, const QwtPlotPrintFilter &pfilter) const;

	void drawInwardTicks(QPainter *painter, const QRect &rect, 
							const QwtDiMap&map, int axis) const;

	void drawInwardMinorTicks(QPainter *painter, const QRect &rect, 
							const QwtDiMap &map, int axis) const;
signals:
	void selectPlot();
	void moveGraph(const QPoint&);
	void releasedGraph();
	void resizeGraph(const QPoint&);
	void resizedGraph();

protected:
	int minTickLength, majTickLength, d_lineWidth;
	QValueList <int> ticksType;
	bool movedGraph, ShiftButton, graphToResize;
};

#endif 



