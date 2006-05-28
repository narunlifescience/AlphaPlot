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

#include <qwt_plot.h>
//Added by qt3to4:
#include <QMouseEvent>
#include <Q3ValueList>

//! Plot window class
class Plot: public QwtPlot
{	
    Q_OBJECT

public:	
	Plot(QWidget *parent = 0, const char *name = 0);
	
	enum TicksType{In=-1, None = 0, Out=1, Both=2};
	
	Q3ValueList <int> getTicksType(){return ticksType;};
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

	void drawPixmap(QPainter *painter, const QRect &rect);
	virtual void print(QPainter *, const QRect &rect,
        const QwtPlotPrintFilter & = QwtPlotPrintFilter()) const;
	
protected:
	void printCanvas(QPainter *painter, const QRect &canvasRect,
   			 const QwtArray<QwtScaleMap> &map, const QwtPlotPrintFilter &pfilter) const;

	virtual void drawCanvasItems (QPainter *painter, const QRect &rect,
			const QwtArray< QwtScaleMap > &map, const QwtPlotPrintFilter &pfilter) const;

	void drawInwardTicks(QPainter *painter, const QRect &rect, 
							const QwtScaleMap&map, int axis) const;

	void drawInwardMinorTicks(QPainter *painter, const QRect &rect, 
							const QwtScaleMap &map, int axis) const;
signals:
	void selectPlot();
	void moveGraph(const QPoint&);
	void releasedGraph();
	void resizeGraph(const QPoint&);
	void resizedGraph();

protected:
	int minTickLength, majTickLength, d_lineWidth;
	Q3ValueList <int> ticksType;
	bool movedGraph, ShiftButton, graphToResize;
};

#endif 

