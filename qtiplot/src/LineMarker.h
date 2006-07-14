/***************************************************************************
    File                 : LineMarker.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Line marker (extension to QwtPlotMarker)
                           
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
#ifndef LINEMARKER_H
#define LINEMARKER_H

#include <qwt_plot_marker.h>
		
//! Line marker (extension to QwtPlotMarker)
class LineMarker: public QwtPlotMarker
{
public:
    LineMarker(QwtPlot *);
    virtual void draw(QPainter *p, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &r) const;

	void setStartPoint(const QPoint& p);
	void setEndPoint(const QPoint& p);
	QPoint startPoint();
	QPoint endPoint();
	double dist(int x, int y);
	double teta(int xs, int ys, int xe, int ye) const;
	double length();

	QwtDoublePoint coordStartPoint();
	void setCoordStartPoint(const QwtDoublePoint& p);

	QwtDoublePoint coordEndPoint();
	void setCoordEndPoint(const QwtDoublePoint& p);

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
	
	void updateBoundingRect();
	
private:
	QPoint d_start, d_end; 
	QPen pen;
	bool startArrow,endArrow, filledArrow;
	int d_headAngle, d_headLength;
	QwtDoubleRect d_rect;
	QwtPlot *d_plot;
};
#endif
