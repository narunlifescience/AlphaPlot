/***************************************************************************
    File                 : FunctionCurve.cpp
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
#include "FunctionCurve.h"
#include <qpainter.h>

FunctionCurve::FunctionCurve(const char *name):
	QwtPlotCurve(name)
{
	d_variable = "x";
}

FunctionCurve::FunctionCurve(const FunctionType& t, const char *name):
	QwtPlotCurve(name),
	d_type(t)
{
	d_variable = "x";
}

void FunctionCurve::setRange(double from, double to)
{
	range_from = from;
	range_to = to;
}

void FunctionCurve::copy(FunctionCurve* f)
{
	d_type = f->functionType();
	d_variable = f->variable();
	d_formulas = f->formulas();
	range_from = f->startRange();
	range_to = f->endRange();
}

int FunctionCurve::rtti () const
{
	return RTTI;
}

QString FunctionCurve::saveToString()
{
	QString s = "FunctionCurve\t";
	s += QString::number(d_type) + ",";
	s += title().text() + ",";
	s += d_formulas.join(",") + "," + d_variable + ",";
	s += QString::number(range_from,'g',15)+",";
	s += QString::number(range_to,'g',15)+"\t";
	s += QString::number(dataSize())+"\t\t\t";	
	//the 2 last tabs are legacy code, kept for compatibility with old project files
	return s;				
}

QString FunctionCurve::legend()
{
	QString label = title().text() + ": ";
	if (d_type == Normal)
		label += d_formulas[0];
	else if (d_type == Parametric)
	{
		label += "X(" + d_variable + ")=" + d_formulas[0];
		label += ", Y(" + d_variable + ")=" + d_formulas[1];
	}
	else if (d_type == Polar)
	{
		label += "R(" + d_variable + ")=" + d_formulas[0];
		label += ", Theta(" + d_variable + ")=" + d_formulas[1];
	}
	return label;
}
