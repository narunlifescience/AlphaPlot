/***************************************************************************
    File                 : Fit.cpp
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Ion Vasilief
    Email (use @ for *)  : ion_vasilief*yahoo.fr
    Description          : Abstract base class for data analysis operations

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
#include "Filter.h"
#include "Legend.h"
#include "ColorBox.h"
#include "Table.h"
#include "FunctionCurve.h"
#include "PlotCurve.h"
#include "core/column/Column.h"

#include <QApplication>
#include <QMessageBox>
#include <QLocale>

#include <gsl/gsl_sort.h>

#include <algorithm>
using namespace std;

Filter::Filter( ApplicationWindow *parent, Graph *g, QString name)
	: QObject(parent)
{
	QObject::setObjectName(name);
	init();
	d_graph = g;
}

Filter::Filter( ApplicationWindow *parent, Table *t, QString name)
	: QObject(parent)
{
	QObject::setObjectName(name);
	init();
	d_table = t;
}

void Filter::init()
{
	d_n = 0;
	d_curveColorIndex = 1;
	d_tolerance = 1e-4;
	d_points = 100;
	d_max_iterations = 1000;
	d_curve = 0;
	d_prec = ((ApplicationWindow *)parent())->fit_output_precision;
	d_init_err = false;
    d_sort_data = false;
    d_min_points = 2;
    d_explanation = objectName();
    d_graph = 0;
    d_table = 0;
}

void Filter::setInterval(double from, double to)
{
	if (!d_curve)
	{
		QMessageBox::critical((ApplicationWindow *)parent(), tr("SciDAVis") + " - " + tr("Error"),
				tr("Please assign a curve first!"));
		return;
	}
	setDataFromCurve (d_curve->title().text(), from, to);
}

void Filter::setDataCurve(int curve, double start, double end)
{
  if (start > end) qSwap(start, end);
    
  if (d_n > 0)
    {//delete previousely allocated memory
      delete[] d_x;
      delete[] d_y;
    }

  d_init_err = false;
  d_curve = d_graph->curve(curve);
  if (d_sort_data)
    d_n = sortedCurveData(d_curve, start, end, &d_x, &d_y);
  else
    d_n = curveData(d_curve, start, end, &d_x, &d_y);

  if (!isDataAcceptable()) {
    d_init_err = true;
    return;
  }

  // ensure range is within data range
  if (d_n>0)
    {
      d_from = max(start, *min_element(d_x,d_x+d_n));
      d_to = min(end, *max_element(d_x,d_x+d_n));
    }
}

bool Filter::isDataAcceptable() {
	if (d_n < d_min_points) {
		QMessageBox::critical((ApplicationWindow *)parent(), tr("SciDAVis") + " - " + tr("Error"),
				tr("You need at least %1 points in order to perform this operation!").arg(d_min_points));
		return false;
	}
	return true;
}

int Filter::curveIndex(const QString& curveTitle, Graph *g)
{
	if (curveTitle.isEmpty())
	{
		QMessageBox::critical((ApplicationWindow *)parent(), tr("Filter Error"),
				tr("Please enter a valid curve name!"));
		d_init_err = true;
		return -1;
	}

	if (g)
		d_graph = g;

	if (!d_graph)
	{
		d_init_err = true;
		return -1;
	}

	return d_graph->curveIndex(curveTitle);
}

bool Filter::setDataFromCurve(const QString& curveTitle, Graph *g)
{
	int index = curveIndex(curveTitle, g);
	if (index < 0)
	{
		d_init_err = true;
		return false;
	}

  	d_graph->range(index, &d_from, &d_to);
    setDataCurve(index, d_from, d_to);
	return true;
}

bool Filter::setDataFromCurve(const QString& curveTitle, double from, double to, Graph *g)
{
	int index = curveIndex(curveTitle, g);
	if (index < 0)
	{
		d_init_err = true;
		return false;
	}

	setDataCurve(index, from, to);
	return true;
}

void Filter::setColor(const QString& colorName)
{
    QColor c = QColor(colorName);
    if (colorName == "green")
        c = QColor(Qt::green);
    else if (colorName == "darkYellow")
        c = QColor(Qt::darkYellow);
    if (!ColorBox::isValidColor(c))
    {
        QMessageBox::critical((ApplicationWindow *)parent(), tr("Color Name Error"),
				tr("The color name '%1' is not valid, a default color (red) will be used instead!").arg(colorName));
        d_curveColorIndex = 1;
        return;
    }

	d_curveColorIndex = ColorBox::colorIndex(c);
}

void Filter::showLegend()
{
	Legend* mrk = d_graph->newLegend(legendInfo());
	if (d_graph->hasLegend())
	{
		Legend* legend = d_graph->legend();
		QPoint p = legend->rect().bottomLeft();
		mrk->setOrigin(QPoint(p.x(), p.y()+20));
	}
	d_graph->replot();
}

bool Filter::run()
{
	if (d_init_err)
		return false;

	if (d_n < 0)
	{
		QMessageBox::critical((ApplicationWindow *)parent(), tr("SciDAVis") + " - " + tr("Error"),
				tr("You didn't specify a valid data set for this operation!"));
		return false;
	}

	QApplication::setOverrideCursor(Qt::WaitCursor);

    output();//data analysis and output
    ((ApplicationWindow *)parent())->updateLog(logInfo());

	QApplication::restoreOverrideCursor();
    return true;
}

void Filter::output()
{
    double *X = new double[d_points];
    double *Y = new double[d_points];

    //do the data analysis
    calculateOutputData(X, Y);

	addResultCurve(X, Y);
}

int Filter::sortedCurveData(QwtPlotCurve *c, double start, double end, double **x, double **y)
{
    if (!c || c->rtti() != QwtPlotItem::Rtti_PlotCurve)
        return 0;
    
    // start/end finding only works on nondecreasing data, so sort first
    int datasize = c->dataSize();
    double *xtemp = new double[datasize];
    for (int i = 0; i < datasize; i++) {
        xtemp[i] = c->x(i);
    }
    size_t *p = new size_t[datasize];
    gsl_sort_index(p, xtemp, 1, datasize);
    delete[] xtemp;
    
    // find indices that, when permuted by the sort result, give start and end
    int i_start, i_end;
    for (i_start = 0; i_start < datasize; i_start++)
  	    if (c->x(p[i_start]) >= start)
          break;
    for (i_end = datasize-1; i_end >= 0; i_end--)
  	    if (c->x(p[i_end]) <= end)
          break;
    
    // make result arrays
    int n = i_end - i_start + 1;
    (*x) = new double[n];
    (*y) = new double[n];
    for (int j = 0, i = i_start; i <= i_end; i++, j++) {
        (*x)[j] = c->x(p[i]);
        (*y)[j] = c->y(p[i]);
    }
    delete[] p;
    return n;
}

int Filter::curveData(QwtPlotCurve *c, double start, double end, double **x, double **y)
{
    if (!c || c->rtti() != QwtPlotItem::Rtti_PlotCurve)
        return 0;

    int datasize = c->dataSize();
    int i_start = 0, i_end = c->dataSize();
    for (i_start = 0; i_start < datasize; i_start++)
  	    if (c->x(i_start) >= start)
          break;
    for (i_end = datasize-1; i_end >= 0; i_end--)
  	    if (c->x(i_end) <= end)
          break;

    int n = i_end - i_start + 1;
    (*x) = new double[n];
    (*y) = new double[n];

    for (int j = 0, i = i_start; i <= i_end; i++, j++) {
        (*x)[j] = c->x(i);
        (*y)[j] = c->y(i);
    }
    return n;
}

QwtPlotCurve* Filter::addResultCurve(double *x, double *y)
{
    ApplicationWindow *app = (ApplicationWindow *)parent();
    const QString tableName = app->generateUniqueName(this->objectName());
	Column *xCol = new Column(tr("1", "filter table x column name"), SciDAVis::Numeric);
	Column *yCol = new Column(tr("2", "filter table y column name"), SciDAVis::Numeric);
	xCol->setPlotDesignation(SciDAVis::X);
	yCol->setPlotDesignation(SciDAVis::Y);
	for (int i=0; i<d_points; i++)
	{
		xCol->setValueAt(i, x[i]);
		yCol->setValueAt(i, y[i]);
	}
	// first set the values, then add the columns to the table, otherwise, we generate too many undo commands
    Table *t = app->newHiddenTable(tableName, d_explanation + " " + tr("of") + " " + d_curve->title().text(), 
		QList<Column *>() << xCol << yCol);

	DataCurve *c = new DataCurve(t, tableName + "_" + xCol->name(), tableName + "_" + yCol->name());
	c->setData(x, y, d_points);
    c->setPen(QPen(ColorBox::color(d_curveColorIndex), 1));
	d_graph->insertPlotItem(c, Graph::Line);
    d_graph->updatePlot();

    delete[] x;
	delete[] y;
	return (QwtPlotCurve*)c;
}

Filter::~Filter()
{
	if (d_n > 0)
	{//delete the memory allocated for the data
		delete[] d_x;
		delete[] d_y;
	}
}
