/***************************************************************************
    File                 : fitclasses.cpp
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Benkert
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : MultiPeakFit module with Lorentz and Gauss peak shapes

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
#include "MultiPeakFit.h"
#include "fit_gsl.h"
#include "table/Table.h"
#include "graph/Layer.h"
#include "graph/PlotCurve.h"
#include "graph/FunctionCurve.h"
#include "lib/ColorBox.h"

#include <QLocale>
#include <QMessageBox>

	MultiPeakFit::MultiPeakFit(ApplicationWindow *parent, Layer *layer, PeakProfile profile, int peaks)
: Fit(parent, layer),
	m_profile(profile)
{
	setName(tr("MultiPeak"));

	if (profile == Gauss)
	{
		m_f = gauss_multi_peak_f;
		m_df = gauss_multi_peak_df;
		m_fdf = gauss_multi_peak_fdf;
		m_fsimplex = gauss_multi_peak_d;
	}
	else
	{
		m_f = lorentz_multi_peak_f;
		m_df = lorentz_multi_peak_df;
		m_fdf = lorentz_multi_peak_fdf;
		m_fsimplex = lorentz_multi_peak_d;
	}

	m_param_init = NULL;
	covar = NULL;
	m_results = NULL;

	setNumPeaks(peaks);

	generate_peak_curves = true;
	m_peaks_color = 2;//green
}

void MultiPeakFit::setNumPeaks(int n)
{
	m_peaks = n;
	if (m_profile == Gauss)
		m_explanation = tr("Gauss Fit");
	else
		m_explanation = tr("Lorentz Fit");
	if (m_peaks > 1)
		m_explanation += "(" + QString::number(m_peaks) +") " + tr("multi-peak");

	m_p = 3*m_peaks + 1;
    m_min_points = m_p;

	if(m_param_init) gsl_vector_free(m_param_init);
	m_param_init = gsl_vector_alloc(m_p);
	gsl_vector_set_all (m_param_init, 1.0);

	if (covar) gsl_matrix_free(covar);
	covar = gsl_matrix_alloc (m_p, m_p);
	if (m_results) delete[] m_results;
	m_results = new double[m_p];

	m_param_names = generateParameterList(m_peaks);
	m_param_explain = generateExplanationList(m_peaks);
	m_formula = generateFormula(m_peaks, m_profile);
}

QStringList MultiPeakFit::generateParameterList(int peaks)
{
	if (peaks == 1)
		return QStringList() << "A" << "xc" << "w" << "y0";

	QStringList lst;
	for (int i = 0; i<peaks; i++)
	{
		QString index = QString::number(i+1);
		lst << "A" + index;
		lst << "xc" + index;
		lst << "w" + index;
	}
	lst << "y0";
	return lst;
}

QStringList MultiPeakFit::generateExplanationList(int peaks)
{
	if (peaks == 1)
		return QStringList() << "(amplitude)" << "(center)" << "(width)" << "(offset)";

	QStringList lst;
	for (int i = 0; i<peaks; i++)
	{
		QString index = QString::number(i+1);
		lst << "(amplitude " + index + ")";
		lst << "(center " + index + ")";
		lst << "(width " + index + ")";
	}
	lst << "(offset)";
	return lst;
}

QString MultiPeakFit::generateFormula(int peaks, PeakProfile profile)
{
	if (peaks == 1)
		switch (profile)
		{
			case Gauss:
				return "y0+A*sqrt(2/PI)/w*exp(-2*((x-xc)/w)^2)";
				break;

			case Lorentz:
				return "y0+2*A/PI*w/(4*(x-xc)^2+w^2)";
				break;
		}

	QString formula = "y0+";
	for (int i = 0; i<peaks; i++)
	{
		formula += peakFormula(i+1, profile);
		if (i < peaks - 1)
			formula += "+";
	}
	return formula;
}

QString MultiPeakFit::peakFormula(int peakIndex, PeakProfile profile)
{
	QString formula;
	QString index = QString::number(peakIndex);
	switch (profile)
	{
		case Gauss:
			formula += "sqrt(2/PI)*A" + index + "/w" + index;
			formula += "*exp(-2*(x-xc" + index + ")^2/w" + index + "^2)";
			break;
		case Lorentz:
			formula += "2*A"+index+"/PI*w"+index+"/(4*(x-xc"+index+")^2+w"+index+"^2)";
			break;
	}
	return formula;
}

void MultiPeakFit::guessInitialValues()
{
	if (m_peaks > 1)
		return;

	gsl_vector_view x = gsl_vector_view_array (m_x, m_n);
	gsl_vector_view y = gsl_vector_view_array (m_y, m_n);

	double min_out, max_out;
	gsl_vector_minmax (&y.vector, &min_out, &max_out);

	if (m_profile == Gauss)
		gsl_vector_set(m_param_init, 0, sqrt(M_2_PI)*(max_out - min_out));
	else if (m_profile == Lorentz)
		gsl_vector_set(m_param_init, 0, 1.0);

	gsl_vector_set(m_param_init, 1, gsl_vector_get(&x.vector, gsl_vector_max_index (&y.vector)));
	gsl_vector_set(m_param_init, 2, 1.0);
	gsl_vector_set(m_param_init, 3, min_out);
}

void MultiPeakFit::storeCustomFitResults(double *par)
{
	for (int i=0; i<m_p; i++)
		m_results[i] = par[i];

	if (m_profile == Lorentz)
	{
		for (int j=0; j<m_peaks; j++)
			m_results[3*j] = M_PI_2*m_results[3*j];
	}
}

void MultiPeakFit::insertPeakFunctionCurve(double *x, double *y, int peak)
{
	QStringList curves = m_layer->curvesList();
	int index = 0;
	for (int i = 0; i<(int)curves.count(); i++ )
	{
		if (curves[i].startsWith(tr("Peak")))
			index++;
	}
	QString title = tr("Peak") + QString::number(++index);

	FunctionCurve *c = new FunctionCurve(FunctionCurve::Normal, title);
	c->setPen(QPen(ColorBox::color(m_peaks_color), 1));
	c->setData(x, y, m_points);
	c->setRange(m_x[0], m_x[m_n-1]);

	QString formula = "y0+"+peakFormula(peak + 1, m_profile);
	QString parameter = QString::number(m_results[m_p-1], 'g', m_prec);
	formula.replace(m_param_names[m_p-1], parameter);
	for (int j=0; j<3; j++)
	{
		int p = 3*peak + j;
		parameter = QString::number(m_results[p], 'g', m_prec);
		formula.replace(m_param_names[p], parameter);
	}
	c->setFormula(formula.replace("--", "+").replace("-+", "-").replace("+-", "-"));
	m_layer->insertPlotItem(c, Layer::Line);
	m_layer->addFitCurve(c);
}

void MultiPeakFit::generateFitCurve(double *par)
{
	ApplicationWindow *app = (ApplicationWindow *)parent();
	if (!m_gen_function)
		m_points = m_n;

	gsl_matrix * m = gsl_matrix_alloc (m_points, m_peaks);
	if (!m)
	{
		QMessageBox::warning(app, tr("Fit Error"), tr("Could not allocate enough memory for the fit curves!"));
		return;
	}

	double *X = new double[m_points];
	double *Y = new double[m_points];
	int i, j;
	int peaks_aux = m_peaks;
	if (m_peaks == 1)
		peaks_aux--;

	if (m_gen_function)
	{
		double step = (m_x[m_n-1] - m_x[0])/(m_points-1);
		for (i = 0; i<m_points; i++)
		{
			X[i] = m_x[0] + i*step;
			double yi=0;
			for (j=0; j<m_peaks; j++)
			{
				double diff = X[i] - par[3*j + 1];
				double w = par[3*j + 2];
				double y_aux = 0;
				if (m_profile == Gauss)
					y_aux += sqrt(M_2_PI)*par[3*j]/w*exp(-2*diff*diff/(w*w));
				else
					y_aux += par[3*j]*w/(4*diff*diff+w*w);

				yi += y_aux;
				y_aux += par[m_p - 1];
				gsl_matrix_set(m, i, j, y_aux);
			}
			Y[i] = yi + par[m_p - 1];//add offset
		}

		if (m_peaks > 1)
			insertFitFunctionCurve(QString(name()) + tr("Fit"), X, Y, 2);
		else
			insertFitFunctionCurve(QString(name()) + tr("Fit"), X, Y);

		if (generate_peak_curves)
		{
			for (i=0; i<peaks_aux; i++)
			{//add the peak curves
				for (j=0; j<m_points; j++)
					Y[j] = gsl_matrix_get (m, j, i);

				insertPeakFunctionCurve(X, Y, i);
			}
		}
	}
	else
	{
		QString tableName = app->generateUniqueName(tr("Fit"));
		QString label = m_explanation + " " + tr("fit of") + " " + m_curve->title().text();

		Table *t = app->newHiddenTable(tableName, label, m_points, peaks_aux + 2);
		QStringList header = QStringList() << "1";
		for (i = 0; i<peaks_aux; i++)
			header << tr("peak") + QString::number(i+1);
		header << "2";
		t->setHeader(header);

		for (i = 0; i<m_points; i++)
		{
			X[i] = m_x[i];
			t->setText(i, 0, QLocale().toString(X[i], 'g', m_prec));

			double yi=0;
			for (j=0; j<m_peaks; j++)
			{
				double diff = X[i] - par[3*j + 1];
				double w = par[3*j + 2];
				double y_aux = 0;
				if (m_profile == Gauss)
					y_aux += sqrt(M_2_PI)*par[3*j]/w*exp(-2*diff*diff/(w*w));
				else
					y_aux += par[3*j]*w/(4*diff*diff+w*w);

				yi += y_aux;
				y_aux += par[m_p - 1];
				t->setText(i, j+1, QLocale().toString(y_aux, 'g', m_prec));
				gsl_matrix_set(m, i, j, y_aux);
			}
			Y[i] = yi + par[m_p - 1];//add offset
			if (m_peaks > 1)
				t->setText(i, m_peaks+1, QLocale().toString(Y[i], 'g', m_prec));
		}

		label = tableName + "_2";
		DataCurve *c = new DataCurve(t, tableName + "_1", label);
		if (m_peaks > 1)
			c->setPen(QPen(ColorBox::color(m_curveColorIndex), 2));
		else
			c->setPen(QPen(ColorBox::color(m_curveColorIndex), 1));
		c->setData(X, Y, m_points);
		m_layer->insertPlotItem(c, Layer::Line);
		m_layer->addFitCurve(c);

		if (generate_peak_curves)
		{
			for (i=0; i<peaks_aux; i++)
			{//add the peak curves
				for (j=0; j<m_points; j++)
					Y[j] = gsl_matrix_get (m, j, i);

				label = tableName + "_" + tr("peak") + QString::number(i+1);
				c = new DataCurve(t, tableName + "_1", label);
				c->setPen(QPen(ColorBox::color(m_peaks_color), 1));
				c->setData(X, Y, m_points);
				m_layer->insertPlotItem(c, Layer::Line);
				m_layer->addFitCurve(c);
			}
		}
	}
	m_layer->replot();

	delete[] par;
	delete[] X;
	delete[] Y;
	gsl_matrix_free(m);
}

QString MultiPeakFit::logFitInfo(double *par, int iterations, int status, const QString& plotName)
{
	QString info = Fit::logFitInfo(par, iterations, status, plotName);
	if (m_peaks == 1)
		return info;

	info += tr("Peak") + "\t" + tr("Area") + "\t";
	info += tr("Center") + "\t" + tr("Width") + "\t" + tr("Height") + "\n";
	info += "---------------------------------------------------------------------------------------\n";
	for (int j=0; j<m_peaks; j++)
	{
		info += QString::number(j+1)+"\t";
		info += QLocale().toString(par[3*j],'g', m_prec)+"\t";
		info += QLocale().toString(par[3*j+1],'g', m_prec)+"\t";
		info += QLocale().toString(par[3*j+2],'g', m_prec)+"\t";

		if (m_profile == Lorentz)
			info += QLocale().toString(M_2_PI*par[3*j]/par[3*j+2],'g', m_prec)+"\n";
		else
			info += QLocale().toString(sqrt(M_2_PI)*par[3*j]/par[3*j+2],'g', m_prec)+"\n";
	}
	info += "---------------------------------------------------------------------------------------\n";
	return info;
}

/*****************************************************************************
 *
 * Class LorentzFit
 *
 *****************************************************************************/

	LorentzFit::LorentzFit(ApplicationWindow *parent, Layer *layer)
: MultiPeakFit(parent, layer, MultiPeakFit::Lorentz, 1)
{
	init();
}

	LorentzFit::LorentzFit(ApplicationWindow *parent, Layer *layer, const QString& curveTitle)
: MultiPeakFit(parent, layer, MultiPeakFit::Lorentz, 1)
{
	init();
	setDataFromCurve(curveTitle);
}

	LorentzFit::LorentzFit(ApplicationWindow *parent, Layer *layer, const QString& curveTitle, double start, double end)
: MultiPeakFit(parent, layer, MultiPeakFit::Lorentz, 1)
{
	init();
	setDataFromCurve(curveTitle, start, end);
}

void LorentzFit::init()
{
	setName("Lorentz");
	m_explanation = tr("Lorentz");
	m_param_explain << tr("(area)") << tr("(center)") << tr("(width)") << tr("(offset)");
}

/*****************************************************************************
 *
 * Class GaussFit
 *
 *****************************************************************************/

	GaussFit::GaussFit(ApplicationWindow *parent, Layer *layer)
: MultiPeakFit(parent, layer, MultiPeakFit::Gauss, 1)
{
	setName("Gauss");
	m_explanation = tr("Gauss");
	m_param_explain << tr("(area)") << tr("(center)") << tr("(width)") << tr("(offset)");
}

	GaussFit::GaussFit(ApplicationWindow *parent, Layer *layer, const QString& curveTitle)
: MultiPeakFit(parent, layer, MultiPeakFit::Gauss, 1)
{
	init();
	setDataFromCurve(curveTitle);
}

	GaussFit::GaussFit(ApplicationWindow *parent, Layer *layer, const QString& curveTitle, double start, double end)
: MultiPeakFit(parent, layer, MultiPeakFit::Gauss, 1)
{
	init();
	setDataFromCurve(curveTitle, start, end);
}

void GaussFit::init()
{
	setName("Gauss");
	m_explanation = tr("Gauss");
	m_param_explain << tr("(area)") << tr("(center)") << tr("(width)") << tr("(offset)");
}

/*****************************************************************************
 *
 * Class GaussAmpFit
 *
 *****************************************************************************/

	GaussAmpFit::GaussAmpFit(ApplicationWindow *parent, Layer *layer)
: Fit(parent, layer)
{
	init();
}

	GaussAmpFit::GaussAmpFit(ApplicationWindow *parent, Layer *layer, const QString& curveTitle)
: Fit(parent, layer)
{
	init();
	setDataFromCurve(curveTitle);
}

	GaussAmpFit::GaussAmpFit(ApplicationWindow *parent, Layer *layer, const QString& curveTitle, double start, double end)
: Fit(parent, layer)
{
	init();
	setDataFromCurve(curveTitle, start, end);
}

void GaussAmpFit::init()
{
	setName("GaussAmp");
	m_f = gauss_f;
	m_df = gauss_df;
	m_fdf = gauss_fdf;
	m_fsimplex = gauss_d;
	m_p = 4;
    m_min_points = m_p;
	m_param_init = gsl_vector_alloc(m_p);
	gsl_vector_set_all (m_param_init, 1.0);
	covar = gsl_matrix_alloc (m_p, m_p);
	m_results = new double[m_p];
	m_param_explain << tr("(offset)") << tr("(height)") << tr("(center)") << tr("(width)");
	m_param_names << "y0" << "A" << "xc" << "w";
	m_explanation = tr("GaussAmp Fit");
	m_formula = "y0+A*exp(-(x-xc)^2/(2*w^2))";
}

void GaussAmpFit::calculateFitCurveData(double *par, double *X, double *Y)
{
	double w2 = par[3]*par[3];
	if (m_gen_function)
	{
		double X0 = m_x[0];
		double step = (m_x[m_n-1]-X0)/(m_points-1);
		for (int i=0; i<m_points; i++)
		{
			X[i] = X0+i*step;
			double diff = X[i]-par[2];
			Y[i] = par[1]*exp(-0.5*diff*diff/w2)+par[0];
		}
	}
	else
	{
		for (int i=0; i<m_points; i++)
		{
			X[i] = m_x[i];
			double diff = X[i]-par[2];
			Y[i] = par[1]*exp(-0.5*diff*diff/w2)+par[0];
		}
	}
	delete[] par;
}
