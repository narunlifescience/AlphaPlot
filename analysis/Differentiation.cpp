/***************************************************************************
    File                 : Differentiation.cpp
    Project              : SciDAVis
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
#include "graph/Graph.h"
#include "graph/TextEnrichment.h"
#include "table/Table.h"

#include <QLocale>

Differentiation::Differentiation(ApplicationWindow *parent, Layer *layer)
: Filter(parent, layer)
{
	init();
}

Differentiation::Differentiation(ApplicationWindow *parent, Layer *layer, const QString& curveTitle)
: Filter(parent, layer)
{
	init();
	setDataFromCurve(curveTitle);
}

Differentiation::Differentiation(ApplicationWindow *parent, Layer *layer, const QString& curveTitle, double start, double end)
: Filter(parent, layer)
{
	init();
	setDataFromCurve(curveTitle, start, end);
}

void Differentiation::init()
{
	setName(tr("Derivative"));
    m_min_points = 4;
}

void Differentiation::output()
{
    double *result = new double[m_n-1];
	for (int i = 1; i < m_n-1; i++)
		result[i]=0.5*((m_y[i+1]-m_y[i])/(m_x[i+1]-m_x[i]) + (m_y[i]-m_y[i-1])/(m_x[i]-m_x[i-1]));

    ApplicationWindow *app = (ApplicationWindow *)parent();
    QString tableName = app->generateUniqueName(QString(name()));
    QString curveTitle = m_curve->title().text();
    Table *t = app->newHiddenTable(tableName, tr("Derivative") + " " + tr("of","Derivative of")  + " " + curveTitle, m_n-2, 2);
	for (int i = 1; i < m_n-1; i++)
	{
		t->setText(i-1, 0, QLocale().toString(m_x[i], 'g', app->m_decimal_digits));
		t->setText(i-1, 1, QLocale().toString(result[i], 'g', app->m_decimal_digits));
	}
    delete[] result;

    Graph *graph = app->newGraph(tr("Plot")+tr("Derivative"));
    graph->activeLayer()->insertCurve(t, tableName + "_2", 0);
    TextEnrichment *l = graph->activeLayer()->legend();
    l->setText("\\c{1}" + tr("Derivative") + " " + tr("of","Derivative of") + " " + curveTitle);
}
