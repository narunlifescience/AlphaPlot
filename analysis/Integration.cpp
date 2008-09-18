/***************************************************************************
    File                 : Integration.cpp
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Ion Vasilief
    Email (use @ for *)  : ion_vasilief*yahoo.fr
    Description          : Numerical integration of data sets

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
#include "Integration.h"
#include "nrutil.h"
#include "graph/Graph.h"

#include <QMessageBox>
#include <QDateTime>
#include <QLocale>

#include <gsl/gsl_spline.h>
#include <gsl/gsl_interp.h>
#include <gsl/gsl_vector.h>

Integration::Integration(ApplicationWindow *parent, Layer *layer)
: Filter(parent, layer)
{
	init();
}

Integration::Integration(ApplicationWindow *parent, Layer *layer, const QString& curveTitle)
: Filter(parent, layer)
{
	init();
	setDataFromCurve(curveTitle);
}

Integration::Integration(ApplicationWindow *parent, Layer *layer, const QString& curveTitle, double start, double end)
: Filter(parent, layer)
{
	init();
	setDataFromCurve(curveTitle, start, end);
}

void Integration::init()
{
	setName(tr("Integration"));
	m_method = 1;
    m_max_iterations = 40;
    m_sort_data = true;
}

QString Integration::logInfo()
{
	// Do the interpolation, use GSL libraries for that
	gsl_interp_accel *acc = gsl_interp_accel_alloc();
	const gsl_interp_type *method;
	// The method for interpolation is chosen based on the number of points
	if(m_n>3)
		method=gsl_interp_linear;
	else if(m_n>4)
		method=gsl_interp_cspline;
	else if(m_n>5)
		method=gsl_interp_akima;

	// If we have enough points use GSL libraries for interpolation, else use the polint algorithm
	gsl_spline *interp ;
	if(m_n>3)
	{
		interp = gsl_spline_alloc (method, m_n);
		gsl_spline_init (interp, m_x, m_y, m_n);
	}

	// Using Numerical Recipes
	// This is Romberg Integration method
	// This method uses the Nevilles' algorithm for interpollation;
	double yup, ylow;
	double xx,tnm,sum,del,ss,dss,error,tsum;
	if(m_n > 3)
	{
		yup = gsl_spline_eval (interp, m_to, acc);
		ylow = gsl_spline_eval (interp, m_from, acc);
	}
	else if (m_n<=3)
	{
		polint(m_x,m_y,m_n,m_to,&yup,&dss);
		polint(m_x,m_y,m_n,m_from,&ylow,&dss);
	}

	double *S = new double[m_max_iterations];
	double *h = new double[m_max_iterations];
	int j,it,l;
	bool success = false;
	h[0]=1.0;
	for(j=0; j < m_max_iterations; j++)
	{//Trapezoid Rule
		if(j==0)
			S[0]=0.5*(m_to-m_from)*(ylow+yup);
		else
		{
			for(it=1,l=1;l<j-1;l++)it<<=1;
			tnm=it;
			del=(m_to-m_from)/tnm;
			xx=m_from+0.5*del;
			for(sum=0.0,l=1;l<=it;l++)
			{
				if(m_n>3)
					sum+=gsl_spline_eval (interp,xx, acc);
				else if(m_n<=3)
				{
					polint(m_x,m_y,m_n,xx,&tsum,&dss);
					sum+=tsum;
				}
				xx+=del;
			}
			S[j]=0.5*(S[j-1]+(m_to-m_from)*sum/tnm);

		}
		if(j>=m_method)
		{
			polint(&h[j-m_method],&S[j-m_method],m_method,0,&ss,&dss);
			S[j]=ss;
		}
		h[j+1]=0.25*h[j];
		S[j+1]=S[j];
		error=fabs(S[j]-S[j-1]);
		if(error<=m_tolerance) success = true;
		if(success) break;
	}

	QString logInfo = "[" + QDateTime::currentDateTime().toString(Qt::LocalDate) + "\t" + tr("Plot")+ ": ''" + m_layer->parentPlotName() + "'']\n";
	logInfo += "\n" + tr("Numerical integration of") + ": " + m_curve->title().text() + " " + tr("using a %1 order method").arg(m_method)+"\n";
	if(success)
		logInfo += tr("Iterations") + ": " + QString::number(j)+"\n";
	if(!success)
		logInfo += tr("Iterations") + ": " + QString::number(j-1)+"\n";

    ApplicationWindow *app = (ApplicationWindow *)parent();
    int prec = app->m_decimal_digits;
	logInfo += tr("Tolerance") + "(" + tr("max") + " = " + QLocale().toString(m_tolerance, 'g', prec)+"): " + QString::number(error)+ "\n";
	logInfo += tr("Points") + ": "+QString::number(m_n) + " " + tr("from") + " x = " +QLocale().toString(m_from, 'g', prec) + " ";
    logInfo += tr("to") + " x = " + QLocale().toString(m_to, 'g', prec) + "\n";

    // using GSL to find maximum value of data set
    gsl_vector *aux = gsl_vector_alloc(m_n);
    for(int i=0; i < m_n; i++)
        gsl_vector_set (aux, i, m_y[i]);
    int maxID=gsl_vector_max_index (aux);
    gsl_vector_free (aux);

    logInfo += tr("Peak at") + " x = " + QLocale().toString(m_x[maxID], 'g', prec)+"\t";
	logInfo += "y = " + QLocale().toString(m_y[maxID], 'g', prec)+"\n";

	logInfo += tr("Area") + "=";
	if(success)
		logInfo += QLocale().toString(S[j], 'g', prec);
	if(!success)
		logInfo += QLocale().toString(S[j-1], 'g', prec);
	logInfo += "\n-------------------------------------------------------------\n";

	if(m_n>3)
		gsl_spline_free (interp);

	gsl_interp_accel_free (acc);
    delete[] S;
    delete[] h;
    return logInfo;
}

void Integration::setMethodOrder(int n)
{
if (n < 1 || n > 5)
    {
        QMessageBox::critical((ApplicationWindow *)parent(), tr("Error"),
        tr("Unknown integration method. Valid values must be in the range: 1 (Trapezoidal Method) to 5."));
        return;
    }

m_method = n;
}
