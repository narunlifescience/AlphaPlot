/***************************************************************************
    File                 : Fit.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : Fit base class

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
#include "Fit.h"
#include "fit_gsl.h"
#include "Table.h"
#include "Matrix.h"
#include "QwtErrorPlotCurve.h"
#include "LegendMarker.h"
#include "FunctionCurve.h"
#include "ColorBox.h"

#include <gsl/gsl_statistics.h>
#include <gsl/gsl_blas.h>

#include <QApplication>
#include <QMessageBox>
#include <QDateTime>
#include <QLocale>

Fit::Fit( ApplicationWindow *parent, Graph *g, const char * name)
: Filter( parent, g, name)
{
	d_p = 0;
	d_n = 0;
	d_curveColorIndex = 1;
	d_solver = ScaledLevenbergMarquardt;
	d_tolerance = 1e-4;
	d_gen_function = true;
	d_points = 100;
	d_max_iterations = 1000;
	d_curve = 0;
	d_formula = QString::null;
	d_explanation = QString::null;
	d_weihting = NoWeighting;
	weighting_dataset = QString::null;
	is_non_linear = true;
	d_results = 0;
	d_errors = 0;
	d_prec = parent->fit_output_precision;
	d_init_err = false;
	chi_2 = -1;
	d_scale_errors = false;
	d_sort_data = true;
}

gsl_multifit_fdfsolver * Fit::fitGSL(gsl_multifit_function_fdf f, int &iterations, int &status)
{
	const gsl_multifit_fdfsolver_type *T;
	if (d_solver)
		T = gsl_multifit_fdfsolver_lmder;
	else
		T = gsl_multifit_fdfsolver_lmsder;

	gsl_multifit_fdfsolver *s = gsl_multifit_fdfsolver_alloc (T, d_n, d_p);
	gsl_multifit_fdfsolver_set (s, &f, d_param_init);

	size_t iter = 0;
	do
	{
		iter++;
		status = gsl_multifit_fdfsolver_iterate (s);

		if (status)
			break;

		status = gsl_multifit_test_delta (s->dx, s->x, d_tolerance, d_tolerance);
	}
	while (status == GSL_CONTINUE && (int)iter < d_max_iterations);

	gsl_multifit_covar (s->J, 0.0, covar);
	iterations = iter;
	return s;
}

gsl_multimin_fminimizer * Fit::fitSimplex(gsl_multimin_function f, int &iterations, int &status)
{
	const gsl_multimin_fminimizer_type *T = gsl_multimin_fminimizer_nmsimplex;

	//size of the simplex
	gsl_vector *ss;
	//initial vertex size vector
	ss = gsl_vector_alloc (f.n);
	//set all step sizes to 1 can be increased to converge faster
	gsl_vector_set_all (ss,10.0);

	gsl_multimin_fminimizer *s_min = gsl_multimin_fminimizer_alloc (T, f.n);
	status = gsl_multimin_fminimizer_set (s_min, &f, d_param_init, ss);
	double size;
	size_t iter = 0;
	do
	{
		iter++;
		status = gsl_multimin_fminimizer_iterate (s_min);

		if (status)
			break;
		size=gsl_multimin_fminimizer_size (s_min);
		status = gsl_multimin_test_size (size, d_tolerance);
	}

	while (status == GSL_CONTINUE && (int)iter < d_max_iterations);

	iterations = iter;
	gsl_vector_free(ss);
	return s_min;
}

void Fit::setDataCurve(int curve, double start, double end)
{
    if (d_n > 0)
		delete[] d_w;

    Filter::setDataCurve(curve, start, end);

    d_w = new double[d_n];
    if (d_graph && d_curve && ((PlotCurve *)d_curve)->type() != Graph::Function)
    {
        QList<DataCurve *> lst = ((DataCurve *)d_curve)->errorBarsList();
        foreach (DataCurve *c, lst)
        {
            QwtErrorPlotCurve *er = (QwtErrorPlotCurve *)c;
            if (!er->xErrors())
            {
                d_weihting = Instrumental;
                for (int i=0; i<d_n; i++)
                    d_w[i] = er->errorValue(i); //d_w are equal to the error bar values
                weighting_dataset = er->title().text();
                return;
            }
        }
    }
	// if no error bars initialize the weighting data to 1.0
    for (int i=0; i<d_n; i++)
        d_w[i] = 1.0;
}

void Fit::setInitialGuesses(double *x_init)
{
	for (int i = 0; i < d_p; i++)
		gsl_vector_set(d_param_init, i, x_init[i]);
}

void Fit::generateFunction(bool yes, int points)
{
	d_gen_function = yes;
	if (d_gen_function)
		d_points = points;
}

QString Fit::logFitInfo(double *par, int iterations, int status, const QString& plotName)
{
	QDateTime dt = QDateTime::currentDateTime ();
	QString info = "[" + dt.toString(Qt::LocalDate)+ "\t" + tr("Plot")+ ": ''" + plotName+ "'']\n";
	info += d_explanation + " " + tr("fit of dataset") + ": " + d_curve->title().text();
	if (!d_formula.isEmpty())
		info +=", " + tr("using function") + ": " + d_formula + "\n";
	else
		info +="\n";

	info += tr("Weighting Method") + ": ";
	switch(d_weihting)
	{
		case NoWeighting:
			info += tr("No weighting");
			break;
		case Instrumental:
			info += tr("Instrumental") + ", " + tr("using error bars dataset") + ": " + weighting_dataset;
			break;
		case Statistical:
			info += tr("Statistical");
			break;
		case Dataset:
			info += tr("Arbitrary Dataset") + ": " + weighting_dataset;
			break;
	}
	info +="\n";

	if (is_non_linear)
	{
		if (d_solver == NelderMeadSimplex)
			info+=tr("Nelder-Mead Simplex");
		else if (d_solver == UnscaledLevenbergMarquardt)
			info+=tr("Unscaled Levenberg-Marquardt");
		else
			info+=tr("Scaled Levenberg-Marquardt");

		info+=tr(" algorithm with tolerance = ") + QLocale().toString(d_tolerance)+"\n";
	}

	info+=tr("From x")+" = "+QLocale().toString(d_x[0], 'g', 15)+" "+tr("to x")+" = "+QLocale().toString(d_x[d_n-1], 'g', 15)+"\n";
	double chi_2_dof = chi_2/(d_n - d_p);
	for (int i=0; i<d_p; i++)
	{
		info += d_param_names[i]+" "+d_param_explain[i]+" = "+QLocale().toString(par[i], 'g', d_prec) + " +/- ";
		if (d_scale_errors)
			info += QLocale().toString(sqrt(chi_2_dof*gsl_matrix_get(covar,i,i)), 'g', d_prec) + "\n";
		else
			info += QLocale().toString(sqrt(gsl_matrix_get(covar,i,i)), 'g', d_prec) + "\n";
	}
	info += "--------------------------------------------------------------------------------------\n";
	info += "Chi^2/doF = " + QLocale().toString(chi_2_dof, 'g', d_prec) + "\n";

	double sst = (d_n-1)*gsl_stats_variance(d_y, 1, d_n);
	info += tr("R^2") + " = " + QLocale().toString(1 - chi_2/sst, 'g', d_prec) + "\n";
	info += "---------------------------------------------------------------------------------------\n";
	if (is_non_linear)
	{
		info += tr("Iterations")+ " = " + QString::number(iterations) + "\n";
		info += tr("Status") + " = " + gsl_strerror (status) + "\n";
		info +="---------------------------------------------------------------------------------------\n";
	}
	return info;
}

double Fit::rSquare()
{
	double sst = (d_n-1)*gsl_stats_variance(d_y, 1, d_n);
	return 1 - chi_2/sst;
}

QString Fit::legendInfo()
{
	QString info = tr("Dataset") + ": " + d_curve->title().text() + "\n";
	info += tr("Function") + ": " + d_formula + "\n\n";

	double chi_2_dof = chi_2/(d_n - d_p);
	info += "Chi^2/doF = " + QLocale().toString(chi_2_dof, 'g', d_prec) + "\n";
	double sst = (d_n-1)*gsl_stats_variance(d_y, 1, d_n);
	info += tr("R^2") + " = " + QLocale().toString(1 - chi_2/sst, 'g', d_prec) + "\n";

	for (int i=0; i<d_p; i++)
	{
		info += d_param_names[i] + " = " + QLocale().toString(d_results[i], 'g', d_prec) + " +/- ";
		if (d_scale_errors)
			info += QLocale().toString(sqrt(chi_2_dof*gsl_matrix_get(covar,i,i)), 'g', d_prec) + "\n";
		else
			info += QLocale().toString(sqrt(gsl_matrix_get(covar,i,i)), 'g', d_prec) + "\n";
	}
	return info;
}

bool Fit::setWeightingData(WeightingMethod w, const QString& colName)
{	
	d_weihting = w;
	switch (d_weihting)
	{
		case NoWeighting:
			{
				weighting_dataset = QString::null;
				for (int i=0; i<d_n; i++)
					d_w[i] = 1.0;
			}
			break;
		case Instrumental:
			{
				bool error = true;
				QwtErrorPlotCurve *er = 0;
				if (((PlotCurve *)d_curve)->type() != Graph::Function)
				{
					QList<DataCurve *> lst = ((DataCurve *)d_curve)->errorBarsList();
                	foreach (DataCurve *c, lst)
                	{
                    	er = (QwtErrorPlotCurve *)c;
                    	if (!er->xErrors())
                    	{
                        	weighting_dataset = er->title().text();
                        	error = false;
                        	break;
                    	}
					}
                }
				if (error)
				{
					QMessageBox::critical((ApplicationWindow *)parent(), tr("QtiPlot - Error"),
					tr("The curve %1 has no associated Y error bars. You cannot use instrumental weighting method.").arg(d_curve->title().text()));
					return false;
				}
				if (er)
				{
					for (int j=0; j<d_n; j++)
						d_w[j] = er->errorValue(j); //d_w are equal to the error bar values
				}
			}
			break;
		case Statistical:
			{
				weighting_dataset = d_curve->title().text();

				for (int i=0; i<d_n; i++)
					d_w[i] = sqrt(d_y[i]);
			}
			break;
		case Dataset:
			{//d_w are equal to the values of the arbitrary dataset
				if (colName.isEmpty())
					return false;

				Table* t = ((ApplicationWindow *)parent())->table(colName);
				if (!t)
					return false;

				if (t->tableRows() < d_n)
  	            {
  	            	QMessageBox::critical((ApplicationWindow *)parent(), tr("QtiPlot - Error"),
  	                tr("The column %1 has less points than the fitted data set. Please choose another column!.").arg(colName));
  	                return false;
  	            }

				weighting_dataset = colName;

				int col = t->colIndex(colName);
				for (int i=0; i<d_n; i++)
					d_w[i] = t->cell(i, col);
			}
			break;
	}
	return true;
}

Table* Fit::parametersTable(const QString& tableName)
{
	ApplicationWindow *app = (ApplicationWindow *)parent();
	Table *t = app->newTable(tableName, d_p, 3);
	t->setHeader(QStringList() << tr("Parameter") << tr("Value") << tr ("Error"));
	for (int i=0; i<d_p; i++)
	{
		t->setText(i, 0, d_param_names[i]);
		t->setText(i, 1, QLocale().toString(d_results[i], 'g', d_prec));
		t->setText(i, 2, QLocale().toString(sqrt(gsl_matrix_get(covar,i,i)), 'g', d_prec));
	}

	t->setColPlotDesignation(2, Table::yErr);
	t->setHeaderColType();
	for (int j=0; j<3; j++)
		t->table()->adjustColumn(j);

	t->showNormal();
	return t;
}

Matrix* Fit::covarianceMatrix(const QString& matrixName)
{
	ApplicationWindow *app = (ApplicationWindow *)parent();
	Matrix* m = app->newMatrix(matrixName, d_p, d_p);
	for (int i = 0; i < d_p; i++)
	{
		for (int j = 0; j < d_p; j++)
			m->setText(i, j, QLocale().toString(gsl_matrix_get(covar, i, j), 'g', d_prec));
	}
	m->showNormal();
	return m;
}

double *Fit::errors()
{
	if (!d_errors) {
		d_errors = new double[d_p];
		double chi_2_dof = chi_2/(d_n - d_p);
		for (int i=0; i<d_p; i++)
		{
			if (d_scale_errors)
				d_errors[i] = sqrt(chi_2_dof*gsl_matrix_get(covar,i,i));
			else
				d_errors[i] = sqrt(gsl_matrix_get(covar,i,i));
		}
	}
	return d_errors;
}

void Fit::storeCustomFitResults(double *par)
{
	for (int i=0; i<d_p; i++)
		d_results[i] = par[i];
}

void Fit::fit()
{
	if (!d_graph || d_init_err)
		return;

	if (!d_n)
	{
		QMessageBox::critical((ApplicationWindow *)parent(), tr("QtiPlot - Fit Error"),
				tr("You didn't specify a valid data set for this fit operation. Operation aborted!"));
		return;
	}
	if (!d_p)
	{
		QMessageBox::critical((ApplicationWindow *)parent(), tr("QtiPlot - Fit Error"),
				tr("There are no parameters specified for this fit operation. Operation aborted!"));
		return;
	}
	if (d_p > d_n)
  	{
  		QMessageBox::critical((ApplicationWindow *)parent(), tr("QtiPlot - Fit Error"),
  	    tr("You need at least %1 data points for this fit operation. Operation aborted!").arg(d_p));
  	    return;
  	}
	if (d_formula.isEmpty())
	{
		QMessageBox::critical((ApplicationWindow *)parent(), tr("QtiPlot - Fit Error"),
				tr("You must specify a valid fit function first. Operation aborted!"));
		return;
	}

	QApplication::setOverrideCursor(Qt::WaitCursor);

	const char *function = d_formula.ascii();
	QString names = d_param_names.join (",");
	const char *parNames = names.ascii();

	struct FitData d_data = {d_n, d_p, d_x, d_y, d_w, function, parNames};

	int status, iterations = d_max_iterations;
	double *par = new double[d_p];
	if(d_solver == NelderMeadSimplex)
	{
		gsl_multimin_function f;
		f.f = d_fsimplex;
		f.n = d_p;
		f.params = &d_data;
		gsl_multimin_fminimizer *s_min = fitSimplex(f, iterations, status);

		for (int i=0; i<d_p; i++)
			par[i]=gsl_vector_get(s_min->x, i);

		// allocate memory and calculate covariance matrix based on residuals
		gsl_matrix *J = gsl_matrix_alloc(d_n, d_p);
		d_df(s_min->x,(void*)f.params, J);
		gsl_multifit_covar (J, 0.0, covar);
		chi_2 = s_min->fval;

		// free previousely allocated memory
		gsl_matrix_free (J);
		gsl_multimin_fminimizer_free (s_min);
	}
	else
	{
		gsl_multifit_function_fdf f;
		f.f = d_f;
		f.df = d_df;
		f.fdf = d_fdf;
		f.n = d_n;
		f.p = d_p;
		f.params = &d_data;
		gsl_multifit_fdfsolver *s = fitGSL(f, iterations, status);

		for (int i=0; i<d_p; i++)
			par[i]=gsl_vector_get(s->x, i);

		chi_2 = pow(gsl_blas_dnrm2(s->f), 2.0);
		gsl_multifit_fdfsolver_free(s);
	}

	storeCustomFitResults(par);

	ApplicationWindow *app = (ApplicationWindow *)parent();
	if (app->writeFitResultsToLog)
		app->updateLog(logFitInfo(d_results, iterations, status, d_graph->parentPlotName()));

	generateFitCurve(par);
	QApplication::restoreOverrideCursor();
}

void Fit::generateFitCurve(double *par)
{
	if (!d_gen_function)
		d_points = d_n;

	double *X = new double[d_points];
	double *Y = new double[d_points];

	calculateFitCurveData(par, X, Y);

	if (d_gen_function)
	{
		insertFitFunctionCurve(QString(name()) + tr("Fit"), X, Y);
		d_graph->replot();
		delete[] X;
		delete[] Y;
	}
	else
        d_graph->addFitCurve(addResultCurve(X, Y));
}

void Fit::insertFitFunctionCurve(const QString& name, double *x, double *y, int penWidth)
{
    QString title = d_graph->generateFunctionName(name);
	FunctionCurve *c = new FunctionCurve(FunctionCurve::Normal, title);
	c->setPen(QPen(ColorBox::color(d_curveColorIndex), penWidth));
	c->setData(x, y, d_points);
	c->setRange(d_x[0], d_x[d_n-1]);

	QString formula = d_formula;
	for (int j=0; j<d_p; j++)
	{
		QString parameter = QString::number(d_results[j], 'g', d_prec);
		formula.replace(d_param_names[j], parameter);
	}
	c->setFormula(formula.replace("--", "+").replace("-+", "-").replace("+-", "-"));
	d_graph->insertPlotItem(c, Graph::Line);
	d_graph->addFitCurve(c);
}

Fit::~Fit()
{
	if (!d_p)
		return;

	if (is_non_linear)
		gsl_vector_free(d_param_init);

	if (d_results) delete[] d_results;
	if (d_errors) delete[] d_errors;
	gsl_matrix_free (covar);
}
