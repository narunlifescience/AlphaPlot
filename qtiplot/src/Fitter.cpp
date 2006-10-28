/***************************************************************************
    File                 : Fitter.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : TODO
                           
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
#include "Fitter.h"
#include "fit.h"
#include "worksheet.h"
#include "ErrorBar.h"
#include "LegendMarker.h"
#include "parser.h"
#include "matrix.h"

#include <gsl/gsl_blas.h>

#include <qapplication.h>
#include <qlibrary.h>
#include <qmessagebox.h>
#include <QDateTime>

Fitter::Fitter( ApplicationWindow *parent, Graph *g)
: QObject( parent ),
	d_graph(g)
{
	d_curveColorIndex = 1;
	d_solver = ScaledLevenbergMarquardt;
	d_tolerance = 1e-4;
	gen_x_data = true;
	d_result_points = 100;
	d_max_iterations = 1000;
	d_curve = 0;
	d_formula = QString::null;
	d_fit_type = QString::null;
	s = 0;
	s_min = 0;
	d_weihting = NoWeighting;
	weighting_dataset = QString::null;
}

void Fitter::fitGSL(gsl_multifit_function_fdf f, int &iterations, int &status)
{
	const gsl_multifit_fdfsolver_type *T;
	if (d_solver)
		T = gsl_multifit_fdfsolver_lmder;
	else
		T = gsl_multifit_fdfsolver_lmsder;

	if (s)// free previousely allocated memory
		gsl_multifit_fdfsolver_free(s);

	s = gsl_multifit_fdfsolver_alloc (T, d_n, d_p);
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
}

void Fitter::fitSimplex(gsl_multimin_function f, int &iterations, int &status)
{
	const gsl_multimin_fminimizer_type *T = gsl_multimin_fminimizer_nmsimplex;

	//size of the simplex
	gsl_vector *ss;
	//initial vertex size vector
	ss = gsl_vector_alloc (f.n);
	//set all step sizes to 1 can be increased to converge faster
	gsl_vector_set_all (ss,10.0);

	if (s_min)// free previousely allocated memory
		gsl_multimin_fminimizer_free (s_min);

	s_min = gsl_multimin_fminimizer_alloc (T, f.n);
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
}

void Fitter::setDataFromCurve(QwtPlotCurve *curve, int start, int end)
{ 
	d_curve = curve;
	d_n = end - start + 1;

	d_x = new double[d_n];
	d_y = new double[d_n];
	d_w = new double[d_n];

	int aux = 0;
	for (int i = start; i <= end; i++)
	{// This is the data to be fitted 
		d_x[aux]=curve->x(i);
		d_y[aux]=curve->y(i);
		d_w[aux] = 1.0; // initialize the weighting data
		aux++;
	}
}

void Fitter::setDataFromCurve(const QString& curveTitle)
{  
	if (!d_graph)
		return;

	int n, start, end;
	QwtPlotCurve *c = d_graph->getValidCurve(curveTitle, d_p, n, start, end);
	if (!c)
		return;

	setDataFromCurve(c, start, end);
}

void Fitter::setDataFromCurve(const QString& curveTitle, double from, double to)
{  
	if (!d_graph)
		return;

	int start, end;
	QwtPlotCurve *c = d_graph->getFitLimits(curveTitle, from, to, d_p, start, end);
	if (!c)
		return;

	setDataFromCurve(c, start, end);
}

void Fitter::setInitialGuesses(double *x_init)
{
	for (int i = 0; i < d_p; i++)
		gsl_vector_set(d_param_init, i, x_init[i]);
}

void Fitter::setFitCurveParameters(bool generate, int points)
{
	gen_x_data = generate;
	if (gen_x_data)
		d_result_points = points;
	else
		d_result_points = d_n;
}

QString Fitter::logFitInfo(double *par, int iterations, int status)
{
	ApplicationWindow *app = (ApplicationWindow *)parent();
	int prec = app->fit_output_precision;

	QDateTime dt = QDateTime::currentDateTime ();
	QString info = dt.toString(Qt::LocalDate)+"\t " + tr("Fit") + QString::number(app->fitNumber)+ ":\n";
	info += d_fit_type +" " + tr("fit of dataset") + ": " + d_curve->title().text();
	if (!d_formula.isEmpty())
		info +=", " + tr("using function") + " " + d_formula + "\n";
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
		case ArbDataset:
			info += tr("Arbitrary Dataset") + ": " + weighting_dataset;
			break;
	}
	info +="\n";

	if (d_solver == NelderMeadSimplex)
		info+=tr("Nelder-Mead Simplex");
	else if (d_solver == UnscaledLevenbergMarquardt)
		info+=tr("Unscaled Levenberg-Marquardt");
	else
		info+=tr("Scaled Levenberg-Marquardt");

	info+=tr(" algorithm with tolerance = ")+QString::number(d_tolerance)+"\n";
	info+=tr("From x=")+QString::number(d_x[0]) +tr(" to x=")+QString::number(d_x[d_n-1])+"\n";

	for (int i=0; i<d_p; i++)
	{
		info += d_param_names[i] + " = " + QString::number(par[i], 'g', prec) + " +/- ";
		info += QString::number(sqrt(gsl_matrix_get(covar,i,i)), 'g', prec) + "\n";
	}
	info+="-----------------------------------\n";

	QString info2;
	if (d_solver == NelderMeadSimplex)
		info2.sprintf("Chi^2/doF = %g\n",  s_min->fval/(d_n - d_p));
	else
	{
		double chi = gsl_blas_dnrm2(s->f);
		info2.sprintf("Chi^2/doF = %g\n",  pow(chi, 2.0)/(d_n - d_p));
	}
	info+=info2;

	info +="-----------------------------------\n";
	info += tr("Iterations")+ " = " + QString::number(iterations) + "\n";
	info += tr("Status") + " = " + gsl_strerror (status) + "\n";
	info +="---------------------------------------------------------------------------------------\n";

	return info;
}

QString Fitter::legendFitInfo()
{
	ApplicationWindow *app = (ApplicationWindow *)parent();
	int prec = app->fit_output_precision;

	QString info = tr("Dataset") + ": " + d_curve->title().text() + "\n";

	info += tr("Function") + ": " + d_formula + "\n<br>";

	QString info2;
	if (d_solver == NelderMeadSimplex)
		info2.sprintf("Chi^2/doF = %g\n",  s_min->fval/(d_n - d_p));
	else
	{
		double chi = gsl_blas_dnrm2(s->f);
		info2.sprintf("Chi^2/doF = %g\n",  pow(chi, 2.0)/(d_n - d_p));
	}

	info += info2 + "<br>";
	for (int i=0; i<d_p; i++)
	{
		info += d_param_names[i] + " = " + QString::number(d_results[i], 'g', prec) + " +/- ";
		info += QString::number(sqrt(gsl_matrix_get(covar,i,i)), 'g', prec) + "\n";
	}
	return info;
}

bool Fitter::setWeightingData(WeightingMethod w, const QString& colName)
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
				QString yColName = d_curve->title().text();
				QStringList lst = d_graph->plotAssociations();
				bool error = true;
				QwtErrorPlotCurve *er = 0;
				for (int i=0; i<(int)lst.count(); i++)
				{
					if (lst[i].contains(yColName) && d_graph->curveType(i) == Graph::ErrorBars)
					{
						er = (QwtErrorPlotCurve *)d_graph->curve(i);
						if (er && !er->xErrors())
						{
							weighting_dataset = er->title().text();
							error = false;
							break;
						}
					}
				}
				if (error)
				{
					QMessageBox::critical((ApplicationWindow *)parent(), tr("Error"), 
							tr("The curve %1 has no associated Y error bars. You cannot use instrumental weighting method.").arg(yColName));
					return false;
				}

				for (int j=0; j<d_n; j++)
					d_w[j] = er->errorValue(j); //d_w are equal to the error bar values
			}
			break;
		case Statistical:
			{
				weighting_dataset = d_curve->title().text();

				for (int i=0; i<d_n; i++)
					d_w[i] = sqrt(d_y[i]);
			}
			break;
		case ArbDataset:
			{//d_w are equal to the values of the arbitrary dataset
				if (colName.isEmpty())
					return false;

				Table* t = ((ApplicationWindow *)parent())->table(colName);
				if (!t)
					return false;

				weighting_dataset = colName;

				int col = t->colIndex(colName);
				for (int i=0; i<d_n; i++)
					d_w[i] = t->text(i, col).toDouble(); 
			}
			break;
	}
	return true;
}

void Fitter::showParametersTable(const QString& tableName)
{
	ApplicationWindow *app = (ApplicationWindow *)parent();
	int prec = app->fit_output_precision;

	Table *t= app->newTable(tableName, d_p, 3);
	t->setHeader(QStringList() << tr("Parameter") << tr("Value") << tr ("Error"));
	for (int i=0; i<d_p; i++)
	{
		t->setText(i, 0, d_param_names[i]);
		t->setText(i, 1, QString::number(d_results[i], 'g', prec));
		t->setText(i, 2, QString::number(sqrt(gsl_matrix_get(covar,i,i)), 'g', prec));
	}

	t->setColPlotDesignation(2, Table::yErr);
	t->setHeaderColType();
	for (int j=0; j<3; j++)
		t->table()->adjustColumn(j);

	t->show();
	QApplication::restoreOverrideCursor();
}

void Fitter::showCovarianceMatrix(const QString& matrixName)
{
	ApplicationWindow *app = (ApplicationWindow *)parent();
	int prec = app->fit_output_precision;

	Matrix* m = app->newMatrix(matrixName, d_p, d_p);
	for (int i = 0; i < d_p; i++)
	{
		for (int j = 0; j < d_p; j++)
			m->setText(i, j, QString::number(gsl_matrix_get(covar, i, j), 'g', prec));
	}

	m->show();
	QApplication::restoreOverrideCursor();
}

QStringList Fitter::fitResultsList()
{
	ApplicationWindow *app = (ApplicationWindow *)parent();
	int prec = app->fit_output_precision;

	QStringList fit_results;
	for (int i=0; i<d_p; i++)
		fit_results << QString::number(d_results[i], 'g', prec);

	return fit_results;
}

void Fitter::storeCustomFitResults(double *par)
{
	for (int i=0; i<d_p; i++)
		d_results[i] = par[i];//store the results*/
}

void Fitter::fit()
{  
	if (!d_graph)
		return;

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
		fitSimplex(f, iterations, status);

		for (int i=0; i<d_p; i++)
			par[i]=gsl_vector_get(s_min->x, i);

		//allocate memory and calculate covariance matrix based on residuals
		gsl_matrix *J = gsl_matrix_alloc(d_n, d_p);
		d_df(s_min->x,(void*)f.params, J);
		gsl_multifit_covar (J, 0.0, covar);
		gsl_matrix_free (J);
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
		fitGSL(f, iterations, status);

		for (int i=0; i<d_p; i++)
			par[i]=gsl_vector_get(s->x, i);
	}

	storeCustomFitResults(par);

	ApplicationWindow *app = (ApplicationWindow *)parent();
	if (app->writeFitResultsToLog)
		app->updateLog(logFitInfo(d_results, iterations, status));

	double *X = new double[d_result_points]; 
	double *Y = new double[d_result_points]; 
	generateFitCurve(par, X, Y);

	d_graph->addResultCurve(d_result_points, X, Y, d_curveColorIndex, "Fit" + QString::number(app->fitNumber), 
			d_fit_type + tr(" fit of ") +  d_curve->title().text());
}

Fitter::~Fitter()
{
	delete[] d_x;
	delete[] d_y;
	delete[] d_w;
	delete[] d_results;
	gsl_vector_free(d_param_init);
	gsl_matrix_free (covar);

	if (s)
		gsl_multifit_fdfsolver_free (s);

	if (s_min)
		gsl_multimin_fminimizer_free (s_min);
}

/*****************************************************************************
 *
 * Class ExponentialFitter
 *
 *****************************************************************************/

	ExponentialFitter::ExponentialFitter(bool expGrowth, ApplicationWindow *parent, Graph *g)
: Fitter(parent, g),
	is_exp_growth(expGrowth)
{
	d_f = exp_f;
	d_df = exp_df;
	d_fdf = exp_fdf;
	d_fsimplex = exp_d;
	d_p = 3;
	d_param_init = gsl_vector_alloc(d_p);
	covar = gsl_matrix_alloc (d_p, d_p);
	d_results = new double[d_p];
	d_param_names << "A" << "t" << "y0";

	if (is_exp_growth)
	{
		d_fit_type = tr("Exponential growth");
		d_formula = "y = Aexp(x/t)+y0";
	}
	else
	{
		d_fit_type = tr("Exponential decay");
		d_formula = "y = Aexp(-x/t)+y0";
	}
}

void ExponentialFitter::storeCustomFitResults(double *par)
{
	for (int i=0; i<d_p; i++)
		d_results[i] = par[i];

	if (is_exp_growth)
		d_results[1]=-1.0/d_results[1];
	else
		d_results[1]=1.0/d_results[1];
}

void ExponentialFitter::generateFitCurve(double *par, double *X, double *Y)
{
	if (gen_x_data)
	{
		double X0 = d_x[0];
		double XN = d_x[d_n-1];
		double step = (XN-X0)/(d_result_points-1);
		for (int i=0; i<d_result_points; i++)
		{
			X[i] = X0+i*step;
			Y[i] = par[0]*exp(-par[1]*X[i])+par[2];
		}
	}
	else
	{
		for (int i=0; i<d_n; i++)
		{
			X[i] = d_x[i];
			Y[i] = par[0]*exp(-par[1]*X[i])+par[2];
		}
	}
	delete[] par;
}

/*****************************************************************************
 *
 * Class TwoExpFitter
 *
 *****************************************************************************/

	TwoExpFitter::TwoExpFitter(ApplicationWindow *parent, Graph *g)
: Fitter(parent, g)
{
	d_f = expd2_f;
	d_df = expd2_df;
	d_fdf = expd2_fdf;
	d_fsimplex = expd2_d;
	d_p = 5;
	d_param_init = gsl_vector_alloc(d_p);
	covar = gsl_matrix_alloc (d_p, d_p);
	d_results = new double[d_p];
	d_param_names << "A1" << "t1" << "A2" << "t2" << "y0";
	d_fit_type = tr("Exponential decay");
	d_formula = "y = A1*exp(-x/t1)+A2*exp(-x/t2)+y0";
}

void TwoExpFitter::storeCustomFitResults(double *par)
{
	for (int i=0; i<d_p; i++)
		d_results[i] = par[i];

	d_results[1]=1.0/d_results[1];
	d_results[3]=1.0/d_results[3];
}

void TwoExpFitter::generateFitCurve(double *par, double *X, double *Y)
{
	if (gen_x_data)
	{
		double X0 = d_x[0];
		double XN = d_x[d_n-1];
		double step = (XN-X0)/(d_result_points-1);
		for (int i=0; i<d_result_points; i++)
		{
			X[i] = X0+i*step;
			Y[i] = par[0]*exp(-par[1]*X[i])+par[2]*exp(-par[3]*X[i])+par[4];
		}
	}
	else
	{
		for (int i=0; i<d_n; i++)
		{
			X[i] = d_x[i];
			Y[i] = par[0]*exp(-par[1]*X[i])+par[2]*exp(-par[3]*X[i])+par[4];
		}
	}
	delete[] par;
}

/*****************************************************************************
 *
 * Class ThreeExpFitter
 *
 *****************************************************************************/

	ThreeExpFitter::ThreeExpFitter(ApplicationWindow *parent, Graph *g)
: Fitter(parent, g)
{
	d_f = expd3_f;
	d_df = expd3_df;
	d_fdf = expd3_fdf;
	d_fsimplex = expd3_d;
	d_p = 7;
	d_param_init = gsl_vector_alloc(d_p);
	covar = gsl_matrix_alloc (d_p, d_p);
	d_results = new double[d_p];
	d_param_names << "A1" << "t1" << "A2" << "t2" << "A3" << "t3" << "y0";
	d_fit_type = tr("Exponential decay");
	d_formula = "y = A1*exp(-x/t1)+A2*exp(-x/t2)+A3*exp(-x/t3)+y0";
}

void ThreeExpFitter::storeCustomFitResults(double *par)
{
	for (int i=0; i<d_p; i++)
		d_results[i] = par[i];

	d_results[1]=1.0/d_results[1];
	d_results[3]=1.0/d_results[3];
	d_results[5]=1.0/d_results[5];
}

void ThreeExpFitter::generateFitCurve(double *par, double *X, double *Y)
{
	if (gen_x_data)
	{
		double X0 = d_x[0];
		double XN = d_x[d_n-1];
		double step = (XN-X0)/(d_result_points-1);
		for (int i=0; i<d_result_points; i++)
		{
			X[i]=X0+i*step;
			Y[i]=par[0]*exp(-X[i]*par[1])+par[2]*exp(-X[i]*par[3])+par[4]*exp(-X[i]*par[5])+par[6];
		}
	}
	else
	{
		for (int i=0; i<d_n; i++)
		{
			X[i]=d_x[i];
			Y[i]=par[0]*exp(-X[i]*par[1])+par[2]*exp(-X[i]*par[3])+par[4]*exp(-X[i]*par[5])+par[6];
		}
	}
	delete[] par;
}

/*****************************************************************************
 *
 * Class SigmoidalFitter
 *
 *****************************************************************************/

	SigmoidalFitter::SigmoidalFitter(ApplicationWindow *parent, Graph *g)
: Fitter(parent, g)
{
	d_f = boltzmann_f;
	d_df = boltzmann_df;
	d_fdf = boltzmann_fdf;
	d_fsimplex = boltzmann_d;
	d_p = 4;
	d_param_init = gsl_vector_alloc(d_p);
	covar = gsl_matrix_alloc (d_p, d_p);
	d_results = new double[d_p];
	d_param_names << tr("A1 (init value)") << tr("A2 (final value)") << tr("x0 (center)") << tr("dx (time constant)");
	d_fit_type = tr("Boltzmann (Sigmoidal)");
	d_formula = "y = (A1-A2)/(1+exp((x-x0)/dx))+A2";
}

void SigmoidalFitter::generateFitCurve(double *par, double *X, double *Y)
{
	if (gen_x_data)
	{
		double X0 = d_x[0];
		double XN = d_x[d_n-1];
		double step = (XN-X0)/(d_result_points-1);
		for (int i=0; i<d_result_points; i++)
		{
			X[i] = X0+i*step;
			Y[i] = (par[0]-par[1])/(1+exp((X[i]-par[2])/par[3]))+par[1];
		}
	}
	else
	{
		for (int i=0; i<d_n; i++)
		{
			X[i] = d_x[i];
			Y[i] = (par[0]-par[1])/(1+exp((X[i]-par[2])/par[3]))+par[1];
		}
	}
	delete[] par;
}

void SigmoidalFitter::guessInitialValues()
{
	gsl_vector_view x = gsl_vector_view_array (d_x, d_n);
	gsl_vector_view y = gsl_vector_view_array (d_y, d_n);

	double min_out, max_out;
	gsl_vector_minmax (&y.vector, &min_out, &max_out);

	gsl_vector_set(d_param_init, 0, min_out);
	gsl_vector_set(d_param_init, 1, max_out);
	gsl_vector_set(d_param_init, 2, gsl_vector_get (&x.vector, d_n/2));
	gsl_vector_set(d_param_init, 3, 1.0);
}

/*****************************************************************************
 *
 * Class GaussFitter
 *
 *****************************************************************************/

	GaussFitter::GaussFitter(ApplicationWindow *parent, Graph *g)
: Fitter(parent, g)
{
	d_f = gauss_f;
	d_df = gauss_df;
	d_fdf = gauss_fdf;
	d_fsimplex = gauss_d;
	d_p = 4;
	d_param_init = gsl_vector_alloc(d_p);
	covar = gsl_matrix_alloc (d_p, d_p);
	d_results = new double[d_p];
	d_param_names << "y0 (offset)" << "A (height)" << "xc (center)" << "w (width)";
	d_fit_type = tr("Gauss");
	d_formula = "y = y0+A*exp[-(x-xc)^2/(2*w^2)]";
}

void GaussFitter::generateFitCurve(double *par, double *X, double *Y)
{
	double w2 = par[3]*par[3];
	if (gen_x_data)
	{
		double X0 = d_x[0];
		double XN = d_x[d_n-1];
		double step = (XN-X0)/(d_result_points-1);
		for (int i=0; i<d_result_points; i++)
		{
			X[i] = X0+i*step;
			double diff = X[i]-par[2];
			Y[i] = par[1]*exp(-0.5*diff*diff/w2)+par[0];
		}
	}
	else
	{
		for (int i=0; i<d_n; i++)
		{
			X[i] = d_x[i];		
			double diff = X[i]-par[2];
			Y[i] = par[1]*exp(-0.5*diff*diff/w2)+par[0];
		}
	}
	delete[] par;
}

void GaussFitter::guessInitialValues()
{
	gsl_vector_view x = gsl_vector_view_array (d_x, d_n);
	gsl_vector_view y = gsl_vector_view_array (d_y, d_n);

	double min_out, max_out;
	gsl_vector_minmax (&y.vector, &min_out, &max_out);

	gsl_vector_set(d_param_init, 0, max_out - min_out);
	gsl_vector_set(d_param_init, 1, gsl_vector_get(&x.vector, gsl_vector_max_index (&y.vector)));
	gsl_vector_set(d_param_init, 2, 1.0);
	gsl_vector_set(d_param_init, 3, min_out);
}

/*****************************************************************************
 *
 * Class LorentzFitter
 *
 *****************************************************************************/

	LorentzFitter::LorentzFitter(ApplicationWindow *parent, Graph *g)
: Fitter(parent, g)
{
	d_f = lorentz_f;
	d_df = lorentz_df;
	d_fdf = lorentz_fdf;
	d_fsimplex = lorentz_d;
	d_p = 4;
	d_param_init = gsl_vector_alloc(d_p);
	covar = gsl_matrix_alloc (d_p, d_p);
	d_results = new double[d_p];
	d_param_names << "y0 (offset)" << "A (area)" << "xc (center)" << "w (width)";
	d_fit_type = tr("Lorentz");
	d_formula = "y = y0+2A/pi*w/[4*(x-xc)^2+w^2)]";
}

void LorentzFitter::generateFitCurve(double *par, double *X, double *Y)
{
	double aw = par[1]*par[3];
	double w2 = par[3]*par[3];

	if (gen_x_data)
	{
		double X0 = d_x[0];
		double XN = d_x[d_n-1];
		double step = (XN-X0)/(d_result_points-1);
		for (int i=0; i<d_result_points; i++)
		{
			X[i] = X0+i*step;
			double diff = X[i]-par[2];
			Y[i] = aw/(4*diff*diff+w2)+par[0];
		}
	}
	else
	{
		for (int i=0; i<d_n; i++)
		{
			X[i] = d_x[i];		
			double diff = X[i]-par[2];
			Y[i] = aw/(4*diff*diff+w2)+par[0];
		}
	}
	delete[] par;
}

void LorentzFitter::guessInitialValues()
{
	gsl_vector_view x = gsl_vector_view_array (d_x, d_n);
	gsl_vector_view y = gsl_vector_view_array (d_y, d_n);

	double min_out, max_out;
	gsl_vector_minmax (&y.vector, &min_out, &max_out);

	gsl_vector_set(d_param_init, 0, 1.0);
	gsl_vector_set(d_param_init, 1, gsl_vector_get(&x.vector, gsl_vector_max_index (&y.vector)));
	gsl_vector_set(d_param_init, 2, 1.0);
	gsl_vector_set(d_param_init, 3, gsl_vector_min(&y.vector));
}

/*****************************************************************************
 *
 * Class NonLinearFitter
 *
 *****************************************************************************/

	NonLinearFitter::NonLinearFitter(ApplicationWindow *parent, Graph *g)
: Fitter(parent, g)
{
	d_f = user_f;
	d_df = user_df;
	d_fdf = user_fdf;
	d_fsimplex = user_d;
	d_fit_type = tr("Non-linear");
}

void NonLinearFitter::setParametersList(const QStringList& lst)
{
	d_param_names = lst;
	d_p = (int)lst.count();
	d_param_init = gsl_vector_alloc(d_p);
	covar = gsl_matrix_alloc (d_p, d_p);
	d_results = new double[d_p];
}

void NonLinearFitter::setInitialGuesses(const QStringList& lst)
{
	for (int i = 0; i < d_p; i++)
		gsl_vector_set(d_param_init, i, lst[i].toDouble());
}

void NonLinearFitter::generateFitCurve(double *par, double *X, double *Y)
{
	MyParser parser;
	for (int i=0; i<d_p; i++)
		parser.DefineVar(d_param_names[i].ascii(), &par[i]);

	double xvar;
	parser.DefineVar("x", &xvar);
	parser.SetExpr(d_formula.ascii());

	if (gen_x_data)
	{
		double X0 = d_x[0];
		double XN = d_x[d_n-1];
		double step = (XN-X0)/(d_result_points-1);
		for (int i=0; i<d_result_points; i++)
		{
			X[i] = X0+i*step;
			xvar = X[i];
			Y[i] = parser.Eval();
		}
	}
	else
	{
		for (int i=0; i<d_n; i++)
		{
			X[i] = d_x[i];		
			xvar = X[i];
			Y[i] = parser.Eval();
		}
	}
	delete[] par;
}

/*****************************************************************************
 *
 * Class PluginFitter
 *
 *****************************************************************************/

	PluginFitter::PluginFitter(ApplicationWindow *parent, Graph *g)
: Fitter(parent, g)
{
	d_fit_type = tr("Non-linear");
}

bool PluginFitter::load(const QString& pluginName)
{
	if (!QFile::exists (pluginName))
	{
		QMessageBox::critical((ApplicationWindow *)parent(), tr("QtiPlot - File not found"),
				tr("Plugin file: <p><b> %1 </b> <p>not found. Operation aborted!").arg(pluginName));
		return false;
	}

	QLibrary lib(pluginName);
	lib.setAutoUnload(false);

	d_fsimplex = (fit_function_simplex) lib.resolve( "function_d" );
	if (!d_fsimplex)
	{
		QMessageBox::critical((ApplicationWindow *)parent(), tr("QtiPlot - Plugin Error"), 
				tr("The plugin does not implement a %1 method necessary for simplex fitting.").arg("function_d"));
		return false;
	}

	d_f = (fit_function) lib.resolve( "function_f" );
	if (!d_f)
	{
		QMessageBox::critical((ApplicationWindow *)parent(), tr("QtiPlot - Plugin Error"), 
				tr("The plugin does not implement a %1 method necessary for Levenberg-Marquardt fitting.").arg("function_f"));
		return false;
	}

	d_df = (fit_function_df) lib.resolve( "function_df" );
	if (!d_df)
	{
		QMessageBox::critical((ApplicationWindow *)parent(), tr("QtiPlot - Plugin Error"), 
				tr("The plugin does not implement a %1 method necessary for Levenberg-Marquardt fitting.").arg("function_ff"));
		return false;
	}

	d_fdf = (fit_function_fdf) lib.resolve( "function_fdf" );
	if (!d_fdf)
	{
		QMessageBox::critical((ApplicationWindow *)parent(), tr("QtiPlot - Plugin Error"), 
				tr("The plugin does not implement a %1 method necessary for Levenberg-Marquardt fitting.").arg("function_fdf"));
		return false;
	}

	f_eval = (fitFunctionEval) lib.resolve("function_eval");
	if (!f_eval)
		return false;

	typedef char* (*fitFunc)();
	fitFunc fitFunction = (fitFunc) lib.resolve("parameters");
	if (fitFunction)
	{
		d_param_names = QStringList::split(",", QString(fitFunction()), false);
		d_p = (int)d_param_names.count();
		d_param_init = gsl_vector_alloc(d_p);
		covar = gsl_matrix_alloc (d_p, d_p);
		d_results = new double[d_p];
	}
	else
		return false;

	fitFunction = (fitFunc) lib.resolve( "name" );
	if (fitFunction)
		d_formula = QString(fitFunction()) + " = ";
	else
		return false;

	fitFunction = (fitFunc) lib.resolve( "function" );
	if (fitFunction)
		d_formula += QString(fitFunction());
	else
		return false;

	return true;
}

void PluginFitter::setInitialGuesses(const QStringList& lst)
{
	for (int i = 0; i < d_p; i++)
		gsl_vector_set(d_param_init, i, lst[i].toDouble());
}

void PluginFitter::generateFitCurve(double *par, double *X, double *Y)
{
	if (gen_x_data)
	{
		double X0 = d_x[0];
		double XN = d_x[d_n-1];
		double step = (XN-X0)/(d_result_points-1);
		for (int i=0; i<d_result_points; i++)
		{
			X[i] = X0+i*step;
			Y[i]= f_eval(X[i], par);
		}
	}
	else
	{
		for (int i=0; i<d_n; i++)
		{
			X[i] = d_x[i];		
			Y[i]= f_eval(X[i], par);
		}
	}
	delete[] par;
}

/*****************************************************************************
 *
 * Class MultiPeakFitter
 *
 *****************************************************************************/

	MultiPeakFitter::MultiPeakFitter(ApplicationWindow *parent, Graph *g, PeakProfile profile, int peaks)
: Fitter(parent, g),
	d_profile(profile),
	d_peaks(peaks)
{
	if (profile == Gauss)
	{
		d_fit_type = tr("Gauss");

		d_f = gauss_multi_peak_f;
		d_df = gauss_multi_peak_df;
		d_fdf = gauss_multi_peak_fdf;
		d_fsimplex = gauss_multi_peak_d;
	}
	else
	{
		d_fit_type = tr("Lorentz");

		d_f = lorentz_multi_peak_f;
		d_df = lorentz_multi_peak_df;
		d_fdf = lorentz_multi_peak_fdf;
		d_fsimplex = lorentz_multi_peak_d;
	}

	d_fit_type += "(" + QString::number(d_peaks) +") " + tr("multi-peak");

	d_p = 3*d_peaks + 1;
	d_param_init = gsl_vector_alloc(d_p);
	gsl_vector_set_all (d_param_init, 1.0);

	covar = gsl_matrix_alloc (d_p, d_p);
	d_results = new double[d_p];

	for (int i = 0; i<d_peaks; i++)
	{
		d_param_names << "A" + QString::number(i+1);
		d_param_names << "xc" + QString::number(i+1);
		d_param_names << "w" + QString::number(i+1);
	}
	d_param_names << tr("y0 (offset)");
}

void MultiPeakFitter::generateFitCurve(double *par, double *X, double *Y)
{
	double X0 = d_x[0];
	double XN = d_x[d_n-1];
	double step = (XN-X0)/(d_result_points-1);

	for (int i = 0; i<d_result_points; i++)
	{
		X[i] = X0 + i*step;

		double yi=0;
		for (int j=0; j<d_peaks; j++)
		{
			double diff = X[i] - par[3*j + 1];
			double w = par[3*j + 2];

			if (d_profile == Gauss)
				yi += par[3*j]*exp(-0.5*diff*diff/(w*w));
			else
				yi += par[3*j]*w/(4*diff*diff+w*w);
		}

		Y[i] = yi + par[d_p - 1];//add offset
	}

	delete[] par;
}

