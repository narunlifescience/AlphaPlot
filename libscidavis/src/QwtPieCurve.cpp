/***************************************************************************
    File                 : QwtPieCurve.cpp
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Benkert
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
#include "core/column/Column.h"

#include <QPaintDevice>
#include <QPainter>
#include <QVarLengthArray>
#include <QLocale>

#include <qwt_plot_layout.h>

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
	// This has to be synced with Graph::plotPie() for now... until we have a clean solution.
	QRect canvas_rect = plot()->plotLayout()->canvasRect();
	int radius = 0.4*qMin(canvas_rect.width(), canvas_rect.height());

	QRect pieRect;
	pieRect.setX(canvas_rect.center().x() - radius);
	pieRect.setY(canvas_rect.center().y() - radius);
	pieRect.setWidth(2*radius);
	pieRect.setHeight(2*radius);

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

bool QwtPieCurve::loadData()
{
	QVarLengthArray<double> Y(abs(d_end_row - d_start_row) + 1);
	int size = 0;
	int ycol = d_table->colIndex(title().text());
	Column *y_col_ptr = d_table->column(ycol);
	int yColType = d_table->columnType(ycol);

	for (int row = d_start_row; row <= d_end_row && row < y_col_ptr->rowCount(); row++ ) {
		if (!y_col_ptr->isInvalid(row)) {
			if (yColType == Table::Text) {
				QString yval = y_col_ptr->textAt(row);
				bool valid_data = true;
				Y[size] = QLocale().toDouble(yval, &valid_data);
				if (!valid_data)
					continue;
			}
			else
				Y[size] = y_col_ptr->valueAt(row);

			size++;
		}
	}
	Y.resize(size);
	setData(Y.data(), Y.data(), size);

	return true;
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
