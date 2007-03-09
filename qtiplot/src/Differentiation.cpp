/***************************************************************************
    File                 : Differentiation.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Ion Vasilief
    Email (use @ for *)  : ion_vasilief*yahoo.fr
    Description          : Numerical differentiation of data sets

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
#include "Differentiation.h"
#include "MultiLayer.h"
#include "LegendMarker.h"

Differentiation::Differentiation(ApplicationWindow *parent, Graph *g)
: Filter(parent, g)
{
	init();
}

Differentiation::Differentiation(ApplicationWindow *parent, Graph *g, const QString& curveTitle)
: Filter(parent, g)
{
	init();
	setDataFromCurve(curveTitle);
}

Differentiation::Differentiation(ApplicationWindow *parent, Graph *g, const QString& curveTitle, double start, double end)
: Filter(parent, g)
{
	init();
	setDataFromCurve(curveTitle, start, end);
}

void Differentiation::init()
{
	setName(tr("Derivative"));
    d_min_points = 4;
}

void Differentiation::output()
{
    double *result = new double[d_n-1];
	for (int i = 1; i < d_n-1; i++)
		result[i]=0.5*((d_y[i+1]-d_y[i])/(d_x[i+1]-d_x[i]) + (d_y[i]-d_y[i-1])/(d_x[i]-d_x[i-1]));

    ApplicationWindow *app = (ApplicationWindow *)parent();
    QString tableName = app->generateUniqueName(QString(name()));
    QString curveTitle = d_curve->title().text();
    Table *t = app->newHiddenTable(tableName, tr("Derivative") + " " + tr("of")  + " " + curveTitle, d_n-2, 2);
	for (int i = 1; i < d_n-1; i++)
	{
		t->setText(i, 0, QString::number(d_x[i]));
		t->setText(i, 1, QString::number(result[i]));
	}
    delete[] result;

    MultiLayer *ml = app->newGraph(tr("Plot")+tr("Derivative"));
    ml->activeGraph()->insertCurve(t, tableName + "_2", 0);
    LegendMarker *l = ml->activeGraph()->legend();
    l->setText("\\c{1}" + tr("Derivative") + " " + tr("of") + " " + curveTitle);
}
