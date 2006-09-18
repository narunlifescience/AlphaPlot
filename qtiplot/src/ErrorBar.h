/***************************************************************************
    File                 : ErrorBar.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Error bars curve
                           
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
#ifndef ERRORBARS_H
#define ERRORBARS_H

#include <qwt_plot.h>
#include <qwt_plot_curve.h>

//! Error bars curve
class QwtErrorPlotCurve: public QwtPlotCurve
{
public:
	enum Orientation{Horizontal = 0, Vertical = 1};

	QwtErrorPlotCurve(int orientation, QwtPlot *parent, const char *name);
	QwtErrorPlotCurve(QwtPlot *parent, const char *name=0);
			
	void copy(const QwtErrorPlotCurve *e);

	QwtDoubleRect boundingRect() const;

	virtual void draw(QPainter *painter,const QwtScaleMap &xMap, 
		const QwtScaleMap &yMap, int from, int to) const;

	virtual void drawErrorBars(QPainter *painter, const QwtScaleMap &xMap, 
		const QwtScaleMap &yMap, int from, int to) const;

	QwtArray<double> errors();
	void setErrors(const QwtArray<double>&data);
	void setSymbolSize(const QSize& sz);

	int capLength();
	void setCapLength(int t);

	int width();
	void setWidth(int w);

	QColor color();
	void setColor(const QColor& c);

	int direction(){return type;};
	void setDirection(int o){type = o;};

	bool xErrors();
	void setXErrors(bool yes);

	bool throughSymbol();
	void drawThroughSymbol(bool yes);

	bool plusSide();
	void drawPlusSide(bool yes);

	bool minusSide();
	void drawMinusSide(bool yes);

	double xDataOffset() const;
	void setXDataOffset(double offset){d_xOffset = offset;};

	double yDataOffset() const;
	void setYDataOffset(double offset){d_yOffset = offset;};

private:
    QwtArray<double> err;
	QPen pen;
	QSize size;
	int type, cap;
	bool plus, minus, through;
	double d_xOffset, d_yOffset;
};

#endif


