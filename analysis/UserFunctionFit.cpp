/***************************************************************************
    File                 : UserFunctionFit.cpp
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Benkert
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : UserFunctionFit class

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
#include "UserFunctionFit.h"
#include "core/MyParser.h"
#include "fit_gsl.h"

#include <QMessageBox>

UserFunctionFit::UserFunctionFit(ApplicationWindow *parent, Layer *layer)
	: Fit(parent, layer)
{
	init();
}

UserFunctionFit::UserFunctionFit(ApplicationWindow *parent, Layer *layer, const QString& curveTitle)
	: Fit(parent, layer)
{
	init();
	setDataFromCurve(curveTitle);
}

UserFunctionFit::UserFunctionFit(ApplicationWindow *parent, Layer *layer, const QString& curveTitle, double start, double end)
	: Fit(parent, layer)
{
	init();
	setDataFromCurve(curveTitle, start, end);
}

void UserFunctionFit::init()
{
	setName(tr("UserFunction"));
	m_formula = QString::null;
	m_f = user_f;
	m_df = user_df;
	m_fdf = user_fdf;
	m_fsimplex = user_d;
	m_explanation = tr("Non-linear");
}

void UserFunctionFit::setFormula(const QString& s)
{
	if (s.isEmpty())
	{
		QMessageBox::critical((ApplicationWindow *)parent(),  tr("Input function error"),
				tr("Please enter a valid non-empty expression! Operation aborted!"));
		m_init_err = true;
		return;
	}

	if (!m_p)
	{
		QMessageBox::critical((ApplicationWindow *)parent(), tr("Fit Error"),
				tr("There are no parameters specified for this fit operation. Please define a list of parameters first!"));
		m_init_err = true;
		return;
	}

	if (m_formula == s)
		return;

	try
	{
		double *param = new double[m_p];
		MyParser parser;
		double xvar;
		parser.DefineVar("x", &xvar);
		for (int k=0; k<(int)m_p; k++)
		{
			param[k]=gsl_vector_get(m_param_init, k);
			parser.DefineVar(m_param_names[k].toAscii().constData(), &param[k]);
		}
		parser.SetExpr(s.toAscii().constData());
		parser.Eval() ;
		delete[] param;
	}
	catch(mu::ParserError &e)
	{
		QMessageBox::critical((ApplicationWindow *)parent(),  tr("Input function error"), QString::fromStdString(e.GetMsg()));
		m_init_err = true;
		return;
	}

	m_init_err = false;
	m_formula = s;
}

void UserFunctionFit::setParametersList(const QStringList& lst)
{
	if ((int)lst.count() < 1)
	{
		QMessageBox::critical((ApplicationWindow *)parent(), tr("Fit Error"),
				tr("You must provide a list containing at least one parameter for this type of fit. Operation aborted!"));
		m_init_err = true;
		return;
	}

	m_init_err = false;
	m_param_names = lst;

	if (m_p > 0)
	{//free previously allocated memory
		gsl_vector_free(m_param_init);
		gsl_matrix_free (covar);
		delete[] m_results;
	}

	m_p = (int)lst.count();
    m_min_points = m_p;
	m_param_init = gsl_vector_alloc(m_p);
	gsl_vector_set_all (m_param_init, 1.0);

	covar = gsl_matrix_alloc (m_p, m_p);
	m_results = new double[m_p];

	for (int i=0; i<m_p; i++)
		m_param_explain << "";
}

void UserFunctionFit::calculateFitCurveData(double *par, double *X, double *Y)
{
	MyParser parser;
	for (int i=0; i<m_p; i++)
		parser.DefineVar(m_param_names[i].toAscii().constData(), &par[i]);

	double xvar;
	parser.DefineVar("x", &xvar);
	parser.SetExpr(m_formula.toAscii().constData());

	if (m_gen_function)
	{
		double X0 = m_x[0];
		double step = (m_x[m_n-1]-X0)/(m_points-1);
		for (int i=0; i<m_points; i++)
		{
			X[i] = X0+i*step;
			xvar = X[i];
			Y[i] = parser.Eval();
		}
	}
	else
	{
		for (int i=0; i<m_points; i++)
		{
			X[i] = m_x[i];
			xvar = X[i];
			Y[i] = parser.Eval();
		}
	}
	delete[] par;
}
