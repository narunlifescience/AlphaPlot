/***************************************************************************
    File                 : VectorCurve.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : Vector curve class

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
#ifndef VECTORCURVE_H
#define VECTORCURVE_H

#include "PlotCurve.h"
#include <qwt_plot.h>

class QwtPlot;

//! Vector curve class
class VectorCurve: public DataCurve
{
public:
	enum VectorStyle{XYXY, XYAM};

	VectorCurve(VectorStyle style, Table *t, const QString& xColName, const char *name,
				const QString& endCol1, const QString& endCol2, int startRow, int endRow);
	~VectorCurve();

	enum Position{Tail, Middle, Head};

	void copy(const VectorCurve *vc);

	QwtDoubleRect boundingRect() const;

	virtual void draw(QPainter *painter,const QwtScaleMap &xMap,
		const QwtScaleMap &yMap, int from, int to) const;

	virtual void drawVector(QPainter *painter, const QwtScaleMap &xMap,
		const QwtScaleMap &yMap, int from, int to) const;

	void drawArrowHead(QPainter *p, int xs, int ys, int xe, int ye) const;
	double theta(int x0, int y0, int x1, int y1) const;

	QString vectorEndXAColName(){return d_end_x_a;};
	QString vectorEndYMColName(){return d_end_y_m;};
	void setVectorEnd(const QString& xColName, const QString& yColName);
	void setVectorEnd(const QwtArray<double>&x, const QwtArray<double>&y);

	int width();
	void setWidth(int w);

	QColor color();
	void setColor(const QColor& c);

	int headLength(){return d_headLength;};
	void setHeadLength(int l);

	int headAngle(){return d_headAngle;};
	void setHeadAngle(int a);

	bool filledArrowHead(){return filledArrow;};
	void fillArrowHead(bool fill);

	int position(){return d_position;};
	void setPosition(int pos){d_position = pos;};

	int vectorStyle(){return d_style;};
	void setVectorStyle(int style){d_style = style;};

	bool updateData(Table *t, const QString& colName);
	void loadData();

    QString plotAssociation();
	void updateColumnNames(const QString& oldName, const QString& newName, bool updateTableName);

protected:
	QwtArrayData *vectorEnd;
	QPen pen;
	bool filledArrow;
	int d_style, d_headLength, d_headAngle, d_position;

	QString d_end_x_a;
	QString d_end_y_m;
};

#endif
