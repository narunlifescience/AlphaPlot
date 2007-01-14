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
class Fit : public QObject
{
	Q_OBJECT

	public:

		typedef double (*fit_function_simplex)(const gsl_vector *, void *);
		typedef int (*fit_function)(const gsl_vector *, void *, gsl_vector *);
		typedef int (*fit_function_df)(const gsl_vector *, void *, gsl_matrix *);
		typedef int (*fit_function_fdf)(const gsl_vector *, void *, gsl_vector *, gsl_matrix *);

		enum Algorithm{ScaledLevenbergMarquardt, UnscaledLevenbergMarquardt, NelderMeadSimplex};
		enum WeightingMethod{NoWeighting, Instrumental, Statistical, ArbDataset};

		Fit(ApplicationWindow *parent, Graph *g = 0, const char * name = 0);
		~Fit();

		//! Actually does the fit. Should be reimplemented in derived classes.
		virtual void fit();

		bool setWeightingData(WeightingMethod w, const QString& colName = QString::null);

		bool setDataFromCurve(const QString& curveTitle, Graph *g = 0);
		bool setDataFromCurve(const QString& curveTitle, double from, double to, Graph *g = 0);
		void setDataFromCurve(QwtPlotCurve *curve, int start, int end);

		//! Changes the data range if the source curve was already assigned. Provided for convenience.
		void setRange(double from, double to);

		QString formula(){return d_formula;};
		virtual void setParametersList(const QStringList& lst){ Q_UNUSED(lst) };
		int numParameters() { return d_p; }

		void setInitialGuess(int parIndex, double val){gsl_vector_set(d_param_init, parIndex, val);};
		void setInitialGuesses(double *x_init);

		virtual void guessInitialValues(){};

		void setAlgorithm(Algorithm s){d_solver = s;};

		void setTolerance(double eps){d_tolerance = eps;};
		void setFitCurveColor(int colorId){d_curveColorIndex = colorId;};

		void setFitCurveParameters(bool generate, int points = 0);

		void setMaximumIterations(int iter){d_max_iterations = iter;};

		//! Added a new legend to the plot. Calls virtual legendFitInfo()
		void showLegend();

		//! Output string added to the plot as a new legend
		virtual QString legendFitInfo(int prec);

		//! Returns a vector with the fit results
		double* results(){return d_results;};

		//! Returns a vector with the standard deviations of the results
		double* errors();

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
		//! Adds the result curve as a FunctionCurve to the plot, if gen_x_data = true
		void insertFitFunctionCurve(const QString& name, double *x, double *y, int prec, int penWidth = 1);

		//! Adds the result curve to the plot
		virtual void generateFitCurve(double *par);

		//! Calculates the data for the output fit curve and store itin the X an Y vectors
		virtual void calculateFitCurveData(double *par, double *X, double *Y) { Q_UNUSED(par) Q_UNUSED(X) Q_UNUSED(Y)   };

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

		//! Stores a list of short explanations for the significance of the fit parameters
		QStringList d_param_explain;

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

		//! Stores standard deviations of the result parameters
		double *d_errors;

		//! The sum of squares of the residuals from the best-fit line
		double chi_2;
};

class ExponentialFit : public Fit
{
	Q_OBJECT

	public:
		ExponentialFit(ApplicationWindow *parent, Graph *g,  bool expGrowth = false);
		ExponentialFit(ApplicationWindow *parent, Graph *g, const QString& curveTitle, bool expGrowth = false);
		ExponentialFit(ApplicationWindow *parent, Graph *g, const QString& curveTitle, 
				int start, int end, bool expGrowth = false);

		/* needed to support SIP versions before 4.4 (which introduced /NoDerived/) */
		/* DON'T USE! */
		ExponentialFit(Graph *, const QString&, bool) : Fit(NULL, NULL, NULL) {};
		ExponentialFit(Graph *, const QString&, int, int, bool) : Fit(NULL, NULL, NULL) {};

	private:
		void init();
		void storeCustomFitResults(double *par);
		void calculateFitCurveData(double *par, double *X, double *Y);

		bool is_exp_growth;
};

class TwoExpFit : public Fit
{
	Q_OBJECT

	public:
		TwoExpFit(ApplicationWindow *parent, Graph *g);
		TwoExpFit(ApplicationWindow *parent, Graph *g, const QString& curveTitle);
		TwoExpFit(ApplicationWindow *parent, Graph *g, const QString& curveTitle, int start, int end);

		/* needed to support SIP versions before 4.4 (which introduced /NoDerived/) */
		/* DON'T USE! */
		TwoExpFit(Graph *, const QString&) : Fit(NULL, NULL, NULL) {};
		TwoExpFit(Graph *, const QString&, int, int) : Fit(NULL, NULL, NULL) {};

	private:
		void init();
		void storeCustomFitResults(double *par);
		void calculateFitCurveData(double *par, double *X, double *Y);
};

class ThreeExpFit : public Fit
{
	Q_OBJECT

	public:
		ThreeExpFit(ApplicationWindow *parent, Graph *g);
		ThreeExpFit(ApplicationWindow *parent, Graph *g, const QString& curveTitle);
		ThreeExpFit(ApplicationWindow *parent, Graph *g, const QString& curveTitle, int start, int end);

		/* needed to support SIP versions before 4.4 (which introduced /NoDerived/) */
		/* DON'T USE! */
		ThreeExpFit(Graph *, const QString&) : Fit(NULL, NULL, NULL) {};
		ThreeExpFit(Graph *, const QString&, int, int) : Fit(NULL, NULL, NULL) {};

	private:
		void init();
		void storeCustomFitResults(double *par);
		void calculateFitCurveData(double *par, double *X, double *Y);
};

class SigmoidalFit : public Fit
{
	Q_OBJECT

	public:
		SigmoidalFit(ApplicationWindow *parent, Graph *g);
		SigmoidalFit(ApplicationWindow *parent, Graph *g, const QString& curveTitle);
		SigmoidalFit(ApplicationWindow *parent, Graph *g, const QString& curveTitle, int start, int end);
		void guessInitialValues();

		/* needed to support SIP versions before 4.4 (which introduced /NoDerived/) */
		/* DON'T USE! */
		SigmoidalFit(Graph *, const QString&) : Fit(NULL, NULL, NULL) {};
		SigmoidalFit(Graph *, const QString&, int, int) : Fit(NULL, NULL, NULL) {};

	private:
		void init();
		void calculateFitCurveData(double *par, double *X, double *Y);
};

class GaussAmpFit : public Fit
{
	Q_OBJECT

	public:
		GaussAmpFit(ApplicationWindow *parent, Graph *g);
		GaussAmpFit(ApplicationWindow *parent, Graph *g, const QString& curveTitle);
		GaussAmpFit(ApplicationWindow *parent, Graph *g, const QString& curveTitle, int start, int end);

		/* needed to support SIP versions before 4.4 (which introduced /NoDerived/) */
		/* DON'T USE! */
		GaussAmpFit(Graph *, const QString&) : Fit(NULL, NULL, NULL) {};
		GaussAmpFit(Graph *, const QString&, int, int) : Fit(NULL, NULL, NULL) {};

	private:
		void init();
		void calculateFitCurveData(double *par, double *X, double *Y);
};

class NonLinearFit : public Fit
{
	Q_OBJECT

	public:
		NonLinearFit(ApplicationWindow *parent, Graph *g);
		NonLinearFit(ApplicationWindow *parent, Graph *g, const QString& curveTitle);
		NonLinearFit(ApplicationWindow *parent, Graph *g, const QString& curveTitle, int start, int end);

		/* needed to support SIP versions before 4.4 (which introduced /NoDerived/) */
		/* DON'T USE! */
		NonLinearFit(Graph *, const QString&) : Fit(NULL, NULL, NULL) {};
		NonLinearFit(Graph *, const QString&, int, int) : Fit(NULL, NULL, NULL) {};

		void setParametersList(const QStringList& lst);
		void setFormula(const QString& s){if (d_formula != s) d_formula = s;};

	private:
		void calculateFitCurveData(double *par, double *X, double *Y);
		void init();
};

class PluginFit : public Fit
{
	Q_OBJECT

	public:
		PluginFit(ApplicationWindow *parent, Graph *g);
		PluginFit(ApplicationWindow *parent, Graph *g, const QString& curveTitle);
		PluginFit(ApplicationWindow *parent, Graph *g, const QString& curveTitle, int start, int end);

		/* needed to support SIP versions before 4.4 (which introduced /NoDerived/) */
		/* DON'T USE! */
		PluginFit(Graph *, const QString&) : Fit(NULL, NULL, NULL) {};
		PluginFit(Graph *, const QString&, int, int) : Fit(NULL, NULL, NULL) {};

		bool load(const QString& pluginName);

	private:
		void init();
		typedef double (*fitFunctionEval)(double, double *);
		void calculateFitCurveData(double *par, double *X, double *Y);
		fitFunctionEval f_eval;
};

class MultiPeakFit : public Fit
{
	Q_OBJECT

	public:		
		enum PeakProfile{Gauss, Lorentz};
		MultiPeakFit(ApplicationWindow *parent, Graph *g = 0, PeakProfile profile = Gauss, int peaks = 1);

		int peaks(){return d_peaks;};

		void enablePeakCurves(bool on){generate_peak_curves = on;};
		void setPeakCurvesColor(int colorIndex){d_peaks_color = colorIndex;};

		static QString generateFormula(int order, PeakProfile profile);
		static QStringList generateParameterList(int order);

	private:
		QString logFitInfo(double *par, int iterations, int status, int prec, const QString& plotName);
		void generateFitCurve(double *par);
		static QString peakFormula(int peakIndex, PeakProfile profile);
		//! Inserts a peak function curve into the plot 
		void insertPeakFunctionCurve(double *x, double *y, int prec, int peak);
		void storeCustomFitResults(double *par);

		//! Used by the GaussFit and LorentzFit derived classes to calculate initial values for the parameters 
		void guessInitialValues();

		//! Number of peaks
		int d_peaks;

		//! Tells weather the peak curves should be displayed together with the best line fit.
		bool generate_peak_curves;

		//! Color index for the peak curves
		int d_peaks_color;

		//! The peak profile
		PeakProfile d_profile;
};

class LorentzFit : public MultiPeakFit
{
	Q_OBJECT

	public:
		LorentzFit(ApplicationWindow *parent, Graph *g);
		LorentzFit(ApplicationWindow *parent, Graph *g, const QString& curveTitle);
		LorentzFit(ApplicationWindow *parent, Graph *g, const QString& curveTitle, int start, int end);

		/* needed to support SIP versions before 4.4 (which introduced /NoDerived/) */
		/* DON'T USE! */
		LorentzFit(Graph *, const QString&) : MultiPeakFit(NULL) {};
		LorentzFit(Graph *, const QString&, int, int) : MultiPeakFit(NULL) {};

	private:
		void init();
};

class GaussFit : public MultiPeakFit
{
	Q_OBJECT

	public:
		GaussFit(ApplicationWindow *parent, Graph *g);
		GaussFit(ApplicationWindow *parent, Graph *g, const QString& curveTitle);
		GaussFit(ApplicationWindow *parent, Graph *g, const QString& curveTitle, int start, int end);

		/* needed to support SIP versions before 4.4 (which introduced /NoDerived/) */
		/* DON'T USE! */
		GaussFit(Graph *, const QString&) : MultiPeakFit(NULL) {};
		GaussFit(Graph *, const QString&, int, int) : MultiPeakFit(NULL) {};

	private:
		void init();
};

class PolynomialFit : public Fit
{
	Q_OBJECT

	public:
		PolynomialFit(ApplicationWindow *parent, Graph *g, int order = 2, bool legend = false);
		PolynomialFit(ApplicationWindow *parent, Graph *g, QString& curveTitle, int order = 2, bool legend = false);
		PolynomialFit(ApplicationWindow *parent, Graph *g, QString& curveTitle, int start, int end, int order = 2, bool legend = false);

		/* needed to support SIP versions before 4.4 (which introduced /NoDerived/) */
		/* DON'T USE! */
		PolynomialFit(Graph *, QString&, int, bool) : Fit(NULL, NULL, NULL) {};
		PolynomialFit(Graph *, QString&, int, int, int, bool) : Fit(NULL, NULL, NULL) {};

		virtual QString legendFitInfo(int prec);
		void fit();

		static QString generateFormula(int order);
		static QStringList generateParameterList(int order);

	private:
		void init();
		void calculateFitCurveData(double *par, double *X, double *Y);

		int d_order;
		bool show_legend;
};

class LinearFit : public Fit
{
	Q_OBJECT

	public:
		LinearFit(ApplicationWindow *parent, Graph *g);
		LinearFit(ApplicationWindow *parent, Graph *g, const QString& curveTitle);
		LinearFit(ApplicationWindow *parent, Graph *g, const QString& curveTitle, int start, int end);

		/* needed to support SIP versions before 4.4 (which introduced /NoDerived/) */
		/* DON'T USE! */
		LinearFit(Graph *, const QString&) : Fit(NULL, NULL, NULL) {};
		LinearFit(Graph *, const QString&, int, int) : Fit(NULL, NULL, NULL) {};

		void fit();

	private:
		void init();
		void calculateFitCurveData(double *par, double *X, double *Y);
};
#endif
