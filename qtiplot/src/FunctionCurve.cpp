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
#include "MyParser.h"

#include <QMessageBox>

FunctionCurve::FunctionCurve(const char *name):
	PlotCurve(name)
{
	d_variable = "x";
	setType(Graph::Function);
}

FunctionCurve::FunctionCurve(const FunctionType& t, const char *name):
	PlotCurve(name),
	d_function_type(t)
{
	d_variable = "x";
	setType(Graph::Function);
}

void FunctionCurve::setRange(double from, double to)
{
	d_from = from;
	d_to = to;
}

void FunctionCurve::copy(FunctionCurve* f)
{
	d_function_type = f->functionType();
	d_variable = f->variable();
	d_formulas = f->formulas();
	d_from = f->startRange();
	d_to = f->endRange();
}

QString FunctionCurve::saveToString()
{
	QString s = "FunctionCurve\t";
	s += QString::number(d_function_type) + ",";
	s += title().text() + ",";
	s += d_formulas.join(",") + "," + d_variable + ",";
	s += QString::number(d_from,'g',15)+",";
	s += QString::number(d_to,'g',15)+"\t";
	s += QString::number(dataSize())+"\t\t\t";
	//the 2 last tabs are legacy code, kept for compatibility with old project files
	return s;
}

QString FunctionCurve::legend()
{
	QString label = title().text() + ": ";
	if (d_function_type == Normal)
		label += d_formulas[0];
	else if (d_function_type == Parametric)
	{
		label += "X(" + d_variable + ")=" + d_formulas[0];
		label += ", Y(" + d_variable + ")=" + d_formulas[1];
	}
	else if (d_function_type == Polar)
	{
		label += "R(" + d_variable + ")=" + d_formulas[0];
		label += ", Theta(" + d_variable + ")=" + d_formulas[1];
	}
	return label;
}

void FunctionCurve::loadData(int points)
{
    if (!points)
        points = dataSize();

    double X[points], Y[points];
    double step = (d_to - d_from)/(double)(points - 1);
    bool error = false;

	if (d_function_type == Normal)
	{
		MyParser parser;
		double x;
		try
		{
			parser.DefineVar(d_variable.ascii(), &x);
			parser.SetExpr(d_formulas[0].ascii());

			X[0] = d_from; x = d_from; Y[0]=parser.Eval();
			for (int i = 1; i<points; i++ )
			{
				x += step;
				X[i] = x;
				Y[i] = parser.Eval();
			}
		}
		catch(mu::ParserError &)
		{
			error = true;
		}
	}
	else if (d_function_type == Parametric || d_function_type == Polar)
	{
		QStringList aux = d_formulas;
		MyParser xparser;
		MyParser yparser;
		double par;
		if (d_function_type == Polar)
		{
			QString swap=aux[0];
			aux[0]="("+swap+")*cos("+aux[1]+")";
			aux[1]="("+swap+")*sin("+aux[1]+")";
		}
		try
		{
			xparser.DefineVar(d_variable.ascii(), &par);
			yparser.DefineVar(d_variable.ascii(), &par);
			xparser.SetExpr(aux[0].ascii());
			yparser.SetExpr(aux[1].ascii());
			par = d_from;
			for (int i = 0; i<points; i++ )
			{
				X[i]=xparser.Eval();
				Y[i]=yparser.Eval();
				par+=step;
			}
		}
		catch(mu::ParserError &)
		{
			error = true;
		}
	}

	if (error)
		return;

	setData(X, Y, points);
}
