/***************************************************************************
    File                 : SigmoidalFit.cpp
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Benkert
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : Sigmoidal (Boltzmann) Fit class

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
#include "SigmoidalFit.h"
#include "fit_gsl.h"

#include <QMessageBox>

	SigmoidalFit::SigmoidalFit(ApplicationWindow *parent, Layer *layer)
: Fit(parent, layer)
{
	init();
}

	SigmoidalFit::SigmoidalFit(ApplicationWindow *parent, Layer *layer, const QString& curveTitle)
: Fit(parent, layer)
{
	init();
	setDataFromCurve(curveTitle);
}

	SigmoidalFit::SigmoidalFit(ApplicationWindow *parent, Layer *layer, const QString& curveTitle, double start, double end)
: Fit(parent, layer)
{
	init();
	setDataFromCurve(curveTitle, start, end);
}

void SigmoidalFit::init()
{
	setName("Boltzmann");
	m_f = boltzmann_f;
	m_df = boltzmann_df;
	m_fdf = boltzmann_fdf;
	m_fsimplex = boltzmann_d;
	m_p = 4;
    m_min_points = m_p;
	m_param_init = gsl_vector_alloc(m_p);
	gsl_vector_set_all (m_param_init, 1.0);
	covar = gsl_matrix_alloc (m_p, m_p);
	m_results = new double[m_p];
	m_param_explain << tr("(init value)") << tr("(final value)") << tr("(center)") << tr("(time constant)");
	m_param_names << "A1" << "A2" << "x0" << "dx";
	m_explanation = tr("Boltzmann (Sigmoidal) Fit");
	m_formula = "(A1-A2)/(1+exp((x-x0)/dx))+A2";
}

void SigmoidalFit::calculateFitCurveData(double *par, double *X, double *Y)
{
	if (m_gen_function)
	{
		double X0 = m_x[0];
		double step = (m_x[m_n-1]-X0)/(m_points-1);
		for (int i=0; i<m_points; i++)
		{
			X[i] = X0+i*step;
			Y[i] = (par[0]-par[1])/(1+exp((X[i]-par[2])/par[3]))+par[1];
		}
	}
	else
	{
		for (int i=0; i<m_points; i++)
		{
			X[i] = m_x[i];
			Y[i] = (par[0]-par[1])/(1+exp((X[i]-par[2])/par[3]))+par[1];
		}
	}
	delete[] par;
}

void SigmoidalFit::guessInitialValues()
{
	gsl_vector_view x = gsl_vector_view_array (m_x, m_n);
	gsl_vector_view y = gsl_vector_view_array (m_y, m_n);

	double min_out, max_out;
	gsl_vector_minmax (&y.vector, &min_out, &max_out);

	gsl_vector_set(m_param_init, 0, min_out);
	gsl_vector_set(m_param_init, 1, max_out);
	gsl_vector_set(m_param_init, 2, gsl_vector_get (&x.vector, m_n/2));
	gsl_vector_set(m_param_init, 3, 1.0);
}

