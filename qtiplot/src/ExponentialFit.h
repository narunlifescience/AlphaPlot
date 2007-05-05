/***************************************************************************
    File                 : fitclasses.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : Exponential fit classes
                           
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
#ifndef EXPONENTIALFIT_H
#define EXPONENTIALFIT_H

#include "Fit.h"

class ExponentialFit : public Fit
{
	Q_OBJECT

	public:
		ExponentialFit(ApplicationWindow *parent, Graph *g,  bool expGrowth = false);
		ExponentialFit(ApplicationWindow *parent, Graph *g, const QString& curveTitle, bool expGrowth = false);
		ExponentialFit(ApplicationWindow *parent, Graph *g, const QString& curveTitle, 
				double start, double end, bool expGrowth = false);

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
		TwoExpFit(ApplicationWindow *parent, Graph *g, const QString& curveTitle, double start, double end);

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
		ThreeExpFit(ApplicationWindow *parent, Graph *g, const QString& curveTitle, double start, double end);

	private:
		void init();
		void storeCustomFitResults(double *par);
		void calculateFitCurveData(double *par, double *X, double *Y);
};
#endif
