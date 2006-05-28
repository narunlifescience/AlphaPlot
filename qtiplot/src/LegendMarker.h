/***************************************************************************
    File                 : LegendMarker.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Legend marker (extension to QwtPlotMarker)
                           
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
#ifndef LEGENDMARKER_H
#define LEGENDMARKER_H

#include <qfont.h>
#include <qpen.h>

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_marker.h>
#include <qwt_array.h>
#include <qwt_text.h>

#include "graph.h"

class Graph;
	
//! Legend marker (extension to QwtPlotMarker)
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

