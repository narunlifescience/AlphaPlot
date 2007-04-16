/***************************************************************************
    File                 : QwtBarCurve.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : Bar curve

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
#ifndef BARCURVE_H
#define BARCURVE_H

#include "PlotCurve.h"
#include <qwt_plot.h>

//! Bar curve
class QwtBarCurve: public DataCurve
{
public:
	enum BarStyle{Vertical = 0, Horizontal = 1};
	QwtBarCurve(BarStyle style, Table *t, const QString& xColName, const char *name, int startRow, int endRow);

	void copy(const QwtBarCurve *b);

	virtual QwtDoubleRect boundingRect() const;

	BarStyle orientation(){return bar_style;};

	void setGap (int gap);
	int gap() const {return bar_gap;};

	void setOffset(int offset);
	int offset() const {return bar_offset;};

	double dataOffset();

private:
	virtual void draw(QPainter *painter,const QwtScaleMap &xMap,
		const QwtScaleMap &yMap, int from, int to) const;

	int bar_gap, bar_offset;
	BarStyle bar_style;
};

#endif
