/***************************************************************************
    File                 : Convolution.cpp
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Ion Vasilief
    Email (use @ for *)  : ion_vasilief*yahoo.fr
    Description          : Numerical convolution/deconvolution of data sets

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
#include "Convolution.h"
#include "graph/Graph.h"
#include "graph/Plot.h"
#include "graph/PlotCurve.h"
#include "lib/ColorBox.h"
#include "core/AbstractDataSource.h"
#include "table/Table.h"

#include <QMessageBox>
#include <QLocale>
#include <gsl/gsl_fft_halfcomplex.h>

Convolution::Convolution(ApplicationWindow *parent, Table *t, const QString& signalColName, const QString& responseColName)
: Filter(parent, t)
{
	setName(tr("Convolution"));
    setDataFromTable(t, signalColName, responseColName);
}

void Convolution::setDataFromTable(Table *t, const QString& signalColName, const QString& responseColName)
{
    if (t && m_table != t)
        m_table = t;

    int signal_col = m_table->colIndex(signalColName);
	int response_col = m_table->colIndex(responseColName);

	if (signal_col < 0)
	{
		QMessageBox::warning((ApplicationWindow *)parent(), tr("SciDAVis") + " - " + tr("Error"),
		tr("The signal data set %1 does not exist!").arg(signalColName));
		m_init_err = true;
		return;
	}
	else if (response_col < 0)
	{
		QMessageBox::warning((ApplicationWindow *)parent(), tr("SciDAVis") + " - " + tr("Error"),
		tr("The response data set %1 does not exist!").arg(responseColName));
		m_init_err = true;
		return;
	}

    if (m_n > 0)
	{//delete previously allocated memory
		delete[] m_x;
		delete[] m_y;
	}

	m_n_response = 0;
	int rows = m_table->rowCount();
	for (int i=0; i<rows; i++)
	{
		if (!m_table->text(i, response_col).isEmpty())
			m_n_response++;
	}
	if (m_n_response >= rows/2)
	{
		QMessageBox::warning((ApplicationWindow *)parent(), tr("SciDAVis") + " - " + tr("Error"),
		tr("The response dataset '%1' must be less then half the size of the signal dataset '%2'!").arg(responseColName).arg(signalColName));
		m_init_err = true;
		return;
	}
	else if (m_n_response%2 == 0)
	{
		QMessageBox::warning((ApplicationWindow *)parent(), tr("SciDAVis") + " - " + tr("Error"),
		tr("The response dataset '%1' must contain an odd number of points!").arg(responseColName));
		m_init_err = true;
		return;
	}

	m_n = rows;

	m_n_signal = 16;// tmp number of points
	while (m_n_signal < m_n + m_n_response/2)
		m_n_signal *= 2;

    m_x = new double[m_n_signal]; //signal
	m_y = new double[m_n_response]; //response

    if(m_y && m_x)
	{
		memset( m_x, 0, m_n_signal * sizeof( double ) );// zero-pad signal data array
		for(int i=0; i<m_n; i++)
			m_x[i] = m_table->cell(i, signal_col);
		for(int i=0; i<m_n_response; i++)
			m_y[i] = m_table->cell(i, response_col);
	}
	else
	{
		QMessageBox::critical((ApplicationWindow *)parent(), tr("SciDAVis") + " - " + tr("Error"),
                        tr("Could not allocate memory, operation aborted!"));
        m_init_err = true;
		m_n = 0;
	}
}

void Convolution::output()
{
	convlv(m_x, m_n_signal, m_y, m_n_response, 1);
	addResultCurve();
}

void Convolution::addResultCurve()
{
    ApplicationWindow *app = (ApplicationWindow *)parent();
    if (!app)
        return;

	int cols = m_table->columnCount();
	int cols2 = cols+1;

	m_table->addCol();
	m_table->addCol();
	double x_temp[m_n];
	for (int i = 0; i<m_n; i++)
	{
		double x = i+1;
		x_temp[i] = x;

		m_table->setText(i, cols, QString::number(x));
		m_table->setText(i, cols2, QLocale().toString(m_x[i], 'g', app->m_decimal_digits));
	}

	QStringList l = m_table->colNames().grep(tr("Index"));
	QString id = QString::number((int)l.size()+1);
	QString label = name() + id;

	m_table->setColName(cols, tr("Index") + id);
	m_table->setColName(cols2, label);
	m_table->setColPlotDesignation(cols, SciDAVis::X);
	// TODO
	//m_table->setHeaderColType();

	Graph *graph = app->newGraph(name() + tr("Plot"));
	if (!graph)
        return;

    DataCurve *c = new DataCurve(m_table, m_table->colName(cols), m_table->colName(cols2));
	c->setData(x_temp, m_x, m_n);
	c->setPen(QPen(ColorBox::color(m_curveColorIndex), 1));
	graph->activeLayer()->insertPlotItem(c, Layer::Line);
	graph->activeLayer()->updatePlot();
}

void Convolution::convlv(double *sig, int n, double *dres, int m, int sign)
{
	double *res = new double[n];
	memset(res,0,n*sizeof(double));
	int i, m2 = m/2;
	for (i=0;i<m2;i++)
	{//store the response in wrap around order, see Numerical Recipes doc
		res[i] = dres[m2+i];
		res[n-m2+i] = dres[i];
	}

	if(m2%2==1)
		res[m2]=dres[m-1];

	// calculate ffts
	gsl_fft_real_radix2_transform(res,1,n);
	gsl_fft_real_radix2_transform(sig,1,n);

	double re, im, size;
	for (i=0;i<n/2;i++)
	{// multiply/divide both ffts
		if(i==0 || i==n/2-1)
		{
			if(sign == 1)
				sig[i] = res[i]*sig[i];
			else
				sig[i] = sig[i]/res[i];
		}
		else
		{
			int ni = n-i;
			if(sign == 1)
			{
				re = res[i]*sig[i]-res[ni]*sig[ni];
				im = res[i]*sig[ni]+res[ni]*sig[i];
			}
			else
			{
				size = res[i]*res[i]+res[ni]*res[ni];
				re = res[i]*sig[i]+res[ni]*sig[ni];
				im = res[i]*sig[ni]-res[ni]*sig[i];
				re /= size;
				im /= size;
			}

			sig[i] = re;
			sig[ni] = im;
		}
	}
	delete[] res;
	gsl_fft_halfcomplex_radix2_inverse(sig,1,n);// inverse fft
}
 /**************************************************************************
 *             Class Deconvolution                                         *
 ***************************************************************************/

Deconvolution::Deconvolution(ApplicationWindow *parent, Table *t, const QString& signalColName, const QString& responseColName)
: Convolution(parent, t, signalColName, responseColName)
{
	setName(tr("Deconvolution"));
    setDataFromTable(t, signalColName, responseColName);
}

void Deconvolution::output()
{
	convlv(m_x, signalDataSize(), m_y, responseDataSize(), -1);
	addResultCurve();
}
