/***************************************************************************
    File                 : Integration.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Ion Vasilief
    Email (use @ for *)  : ion_vasilief*yahoo.fr
    Description          : Numerical integration/differetiation of data sets

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
#include "IntDiff.h"
#include "nrutil.h"
#include "multilayer.h"
#include "LegendMarker.h"

#include <QApplication>
#include <QMessageBox>
#include <QDateTime>

#include <gsl/gsl_spline.h>
#include <gsl/gsl_interp.h>
#include <gsl/gsl_vector.h>

Integration::Integration(ApplicationWindow *parent, Graph *g)
: Filter(parent, g)
{
	init();
}

Integration::Integration(ApplicationWindow *parent, Graph *g, const QString& curveTitle)
: Filter(parent, g)
{
	init();
	setDataFromCurve(curveTitle);
}

Integration::Integration(ApplicationWindow *parent, Graph *g, const QString& curveTitle, double start, double end)
: Filter(parent, g)
{
	init();
	setDataFromCurve(curveTitle, start, end);
}

void Integration::init()
{
	setName(tr("Integration"));
	d_method = 1;
    d_max_iterations = 40;
    d_sort_data = true;
}

QString Integration::logInfo()
{
	// Do the interpolation, use GSL libraries for that
	gsl_interp_accel *acc = gsl_interp_accel_alloc();
	const gsl_interp_type *method;
	// The method for interpolation is chosen based on the number of points
	if(d_n>3)
		method=gsl_interp_linear;
	else if(d_n>4)
		method=gsl_interp_cspline;
	else if(d_n>5)
		method=gsl_interp_akima;

	// If we have enough points use GSL libraries for interpolation, else use the polint algorithm
	gsl_spline *interp ;
	if(d_n>3)
	{
		interp = gsl_spline_alloc (method, d_n);
		gsl_spline_init (interp, d_x, d_y, d_n);
	}

	// Using Numerical Recipes
	// This is Romberg Integration method
	// This method uses the Nevilles' algorithm for interpollation;
	double yup, ylow;
	double xx,tnm,sum,del,ss,dss,error,tsum;
	if(d_n > 3)
	{
		yup = gsl_spline_eval (interp, d_to, acc);
		ylow = gsl_spline_eval (interp, d_from, acc);
	}
	else if (d_n<=3)
	{
		polint(d_x,d_y,d_n,d_to,&yup,&dss);
		polint(d_x,d_y,d_n,d_from,&ylow,&dss);
	}

	double *S = new double[d_max_iterations];
	double *h = new double[d_max_iterations];
	int j,it,l;
	bool success = false;
	h[0]=1.0;
	for(j=0; j < d_max_iterations; j++)
	{//Trapezoid Rule
		if(j==0)
			S[0]=0.5*(d_to-d_from)*(ylow+yup);
		else
		{
			for(it=1,l=1;l<j-1;l++)it<<=1;
			tnm=it;
			del=(d_to-d_from)/tnm;
			xx=d_from+0.5*del;
			for(sum=0.0,l=1;l<=it;l++)
			{
				if(d_n>3)
					sum+=gsl_spline_eval (interp,xx, acc);
				else if(d_n<=3)
				{
					polint(d_x,d_y,d_n,xx,&tsum,&dss);
					sum+=tsum;
				}
				xx+=del;
			}
			S[j]=0.5*(S[j-1]+(d_to-d_from)*sum/tnm);

		}
		if(j>=d_method)
		{
			polint(&h[j-d_method],&S[j-d_method],d_method,0,&ss,&dss);
			S[j]=ss;
		}
		h[j+1]=0.25*h[j];
		S[j+1]=S[j];
		error=fabs(S[j]-S[j-1]);
		if(error<=d_tolerance) success = true;
		if(success) break;
	}

	QString logInfo = "[" + QDateTime::currentDateTime().toString(Qt::LocalDate) + "\t" + tr("Plot")+ ": ''" + d_graph->parentPlotName() + "'']\n";
	logInfo += "\n" + tr("Numerical integration of") + ": " + d_curve->title().text() + " " + tr("using a %1 order method").arg(d_method)+"\n";
	if(success)
		logInfo += tr("Iterations") + ": " + QString::number(j)+"\n";
	if(!success)
		logInfo += tr("Iterations") + ": " + QString::number(j-1)+"\n";

	logInfo += tr("Tolerance") + "(" + tr("max") + " = " + QString::number(d_tolerance)+"): " + QString::number(error)+ "\n";
	logInfo += tr("Points") + ": "+QString::number(d_n) + " " + tr("from") + " x = " +QString::number(d_from) + " ";
    logInfo += tr("to") + " x = " + QString::number(d_to) + "\n";

    // using GSL to find maximum value of data set
    gsl_vector *aux = gsl_vector_alloc(d_n);
    for(int i=0; i < d_n; i++)
        gsl_vector_set (aux, i, d_y[i]);
    int maxID=gsl_vector_max_index (aux);
    gsl_vector_free (aux);

    logInfo += tr("Peak at") + " x = " + QString::number(d_x[maxID])+"\t";
	logInfo += "y = " + QString::number(d_y[maxID])+"\n";

	logInfo += tr("Area") + "=";
	if(success)
		logInfo += QString::number(S[j]);
	if(!success)
		logInfo += QString::number(S[j-1]);
	logInfo += "\n-------------------------------------------------------------\n";

	if(d_n>3)
		gsl_spline_free (interp);

	gsl_interp_accel_free (acc);
    delete[] S;
    delete[] h;
    return logInfo;
}

void Integration::setMethod(int n)
{
if (n < 1 || n > 5)
    {
        QMessageBox::critical((ApplicationWindow *)parent(), tr("QtiPlot - Error"),
        tr("Unknown integration method. Valid values must be in the range: 1 (Trapezoidal Method) to 5."));
        return;
    }

d_method = n;
}

/*****************************************************************************
 *
 * Class Differentiation
 *
 *****************************************************************************/

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

	QString text = "1\t2\n";
	for (int i = 1; i < d_n-1; i++)
	{
		text+=QString::number(d_x[i]);
		text+="\t";
		text+=QString::number(result[i]);
		text+="\n";
	}

    QString curveTitle = d_curve->title().text();
    ApplicationWindow *app = (ApplicationWindow *)parent();
    QString tableName = app->generateUniqueName(QString(name()));
    Table *t = app->newHiddenTable(tableName, tr("Derivative") + " " + tr("of")  + " " + curveTitle, d_n-2, 2, text);
	delete[] result;

    MultiLayer *ml = app->newGraph(tr("Plot")+tr("Derivative"));
    ml->activeGraph()->insertCurve(t, tableName + "_2", 0);
    LegendMarker *l = ml->activeGraph()->legend();
    l->setText("\\c{1}" + tr("Derivative") + " " + tr("of") + " " + curveTitle);
}


