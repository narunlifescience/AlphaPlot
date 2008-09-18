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
#include "graph/Graph.h"
#include "graph/Plot.h"
#include "lib/ColorBox.h"
#include "table/Table.h"

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

FFT::FFT(ApplicationWindow *parent, Layer *layer, const QString& curveTitle)
: Filter(parent, layer)
{
	init();
    setDataFromCurve(curveTitle);
}

void FFT::init ()
{
    setName(tr("FFT"));
    m_inverse = false;
    m_normalize = true;
    m_shift_order = true;
    m_real_col = -1;
    m_imag_col = -1;
	m_sampling = 1.0;
}

QString FFT::fftCurve()
{
    int i, i2;
	int n2 = m_n/2;
	double *amp = new double[m_n];
	double *result = new double[2*m_n];

	if(!amp || !result)
	{
		QMessageBox::critical((ApplicationWindow *)parent(), tr("SciDAVis") + " - " + tr("Error"),
                        tr("Could not allocate memory, operation aborted!"));
        m_init_err = true;
        return "";
	}

	double df = 1.0/(double)(m_n*m_sampling);//frequency sampling
	double aMax = 0.0;//max amplitude
	QString text;
	if(!m_inverse)
	{
        m_explanation = tr("Forward") + " " + tr("FFT") + " " + tr("of") + " " + m_curve->title().text();
		text = tr("Frequency");

		gsl_fft_real_workspace *work=gsl_fft_real_workspace_alloc(m_n);
		gsl_fft_real_wavetable *real=gsl_fft_real_wavetable_alloc(m_n);

		if(!work || !real)
		{
			QMessageBox::critical((ApplicationWindow *)parent(), tr("SciDAVis") + " - " + tr("Error"),
                        tr("Could not allocate memory, operation aborted!"));
            m_init_err = true;
			return "";
		}

		gsl_fft_real_transform(m_y, 1, m_n, real,work);
		gsl_fft_halfcomplex_unpack (m_y, result, 1, m_n);

		gsl_fft_real_wavetable_free(real);
		gsl_fft_real_workspace_free(work);
	}
	else
	{
        m_explanation = tr("Inverse") + " " + tr("FFT") + " " + tr("of") + " " + m_curve->title().text();
		text = tr("Time");

		gsl_fft_real_unpack (m_y, result, 1, m_n);
		gsl_fft_complex_wavetable *wavetable = gsl_fft_complex_wavetable_alloc (m_n);
		gsl_fft_complex_workspace *workspace = gsl_fft_complex_workspace_alloc (m_n);

		if(!workspace || !wavetable)
		{
			QMessageBox::critical((ApplicationWindow *)parent(), tr("SciDAVis") + " - " + tr("Error"),
                        tr("Could not allocate memory, operation aborted!"));
            m_init_err = true;
			return "";
		}

		gsl_fft_complex_inverse (result, 1, m_n, wavetable, workspace);
		gsl_fft_complex_wavetable_free (wavetable);
		gsl_fft_complex_workspace_free (workspace);
	}

	if (m_shift_order)
	{
		for(i=0; i<m_n; i++)
		{
			m_x[i] = (i-n2)*df;
			int j = i + m_n;
			double aux = result[i];
			result[i] = result[j];
			result[j] = aux;
		}
	}
	else
	{
		for(i=0; i<m_n; i++)
			m_x[i] = i*df;
	}

	for(i=0;i<m_n;i++)
	{
		i2 = 2*i;
		double real_part = result[i2];
		double im_part = result[i2+1];
		double a = sqrt(real_part*real_part + im_part*im_part);
		amp[i]= a;
		if (a > aMax)
			aMax = a;
	}

	ApplicationWindow *app = (ApplicationWindow *)parent();
	int prec = app->m_decimal_digits;

	text += "\t"+tr("Real")+"\t"+tr("Imaginary")+"\t"+ tr("Amplitude")+"\t"+tr("Angle")+"\n";
	for (i=0;i<m_n;i++)
	{
		i2 = 2*i;
		text += QLocale().toString(m_x[i], 'g', prec)+"\t";
		text += QLocale().toString(result[i2], 'g', prec)+"\t";
		text += QLocale().toString(result[i2+1], 'g', prec)+"\t";
		if (m_normalize)
			text += QLocale().toString(amp[i]/aMax, 'g', prec)+"\t";
		else
			text += QLocale().toString(amp[i], 'g', prec)+"\t";
		text += QLocale().toString(atan(result[i2+1]/result[i2]), 'g', prec)+"\n";
	}
	delete[] amp;
	delete[] result;
    return text;
}

QString FFT::fftTable()
{
    int i;
	int rows = m_table->rowCount();
	double *amp = new double[rows];

	gsl_fft_complex_wavetable *wavetable = gsl_fft_complex_wavetable_alloc (rows);
	gsl_fft_complex_workspace *workspace = gsl_fft_complex_workspace_alloc (rows);

	if(!amp || !wavetable || !workspace)
	{
		QMessageBox::critical((ApplicationWindow *)parent(), tr("SciDAVis") + " - " + tr("Error"),
                        tr("Could not allocate memory, operation aborted!"));
        m_init_err = true;
        return "";
	}

	double df = 1.0/(double)(rows*m_sampling);//frequency sampling
	double aMax = 0.0;//max amplitude
	QString text;
	if(!m_inverse)
	{
		text = tr("Frequency");
		gsl_fft_complex_forward (m_y, 1, rows, wavetable, workspace);
	}
	else
	{
		text = tr("Time");
		gsl_fft_complex_inverse (m_y, 1, rows, wavetable, workspace);
	}

	gsl_fft_complex_wavetable_free (wavetable);
	gsl_fft_complex_workspace_free (workspace);

	if (m_shift_order)
	{
		int n2 = rows/2;
		for(i=0; i<rows; i++)
		{
			m_x[i] = (i-n2)*df;
			int j = i + rows;
			double aux = m_y[i];
			m_y[i] = m_y[j];
			m_y[j] = aux;
		}
	}
	else
	{
		for(i=0; i<rows; i++)
			m_x[i] = i*df;
	}

	for(i=0; i<rows; i++)
	{
		int i2 = 2*i;
		double a = sqrt(m_y[i2]*m_y[i2] + m_y[i2+1]*m_y[i2+1]);
		amp[i]= a;
		if (a > aMax)
			aMax = a;
	}

    ApplicationWindow *app = (ApplicationWindow *)parent();
	int prec = app->m_decimal_digits;

	text += "\t"+tr("Real")+"\t"+tr("Imaginary")+"\t"+tr("Amplitude")+"\t"+tr("Angle")+"\n";
	for (i=0; i<rows; i++)
	{
		int i2 = 2*i;
		text += QLocale().toString(m_x[i], 'g', prec)+"\t";
		text += QLocale().toString(m_y[i2], 'g', prec)+"\t";
		text += QLocale().toString(m_y[i2+1], 'g', prec)+"\t";
		if (m_normalize)
			text += QLocale().toString(amp[i]/aMax, 'g', prec)+"\t";
		else
			text += QLocale().toString(amp[i], 'g', prec)+"\t";
		text += QLocale().toString(atan(m_y[i2+1]/m_y[i2]), 'g', prec)+"\n";
	}
	delete[] amp;
    return text;
}

void FFT::output()
{
    QString text;
    if (m_layer && m_curve)
        text = fftCurve();
    else if (m_table)
        text = fftTable();

    if (!text.isEmpty())
        output(text);
}

void FFT::output(const QString &text)
{
    ApplicationWindow *app = (ApplicationWindow *)parent();
    QString tableName = app->generateUniqueName(QString(name()));
    Table *t = app->newHiddenTable(tableName, m_explanation, m_n, 5, text);
	Graph *graph = app->multilayerPlot(t, QStringList() << tableName + "_" + tr("Amplitude"), 0);
   	if (!graph)
		return;

	Layer* layer = graph->activeLayer();
	if (layer)
	{
		layer->setCurvePen(0, QPen(ColorBox::color(m_curveColorIndex), 1));

        Plot* plot = layer->plotWidget();
		plot->setTitle(QString());
		if (!m_inverse)
			plot->setAxisTitle(QwtPlot::xBottom, tr("Frequency") + " (" + tr("Hz") + ")");
		else
			plot->setAxisTitle(QwtPlot::xBottom, tr("Time") + + " (" + tr("s") + ")");

		plot->setAxisTitle(QwtPlot::yLeft, tr("Amplitude"));
		plot->replot();
	}
	graph->showMaximized();
}

void FFT::setDataFromTable(Table *t, const QString& realColName, const QString& imagColName)
{
    if (t && m_table != t)
        m_table = t;

    m_real_col = m_table->colIndex(realColName);

    if (!imagColName.isEmpty())
        m_imag_col = m_table->colIndex(imagColName);

    if (m_n > 0)
	{//delete previousely allocated memory
		delete[] m_x;
		delete[] m_y;
	}

    m_n = m_table->rowCount();
    int n2 = 2*m_n;
    m_y = new double[n2];
    m_x = new double[m_n];

    if(m_y && m_x)
	{// zero-pad data array
		memset( m_y, 0, n2* sizeof( double ) );
		for(int i=0; i<m_n; i++)
		{
			int i2 = 2*i;
			m_y[i2] = m_table->cell(i, m_real_col);
			if (m_imag_col >= 0)
				m_y[i2+1] = m_table->cell(i, m_imag_col);
		}
	}
	else
	{
		QMessageBox::critical((ApplicationWindow *)parent(), tr("SciDAVis") + " - " + tr("Error"),
                        tr("Could not allocate memory, operation aborted!"));
        m_init_err = true;
	}
}
