/***************************************************************************
    File                 : QwtErrorPlotCurve.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
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

#include "PlotCurve.h"
#include <qwt_plot.h>

//! Error bars curve
class QwtErrorPlotCurve: public DataCurve
{
public:
	enum Orientation{Horizontal = 0, Vertical = 1};

	QwtErrorPlotCurve(int orientation, Table *t, const char *name);
	QwtErrorPlotCurve(Table *t, const char *name);

	void copy(const QwtErrorPlotCurve *e);

	QwtDoubleRect boundingRect() const;

	double errorValue(int i);
	QwtArray<double> errors(){return err;};
	void setErrors(const QwtArray<double>&data){err=data;};

	int capLength(){return cap;};
	void setCapLength(int t){cap=t;};

	int width(){return pen().width ();};
	void setWidth(int w);

	QColor color(){return pen().color();};
	void setColor(const QColor& c);

	int direction(){return type;};
	void setDirection(int o){type = o;};

	bool xErrors();
	void setXErrors(bool yes);

	bool throughSymbol(){return through;};
	void drawThroughSymbol(bool yes){through=yes;};

	bool plusSide(){return plus;};
	void drawPlusSide(bool yes){plus=yes;};

	bool minusSide(){return minus;};
	void drawMinusSide(bool yes){minus=yes;};

	//! Returns the master curve to which this error bars curve is attached.
	DataCurve* masterCurve(){return d_master_curve;};
	void setMasterCurve(DataCurve *c);

	//! Causes the master curve to delete this curve from its managed error bars list.
	void detachFromMasterCurve(){d_master_curve->removeErrorBars(this);};

    QString plotAssociation();

	bool updateData(Table *t, const QString& colName);
	void loadData();

private:
	virtual void draw(QPainter *painter,const QwtScaleMap &xMap,
		const QwtScaleMap &yMap, int from, int to) const;

	void drawErrorBars(QPainter *painter, const QwtScaleMap &xMap,
		const QwtScaleMap &yMap, int from, int to) const;

    //! Stores the error bar values
    QwtArray<double> err;

	//! Orientation of the bars: Horizontal or Vertical
	int type;

	//! Length of the bar cap decoration
	int cap;

	bool plus, minus, through;

	//! Reference to the master curve to which this error bars curve is attached.
	DataCurve *d_master_curve;
};

#endif
