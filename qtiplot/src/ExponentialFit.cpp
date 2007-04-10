/***************************************************************************
    File                 : fitclasses.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : Exponential fit classes

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
#include "ExponentialFit.h"
#include "fit_gsl.h"

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
		d_formula = "y0+A*exp(x/t)";
		d_param_explain << "(amplitude)" << "(lifetime)" << "(offset)";
	}
	else
	{
		setName("ExpDecay");
		d_explanation = tr("Exponential decay");
		d_formula = "y0+A*exp(-x/t)";
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
