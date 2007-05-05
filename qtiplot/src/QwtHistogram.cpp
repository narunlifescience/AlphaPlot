/***************************************************************************
    File                 : QwtHistogram.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : Histogram class

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
#include "QwtHistogram.h"
#include <QPainter>

#include <gsl/gsl_vector.h>
#include <gsl/gsl_histogram.h>

QwtHistogram::QwtHistogram(Table *t, const QString& xColName, const char *name, int startRow, int endRow):
	QwtBarCurve(QwtBarCurve::Vertical, t, xColName, name, startRow, endRow)
{}

void QwtHistogram::copy(const QwtHistogram *h)
{
	QwtBarCurve::copy((const QwtBarCurve *)h);

	d_autoBin = h->d_autoBin;
	d_bin_size = h->d_bin_size;
	d_begin = h->d_begin;
	d_end = h->d_end;
}

void QwtHistogram::draw(QPainter *painter,
		const QwtScaleMap &xMap, const QwtScaleMap &yMap, int from, int to) const
{
	if ( !painter || dataSize() <= 0 )
		return;

	if (to < 0)
		to = dataSize() - 1;


	painter->save();
	painter->setPen(QwtPlotCurve::pen());
	painter->setBrush(QwtPlotCurve::brush());

	const int ref= yMap.transform(baseline());
	const int dx=abs(xMap.transform(x(from+1)) - xMap.transform(x(from)));
	const int bar_width=int(dx*(1-gap()*0.01));
	const int half_width = int(0.5*(dx-bar_width));
	const int xOffset = int(0.01*offset()*bar_width);

	for (int i=from; i<=to; i++)
	{
		const int px1 = xMap.transform(x(i));
		const int py1 = yMap.transform(y(i));
		painter->drawRect(px1+half_width+xOffset,py1,bar_width+1,(ref-py1+1));
	}

	painter->restore();
}

QwtDoubleRect QwtHistogram::boundingRect() const
{
	QwtDoubleRect rect = QwtPlotCurve::boundingRect();
	rect.setLeft(rect.left()-x(1));
	rect.setRight(rect.right()+x(dataSize()-1));
	rect.setTop(0);
	rect.setBottom(1.2*rect.bottom());
	return rect;
}

void QwtHistogram::setBinning(bool autoBin, double size, double begin, double end)
{
	d_autoBin = autoBin;
	d_bin_size = size;
	d_begin = begin;
	d_end = end;
}

void QwtHistogram::loadData()
{
    int r = abs(d_end_row - d_start_row) + 1;
	QVarLengthArray<double> Y(r);

    int ycol = d_table->colIndex(title().text());
	int size = 0;
	for (int i = 0; i<r; i++ )
	{
		QString yval = d_table->text(i, ycol);
		if (!yval.isEmpty())
            Y[size++] = Table::stringToDouble(yval);
	}
	if(size < 2 || (size==2 && Y[0] == Y[1]))
	{//non valid histogram
		double X[2];
		Y.resize(2);
		for (int i = 0; i<2; i++ )
		{
			Y[i] = 0;
			X[i] = 0;
		}
		setData(X, Y.data(), 2);
		return;
	}

	int n;
	gsl_histogram *h;
	if (d_autoBin)
	{
		n = 10;
		h = gsl_histogram_alloc (n);
		if (!h)
			return;

		gsl_vector *v = gsl_vector_alloc (size);
		for (int i = 0; i<size; i++ )
			gsl_vector_set (v, i, Y[i]);

		double min, max;
		gsl_vector_minmax (v, &min, &max);
		gsl_vector_free (v);

		d_begin = floor(min);
		d_end = ceil(max);
		d_bin_size = (d_end - d_begin)/(double)n;

		gsl_histogram_set_ranges_uniform (h, floor(min), ceil(max));
	}
	else
	{
		n = int((d_end - d_begin)/d_bin_size + 1);
		h = gsl_histogram_alloc (n);
		if (!h)
			return;

		double *range = new double[n+2];
		for (int i = 0; i<= n+1; i++ )
			range[i] = d_begin + i*d_bin_size;

		gsl_histogram_set_ranges (h, range, n+1);
		delete[] range;
	}

	for (int i = 0; i<size; i++ )
		gsl_histogram_increment (h, Y[i]);

	double X[n]; //stores ranges (x) and bins (y)
	Y.resize(n);
	for (int i = 0; i<n; i++ )
	{
		Y[i] = gsl_histogram_get (h, i);
		double lower, upper;
		gsl_histogram_get_range (h, i, &lower, &upper);
		X[i] = lower;
	}
	setData(X, Y.data(), n);

	d_mean = gsl_histogram_mean(h);
	d_standard_deviation = gsl_histogram_sigma(h);
	d_min = gsl_histogram_min_val(h);
	d_max = gsl_histogram_max_val(h);

	gsl_histogram_free (h);
}

void QwtHistogram::initData(const QVector<double>& Y, int size)
{
	if(size<2 || (size == 2 && Y[0] == Y[1]))
	{//non valid histogram data
		double x[2], y[2];
		for (int i = 0; i<2; i++ )
		{
			y[i] = 0;
			x[i] = 0;
		}
		setData(x, y, 2);
		return;
	}

	int n = 10;//default value
	double x[n], y[n]; //store ranges (x) and bins (y)
	gsl_histogram * h = gsl_histogram_alloc (n);
	if (!h)
		return;

	gsl_vector *v;
	v = gsl_vector_alloc (size);
	for (int i = 0; i<size; i++ )
		gsl_vector_set (v, i, Y[i]);

	double min, max;
	gsl_vector_minmax (v, &min, &max);
	gsl_vector_free (v);

	d_begin = floor(min);
	d_end = ceil(max);

	gsl_histogram_set_ranges_uniform (h, floor(min), ceil(max));

	for (int i = 0; i<size; i++ )
		gsl_histogram_increment (h, Y[i]);

	for (int i = 0; i<n; i++ )
	{
		y[i] = gsl_histogram_get (h, i);
		double lower, upper;
		gsl_histogram_get_range (h, i, &lower, &upper);
		x[i] = lower;
	}

	setData(x, y, n);

	d_bin_size = (d_end - d_begin)/(double)n;
	d_autoBin = true;
    d_mean = gsl_histogram_mean(h);
	d_standard_deviation = gsl_histogram_sigma(h);
	d_min = gsl_histogram_min_val(h);
	d_max = gsl_histogram_max_val(h);

	gsl_histogram_free (h);
}
