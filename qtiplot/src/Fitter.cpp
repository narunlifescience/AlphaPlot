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
#include "FunctionCurve.h"
#include "colorBox.h"

#include <gsl/gsl_blas.h>
#include <gsl/gsl_multifit_nlin.h>
#include <gsl/gsl_multifit.h>
#include <gsl/gsl_fit.h>
#include <gsl/gsl_statistics.h>

#include <qapplication.h>
#include <qlibrary.h>
#include <qmessagebox.h>
#include <QDateTime>

Fit::Fit( ApplicationWindow *parent, Graph *g, const char * name)
: QObject( parent, name),
	d_graph(g)
{
	d_p = 0;
	d_n = 0;
	d_curveColorIndex = 1;
	d_solver = ScaledLevenbergMarquardt;
	d_tolerance = 1e-4;
	d_gen_function = true;
	d_result_points = 100;
	d_max_iterations = 1000;
	d_curve = 0;
	d_formula = QString::null;
	d_fit_type = QString::null;
	d_weihting = NoWeighting;
	weighting_dataset = QString::null;
	is_non_linear = true;
	d_results = 0;
	d_errors = 0;
	d_prec = parent->fit_output_precision;
	d_init_err = false;
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

void Fit::setInterval(double from, double to)
{ 
	if (!d_curve)
	{
		QMessageBox::critical((ApplicationWindow *)parent(), tr("QtiPlot - Fit Error"), 
				tr("No curve assigned to the fitter! Please assign a curve first!"));
		return;
	}
	setDataFromCurve (d_curve->title().text(), from, to);
}

void Fit::setDataFromCurve(QwtPlotCurve *curve, int start, int end)
{ 
	if (d_n > 0)
	{//delete previousely allocated memory
		delete[] d_x;
		delete[] d_y;
		delete[] d_w;
	}

	d_init_err = false;
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

bool Fit::setDataFromCurve(const QString& curveTitle, Graph *g)
{ 
	if (curveTitle.isEmpty())
	{
		QMessageBox::critical((ApplicationWindow *)parent(), tr("QtiPlot - Fit Error"), 
				tr("Please enter a valid curve name!"));
		d_init_err = true;
		return false;
	}

	if (g)
		d_graph = g;

	if (!d_graph)
	{
		d_init_err = true;
		return false;
	}

	int n, start, end;
	QwtPlotCurve *c = d_graph->getValidCurve(curveTitle, d_p, n, start, end);
	if (!c)
	{
		d_init_err = true;
		return false;
	}

	setDataFromCurve(c, start, end);
	return true;
}

bool Fit::setDataFromCurve(const QString& curveTitle, double from, double to, Graph *g)
{  
	if (curveTitle.isEmpty())
	{
		QMessageBox::critical((ApplicationWindow *)parent(), tr("QtiPlot - Fit Error"), 
				tr("Please enter a valid curve name!"));
		d_init_err = true;
		return false;
	}

	if (g)
		d_graph = g;

	if (!d_graph)
	{
		d_init_err = true;
		return false;
	}

	int start, end;
	QwtPlotCurve *c = d_graph->getFitLimits(curveTitle, from, to, d_p, start, end);
	if (!c)
	{
		d_init_err = true;
		return false;
	}

	setDataFromCurve(c, start, end);
	return true;
}

void Fit::setInitialGuesses(double *x_init)
{
	for (int i = 0; i < d_p; i++)
		gsl_vector_set(d_param_init, i, x_init[i]);
}

void Fit::setColor(const QColor& c)
{
	d_curveColorIndex = ColorBox::colorIndex(c);	
}

void Fit::generateFunction(bool yes, int points)
{
	d_gen_function = yes;
	if (d_gen_function)
		d_result_points = points;
}

QString Fit::logFitInfo(double *par, int iterations, int status, const QString& plotName)
{
	QDateTime dt = QDateTime::currentDateTime ();
	QString info = "[" + dt.toString(Qt::LocalDate)+ "\t" + tr("Plot")+ ": ''" + plotName+ "'']\n";
	info += d_fit_type +" " + tr("fit of dataset") + ": " + d_curve->title().text();
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
		case ArbDataset:
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

		info+=tr(" algorithm with tolerance = ")+QString::number(d_tolerance)+"\n";
	}

	info+=tr("From x")+" = "+QString::number(d_x[0], 'g', 15)+" "+tr("to x")+" = "+QString::number(d_x[d_n-1], 'g', 15)+"\n";

	for (int i=0; i<d_p; i++)
	{
		info += d_param_names[i]+" "+d_param_explain[i]+" = "+QString::number(par[i], 'g', d_prec) + " +/- ";
		info += QString::number(sqrt(gsl_matrix_get(covar,i,i)), 'g', d_prec) + "\n";
	}
	info += "--------------------------------------------------------------------------------------\n";

	QString info2;
	info2.sprintf("Chi^2/doF = %g\n",  chi_2/(d_n - d_p));
	info+=info2;
	double sst = (d_n-1)*gsl_stats_variance(d_y, 1, d_n);
	info2.sprintf(tr("R^2") + " = %g\n",  1 - chi_2/sst);
	info+=info2;
	info += "---------------------------------------------------------------------------------------\n";
	if (is_non_linear)
	{
		info += tr("Iterations")+ " = " + QString::number(iterations) + "\n";
		info += tr("Status") + " = " + gsl_strerror (status) + "\n";
		info +="---------------------------------------------------------------------------------------\n";
	}
	return info;
}

void Fit::showLegend()
{
	ApplicationWindow *app = (ApplicationWindow *)parent();
	LegendMarker* mrk = d_graph->newLegend(legendFitInfo());
	if (d_graph->hasLegend())
	{
		LegendMarker* legend = d_graph->legend();
		QPoint p = legend->rect().bottomLeft();
		mrk->setOrigin(QPoint(p.x(), p.y()+20));
	}
	d_graph->replot();
}

QString Fit::legendFitInfo()
{
	QString info = tr("Dataset") + ": " + d_curve->title().text() + "\n";
	info += tr("Function") + ": " + d_formula + "\n<br>";

	QString info2;
	info2.sprintf("Chi^2/doF = %g\n",  chi_2/(d_n - d_p));
	info+=info2;
	double sst = (d_n-1)*gsl_stats_variance(d_y, 1, d_n);
	info2.sprintf(" R^2 = %g\n",  1 - chi_2/sst);
	info += info2 + "<br>";
	for (int i=0; i<d_p; i++)
	{
		info += d_param_names[i] + " = " + QString::number(d_results[i], 'g', d_prec) + " +/- ";
		info += QString::number(sqrt(gsl_matrix_get(covar,i,i)), 'g', d_prec) + "\n";
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

Table* Fit::parametersTable(const QString& tableName)
{
	ApplicationWindow *app = (ApplicationWindow *)parent();
	Table *t = app->newTable(tableName, d_p, 3);
	t->setHeader(QStringList() << tr("Parameter") << tr("Value") << tr ("Error"));
	for (int i=0; i<d_p; i++)
	{
		t->setText(i, 0, d_param_names[i]);
		t->setText(i, 1, QString::number(d_results[i], 'g', d_prec));
		t->setText(i, 2, QString::number(sqrt(gsl_matrix_get(covar,i,i)), 'g', d_prec));
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
			m->setText(i, j, QString::number(gsl_matrix_get(covar, i, j), 'g', d_prec));
	}
	m->showNormal();
	return m;
}

double *Fit::errors()
{
	if (!d_errors) {
		d_errors = new double[d_p];
		for (int i=0; i<d_p; i++)
			d_errors[i] = sqrt(gsl_matrix_get(covar,i,i));
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
				tr("You didn't specify a data set for this fit operation. Operation aborted!"));
		return;
	}
	if (!d_p)
	{
		QMessageBox::critical((ApplicationWindow *)parent(), tr("QtiPlot - Fit Error"),
				tr("There are no parameters specified for this fit operation. Operation aborted!"));
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
		d_result_points = d_n;

	double *X = new double[d_result_points]; 
	double *Y = new double[d_result_points]; 

	calculateFitCurveData(par, X, Y);

	ApplicationWindow *app = (ApplicationWindow *)parent();
	if (d_gen_function)
	{
		insertFitFunctionCurve(QString(name()) + tr("Fit"), X, Y);
		d_graph->replot();
		delete[] X;
		delete[] Y;
	}
	else
	{
		d_graph->addResultCurve(d_result_points, X, Y, d_curveColorIndex, 
				app->generateUnusedName(QString(name())+tr("Fit")), d_fit_type + tr(" of ") + d_curve->title().text());
	}
}

void Fit::insertFitFunctionCurve(const QString& name, double *x, double *y, int penWidth)
{
	QStringList curves = d_graph->curvesList();
	int index = 0;
	for (int i = 0; i<(int)curves.count(); i++ )
	{
		if (curves[i].startsWith(name))
			index++;
	}
	QString title = name + QString::number(++index);

	FunctionCurve *c = new FunctionCurve(FunctionCurve::Normal, title);
	c->setPen(QPen(ColorBox::color(d_curveColorIndex), penWidth)); 
	c->setData(x, y, d_result_points);
	c->setRange(d_x[0], d_x[d_n-1]);

	QString formula = d_formula;
	for (int j=0; j<d_p; j++)
	{
		QString parameter = QString::number(d_results[j], 'g', d_prec);
		formula.replace(d_param_names[j], parameter);
	}
	c->setFormula(formula);
	d_graph->insertCurve(c, title);
}

Fit::~Fit()
{
	if (d_n > 0)
	{//delete the memory allocated for the fitting data
		delete[] d_x;
		delete[] d_y;
		delete[] d_w;
	}

	if (!d_p)
		return;

	if (is_non_linear)
		gsl_vector_free(d_param_init);

	if (d_results) delete[] d_results;
	if (d_errors) delete[] d_errors;
	gsl_matrix_free (covar);
}

/*****************************************************************************
 *
 * Class ExponentialFit
 *
 *****************************************************************************/

	ExponentialFit::ExponentialFit(ApplicationWindow *parent, Graph *g, bool expGrowth)
: Fit(parent, g),
	is_exp_growth(expGrowth)
{
	init();
}

ExponentialFit::ExponentialFit(ApplicationWindow *parent, Graph *g, 
		const QString& curveTitle, bool expGrowth)
: Fit(parent, g),
	is_exp_growth(expGrowth)
{
	init();
	setDataFromCurve(curveTitle);
}

ExponentialFit::ExponentialFit(ApplicationWindow *parent, Graph *g,
		const QString& curveTitle, int start, int end, bool expGrowth)
: Fit(parent, g),
	is_exp_growth(expGrowth)
{
	init();
	setDataFromCurve(curveTitle, start, end);
}

void ExponentialFit::init()
{
	d_f = exp_f;
	d_df = exp_df;
	d_fdf = exp_fdf;
	d_fsimplex = exp_d;
	d_p = 3;
	d_param_init = gsl_vector_alloc(d_p);
	gsl_vector_set_all (d_param_init, 1.0);

	covar = gsl_matrix_alloc (d_p, d_p);
	d_results = new double[d_p];
	d_param_names << "A" << "t" << "y0";

	if (is_exp_growth)
	{
		setName("ExpGrowth");
		d_fit_type = tr("Exponential growth");
		d_formula = "y0 + Aexp(x/t)";
		d_param_explain << "(amplitude)" << "(lifetime)" << "(offset)";
	}
	else
	{
		setName("ExpDecay");
		d_fit_type = tr("Exponential decay");
		d_formula = "y0 + A*exp(-x/t)";
		d_param_explain << "(amplitude)" << "(e-folding time)" << "(offset)";
	}
}

void ExponentialFit::storeCustomFitResults(double *par)
{
	for (int i=0; i<d_p; i++)
		d_results[i] = par[i];

	if (is_exp_growth)
		d_results[1]=-1.0/d_results[1];
	else
		d_results[1]=1.0/d_results[1];
}

void ExponentialFit::calculateFitCurveData(double *par, double *X, double *Y)
{
	if (d_gen_function)
	{
		double X0 = d_x[0];
		double step = (d_x[d_n-1]-X0)/(d_result_points-1);
		for (int i=0; i<d_result_points; i++)
		{
			X[i] = X0+i*step;
			Y[i] = par[0]*exp(-par[1]*X[i])+par[2];
		}
	}
	else
	{
		for (int i=0; i<d_result_points; i++)
		{
			X[i] = d_x[i];
			Y[i] = par[0]*exp(-par[1]*X[i])+par[2];
		}
	}
	delete[] par;
}

/*****************************************************************************
 *
 * Class TwoExpFit
 *
 *****************************************************************************/

	TwoExpFit::TwoExpFit(ApplicationWindow *parent, Graph *g)
: Fit(parent, g)
{
	init();
}

	TwoExpFit::TwoExpFit(ApplicationWindow *parent, Graph *g, const QString& curveTitle)
: Fit(parent, g)
{
	init();
	setDataFromCurve(curveTitle);
}

	TwoExpFit::TwoExpFit(ApplicationWindow *parent, Graph *g, const QString& curveTitle, int start, int end)
: Fit(parent, g)
{
	init();
	setDataFromCurve(curveTitle, start, end);
}

void TwoExpFit::init()
{
	setName("ExpDecay");
	d_f = expd2_f;
	d_df = expd2_df;
	d_fdf = expd2_fdf;
	d_fsimplex = expd2_d;
	d_p = 5;
	d_param_init = gsl_vector_alloc(d_p);
	gsl_vector_set_all (d_param_init, 1.0);
	covar = gsl_matrix_alloc (d_p, d_p);
	d_results = new double[d_p];
	d_param_names << "A1" << "t1" << "A2" << "t2" << "y0";
	d_fit_type = tr("Exponential decay");
	d_formula = "A1*exp(-x/t1)+A2*exp(-x/t2)+y0";
	d_param_explain << "(first amplitude)" << "(first lifetime)" << "(second amplitude)" << "(second lifetime)" << "(offset)";
}

void TwoExpFit::storeCustomFitResults(double *par)
{
	for (int i=0; i<d_p; i++)
		d_results[i] = par[i];

	d_results[1]=1.0/d_results[1];
	d_results[3]=1.0/d_results[3];
}

void TwoExpFit::calculateFitCurveData(double *par, double *X, double *Y)
{
	if (d_gen_function)
	{
		double X0 = d_x[0];
		double step = (d_x[d_n-1]-X0)/(d_result_points-1);
		for (int i=0; i<d_result_points; i++)
		{
			X[i] = X0+i*step;
			Y[i] = par[0]*exp(-par[1]*X[i])+par[2]*exp(-par[3]*X[i])+par[4];
		}
	}
	else
	{
		for (int i=0; i<d_result_points; i++)
		{
			X[i] = d_x[i];
			Y[i] = par[0]*exp(-par[1]*X[i])+par[2]*exp(-par[3]*X[i])+par[4];
		}
	}
	delete[] par;
}

/*****************************************************************************
 *
 * Class ThreeExpFit
 *
 *****************************************************************************/

	ThreeExpFit::ThreeExpFit(ApplicationWindow *parent, Graph *g)
: Fit(parent, g)
{
	init();
}

	ThreeExpFit::ThreeExpFit(ApplicationWindow *parent, Graph *g, const QString& curveTitle)
: Fit(parent, g)
{
	init();
	setDataFromCurve(curveTitle);
}

	ThreeExpFit::ThreeExpFit(ApplicationWindow *parent, Graph *g, const QString& curveTitle, int start, int end)
: Fit(parent, g)
{
	init();
	setDataFromCurve(curveTitle, start, end);
}

void ThreeExpFit::init()
{
	setName("ExpDecay");
	d_f = expd3_f;
	d_df = expd3_df;
	d_fdf = expd3_fdf;
	d_fsimplex = expd3_d;
	d_p = 7;
	d_param_init = gsl_vector_alloc(d_p);
	gsl_vector_set_all (d_param_init, 1.0);
	covar = gsl_matrix_alloc (d_p, d_p);
	d_results = new double[d_p];
	d_param_names << "A1" << "t1" << "A2" << "t2" << "A3" << "t3" << "y0";
	d_fit_type = tr("Exponential decay");
	d_formula = "A1*exp(-x/t1)+A2*exp(-x/t2)+A3*exp(-x/t3)+y0";
	d_param_explain << "(first amplitude)" << "(first lifetime)" << "(second amplitude)" << "(second lifetime)" << "(third amplitude)" << "(third lifetime)" << "(offset)";
}

void ThreeExpFit::storeCustomFitResults(double *par)
{
	for (int i=0; i<d_p; i++)
		d_results[i] = par[i];

	d_results[1]=1.0/d_results[1];
	d_results[3]=1.0/d_results[3];
	d_results[5]=1.0/d_results[5];
}

void ThreeExpFit::calculateFitCurveData(double *par, double *X, double *Y)
{
	if (d_gen_function)
	{
		double X0 = d_x[0];
		double step = (d_x[d_n-1]-X0)/(d_result_points-1);
		for (int i=0; i<d_result_points; i++)
		{
			X[i]=X0+i*step;
			Y[i]=par[0]*exp(-X[i]*par[1])+par[2]*exp(-X[i]*par[3])+par[4]*exp(-X[i]*par[5])+par[6];
		}
	}
	else
	{
		for (int i=0; i<d_result_points; i++)
		{
			X[i]=d_x[i];
			Y[i]=par[0]*exp(-X[i]*par[1])+par[2]*exp(-X[i]*par[3])+par[4]*exp(-X[i]*par[5])+par[6];
		}
	}
	delete[] par;
}

/*****************************************************************************
 *
 * Class SigmoidalFit
 *
 *****************************************************************************/

	SigmoidalFit::SigmoidalFit(ApplicationWindow *parent, Graph *g)
: Fit(parent, g)
{
	init();
}

	SigmoidalFit::SigmoidalFit(ApplicationWindow *parent, Graph *g, const QString& curveTitle)
: Fit(parent, g)
{
	init();
	setDataFromCurve(curveTitle);
}

	SigmoidalFit::SigmoidalFit(ApplicationWindow *parent, Graph *g, const QString& curveTitle, int start, int end)
: Fit(parent, g)
{
	init();
	setDataFromCurve(curveTitle, start, end);
}

void SigmoidalFit::init()
{
	setName("Boltzmann");
	d_f = boltzmann_f;
	d_df = boltzmann_df;
	d_fdf = boltzmann_fdf;
	d_fsimplex = boltzmann_d;
	d_p = 4;
	d_param_init = gsl_vector_alloc(d_p);
	gsl_vector_set_all (d_param_init, 1.0);
	covar = gsl_matrix_alloc (d_p, d_p);
	d_results = new double[d_p];
	d_param_explain << tr("(init value)") << tr("(final value)") << tr("(center)") << tr("(time constant)");
	d_param_names << "A1" << "A2" << "x0" << "dx";
	d_fit_type = tr("Boltzmann (Sigmoidal)");
	d_formula = "(A1-A2)/(1+exp((x-x0)/dx))+A2";
}

void SigmoidalFit::calculateFitCurveData(double *par, double *X, double *Y)
{
	if (d_gen_function)
	{
		double X0 = d_x[0];
		double step = (d_x[d_n-1]-X0)/(d_result_points-1);
		for (int i=0; i<d_result_points; i++)
		{
			X[i] = X0+i*step;
			Y[i] = (par[0]-par[1])/(1+exp((X[i]-par[2])/par[3]))+par[1];
		}
	}
	else
	{
		for (int i=0; i<d_result_points; i++)
		{
			X[i] = d_x[i];
			Y[i] = (par[0]-par[1])/(1+exp((X[i]-par[2])/par[3]))+par[1];
		}
	}
	delete[] par;
}

void SigmoidalFit::guessInitialValues()
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
 * Class GaussAmpFit
 *
 *****************************************************************************/

	GaussAmpFit::GaussAmpFit(ApplicationWindow *parent, Graph *g)
: Fit(parent, g)
{
	init();
}

	GaussAmpFit::GaussAmpFit(ApplicationWindow *parent, Graph *g, const QString& curveTitle)
: Fit(parent, g)
{
	init();
	setDataFromCurve(curveTitle);
}

	GaussAmpFit::GaussAmpFit(ApplicationWindow *parent, Graph *g, const QString& curveTitle, int start, int end)
: Fit(parent, g)
{
	init();
	setDataFromCurve(curveTitle, start, end);
}

void GaussAmpFit::init()
{
	setName("GaussAmp");
	d_f = gauss_f;
	d_df = gauss_df;
	d_fdf = gauss_fdf;
	d_fsimplex = gauss_d;
	d_p = 4;
	d_param_init = gsl_vector_alloc(d_p);
	gsl_vector_set_all (d_param_init, 1.0);
	covar = gsl_matrix_alloc (d_p, d_p);
	d_results = new double[d_p];
	d_param_explain << tr("(offset)") << tr("(height)") << tr("(center)") << tr("(width)");
	d_param_names << "y0" << "A" << "xc" << "w";
	d_fit_type = tr("GaussAmp");
	d_formula = "y0 + A*exp(-(x-xc)^2/(2*w^2))";
}

void GaussAmpFit::calculateFitCurveData(double *par, double *X, double *Y)
{
	double w2 = par[3]*par[3];
	if (d_gen_function)
	{
		double X0 = d_x[0];
		double step = (d_x[d_n-1]-X0)/(d_result_points-1);
		for (int i=0; i<d_result_points; i++)
		{
			X[i] = X0+i*step;
			double diff = X[i]-par[2];
			Y[i] = par[1]*exp(-0.5*diff*diff/w2)+par[0];
		}
	}
	else
	{
		for (int i=0; i<d_result_points; i++)
		{
			X[i] = d_x[i];		
			double diff = X[i]-par[2];
			Y[i] = par[1]*exp(-0.5*diff*diff/w2)+par[0];
		}
	}
	delete[] par;
}

/*****************************************************************************
 *
 * Class NonLinearFit
 *
 *****************************************************************************/

	NonLinearFit::NonLinearFit(ApplicationWindow *parent, Graph *g)
: Fit(parent, g)
{
	init();
}

	NonLinearFit::NonLinearFit(ApplicationWindow *parent, Graph *g, const QString& curveTitle)
: Fit(parent, g)
{
	init();
	setDataFromCurve(curveTitle);
}

	NonLinearFit::NonLinearFit(ApplicationWindow *parent, Graph *g, const QString& curveTitle, int start, int end)
: Fit(parent, g)
{
	init();
	setDataFromCurve(curveTitle, start, end);
}

void NonLinearFit::init()
{
	setName(tr("NonLinear"));
	d_formula = QString::null;
	d_f = user_f;
	d_df = user_df;
	d_fdf = user_fdf;
	d_fsimplex = user_d;
	d_fit_type = tr("Non-linear");
}

void NonLinearFit::setFormula(const QString& s)
{
	if (s.isEmpty()) 
	{
		QMessageBox::critical((ApplicationWindow *)parent(),  tr("QtiPlot - Input function error"), 
				tr("Please enter a valid non-empty expression! Operation aborted!"));
		d_init_err = true;
		return;
	}

	if (d_p < 2)
	{
		QMessageBox::critical((ApplicationWindow *)parent(), tr("QtiPlot - Fit Error"),
				tr("There are no parameters specified for this fit operation. Please define a list of parameters first!"));
		d_init_err = true;
		return;
	}

	if (d_formula == s) 
		return;

	try
	{
		double *param = new double[d_p];
		MyParser parser;
		double xvar; 
		parser.DefineVar("x", &xvar);
		for (int k=0; k<(int)d_p; k++)
		{
			param[k]=gsl_vector_get(d_param_init, k);
			parser.DefineVar(d_param_names[k].ascii(), &param[k]);
		}
		parser.SetExpr(s.ascii());
		parser.Eval() ;
		delete[] param;
	}
	catch(mu::ParserError &e)
	{
		QMessageBox::critical((ApplicationWindow *)parent(),  tr("QtiPlot - Input function error"), QString::fromStdString(e.GetMsg()));
		d_init_err = true;
		return;
	}

	d_init_err = false;	
	d_formula = s;
}

void NonLinearFit::setParametersList(const QStringList& lst)
{	
	if ((int)lst.count() < 2)
	{
		QMessageBox::critical((ApplicationWindow *)parent(), tr("QtiPlot - Fit Error"),
				tr("You must provide a list containing at least 2 parameters for this type of fit. Operation aborted!"));
		d_init_err = true;
		return;
	}

	d_init_err = false;	
	d_param_names = lst;

	if (d_p > 0)
	{//free previously allocated memory
		gsl_vector_free(d_param_init);
		gsl_matrix_free (covar);
		delete[] d_results;
	}

	d_p = (int)lst.count();
	d_param_init = gsl_vector_alloc(d_p);
	gsl_vector_set_all (d_param_init, 1.0);

	covar = gsl_matrix_alloc (d_p, d_p);
	d_results = new double[d_p];

	for (int i=0; i<d_p; i++)
		d_param_explain << "";
}

void NonLinearFit::calculateFitCurveData(double *par, double *X, double *Y)
{
	MyParser parser;
	for (int i=0; i<d_p; i++)
		parser.DefineVar(d_param_names[i].ascii(), &par[i]);

	double xvar;
	parser.DefineVar("x", &xvar);
	parser.SetExpr(d_formula.ascii());

	if (d_gen_function)
	{
		double X0 = d_x[0];
		double step = (d_x[d_n-1]-X0)/(d_result_points-1);
		for (int i=0; i<d_result_points; i++)
		{
			X[i] = X0+i*step;
			xvar = X[i];
			Y[i] = parser.Eval();
		}
	}
	else
	{
		for (int i=0; i<d_result_points; i++)
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
 * Class PluginFit
 *
 *****************************************************************************/

	PluginFit::PluginFit(ApplicationWindow *parent, Graph *g)
: Fit(parent, g)
{
	init();
}

	PluginFit::PluginFit(ApplicationWindow *parent, Graph *g, const QString& curveTitle)
: Fit(parent, g)
{
	init();
	setDataFromCurve(curveTitle);
}

	PluginFit::PluginFit(ApplicationWindow *parent, Graph *g, const QString& curveTitle, int start, int end)
: Fit(parent, g)
{
	init();
	setDataFromCurve(curveTitle, start, end);
}

void PluginFit::init()
{
	d_fit_type = tr("Plugin");
}

bool PluginFit::load(const QString& pluginName)
{
	if (!QFile::exists (pluginName))
	{
		QMessageBox::critical((ApplicationWindow *)parent(), tr("QtiPlot - File not found"),
				tr("Plugin file: <p><b> %1 </b> <p>not found. Operation aborted!").arg(pluginName));
		return false;
	}

	setName(pluginName);
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
				tr("The plugin does not implement a %1 method necessary for Levenberg-Marquardt fitting.").arg("function_df"));
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

	fitFunc fitExplain = (fitFunc) lib.resolve("explanations");
	if (fitExplain)
		d_param_explain = QStringList::split(",", QString(fitExplain()), false);
	else
		for (int i=0; i<d_p; i++)
			d_param_explain << "";


	fitFunction = (fitFunc) lib.resolve( "name" );
	setName(QString(fitFunction()));

	fitFunction = (fitFunc) lib.resolve( "function" );
	if (fitFunction)
		d_formula = QString(fitFunction());
	else
		return false;

	return true;
}

void PluginFit::calculateFitCurveData(double *par, double *X, double *Y)
{
	if (d_gen_function)
	{
		double X0 = d_x[0];
		double step = (d_x[d_n-1]-X0)/(d_result_points-1);
		for (int i=0; i<d_result_points; i++)
		{
			X[i] = X0+i*step;
			Y[i]= f_eval(X[i], par);
		}
	}
	else
	{
		for (int i=0; i<d_result_points; i++)
		{
			X[i] = d_x[i];		
			Y[i]= f_eval(X[i], par);
		}
	}
	delete[] par;
}

/*****************************************************************************
 *
 * Class MultiPeakFit
 *
 *****************************************************************************/

	MultiPeakFit::MultiPeakFit(ApplicationWindow *parent, Graph *g, PeakProfile profile, int peaks)
: Fit(parent, g),
	d_profile(profile),
	d_peaks(peaks)
{
	setName(tr("MultiPeak"));

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

	if (d_peaks > 1)
		d_fit_type += "(" + QString::number(d_peaks) +") " + tr("multi-peak");

	d_p = 3*d_peaks + 1;
	d_param_init = gsl_vector_alloc(d_p);
	gsl_vector_set_all (d_param_init, 1.0);

	covar = gsl_matrix_alloc (d_p, d_p);
	d_results = new double[d_p];

	d_param_names = generateParameterList(d_peaks);
	d_formula = generateFormula(d_peaks, d_profile);
	d_param_explain = generateExplanationList(d_peaks);

	generate_peak_curves = true;
	d_peaks_color = 2;//green
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
				return "y0 + A*sqrt(2/PI)/w*exp(-2*((x-xc)/w)^2)";
				break;

			case Lorentz:
				return "y0 + 2*A/PI*w/(4*(x-xc)^2+w^2)";
				break;
		}

	QString formula = "y0 + ";
	for (int i = 0; i<peaks; i++)
	{
		formula += peakFormula(i+1, profile);
		if (i < peaks - 1)
			formula += " + ";
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
	if (d_peaks > 1)
		return;

	gsl_vector_view x = gsl_vector_view_array (d_x, d_n);
	gsl_vector_view y = gsl_vector_view_array (d_y, d_n);

	double min_out, max_out;
	gsl_vector_minmax (&y.vector, &min_out, &max_out);

	if (d_profile == Gauss)
		gsl_vector_set(d_param_init, 0, sqrt(M_2_PI)*(max_out - min_out));
	else if (d_profile == Lorentz)
		gsl_vector_set(d_param_init, 0, 1.0);

	gsl_vector_set(d_param_init, 1, gsl_vector_get(&x.vector, gsl_vector_max_index (&y.vector)));
	gsl_vector_set(d_param_init, 2, 1.0);
	gsl_vector_set(d_param_init, 3, min_out);
}

void MultiPeakFit::storeCustomFitResults(double *par)
{
	for (int i=0; i<d_p-1; i++)
		d_results[i] = fabs(par[i]);

	d_results[d_p-1] = par[d_p-1];//the offset may be negatif

	if (d_profile == Lorentz)
	{
		for (int j=0; j<d_peaks; j++)
			d_results[3*j] = M_PI_2*d_results[3*j];
	}
}

void MultiPeakFit::insertPeakFunctionCurve(double *x, double *y, int peak)
{
	QStringList curves = d_graph->curvesList();
	int index = 0;
	for (int i = 0; i<(int)curves.count(); i++ )
	{
		if (curves[i].startsWith(tr("Peak")))
			index++;
	}
	QString title = tr("Peak") + QString::number(++index);

	FunctionCurve *c = new FunctionCurve(FunctionCurve::Normal, title);
	c->setPen(QPen(ColorBox::color(d_peaks_color), 1)); 
	c->setData(x, y, d_result_points);
	c->setRange(d_x[0], d_x[d_n-1]);

	QString formula = "y0+"+peakFormula(peak + 1, d_profile);
	QString parameter = QString::number(d_results[d_p-1], 'g', d_prec);
	formula.replace(d_param_names[d_p-1], parameter);
	for (int j=0; j<3; j++)
	{
		int p = 3*peak + j;
		parameter = QString::number(d_results[p], 'g', d_prec);
		formula.replace(d_param_names[p], parameter);
	}
	c->setFormula(formula);
	d_graph->insertCurve(c, title);
}

void MultiPeakFit::generateFitCurve(double *par)
{
	ApplicationWindow *app = (ApplicationWindow *)parent();
	if (!d_gen_function)
		d_result_points = d_n;

	gsl_matrix * m = gsl_matrix_alloc (d_result_points, d_peaks);
	if (!m)
	{
		QMessageBox::warning(app, tr("QtiPlot - Fit Error"), tr("Could not allocate enough memory for the fit curves!"));
		return;
	}

	double *X = new double[d_result_points]; 
	double *Y = new double[d_result_points]; 
	int i, j;
	int peaks_aux = d_peaks;
	if (d_peaks == 1)
		peaks_aux--;

	if (d_gen_function)
	{
		double step = (d_x[d_n-1] - d_x[0])/(d_result_points-1);	
		for (i = 0; i<d_result_points; i++)
		{
			X[i] = d_x[0] + i*step;
			double yi=0;
			for (j=0; j<d_peaks; j++)
			{
				double diff = X[i] - par[3*j + 1];
				double w = par[3*j + 2];
				double y_aux = 0;
				if (d_profile == Gauss)
					y_aux += sqrt(M_2_PI)*par[3*j]/w*exp(-2*diff*diff/(w*w));
				else
					y_aux += par[3*j]*w/(4*diff*diff+w*w);

				yi += y_aux;
				y_aux += par[d_p - 1];
				gsl_matrix_set(m, i, j, y_aux);
			}
			Y[i] = yi + par[d_p - 1];//add offset
		}

		if (d_peaks > 1)
			insertFitFunctionCurve(QString(name()) + tr("Fit"), X, Y, 2);
		else
			insertFitFunctionCurve(QString(name()) + tr("Fit"), X, Y);

		if (generate_peak_curves)
		{
			for (i=0; i<peaks_aux; i++)
			{//add the peak curves
				for (j=0; j<d_result_points; j++)
					Y[j] = gsl_matrix_get (m, j, i);

				insertPeakFunctionCurve(X, Y, i);
			}
		}
	}
	else
	{
		QString tableName = app->generateUnusedName(tr("Fit"));
		QString label = d_fit_type + " " + tr("fit of") + " " + d_curve->title().text();

		Table *t= app->newHiddenTable(tableName, label, d_result_points, peaks_aux + 2);
		QStringList header = QStringList() << "1";
		for (i = 0; i<peaks_aux; i++)
			header << tr("peak") + QString::number(i+1);
		header << "2";
		t->setHeader(header);

		for (i = 0; i<d_result_points; i++)
		{
			X[i] = d_x[i];
			t->setText(i, 0, QString::number(X[i], 'g', d_prec));

			double yi=0;
			for (j=0; j<d_peaks; j++)
			{
				double diff = X[i] - par[3*j + 1];
				double w = par[3*j + 2];
				double y_aux = 0;
				if (d_profile == Gauss)
					y_aux += sqrt(M_2_PI)*par[3*j]/w*exp(-2*diff*diff/(w*w));
				else
					y_aux += par[3*j]*w/(4*diff*diff+w*w);

				yi += y_aux;
				y_aux += par[d_p - 1];
				t->setText(i, j+1, QString::number(y_aux, 'g', d_prec));
				gsl_matrix_set(m, i, j, y_aux);
			}
			Y[i] = yi + par[d_p - 1];//add offset
			if (d_peaks > 1)
				t->setText(i, d_peaks+1, QString::number(Y[i], 'g', d_prec));
		}

		label = tableName + "_" + "2";
		QwtPlotCurve *c = new QwtPlotCurve(label);
		if (d_peaks > 1)
			c->setPen(QPen(ColorBox::color(d_curveColorIndex), 2)); 
		else
			c->setPen(QPen(ColorBox::color(d_curveColorIndex), 1)); 
		c->setData(X, Y, d_result_points);	
		d_graph->insertCurve(c, tableName+"_1(X),"+label+"(Y)");

		if (generate_peak_curves)
		{
			for (i=0; i<peaks_aux; i++)
			{//add the peak curves
				for (j=0; j<d_result_points; j++)
					Y[j] = gsl_matrix_get (m, j, i);

				label = tableName + "_" + tr("peak") + QString::number(i+1);
				c = new QwtPlotCurve(label);
				c->setPen(QPen(ColorBox::color(d_peaks_color), 1)); 
				c->setData(X, Y, d_result_points);	
				d_graph->insertCurve(c, tableName+"_1(X),"+label+"(Y)");
			}
		}
	}
	d_graph->replot();

	delete[] par;
	delete[] X;
	delete[] Y;
	gsl_matrix_free(m);
}

QString MultiPeakFit::logFitInfo(double *par, int iterations, int status, const QString& plotName)
{
	QString info = Fit::logFitInfo(par, iterations, status, plotName);
	if (d_peaks == 1)
		return info;

	info += tr("Peak") + "\t" + tr("Area") + "\t";
	info += tr("Center") + "\t" + tr("Width") + "\t" + tr("Height") + "\n";
	info += "---------------------------------------------------------------------------------------\n";
	for (int j=0; j<d_peaks; j++)
	{
		info += QString::number(j+1)+"\t";
		info += QString::number(par[3*j],'g', d_prec)+"\t";
		info += QString::number(par[3*j+1],'g', d_prec)+"\t";
		info += QString::number(par[3*j+2],'g', d_prec)+"\t";

		if (d_profile == Lorentz)
			info += QString::number(M_2_PI*par[3*j]/par[3*j+2],'g', d_prec)+"\n";
		else
			info += QString::number(sqrt(M_2_PI)*par[3*j]/par[3*j+2],'g', d_prec)+"\n";
	}
	info += "---------------------------------------------------------------------------------------\n";
	return info;
}

/*****************************************************************************
 *
 * Class LorentzFit
 *
 *****************************************************************************/

	LorentzFit::LorentzFit(ApplicationWindow *parent, Graph *g)
: MultiPeakFit(parent, g, MultiPeakFit::Lorentz, 1)
{
	init();
}

	LorentzFit::LorentzFit(ApplicationWindow *parent, Graph *g, const QString& curveTitle)
: MultiPeakFit(parent, g, MultiPeakFit::Lorentz, 1)
{
	init();
	setDataFromCurve(curveTitle);
}

	LorentzFit::LorentzFit(ApplicationWindow *parent, Graph *g, const QString& curveTitle, int start, int end)
: MultiPeakFit(parent, g, MultiPeakFit::Lorentz, 1)
{
	init();
	setDataFromCurve(curveTitle, start, end);
}

void LorentzFit::init()
{
	setName("Lorentz");
	d_fit_type = tr("Lorentz");
	d_param_explain << tr("(area)") << tr("(center)") << tr("(width)") << tr("(offset)");
}

/*****************************************************************************
 *
 * Class GaussFit
 *
 *****************************************************************************/

	GaussFit::GaussFit(ApplicationWindow *parent, Graph *g)
: MultiPeakFit(parent, g, MultiPeakFit::Gauss, 1)
{
	setName("Gauss");
	d_fit_type = tr("Gauss");
	d_param_explain << tr("(area)") << tr("(center)") << tr("(width)") << tr("(offset)");
}

	GaussFit::GaussFit(ApplicationWindow *parent, Graph *g, const QString& curveTitle)
: MultiPeakFit(parent, g, MultiPeakFit::Gauss, 1)
{
	init();
	setDataFromCurve(curveTitle);
}

	GaussFit::GaussFit(ApplicationWindow *parent, Graph *g, const QString& curveTitle, int start, int end)
: MultiPeakFit(parent, g, MultiPeakFit::Gauss, 1)
{
	init();
	setDataFromCurve(curveTitle, start, end);
}

void GaussFit::init()
{
	setName("Gauss");
	d_fit_type = tr("Gauss");
	d_param_explain << tr("(area)") << tr("(center)") << tr("(width)") << tr("(offset)");
}

/*****************************************************************************
 *
 * Class PolynomialFit
 *
 *****************************************************************************/

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

	PolynomialFit::PolynomialFit(ApplicationWindow *parent, Graph *g, QString& curveTitle, int start, int end, int order, bool legend)
: Fit(parent, g), d_order(order), show_legend(legend)
{
	init();
	setDataFromCurve(curveTitle, start, end);
}

void PolynomialFit::init()
{
	setName(tr("Poly"));
	is_non_linear = false;
	d_fit_type = tr("Polynomial");
	d_p = d_order + 1;

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
			formula += " + ";
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
		double step = (d_x[d_n-1]-X0)/(d_result_points-1);
		for (int i=0; i<d_result_points; i++)
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
		for (int i=0; i<d_result_points; i++)
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
	gsl_matrix *X = gsl_matrix_alloc (d_n, d_p);
	gsl_vector *c = gsl_vector_alloc (d_p);

	int i;
	for (i = 0; i <d_n; i++)
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

	for (i = 0; i < d_p; i++)
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

QString PolynomialFit::legendFitInfo()
{		
	QString legend = "Y=" + QString::number(d_results[0], 'g', d_prec);		
	for (int j = 1; j < d_p; j++)
	{
		double cj = d_results[j];
		if (cj>0 && !legend.isEmpty())
			legend += "+";

		QString s;
		s.sprintf("%.5f",cj);	
		if (s != "1.00000")
			legend += QString::number(cj, 'g', d_prec);

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

	LinearFit::LinearFit(ApplicationWindow *parent, Graph *g, const QString& curveTitle, int start, int end)
: Fit(parent, g)
{
	init();
	setDataFromCurve(curveTitle, start, end);
}

void LinearFit::init()
{
	d_p = 2;
	covar = gsl_matrix_alloc (d_p, d_p);
	d_results = new double[d_p];

	is_non_linear = false;
	d_formula = "A*x + B";
	d_param_names << "B" << "A";
	d_param_explain << "(y-intercept)" << "(slope)";
	d_fit_type = tr("Linear Regression");
	setName(tr("Linear"));
}

void LinearFit::fit()
{
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
		double step = (d_x[d_n-1]-X0)/(d_result_points-1);
		for (int i=0; i<d_result_points; i++)
		{
			X[i] = X0+i*step;
			Y[i] = par[0]+par[1]*X[i];
		}
	}
	else
	{
		for (int i=0; i<d_result_points; i++)
		{
			X[i] = d_x[i];		
			Y[i] = par[0]+par[1]*X[i];
		}
	}
}
