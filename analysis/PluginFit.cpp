/***************************************************************************
    File                 : PluginFit.cpp
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Benkert
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : Plugin Fit class

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
#include "PluginFit.h"

#include <QLibrary>
#include <QMessageBox>
#include <QFile>

	PluginFit::PluginFit(ApplicationWindow *parent, Layer *layer)
: Fit(parent, layer)
{
	init();
}

	PluginFit::PluginFit(ApplicationWindow *parent, Layer *layer, const QString& curveTitle)
: Fit(parent, layer)
{
	init();
	setDataFromCurve(curveTitle);
}

	PluginFit::PluginFit(ApplicationWindow *parent, Layer *layer, const QString& curveTitle, double start, double end)
: Fit(parent, layer)
{
	init();
	setDataFromCurve(curveTitle, start, end);
}

void PluginFit::init()
{
	m_explanation = tr("Plugin Fit");
}

bool PluginFit::load(const QString& pluginName)
{
	if (!QFile::exists (pluginName))
	{
		QMessageBox::critical((ApplicationWindow *)parent(), tr("File not found"),
				tr("Plugin file: <p><b> %1 </b> <p>not found. Operation aborted!").arg(pluginName));
		return false;
	}

	setName(pluginName);
	QLibrary lib(pluginName);
	lib.setAutoUnload(false);

	m_fsimplex = (fit_function_simplex) lib.resolve( "function_d" );
	if (!m_fsimplex)
	{
		QMessageBox::critical((ApplicationWindow *)parent(), tr("Plugin Error"),
				tr("The plugin does not implement a %1 method necessary for simplex fitting.").arg("function_d"));
		return false;
	}

	m_f = (fit_function) lib.resolve( "function_f" );
	if (!m_f)
	{
		QMessageBox::critical((ApplicationWindow *)parent(), tr("Plugin Error"),
				tr("The plugin does not implement a %1 method necessary for Levenberg-Marquardt fitting.").arg("function_f"));
		return false;
	}

	m_df = (fit_function_df) lib.resolve( "function_df" );
	if (!m_df)
	{
		QMessageBox::critical((ApplicationWindow *)parent(), tr("Plugin Error"),
				tr("The plugin does not implement a %1 method necessary for Levenberg-Marquardt fitting.").arg("function_df"));
		return false;
	}

	m_fdf = (fit_function_fdf) lib.resolve( "function_fdf" );
	if (!m_fdf)
	{
		QMessageBox::critical((ApplicationWindow *)parent(), tr("Plugin Error"),
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
		m_param_names = QString(fitFunction()).split(",", QString::SkipEmptyParts);
		m_p = (int)m_param_names.count();
        m_min_points = m_p;
		m_param_init = gsl_vector_alloc(m_p);
		covar = gsl_matrix_alloc (m_p, m_p);
		m_results = new double[m_p];
	}
	else
		return false;

	fitFunc fitExplain = (fitFunc) lib.resolve("explanations");
	if (fitExplain)
		m_param_explain = QString(fitExplain()).split(",", QString::SkipEmptyParts);
	else
		for (int i=0; i<m_p; i++)
			m_param_explain << "";

	fitFunction = (fitFunc) lib.resolve( "name" );
	setName(QString(fitFunction()));

	fitFunction = (fitFunc) lib.resolve( "function" );
	if (fitFunction)
		m_formula = QString(fitFunction());
	else
		return false;

	return true;
}

void PluginFit::calculateFitCurveData(double *par, double *X, double *Y)
{
	if (m_gen_function)
	{
		double X0 = m_x[0];
		double step = (m_x[m_n-1]-X0)/(m_points-1);
		for (int i=0; i<m_points; i++)
		{
			X[i] = X0+i*step;
			Y[i]= f_eval(X[i], par);
		}
	}
	else
	{
		for (int i=0; i<m_points; i++)
		{
			X[i] = m_x[i];
			Y[i]= f_eval(X[i], par);
		}
	}
	delete[] par;
}

