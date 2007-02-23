/***************************************************************************
    File                 : Fitter.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief
    Email                : ion_vasilief@yahoo.fr
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
#include "colorBox.h"

#include <QApplication>
#include <QMessageBox>

Filter::Filter( ApplicationWindow *parent, Graph *g, const char * name)
: QObject( parent, name),
	d_graph(g)
{
	d_n = 0;
	d_curveColorIndex = 1;
	d_tolerance = 1e-4;
	d_points = 100;
	d_max_iterations = 1000;
	d_curve = 0;
	d_prec = parent->fit_output_precision;
	d_init_err = false;
    d_sort_data = false;
    d_min_points = 2;
    d_explanation = QString(name);
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

void Filter::setDataFromCurve(int curve, double start, double end)
{ 
	if (d_n > 0)
	{//delete previousely allocated memory
		delete[] d_x;
		delete[] d_y;
	}

	d_init_err = false;
	d_curve = d_graph->curve(curve);
    if (d_sort_data)
        d_n = d_graph->sortedCurveData(curve, start, end, &d_x, &d_y);
    else
    	d_n = d_graph->curveData(curve, start, end, &d_x, &d_y);

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
    setDataFromCurve(index, d_from, d_to);
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

	setDataFromCurve(index, from, to);
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
	if (!d_graph || d_init_err)
		return false;

	if (d_n < 0)
	{
		QMessageBox::critical((ApplicationWindow *)parent(), tr("QtiPlot") + " - " + tr("Error"),
				tr("You didn't specify a valid data set for this operation!"));
		return false;
	}

	QApplication::setOverrideCursor(Qt::WaitCursor);

    addResultCurve();//data analysis and output
    ((ApplicationWindow *)parent())->updateLog(logInfo());

	QApplication::restoreOverrideCursor();
    return true;
}

void Filter::addResultCurve()
{
    double *X = new double[d_points];
    double *Y = new double[d_points];

    //do the data analysis
    calculateOutputData(X, Y);

	d_graph->addResultCurve(d_points, X, Y, d_curveColorIndex,
			((ApplicationWindow *)parent())->generateUniqueName(QString(this->name())),
            d_explanation + " " + tr("of") + " " + d_curve->title().text());
}

Filter::~Filter()
{
	if (d_n > 0)
	{//delete the memory allocated for the data
		delete[] d_x;
		delete[] d_y;
	}
}



