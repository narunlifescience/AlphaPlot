/***************************************************************************
    File                 : fitclasses.cpp
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Benkert
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

	ExponentialFit::ExponentialFit(ApplicationWindow *parent, Layer *g, bool expGrowth)
: Fit(parent, g),
	is_exp_growth(expGrowth)
{
	init();
}

ExponentialFit::ExponentialFit(ApplicationWindow *parent, Layer *g, const QString& curveTitle, bool expGrowth)
: Fit(parent, g),
  is_exp_growth(expGrowth)
{
	init();
	setDataFromCurve(curveTitle);
}

ExponentialFit::ExponentialFit(ApplicationWindow *parent, Layer *g,
		const QString& curveTitle, double start, double end, bool expGrowth)
: Fit(parent, g),
	is_exp_growth(expGrowth)
{
	init();
	setDataFromCurve(curveTitle, start, end);
}

void ExponentialFit::init()
{
	m_f = exp_f;
	m_df = exp_df;
	m_fdf = exp_fdf;
	m_fsimplex = exp_d;
	m_p = 3;
    m_min_points = m_p;
	m_param_init = gsl_vector_alloc(m_p);
	gsl_vector_set_all (m_param_init, 1.0);

	covar = gsl_matrix_alloc (m_p, m_p);
	m_results = new double[m_p];
	m_param_names << "A" << "t" << "y0";

	if (is_exp_growth)
	{
		setName("ExpGrowth");
		m_explanation = tr("Exponential growth");
		m_formula = "y0+A*exp(x/t)";
		m_param_explain << "(amplitude)" << "(lifetime)" << "(offset)";
	}
	else
	{
		setName("ExpDecay");
		m_explanation = tr("Exponential decay");
		m_formula = "y0+A*exp(-x/t)";
		m_param_explain << "(amplitude)" << "(e-folding time)" << "(offset)";
	}
}

void ExponentialFit::storeCustomFitResults(double *par)
{
	for (int i=0; i<m_p; i++)
		m_results[i] = par[i];

	if (is_exp_growth)
		m_results[1]=-1.0/m_results[1];
	else
		m_results[1]=1.0/m_results[1];
}

void ExponentialFit::calculateFitCurveData(double *par, double *X, double *Y)
{
	if (m_gen_function)
	{
		double X0 = m_x[0];
		double step = (m_x[m_n-1]-X0)/(m_points-1);
		for (int i=0; i<m_points; i++)
		{
			X[i] = X0+i*step;
			Y[i] = par[0]*exp(-par[1]*X[i])+par[2];
		}
	}
	else
	{
		for (int i=0; i<m_points; i++)
		{
			X[i] = m_x[i];
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

	TwoExpFit::TwoExpFit(ApplicationWindow *parent, Layer *g)
: Fit(parent, g)
{
	init();
}

	TwoExpFit::TwoExpFit(ApplicationWindow *parent, Layer *g, const QString& curveTitle)
: Fit(parent, g)
{
	init();
	setDataFromCurve(curveTitle);
}

	TwoExpFit::TwoExpFit(ApplicationWindow *parent, Layer *g, const QString& curveTitle, double start, double end)
: Fit(parent, g)
{
	init();
	setDataFromCurve(curveTitle, start, end);
}

void TwoExpFit::init()
{
	setName("ExpDecay");
	m_f = expd2_f;
	m_df = expd2_df;
	m_fdf = expd2_fdf;
	m_fsimplex = expd2_d;
	m_p = 5;
    m_min_points = m_p;
	m_param_init = gsl_vector_alloc(m_p);
	gsl_vector_set_all (m_param_init, 1.0);
	covar = gsl_matrix_alloc (m_p, m_p);
	m_results = new double[m_p];
	m_param_names << "A1" << "t1" << "A2" << "t2" << "y0";
	m_explanation = tr("Exponential decay");
	m_formula = "A1*exp(-x/t1)+A2*exp(-x/t2)+y0";
	m_param_explain << "(first amplitude)" << "(first lifetime)" << "(second amplitude)" << "(second lifetime)" << "(offset)";
}

void TwoExpFit::storeCustomFitResults(double *par)
{
	for (int i=0; i<m_p; i++)
		m_results[i] = par[i];

	m_results[1]=1.0/m_results[1];
	m_results[3]=1.0/m_results[3];
}

void TwoExpFit::calculateFitCurveData(double *par, double *X, double *Y)
{
	if (m_gen_function)
	{
		double X0 = m_x[0];
		double step = (m_x[m_n-1]-X0)/(m_points-1);
		for (int i=0; i<m_points; i++)
		{
			X[i] = X0+i*step;
			Y[i] = par[0]*exp(-par[1]*X[i])+par[2]*exp(-par[3]*X[i])+par[4];
		}
	}
	else
	{
		for (int i=0; i<m_points; i++)
		{
			X[i] = m_x[i];
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

	ThreeExpFit::ThreeExpFit(ApplicationWindow *parent, Layer *g)
: Fit(parent, g)
{
	init();
}

	ThreeExpFit::ThreeExpFit(ApplicationWindow *parent, Layer *g, const QString& curveTitle)
: Fit(parent, g)
{
	init();
	setDataFromCurve(curveTitle);
}

	ThreeExpFit::ThreeExpFit(ApplicationWindow *parent, Layer *g, const QString& curveTitle, double start, double end)
: Fit(parent, g)
{
	init();
	setDataFromCurve(curveTitle, start, end);
}

void ThreeExpFit::init()
{
	setName("ExpDecay");
	m_f = expd3_f;
	m_df = expd3_df;
	m_fdf = expd3_fdf;
	m_fsimplex = expd3_d;
	m_p = 7;
    m_min_points = m_p;
	m_param_init = gsl_vector_alloc(m_p);
	gsl_vector_set_all (m_param_init, 1.0);
	covar = gsl_matrix_alloc (m_p, m_p);
	m_results = new double[m_p];
	m_param_names << "A1" << "t1" << "A2" << "t2" << "A3" << "t3" << "y0";
	m_explanation = tr("Exponential decay");
	m_formula = "A1*exp(-x/t1)+A2*exp(-x/t2)+A3*exp(-x/t3)+y0";
	m_param_explain << "(first amplitude)" << "(first lifetime)" << "(second amplitude)" << "(second lifetime)" << "(third amplitude)" << "(third lifetime)" << "(offset)";
}

void ThreeExpFit::storeCustomFitResults(double *par)
{
	for (int i=0; i<m_p; i++)
		m_results[i] = par[i];

	m_results[1]=1.0/m_results[1];
	m_results[3]=1.0/m_results[3];
	m_results[5]=1.0/m_results[5];
}

void ThreeExpFit::calculateFitCurveData(double *par, double *X, double *Y)
{
	if (m_gen_function)
	{
		double X0 = m_x[0];
		double step = (m_x[m_n-1]-X0)/(m_points-1);
		for (int i=0; i<m_points; i++)
		{
			X[i]=X0+i*step;
			Y[i]=par[0]*exp(-X[i]*par[1])+par[2]*exp(-X[i]*par[3])+par[4]*exp(-X[i]*par[5])+par[6];
		}
	}
	else
	{
		for (int i=0; i<m_points; i++)
		{
			X[i]=m_x[i];
			Y[i]=par[0]*exp(-X[i]*par[1])+par[2]*exp(-X[i]*par[3])+par[4]*exp(-X[i]*par[5])+par[6];
		}
	}
	delete[] par;
}
