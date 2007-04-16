/***************************************************************************
    File                 : FunctionCurve.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : Function curve class

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
#ifndef FUNCTIONCURVE_H
#define FUNCTIONCURVE_H

#include "PlotCurve.h"

// Function curve class
class FunctionCurve: public PlotCurve
{
public:
	enum FunctionType{Normal = 0, Parametric = 1, Polar = 2};

	FunctionCurve(const FunctionType& t, const char *name=0);
	FunctionCurve(const char *name=0);

	double startRange(){return d_from;};
	double endRange(){return d_to;};
	void setRange(double from, double to);

	QStringList formulas(){return d_formulas;};
	void setFormulas(const QStringList& lst){d_formulas = lst;};

	//! Provided for convenience when dealing with normal functions
	void setFormula(const QString& s){d_formulas = QStringList() << s;};

	QString variable(){return d_variable;};
	void setVariable(const QString& s){d_variable = s;};

	FunctionType functionType(){return d_function_type;};
	void setFunctionType(const FunctionType& t){d_function_type = t;};

	void copy(FunctionCurve *f);

	//! Returns a string used when saving to a project file
	QString saveToString();

	//! Returns a string that can be displayed in a plot legend
	QString legend();

	void loadData(int points = 0);

private:
	FunctionType d_function_type;
	QString d_variable;
	QStringList d_formulas;
	double d_from, d_to;
};

#endif

