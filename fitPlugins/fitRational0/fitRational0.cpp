/***************************************************************************
   File                 : fitRational0.cpp
   Project              : SciDAVis
   Description          : Fit plugin for "(b + c*x)/(1 + a*x)"
   --------------------------------------------------------------------
   Copyright            : (C) 2005 Ion Vasilief (ion_vasilief*yahoo.fr)
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

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>

#if defined(_MSC_VER) //MSVC Compiler
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif

struct data {
	size_t n;
	size_t p;
	double * X;
	double * Y;
	double * sigma;//weighting data
};

extern "C" MY_EXPORT const char *name()
{
	return "Rational0";
}

extern "C" MY_EXPORT const char *function()
{
	return "(b + c*x)/(1 + a*x)";
}

extern "C" MY_EXPORT const char *parameters()
{
	return "a,b,c";
}

extern "C" MY_EXPORT double function_eval(double x, double *params)
{
	return (params[1] + x*params[2])/(1 + x*params[0]);
}

extern "C" MY_EXPORT int function_f (const gsl_vector * x, void *params,
		gsl_vector * f)
{
	size_t n = ((struct data *)params)->n;
	double *X = ((struct data *)params)->X;
	double *Y = ((struct data *)params)->Y;
	double *sigma = ((struct data *)params)->sigma;

	double a = gsl_vector_get (x, 0);
	double b = gsl_vector_get (x, 1);
	double c = gsl_vector_get (x, 2);

	size_t i;
	for (i = 0; i < n; i++)
	{
		double Yi = (b + c*X[i])/(1 + a*X[i]);
		gsl_vector_set (f, i, (Yi - Y[i])/sigma[i]);
	}

	return GSL_SUCCESS;
}
extern "C" MY_EXPORT double function_d (const gsl_vector * x, void *params)
{
	size_t n = ((struct data *)params)->n;
	double *X = ((struct data *)params)->X;
	double *Y = ((struct data *)params)->Y;
	double *sigma = ((struct data *)params)->sigma;

	double a = gsl_vector_get (x, 0);
	double b = gsl_vector_get (x, 1);
	double c = gsl_vector_get (x, 2);

	size_t i;
	double val=0;
	for (i = 0; i < n; i++)
	{
		double dYi = (((b + c*X[i])/(1 + a*X[i]))-Y[i])/sigma[i];
		val += dYi*dYi;
	}

	return val;
}

extern "C" MY_EXPORT int function_df (const gsl_vector * x, void *params,
		gsl_matrix * J)
{
	size_t n = ((struct data *)params)->n;
	double *X = ((struct data *)params)->X;
	double *sigma = ((struct data *)params)->sigma;

	double a = gsl_vector_get (x, 0);
	double b = gsl_vector_get (x, 1);
	double c = gsl_vector_get (x, 2);

	size_t i;
	for (i = 0; i < n; i++)
	{
		/* Jacobian matrix J(i,j) = dfi / dxj, 
		   where fi = (Yi - Y[i])/sigma[i],				    
		   Yi = (b + c*X[i])/(1 + a*X[i])         
		   and the xj are the parameters (a, b, c) */

		double s = sigma[i];
		double t = X[i];
		double e = 1/(1 + a*t);
		gsl_matrix_set (J, i, 0, -e*e*a*(b+c*t)/s);
		gsl_matrix_set (J, i, 1, e/s);
		gsl_matrix_set (J, i, 2, e*t/s);

	}
	return GSL_SUCCESS;
}

extern "C" MY_EXPORT int function_fdf (const gsl_vector * x, void *params,
		gsl_vector * f, gsl_matrix * J)
{
	function_f (x, params, f);
	function_df (x, params, J);

	return GSL_SUCCESS;
}
