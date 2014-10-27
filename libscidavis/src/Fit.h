/***************************************************************************
    File                 : Fit.h
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Benkert
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : Fit base class
                           
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
#ifndef FIT_H
#define FIT_H

#include <QObject>

#include "Filter.h"
#include "Script.h"

#include <gsl/gsl_multifit_nlin.h>
#include <gsl/gsl_multimin.h>

class Table;
class Matrix;
class ApplicationWindow;
class Script;

//! Fit base class
class Fit : public Filter, public scripted
{
	Q_OBJECT

	public:

		typedef double (*fit_function_simplex)(const gsl_vector *, void *);
		typedef int (*fit_function)(const gsl_vector *, void *, gsl_vector *);
		typedef int (*fit_function_df)(const gsl_vector *, void *, gsl_matrix *);
		typedef int (*fit_function_fdf)(const gsl_vector *, void *, gsl_vector *, gsl_matrix *);

		enum Algorithm{ScaledLevenbergMarquardt, UnscaledLevenbergMarquardt, NelderMeadSimplex};
		enum ErrorSource {UnknownErrors, AssociatedErrors, PoissonErrors, CustomErrors};

		Fit(ApplicationWindow *parent, Graph *g = 0, QString name = QString());
		~Fit();

		//! Actually does the fit. Should be reimplemented in derived classes.
		virtual void fit();

		//! Sets the data set to be used as source of Y errors.
		bool setYErrorSource(ErrorSource err, const QString& colName = QString::null, bool fail_silently=false);

		void setDataCurve(int curve, double start, double end);

		QString formula(){return d_formula;};
		int numParameters() {return d_p;}

		void setInitialGuess(int parIndex, double val){gsl_vector_set(d_param_init, parIndex, val);};
		void setInitialGuesses(double *x_init);

		virtual void guessInitialValues(){};

		void setAlgorithm(Algorithm s){d_solver = s;};

		//! Specifies weather the result of the fit is a function curve
		void generateFunction(bool yes, int points = 100);

		//! Output string added to the plot as a new legend
		virtual QString legendInfo();

		//! Returns a vector with the fit results
		double* results(){return d_results;};

		//! Returns a vector with the standard deviations of the results
		double* errors();

		//! Returns the sum of squares of the residuals from the best-fit line
		double chiSquare() {return chi_2;};

		//! Returns the coefficient of determination, R^2
		double rSquare();

		//! Specifies wheather the errors must be scaled with sqrt(chi_2/dof)
		void scaleErrors(bool yes = true){d_scale_errors = yes;};

		Table* parametersTable(const QString& tableName);
		Matrix* covarianceMatrix(const QString& matrixName);

                int evaluate_f(const gsl_vector * x, gsl_vector * f);
                double evaluate_d(const gsl_vector * x);
                int evaluate_df(const gsl_vector *x, gsl_matrix *J);
                static double evaluate_df_helper(double x, void * param);

        protected slots:
                void scriptError(const QString& message,const QString& script_name,int line_number);

	private:
		//! Execute the fit using GSL multidimensional minimization (Nelder-Mead Simplex).
		double * fitGslMultimin(int &iterations, int &status);

		//! Execute the fit using GSL non-linear least-squares fitting (Levenberg-Marquardt).
		double * fitGslMultifit(int &iterations, int &status);

		//! Customs and stores the fit results according to the derived class specifications. Used by exponential fits.
		virtual void storeCustomFitResults(double *par);

	protected:
		//! Adds the result curve as a FunctionCurve to the plot, if d_gen_function = true
		void insertFitFunctionCurve(const QString& name, double *x, double *y, int penWidth = 1);

		//! Adds the result curve to the plot
		virtual void generateFitCurve(double *par);

		//! Calculates the data for the output fit curve and store itin the X an Y vectors
		virtual void calculateFitCurveData(double *par, double *X, double *Y) { Q_UNUSED(par) Q_UNUSED(X) Q_UNUSED(Y)   };

		//! Output string added to the result log
		virtual QString logFitInfo(double *par, int iterations, int status, const QString& plotName);

		fit_function d_f;
		fit_function_df d_df;
		fit_function_fdf d_fdf;
		fit_function_simplex d_fsimplex;

		//! Number of fit parameters
		int d_p;

		//! Initial guesses for the fit parameters 
		gsl_vector *d_param_init;

		/*! \brief Tells whether the fitter uses non-linear/simplex fitting 
		 * with an initial parameters set, that must be freed in the destructor.
		 */
		bool is_non_linear;

		//! Standard deviations of Y input data.
		double *d_y_errors;

		//! Names of the fit parameters
		QStringList d_param_names;

		//! Stores a list of short explanations for the significance of the fit parameters
		QStringList d_param_explain;

		//! Specifies weather the result curve is a FunctionCurve or a normal curve with the same x values as the fit data
		bool d_gen_function;

		//! Algorithm type
		Algorithm d_solver;

		//! The fit formula
		QString d_formula;

		//! Covariance matrix
		gsl_matrix *covar;

		//! Where standard errors of the input data are taken from.
		ErrorSource d_y_error_source;

		//! The name of the dataset containing Y standard errors (if applicable).
		QString d_y_error_dataset;

		//! Stores the result parameters
		double *d_results;

		//! Stores standard deviations of the result parameters
		double *d_result_errors;

		//! The sum of squares of the residuals from the best-fit line
		double chi_2;

		//! Specifies wheather the errors must be scaled with sqrt(chi_2/dof)
		bool d_scale_errors;

                //! Script used to evaluate user-defined functions.
                Script * d_script;
};

#endif
