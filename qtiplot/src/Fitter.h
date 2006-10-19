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
#include "application.h"

class QwtPlotCurve;
class LegendMarker;

//! TODO
class Fitter : public QObject
{
    Q_OBJECT

public:
	enum Solver{ScaledLevenbergMarquardt, UnscaledLevenbergMarquardt, NelderMeadSimplex};
	enum WeightingMethod{NoWeighting, Instrumental, Statistical, ArbDataset};

    Fitter( ApplicationWindow *parent, Graph *g = 0);
    ~Fitter();

	void fitSimplex(gsl_multimin_function f, int &iterations, int &status);
	void fitGSL(gsl_multifit_function_fdf f, int &iterations, int &status);
	virtual void fit(){};

	void setParametersList(const QStringList& lst){d_param_names = lst;};

	bool setWeightingData(WeightingMethod w, const QString& colName = QString::null);

	void setDataFromCurve(const QString& curveTitle);
	void setDataFromCurve(const QString& curveTitle, double from, double to);
	void setDataFromCurve(QwtPlotCurve *curve, int start, int end);

	void setGraph(Graph *g){d_graph = g;};

	void setInitialGuesses(double *x_init);
	void setSolver(Solver s){d_solver = s;};

	void setTolerance(double eps){d_tolerance = eps;};
	void setFitCurveColor(int colorId){d_curveColorIndex = colorId;};

	virtual void generateFitCurve(double *par, double *X, double *Y){};
	void setFitCurveParameters(bool generate, int points = 0);

	void setMaximumIterations(int iter){d_max_iterations = iter;};

	//! Output string added to the result log
	QString logFitInfo(double *par, int iterations, int status);

	//! Output string added to the plot as a new legend
	QString legendFitInfo();

	//! Returns the fit results as a string list
	QStringList fitResultsList();

	void showParametersTable(const QString& tableName);
	void showCovarianceMatrix(const QString& matrixName);

protected:
	//! The graph where the result curve should be displayed
	Graph *d_graph;

	//! Pointer to the GSL multifit solver
	gsl_multifit_fdfsolver *s;

	//! Pointer to the GSL multifit minimizer (for simplex algorithm)
	gsl_multimin_fminimizer *s_min;

	//! Size of the data arrays
	int d_n;

	//! Number of fit parameters
	int d_p;

	//! Initial guesses for the fit parameters 
	gsl_vector *d_param_init;

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

	//! Solver type
	Solver d_solver;

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
};

class ExponentialFitter : public Fitter
{
    Q_OBJECT

public:
	ExponentialFitter( bool expGrowth, ApplicationWindow *parent, Graph *g);

	void fit();
	void generateFitCurve(double *par, double *X, double *Y);

private:
	bool is_exp_growth;
};
#endif
