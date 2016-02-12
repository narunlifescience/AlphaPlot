/***************************************************************************
    File                 : QwtErrorPlotCurve.cpp
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Benkert
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
#include "QwtErrorPlotCurve.h"
#include "QwtBarCurve.h"
#include "core/column/Column.h"

#include <qwt_painter.h>
#include <qwt_symbol.h>

#include <QPainter>
#include <QLocale>

QwtErrorPlotCurve::QwtErrorPlotCurve(int orientation, Table *t, const char *name):
	DataCurve(t, QString(), name),
	d_master_curve(NULL)
{
	cap = 10;
	type = orientation;
	plus = true;
	minus = true;
	through = false;
	setType(Graph::ErrorBars);
}

QwtErrorPlotCurve::QwtErrorPlotCurve(Table *t, const char *name):
	DataCurve(t, QString(), name),
	d_master_curve(NULL)
{
	cap=10;
	type = Vertical;
	plus = true;
	minus = true;
	through = false;
	setType(Graph::ErrorBars);
}

void QwtErrorPlotCurve::copy(const QwtErrorPlotCurve *e)
{
	cap = e->cap;
	type = e->type;
	plus = e->plus;
	minus = e->minus;
	through = e->through;
	setPen(e->pen());
	err = e->err;
}

void QwtErrorPlotCurve::draw(QPainter *painter,
		const QwtScaleMap &xMap, const QwtScaleMap &yMap, int from, int to) const
{
	if ( !painter || dataSize() <= 0 )
		return;

	if (to < 0)
		to = dataSize() - 1;

  painter->save();
  painter->setPen(pen());
  drawErrorBars(painter, xMap, yMap, from, to);
  painter->restore();
}

void QwtErrorPlotCurve::drawErrorBars(QPainter *painter,
		const QwtScaleMap &xMap, const QwtScaleMap &yMap, int from, int to) const
{
    int sh = 0;
    // int sw = 0;
    const QwtSymbol symbol = d_master_curve->symbol();
    if (symbol.style() != QwtSymbol::NoSymbol){
        sh = symbol.size().height();
        // sw = symbol.size().width();
    }

	double d_xOffset = 0.0;
	double d_yOffset = 0.0;
	if (d_master_curve->type() == Graph::VerticalBars)
		d_xOffset = ((QwtBarCurve *)d_master_curve)->dataOffset();
	else if (d_master_curve->type() == Graph::HorizontalBars)
		d_yOffset = ((QwtBarCurve *)d_master_curve)->dataOffset();

	for (int i = from; i <= to; i++){
		const int xi = xMap.transform(x(i) + d_xOffset);
		const int yi = yMap.transform(y(i) + d_yOffset);

		if (type == Vertical){
			int y_plus = yMap.transform(y(i)+err[i]);
			int y_minus = yMap.transform(y(i)-err[i]);
			bool y_minus_is_finite = true;

			if (yMap.transformation()->type() == QwtScaleTransformation::Log10 && err[i] >= y(i)) {
				y_minus = yMap.transform(qMin(yMap.s1(), yMap.s2()));
				y_minus_is_finite = false;
			}

			// draw caps
			if (plus)
				QwtPainter::drawLine(painter, xi-cap/2, y_plus, xi+cap/2, y_plus);
			if (minus && y_minus_is_finite)
				QwtPainter::drawLine(painter, xi-cap/2, y_minus, xi+cap/2, y_minus);

			// draw vertical line
			if (through) {
				if (plus && minus)
					QwtPainter::drawLine(painter, xi, y_minus, xi, y_plus);
				else if (plus)
					QwtPainter::drawLine(painter, xi, yi, xi, y_plus);
				else if (minus)
					QwtPainter::drawLine(painter, xi, y_minus, xi, yi);
			} else if (y_plus <= y_minus) {
				if (plus && y_plus < yi-sh/2)
					QwtPainter::drawLine(painter, xi, yi-sh/2, xi, y_plus);
				if (minus && y_minus > yi+sh/2)
					QwtPainter::drawLine(painter, xi, yi+sh/2, xi, y_minus);
			} else { // inverted scale
				if (plus && y_plus > yi+sh/2)
					QwtPainter::drawLine(painter, xi, yi+sh/2, xi, y_plus);
				if (minus && y_minus < yi-sh/2)
					QwtPainter::drawLine(painter, xi, yi-sh/2, xi, y_minus);
			}
		} else if (type == Horizontal) {
			int x_plus = xMap.transform(x(i)+err[i]);
			int x_minus = xMap.transform(x(i)-err[i]);
			bool x_minus_is_finite = true;

			if (xMap.transformation()->type() == QwtScaleTransformation::Log10 && err[i] >= x(i)) {
				x_minus = xMap.transform(qMin(xMap.s1(), xMap.s2()));
				x_minus_is_finite = false;
			}

			// draw caps
			if (plus)
				QwtPainter::drawLine(painter, x_plus, yi-cap/2, x_plus, yi+cap/2);
			if (minus && x_minus_is_finite)
				QwtPainter::drawLine(painter, x_minus, yi-cap/2, x_minus, yi+cap/2);

			// draw vertical line
			if (through) {
				if (plus && minus)
					QwtPainter::drawLine(painter, x_minus, yi, x_plus, yi);
				else if (plus)
					QwtPainter::drawLine(painter, xi, yi, x_plus, yi);
				else if (minus)
					QwtPainter::drawLine(painter, x_minus, yi, xi, yi);
			} else if (x_plus >= x_minus) {
				if (plus && x_plus > xi+sh/2)
					QwtPainter::drawLine(painter, xi+sh/2, yi, x_plus, yi);
				if (minus && x_minus < xi-sh/2)
					QwtPainter::drawLine(painter, xi-sh/2, yi, x_minus, yi);
			} else { // inverted scale
				if (plus && x_plus < xi-sh/2)
					QwtPainter::drawLine(painter, xi-sh/2, yi, x_plus, yi);
				if (minus && x_minus > xi+sh/2)
					QwtPainter::drawLine(painter, xi+sh/2, yi, x_minus, yi);
			}
		}
	}
}

double QwtErrorPlotCurve::errorValue(int i)
{
	if (i >= 0 && i < dataSize())
		return err[i];
	else
		return 0.0;
}

bool QwtErrorPlotCurve::xErrors()
{
	bool x = false;
	if (type == Horizontal)
		x = true;

	return x;
}

void QwtErrorPlotCurve::setXErrors(bool yes)
{
	if (yes)
		type = Horizontal;
	else
		type = Vertical;
}

void QwtErrorPlotCurve::setWidth(int w)
{
	QPen p = pen();
 	p.setWidth (w);
  	setPen(p);
}

void QwtErrorPlotCurve::setColor(const QColor& c)
{
	QPen p = pen();
  	p.setColor(c);
  	setPen(p);
}

QwtDoubleRect QwtErrorPlotCurve::boundingRect() const
{
	QwtDoubleRect rect = QwtPlotCurve::boundingRect();

	int size = dataSize();

	QwtArray <double> X(size), Y(size), min(size), max(size);
	for (int i=0; i<size; i++){
		X[i]=x(i);
		Y[i]=y(i);
		if (type == Vertical){
			min[i] = y(i) - err[i];
			max[i] = y(i) + err[i];
		} else {
			min[i] = x(i) - err[i];
			max[i] = x(i) + err[i];
		}
	}

	QwtArrayData *erMin, *erMax;
	if (type == Vertical) {
		erMin=new QwtArrayData(X, min);
		erMax=new QwtArrayData(X, max);
	} else {
		erMin=new QwtArrayData(min, Y);
		erMax=new QwtArrayData(max, Y);
	}

	QwtDoubleRect minrect = erMin->boundingRect();
	QwtDoubleRect maxrect = erMax->boundingRect();

	rect.setTop(QMIN(minrect.top(), maxrect.top()));
	rect.setBottom(QMAX(minrect.bottom(), maxrect.bottom()));
	rect.setLeft(QMIN(minrect.left(), maxrect.left()));
	rect.setRight(QMAX(minrect.right(), maxrect.right()));

	delete erMin;
	delete erMax;

	return rect;
}

void QwtErrorPlotCurve::setMasterCurve(DataCurve *c)
{
	if (!c || d_master_curve == c)
		return;

	d_master_curve = c;
	setAxis(c->xAxis(), c->yAxis());
	d_start_row = c->startRow();
	d_end_row = c->endRow();
	c->addErrorBars(this);

	loadData();
}

bool QwtErrorPlotCurve::loadData()
{
	if (!d_master_curve) return false;
	Table *mt = d_master_curve->table();
	if (!mt) return false;
	Column *x = mt->column(d_master_curve->xColumnName());
	Column *y = mt->column(d_master_curve->title().text());

	Column *err = d_table->column(title().text());

	if (!x || !y || !err) {
		remove();
		return false;
	}

	QList< QVector<double> > data = convertData(
			d_master_curve->type() == Graph::HorizontalBars ? (QList<Column*>() << y << x << err) : (QList<Column*>() << x << y << err),
			QList<int>() << xAxis() << yAxis() << (type==Horizontal ? xAxis() : yAxis()) );

	if (data.isEmpty() || data[0].size() == 0) {
		remove();
		return false;
	}

	setData(data[0].data(), data[1].data(), data[0].size());
	setErrors(data[2]);

	return true;
}

QString QwtErrorPlotCurve::plotAssociation()
{
    if (!d_master_curve)
		return QString();

    QString base = d_master_curve->xColumnName() + "(X)," + d_master_curve->title().text() + "(Y)," + title().text();
    if (type == Horizontal)
        return base + "(xErr)";
    else
        return base + "(yErr)";
}

bool QwtErrorPlotCurve::updateData(Table *t, const QString& colName)
{
	if (d_table != t || colName != title().text())
		return false;

	loadData();
	return true;
}
