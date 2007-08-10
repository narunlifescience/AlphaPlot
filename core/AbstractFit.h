/***************************************************************************
    File                 : AbstractFit.h
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Knut Franke
    Email (use @ for *)  : knut.franke*gmx.de
    Description          : Base class for doing fits using the algorithms
                           provided by GSL.

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
#ifndef ABSTRACT_FIT_H
#define ABSTRACT_FIT_H

#include "AbstractFilter.h"

/**
 * \brief Base class for doing fits using the algorithms provided by GSL.
 *
 * Implementations of AbstractFit essentially provide methods to compute a specific function to be
 * fitted, its derivatives with respect to the fit parameters and sensible estimates for the
 * parameters (given the data to be fitted).
 *
 * Input is accepted on two ports: X and Y. Y is assumed to be a function of X, which is
 * approximated by the function specified by the implementation of AbstractFit being used.
 *
 * On its output ports, AbstractFit provides (in order) the current values of the parameters,
 * their names, and textual descriptions. Current values can be initial values specified by
 * the user, automated estimates made by the implementation, or the results of the fit.
 * You only ever get to see the first two when you haven't connected the input ports of
 * AbstractFit yet, because after the first connect (and possibly subsequent calls to
 * setInitialValue()), the fit is automatically recomputed. After calling setAutoRefit(),
 * it is also recomputed whenever the input data changes.
 */
class AbstractFit : public AbstractFilter
{
	public:
		enum Algorithm { ScaledLevenbergMarquardt, UnscaledLevenbergMarquardt, NelderMeadSimplex };
		//! Possible sources for weighting data.
		enum  {
			NoWeighting, //!< Use 1.0 as weight for all points (?!)
			Instrumental, //!< Use data source of error bars for weighting.
			Statistical, //!< Assume data follows Poisson distribution (weight with sqrt(N)).
			Dataset //!< Use weighting data from a user-specified data source.
		};
		AbstractFit() : d_algorithm(ScaledLevenbergMarquardt), d_tolerance(1e-8), d_maxiter(1000), d_auto_refit(false), d_outdated(true) {}
		virtual ~AbstractFit() {}
		bool isOutdated() const { return d_outdated; }
		void setAlgorithm(Algorithm a) { d_algorithm = a; }
		void setAutoRefit(bool auto_refit = true) { d_auto_refit = auto_refit; }

		//!\name Handling of fit parameters
		//@{
		void setInitialValue(int parameter, double value);
		virtual void guessInitialValues() = 0;
		//@}

		//!\name Reimplemented from AbstractFilter
		//@{
	public:
		virtual int inputCount() const { return 2; }
		virtual QString inputName(int port) const { return port==0 ? "X" : "Y"; }
	protected:
		virtual void dataChanged(AbstractDataSource*) {
		}
		//@}

	protected:
		static virtual double fitFunctionSimplex(const gsl_vector*, void*) = 0;
		static virtual int fitFunction(const gsl_vector*, void*, gsl_vector*) = 0;
		static virtual int fitFunctionDf(const gsl_vector*, void*, gsl_matrix*) = 0;
		static virtual int fitFunctionFdf(const gsl_vector*, void*, gsl_vector*, gsl_matrix*) {
		}

	private:
		//! Fit algorithm to use.
		Algorithm d_algorithm;
		//! Where to take weights from.
		WeightingMethod d_weighting;
		//! The tolerance ("epsilon") to be used for deciding when the fit was successful.
		double d_tolerance;
		//! The maximum number of iterations to do before declaring the fit to have failed.
		int d_maxiter;
		//! Whether to redo the fit each time input data changes.
		bool d_auto_refit;
		//! If #d_auto_refit is false, this is true when the input data has changed since the last fit.
		bool d_outdated;
};

#endif // ifndef ABSTRACT_FIT_H
