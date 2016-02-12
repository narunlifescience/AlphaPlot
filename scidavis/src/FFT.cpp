/***************************************************************************
    File                 : FFT.cpp
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Ion Vasilief
    Email (use @ for *)  : ion_vasilief*yahoo.fr
    Description          : Numerical FFT of data sets

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
#include "FFT.h"
#include "MultiLayer.h"
#include "Plot.h"
#include "ColorBox.h"
#include "core/column/Column.h"

#include <QMessageBox>
#include <QLocale>

#include <gsl/gsl_fft_complex.h>
#include <gsl/gsl_fft_halfcomplex.h>

FFT::FFT(ApplicationWindow *parent, Table *t, const QString& realColName, const QString& imagColName)
: Filter(parent, t)
{
	init();
    setDataFromTable(t, realColName, imagColName);
}

FFT::FFT(ApplicationWindow *parent, Graph *g, const QString& curveTitle)
: Filter(parent, g)
{
	init();
    setDataFromCurve(curveTitle);
}

void FFT::init ()
{
    setObjectName(tr("FFT"));
    d_inverse = false;
    d_normalize = true;
    d_shift_order = true;
    d_real_col = -1;
    d_imag_col = -1;
	d_sampling = 1.0;
}

QList<Column *> FFT::fftCurve()
{
    int i, i2;
	int n2 = d_n/2;
	double *amp = new double[d_n];
	double *result = new double[2*d_n];

	if(!amp || !result)
	{
		QMessageBox::critical((ApplicationWindow *)parent(), tr("SciDAVis") + " - " + tr("Error"),
                        tr("Could not allocate memory, operation aborted!"));
        d_init_err = true;
        return QList<Column *>();
	}

	double df = 1.0/(double)(d_n*d_sampling);//frequency sampling
	double aMax = 0.0;//max amplitude
	QList<Column *> columns;
	if(!d_inverse)
	{
        d_explanation = tr("Forward") + " " + tr("FFT") + " " + tr("of") + " " + d_curve->title().text();
		columns << new Column(tr("Frequency"), SciDAVis::Numeric);

		gsl_fft_real_workspace *work=gsl_fft_real_workspace_alloc(d_n);
		gsl_fft_real_wavetable *real=gsl_fft_real_wavetable_alloc(d_n);

		if(!work || !real)
		{
			QMessageBox::critical((ApplicationWindow *)parent(), tr("SciDAVis") + " - " + tr("Error"),
                        tr("Could not allocate memory, operation aborted!"));
            d_init_err = true;
			return QList<Column *>();
		}

		gsl_fft_real_transform(d_y, 1, d_n, real,work);
		gsl_fft_halfcomplex_unpack (d_y, result, 1, d_n);

		gsl_fft_real_wavetable_free(real);
		gsl_fft_real_workspace_free(work);
	}
	else
	{
        d_explanation = tr("Inverse") + " " + tr("FFT") + " " + tr("of") + " " + d_curve->title().text();
		columns << new Column(tr("Time"), SciDAVis::Numeric);

		gsl_fft_real_unpack (d_y, result, 1, d_n);
		gsl_fft_complex_wavetable *wavetable = gsl_fft_complex_wavetable_alloc (d_n);
		gsl_fft_complex_workspace *workspace = gsl_fft_complex_workspace_alloc (d_n);

		if(!workspace || !wavetable)
		{
			QMessageBox::critical((ApplicationWindow *)parent(), tr("SciDAVis") + " - " + tr("Error"),
                        tr("Could not allocate memory, operation aborted!"));
            d_init_err = true;
			return QList<Column *>();
		}

		gsl_fft_complex_inverse (result, 1, d_n, wavetable, workspace);
		gsl_fft_complex_wavetable_free (wavetable);
		gsl_fft_complex_workspace_free (workspace);
	}

	if (d_shift_order)
	{
		for(i=0; i<d_n; i++)
		{
			d_x[i] = (i-n2)*df;
			int j = i + d_n;
			double aux = result[i];
			result[i] = result[j];
			result[j] = aux;
		}
	}
	else
	{
		for(i=0; i<d_n; i++)
			d_x[i] = i*df;
	}

	for(i=0;i<d_n;i++)
	{
		i2 = 2*i;
		double real_part = result[i2];
		double im_part = result[i2+1];
		double a = sqrt(real_part*real_part + im_part*im_part);
		amp[i]= a;
		if (a > aMax)
			aMax = a;
	}

        //	ApplicationWindow *app = (ApplicationWindow *)parent();

	columns << new Column(tr("Real"), SciDAVis::Numeric);
	columns << new Column(tr("Imaginary"), SciDAVis::Numeric);
	columns << new Column(tr("Amplitude"), SciDAVis::Numeric);
	columns << new Column(tr("Angle"), SciDAVis::Numeric);
	for (i=0;i<d_n;i++)
	{
		i2 = 2*i;
		columns.at(0)->setValueAt(i, d_x[i]);
		columns.at(1)->setValueAt(i, result[i2]);
		columns.at(2)->setValueAt(i, result[i2+1]);
		if (d_normalize)
			columns.at(3)->setValueAt(i, amp[i]/aMax);
		else
			columns.at(3)->setValueAt(i, amp[i]);
		columns.at(4)->setValueAt(i, atan(result[i2+1]/result[i2]));
	}
	delete[] amp;
	delete[] result;
	columns.at(0)->setPlotDesignation(SciDAVis::X);
	columns.at(1)->setPlotDesignation(SciDAVis::Y);
	columns.at(2)->setPlotDesignation(SciDAVis::Y);
	columns.at(3)->setPlotDesignation(SciDAVis::Y);
	columns.at(4)->setPlotDesignation(SciDAVis::Y);
    return columns;
}

QList<Column *> FFT::fftTable()
{
    int i;
	int rows = d_table->numRows();
	double *amp = new double[rows];

	gsl_fft_complex_wavetable *wavetable = gsl_fft_complex_wavetable_alloc (rows);
	gsl_fft_complex_workspace *workspace = gsl_fft_complex_workspace_alloc (rows);

	if(!amp || !wavetable || !workspace)
	{
		QMessageBox::critical((ApplicationWindow *)parent(), tr("SciDAVis") + " - " + tr("Error"),
                        tr("Could not allocate memory, operation aborted!"));
        d_init_err = true;
        return QList<Column *>();
	}

	double df = 1.0/(double)(rows*d_sampling);//frequency sampling
	double aMax = 0.0;//max amplitude
	QList<Column *> columns;
	if(!d_inverse)
	{
		columns << new Column(tr("Frequency"), SciDAVis::Numeric);
		gsl_fft_complex_forward (d_y, 1, rows, wavetable, workspace);
	}
	else
	{
		columns << new Column(tr("Time"), SciDAVis::Numeric);
		gsl_fft_complex_inverse (d_y, 1, rows, wavetable, workspace);
	}

	gsl_fft_complex_wavetable_free (wavetable);
	gsl_fft_complex_workspace_free (workspace);

	if (d_shift_order)
	{
		int n2 = rows/2;
		for(i=0; i<rows; i++)
		{
			d_x[i] = (i-n2)*df;
			int j = i + rows;
			double aux = d_y[i];
			d_y[i] = d_y[j];
			d_y[j] = aux;
		}
	}
	else
	{
		for(i=0; i<rows; i++)
			d_x[i] = i*df;
	}

	for(i=0; i<rows; i++)
	{
		int i2 = 2*i;
		double a = sqrt(d_y[i2]*d_y[i2] + d_y[i2+1]*d_y[i2+1]);
		amp[i]= a;
		if (a > aMax)
			aMax = a;
	}

	columns << new Column(tr("Real"), SciDAVis::Numeric);
	columns << new Column(tr("Imaginary"), SciDAVis::Numeric);
	columns << new Column(tr("Amplitude"), SciDAVis::Numeric);
	columns << new Column(tr("Angle"), SciDAVis::Numeric);
	for (i=0; i<rows; i++)
	{
		int i2 = 2*i;
		columns.at(0)->setValueAt(i, d_x[i]);
		columns.at(1)->setValueAt(i, d_y[i2]);
		columns.at(2)->setValueAt(i, d_y[i2+1]);
		if (d_normalize)
			columns.at(3)->setValueAt(i, amp[i]/aMax);
		else
			columns.at(3)->setValueAt(i, amp[i]);
		columns.at(4)->setValueAt(i, atan(d_y[i2+1]/d_y[i2]));
	}
	delete[] amp;
	columns.at(0)->setPlotDesignation(SciDAVis::X);
	columns.at(1)->setPlotDesignation(SciDAVis::Y);
	columns.at(2)->setPlotDesignation(SciDAVis::Y);
	columns.at(3)->setPlotDesignation(SciDAVis::Y);
	columns.at(4)->setPlotDesignation(SciDAVis::Y);
    return columns;
}

void FFT::output()
{
    QList<Column *> columns;
    if (d_graph && d_curve)
        columns = fftCurve();
    else if (d_table)
        columns = fftTable();

    if (!columns.isEmpty())
        output(columns);
}

void FFT::output(QList<Column *> columns)
{
    ApplicationWindow *app = (ApplicationWindow *)parent();
    QString tableName = app->generateUniqueName(objectName());
    Table *t = app->newHiddenTable(tableName, d_explanation, columns);
	MultiLayer *ml = app->multilayerPlot(t, QStringList() << tableName + "_" + tr("Amplitude"), 0);
   	if (!ml)
		return;

	Graph* g = ml->activeGraph();
	if ( g )
	{
		g->setCurvePen(0, QPen(ColorBox::color(d_curveColorIndex), 1));

        Plot* plot = g->plotWidget();
		plot->setTitle(QString());
		if (!d_inverse)
			plot->setAxisTitle(QwtPlot::xBottom, tr("Frequency") + " (" + tr("Hz") + ")");
		else
			plot->setAxisTitle(QwtPlot::xBottom, tr("Time") + + " (" + tr("s") + ")");

		plot->setAxisTitle(QwtPlot::yLeft, tr("Amplitude"));
		plot->replot();
	}
	ml->showMaximized();
}

void FFT::setDataFromTable(Table *t, const QString& realColName, const QString& imagColName)
{
    if (t && d_table != t)
        d_table = t;

    d_real_col = d_table->colIndex(realColName);

    if (!imagColName.isEmpty())
        d_imag_col = d_table->colIndex(imagColName);

    if (d_n > 0)
	{//delete previousely allocated memory
		delete[] d_x;
		delete[] d_y;
	}

    d_n = d_table->numRows();
    int n2 = 2*d_n;
    d_y = new double[n2];
    d_x = new double[d_n];

    if(d_y && d_x)
	{// zero-pad data array
		memset( d_y, 0, n2* sizeof( double ) );
		for(int i=0; i<d_n; i++)
		{
			int i2 = 2*i;
			d_y[i2] = d_table->cell(i, d_real_col);
			if (d_imag_col >= 0)
				d_y[i2+1] = d_table->cell(i, d_imag_col);
		}
	}
	else
	{
		QMessageBox::critical((ApplicationWindow *)parent(), tr("SciDAVis") + " - " + tr("Error"),
                        tr("Could not allocate memory, operation aborted!"));
        d_init_err = true;
	}
}
