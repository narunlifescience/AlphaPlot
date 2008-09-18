/***************************************************************************
    File                 : FunctionCurve.cpp
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Benkert
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
#include "Layer.h"
#include "core/MyParser.h"

#include <QMessageBox>

FunctionCurve::FunctionCurve(const char *name):
	PlotCurve(name)
{
	m_variable = "x";
	setType(Layer::Function);
}

FunctionCurve::FunctionCurve(const FunctionType& t, const char *name):
	PlotCurve(name),
	m_function_type(t)
{
	m_variable = "x";
	setType(Layer::Function);
}

void FunctionCurve::setRange(double from, double to)
{
	m_from = from;
	m_to = to;
}

void FunctionCurve::copy(FunctionCurve* f)
{
	m_function_type = f->functionType();
	m_variable = f->variable();
	m_formulas = f->formulas();
	m_from = f->startRange();
	m_to = f->endRange();
}

QString FunctionCurve::saveToString()
{
	QString s = "FunctionCurve\t";
	s += QString::number(m_function_type) + ",";
	s += title().text() + ",";
	s += m_formulas.join(",") + "," + m_variable + ",";
	s += QString::number(m_from,'g',15)+",";
	s += QString::number(m_to,'g',15)+"\t";
	s += QString::number(dataSize())+"\t\t\t";
	//the 2 last tabs are legacy code, kept for compatibility with old project files
	return s;
}

QString FunctionCurve::legend()
{
	QString label = title().text() + ": ";
	if (m_function_type == Normal)
		label += m_formulas[0];
	else if (m_function_type == Parametric)
	{
		label += "X(" + m_variable + ")=" + m_formulas[0];
		label += ", Y(" + m_variable + ")=" + m_formulas[1];
	}
	else if (m_function_type == Polar)
	{
		label += "R(" + m_variable + ")=" + m_formulas[0];
		label += ", Theta(" + m_variable + ")=" + m_formulas[1];
	}
	return label;
}

void FunctionCurve::loadData(int points)
{
    if (!points)
        points = dataSize();

    double X[points], Y[points];
    double step = (m_to - m_from)/(double)(points - 1);
    bool error = false;

	if (m_function_type == Normal)
	{
		MyParser parser;
		double x;
		try
		{
			parser.DefineVar(m_variable.toAscii().constData(), &x);
			parser.SetExpr(m_formulas[0].toAscii().constData());

			X[0] = m_from; x = m_from; Y[0]=parser.Eval();
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
	else if (m_function_type == Parametric || m_function_type == Polar)
	{
		QStringList aux = m_formulas;
		MyParser xparser;
		MyParser yparser;
		double par;
		if (m_function_type == Polar)
		{
			QString swap=aux[0];
			aux[0]="("+swap+")*cos("+aux[1]+")";
			aux[1]="("+swap+")*sin("+aux[1]+")";
		}
		try
		{
			xparser.DefineVar(m_variable.toAscii().constData(), &par);
			yparser.DefineVar(m_variable.toAscii().constData(), &par);
			xparser.SetExpr(aux[0].toAscii().constData());
			yparser.SetExpr(aux[1].toAscii().constData());
			par = m_from;
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
