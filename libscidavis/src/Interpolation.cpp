/***************************************************************************
    File                 : Interpolation.cpp
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Ion Vasilief
    Email (use @ for *)  : ion_vasilief*yahoo.fr
    Description          : Numerical interpolation of data sets

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
#include "Interpolation.h"

#include <QMessageBox>

#include <gsl/gsl_sort.h>
#include <gsl/gsl_spline.h>
#include <gsl/gsl_interp.h>

Interpolation::Interpolation(ApplicationWindow *parent, Graph *g, const QString& curveTitle, int m)
: Filter(parent, g)
{
	init(m);
	setDataFromCurve(curveTitle);
}

Interpolation::Interpolation(ApplicationWindow *parent, Graph *g, const QString& curveTitle,
                             double start, double end, int m)
: Filter(parent, g)
{
	init(m);
	setDataFromCurve(curveTitle, start, end);
}

void Interpolation::init(int m)
{
    if (m < 0 || m > 2)
    {
        QMessageBox::critical((ApplicationWindow *)parent(), tr("SciDAVis") + " - " + tr("Error"),
        tr("Unknown interpolation method. Valid values are: 0 - Linear, 1 - Cubic, 2 - Akima."));
        d_init_err = true;
        return;
    }
    d_method = m;
	switch(d_method)
	{
		case 0:
			setObjectName(tr("Linear") + tr("Int"));
			d_explanation = tr("Linear") + " " + tr("Interpolation");
			break;
		case 1:
			setObjectName(tr("Cubic") + tr("Int"));
			d_explanation = tr("Cubic") + " " + tr("Interpolation");
			break;
		case 2:
			setObjectName(tr("Akima") + tr("Int"));
			d_explanation = tr("Akima") + " " + tr("Interpolation");
			break;
	}
    d_sort_data = true;
    d_min_points = d_method + 3;
}


void Interpolation::setMethod(int m)
{
if (m < 0 || m > 2)
    {
        QMessageBox::critical((ApplicationWindow *)parent(), tr("Error"),
        tr("Unknown interpolation method, valid values are: 0 - Linear, 1 - Cubic, 2 - Akima."));
        d_init_err = true;
        return;
    }
int min_points = m + 3;
if (d_n < min_points)
	{
		QMessageBox::critical((ApplicationWindow *)parent(), tr("SciDAVis") + " - " + tr("Error"),
				tr("You need at least %1 points in order to perform this operation!").arg(min_points));
        d_init_err = true;
        return;
	}
d_method = m;
d_min_points = min_points;
}

void Interpolation::calculateOutputData(double *x, double *y)
{
	gsl_interp_accel *acc = gsl_interp_accel_alloc ();
	const gsl_interp_type *method=NULL;
	switch(d_method)
	{
		case 0:
			method = gsl_interp_linear;
			break;
		case 1:
			method = gsl_interp_cspline;
			break;
		case 2:
			method = gsl_interp_akima;
			break;
	}

	gsl_spline *interp = gsl_spline_alloc (method, d_n);
	gsl_spline_init (interp, d_x, d_y, d_n);

    double step = (d_to - d_from)/(double)(d_points - 1);
    for (int j = 0; j < d_points; j++)
	{
	   x[j] = d_from + j*step;
	   y[j] = gsl_spline_eval (interp, x[j], acc);
	}

	gsl_spline_free (interp);
	gsl_interp_accel_free (acc);
}

bool Interpolation::isDataAcceptable()
{
	// GSL interpolation routines fail with division by zero on such data
	for (int i=1; i<d_n; i++)
		if (d_x[i-1] == d_x[i]) {
			QMessageBox::critical((ApplicationWindow *)parent(), tr("SciDAVis") + " - " + tr("Error"),
					tr("Several data points have the same x value causing divisions by zero, operation aborted!"));
			return false;
		}

	return Filter::isDataAcceptable();
}
