/***************************************************************************
    File                 : PolynomialFit.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : Polynomial Fit and Linear Fit classes

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
#include "PolynomialFit.h"

#include <QMessageBox>
#include <QLocale>

#include <gsl/gsl_multifit.h>
#include <gsl/gsl_fit.h>

	PolynomialFit::PolynomialFit(ApplicationWindow *parent, Graph *g, int order, bool legend)
: Fit(parent, g), d_order(order), show_legend(legend)
{
	init();
}

	PolynomialFit::PolynomialFit(ApplicationWindow *parent, Graph *g, QString& curveTitle, int order, bool legend)
: Fit(parent, g), d_order(order), show_legend(legend)
{
	init();
	setDataFromCurve(curveTitle);
}

	PolynomialFit::PolynomialFit(ApplicationWindow *parent, Graph *g, QString& curveTitle, double start, double end, int order, bool legend)
: Fit(parent, g), d_order(order), show_legend(legend)
{
	init();
	setDataFromCurve(curveTitle, start, end);
}

void PolynomialFit::init()
{
	setName(tr("Poly"));
	is_non_linear = false;
	d_explanation = tr("Polynomial");
	d_p = d_order + 1;
    d_min_points = d_p;

	covar = gsl_matrix_alloc (d_p, d_p);
	d_results = new double[d_p];

	d_formula = generateFormula(d_order);
	d_param_names = generateParameterList(d_order);

	for (int i=0; i<d_p; i++)
		d_param_explain << "";
}

QString PolynomialFit::generateFormula(int order)
{
	QString formula;
	for (int i = 0; i < order+1; i++)
	{
		QString par = "a" + QString::number(i);
		formula += par;
		if (i>0)
			formula +="*x";
		if (i>1)
			formula += "^"+QString::number(i);
		if (i != order)
			formula += "+";
	}
	return formula;
}

QStringList PolynomialFit::generateParameterList(int order)
{
	QStringList lst;
	for (int i = 0; i < order+1; i++)
		lst << "a" + QString::number(i);
	return lst;
}

void PolynomialFit::calculateFitCurveData(double *par, double *X, double *Y)
{
	if (d_gen_function)
	{
		double X0 = d_x[0];
		double step = (d_x[d_n-1]-X0)/(d_points-1);
		for (int i=0; i<d_points; i++)
		{
			X[i] = X0+i*step;
			double 	yi = 0.0;
			for (int j=0; j<d_p;j++)
				yi += par[j]*pow(X[i],j);

			Y[i] = yi;
		}
	}
	else
	{
		for (int i=0; i<d_points; i++)
		{
			X[i] = d_x[i];
			double 	yi = 0.0;
			for (int j=0; j<d_p;j++)
				yi += par[j]*pow(X[i],j);

			Y[i] = yi;
		}
	}
}

void PolynomialFit::fit()
{
    if (d_init_err)
        return;

	if (d_p > d_n)
  	{
  		QMessageBox::critical((ApplicationWindow *)parent(), tr("QtiPlot - Fit Error"),
  	    tr("You need at least %1 data points for this fit operation. Operation aborted!").arg(d_p));
  		return;
  	}

	gsl_matrix *X = gsl_matrix_alloc (d_n, d_p);
	gsl_vector *c = gsl_vector_alloc (d_p);

	for (int i = 0; i <d_n; i++)
	{
		for (int j= 0; j < d_p; j++)
			gsl_matrix_set (X, i, j, pow(d_x[i],j));
	}

	gsl_vector_view y = gsl_vector_view_array (d_y, d_n);
	gsl_vector_view w = gsl_vector_view_array (d_w, d_n);
	gsl_multifit_linear_workspace * work = gsl_multifit_linear_alloc (d_n, d_p);

	if (d_weihting == NoWeighting)
		gsl_multifit_linear (X, &y.vector, c, covar, &chi_2, work);
	else
		gsl_multifit_wlinear (X, &w.vector, &y.vector, c, covar, &chi_2, work);

	for (int i = 0; i < d_p; i++)
		d_results[i] = gsl_vector_get(c, i);

	gsl_multifit_linear_free (work);
	gsl_matrix_free (X);
	gsl_vector_free (c);

	ApplicationWindow *app = (ApplicationWindow *)parent();
	if (app->writeFitResultsToLog)
		app->updateLog(logFitInfo(d_results, 0, 0, d_graph->parentPlotName()));

	if (show_legend)
		showLegend();

	generateFitCurve(d_results);
}

QString PolynomialFit::legendInfo()
{
	QString legend = "Y=" + QLocale().toString(d_results[0], 'g', d_prec);
	for (int j = 1; j < d_p; j++)
	{
		double cj = d_results[j];
		if (cj>0 && !legend.isEmpty())
			legend += "+";

		QString s;
		s.sprintf("%.5f",cj);
		if (s != "1.00000")
			legend += QLocale().toString(cj, 'g', d_prec);

		legend += "X";
		if (j>1)
			legend += "<sup>" + QString::number(j) + "</sup>";
	}
	return legend;
}

/*****************************************************************************
 *
 * Class LinearFit
 *
 *****************************************************************************/

	LinearFit::LinearFit(ApplicationWindow *parent, Graph *g)
: Fit(parent, g)
{
	init();
}

	LinearFit::LinearFit(ApplicationWindow *parent, Graph *g, const QString& curveTitle)
: Fit(parent, g)
{
	init();
	setDataFromCurve(curveTitle);
}

	LinearFit::LinearFit(ApplicationWindow *parent, Graph *g, const QString& curveTitle, double start, double end)
: Fit(parent, g)
{
	init();
	setDataFromCurve(curveTitle, start, end);
}

void LinearFit::init()
{
	d_p = 2;
    d_min_points = d_p;

	covar = gsl_matrix_alloc (d_p, d_p);
	d_results = new double[d_p];

	is_non_linear = false;
	d_formula = "A*x+B";
	d_param_names << "B" << "A";
	d_param_explain << "(y-intercept)" << "(slope)";
	d_explanation = tr("Linear Regression");
	setName(tr("Linear"));
}

void LinearFit::fit()
{
    if (d_init_err)
        return;

	if (d_p > d_n)
  	{
  		QMessageBox::critical((ApplicationWindow *)parent(), tr("QtiPlot - Fit Error"),
  	    tr("You need at least %1 data points for this fit operation. Operation aborted!").arg(d_p));
  		return;
  	}

	gsl_vector *c = gsl_vector_alloc (d_p);

	double c0, c1, cov00, cov01, cov11;
	if (d_weihting == NoWeighting)
		gsl_fit_linear(d_x, 1, d_y, 1, d_n, &c0, &c1, &cov00, &cov01, &cov11, &chi_2);
	else
		gsl_fit_wlinear(d_x, 1, d_w, 1, d_y, 1, d_n, &c0, &c1, &cov00, &cov01, &cov11, &chi_2);

	d_results[0] = c0;
	d_results[1] = c1;
	gsl_vector_free (c);

	gsl_matrix_set(covar, 0, 0, cov00);
	gsl_matrix_set(covar, 0, 1, cov01);
	gsl_matrix_set(covar, 1, 1, cov11);
	gsl_matrix_set(covar, 1, 0, cov01);

	ApplicationWindow *app = (ApplicationWindow *)parent();
	if (app->writeFitResultsToLog)
		app->updateLog(logFitInfo(d_results, 0, 0, d_graph->parentPlotName()));

	generateFitCurve(d_results);
}

void LinearFit::calculateFitCurveData(double *par, double *X, double *Y)
{
	if (d_gen_function)
	{
		double X0 = d_x[0];
		double step = (d_x[d_n-1]-X0)/(d_points-1);
		for (int i=0; i<d_points; i++)
		{
			X[i] = X0+i*step;
			Y[i] = par[0]+par[1]*X[i];
		}
	}
	else
	{
		for (int i=0; i<d_points; i++)
		{
			X[i] = d_x[i];
			Y[i] = par[0]+par[1]*X[i];
		}
	}
}
