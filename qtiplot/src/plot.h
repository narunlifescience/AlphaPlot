/***************************************************************************
    File                 : plot.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Plot window class
                           
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *  This program is free software; you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation; either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the Free Software           *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor,                    *
 *   Boston, MA  02110-1301  USA                                           *
 *                                                                         *
 ***************************************************************************/
#ifndef PLOT_H
#define PLOT_H

#include <qobject.h>
#include <qmap.h>

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_marker.h>

//! Plot window class
class Plot: public QwtPlot
{	
    Q_OBJECT

public:	
	Plot(QWidget *parent = 0, const char *name = 0);
	
	enum TicksType{None = 0, Out = 1, Both = 2, In = 3};
	
	QwtPlotGrid *grid(){return d_grid;};
	QList<int> curveKeys(){return d_curves.keys();};

	int insertCurve(QwtPlotCurve *c);
	void removeCurve(int index);

	int closestCurve(int xpos, int ypos, int &dist, int &point);
	QwtPlotCurve* curve(int index){return d_curves[index];};
	QMap<int, QwtPlotCurve*> curves(){return d_curves;};

	QwtDoubleRect boundingRect();

	QwtPlotMarker* marker(int index){return d_markers[index];};
	QList<int> markerKeys(){return d_markers.keys();};
	int insertMarker(QwtPlotMarker *m);
	void removeMarker(int index);

	QList<int> getMajorTicksType(){return majorTicksType;};
	void setMajorTicksType(int axis, int type){majorTicksType[axis]=type;}

	QList<int> getMinorTicksType(){return minorTicksType;};
	void setMinorTicksType(int axis, int type){minorTicksType[axis]=type;}

	int minorTickLength() const;
	int majorTickLength() const;
	void setTickLength (int minLength, int majLength);

	int axesLinewidth() const;
	void setAxesLinewidth(int width);

	void setAxisLabelFormat(int axis, char f, int prec);
    void axisLabelFormat(int axis, char &f, int &prec) const;

	void printFrame(QPainter *painter, const QRect &rect) const;

	QColor frameColor();

	void mousePressEvent ( QMouseEvent * e );
	void mouseReleaseEvent ( QMouseEvent * e );
	void mouseMoveEvent ( QMouseEvent * e );

	void drawPixmap(QPainter *painter, const QRect &rect);
	virtual void print(QPainter *, const QRect &rect,
        const QwtPlotPrintFilter & = QwtPlotPrintFilter()) const;
	
protected:
	/*void printCanvas(QPainter *painter, const QRect &canvasRect,
   			 const QwtArray<QwtScaleMap> &map, const QwtPlotPrintFilter &pfilter) const;*/

	virtual void drawItems (QPainter *painter, const QRect &rect,
			const QwtArray< QwtScaleMap > &map, const QwtPlotPrintFilter &pfilter) const;

	void drawInwardTicks(QPainter *painter, const QRect &rect, 
							const QwtScaleMap&map, int axis) const;

	void drawInwardMinorTicks(QPainter *painter, const QRect &rect, 
							const QwtScaleMap &map, int axis) const;

	void drawInwardMajorTicks(QPainter *painter, const QRect &rect, 
							const QwtScaleMap &map, int axis) const;
signals:
	void selectPlot();
	void moveGraph(const QPoint&);
	void releasedGraph();
	void resizeGraph(const QPoint&);
	void resizedGraph();

protected:
	QwtPlotGrid *d_grid;
	QMap<int, QwtPlotCurve*> d_curves;
	QMap<int, QwtPlotMarker*> d_markers;

	QList<int> minorTicksType;
	QList<int> majorTicksType;

	int minTickLength, majTickLength;
	bool movedGraph, ShiftButton, graphToResize;
	QPoint presspos;
	int marker_key;
	int curve_key;
};

#endif
