/***************************************************************************
    File                 : Fit.cpp
    Project              : QtiPlot
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
#include "LegendMarker.h"
#include "ColorBox.h"
#include "Table.h"

#include <QApplication>
#include <QMessageBox>

#include <gsl/gsl_sort.h>

Filter::Filter( ApplicationWindow *parent, Graph *g, const char * name)
: QObject( parent, name)
{
	init();
	d_graph = g;
}

Filter::Filter( ApplicationWindow *parent, Table *t, const char * name)
: QObject( parent, name)
{
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
    d_explanation = QString(name());
    d_graph = 0;
    d_table = 0;
}

void Filter::setInterval(double from, double to)
{
	if (!d_curve)
	{
		QMessageBox::critical((ApplicationWindow *)parent(), tr("QtiPlot") + " - " + tr("Error"),
				tr("Please assign a curve first!"));
		return;
	}
	setDataFromCurve (d_curve->title().text(), from, to);
}

void Filter::setDataCurve(int curve, double start, double end)
{
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

	if (d_n == -1)
	{
		QMessageBox::critical((ApplicationWindow *)parent(), tr("QtiPlot") + " - " + tr("Error"),
				tr("Several data points have the same x value causing divisions by zero, operation aborted!"));
		d_init_err = true;
        return;
	}
    else if (d_n < d_min_points)
	{
		QMessageBox::critical((ApplicationWindow *)parent(), tr("QtiPlot") + " - " + tr("Error"),
				tr("You need at least %1 points in order to perform this operation!").arg(d_min_points));
		d_init_err = true;
        return;
	}

    d_from = start;
    d_to = end;
}

int Filter::curveIndex(const QString& curveTitle, Graph *g)
{
	if (curveTitle.isEmpty())
	{
		QMessageBox::critical((ApplicationWindow *)parent(), tr("QtiPlot - Filter Error"),
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

	return d_graph->curveIndex(curveTitle);;
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
        QMessageBox::critical((ApplicationWindow *)parent(), tr("QtiPlot - Color Name Error"),
				tr("The color name '%1' is not valid, a default color (red) will be used instead!").arg(colorName));
        d_curveColorIndex = 1;
        return;
    }

	d_curveColorIndex = ColorBox::colorIndex(c);
}

void Filter::showLegend()
{
	LegendMarker* mrk = d_graph->newLegend(legendInfo());
	if (d_graph->hasLegend())
	{
		LegendMarker* legend = d_graph->legend();
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
		QMessageBox::critical((ApplicationWindow *)parent(), tr("QtiPlot") + " - " + tr("Error"),
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

    int i_start = 0, i_end = c->dataSize();
    for (int i = 0; i < i_end; i++)
  	    if (c->x(i) >= start)
        {
  	      i_start = i;
          break;
        }
    for (int i = i_end-1; i >= 0; i--)
  	    if (c->x(i) <= end)
        {
  	      i_end = i;
          break;
        }
    int n = i_end - i_start + 1;
    (*x) = new double[n];
    (*y) = new double[n];
    double *xtemp = new double[n];
    double *ytemp = new double[n];

	double pr_x;
  	int j=0;
    for (int i = i_start; i <= i_end; i++)
    {
        xtemp[j] = c->x(i);
        if (xtemp[j] == pr_x)
        {
            delete (*x);
            delete (*y);
            return -1;//this kind of data causes division by zero in GSL interpolation routines
        }
        pr_x = xtemp[j];
        ytemp[j++] = c->y(i);
    }
    size_t *p = new size_t[n];
    gsl_sort_index(p, xtemp, 1, n);
    for (int i=0; i<n; i++)
    {
        (*x)[i] = xtemp[p[i]];
  	    (*y)[i] = ytemp[p[i]];
    }
    delete[] xtemp;
    delete[] ytemp;
    delete[] p;
    return n;
}

int Filter::curveData(QwtPlotCurve *c, double start, double end, double **x, double **y)
{
    if (!c || c->rtti() != QwtPlotItem::Rtti_PlotCurve)
        return 0;

    int i_start = 0, i_end = c->dataSize();
    for (int i = 0; i < i_end; i++)
  	    if (c->x(i) >= start)
        {
  	      i_start = i;
          break;
        }
    for (int i = i_end-1; i >= 0; i--)
  	    if (c->x(i) <= end)
        {
  	      i_end = i;
          break;
        }
    int n = i_end - i_start + 1;
    (*x) = new double[n];
    (*y) = new double[n];

    double pr_x;
    int j=0;
    for (int i = i_start; i <= i_end; i++)
    {
        (*x)[j] = c->x(i);
        if ((*x)[j] == pr_x)
        {
            delete (*x);
            delete (*y);
            return -1;//this kind of data causes division by zero in GSL interpolation routines
        }
        pr_x = (*x)[j];
        (*y)[j++] = c->y(i);
    }
    return n;
}

void Filter::addResultCurve(double *x, double *y)
{
    ApplicationWindow *app = (ApplicationWindow *)parent();
    const QString tableName = app->generateUniqueName(QString(this->name()));
	const QString label = tableName + "_2";
	QwtPlotCurve *c = new QwtPlotCurve(label);
	c->setData(x, y, d_points);
    c->setPen(QPen(ColorBox::color(d_curveColorIndex), 1));
	d_graph->insertPlotItem(c, tableName + "_1(X)," + label + "(Y)", Graph::Line);
    d_graph->updatePlot();

    Table *t = app->newHiddenTable(tableName, d_explanation + " " + tr("of") + " " + d_curve->title().text(), d_points, 2);
	for (int i=0; i<d_points; i++)
	{
		t->setText(i, 0, QString::number(x[i], 'g', 15));
		t->setText(i, 1, QString::number(y[i], 'g', 15));
	}

    delete[] x;
	delete[] y;
}

Filter::~Filter()
{
	if (d_n > 0)
	{//delete the memory allocated for the data
		delete[] d_x;
		delete[] d_y;
	}
}



