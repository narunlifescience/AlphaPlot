/***************************************************************************
    File                 : AbstractNonlinearFit.cpp
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2008 Knut Franke
    Email (use @ for *)  : Knut.Franke*gmx.net
    Description          : Base class for non-linear fitting (using GSL)

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

#include "AbstractNonlinearFit.h"

#include <gsl/gsl_multifit_nlin.h>
#include <gsl/gsl_multimin.h>
#include <gsl/gsl_blas.h>
#include <QUndoCommand>

class FitSetAlgorithmCmd : public QUndoCommand
{
	public:
		FitSetAlgorithmCmd(AbstractNonlinearFit *target, AbstractNonlinearFit::Algorithm algo)
			: m_target(target), m_other_algo(algo) {
				setText(QObject::tr("%1: change fit algorithm to %2.").arg(m_target->name()).arg(AbstractNonlinearFit::nameOf(m_other_algo)));
			}

		virtual void undo() {
			AbstractNonlinearFit::Algorithm tmp = m_target->m_algorithm;
			m_target->m_algorithm = m_other_algo;
			m_other_algo = tmp;
			m_target->dataChanged(0);
		}

		virtual void redo() { undo(); }

	private:
		AbstractNonlinearFit *m_target;
		AbstractNonlinearFit::Algorithm m_other_algo;
};

AbstractNonlinearFit::AbstractNonlinearFit()
	: m_algorithm(ScaledLevenbergMarquardt),
	m_tolerance(1e-8),
	m_max_iterations(1000),
	m_exit_status(GSL_EINVAL),
	m_iterations(0)
{
}

void AbstractNonlinearFit::setAlgorithm(Algorithm a)
{
	exec(new FitSetAlgorithmCmd(this, a));
}

int AbstractNonlinearFit::fitFunction(const gsl_vector * params, void * self, gsl_vector * out)
{
	AbstractNonlinearFit * me = static_cast<AbstractNonlinearFit*>(self);
	const AbstractColumn * x = me->m_inputs.value(0);
	const AbstractColumn * y = me->m_inputs.value(1);
	if (!x || !y) return GSL_EINVAL;
	int n = me->m_input_points;
	for (int i=0; i<n; i++)
		gsl_vector_set(out, i, (me->f(params, x->valueAt(i)) - y->valueAt(i)) / me->m_y_errors[i]);
	return GSL_SUCCESS;
}

int AbstractNonlinearFit::fitFunctionDf(const gsl_vector * params, void * self, gsl_matrix * out)
{
	AbstractNonlinearFit * me = static_cast<AbstractNonlinearFit*>(self);
	const AbstractColumn * x = me->m_inputs.value(0);
	const AbstractColumn * y = me->m_inputs.value(1);
	if (!x || !y) return GSL_EINVAL;
	int n = me->m_input_points;
	for (int i=0; i<n; i++) {
		gsl_vector_view row_i = gsl_matrix_row(out, i);
		me->df(params, x->valueAt(i), &row_i.vector);
		gsl_vector_scale(&row_i.vector, 1.0/me->m_y_errors[i]);
	}
	return GSL_SUCCESS;
}

int AbstractNonlinearFit::fitFunctionFDf(const gsl_vector * params, void * self, gsl_vector * out_f, gsl_matrix * out_df)
{
	AbstractNonlinearFit * me = static_cast<AbstractNonlinearFit*>(self);
	const AbstractColumn * x = me->m_inputs.value(0);
	const AbstractColumn * y = me->m_inputs.value(1);
	if (!x || !y) return GSL_EINVAL;
	int n = me->m_input_points;
	for (int i=0; i<n; i++) {
		gsl_vector_set(out_f, i, (me->f(params, x->valueAt(i)) - y->valueAt(i)) / me->m_y_errors[i]);
		gsl_vector_view row_i = gsl_matrix_row(out_df, i);
		me->df(params, x->valueAt(i), &row_i.vector);
		gsl_vector_scale(&row_i.vector, 1.0/me->m_y_errors[i]);
	}
	return GSL_SUCCESS;
}

double AbstractNonlinearFit::multiminFunction(const gsl_vector * params, void * self)
{
	AbstractNonlinearFit * me = static_cast<AbstractNonlinearFit*>(self);
	const AbstractColumn * x = me->m_inputs.value(0);
	const AbstractColumn * y = me->m_inputs.value(1);
	if (!x || !y) return GSL_EINVAL;

	double result = 0;
	size_t i;
	int n = me->m_input_points;

	for (i=0; i<n; i++)
		result += pow((me->f(params, x->valueAt(i)) - y->valueAt(i)) / me->m_y_errors[i], 2);

	return result;
}

void AbstractNonlinearFit::dataChanged(AbstractColumn* s)
{
	AbstractFit::dataChanged(s);

	if (numParameters() < 1) return;
	if (m_input_points < numParameters()) return;

	if (m_algorithm == NelderMeadSimplex)
		fitGslMultimin();
	else
		fitGslMultifit();
}

void AbstractNonlinearFit::fitGslMultimin()
{
	// initialize minimizer
	const gsl_multimin_fminimizer_type * type = gsl_multimin_fminimizer_nmsimplex;
	gsl_multimin_fminimizer *minimizer = gsl_multimin_fminimizer_alloc(type, numParameters());
	gsl_multimin_function f;
	f.f = &multiminFunction;
	f.n = numParameters();
	f.params = this;
	gsl_vector * step_size = gsl_vector_alloc (numParameters());
	gsl_vector_set_all (step_size, 10.0);
	gsl_multimin_fminimizer_set (minimizer, &f, m_initial_values, step_size);

	// iterate minimization algorithm
	for (m_iterations = 0; m_iterations < m_max_iterations; m_iterations++) {
		m_exit_status = gsl_multimin_fminimizer_iterate(minimizer);
		if (m_exit_status) break;

		double size = gsl_multimin_fminimizer_size (minimizer);
		m_exit_status = gsl_multimin_test_size (size, m_tolerance);
		if (m_exit_status != GSL_CONTINUE) break;
	}

	// grab results
	gsl_vector_memcpy(m_results, minimizer->x);
	m_chi_square = minimizer->fval;
	gsl_matrix *J = gsl_matrix_alloc(m_input_points, numParameters());
	fitFunctionDf(minimizer->x, this, J);
	gsl_multifit_covar(J, 0.0, m_covariance_matrix);
	gsl_matrix_free(J);
	if (m_y_error_source == UnknownErrors) {
		// multiply covar by variance of residuals, which is used as an estimate for the
		// statistical errors (this relies on the Y errors being set to 1.0)
		gsl_matrix_scale(m_covariance_matrix, m_chi_square/(m_input_points-numParameters()));
	}

	// free previously allocated memory
	gsl_multimin_fminimizer_free(minimizer);
	gsl_vector_free(step_size);
}

void AbstractNonlinearFit::fitGslMultifit()
{
	// initialize solver
	const gsl_multifit_fdfsolver_type * type;
	switch(m_algorithm) {
		case ScaledLevenbergMarquardt:
			type = gsl_multifit_fdfsolver_lmsder;
			break;
		case UnscaledLevenbergMarquardt:
			type = gsl_multifit_fdfsolver_lmder;
			break;
	}
	gsl_multifit_fdfsolver * solver = gsl_multifit_fdfsolver_alloc (type, m_input_points, numParameters());
	gsl_multifit_function_fdf f;
	f.f = &fitFunction;
	f.df = &fitFunctionDf;
	f.fdf = &fitFunctionFDf;
	f.n = m_input_points;
	f.p = numParameters();
	f.params = this;
	gsl_multifit_fdfsolver_set (solver, &f, m_initial_values);

	// iterate solver algorithm
	for (m_iterations=0; m_iterations < m_max_iterations; m_iterations++) {
		m_exit_status = gsl_multifit_fdfsolver_iterate (solver);
		if (m_exit_status) break;

		m_exit_status = gsl_multifit_test_delta (solver->dx, solver->x, m_tolerance, 0);
		if (m_exit_status != GSL_CONTINUE) break;
	}

	// grab results
	gsl_vector_memcpy(m_results, solver->x);
	gsl_blas_ddot(solver->f, solver->f, &m_chi_square);
	gsl_multifit_covar(solver->J, 0.0, m_covariance_matrix);
	if (m_y_error_source == UnknownErrors) {
		// multiply covar by variance of residuals, which is used as an estimate for the
		// statistical errors (this relies on the Y errors being set to 1.0, so that
		// solver->f is properly normalized)
		gsl_matrix_scale(m_covariance_matrix, m_chi_square/(m_input_points-numParameters()));
	}

	// free memory allocated for fitting
	gsl_multifit_fdfsolver_free(solver);
}
