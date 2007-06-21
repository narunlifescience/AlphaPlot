/***************************************************************************
    File                 : BoxCurve.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : Box curve

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
#include "BoxCurve.h"
#include <QPainter>
#include <QLocale>

#include <gsl/gsl_sort.h>
#include <gsl/gsl_statistics.h>

BoxCurve::BoxCurve(Table *t, const char *name, int startRow, int endRow):
	DataCurve(t, QString(), name, startRow, endRow)
{
	mean_style = QwtSymbol::Rect;
	max_style = QwtSymbol::XCross;
	min_style = QwtSymbol::XCross;
	p99_style = QwtSymbol::NoSymbol;
	p1_style = QwtSymbol::NoSymbol;

	b_style = Rect;
	b_coeff = 75.0;
	b_range = r25_75;
	w_range = r5_95;
	w_coeff = 95.0;
	b_width = 80;

	setType(Graph::Box);
}

void BoxCurve::copy(const BoxCurve *b)
{
	mean_style = b->mean_style;
	max_style = b->max_style;
	min_style = b->min_style;
	p99_style = b->p99_style;
	p1_style = b->p1_style;

	b_style = b->b_style;
	b_coeff = b->b_coeff;
	b_range = b->b_range;
	w_range = b->w_range;
	w_coeff = b->w_coeff;
	b_width = b->b_width;
}

void BoxCurve::draw(QPainter *painter,
		const QwtScaleMap &xMap, const QwtScaleMap &yMap, int from, int to) const
{
	if ( !painter || dataSize() <= 0 )
		return;

	int size = dataSize();
	if (to < 0)
		to = size - 1;

	painter->save();
	painter->setPen(QwtPlotCurve::pen());

	double *dat = new double[size];
	for (int i = from; i<= to; i++)
		dat[i] = y(i);

	drawBox(painter, xMap, yMap, dat, size);
	drawSymbols(painter, xMap, yMap, dat, size);

	painter->restore();
	delete[] dat;
}

void BoxCurve::drawBox(QPainter *painter, const QwtScaleMap &xMap,
		const QwtScaleMap &yMap, double *dat, int size) const
{
	const int px = xMap.transform(x(0));
	const int px_min = xMap.transform(x(0) - 0.5);
	const int px_max = xMap.transform(x(0) + 0.5);
	const int box_width = 1+(px_max - px_min)*b_width/100;
	const int hbw = box_width/2;
	const int median = yMap.transform(gsl_stats_median_from_sorted_data (dat, 1, size));
	int b_lowerq, b_upperq;
	double sd, se, mean;
	if(w_range == SD || w_range == SE || b_range == SD || b_range == SE)
	{
		sd = gsl_stats_sd(dat, 1, size);
		se = sd/sqrt((double)size);
		mean = gsl_stats_mean(dat, 1, size);
	}

	if(b_range == SD)
	{
		b_lowerq = yMap.transform(mean - sd*b_coeff);
		b_upperq = yMap.transform(mean + sd*b_coeff);
	}
	else if(b_range == SE)
	{
		b_lowerq = yMap.transform(mean - se*b_coeff);
		b_upperq = yMap.transform(mean + se*b_coeff);
	}
	else
	{
		b_lowerq = yMap.transform(gsl_stats_quantile_from_sorted_data (dat, 1, size, 1-0.01*b_coeff));
		b_upperq = yMap.transform(gsl_stats_quantile_from_sorted_data (dat, 1, size, 0.01*b_coeff));
	}

	//draw box
	if (b_style == Rect)
	{
		const QRect r = QRect(px - hbw, b_upperq, box_width, b_lowerq - b_upperq + 1);
		painter->fillRect(r, QwtPlotCurve::brush());
		painter->drawRect(r);
	}
	else if (b_style == Diamond)
	{
		QPolygon pa(4);
		pa[0] = QPoint(px, b_upperq);
		pa[1] = QPoint(px + hbw, median);
		pa[2] = QPoint(px, b_lowerq);
		pa[3] = QPoint(px - hbw, median);

		painter->setBrush(QwtPlotCurve::brush());
		painter->drawPolygon(pa);
	}
	else if (b_style == WindBox)
	{
		const int lowerq = yMap.transform(gsl_stats_quantile_from_sorted_data (dat, 1, size, 0.25));
		const int upperq = yMap.transform(gsl_stats_quantile_from_sorted_data (dat, 1, size, 0.75));
		QPolygon pa(8);
		pa[0] = QPoint(px + hbw, b_upperq);
		pa[1] = QPoint(int(px + 0.4*box_width), upperq);
		pa[2] = QPoint(int(px + 0.4*box_width), lowerq);
		pa[3] = QPoint(px + hbw, b_lowerq);
		pa[4] = QPoint(px - hbw, b_lowerq);
		pa[5] = QPoint(int(px - 0.4*box_width), lowerq);
		pa[6] = QPoint(int(px - 0.4*box_width), upperq);
		pa[7] = QPoint(px - hbw, b_upperq);

		painter->setBrush(QwtPlotCurve::brush());
		painter->drawPolygon(pa);
	}
	else if (b_style == Notch)
	{
		int j = (int)ceil(0.5*(size - 1.96*sqrt((double)size)));
		int k = (int)ceil(0.5*(size + 1.96*sqrt((double)size)));
		const int lowerCI = yMap.transform(dat[j]);
		const int upperCI = yMap.transform(dat[k]);

		QPolygon pa(10);
		pa[0] = QPoint(px + hbw, b_upperq);
		pa[1] = QPoint(px + hbw, upperCI);
		pa[2] = QPoint(int(px + 0.25*hbw), median);
		pa[3] = QPoint(px + hbw, lowerCI);
		pa[4] = QPoint(px + hbw, b_lowerq);
		pa[5] = QPoint(px - hbw, b_lowerq);
		pa[6] = QPoint(px - hbw, lowerCI);
		pa[7] = QPoint(int(px - 0.25*hbw), median);
		pa[8] = QPoint(px - hbw, upperCI);
		pa[9] = QPoint(px - hbw, b_upperq);

		painter->setBrush(QwtPlotCurve::brush());
		painter->drawPolygon(pa);
	}

	if (w_range)
	{//draw whiskers
		const int l = int(0.1*box_width);
		int w_upperq, w_lowerq;
		if(w_range == SD)
		{
			w_lowerq = yMap.transform(mean - sd*w_coeff);
			w_upperq = yMap.transform(mean + sd*w_coeff);
		}
		else if(w_range == SE)
		{
			w_lowerq = yMap.transform(mean - se*w_coeff);
			w_upperq = yMap.transform(mean + se*w_coeff);
		}
		else
		{
			w_lowerq = yMap.transform(gsl_stats_quantile_from_sorted_data (dat, 1, size, 1-0.01*w_coeff));
			w_upperq = yMap.transform(gsl_stats_quantile_from_sorted_data (dat, 1, size, 0.01*w_coeff));
		}

		painter->drawLine(px - l, w_lowerq, px + l, w_lowerq);
		painter->drawLine(px - l, w_upperq, px + l, w_upperq);

		if (b_style)
		{
			if (w_upperq != b_upperq)
				painter->drawLine(px, w_upperq, px, b_upperq);
			if (w_lowerq != b_lowerq)
				painter->drawLine(px, w_lowerq, px, b_lowerq);
		}
		else
			painter->drawLine(px, w_upperq, px, w_lowerq);
	}

	//draw median line
	if (b_style == Notch || b_style == NoBox)
		return;
	if (b_style == WindBox)
		painter->drawLine(int(px - 0.4*box_width), median, int(px + 0.4*box_width), median);
	else
		painter->drawLine(px - hbw, median, px + hbw, median);
}

void BoxCurve::drawSymbols(QPainter *painter, const QwtScaleMap &xMap,
		const QwtScaleMap &yMap, double *dat, int size) const
{
	const int px = xMap.transform(x(0));

	QwtSymbol s = this->symbol();
	if (min_style != QwtSymbol::NoSymbol)
	{
		const int py_min = yMap.transform(y(0));
		s.setStyle(min_style);
		s.draw(painter, px, py_min);
	}
	if (max_style != QwtSymbol::NoSymbol)
	{
		const int py_max = yMap.transform(y(size - 1));
		s.setStyle(max_style);
		s.draw(painter, px, py_max);
	}
	if (p1_style != QwtSymbol::NoSymbol)
	{
		const int p1 = yMap.transform(gsl_stats_quantile_from_sorted_data (dat, 1, size, 0.01));
		s.setStyle(p1_style);
		s.draw(painter, px, p1);
	}
	if (p99_style != QwtSymbol::NoSymbol)
	{
		const int p99 = yMap.transform(gsl_stats_quantile_from_sorted_data (dat, 1, size, 0.99));
		s.setStyle(p99_style);
		s.draw(painter, px, p99);
	}
	if (mean_style != QwtSymbol::NoSymbol)
	{
		const int mean = yMap.transform(gsl_stats_mean(dat, 1, size));
		s.setStyle(mean_style);
		s.draw(painter, px, mean);
	}
}

void BoxCurve::setBoxStyle(int style)
{
	if (b_style == style)
		return;
	b_style=style;
}

void BoxCurve::setBoxRange(int type, double coeff)
{
	if (b_style == WindBox)
	{
		b_range = r10_90;
		b_coeff = 90.0;
		return;
	}

	b_range = type;

	if (b_range == r25_75)
		b_coeff = 75.0;
	else if (b_range == r10_90)
		b_coeff = 90.0;
	else if (b_range == r5_95)
		b_coeff = 95.0;
	else if (b_range == r1_99)
		b_coeff = 99.0;
	else if (b_range == MinMax)
		b_coeff = 100.0;
	else
		b_coeff = coeff;
}

void BoxCurve::setWhiskersRange(int type, double coeff)
{
	w_range = type;

	if (w_range == r25_75)
		w_coeff = 75.0;
	else if (w_range == r10_90)
		w_coeff = 90.0;
	else if (w_range == r5_95)
		w_coeff = 95.0;
	else if (w_range == r1_99)
		w_coeff = 99.0;
	else if (w_range == MinMax)
		w_coeff = 100.0;
	else
		w_coeff = coeff;
}

QwtDoubleRect BoxCurve::boundingRect() const
{
	QwtDoubleRect rect = QwtPlotCurve::boundingRect();

	double dy=0.2*(rect.bottom()-rect.top());
	rect.setTop(rect.top()-dy);
	rect.setBottom(rect.bottom()+dy);

	rect.setLeft(rect.left()-0.5);
	rect.setRight(rect.right()+0.5);
	return rect;
}

void BoxCurve::loadData()
{
	QVector<double> Y(abs(d_end_row - d_start_row) + 1);
    int ycol = d_table->colIndex(title().text());
	int size = 0;
	for (int i = d_start_row; i <= d_end_row; i++){
		QString s = d_table->text(i, ycol);
        if (!s.isEmpty()){
            bool valid_data = true;
            Y[size] = QLocale().toDouble(s, &valid_data);
            if (valid_data)
                size++;
        }
	}

	if (size>0){
		Y.resize(size);
		gsl_sort (Y.data(), 1, size);
        setData(QwtSingleArrayData(this->x(0), Y, size));
	} else
		remove();
}
