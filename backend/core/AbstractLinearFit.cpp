/***************************************************************************
    File                 : AbstractLinearFit.cpp
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2008 Knut Franke
    Email (use @ for *)  : Knut.Franke*gmx.net
    Description          : Base class for linear fitting (using GSL).

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

#include "AbstractLinearFit.h"
#include <gsl/gsl_multifit.h>

void AbstractLinearFit::dataChanged(AbstractColumn* s)
{
	AbstractFit::dataChanged(s);

	if (numParameters() < 1) return;
	if (m_input_points < numParameters()) return;

	const AbstractColumn * x_col = m_inputs.value(0);
	const AbstractColumn * y_col = m_inputs.value(1);

	if (!x_col || !y_col) return;

	gsl_matrix * X = gsl_matrix_alloc (m_input_points, numParameters());
	for (int i=0; i<m_input_points; i++) {
		gsl_vector_view row_i = gsl_matrix_row(X, i);
		df(x_col->valueAt(i), &row_i.vector);
	}

	gsl_vector * y = gsl_vector_alloc(m_input_points);
	for (int i=0; i<m_input_points; i++)
		gsl_vector_set(y, i, y_col->valueAt(i));

	gsl_vector * weights = gsl_vector_alloc(m_input_points);
	for (int i=0; i<m_input_points; i++)
		gsl_vector_set(weights, i, 1.0/pow(m_y_errors[i], 2));

	gsl_multifit_linear_workspace * work = gsl_multifit_linear_alloc(m_input_points, numParameters());

	if (m_y_error_source == UnknownErrors)
		gsl_multifit_linear (X, y, m_results, m_covariance_matrix, &m_chi_square, work);
	else
		gsl_multifit_wlinear (X, weights, y, m_results, m_covariance_matrix, &m_chi_square, work);

	gsl_matrix_free(X);
	gsl_vector_free(y);
	gsl_vector_free(weights);
	gsl_multifit_linear_free(work);
}
