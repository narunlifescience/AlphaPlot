/***************************************************************************
    File                 : Fitter.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : TODO
                           
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
#
#ifndef FITTER_H
#define FITTER_H

#include <qobject.h>
#include <qlibrary.h>

#include "application.h"

class QwtPlotCurve;
class LegendMarker;
class Table;
class Matrix;

//! TODO
class Fitter : public QObject
{
	Q_OBJECT

	public:
		typedef double (*fit_function_simplex)(const gsl_vector *, void *);
		typedef int (*fit_function)(const gsl_vector *, void *, gsl_vector *);
		typedef int (*fit_function_df)(const gsl_vector *, void *, gsl_matrix *);
		typedef int (*fit_function_fdf)(const gsl_vector *, void *, gsl_vector *, gsl_matrix *);

		enum Algorithm{ScaledLevenbergMarquardt, UnscaledLevenbergMarquardt, NelderMeadSimplex};
		enum WeightingMethod{NoWeighting, Instrumental, Statistical, ArbDataset};

		Fitter(ApplicationWindow *parent, Graph *g = 0, const char * name = 0);
		~Fitter();

		virtual void fit();

		bool setWeightingData(WeightingMethod w, const QString& colName = QString::null);

		bool setDataFromCurve(const QString& curveTitle);
		bool setDataFromCurve(const QString& curveTitle, double from, double to);
		void setDataFromCurve(QwtPlotCurve *curve, int start, int end);

		void setGraph(Graph *g){d_graph = g;};

		QString formula(){return d_formula;};
		virtual void setParametersList(const QStringList& lst){};

		void setInitialGuess(int parIndex, double val){gsl_vector_set(d_param_init, parIndex, val);};
		void setInitialGuesses(double *x_init);

		virtual void guessInitialValues(){};

		void setAlgorithm(Algorithm s){d_solver = s;};

		void setTolerance(double eps){d_tolerance = eps;};
		void setFitCurveColor(int colorId){d_curveColorIndex = colorId;};

		virtual void generateFitCurve(double *par){};
		void setFitCurveParameters(bool generate, int points = 0);

		void setMaximumIterations(int iter){d_max_iterations = iter;};

		//! Added a new legend to the plot. Calls virtual legendFitInfo()
		void showLegend();

		//! Output string added to the plot as a new legend
		virtual QString legendFitInfo(int prec);

		//! Returns a vector with the fit results
		double* fitResults(){return d_results;};

		//! Returns the sum of squares of the residuals from the best-fit line
		double chiSquare() {return chi_2;};

		Table* parametersTable(const QString& tableName);
		Matrix* covarianceMatrix(const QString& matrixName);

	private:
		//! Pointer to the GSL multifit minimizer (for simplex algorithm)
		gsl_multimin_fminimizer * fitSimplex(gsl_multimin_function f, int &iterations, int &status);

		//! Pointer to the GSL multifit solver
		gsl_multifit_fdfsolver * fitGSL(gsl_multifit_function_fdf f, int &iterations, int &status);

		//! Customs and stores the fit results according to the derived class specifications. Used by exponential fits.
		virtual void storeCustomFitResults(double *par);

	protected:
		//! Output string added to the result log
		virtual QString logFitInfo(double *par, int iterations, int status, int prec, const QString& plotName);

		//! The graph where the result curve should be displayed
		Graph *d_graph;

		fit_function d_f;
		fit_function_df d_df;
		fit_function_fdf d_fdf;
		fit_function_simplex d_fsimplex;

		//! Size of the data arrays
		int d_n;

		//! Number of fit parameters
		int d_p;

		//! Initial guesses for the fit parameters 
		gsl_vector *d_param_init;

		//! Tells weather the fitter uses non-linear/simplex fitting 
		// with an initial parameters set, that must be freed in the destructor.
		bool is_non_linear;

		//! x data set to be fitted
		double *d_x;

		//! y data set to be fitted
		double *d_y;

		//! weighting data set used for the fit
		double *d_w;

		//! GSL Tolerance
		double d_tolerance;

		//! Names of the fit parameters
		QStringList d_param_names;

		//! Tells weather the result curve has the same x values as the fit data or not
		bool gen_x_data;

		//! Number of result points to de calculated and displayed in the result curve
		int d_result_points;

		//! Color index of the result curve
		int d_curveColorIndex;

		//! Maximum number of iterations per fit
		int d_max_iterations;

		//! Algorithm type
		Algorithm d_solver;

		//! The curve to be fitted
		QwtPlotCurve *d_curve; 

		//! The fit formula
		QString d_formula;

		//! The fit type: exponential decay, gauss etc...
		QString d_fit_type;

		//! Covariance matrix
		gsl_matrix *covar;

		//! The kind of weighting to be performed on the data
		WeightingMethod d_weihting;

		//! The name of the weighting dataset
		QString weighting_dataset;

		//! Stores the result parameters
		double *d_results;

		//! The sum of squares of the residuals from the best-fit line
		double chi_2;
};

class ExponentialFitter : public Fitter
{
	Q_OBJECT

	public:
		ExponentialFitter( bool expGrowth, ApplicationWindow *parent, Graph *g);

	private:
		void storeCustomFitResults(double *par);
		void generateFitCurve(double *par);

		bool is_exp_growth;
};

class TwoExpFitter : public Fitter
{
	Q_OBJECT

	public:
		TwoExpFitter(ApplicationWindow *parent, Graph *g);

	private:
		void storeCustomFitResults(double *par);
		void generateFitCurve(double *par);
};

class ThreeExpFitter : public Fitter
{
	Q_OBJECT

	public:
		ThreeExpFitter(ApplicationWindow *parent, Graph *g);

	private:
		void storeCustomFitResults(double *par);
		void generateFitCurve(double *par);
};

class SigmoidalFitter : public Fitter
{
	Q_OBJECT

	public:
		SigmoidalFitter(ApplicationWindow *parent, Graph *g);
		void guessInitialValues();

	private:
		void generateFitCurve(double *par);
};

class GaussFitter : public Fitter
{
	Q_OBJECT

	public:
		GaussFitter(ApplicationWindow *parent, Graph *g);
		void guessInitialValues();

	private:
		void generateFitCurve(double *par);
};

class LorentzFitter : public Fitter
{
	Q_OBJECT

	public:
		LorentzFitter(ApplicationWindow *parent, Graph *g);
		void guessInitialValues();

	private:
		void storeCustomFitResults(double *par);
		void generateFitCurve(double *par);
};

class NonLinearFitter : public Fitter
{
	Q_OBJECT

	public:
		NonLinearFitter(ApplicationWindow *parent, Graph *g, const QString& formula = QString::null);
		void setParametersList(const QStringList& lst);
		void setFormula(const QString& s){if (d_formula != s) d_formula = s;};

	private:
		void generateFitCurve(double *par);
};

class PluginFitter : public Fitter
{
	Q_OBJECT

	public:
		typedef double (*fitFunctionEval)(double, double *);
		PluginFitter(ApplicationWindow *parent, Graph *g);
		bool load(const QString& pluginName);

	private:
		void generateFitCurve(double *par);
		fitFunctionEval f_eval;
};

class MultiPeakFitter : public Fitter
{
	Q_OBJECT

	public:

		enum PeakProfile{Gauss, Lorentz};
		MultiPeakFitter(ApplicationWindow *parent, Graph *g = 0, PeakProfile profile = Gauss, int peaks = 0);

		int peaks(){return d_peaks;};
		static QString generateFormula(int order, PeakProfile profile);
		static QStringList generateParameterList(int order);

	private:
		QString logFitInfo(double *par, int iterations, int status, int prec, const QString& plotName);
		void generateFitCurve(double *par);
		void storeCustomFitResults(double *par);

		int d_peaks;
		PeakProfile d_profile;
};

class PolynomialFitter : public Fitter
{
	Q_OBJECT

	public:
		PolynomialFitter(ApplicationWindow *parent, Graph *g, int order = 2, bool legend = false);

		virtual QString legendFitInfo(int prec);
		void fit();

		static QString generateFormula(int order);
		static QStringList generateParameterList(int order);

	private:
		void generateFitCurve(double *par);

		int d_order;
		bool show_legend;
};

class LinearFitter : public Fitter
{
	Q_OBJECT

	public:
		LinearFitter(ApplicationWindow *parent, Graph *g);
		void fit();
		void generateFitCurve(double *par);
};
#endif
