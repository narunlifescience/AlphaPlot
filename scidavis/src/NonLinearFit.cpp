/***************************************************************************
    File                 : NonLinearFit.cpp
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Benkert
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : NonLinearFit class

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
#include "NonLinearFit.h"
#include "MyParser.h"
#include "fit_gsl.h"

#include <QMessageBox>

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
	setObjectName(tr("NonLinear"));
	d_formula = QString::null;
	d_f = user_f;
	d_df = user_df;
	d_fdf = user_fdf;
	d_fsimplex = user_d;
	d_explanation = tr("Non-linear");
        d_init_err = false;
}

void NonLinearFit::setFormula(const QString& s)
{
	d_formula = s;
}

void NonLinearFit::setParametersList(const QStringList& lst)
{
	if ((int)lst.count() < 1)
	{
		QMessageBox::critical((ApplicationWindow *)parent(), tr("Fit Error"),
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
    for (int i=0; i<d_p; i++)
        d_script->setDouble(par[i], d_param_names[i]);

    if (d_gen_function) {
        double X0 = d_x[0];
        double step = (d_x[d_n-1]-X0)/(d_points-1);
        for (int i=0; i<d_points; i++) {
            X[i] = X0+i*step;
            d_script->setDouble(X[i], "x");
            Y[i] = d_script->eval().toDouble();
        }
    } else {
        for (int i=0; i<d_points; i++) {
            X[i] = d_x[i];
            d_script->setDouble(X[i], "x");
            Y[i] = d_script->eval().toDouble();
        }
    }
}
