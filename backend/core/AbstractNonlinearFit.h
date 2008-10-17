/***************************************************************************
    File                 : AbstractNonlinearFit.h
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

#ifndef ABSTRACT_NONLINEAR_FIT_H
#define ABSTRACT_NONLINEAR_FIT_H

#include "AbstractFit.h"

class FitSetAlgorithmCmd;

//! Base class for non-linear fitting (using GSL).
/**
 * Here, non-linearity refers to the fit parameter dependence of the model function;
 * not to its dependence on the predictor, x.
 *
 * Implementations of AbstractNonlinearFit essentially provide methods to compute a specific
 * function to be fitted, its derivatives with respect to the fit parameters and sensible
 * estimates for the parameters (given the data to be fitted). Specifically, these are
 * provided by implementing f(), df() and guessInitialValues().
 *
 * GSL, being a C library, uses void pointers to pass along information needed by fit
 * functions. We take a fully object-oriented approach here by setting having this
 * pointer point to an instance of AbstractNonlinearFit.
 *
 * In order to reduce code duplication and make the fitting code easier to understand, some
 * compromises have been made in terms of efficiency. While GSL routines allow a large degree
 * of optimization in the model computations (by reusing partial results), the signature
 * chosen for f() and df() doesn't allow this; and it means we need more functions calls,
 * some of which are virtual. Generally, the approach here is meant to be the most elegant
 * and simple; following the rule that premature optimization is the root of all evil.
 * TODO: run some benchmarks in order to decide whether we need optimizations, and where
 *       they make sense
 */
class AbstractNonlinearFit : public AbstractFit
{
	Q_OBJECT

	public:
		enum Algorithm {
			ScaledLevenbergMarquardt,
			UnscaledLevenbergMarquardt,
			NelderMeadSimplex
		};
		static QString nameOf(Algorithm algo) {
			switch(algo) {
				case ScaledLevenbergMarquardt: return tr("scaled Levenberg-Marquardt");
				case UnscaledLevenbergMarquardt: return tr("unscaled Levenberg-Marquardt");
				case NelderMeadSimplex: return tr("Nelder-Mead / simplex");
			}
		};
		AbstractNonlinearFit();
		Algorithm algorithm() const { return m_algorithm; }
		void setAlgorithm(Algorithm a);

		//!\name Handling of fit parameters
		//@{
		void setInitialValue(int parameter, double value);
		virtual void guessInitialValues() = 0;
		//@}

	protected:
		virtual void dataChanged(AbstractColumn*);

		//! Given fit parameters, compute model function.
		virtual double f(const gsl_vector * params, double x) const = 0;
		//! Given fit parameters, compute derivative of model function with respect to the parameters.
		virtual void df(const gsl_vector * params, double x, gsl_vector * out) const = 0;

		//! Fit function for GSL multifit routines.
		/**
		 * out_i is set to F(x_i), where F(x_i) = (f(x_i) - Y(i)) / m_y_errors[i]
		 */
		static int fitFunction(const gsl_vector * params, void * self, gsl_vector * out);
		//! Derivative of fitFunction for GSL multifit routines.
		/**
		 * out_{i,j} ist set to (d F(x_i) / d param_j); that is, df(x_i)[j] / m_y_errors[i].
		 */
		static int fitFunctionDf(const gsl_vector * params, void * self, gsl_matrix * out);
		//! Combined function/derivative for GSL multifit routines.
		static int fitFunctionFDf(const gsl_vector * params, void * self, gsl_vector * out_f, gsl_matrix * out_df);
		//! Adapter function for fitting with GSL multimin routines.
		/**
		 * \returns \sum_i ((f(x_i) - Y(i)) / m_y_errors[i])^2
		 */
		static double multiminFunction(const gsl_vector * params, void * self);

	private:
		void fitGslMultimin();
		void fitGslMultifit();

		//! Fit algorithm to use.
		Algorithm m_algorithm;
		//! The tolerance ("epsilon") to be used for deciding when the fit was successful.
		double m_tolerance;
		//! The maximum number of iterations to do before declaring the fit to have failed.
		int m_max_iterations;
		//! Initial values for fit parameters.
		gsl_vector * m_initial_values;
		//! Exit status of last fit operation.
		int m_exit_status;
		//! Number of iterations the last fit took.
		int m_iterations;

	friend class FitSetAlgorithmCmd;
};

#endif // ifndef ABSTRACT_NONLINEAR_FIT_H
