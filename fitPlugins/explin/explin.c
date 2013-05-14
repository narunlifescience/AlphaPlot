/***************************************************************************
   File                 : planck_wavelength.c
   Project              : SciDAVis
   Description          : Fit plugin for "exp(-b1*x)/(b2+b3*x)"
   --------------------------------------------------------------------
   Copyright            : (C) 2008 Knut Franke (knut.franke*gmx.de)
                          (replace * with @ in the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *  This program is free software; you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation; either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  As a special exception, you may omit the above copyright notice when   *
 *  distributing modified copies of this file (for instance, when using it *
 *  as a template for your own fit plugin).                                *
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

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <math.h>

struct data {
	size_t n;
	size_t p;
	double * X;
	double * Y;
	double * sigma;
};

char * name() { return "Exp/Lin"; }

char * function() { return "exp(-b1*x)/(b2+b3*x)"; }

char * parameters() { return "b1,b2,b3"; }

double function_eval(double x, double * params)
{
	return exp(-params[0]*x)/(params[1]+params[2]*x);
}

int function_f(const gsl_vector * params, void * void_data, gsl_vector * f)
{
	struct data * d = (struct data*) void_data;
	double p[3];
	size_t i;
	for (i=0; i<d->p; i++)
		p[i] = gsl_vector_get(params, i);

	for (i=0; i<d->n; i++)
		gsl_vector_set(f, i, (function_eval(d->X[i], p) - d->Y[i])/d->sigma[i]);

	return GSL_SUCCESS;
}

double function_d(const gsl_vector * params, void * void_data)
{
	struct data * d = (struct data*) void_data;
	gsl_vector * f = gsl_vector_alloc(d->n);
	double result = 0;
	size_t i;

	function_f(params, void_data, f);
	for (i=0; i<d->n; i++)
		result += pow(gsl_vector_get(f, i), 2);

	gsl_vector_free(f);
	return result;
}

int function_df(const gsl_vector * params, void * void_data, gsl_matrix * J)
{
	struct data * d = (struct data*) void_data;
	double b1 = gsl_vector_get(params, 0);
	double b2 = gsl_vector_get(params, 1);
	double b3 = gsl_vector_get(params, 2);
	size_t i;
	for (i=0; i<d->n; i++) {
		double x = d->X[i];
		double l = b2+b3*x;
		double f = (-1) * exp(-b1*x) / (l * d->sigma[i]);
		gsl_matrix_set(J, i, 0, x*f);
		gsl_matrix_set(J, i, 1, f/l);
		gsl_matrix_set(J, i, 2, x*f/l);
	}
	return GSL_SUCCESS;
}

int function_fdf(const gsl_vector * params, void * void_data, gsl_vector * f, gsl_matrix * J)
{
	function_f(params, void_data, f);
	function_df(params, void_data, J);
	return GSL_SUCCESS;
}
