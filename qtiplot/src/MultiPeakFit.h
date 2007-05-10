/***************************************************************************
    File                 : MultiPeakFit.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : MultiPeakFit module with Lorentz and Gauss peak shapes
                           
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
#ifndef MULTIPEAKFIT_H
#define MULTIPEAKFIT_H

#include "Fit.h"

class MultiPeakFit : public Fit
{
	Q_OBJECT

	public:		
		enum PeakProfile{Gauss, Lorentz};
		MultiPeakFit(ApplicationWindow *parent, Graph *g = 0, PeakProfile profile = Gauss, int peaks = 1);

		int peaks(){return d_peaks;};
		void setNumPeaks(int n);

		void enablePeakCurves(bool on){generate_peak_curves = on;};
		void setPeakCurvesColor(int colorIndex){d_peaks_color = colorIndex;};

		static QString generateFormula(int order, PeakProfile profile);
		static QStringList generateParameterList(int order);
		static QStringList generateExplanationList(int order);

	private:
		QString logFitInfo(double *par, int iterations, int status, const QString& plotName);
		void generateFitCurve(double *par);
		static QString peakFormula(int peakIndex, PeakProfile profile);
		//! Inserts a peak function curve into the plot 
		void insertPeakFunctionCurve(double *x, double *y, int peak);
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
		LorentzFit(ApplicationWindow *parent, Graph *g, const QString& curveTitle, double start, double end);

	private:
		void init();
};

class GaussFit : public MultiPeakFit
{
	Q_OBJECT

	public:
		GaussFit(ApplicationWindow *parent, Graph *g);
		GaussFit(ApplicationWindow *parent, Graph *g, const QString& curveTitle);
		GaussFit(ApplicationWindow *parent, Graph *g, const QString& curveTitle, double start, double end);

	private:
		void init();
};

class GaussAmpFit : public Fit
{
	Q_OBJECT

	public:
		GaussAmpFit(ApplicationWindow *parent, Graph *g);
		GaussAmpFit(ApplicationWindow *parent, Graph *g, const QString& curveTitle);
		GaussAmpFit(ApplicationWindow *parent, Graph *g, const QString& curveTitle, double start, double end);

	private:
		void init();
		void calculateFitCurveData(double *par, double *X, double *Y);
};
#endif
