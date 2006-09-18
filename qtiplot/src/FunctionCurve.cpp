/***************************************************************************
    File                 : FunctionCurve.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
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
}

FunctionCurve::FunctionCurve(const FunctionType& t, const char *name):
    QwtPlotCurve(name),
	d_type(t)
{
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
