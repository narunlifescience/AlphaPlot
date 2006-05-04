#ifndef LEGENDMARKER_H
#define LEGENDMARKER_H

#include <qfont.h>
#include <qpen.h>

#include <qwt_plot.h>
#include <qwt_plot_classes.h>
#include <qwt_marker.h>
#include <qwt_array.h>
#include <qwt_text.h>

#include "graph.h"

class Graph;
	
class LegendMarker: public QwtPlotMarker
{
public:
    LegendMarker(QwtPlot *);
	~LegendMarker();

	enum FrameStyle{None = 0, Line = 1, Shadow=2};
	
    virtual void draw(QPainter *p, int x, int y, const QRect &);

	QString getText();
	void setText(const QString& s);

	QRect rect();
	void setOrigin(const QPoint & p);

	QColor getTextColor();
	void setTextColor(const QColor& c);
	
	QColor backgroundColor(){return bkgColor;};
	void setBackgroundColor(const QColor& c);

	int getBkgType();
	void setBackground(int bkg);

	QFont getFont();
	void setFont(const QFont& font);

	void setAngle(int ang);	
	int getAngle();
	
	void drawFrame(QPainter *p, int type, const QRect& rect);
	void drawSymbols(QPainter *p, const QRect& rect,QwtArray<long> height);
	void drawLegends(QPainter *p, const QRect& rect, QwtArray<long> height);
	void drawVector(QPainter *p, int x, int y, int l, int curveIndex);
														
	QRect scaledLegendRect(QPainter *p, const QPoint& canvas_origin, const QRect& rect);
	int symbolsMaxLineLength();

	Graph * parentGraph(QwtPlot *plot);

protected:
	QwtArray<long> heights;
	QRect lRect;
	int bkgType,angle,symbolLineLength;
	QwtText* d_text;
	QColor bkgColor;
};

#endif
