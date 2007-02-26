/***************************************************************************
    File                 : Interpolation.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Ion Vasilief
    Email                : ion_vasilief@yahoo.fr
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

#include <QApplication>
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
        QMessageBox::critical((ApplicationWindow *)parent(), tr("QtiPlot") + " - " + tr("Error"),
        tr("Unknown interpolation method. Valid values are: 0 - Linear, 1 - Cubic, 2 - Akima."));
        d_init_err = true;
        return;
    }
    d_method = m;
	switch(d_method)
	{
		case 0:
			setName(tr("Linear") + tr("Int"));
			d_explanation = tr("Linear") + " " + tr("Interpolation");
			break;
		case 1:
			setName(tr("Cubic") + tr("Int"));
			d_explanation = tr("Cubic") + " " + tr("Interpolation");
			break;
		case 2:
			setName(tr("Akima") + tr("Int"));
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
        QMessageBox::critical((ApplicationWindow *)parent(), tr("QtiPlot - Error"),
        tr("Unknown interpolation method, valid values are: 0 - Linear, 1 - Cubic, 2 - Akima."));
        d_init_err = true;
        return;
    }
int min_points = m + 3;
if (d_n < min_points)
	{
		QMessageBox::critical((ApplicationWindow *)parent(), tr("QtiPlot") + " - " + tr("Error"),
				tr("You need at least %1 points in order to perform this operation!").arg(min_points));
        d_init_err = true;
        return;
	}
d_method = m;
d_min_points = min_points;
}

void Interpolation::calculateOutputData(double *X, double *Y)
{
	gsl_interp_accel *acc = gsl_interp_accel_alloc ();
	const gsl_interp_type *method;
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
	   X[j] = d_from + j*step;
	   Y[j] = gsl_spline_eval (interp, X[j], acc);
	}

	gsl_spline_free (interp);
	gsl_interp_accel_free (acc);
}

int Interpolation::sortedCurveData(QwtPlotCurve *c, double start, double end, double **x, double **y)
{
    if (!c || c->rtti() != QwtPlotItem::Rtti_PlotCurve)
        return 0;

    int i_start = 0, i_end = c->dataSize();
    for (int i = 0; i < i_end; i++)
  	    if (c->x(i) > start && i)
        {
  	      i_start = i - 1;
          break;
        }
    for (int i = i_end-1; i >= 0; i--)
  	    if (c->x(i) < end && i < c->dataSize())
        {
  	      i_end = i + 1;
          break;
        }
    int n = i_end - i_start + 1;
    (*x) = new double[n];
    (*y) = new double[n];
    double *xtemp = new double[n];
    double *ytemp = new double[n];

	double pr_x;
  	int j=0;
    for (int i = i_start; i <= i_end; i++)
    {
        xtemp[j] = c->x(i);
        if (xtemp[j] == pr_x)
        {
            delete (*x);
            delete (*y);
            return -1;//this kind of data causes division by zero in GSL interpolation routines
        }
        pr_x = xtemp[j];
        ytemp[j++] = c->y(i);
    }
    size_t *p = new size_t[n];
    gsl_sort_index(p, xtemp, 1, n);
    for (int i=0; i<n; i++)
    {
        (*x)[i] = xtemp[p[i]];
  	    (*y)[i] = ytemp[p[i]];
    }
    delete[] xtemp;
    delete[] ytemp;
    delete[] p;
    return n;
}
