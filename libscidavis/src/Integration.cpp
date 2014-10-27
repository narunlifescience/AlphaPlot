/***************************************************************************
    File                 : Integration.cpp
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Ion Vasilief
    Email (use @ for *)  : ion_vasilief*yahoo.fr
    Description          : Numerical integration of data sets

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
#include "Integration.h"
#include "MultiLayer.h"
#include "Legend.h"

#include <QMessageBox>
#include <QDateTime>
#include <QLocale>

#include <gsl/gsl_spline.h>
#include <gsl/gsl_interp.h>
#include <gsl/gsl_vector.h>

Integration::Integration(ApplicationWindow *parent, Graph *g)
: Filter(parent, g)
{
	init();
}

Integration::Integration(ApplicationWindow *parent, Graph *g, const QString& curveTitle)
: Filter(parent, g)
{
	init();
	setDataFromCurve(curveTitle);
}

Integration::Integration(ApplicationWindow *parent, Graph *g, const QString& curveTitle, double start, double end)
: Filter(parent, g)
{
	init();
	setDataFromCurve(curveTitle, start, end);
}

void Integration::init()
{
	setObjectName(tr("Integration"));
	d_method = Linear;
	d_sort_data = true;
	d_result = NAN;
}

bool Integration::isDataAcceptable()
{
	const gsl_interp_type *method_t;
	switch (d_method) {
		case Linear:
			method_t = gsl_interp_linear;
			break;
		case Cubic:
			method_t = gsl_interp_cspline;
			break;
		case Akima:
			method_t = gsl_interp_akima;
			break;
		default:
			QMessageBox::critical((ApplicationWindow *)parent(), tr("SciDAVis") + " - " + tr("Error"),
			tr("Unknown interpolation method. Valid values are: 0 - Linear, 1 - Cubic, 2 - Akima."));
			d_init_err = true;
			return true;

	}
	// GSL interpolation routines fail with division by zero on such data
	for (int i=1; i<d_n; i++)
		if (d_x[i-1] == d_x[i]) {
			QMessageBox::critical((ApplicationWindow *)parent(), tr("SciDAVis") + " - " + tr("Error"),
					tr("Several data points have the same x value causing divisions by zero, operation aborted!"));
			return false;
		}

	return Filter::isDataAcceptable();
}

QString Integration::logInfo()
{
	const gsl_interp_type *method_t;
	QString method_name;
	switch (d_method) {
		case Linear:
			method_t = gsl_interp_linear;
			method_name = tr("Linear");
			break;
		case Cubic:
			method_t = gsl_interp_cspline;
			method_name = tr("Cubic");
			break;
		case Akima:
			method_t = gsl_interp_akima;
			method_name = tr("Akima");
			break;
	}

	gsl_interp *interpolation = gsl_interp_alloc(method_t, d_n);
	gsl_interp_init(interpolation, d_x, d_y, d_n);

	if (d_n < gsl_interp_min_size(interpolation))
	{
		QMessageBox::critical((ApplicationWindow *)parent(), tr("SciDAVis") + " - " + tr("Error"),
				tr("You need at least %1 points in order to perform this operation!").arg(gsl_interp_min_size(interpolation)));
		d_init_err = true;
		return "";
	}

	QString logInfo = "[" + QDateTime::currentDateTime().toString(Qt::LocalDate) + "\t" + tr("Plot")+ ": ''" + d_graph->parentPlotName() + "'']\n";
	logInfo += "\n" + tr("Numerical integration of") + ": " + d_curve->title().text() + tr(" using ") + method_name + tr("Interpolation") + "\n";

	ApplicationWindow *app = (ApplicationWindow *)parent();
	int prec = app->d_decimal_digits;
	logInfo += tr("Points") + ": "+QString::number(d_n) + " " + tr("from") + " x = " +QLocale().toString(d_from, 'g', prec) + " ";
	logInfo += tr("to") + " x = " + QLocale().toString(d_to, 'g', prec) + "\n";

	// using GSL to find maximum value of data set
	gsl_vector *aux = gsl_vector_alloc(d_n);
	for(int i=0; i < d_n; i++)
		gsl_vector_set (aux, i, d_y[i]);
	int maxID=gsl_vector_max_index (aux);
	gsl_vector_free (aux);

	// calculate result
	d_result = gsl_interp_eval_integ(interpolation, d_x, d_y, d_from, d_to, 0);

	logInfo += tr("Peak at") + " x = " + QLocale().toString(d_x[maxID], 'g', prec)+"\t";
	logInfo += "y = " + QLocale().toString(d_y[maxID], 'g', prec)+"\n";

	logInfo += tr("Area") + "=";
	logInfo += QLocale().toString(d_result, 'g', prec);
	logInfo += "\n-------------------------------------------------------------\n";

	gsl_interp_free(interpolation);

	return logInfo;
}

