/***************************************************************************
    File                 : fitclasses.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : Multiple classes derived from Fit
                           
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
#include "fitclasses.h"
#include "fit_gsl.h"

#include "MyParser.h"
#include "FunctionCurve.h"
#include "ColorBox.h"

#include <gsl/gsl_multifit.h>
#include <gsl/gsl_fit.h>

#include <QLibrary>
#include <QMessageBox>
#include <QDateTime>

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

ExponentialFit::ExponentialFit(ApplicationWindow *parent, Graph *g, const QString& curveTitle, bool expGrowth)
: Fit(parent, g),
  is_exp_growth(expGrowth)
{
	init();
	setDataFromCurve(curveTitle);
}

ExponentialFit::ExponentialFit(ApplicationWindow *parent, Graph *g,
		const QString& curveTitle, double start, double end, bool expGrowth)
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
    d_min_points = d_p;
	d_param_init = gsl_vector_alloc(d_p);
	gsl_vector_set_all (d_param_init, 1.0);

	covar = gsl_matrix_alloc (d_p, d_p);
	d_results = new double[d_p];
	d_param_names << "A" << "t" << "y0";

	if (is_exp_growth)
	{
		setName("ExpGrowth");
		d_explanation = tr("Exponential growth");
		d_formula = "y0 + Aexp(x/t)";
		d_param_explain << "(amplitude)" << "(lifetime)" << "(offset)";
	}
	else
	{
		setName("ExpDecay");
		d_explanation = tr("Exponential decay");
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
		double step = (d_x[d_n-1]-X0)/(d_points-1);
		for (int i=0; i<d_points; i++)
		{
			X[i] = X0+i*step;
			Y[i] = par[0]*exp(-par[1]*X[i])+par[2];
		}
	}
	else
	{
		for (int i=0; i<d_points; i++)
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

	TwoExpFit::TwoExpFit(ApplicationWindow *parent, Graph *g, const QString& curveTitle, double start, double end)
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
    d_min_points = d_p;
	d_param_init = gsl_vector_alloc(d_p);
	gsl_vector_set_all (d_param_init, 1.0);
	covar = gsl_matrix_alloc (d_p, d_p);
	d_results = new double[d_p];
	d_param_names << "A1" << "t1" << "A2" << "t2" << "y0";
	d_explanation = tr("Exponential decay");
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
		double step = (d_x[d_n-1]-X0)/(d_points-1);
		for (int i=0; i<d_points; i++)
		{
			X[i] = X0+i*step;
			Y[i] = par[0]*exp(-par[1]*X[i])+par[2]*exp(-par[3]*X[i])+par[4];
		}
	}
	else
	{
		for (int i=0; i<d_points; i++)
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

	ThreeExpFit::ThreeExpFit(ApplicationWindow *parent, Graph *g, const QString& curveTitle, double start, double end)
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
    d_min_points = d_p;
	d_param_init = gsl_vector_alloc(d_p);
	gsl_vector_set_all (d_param_init, 1.0);
	covar = gsl_matrix_alloc (d_p, d_p);
	d_results = new double[d_p];
	d_param_names << "A1" << "t1" << "A2" << "t2" << "A3" << "t3" << "y0";
	d_explanation = tr("Exponential decay");
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
		double step = (d_x[d_n-1]-X0)/(d_points-1);
		for (int i=0; i<d_points; i++)
		{
			X[i]=X0+i*step;
			Y[i]=par[0]*exp(-X[i]*par[1])+par[2]*exp(-X[i]*par[3])+par[4]*exp(-X[i]*par[5])+par[6];
		}
	}
	else
	{
		for (int i=0; i<d_points; i++)
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

	SigmoidalFit::SigmoidalFit(ApplicationWindow *parent, Graph *g, const QString& curveTitle, double start, double end)
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
    d_min_points = d_p;
	d_param_init = gsl_vector_alloc(d_p);
	gsl_vector_set_all (d_param_init, 1.0);
	covar = gsl_matrix_alloc (d_p, d_p);
	d_results = new double[d_p];
	d_param_explain << tr("(init value)") << tr("(final value)") << tr("(center)") << tr("(time constant)");
	d_param_names << "A1" << "A2" << "x0" << "dx";
	d_explanation = tr("Boltzmann (Sigmoidal) Fit");
	d_formula = "(A1-A2)/(1+exp((x-x0)/dx))+A2";
}

void SigmoidalFit::calculateFitCurveData(double *par, double *X, double *Y)
{
	if (d_gen_function)
	{
		double X0 = d_x[0];
		double step = (d_x[d_n-1]-X0)/(d_points-1);
		for (int i=0; i<d_points; i++)
		{
			X[i] = X0+i*step;
			Y[i] = (par[0]-par[1])/(1+exp((X[i]-par[2])/par[3]))+par[1];
		}
	}
	else
	{
		for (int i=0; i<d_points; i++)
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

	GaussAmpFit::GaussAmpFit(ApplicationWindow *parent, Graph *g, const QString& curveTitle, double start, double end)
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
    d_min_points = d_p;
	d_param_init = gsl_vector_alloc(d_p);
	gsl_vector_set_all (d_param_init, 1.0);
	covar = gsl_matrix_alloc (d_p, d_p);
	d_results = new double[d_p];
	d_param_explain << tr("(offset)") << tr("(height)") << tr("(center)") << tr("(width)");
	d_param_names << "y0" << "A" << "xc" << "w";
	d_explanation = tr("GaussAmp Fit");
	d_formula = "y0 + A*exp(-(x-xc)^2/(2*w^2))";
}

void GaussAmpFit::calculateFitCurveData(double *par, double *X, double *Y)
{
	double w2 = par[3]*par[3];
	if (d_gen_function)
	{
		double X0 = d_x[0];
		double step = (d_x[d_n-1]-X0)/(d_points-1);
		for (int i=0; i<d_points; i++)
		{
			X[i] = X0+i*step;
			double diff = X[i]-par[2];
			Y[i] = par[1]*exp(-0.5*diff*diff/w2)+par[0];
		}
	}
	else
	{
		for (int i=0; i<d_points; i++)
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

	NonLinearFit::NonLinearFit(ApplicationWindow *parent, Graph *g, const QString& curveTitle, double start, double end)
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
	d_explanation = tr("Non-linear Fit");
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

	if (!d_p)
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
	if ((int)lst.count() < 1)
	{
		QMessageBox::critical((ApplicationWindow *)parent(), tr("QtiPlot - Fit Error"),
				tr("You must provide a list containing at least one parameter for this type of fit. Operation aborted!"));
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
    d_min_points = d_p;
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
		double step = (d_x[d_n-1]-X0)/(d_points-1);
		for (int i=0; i<d_points; i++)
		{
			X[i] = X0+i*step;
			xvar = X[i];
			Y[i] = parser.Eval();
		}
	}
	else
	{
		for (int i=0; i<d_points; i++)
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

	PluginFit::PluginFit(ApplicationWindow *parent, Graph *g, const QString& curveTitle, double start, double end)
: Fit(parent, g)
{
	init();
	setDataFromCurve(curveTitle, start, end);
}

void PluginFit::init()
{
	d_explanation = tr("Plugin Fit");
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
		d_param_names = QString(fitFunction()).split(",", QString::SkipEmptyParts);
		d_p = (int)d_param_names.count();
        d_min_points = d_p;
		d_param_init = gsl_vector_alloc(d_p);
		covar = gsl_matrix_alloc (d_p, d_p);
		d_results = new double[d_p];
	}
	else
		return false;

	fitFunc fitExplain = (fitFunc) lib.resolve("explanations");
	if (fitExplain)
		d_param_explain = QString(fitExplain()).split(",", QString::SkipEmptyParts);
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
		double step = (d_x[d_n-1]-X0)/(d_points-1);
		for (int i=0; i<d_points; i++)
		{
			X[i] = X0+i*step;
			Y[i]= f_eval(X[i], par);
		}
	}
	else
	{
		for (int i=0; i<d_points; i++)
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
	d_profile(profile)
{
	setName(tr("MultiPeak"));

	if (profile == Gauss)
	{
		d_f = gauss_multi_peak_f;
		d_df = gauss_multi_peak_df;
		d_fdf = gauss_multi_peak_fdf;
		d_fsimplex = gauss_multi_peak_d;
	}
	else
	{
		d_f = lorentz_multi_peak_f;
		d_df = lorentz_multi_peak_df;
		d_fdf = lorentz_multi_peak_fdf;
		d_fsimplex = lorentz_multi_peak_d;
	}

	d_param_init = NULL;
	covar = NULL;
	d_results = NULL;

	setNumPeaks(peaks);

	generate_peak_curves = true;
	d_peaks_color = 2;//green
}

void MultiPeakFit::setNumPeaks(int n)
{
	d_peaks = n;
	if (d_profile == Gauss)
		d_explanation = tr("Gauss Fit");
	else
		d_explanation = tr("Lorentz Fit");
	if (d_peaks > 1)
		d_explanation += "(" + QString::number(d_peaks) +") " + tr("multi-peak");

	d_p = 3*d_peaks + 1;
    d_min_points = d_p;

	if(d_param_init) gsl_vector_free(d_param_init);
	d_param_init = gsl_vector_alloc(d_p);
	gsl_vector_set_all (d_param_init, 1.0);

	if (covar) gsl_matrix_free(covar);
	covar = gsl_matrix_alloc (d_p, d_p);
	if (d_results) delete[] d_results;
	d_results = new double[d_p];

	d_param_names = generateParameterList(d_peaks);
	d_formula = generateFormula(d_peaks, d_profile);
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
	for (int i=0; i<d_p; i++)
		d_results[i] = par[i];

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
	c->setData(x, y, d_points);
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
	c->setFormula(formula.replace("--", "+").replace("-+", "-").replace("+-", "-"));
	d_graph->insertPlotItem(c, title, Graph::Line);
}

void MultiPeakFit::generateFitCurve(double *par)
{
	ApplicationWindow *app = (ApplicationWindow *)parent();
	if (!d_gen_function)
		d_points = d_n;

	gsl_matrix * m = gsl_matrix_alloc (d_points, d_peaks);
	if (!m)
	{
		QMessageBox::warning(app, tr("QtiPlot - Fit Error"), tr("Could not allocate enough memory for the fit curves!"));
		return;
	}

	double *X = new double[d_points];
	double *Y = new double[d_points];
	int i, j;
	int peaks_aux = d_peaks;
	if (d_peaks == 1)
		peaks_aux--;

	if (d_gen_function)
	{
		double step = (d_x[d_n-1] - d_x[0])/(d_points-1);
		for (i = 0; i<d_points; i++)
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
				for (j=0; j<d_points; j++)
					Y[j] = gsl_matrix_get (m, j, i);

				insertPeakFunctionCurve(X, Y, i);
			}
		}
	}
	else
	{
		QString tableName = app->generateUniqueName(tr("Fit"));
		QString label = d_explanation + " " + tr("fit of") + " " + d_curve->title().text();

		Table *t = app->newHiddenTable(tableName, label, d_points, peaks_aux + 2);
		QStringList header = QStringList() << "1";
		for (i = 0; i<peaks_aux; i++)
			header << tr("peak") + QString::number(i+1);
		header << "2";
		t->setHeader(header);

		for (i = 0; i<d_points; i++)
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
		c->setData(X, Y, d_points);
		d_graph->insertPlotItem(c, tableName+"_1(X),"+label+"(Y)", Graph::Line);

		if (generate_peak_curves)
		{
			for (i=0; i<peaks_aux; i++)
			{//add the peak curves
				for (j=0; j<d_points; j++)
					Y[j] = gsl_matrix_get (m, j, i);

				label = tableName + "_" + tr("peak") + QString::number(i+1);
				c = new QwtPlotCurve(label);
				c->setPen(QPen(ColorBox::color(d_peaks_color), 1)); 
				c->setData(X, Y, d_points);
				d_graph->insertPlotItem(c, tableName+"_1(X),"+label+"(Y)", Graph::Line);
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

	LorentzFit::LorentzFit(ApplicationWindow *parent, Graph *g, const QString& curveTitle, double start, double end)
: MultiPeakFit(parent, g, MultiPeakFit::Lorentz, 1)
{
	init();
	setDataFromCurve(curveTitle, start, end);
}

void LorentzFit::init()
{
	setName("Lorentz");
	d_explanation = tr("Lorentz");
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
	d_explanation = tr("Gauss");
	d_param_explain << tr("(area)") << tr("(center)") << tr("(width)") << tr("(offset)");
}

	GaussFit::GaussFit(ApplicationWindow *parent, Graph *g, const QString& curveTitle)
: MultiPeakFit(parent, g, MultiPeakFit::Gauss, 1)
{
	init();
	setDataFromCurve(curveTitle);
}

	GaussFit::GaussFit(ApplicationWindow *parent, Graph *g, const QString& curveTitle, double start, double end)
: MultiPeakFit(parent, g, MultiPeakFit::Gauss, 1)
{
	init();
	setDataFromCurve(curveTitle, start, end);
}

void GaussFit::init()
{
	setName("Gauss");
	d_explanation = tr("Gauss");
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
	d_formula = "A*x + B";
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
