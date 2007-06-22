/***************************************************************************
    File                 : QwtPieCurve.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : Pie plot class

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
#include "QwtPieCurve.h"
#include "ColorBox.h"
#include "Table.h"

#include <QPaintDevice>
#include <QPainter>
#include <QVarLengthArray>
#include <QLocale>

QwtPieCurve::QwtPieCurve(Table *t, const char *name, int startRow, int endRow):
	DataCurve(t, QString(), name, startRow, endRow)
{
	d_pie_ray = 100;
	d_first_color = 0;
	setPen(QPen(QColor(Qt::black), 1, Qt::SolidLine));
	setBrush(QBrush(Qt::black, Qt::SolidPattern));

	setType(Graph::Pie);
}

void QwtPieCurve::draw(QPainter *painter,
		const QwtScaleMap &xMap, const QwtScaleMap &yMap, int from, int to) const
{
	if ( !painter || dataSize() <= 0 )
		return;

	if (to < 0)
		to = dataSize() - 1;

	drawPie(painter, xMap, yMap, from, to);
}

void QwtPieCurve::drawPie(QPainter *painter,
		const QwtScaleMap &xMap, const QwtScaleMap &yMap, int from, int to) const
{
	int x_center = (xMap.p1() + xMap.p2())/2;
	int y_center = (yMap.p1() + yMap.p2())/2;

	int ray = (x_center - xMap.transform(d_left_coord));
    int d = 2*ray;

	QRect pieRect;
	pieRect.setX(x_center - ray);
	pieRect.setY(y_center - ray);
	pieRect.setWidth(d);
	pieRect.setHeight(d);

	double sum = 0.0;
	for (int i = from; i <= to; i++){
		const double yi = y(i);
		sum += yi;
	}

	int angle = (int)(5760 * 0.75);
	painter->save();
	for (int i = from; i <= to; i++){
		const double yi = y(i);
		const int value = (int)(yi/sum*5760);

		painter->setPen(QwtPlotCurve::pen());
		painter->setBrush(QBrush(color(i), QwtPlotCurve::brush().style()));
		painter->drawPie(pieRect, -angle, -value);

		angle += value;
	}
	painter->restore();
}

QColor QwtPieCurve::color(int i) const
{
	int index=(d_first_color+i) % ColorBox::numPredefinedColors();
	return ColorBox::color(index);
}

void QwtPieCurve::setBrushStyle(const Qt::BrushStyle& style)
{
	QBrush br = QwtPlotCurve::brush();
	if (br.style() == style)
		return;

	br.setStyle(style);
	setBrush(br);
}

void QwtPieCurve::loadData()
{
	QVarLengthArray<double> X(abs(d_end_row - d_start_row) + 1);
	int size = 0;
	int ycol = d_table->colIndex(title().text());
	for (int i = d_start_row; i <= d_end_row; i++ ){
		QString xval = d_table->text(i, ycol);
		bool valid_data = true;
		if (!xval.isEmpty()){
            X[size] = QLocale().toDouble(xval, &valid_data);
            if (valid_data)
                size++;
		}
	}
	X.resize(size);
	setData(X.data(), X.data(), size);
}

void QwtPieCurve::updateBoundingRect()
{
    if (!plot())
        return;

    QwtScaleMap xMap = plot()->canvasMap(xAxis());
    int x_center = (xMap.p1() + xMap.p2())/2;
    int x_left = x_center - d_pie_ray;
    d_left_coord = xMap.invTransform(x_left);
}
