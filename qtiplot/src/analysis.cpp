/***************************************************************************
    File                 : analysis.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Analysis methods for class 'Graph'
                           
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
#include "graph.h"
#include "plot.h"
#include "fit.h"
#include "ImageMarker.h"
#include "LegendMarker.h"
#include "parser.h"
#include "Histogram.h"
#include "nrutil.h"

#include <qapplication.h>
#include <qdatetime.h> 
#include <qimage.h>
#include <qmessagebox.h>
#include <qfile.h>
#include <qlibrary.h>

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_multifit_nlin.h>
#include <gsl/gsl_fit.h>
#include <gsl/gsl_sort.h>
#include <gsl/gsl_sort_vector.h>
#include <gsl/gsl_multifit.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_spline.h>
#include <gsl/gsl_interp.h>
#include <gsl/gsl_fft_real.h>
#include <gsl/gsl_fft_halfcomplex.h>
#include <gsl/gsl_fft_complex.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_histogram.h>
#include <gsl/gsl_statistics.h>
#include <gsl/gsl_diff.h>
#include <gsl/gsl_multimin.h>

void Graph::updateHistogram(Table* w, const QString& curveName, int curve, bool automatic, double binSize, double begin, double end)
{
	int i;
	long curveID = curveKey(curve);
	QwtHistogram *cv = (QwtHistogram *)d_plot->curve (curveID);
	if (!cv)
		return;
	if (cv->autoBinning() && automatic)
		return;

	int ycol=w->colIndex(curveName);
	int r=w->tableRows();

	Q3MemArray<double> Y(1);
	int it=0;
	for (i = 0;i<r;i++ )
	{
		QString yval=w->text(i,ycol);
		if (!yval.isEmpty())
		{
			it++;
			Y.resize(it);
			Y[it-1]=yval.toDouble();
		}
	}

	if(it<2 || (it==2 && Y[0] == Y[1]))
	{//non valid histogram
		Q3MemArray<double> X(2); 
		Y.resize(2);
		for (i = 0;i<2;i++ )
		{
			Y[i] = 0;
			X[i] = 0;
		}
		cv->setData(X, Y, 2);
		d_plot->replot(); 
		updateScale();
		return;
	}

	int n;
	gsl_histogram * h; 
	if (automatic)
	{
		n=10;
		h = gsl_histogram_alloc (n); 
		if (!h)  
			return;

		gsl_vector *v;
		v = gsl_vector_alloc (it);
		for (i = 0;i<it;i++ )
			gsl_vector_set (v, i, Y[i]);

		double min, max;
		gsl_vector_minmax (v, &min, &max);
		gsl_vector_free (v);

		double from=floor(min);
		double to=ceil(max);

		gsl_histogram_set_ranges_uniform (h, floor(min), ceil(max));
		cv->setBinning(true, from, to, (double)(to-from)/(double)n);
	}
	else
	{
		n=int((end - begin)/binSize+1);
		h = gsl_histogram_alloc (n); 
		if (!h)  
			return;

		double *range;
		range=vector(0,n+1);

		for (i = 0;i<=n+1;i++ )
			range[i]=begin+i*binSize;

		gsl_histogram_set_ranges (h, range, n+1);
		free_vector(range,0,n+1);
		cv->setBinning(false, begin, end, binSize);
	}

	for (i = 0;i<it;i++ )
		gsl_histogram_increment (h, Y[i]);

	Q3MemArray<double> X(n); //stores ranges (x) and bins (y)
	Y.resize(n);
	for (i = 0;i<n;i++ )
	{
		Y[i] = gsl_histogram_get (h, i);
		double lower, upper;
		gsl_histogram_get_range (h, i, &lower, &upper);
		X[i] = lower;
	}
	cv->setData(X, Y, n);
	gsl_histogram_free (h);
	d_plot->replot(); 
	updateScale();
	emit modifiedGraph();
}

void Graph::updateHistogram(Table* w, const QString& curveName, int curve)
{
	int i;
	long curveID = curveKey(curve);
	QwtHistogram *cv = (QwtHistogram *)d_plot->curve (curveID);
	if (!cv)
		return;

	int ycol=w->colIndex(curveName);
	int r=w->tableRows();

	Q3MemArray<double> Y(1);
	int it=0;
	for (i = 0;i<r;i++ )
	{
		QString yval=w->text(i,ycol);
		if (!yval.isEmpty())
		{
			it++;
			Y.resize(it);
			Y[it-1]=yval.toDouble();
		}
	}

	if (!it)
		removeCurve(curve);

	if(it<2 || (it==2 && Y[0] == Y[1]))
	{//non valid histogram data
		Q3MemArray<double> X(2); 
		Y.resize(2);
		for (i = 0;i<2;i++ )
		{
			Y[i] = 0;
			X[i] = 0;
		}

		cv->setData(X, Y, 2);
		d_plot->replot(); 
		updateScale();
		return;
	}

	double binSize = cv->binSize();
	double begin = cv->begin();
	double end = cv->end();
	int n;
	gsl_histogram * h; 

	if (cv->autoBinning())
	{
		n=10;
		h = gsl_histogram_alloc (n); 
		if (!h)  
			return;

		gsl_vector *v;
		v = gsl_vector_alloc (it);
		for (i = 0;i<it;i++ )
			gsl_vector_set (v, i, Y[i]);

		double min, max;
		gsl_vector_minmax (v, &min, &max);
		gsl_vector_free (v);

		double from=floor(min);
		double to=ceil(max);

		gsl_histogram_set_ranges_uniform (h, floor(min), ceil(max));
		cv->setBinning(true, from, to, (double)(to-from)/(double)n);
	}
	else
	{
		n=int((end - begin)/binSize+1);
		h = gsl_histogram_alloc (n); 
		if (!h)  
			return;

		double *range;
		range=vector(0,n+1);

		for (i = 0;i<=n+1;i++ )
			range[i]=begin+i*binSize;

		gsl_histogram_set_ranges (h, range, n+1);
		free_vector(range,0,n+1);
	}

	for (i = 0;i<it;i++ )
		gsl_histogram_increment (h, Y[i]);

	Q3MemArray<double> X(n); //stores ranges (x) and bins (y)
	Y.resize(n);
	for (i = 0;i<n;i++ )
	{
		Y[i] = gsl_histogram_get (h, i);
		double lower, upper;
		gsl_histogram_get_range (h, i, &lower, &upper);
		X[i] = lower;
	}

	cv->setData(X, Y, n);
	gsl_histogram_free (h);
	d_plot->replot(); 
	updateScale();
}

QString Graph::showHistogramStats(Table* w, const QString& curveName, int curve)
{
	int i;
	long curveID = curveKey(curve);
	QwtHistogram *cv= (QwtHistogram *)d_plot->curve (curveID);
	if (!cv)
		return "";

	int ycol=w->colIndex(curveName);
	int r=w->tableRows();

	Q3MemArray<double> Y(1);
	int it=0;
	for (i = 0;i<r;i++ )
	{
		QString yval=w->text(i,ycol);
		if (!yval.isEmpty())
		{
			it++;
			Y.resize(it);
			Y[it-1]=yval.toDouble();
		}
	}

	if(it<2 || (it==2 && Y[0] == Y[1]))
	{//non valid histogram data
		QMessageBox::critical(0,tr("Error - QtiPlot"), tr("Your data is not valid. You need at least two different points for a histogram!"));
		return "";
	}

	bool automatic = cv->autoBinning();
	double binSize = cv->binSize();
	double begin = cv->begin();
	double end = cv->end();
	int n;
	gsl_histogram * h; 

	if (automatic)
	{
		n=10;
		h = gsl_histogram_alloc (n); 
		if (!h)  
			return "";

		gsl_vector *v;
		v = gsl_vector_alloc (it);
		for (i = 0;i<it;i++ )
			gsl_vector_set (v, i, Y[i]);

		double min, max;
		gsl_vector_minmax (v, &min, &max);
		gsl_vector_free (v);

		gsl_histogram_set_ranges_uniform (h, floor(min), ceil(max));
	}
	else
	{
		n=int((end - begin)/binSize+1);
		h = gsl_histogram_alloc (n); 
		if (!h)  
			return "";

		double *range;
		range=vector(0,n+1);

		for (i = 0;i<=n+1;i++ )
			range[i]=begin+i*binSize;

		gsl_histogram_set_ranges (h, range, n+1);
		free_vector(range,0,n+1);
	}

	for (i = 0;i<it;i++ )
		gsl_histogram_increment (h, Y[i]);

	Q3MemArray<double> X(n); //stock ranges (x) and bins (y)
	Y.resize(n);
	for (i = 0;i<n;i++ )
	{
		Y[i] = gsl_histogram_get (h, i);
		double lower, upper;
		gsl_histogram_get_range (h, i, &lower, &upper);
		X[i] = lower;
	}

	double h_sum=0.0; 
	for (i = 0;i<n;i++ )
		h_sum+=Y[i];

	double sum=0.0;
	QString text=tr("Bins")+"\t"+tr("Quantity")+"\t"+tr("Sum")+"\t"+tr("Percent")+"\n";
	for (i = 0;i<n;i++ )
	{
		sum+=Y[i];
		text+=QString::number(X[i])+"\t";
		text+=QString::number(Y[i])+"\t";
		text+=QString::number(sum)+"\t";
		text+=QString::number(sum/h_sum*100)+"\n";
	}

	QDateTime dt = QDateTime::currentDateTime();  
	QString info=dt.toString(Qt::LocalDate)+"\t"+tr("Histogram and Probabilities for")+": "+curveName+"\n";
	info+=tr("Mean")+" = "+QString::number(gsl_histogram_mean (h))+"\t";
	info+=tr("Standard Deviation")+" = "+QString::number(gsl_histogram_sigma (h))+"\n";
	info+=tr("Minimum")+" = "+QString::number(gsl_histogram_min_val(h))+"\t";
	info+=tr("Maximum")+" = "+QString::number(gsl_histogram_max_val(h))+"\t";
	info+=tr("Bins")+" = "+QString::number(gsl_histogram_bins (h))+"\n";
	info+="-------------------------------------------------------------\n";
	gsl_histogram_free (h);

	QString caption="Bins-"+curveName;
	emit createHistogramTable(caption, n, 4, text);
	return info;
}

void Graph::initHistogram(long curveID, const Q3MemArray<double>& Y, int it)
{
	QwtHistogram *hc = (QwtHistogram *)d_plot->curve(curveID);
	if (!hc)
		return;

	int i;
	if(it<2 || (it==2 && Y[0] == Y[1]))
	{//non valid histogram data
		Q3MemArray<double> x(2),y(2); 
		for (i = 0;i<2;i++ )
		{
			y[i] = 0;
			x[i] = 0;
		}
		hc->setData(x, y, 2);
		d_plot->replot(); 
		updateScale();
		return;
	}

	int n=10;//default value
	Q3MemArray<double> x(n),y(n); //store ranges (x) and bins (y)
	gsl_histogram * h = gsl_histogram_alloc (n); 
	if (!h)  
		return;

	gsl_vector *v;
	v = gsl_vector_alloc (it);
	for (i = 0;i<it;i++ )
		gsl_vector_set (v, i, Y[i]);

	double min, max;
	gsl_vector_minmax (v, &min, &max);
	gsl_vector_free (v);

	double from=floor(min);
	double to=ceil(max);

	gsl_histogram_set_ranges_uniform (h, floor(min), ceil(max));

	for (i = 0;i<it;i++ )
		gsl_histogram_increment (h, Y[i]);

	for (i = 0;i<n;i++ )
	{
		y[i] = gsl_histogram_get (h, i);
		double lower, upper;
		gsl_histogram_get_range (h, i, &lower, &upper);
		x[i] = lower;
	}

	hc->setBinning(true, (double)from, (double)to, (double)(to-from)/(double)n);
	hc->setData(x, y, n);
	gsl_histogram_free (h);
}

void Graph::fft(long curveKey, bool forward, double sampling, 
		bool normalizeAmp, bool order)
{
	QwtPlotCurve *curve=d_plot->curve(curveKey);
	if (!curve)
		return;

	QApplication::setOverrideCursor(Qt::waitCursor);

	int i, i2, n=curve->dataSize();
	int n2 = n/2;
	double *x = new double[n];
	double *y = new double[n];
	double *amp = new double[n];
	double *result = new double[2*n];

	if(!x || !y || !amp || !result) 
	{
		QMessageBox::warning(0,"QtiPlot", tr("Could not allocate memory, operation aborted!"));
		return;
	}

	for (i = 0; i<n; i++)
		y[i]=curve->y(i);

	double df = 1.0/(double)(n*sampling);//frequency sampling
	double aMax = 0.0;//max amplitude
	QString label, text;
	if(forward)
	{
		label="ForwardFFT"+QString::number(++fitID);
		text= tr("Frequency");

		gsl_fft_real_workspace *work=gsl_fft_real_workspace_alloc(n);
		gsl_fft_real_wavetable *real=gsl_fft_real_wavetable_alloc(n);

		if(!work || !real) 
		{
			QMessageBox::warning(0,"QtiPlot", tr("Could not allocate memory, operation aborted!"));
			return;
		}

		gsl_fft_real_transform(y,1,n,real,work);
		gsl_fft_halfcomplex_unpack (y, result, 1, n);

		gsl_fft_real_wavetable_free(real);
		gsl_fft_real_workspace_free(work);
	}
	else
	{
		label="InverseFFT"+QString::number(++fitID);
		text= tr("Time");

		gsl_fft_real_unpack (y, result, 1, n);
		gsl_fft_complex_wavetable *wavetable = gsl_fft_complex_wavetable_alloc (n);
		gsl_fft_complex_workspace *workspace = gsl_fft_complex_workspace_alloc (n);

		if(!workspace || !wavetable) 
		{
			QMessageBox::warning(0,"QtiPlot", tr("Could not allocate memory, operation aborted!"));
			return;
		}

		gsl_fft_complex_inverse (result, 1, n, wavetable, workspace);

		gsl_fft_complex_wavetable_free (wavetable);
		gsl_fft_complex_workspace_free (workspace);
	}

	if (order)
	{
		for(i=0;i<n;i++)
		{
			x[i] = (i-n2)*df;
			int j = i + n;
			double aux = result[i];
			result[i] = result[j];
			result[j] = aux;
		}
	}
	else
	{
		for(i=0;i<n;i++)
			x[i] = i*df;
	}

	for(i=0;i<n;i++)
	{
		i2 = 2*i;
		double real_part = result[i2];
		double im_part = result[i2+1];
		double a = sqrt(real_part*real_part + im_part*im_part);
		amp[i]= a;
		if (a > aMax)
			aMax = a;
	}
	text+="\t"+tr("Real")+"\t"+tr("Imaginary")+"\t"+ tr("Amplitude")+"\t"+tr("Angle")+"\n";
	for (i=0;i<n;i++)
	{
		i2 = 2*i;
		text+=QString::number(x[i])+"\t";
		text+=QString::number(result[i2])+"\t";
		text+=QString::number(result[i2+1])+"\t";
		if (normalizeAmp)
			text+=QString::number(amp[i]/aMax)+"\t";
		else
			text+=QString::number(amp[i])+"\t";
		text+=QString::number(atan(result[i2+1]/result[i2]))+"\n";
	}
	delete[] x; 
	delete[] y;
	delete[] amp; 
	delete[] result;

	emit createHiddenTable(label, n, 5, text);	
	QApplication::restoreOverrideCursor();
}

void Graph::interpolate(QwtPlotCurve *curve, int spline, int start, int end, 
		int points, int colorIndex)
{
	size_t i, n = end - start + 1;
	double *x=vector(0, n-1);
	double *y=vector(0, n-1);
	for (i = 0; i < n; i++)
	{// This is the data to be analysed 
		x[i]=curve->x(i+start);
		y[i]=curve->y(i+start);
	}

	// Sort the elements. The unfortunate thing here is that we do not know whether they are
	// sorted or not. Use GSL library for that
	double *ytemp,*xtemp;
	xtemp=vector(0, n-1);
	ytemp=vector(0, n-1);

	size_t *p=ivector(0, n-1);
	gsl_sort_index(p,x,1,n); // Find the permutation
	//Sort y
	for (i=0;i<n;i++)
		ytemp[i]=y[p[i]];
	for (i=0;i<n;i++)
		y[i]=ytemp[i];
	//sort x
	for (i=0;i<n;i++)
		xtemp[i]=x[p[i]];
	for (i=0;i<n;i++)
		x[i]=xtemp[i];	
	// free the auxilary vectors
	free_vector(ytemp,0,n-1);free_vector(xtemp,0,n-1); free_ivector(p,0,n-1);

	// Do the interpolation
	gsl_interp_accel *acc= gsl_interp_accel_alloc ();
	const gsl_interp_type *method;
	QString label, wlabel;
	switch(spline)
	{
		case 0:
			method=gsl_interp_linear;
			label="LinearInt";
			wlabel = tr("Linear interpolation of ")+curve->title().text();
			break;
		case 1:
			method=gsl_interp_cspline;
			label="CubicInt";
			wlabel = tr("Cubic interpolation of ")+curve->title().text();
			break;
		case 2:
			method=gsl_interp_akima;
			label="AkimaInt";
			wlabel = tr("Akima interpolation of ")+curve->title().text();
			break;
	}

	gsl_spline *interp = gsl_spline_alloc (method, n);     
	gsl_spline_init (interp, x, y, n);	

	double origin = x[0];
	double step=(x[n-1]-x[0])/(double)(points-1);
	free_vector(x,0,n-1);	
	free_vector(y,0,n-1);

	x=vector(0, points-1);
	y=vector(0, points-1);
	for (int j=0; j<points; j++)
	{
		x[j]=origin + j*step;
		y[j]=gsl_spline_eval (interp, x[j], acc);
	}

	addResultCurve(points, x, y, colorIndex, label+QString::number(++fitID), wlabel);

	gsl_spline_free (interp);
	gsl_interp_accel_free (acc);	
}

bool Graph::diffCurve(const QString& curveTitle)
{
	int n, start, end;
	QwtPlotCurve *c= getValidCurve(curveTitle, 4, n, start, end);
	if (!c)
		return false;

	double *x=vector(0,n-1);
	double *y=vector(0,n-1);
	double *result=vector(0,n-2);

	int i,aux = 0;
	for (i = start; i <= end; i++)
	{// The data to be fitted 
		x[aux]=c->x(i);
		y[aux]=c->y(i);
		aux++;
	}

	for (i = 1; i < n-1; i++)
		result[i]=0.5*((y[i+1]-y[i])/(x[i+1]-x[i]) + (y[i]-y[i-1])/(x[i]-x[i-1]));

	QString text="x\tderivative\n";
	for (i = 1; i < n-1; i++)
	{
		text+=QString::number(x[i]);
		text+="\t";
		text+=QString::number(result[i]);
		text+="\n";
	}

	emit createHiddenTable(c->title().text()+"\t"+ tr("Derivative of")+" "+c->title().text(),n-2,2,text);
	free_vector(x,0,n-1);free_vector(y,0,n-1);free_vector(result,0,n-2);
	return true;
}

QString Graph::integrateCurve(QwtPlotCurve *c, int order, int iter, double tol, double low, double up)
{
	if (!c)
		return "";

	int i, size=c->dataSize();
	int iStart=0, iEnd=size-1;
	for (i=0;i<size;i++)
	{
		if (c->x(i)>=low)
		{
			iStart=i;
			break;
		}
	}
	for (i=iEnd;i>=0;i--)
	{
		if (c->x(i)<=up)
		{
			iEnd=i;
			break;
		}
	}

	const int n=abs(iEnd-iStart)+1;	
	if (n<2)
	{
		QMessageBox::critical(this,tr("QtiPlot - Integration error"),
				tr("You need at least 2 points to integrate! Integration aborted!"));
		return "";
	}

	double *x,*y;
	x=vector(0,n-1);
	y=vector(0,n-1);

	gsl_vector *Y;
	Y = gsl_vector_alloc (n);

	bool noBijection = false;
	double pr_x = 0;
	if (low<up)
	{
		for (i = 0; i <n; i++)
		{
			x[i]=c->x(i+iStart);
			if (x[i] == pr_x)
			{
				noBijection = true;
				break;
			}

			y[i] = c->y(i+iStart);// This is the data to be fitted 
			gsl_vector_set (Y, i, y[i]);
			pr_x = x[i];
		}
	}
	else
	{
		for (i = 0; i <n; i++)
		{
			x[i]=c->x(i+iEnd);
			if (x[i] == pr_x)
			{
				noBijection = true;
				break;
			}

			y[i] = c->y(i+iEnd);// This is the data to be fitted 
			gsl_vector_set (Y, i, y[i]);
			pr_x = x[i];
		}
	}

	if (noBijection)
	{
		QMessageBox::critical(this, tr("QtiPlot - Integration error"),
				tr("Several points have the same x value causing divisions by zero, integration aborted!"));
		return "";
	}

	//Find the x that has the maximum y
	int maxID=gsl_vector_max_index (Y);
	gsl_vector_free (Y);

	// Sort the vectors, so that they can be interpolated
	size_t *p;
	p=ivector(0,n-1);

	double *ytemp,*xtemp;
	xtemp=vector(0,n-1);
	ytemp=vector(0,n-1);
	// Use GSl libraries for that
	gsl_sort_index(p,x,1,n); // Find the permutation
	//Sort y
	ytemp=vector(0,n-1);
	for (i=0;i<n;i++)
		ytemp[i]=y[p[i]];
	for (i=0;i<n;i++)
		y[i]=ytemp[i];
	//sort x
	xtemp=vector(0,n-1);
	for (i=0;i<n;i++)
		xtemp[i]=x[p[i]];
	for (i=0;i<n;i++)
		x[i]=xtemp[i];	
	// free the auxilary vectors
	free_vector(ytemp,0,n-1);free_vector(xtemp,0,n-1); free_ivector(p,0,n-1);

	// Do the interpolation, use GSL libraries for that
	gsl_interp_accel *acc= gsl_interp_accel_alloc ();
	const gsl_interp_type *method;
	// The method for interpolation is chosen based on the number of points
	if(n>3)
		method=gsl_interp_linear;
	else if(n>4)
		method=gsl_interp_cspline;
	else if(n>5)
		method=gsl_interp_akima;

	// If we have enough points use GSL libraries for interpolation, else use the polint algorithm
	gsl_spline *interp ;
	if(n>3) 
	{	
		interp = gsl_spline_alloc (method, n);
		gsl_spline_init (interp, x, y, n);
	}

	// Now the funny part starts. Use Numerical Recipes
	// This is Romberg Integration method
	// This method uses the Nevilles' algorithm for interpollation; I
	// do not thing I could use the GSl libraries for this
	double yup,ylow;
	double xx,tnm,sum,del,ss,dss,error,tsum;
	if(n>3)
	{
		yup=gsl_spline_eval (interp,up, acc);
		ylow=gsl_spline_eval (interp,low, acc);
	}
	else if (n<=3) 
	{
		polint(x,y,n,up,&yup,&dss);
		polint(x,y,n,low,&ylow,&dss);
	}

	double *S, *h;
	S=vector(0,iter);
	h=vector(0,iter);
	int j,it,l;
	bool success=FALSE;
	h[1]=1.0;
	for(j=1;j<=iter;j++)
	{//Trapezoid Rule
		if(j==1)
			S[j]=0.5*(up-low)*(ylow+yup);
		else
		{
			for(it=1,l=1;l<j-1;l++)it<<=1;
			tnm=it;
			del=(up-low)/tnm;
			xx=low+0.5*del;
			for(sum=0.0,l=1;l<=it;l++) 
			{
				if(n>3)
					sum+=gsl_spline_eval (interp,xx, acc);
				else if(n<=3)
				{
					polint(x,y,n,xx,&tsum,&dss);
					sum+=tsum;
				}
				xx+=del;
			}
			S[j]=0.5*(S[j-1]+(up-low)*sum/tnm);

		}
		if(j>=order)
		{
			polint(&h[j-order],&S[j-order],order,0,&ss,&dss);
			S[j]=ss;
		}
		h[j+1]=0.25*h[j];
		S[j+1]=S[j];
		error=fabs(S[j]-S[j-1]);
		if(error<=tol) success=TRUE;
		if(success) break;
	}

	QDateTime dt = QDateTime::currentDateTime ();
	QString date=dt.toString(Qt::LocalDate);
	QString pref;
	if(order==1) pref="st";
	if(order==2) pref="nd";
	if(order==3) pref="rd";
	if(order>3) pref="th";
	QString info=date+"\t"+this->caption()+"\nNumerical integration of: "+c->title().text()+" with a "+QString::number(order)+pref+" order method \n";
	if(success)
		info+="Number of iterations: "+QString::number(j)+"\n"; 
	if(!success)
		info+="Number of iterations: "+QString::number(j-1)+"\n"; 
	info+="Tolerance (max="+QString::number(tol)+"): "+QString::number(error)+"\n";
	info+="Points: "+QString::number(n)+" from x="+QString::number(low) +" to x="+QString::number(up)+"\n";
	info+="Peak at x=";
	info+=QString::number(x[maxID])+"\t";
	info+="y=";
	info+=QString::number(y[maxID])+"\n";
	info+="Area=";
	if(success)
		info+=QString::number(S[j]);
	if(!success)
		info+=QString::number(S[j-1]);
	info+="\n-------------------------------------------------------------\n";

	if(n>3)
		gsl_spline_free (interp);

	gsl_interp_accel_free (acc);
	free_vector(x,0,n-1);free_vector(y,0,n-1); free_vector(S,0,iter);free_vector(h,0,iter);
	return info;
}

QString Graph::fitLinear(const QString& curveTitle)
{
	int n, start, end;
	QwtPlotCurve *c= getValidCurve(curveTitle, 3, n, start, end);
	if (!c)
		return QString::null;

	double *x=vector(0,n-1);
	double *y=vector(0,n-1);

	int i, aux = 0;
	for (i = start; i <= end; i++)
	{// The data to be fitted 
		x[aux]=c->x(i);
		y[aux]=c->y(i);
		aux++;
	}

	double c0, c1, cov00, cov01, cov11, sumsq;	
	gsl_fit_linear (x, 1, y, 1, n, &c0, &c1, &cov00, &cov01, &cov11, &sumsq);

	double sst=(n-1)*gsl_stats_variance(y,1,n);
	double Rsquare=1-sumsq/sst;

	for (i=0;i<n;i++)
		y[i]=c0+c1*x[i];

	QDateTime dt = QDateTime::currentDateTime ();
	QString date=dt.toString(Qt::LocalDate);
	QString info=date+"\t"+this->caption()+" LinearFit"+ QString::number(fitID)+ ":\n";
	info+="Linear regression of " + c->title().text() + ": y=Ax+B\n";
	info+="From x="+QString::number(x[0]) +" to x="+QString::number(x[n-1])+"\n";
	info+="A = "+QString::number(c1)+" +/- " + QString::number(sqrt(cov11))+"\n";
	info+="B = "+QString::number(c0)+" +/- " + QString::number(sqrt(cov00));
	info+="\n-------------------------------------------------------------\n";
	info+="sumsq = "+QString::number(sumsq);
	info+="\nRsquare = "+QString::number(Rsquare);
	info+="\n-------------------------------------------------------------\n";

	addResultCurve(n, x, y, 1, "LinearFit"+QString::number(++fitID), tr("Linear regression of ")+c->title().text());
	return info;
}

void Graph::fitMultiPeak(int fitType, const QString& curveTitle)
{
	int n, start, end;
	int p = 3*n_peaks+1;
	QwtPlotCurve *c= getValidCurve(curveTitle, p, n, start, end);
	if (!c)
		return;

	double *x=vector(0,n-1);
	double *y=vector(0,n-1);
	if (!x || !y)
	{
		QMessageBox::warning(0, tr("QtiPlot"), tr("Could not allocate memory, operation aborted!"));
		return;
	}

	int i, j, aux = 0;
	double min = y[start];
	for (i = start; i <= end; i++)
	{//The data to be fitted 
		x[aux]=c->x(i);
		y[aux]=c->y(i);
		if (y[aux]<=min)
			min = y[aux];
		aux++;
	}
	struct FitMultiPeakData d = {n, p, x, y};
	gsl_multifit_function_fdf f;
	if (fitType)
	{
		f.f = &lorentz_multi_peak_f;
		f.df = &lorentz_multi_peak_df;
		f.fdf = &lorentz_multi_peak_fdf;
	}
	else
	{
		f.f = &gauss_multi_peak_f;
		f.df = &gauss_multi_peak_df;
		f.fdf = &gauss_multi_peak_fdf;
	}
	f.n = n;
	f.p = p;
	f.params = &d;

	double *x_init = new double[p];
	for (i=0; i<n_peaks; i++)
	{
		x_init[3*i]=peaks_array[2*i];
		x_init[3*i+1]=peaks_array[2*i+1];
		x_init[3*i+2]=1.0;
	}
	x_init[p-1] = min;

	int status, iter=1000;
	gsl_multifit_fdfsolver *s = fitGSL(f, p, n, x_init, 0, 1e-4, iter, status);	
	delete[] x_init;
	delete[] peaks_array;

	double *par=vector(0,p-1);
	for (i=0;i<(int)p;i++)
		par[i]=gsl_vector_get(s->x,i); //retrieve values for the parameters

	double X0 =	x[0];
	double XN =	x[n-1];
	int n1 = (n<100)?100:n;
	double step=(XN-X0)/(n1-1);
	free_vector(x,0,n-1); free_vector(y,0,n-1);
	x=vector(0,n1-1); y=vector(0,n1-1);

	double *a = new double[n_peaks];
	double *xc = new double[n_peaks];
	double *w = new double[n_peaks];
	QStringList params;
	for (i = 0; i<n_peaks; i++)
	{
		a[i] = par[3*i];
		xc[i] = par[3*i+1];
		w[i] = par[3*i+2];
		params << "a" + QString::number(i+1);
		params << "xc" + QString::number(i+1);
		params << "w" + QString::number(i+1);
	}
	params << tr("y0 (offset)");

	QString text="1\t";
	for (i=0; i<n_peaks; i++)
		text+="peak"+QString::number(i+1)+"\t";
	text+="2\n";

	gsl_matrix * m = gsl_matrix_alloc (n1, n_peaks);
	if (!m)
	{
		QMessageBox::warning(0,"QtiPlot", tr("Could not allocate memory, operation aborted!"));
		return;
	}

	for (i = 0; i<n1; i++)
	{
		x[i]=X0+i*step;
		text+=QString::number(x[i], 'g', 15)+"\t";
		double yi=0;
		for (j=0; j<n_peaks; j++)
		{
			double diff=x[i]-xc[j];
			double y_aux;
			if (fitType)
				y_aux = a[j]*w[j]/(4*diff*diff+w[j]*w[j]);
			else
				y_aux = a[j]*exp(-0.5*diff*diff/(w[j]*w[j]));
			yi+= y_aux;
			y_aux+=par[p-1];//add offset
			text+=QString::number(y_aux, 'g', 15)+"\t";
			gsl_matrix_set(m, i, j, y_aux);
		}
		y[i] = yi + par[p-1];//add offset
		text+=QString::number(y[i], 'g', 15)+"\n";
	}
	delete[] a;
	delete[] xc;
	delete[] w;

	QString tableName = "Fit"+QString::number(++fitID);
	QString label=tableName+"_2";

	QwtPlotCurve *cv = new QwtPlotCurve(label);
	long curveID = d_plot->insertCurve(cv);
	cv->setPen(QPen(Qt::red,2)); 
	cv->setData(x, y, n1);	

	c_type.resize(++n_curves);
	c_type[n_curves-1]=Line;

	c_keys.resize(n_curves);
	c_keys[n_curves-1] = curveID;

	addLegendItem(label);	
	label=tableName+"_1(X),"+label+"(Y)";
	associations<<label;

	for (i=0; i<n_peaks; i++)
	{
		for (j=0; j<n1; j++)
			y[j] = gsl_matrix_get (m, j, i);

		label=tableName+"_peak"+QString::number(i+1);
		cv = new QwtPlotCurve(label);
		curveID = d_plot->insertCurve(cv);
		cv->setPen(QPen(Qt::green,1)); 
		cv->setData(x, y, n1);	

		c_type.resize(++n_curves);
		c_type[n_curves-1]=Line;
		c_keys.resize(n_curves);
		c_keys[n_curves-1] = curveID;

		addLegendItem(label);	
		label=tableName+"_1(X),"+label+"(Y)";
		associations<<label;
	}
	gsl_matrix_free(m);
	free_vector(x, 0, n1-1);
	free_vector(y, 0, n1-1);

	QString legend;
	if (fitType)
	{
		legend = tr("Lorentz");
		for (i=0; i<n_peaks; i++)
			par[3*i]*= M_PI_2;
	}
	else
		legend = tr("Gauss");
	legend += "(" + QString::number(n_peaks) +") " + tr("multi-peak");
	emit createHiddenTable(tableName+"\t"+legend+" "+tr("fit of")+" "+curveTitle, n1, 2+n_peaks, text);
	updatePlot();
	emit showFitResults(outputFitString(n1, 1e-4, X0, XN, iter, 0, status, par, s, params,
				curveTitle, QString::null, legend));
}

QString Graph::fitNonlinearCurve(const QString& curve,const QString& formula,
		const QStringList& params,const QStringList& paramsInit,
		double from,double to,int points, int solver, double tolerance, 
		int colorIndex)
{
	const size_t p = params.count();
	int start, end;
	QwtPlotCurve* c = getFitLimits(curve, from, to, p, start, end);
	if (!c)
		return QString::null;	

	int n = end - start + 1;
	double *X = vector(0, n-1);
	double *Y = vector(0, n-1);
	int i, aux = 0;
	for (i = start; i <= end; i++)
	{// This is the data to be fitted 
		X[aux]=c->x(i);
		Y[aux]=c->y(i);
		aux++;
	}

	QStringList equation=QStringList::split("=",formula,FALSE);
	const char *function=equation[1].ascii();
	QString names=params.join (",");
	const char *parNames=names.ascii();
	struct FitParserData d = {n, p, X, Y, function, parNames};
	int status;
	double *par=vector(0,p-1);
	double X0 =	X[0];
	double XN =	X[n-1];
	int n1 = (n<100)?100:n;
	double step=(XN-X0)/(n1-1);
	MyParser parser;
	QString result;
	double xvar;

	if(solver==2)
	{
		gsl_multimin_function f;
		f.f = &user_d;
		f.n = p;
		f.params = &d;

		double *x_init = new double[p];
		for (i=0;i<(int)p;i++)
			x_init[i]=paramsInit[i].toDouble();

		gsl_multimin_fminimizer *s =  fitSimplex(f, x_init, tolerance, points, status);	
		delete[] x_init;

		for (i=0;i<(int)p;i++)
			par[i]=gsl_vector_get(s->x,i); //retrieve values for the parameters

		for (i=0;i<(int)p;i++)
			parser.DefineVar(params[i].ascii(), &par[i]);
		parser.DefineVar("x", &xvar);
		parser.SetExpr(function);

		//allocate and evaluate jacobian of residuals
		gsl_matrix *J = gsl_matrix_alloc(n,p);
		user_df(s->x,(void*)f.params,J);
		free_vector(X,0,n-1); free_vector(Y,0,n-1);
		X=vector(0,n1-1); Y=vector(0,n1-1);
		for (i=0;i<n1;i++)
		{
			X[i]=X0+i*step;
			xvar=X[i];
			Y[i]=parser.Eval();
		}
		addResultCurve(n1, X, Y, colorIndex, "Fit"+QString::number(fitID), tr("Non-linear fit of ")+curve);

		result = outputFitString(n, tolerance, X0, XN, points, J, status, par, s, params,
				curve,   formula, tr("Non-linear"));
	}
	else
	{
		gsl_multifit_function_fdf f;
		f.f = &user_f;
		f.df = &user_df;
		f.fdf = &user_fdf;
		f.n = n;
		f.p = p;
		f.params = &d;

		double *x_init = new double[p];
		for (i=0;i<(int)p;i++)
			x_init[i]=paramsInit[i].toDouble();

		gsl_multifit_fdfsolver *s = fitGSL(f, p, n, x_init, solver, tolerance, points, status);	
		delete[] x_init;

		for (i=0;i<(int)p;i++)
			par[i]=gsl_vector_get(s->x,i); //retrieve values for the parameters

		for (i=0;i<(int)p;i++)
			parser.DefineVar(params[i].ascii(), &par[i]);
		parser.DefineVar("x", &xvar);
		parser.SetExpr(function);

		free_vector(X,0,n-1); free_vector(Y,0,n-1);
		X=vector(0,n1-1); Y=vector(0,n1-1);
		for (i=0;i<n1;i++)
		{
			X[i]=X0+i*step;
			xvar=X[i];
			Y[i]=parser.Eval();
		}
		addResultCurve(n1, X, Y, colorIndex, "Fit"+QString::number(fitID), tr("Non-linear fit of ")+curve);

		result =outputFitString(n, tolerance, X0, XN, points, solver, status, par, s, params,
				curve, formula, tr("Non-linear"));
	}
	return  result;
}	

QString Graph::fitExpDecay(const QString& name, double damping, double amplitude, double yOffset, int colorIndex)
{  
	int n, start, end;
	QwtPlotCurve *c= getValidCurve(name, 4, n, start, end);
	if (!c)
		return QString::null;

	return fitExpDecay(c, damping, amplitude, yOffset, start, end, 1000, 0, 1e-4, colorIndex);
}

QString Graph::fitExpDecay(const QString& name, double damping, double amplitude, double yOffset,
		double from, double to, int iterations, int solver, double tolerance, int colorIndex)
{  
	int start, end;
	QwtPlotCurve* c = getFitLimits(name, from, to, 3, start, end);
	if (!c)
		return "";	
	return fitExpDecay(c,damping,amplitude,yOffset,start,end,iterations,solver,tolerance, colorIndex);
}

QString Graph::fitExpDecay(QwtPlotCurve *curve, double damping, double amplitude, double yOffset,
		int start, int end, int iterations, int solver, double tolerance, int colorIndex)
{  
	int n = end - start + 1;
	double *X = vector(0, n-1);
	double *Y = vector(0, n-1);
	int i, aux = 0;
	for (i = start; i <= end; i++)
	{// This is the data to be fitted 
		X[aux]=curve->x(i);
		Y[aux]=curve->y(i);
		aux++;
	}
	const size_t p = 3;	
	struct FitData d = {n, X, Y};
	double x_init[3] = {amplitude, 1/damping, yOffset};
	int status;
	double *par=vector(0,p-1);
	QString result;
	int n1 = (n<100)?100:n;
	double X0=X[0];
	double XN=X[n-1];
	double step=(XN-X0)/(n1-1);

	QStringList params;
	params << "A" << "t" << "y0";


	if(solver==2)
	{
		gsl_multimin_function f;
		f.f = &exp_d;
		f.n = p;
		f.params = &d;
		gsl_multimin_fminimizer *s =  fitSimplex(f, x_init, tolerance, iterations, status);	

		//allocate and evaluate jacobian of residual 
		gsl_matrix *J = gsl_matrix_alloc(n,p);
		exp_df(s->x,(void*)f.params,J);

		for (i=0;i<(int)p;i++)
			par[i]=gsl_vector_get(s->x,i); 
		par[1]=1.0/par[1];
		result = outputFitString(n, tolerance, X0, XN, iterations, J, status, par, s, params,
				curve->title().text(),  "y=Aexp(-x/t)+y0", "Exponential decay");
		par[1]=1.0/par[1];
	}
	else
	{
		gsl_multifit_function_fdf f;
		f.f = &exp_f;
		f.df = &exp_df;
		f.fdf = &exp_fdf;
		f.n = n;
		f.p = p;
		f.params = &d;

		gsl_multifit_fdfsolver *s = fitGSL(f, p, n, x_init, solver, tolerance, iterations, status);	

		for (i=0;i<(int)p;i++)
			par[i]=gsl_vector_get(s->x,i);
		par[1]=1.0/par[1];
		result=outputFitString(n, tolerance, X0, XN, iterations, solver, status, par, s, params,
				curve->title().text(), "y=Aexp(-x/t)+y0", "Exponential decay");
		par[1]=1.0/par[1];
	}

	free_vector(X,0,n-1); free_vector(Y,0,n-1);	
	X=vector(0,n1-1); Y=vector(0,n1-1);
	for (i=0;i<n1;i++)
	{
		X[i]=X0+i*step;
		Y[i]=par[0]*exp(-par[1]*X[i])+par[2];
	}
	addResultCurve(n1, X, Y, colorIndex, "Fit"+QString::number(fitID), tr("Exponential decay fit of ")+curve->title().text());

	return result;
}

QString Graph::fitExpDecay2(const QString& name, double firstTime, double secondTime,
		double from, double yOffset, int colorIndex)
{
	int start, end;
	QwtPlotCurve* c = getFitLimits(name, from, from - 1, 5, start, end);
	if (!c)
		return "";	
	return fitExpDecay2(c, 1.0, firstTime, 1.0, secondTime, yOffset, start, end, 1000, 0, 1e-4, colorIndex);
}

QString Graph::fitExpDecay2(const QString& name, double amp1, double t1, double amp2, double t2, double yOffset,
		double from, double to, int iterations, int solver, double tolerance, int colorIndex)
{
	int start, end;
	QwtPlotCurve* c = getFitLimits(name, from, to, 5, start, end);
	if (!c)
		return "";	
	return fitExpDecay2(c, amp1, t1, amp2, t2, yOffset, start, end, iterations, solver, tolerance, colorIndex);
}

QString Graph::fitExpDecay2(QwtPlotCurve *curve, double amp1, double t1, double amp2, double t2, double yOffset,
		int start, int end, int iterations, int solver, double tolerance, int colorIndex)
{
	int n = end - start + 1;
	double *X = vector(0, n-1);
	double *Y = vector(0, n-1);
	int i, aux = 0;
	for (i = start; i <= end; i++)
	{// This is the data to be fitted 
		X[aux]=curve->x(i);
		Y[aux]=curve->y(i);
		aux++;
	}
	struct FitData d = {n, X, Y};
	const size_t p = 5;		  
	double x_init[5] = {amp1, 1.0/t1, amp2, 1.0/t2, yOffset};
	int status;
	double *par=vector(0,p-1);
	int n1 = (n<100)?100:n;
	double X0=X[0];
	double XN=X[n-1];
	double step=(XN-X0)/(n1-1);
	QStringList params;
	params << "A1" << "t1" << "A2" << "t2" << "y0";
	QString result;


	if(solver==2)
	{
		gsl_multimin_function f;
		f.f = &expd2_d;
		f.n = p;
		f.params = &d;
		gsl_multimin_fminimizer *s =  fitSimplex(f, x_init, tolerance, iterations, status);	

		//allocate and evaluate jacobian of residual 
		gsl_matrix *J = gsl_matrix_alloc(n,p);
		expd2_df(s->x,(void*)f.params,J);

		for (i=0;i<(int)p;i++)
			par[i]=gsl_vector_get(s->x,i); 
		par[1]=1.0/par[1];
		par[3]=1.0/par[3];

		result = outputFitString(n, tolerance, X0, XN, iterations, J, status, par, s, params,
				curve->title().text(),  "y=A1*exp(-x/t1)+A2*exp(-x/t2)+y0", "Exponential decay");
		par[1]=1.0/par[1];
		par[3]=1.0/par[3];

	}
	else
	{
		gsl_multifit_function_fdf f;
		f.f = &expd2_f;
		f.df = &expd2_df;
		f.fdf = &expd2_fdf;
		f.n = n;
		f.p = p;
		f.params = &d;


		gsl_multifit_fdfsolver *s = fitGSL(f, p, n, x_init, solver, tolerance, iterations, status);	

		for (i=0;i<(int)p;i++)
			par[i]=gsl_vector_get(s->x,i);
		par[1]=1.0/par[1];
		par[3]=1.0/par[3];

		result = outputFitString(n, tolerance, X0, XN, iterations, solver, status, par, s, params,
				curve->title().text(), "y=A1*exp(-x/t1)+A2*exp(-x/t2)+y0", "Exponential decay");
		par[1]=1.0/par[1];
		par[3]=1.0/par[3];

	}


	free_vector(X,0,n-1); free_vector(Y,0,n-1);	
	X=vector(0,n1-1); Y=vector(0,n1-1);
	for (i=0; i<n1; i++)
	{
		X[i]=X0+i*step;
		Y[i]=par[0]*exp(-par[1]*X[i])+par[2]*exp(-par[3]*X[i])+par[4];
	}

	addResultCurve(n1, X, Y, colorIndex, "Fit"+QString::number(fitID), tr("ExpDecay2 fit of ")+curve->title().text());

	return result;
}

QString Graph::fitExpDecay3(const QString& name, double firstTime, double secondTime,
		double thirdTime, double from, double yOffset, int colorIndex)
{ 
	int start, end;
	QwtPlotCurve* c = getFitLimits(name, from, from - 1, 7, start, end);
	if (!c)
		return "";	
	return fitExpDecay3(c, 1.0, firstTime, 1.0, secondTime, 1.0, thirdTime, yOffset,
			start, end, 1000, 0, 1e-4, colorIndex);
}

QString Graph::fitExpDecay3(const QString& name, double amp1, double t1, double amp2, double t2, 
		double amp3, double t3, double yOffset, double from, double to, 
		int iterations, int solver, double tolerance, int colorIndex)
{
	int start, end;
	QwtPlotCurve* c = getFitLimits(name, from, to, 7, start, end);
	if (!c)
		return "";	
	return fitExpDecay3(c, amp1, t1, amp2, t2, amp3, t3, yOffset, 
			start, end, iterations, solver, tolerance, colorIndex);
}

QString Graph::fitExpDecay3(QwtPlotCurve *curve, double amp1, double t1, double amp2, double t2, 
		double amp3, double t3, double yOffset, int start, int end, 
		int iterations, int solver, double tolerance, int colorIndex)
{
	int n = end - start + 1;
	double *X = vector(0, n-1);
	double *Y = vector(0, n-1);
	// This is the data to be fitted 
	int i, aux = 0;
	for (i = start; i <= end; i++)
	{
		X[aux]=curve->x(i);
		Y[aux]=curve->y(i);
		aux++;
	}
	struct FitData d = {n, X, Y};
	const size_t p = 7;		
	double x_init[7] = { amp1, 1.0/t1, amp2, 1.0/t2, amp3, 1.0/t3, yOffset};
	int status;
	double *par=vector(0,p-1);
	int n1 = (n<100)?100:n;
	double X0=X[0];
	double XN=X[n-1];
	double step=(XN-X0)/(n1-1);
	QString result;
	QStringList params;
	params << "A1" << "t1" << "A2" << "t2" << "A3" << "t3" << "y0";

	if(solver ==2)
	{
		gsl_multimin_function f;
		f.f = &expd3_d;
		f.n = p;
		f.params = &d;
		gsl_multimin_fminimizer *s =  fitSimplex(f, x_init, tolerance, iterations, status);	

		//allocate and evaluate jacobian of residual 
		gsl_matrix *J = gsl_matrix_alloc(n,p);
		expd3_df(s->x,(void*)f.params,J);

		for (i=0;i<(int)p;i++)
			par[i]=gsl_vector_get(s->x,i); 
		par[1]=1.0/par[1];
		par[3]=1.0/par[3];
		par[5]=1.0/par[5];

		result = outputFitString(n, tolerance, X0, XN, iterations, J, status, par, s, params,
				curve->title().text(),  "y=A1*exp(-x/t1)+A2*exp(-x/t2)+A3*exp(-x/t3)+y0",
				"Exponential decay");
		par[1]=1.0/par[1];
		par[3]=1.0/par[3];
		par[5]=1.0/par[5];
	}
	else
	{
		gsl_multifit_function_fdf f;
		f.f = &expd3_f;
		f.df = &expd3_df;
		f.fdf = &expd3_fdf;
		f.n = n;
		f.p = p;
		f.params = &d;


		gsl_multifit_fdfsolver *s = fitGSL(f, p, n, x_init, solver, tolerance, iterations, status);	

		for (i=0;i<(int)p;i++)
			par[i]=gsl_vector_get(s->x,i);

		par[1]=1.0/par[1];
		par[3]=1.0/par[3];
		par[5]=1.0/par[5];

		result = outputFitString(n, tolerance, X0, XN, iterations, solver, status, par, s, params,
				curve->title().text(), "y=A1*exp(-x/t1)+A2*exp(-x/t2)+A3*exp(-x/t3)+y0",
				"Exponential decay");	
		par[1]=1.0/par[1];
		par[3]=1.0/par[3];
		par[5]=1.0/par[5];
	}

	free_vector(X,0,n-1); free_vector(Y,0,n-1);	
	X=vector(0,n1-1); Y=vector(0,n1-1);
	for (i=0;i<n1;i++)
	{
		X[i]=X0+i*step;
		Y[i]=par[0]*exp(-X[i]*par[1])+par[2]*exp(-X[i]*par[3])+par[4]*exp(-X[i]*par[5])+par[6];
	}
	addResultCurve(n1, X, Y, colorIndex, "Fit"+QString::number(fitID), tr("ExpDecay3 fit of ")+ curve->title().text());
	return result;
}

QString Graph::fitExpGrowth(const QString& name, double damping, double amplitude, double yOffset, int colorIndex)
{   
	int n, start, end;
	QwtPlotCurve *c= getValidCurve(name, 4, n, start, end);
	if (!c)
		return QString::null;

	return fitExpGrowth(c, damping, amplitude, yOffset, start, end, 1000, 0, 1e-4, colorIndex);
}

QString Graph::fitExpGrowth(const QString& name, double damping, double amplitude, double yOffset,
		double from, double to, int iterations, int solver, double tolerance, int colorIndex)
{
	int start, end;
	QwtPlotCurve* c = getFitLimits(name, from, to, 4, start, end);
	if (!c)
		return "";

	return fitExpGrowth(c,damping,amplitude,yOffset,start,end,iterations,solver,tolerance, colorIndex);
}

QString Graph::fitExpGrowth(QwtPlotCurve *curve, double damping, double amplitude, double yOffset,
		int start, int end, int iterations, int solver, double tolerance, int colorIndex)
{
	int n = end - start + 1;
	double *X = vector(0, n-1);
	double *Y = vector(0, n-1);
	int i, aux = 0;
	for (i = start; i <= end; i++)
	{// This is the data to be fitted 
		X[aux]=curve->x(i);
		Y[aux]=curve->y(i);
		aux++;
	}
	const size_t p = 3;	
	struct FitData d = {n, X, Y};
	double x_init[3] = {amplitude, -1/damping, yOffset};
	int status;
	double *par=vector(0,p-1);
	QStringList params;
	params << "A" << "t" << "y0";
	QString result;
	int n1 = (n<100)?100:n;
	double X0=X[0];
	double XN=X[n-1];
	double step=(XN-X0)/(n1-1);

	if(solver ==2)
	{
		gsl_multimin_function f;
		f.f = &exp_d;
		f.n = p;
		f.params = &d;
		gsl_multimin_fminimizer *s =  fitSimplex(f, x_init, tolerance, iterations, status);	

		//allocate and evaluate jacobian of residual 
		gsl_matrix *J = gsl_matrix_alloc(n,p);
		exp_df(s->x,(void*)f.params,J);

		for (i=0;i<(int)p;i++)
			par[i]=gsl_vector_get(s->x,i); 
		par[1]=-1.0/par[1];
		result = outputFitString(n, tolerance, X0, XN, iterations, J, status, par, s, params,
				curve->title().text(),  "y=Aexp(-x/t)+y0", "Exponential decay");
		par[1]=-1.0/par[1];

	}
	else
	{
		gsl_multifit_function_fdf f;
		f.f = &exp_f;
		f.df = &exp_df;
		f.fdf = &exp_fdf;
		f.n = n;
		f.p = p;
		f.params = &d;


		gsl_multifit_fdfsolver *s = fitGSL(f, p, n, x_init, solver, tolerance, iterations, status);	


		for (i=0;i<(int)p;i++)
			par[i]=gsl_vector_get(s->x,i);

		par[1]=-1.0/par[1];
		result = outputFitString(n, tolerance, X0, XN, iterations, solver, status, par, s, params,
				curve->title().text(), "y=Aexp(x/t)+y0", "Exponential growth");
		par[1]=-1.0/par[1];
	}
	free_vector(X,0,n-1); free_vector(Y,0,n-1);	
	X=vector(0,n1-1); Y=vector(0,n1-1);
	for (i=0;i<n1;i++)
	{
		X[i]=X0+i*step;
		Y[i]=par[0]*exp(-par[1]*X[i])+par[2];
	}
	addResultCurve(n1, X, Y, colorIndex, "Fit"+QString::number(fitID), tr("Exponential growth fit of ")+ curve->title().text());

	return result;
}

QString Graph::fitGauss(const QString& curveTitle)
{ 
	int n, start, end;
	QwtPlotCurve *c= getValidCurve(curveTitle, 4, n, start, end);
	if (!c)
		return QString::null;

	gsl_vector *X = gsl_vector_alloc (n);
	gsl_vector *Y = gsl_vector_alloc (n);
	int aux = 0;
	for (int i = start; i <= end; i++)
	{// This is the data to be fitted 
		gsl_vector_set (X, aux, c->x(i));
		gsl_vector_set (Y, aux, c->y(i));
		aux++;
	}

	double min_out, max_out;
	gsl_vector_minmax (Y, &min_out, &max_out);
	double maxX = gsl_vector_get (X, gsl_vector_max_index (Y));
	gsl_vector_free (X);
	gsl_vector_free (Y);

	return fitGauss(c, max_out - min_out, maxX, 1.0, min_out, start, end, 1000, 0, 1e-4, 1);
}

QString Graph::fitGauss(const QString& name, double amplitude, double center, double width, double offset,
		double from, double to, int iterations, int solver, double tolerance, int colorIndex)
{
	int start, end;
	QwtPlotCurve* c = getFitLimits(name, from, to, 4, start, end);
	if (!c)
		return "";
	return fitGauss(c, amplitude, center, width, offset, start, end, iterations, solver, tolerance, colorIndex);
}

QString Graph::fitGauss(QwtPlotCurve *curve, double amplitude, double center, double width, double offset,
		int start, int end, int iterations, int solver, double tolerance, int colorIndex)
{
	int n = end - start + 1;
	double *X = vector(0, n-1);
	double *Y = vector(0, n-1);
	int i, aux = 0;
	for (i = start; i <= end; i++)
	{// This is the data to be fitted 
		X[aux]=curve->x(i);
		Y[aux]=curve->y(i);
		aux++;
	}
	const size_t p = 4;
	struct FitData d = {n, X, Y};
	double x_init[4] = {offset, amplitude, center, width};
	QString result;
	int status;
	double *par=vector(0,p-1);
	QStringList params;
	params << "y0 (offset)" << "A (height)" << "xc (center)" << "w (width)";
	double X0 =	X[0];
	double XN =	X[n-1];

	if(solver==2)
	{
		gsl_multimin_function f;
		f.f = &gauss_d;
		f.n = p;
		f.params = &d;
		gsl_multimin_fminimizer *s =  fitSimplex(f, x_init, tolerance, iterations, status);	

		//allocate and evaluate jacobian of residuals
		gsl_matrix *J = gsl_matrix_alloc(n,p);
		gauss_df(s->x,(void*)f.params,J);

		for (i=0;i<(int)p;i++)
			par[i]=gsl_vector_get(s->x,i); 

		result = outputFitString(n, tolerance, X0, XN, iterations, J, status, par, s, params,
				curve->title().text(), "y=y0+A*exp[-(x-xc)^2/(2*w^2)]", "Gauss");
	}
	else
	{
		gsl_multifit_function_fdf f;
		f.f = &gauss_f;
		f.df = &gauss_df;
		f.fdf = &gauss_fdf;
		f.n = n;
		f.p = p;
		f.params = &d;

		gsl_multifit_fdfsolver *s = fitGSL(f, p, n, x_init, solver, tolerance, iterations, status);	

		for (i=0;i<(int)p;i++)
			par[i]=gsl_vector_get(s->x,i); 

		result = outputFitString(n, tolerance, X0, XN, iterations, solver, status, par, s, params,
				curve->title().text(), "y=y0+A*exp[-(x-xc)^2/(2*w^2)]", "Gauss");

	}

	int n1 = (n<100)?100:n;
	double step=(XN-X0)/(n1-1);
	free_vector(X,0,n-1); free_vector(Y,0,n-1);
	X=vector(0,n1-1); Y=vector(0,n1-1);

	double w2 = par[3]*par[3];
	for (i=0;i<n1;i++)
	{
		X[i]=X0+i*step;
		double diff=X[i]-par[2];
		Y[i]=par[1]*exp(-0.5*diff*diff/w2)+par[0];
	}
	addResultCurve(n1, X, Y, colorIndex, "Fit"+QString::number(fitID), tr("Gauss fit of ")+ curve->title().text());

	return result;
}

QString Graph::fitLorentz(const QString& curveTitle)
{ 
	int n, start, end;
	QwtPlotCurve *c= getValidCurve(curveTitle, 4, n, start, end);
	if (!c)
		return QString::null;

	gsl_vector *X = gsl_vector_alloc (n);
	gsl_vector *Y = gsl_vector_alloc (n);
	int aux = 0;
	for (int i = start; i <= end; i++)
	{// This is the data to be fitted 
		gsl_vector_set (X, aux, c->x(i));
		gsl_vector_set (Y, aux, c->y(i));
		aux++;
	}

	double minY = gsl_vector_min(Y);
	double maxX = gsl_vector_get (X, gsl_vector_max_index (Y));

	gsl_vector_free (X);
	gsl_vector_free (Y);
	return fitLorentz(c, 1.0, maxX, 1.0, minY, start, end, 1000, 0, 1e-4, 1);
}

QString Graph::fitLorentz(const QString& name, double amplitude, double center, double width, double offset,
		double from, double to, int iterations, int solver, double tolerance, int colorIndex)
{
	int start, end;
	QwtPlotCurve* c = getFitLimits(name, from, to, 4, start, end);
	if (!c)
		return "";
	return fitLorentz(c,amplitude,center,width,offset,start,end,iterations,solver,tolerance, colorIndex);
}

QString Graph::fitLorentz(QwtPlotCurve *curve, double amplitude, double center, double width, double offset,
		int start, int end, int iterations, int solver, double tolerance, int colorIndex)
{
	int n = end - start + 1;
	double *X=vector(0, n-1);
	double *Y=vector(0, n-1);
	int i, aux = 0;
	for (i = start; i <= end; i++)
	{// This is the data to be fitted 
		X[aux]=curve->x(i);
		Y[aux]=curve->y(i);
		aux++;
	}
	const size_t p = 4;
	struct FitData d = {n, X, Y};
	double x_init[4] = {offset, amplitude, center, width};
	int status;
	double *par=vector(0,p-1);
	QStringList params;
	params << "y0 (offset)" << "A (area)" << "xc (center)" << "w (width)";
	double X0 =	X[0];
	double XN =	X[n-1];
	QString result;

	if(solver==2)
	{
		gsl_multimin_function f;
		f.f = &lorentz_d;
		f.n = p;
		f.params = &d;
		gsl_multimin_fminimizer *s =  fitSimplex(f, x_init, tolerance, iterations, status);	

		//allocate and evaluate jacobian of residuals
		gsl_matrix *J = gsl_matrix_alloc(n,p);
		lorentz_df(s->x,(void*)f.params,J);

		for (i=0;i<(int)p;i++)
			par[i]=gsl_vector_get(s->x,i); 
		par[1]= M_PI_2*par[1];


		result = outputFitString(n, tolerance, X0, XN, iterations, J, status, par, s, params,
				curve->title().text(),  "y=y0+2A/pi*w/[4*(x-xc)^2+w^2)]", "Lorentz");
		par[1]=par[1]/M_PI_2;
	}
	else
	{
		gsl_multifit_function_fdf f;
		f.f = &lorentz_f;
		f.df = &lorentz_df;
		f.fdf = &lorentz_fdf;
		f.n = n;
		f.p = p;
		f.params = &d;

		gsl_multifit_fdfsolver *s = fitGSL(f, p, n, x_init, solver, tolerance, iterations, status);	

		for (i=0;i<(int)p;i++)
			par[i]=gsl_vector_get(s->x,i); 
		par[1]= M_PI_2*par[1];

		result = outputFitString(n, tolerance, X0, XN, iterations, solver, status, par, s, params,
				curve->title().text(), "y=y0+2A/pi*w/[4*(x-xc)^2+w^2)]", "Lorentz");
		par[1]=par[1]/M_PI_2;

	}

	int n1 = (n<100)?100:n;
	double step=(XN-X0)/(n1-1);
	free_vector(X,0,n-1); free_vector(Y,0,n-1);
	X=vector(0,n1-1); Y=vector(0,n1-1);

	double aw = par[1]*par[3];
	double w2 = par[3]*par[3];
	for (i=0;i<n1;i++)
	{
		X[i]=X0+i*step;
		double diff=X[i]-par[2];
		Y[i]=aw/(4*diff*diff+w2)+par[0];
	}
	addResultCurve(n1, X, Y, colorIndex, "Fit"+QString::number(fitID), tr("Lorentz fit of ")+curve->title().text());

	return result;
}

QwtPlotCurve* Graph::getValidCurve(const QString& name, int params, 
		int &points, int &start, int &end)
{
	QStringList cl = curvesList();
	int index=cl.findIndex(name);
	if (index < 0)
		return 0;

	QwtPlotCurve *c = curve(index);
	if (!c)
		return 0;

	if (rangeSelectorsEnabled)
	{
		start = QMIN(startPoint, endPoint);
		end = QMAX(startPoint, endPoint);
	}
	else
	{
		start = 0;
		end = c->dataSize() - 1;
	}

	points = abs(end - start) + 1;
	if (points < params)
	{
		QMessageBox::critical(this,tr("QtiPlot - Warning"),
				tr("You need at least %1 points to perform this operation! Operation aborted!").arg(QString::number(params)));
		return 0;
	}
	return c;
}

QwtPlotCurve* Graph::getFitLimits(const QString& name, double from, double to,
		int params, int &start, int &end)
{
	QStringList cl = curvesList();
	int index=cl.findIndex(name);
	if (index < 0)
		return 0;

	QwtPlotCurve *c = curve(index);
	if (!c)
		return 0;

	int i,n = c->dataSize();
	if (n < params)
	{
		QMessageBox::critical(this,tr("QtiPlot - Warning"),
				tr("You need at least %1 points to perform this operation! Operation aborted!").arg(QString::number(params)));
		return 0;
	}

	for (i = 0; i < n; i++)
	{
		if (c->x(i) >= from)
		{
			start = i;
			break;
		}
	}

	if (to < from)
		end = n - 1;
	else
	{
		for (i = n - 1; i >= 0; i--)
		{
			if (c->x(i) <= to)
			{
				end = i;
				break;
			}
		}
	}

	n = abs(end - start) + 1;
	if (n < params)
	{
		QMessageBox::critical(this,tr("QtiPlot - Warning"),
				tr("You need at least %1 points to perform this operation! Operation aborted!").arg(QString::number(params)));
		return 0;
	}
	return c;
}

QString Graph::fitPolynomial(const QString& name,int order, int points,
		double start, double end, bool showFormula, int colorIndex)
{
	int m=order+1;
	QStringList cl = curvesList();	
	int index=cl.findIndex(name);
	QwtPlotCurve *curve= this->curve(index);
	if (!curve)
		return "";

	int i,j;
	int size=curve->dataSize();
	int iStart=0, iEnd=size-1;

	for (i=0;i<size;i++)
	{
		if (curve->x(i)>=start)
		{
			iStart=i;
			break;
		}
	}

	for (i=iEnd;i>=0;i--)
	{
		if (curve->x(i)<=end)
		{
			iEnd=i;
			break;
		}
	}

	const size_t n=iEnd-iStart+1;	
	if ((int)n<m)
	{
		QMessageBox::critical(this, tr("QtiPlot - Warning"),
				tr("You need at least %1 points to perform this operation! Operation aborted!").arg(QString::number(m)));
		return "";
	}

	double xi, yi, chisq;
	gsl_matrix *X, *cov;
	gsl_vector *y, *c, *x;

	X = gsl_matrix_alloc (n, m);
	x = gsl_vector_alloc (n);
	y = gsl_vector_alloc (n);

	c = gsl_vector_alloc (m);
	cov = gsl_matrix_alloc (m,m);

	for (i = 0; i <(int)n; i++)
	{// This is the data to be fitted 
		xi = curve->x(i+iStart);
		yi = curve->y(i+iStart);

		for (j= 0; j < m; j++)
			gsl_matrix_set (X, i, j, pow(xi,j));

		gsl_vector_set (x, i, xi);	
		gsl_vector_set (y, i, yi);
	}

	gsl_multifit_linear_workspace * work = gsl_multifit_linear_alloc (n, m);
	gsl_multifit_linear (X, y, c, cov, &chisq, work);

	QString t=": y=";	
	for (j= 0; j < m; j++)
	{
		t+="a"+QString::number(j);
		if (j>0)
			t+="*X";
		if (j>1)
			t+="^"+QString::number(j);
		t+="+";
	}
	t=t.left(t.length()-1);

	double cj;
	double x0=gsl_vector_get(x,0);
	double xn=gsl_vector_get(x,n-1);		
	double step=double (xn-x0) / double (points-1); 

	QString table_name = "Fit"+ QString::number(fitID);	
	QDateTime dt = QDateTime::currentDateTime();
	QString info=dt.toString(Qt::LocalDate)+"\t"+this->caption()+" "+table_name+ ":\n";
	QString label = tr("Order")+" "+QString::number(order)+" "+tr("Polynomial fit of ")+curve->title().text();
	info+=label+t;
	info+="\nFrom x="+QString::number(x0) +" to x="+QString::number(xn)+"\n";
	QString infoA;
	for (i = 0; i < m; i++)
	{
		double ci=gsl_vector_get(c,i);
		infoA.sprintf("a%d = %.6G +/- %.6G\n", i, ci, sqrt(gsl_matrix_get(cov, i, i)));
		info+=infoA;
	}
	info+="-------------------------------------------------------------\n";
	info+="Chi^2 = "+QString::number(chisq, 'G', 6);
	info+="\n-------------------------------------------------------------\n";

	gsl_multifit_linear_free (work);
	gsl_matrix_free (X);
	gsl_matrix_free (cov);
	gsl_vector_free (y);
	gsl_vector_free (x);

	double *a = vector(0,points-1); 
	double *b = vector(0,points-1);
	QString text="1\t2\n";
	for (i=0;i<points;i++)
	{
		xi=x0+i*step;
		yi=0.0;
		for (j=0; j<m;j++)
		{
			cj=gsl_vector_get(c,j);
			yi+=cj*pow(xi,j);
		}
		a[i]=xi;
		text+=QString::number(xi)+"\t";
		b[i]=yi;
		text+=QString::number(yi)+"\n";
	}

	addResultCurve(points, a, b, colorIndex, table_name, label);	
	updatePlot();

	if (showFormula)
	{
		LegendMarker* aux= new LegendMarker(d_plot);
		QRect rect=QRect(10,10,10,10);
		if (legendMarkerID>=0)
		{
			LegendMarker* mrk=(LegendMarker*) d_plot->marker(legendMarkerID);
			QPoint p=mrk->rect().bottomLeft();
			rect.setTopLeft(QPoint(p.x(),p.y()+10));
		}
		aux->setOrigin(rect.topLeft());

		cj=gsl_vector_get(c,0);
		t= "Y=" + QString::number(cj, 'G', 6);

		for (j= 1; j < m; j++)
		{
			cj=gsl_vector_get(c,j);
			if (cj>0 && !t.isEmpty())
				t+="+";

			QString s;
			s.sprintf("%.5f",cj);	
			if (s != "1.00000")
				t+=QString::number(cj, 'G', 6);

			t+="X";
			if (j>1)
			{
				t+="<sup>";
				t+=QString::number(j);
				t+="</sup>";
			}
		}
		aux->setText(t);	
		d_plot->insertMarker(aux);
		d_plot->replot();		
	}

	gsl_vector_free (c);
	return info;
}

void Graph::calculateLineProfile(const QPoint& start, const QPoint& end)
{	
	ImageMarker* mrk=(ImageMarker*) d_plot->marker(selectedMarker);
	if (!mrk)
	{
		QMessageBox::warning(0,tr("QtiPlot - Pixel selection warning"),  
				"Please select the start line point inside the image rectangle!");
		linesOnPlot--;
		d_plot->removeMarker(lines[linesOnPlot]);
		lines.resize(linesOnPlot);
		return;
	}

	QRect rect=mrk->rect();
	if (!rect.contains(start) || !rect.contains(end))
	{
		QMessageBox::warning(0,tr("QtiPlot - Pixel selection warning"),  
				"Please select the end line point inside the image rectangle!");
		linesOnPlot--;
		d_plot->removeMarker(lines[linesOnPlot]);
		lines.resize(linesOnPlot);		
		return;		
	}

	QPoint o=mrk->getOrigin();
	QPixmap pic=mrk->image();
	QImage image=pic.convertToImage();
	lineProfileOn=FALSE;

	int x1=start.x()-o.x();
	int x2=end.x()-o.x();
	int y1=start.y()-o.y();
	int y2=end.y()-o.y();

	QSize realSize=pic.size();
	QSize actualSize=mrk->size();

	if (realSize != actualSize)
	{
		double ratioX= (double)realSize.width()/(double)actualSize.width();
		double ratioY= (double)realSize.height()/(double)actualSize.height();
		x1=int(x1*ratioX); 
		x2=int(x2*ratioX);
		y1=int(y1*ratioY); 
		y2=int(y2*ratioY); 	
	}

	QString text="pixel\tx\ty\tintensity\n";
	//uses the fast Bresenham's line-drawing algorithm
#define sgn(x) ((x<0)?-1:((x>0)?1:0)) 	
	int i,dx,dy,sdx,sdy,dxabs,dyabs,x,y,px,py,n;

	dx=x2-x1;      //the horizontal distance of the line 
	dy=y2-y1;      //the vertical distance of the line 
	dxabs=abs(dx);
	dyabs=abs(dy);
	sdx=sgn(dx);
	sdy=sgn(dy);
	x=dyabs>>1;
	y=dxabs>>1;
	px=x1;
	py=y1;

	if (dxabs>=dyabs) //the line is more horizontal than vertical 
	{
		for(i=0;i<dxabs;i++)
		{
			y+=dyabs;
			if (y>=dxabs)
			{
				y-=dxabs;
				py+=sdy;
			}
			px+=sdx;

			n=dxabs;
			text+=QString::number(i)+"\t";
			text+=QString::number(px)+"\t";
			text+=QString::number(py)+"\t";
			text+=QString::number(averageImagePixel(image, px, py, averagePixels, TRUE))+"\n"; 
		}
	}
	else // the line is more vertical than horizontal 
	{
		for(i=0;i<dyabs;i++)
		{
			x+=dxabs;
			if (x>=dyabs)
			{
				x-=dyabs;
				px+=sdx;
			}
			py+=sdy;

			n=dyabs;
			text+=QString::number(i)+"\t";	  
			text+=QString::number(px)+"\t";
			text+=QString::number(py)+"\t";
			text+=QString::number(averageImagePixel(image, px, py, averagePixels, FALSE))+"\n"; 
		}
	}
	QString caption="table1";
	emit createTablePlot(caption,n,4, text);
}

int Graph::averageImagePixel(const QImage& image, int px, int py, int average, bool moreHorizontal)
{
	QRgb pixel;
	int sum=0,start,i;
	int middle=int(0.5*(average-1));
	if (moreHorizontal)
	{
		start=py-middle;
		for(i=0;i<average;i++)
		{
			pixel= image.pixel(px,start+i);	
			sum+=qGray(pixel);
		}
	}
	else
	{
		start=px-middle;
		for(i=0;i<average;i++)
		{
			pixel= image.pixel(start+i,py);	
			sum+=qGray(pixel);
		}
	}
	return sum/average;
}

void Graph::calculateProfile(int average, bool ok)
{
	lineProfileOn=ok;	
	if (average % 2 == 0)
		averagePixels=average+1;
	else
		averagePixels=average;
}

gsl_multifit_fdfsolver* Graph::fitGSL(gsl_multifit_function_fdf f, int p, int n, 
		double *x_init, int solver, double tolerance,
		int &iterations, int &status)
{
	const gsl_multifit_fdfsolver_type *T;
	if (solver)
		T = gsl_multifit_fdfsolver_lmder;
	else
		T = gsl_multifit_fdfsolver_lmsder;

	gsl_multifit_fdfsolver *s = gsl_multifit_fdfsolver_alloc (T, n, p);
	gsl_vector_view x = gsl_vector_view_array (x_init, p);
	gsl_multifit_fdfsolver_set (s, &f, &x.vector);

	size_t iter = 0;
	do
	{
		iter++;
		status = gsl_multifit_fdfsolver_iterate (s);

		if (status)
			break;

		status = gsl_multifit_test_delta (s->dx, s->x, tolerance, tolerance);
	}
	while (status == GSL_CONTINUE && (int)iter < iterations);
	iterations = iter;
	return s;
}

gsl_multimin_fminimizer* Graph::fitSimplex(gsl_multimin_function f, 
		double *x_init, double tolerance,
		int &iterations, int &status)
{
	const gsl_multimin_fminimizer_type *T = gsl_multimin_fminimizer_nmsimplex;

	//size of the simplex
	gsl_vector *ss;
	//initial vertex size vector
	ss = gsl_vector_alloc (f.n);
	//set all step sizes to 1 can be increased to converge faster
	gsl_vector_set_all (ss,10.0);
	gsl_multimin_fminimizer *s =gsl_multimin_fminimizer_alloc (T, f.n);
	gsl_vector_view x = gsl_vector_view_array (x_init,f.n);

	status = gsl_multimin_fminimizer_set (s, &f, &x.vector,ss);
	double size;
	size_t iter = 0;
	do
	{
		iter++;
		status = gsl_multimin_fminimizer_iterate (s);

		if (status)
			break;
		size=gsl_multimin_fminimizer_size (s);
		status = gsl_multimin_test_size (size, tolerance);
	}

	while (status == GSL_CONTINUE && (int)iter < iterations);

	iterations = iter;
	gsl_vector_free(ss);
	return s;
}


QString Graph::fitPluginFunction(const QString& curve,const QString& pluginName,
		const QStringList& paramsInit, double from,
		double to,int points, int solver, double tolerance, int colorIndex)
{
	if (!QFile::exists (pluginName))
	{
		QMessageBox::critical(this,tr("QtiPlot - File not found"),
				tr("Plugin file: <p><b> %1 </b> <p>not found. Operation aborted!").arg(pluginName));
		return tr("Error when loading plugin!\n");
	}

	QStringList cl = curvesList();
	int index=cl.findIndex(curve);
	QwtPlotCurve *c=this->curve(index);
	int n=c->dataSize();
	const size_t p = paramsInit.count();
	if (n < (int) p)
	{
		QMessageBox::critical(this,tr("QtiPlot - Warning"),
				tr("You need at least %1 points to perform the fit! Operation aborted!").arg(QString::number(p)));
		return "";
	}
	int i,size=0;
	for (i = 0; i < n; i++)
	{
		if (c->x(i) >= from && c->x(i) <= to)
			size++;
	}
	double *X=vector(0,size-1);
	double *Y=vector(0,size-1);
	size=0;
	for (i = 0; i <n; i++)
	{
		if (c->x(i)>=from && c->x(i) <=to)
		{// This is the data to be fitted 
			X[size]=c->x(i);
			Y[size]=c->y(i);
			size++;
		}
	}
	n=size;

	QLibrary lib(pluginName);
	lib.setAutoUnload(true);
	QStringList params;
	double *x_init = vector(0,p-1);
	int status;
	QString result;
	double *par = vector(0,p-1);
	double X0 =	X[0];
	double XN =	X[n-1];
	int n1 = (n<100)?100:n;
	double step=(XN-X0)/(n1-1);

	if(solver==2)
	{
		typedef char* (*fitFunc)();
		fitFunc fitFunction = (fitFunc) lib.resolve("parameters");
		if (fitFunction)
			params = QStringList::split(",", QString(fitFunction()), false);
		else
			return tr("Error when loading plugin!\n");

		for (i=0;i<(int)p;i++)
			x_init[i]=paramsInit[i].toDouble();

		struct FitData d = {n, X, Y};
		gsl_multimin_function f;
		typedef double (*fitD)(const gsl_vector *, void *);
		fitD fit_d = (fitD) lib.resolve( "function_d" );
		if (fit_d)
			f.f = fit_d;
		else
			return tr("Error when loading plugin!");

		typedef int (*fitDf)(const gsl_vector *, void *, gsl_matrix *);
		fitDf fit_df = (fitDf) lib.resolve( "function_df" );
		if(fit_d);
		else
			return tr("Error when loading plugin!\n");

		f.n = p;
		f.params = &d;

		gsl_multimin_fminimizer *s = fitSimplex(f, x_init, tolerance, points, status);

		for (i=0;i<(int)p;i++)
			par[i]=gsl_vector_get(s->x,i);

		fitFunction = (fitFunc) lib.resolve( "function" );
		QString formula;
		if (fitFunction)
			formula = QString(fitFunction());
		else
			return tr("Error when loading plugin!\n");

		fitFunction = (fitFunc) lib.resolve( "name" );
		QString fname;
		if (fitFunction)
			fname = QString(fitFunction());

		gsl_matrix *J = gsl_matrix_alloc(n,p);
		fit_df(s->x,(void*)f.params,J);

		result = outputFitString(n, tolerance, X0, XN, points, J, status, par, s, params,
				curve, fname+" = "+formula, "Non-linear");

	}
	else
	{

		typedef char* (*fitFunc)();
		fitFunc fitFunction = (fitFunc) lib.resolve("parameters");
		if (fitFunction)
			params = QStringList::split(",", QString(fitFunction()), false);
		else
			return tr("Error when loading plugin!\n");

		for (i=0;i<(int)p;i++)
			x_init[i]=paramsInit[i].toDouble();

		struct FitData d = {n, X, Y};
		gsl_multifit_function_fdf f;

		typedef int (*fitF)(const gsl_vector *, void *, gsl_vector *);
		fitF fit_f = (fitF) lib.resolve( "function_f" );
		if (fit_f)
			f.f = fit_f;
		else
			return tr("Error when loading plugin!");

		typedef int (*fitD)(const gsl_vector *, void *);
		fitD fit_d = (fitD) lib.resolve( "function_d" );
		if (fit_d)
			f.f = fit_f;
		else
			return tr("Error when loading plugin!");

		typedef int (*fitDf)(const gsl_vector *, void *, gsl_matrix *);
		fitDf fit_df = (fitDf) lib.resolve( "function_df" );
		if (fit_df)
			f.df = fit_df;
		else
			return tr("Error when loading plugin!\n");

		typedef int (*fitFDf)(const gsl_vector *, void *, gsl_vector *, gsl_matrix *);
		fitFDf fit_fdf = (fitFDf) lib.resolve( "function_fdf" );
		if (fit_fdf)
			f.fdf = fit_fdf;
		else
			return tr("Error when loading plugin!\n");

		f.n = n;
		f.p = p;
		f.params = &d;


		gsl_multifit_fdfsolver *s = fitGSL(f, p, n, x_init, solver, tolerance, points, status);

		for (i=0;i<(int)p;i++)
			par[i]=gsl_vector_get(s->x,i);

		fitFunction = (fitFunc) lib.resolve( "function" );
		QString formula;
		if (fitFunction)
			formula = QString(fitFunction());
		else
			return tr("Error when loading plugin!\n");

		fitFunction = (fitFunc) lib.resolve( "name" );
		QString fname;
		if (fitFunction)
			fname = QString(fitFunction());

		result= outputFitString(n, tolerance, X0, XN, points, solver, status, par, s, params,
				curve, fname+" = "+formula, "Non-linear");

	}

	free_vector(X,0,n-1); free_vector(Y,0,n-1);
	X=vector(0,n1-1); Y=vector(0,n1-1);

	typedef double (*fitFunctionEval)(double, double *);
	fitFunctionEval f_eval = (fitFunctionEval) lib.resolve("function_eval");
	if (!f_eval)
		return tr("Error when loading plugin!\n");

	for (i=0;i<n1;i++)
	{
		X[i]=X0+i*step;
		Y[i]= f_eval(X[i], par);
	}

	addResultCurve(n1, X, Y, colorIndex, "Fit"+QString::number(fitID), tr("Non-linear fit of ")+curve);

	return result;
}

QString Graph::outputFitString(int n, double tolerance, double from, double to, int iter,
		int solver, int status, double *params, gsl_multifit_fdfsolver *s, 
		const QStringList& parNames,const QString& curve,
		const QString& f, const QString& fitType)
{
	int p = (int)parNames.count();
	gsl_matrix *covar = gsl_matrix_alloc (p, p);
	gsl_multifit_covar (s->J, 0.0, covar);

	QDateTime dt = QDateTime::currentDateTime ();
	QString info=dt.toString(Qt::LocalDate)+"\t"+this->caption()+" Fit"+ QString::number(fitID)+ ":\n";
	info+=fitType+" "+tr("fit of")+ " "+curve;
	if (!f.isEmpty())
		info+=", "+tr("using function")+" "+f+"\n";
	else
		info+="\n";

	if (solver)
		info+=tr("Unscaled Levenberg-Marquardt");
	else
		info+=tr("Scaled Levenberg-Marquardt");
	info+=tr(" algorithm with tolerance = ")+QString::number(tolerance)+"\n";
	info+=tr("From x=")+QString::number(from) +tr(" to x=")+QString::number(to)+"\n";

	QString info2, error;
	fit_results.clear();
	for (int i=0; i<p; i++)
	{
		info2.sprintf(parNames[i]+"=%.15g +/- %.6g\n",params[i],sqrt(gsl_matrix_get(covar,i,i)));
		info+=info2;
		fit_results << QString::number(params[i], 'g', 15);
	}
	info+="-----------------------------------\n";
	double chi = gsl_blas_dnrm2(s->f);
	info2.sprintf("Chi^2/doF = %g\n",  pow(chi, 2.0)/(n - p));
	info+=info2;
	info+="-----------------------------------\n";
	info+=tr("Iterations = ")+QString::number(iter)+"\n";
	error.sprintf ("Status = %s\n", gsl_strerror (status));
	info+=error;
	info+="---------------------------------------------------------------------------------------\n";

	if (fitType.contains(tr("multi-peak")))
	{
		info+=tr("Peak")+"\t";
		if (fitType.contains(tr("Gauss")))
			info+=tr("Height")+"\t\t";
		else if (fitType.contains(tr("Lorentz")))
			info+=tr("Area")+"\t\t";
		info+=tr("Center")+"\t\t"+tr("Width")+"\n";
		info+="---------------------------------------------------------------------------------------\n";
		for (int j=0; j<n_peaks; j++)
		{
			info+=QString::number(j+1)+"\t"+QString::number(params[3*j],'g',11)+"\t";
			info+=QString::number(params[3*j+1],'g',11)+"\t"+QString::number(params[3*j+2],'g',11)+"\n";
		}
		info+="---------------------------------------------------------------------------------------\n";
	}
	free_vector(params,0,p-1);
	gsl_matrix_free (covar);
	gsl_multifit_fdfsolver_free (s);

	return info;
}

QString Graph::outputFitString(int n, double tolerance, double from, double to, int iter,
		gsl_matrix *J, int status, double *params, gsl_multimin_fminimizer *s, 
		const QStringList& parNames,const QString& curve,
		const QString& f, const QString& fitType)
{
	int p = (int)parNames.count();

	//allocate memory for covariance and calculate based on residuals
	gsl_matrix *covar = gsl_matrix_alloc (p, p);
	gsl_multifit_covar (J, 0.0, covar);

	QDateTime dt = QDateTime::currentDateTime ();
	QString info=dt.toString(Qt::LocalDate)+"\t"+this->caption()+" Fit"+ QString::number(fitID)+ ":\n";
	info+=fitType+" "+tr("fit of")+ " "+curve;
	if (!f.isEmpty())
		info+=", "+tr("using function")+" "+f+"\n";
	else
		info+="\n";

	info+=tr("Nelder-Mead Simplex");
	info+=tr(" algorithm with tolerance = ")+QString::number(tolerance)+"\n";
	info+=tr("From x=")+QString::number(from)+tr(" to x=")+QString::number(to)+"\n";

	QString info2, error;
	fit_results.clear();
	for (int i=0; i<p; i++)
	{
		info2.sprintf(parNames[i]+"=%.15g +/- %.6g\n",params[i],sqrt(gsl_matrix_get(covar,i,i)));
		info+=info2;
		fit_results << QString::number(params[i], 'g', 15);
	}
	info+="-----------------------------------\n";
	info2.sprintf("Chi^2/doF = %g\n",  s->fval/(n - p));
	info+=info2;
	info+="-----------------------------------\n";
	info+=tr("Iterations = ")+QString::number(iter)+"\n";
	error.sprintf ("Status = %s\n", gsl_strerror (status));
	info+=error;
	info+="---------------------------------------------------------------------------------------\n";

	if (fitType.contains(tr("multi-peak")))
	{
		info+=tr("Peak")+"\t";
		if (fitType.contains(tr("Gauss")))
			info+=tr("Height")+"\t\t";
		else if (fitType.contains(tr("Lorentz")))
			info+=tr("Area")+"\t\t";
		info+=tr("Center")+"\t\t"+tr("Width")+"\n";
		info+="---------------------------------------------------------------------------------------\n";
		for (int j=0; j<n_peaks; j++)
		{
			info+=QString::number(j+1)+"\t"+QString::number(params[3*j],'g',11)+"\t";
			info+=QString::number(params[3*j+1],'g',11)+"\t"+QString::number(params[3*j+2],'g',11)+"\n";
		}
		info+="---------------------------------------------------------------------------------------\n";
	}
	free_vector(params,0,p-1);
	gsl_matrix_free (covar);
	gsl_matrix_free (J);
	gsl_multimin_fminimizer_free (s);

	return info;
}

void Graph::addResultCurve(int n, double *x, double *y, int colorIndex,
		const QString& tableName, const QString& legend)
{
	QString label=tableName+"_2";
	QwtPlotCurve *c = new QwtPlotCurve(label);
	long curveID = d_plot->insertCurve(c);
	c->setPen(QPen(color(colorIndex),1)); 

	c_type.resize(++n_curves);
	c_type[n_curves-1]=Line;

	c_keys.resize(n_curves);
	c_keys[n_curves-1] = curveID;

	addLegendItem(label);	
	label=tableName+"_1(X),"+label+"(Y)";
	associations<<label;

	c->setData(x, y, n);	
	QString text="1\t2\n";

	for (int i=0; i<n; i++)
	{
		text+=QString::number(x[i], 'g', 15);
		text+="\t";
		text+=QString::number(y[i], 'g', 15);
		text+="\n";
	}
	free_vector(x,0,n-1);
	free_vector(y,0,n-1);

	emit createHiddenTable(tableName+"\t"+legend, n, 2, text);
	updatePlot();
}

void Graph::smoothSavGol(long curveKey, int order, int nl, int nr, int colIndex)
{
	QwtPlotCurve *curve=d_plot->curve(curveKey);
	if (!curve)
		return;

	QApplication::setOverrideCursor(Qt::waitCursor);

	int i,n=curve->dataSize();
	double *x = vector(0,n-1);
	double *y = vector(0,n-1);
	double *s = vector(0,n-1);
	int np = nl+nr+1;
	double *c = vector(1, np);

	for (i = 0; i<n; i++)
	{// The data to be smoothed 
		x[i]=curve->x(i);
		y[i]=curve->y(i);
	}

	//seek shift index for given case nl, nr, m (see savgol).
	int *index = intvector(1, np);
	index[1]=0;
	int j=3;
	for (i=2; i<=nl+1; i++) 
	{// index(2)=-1; index(3)=-2; index(4)=-3; index(5)=-4; index(6)=-5
		index[i]=i-j;
		j += 2;
	}
	j=2;
	for (i=nl+2; i<=np; i++) 
	{// index(7)= 5; index(8)= 4; index(9)= 3; index(10)=2; index(11)=1
		index[i]=i-j;
		j += 2;
	}

	//calculate Savitzky-Golay filter coefficients.
	savgol(c, np, nl, nr, 0, order);

	for (i=0; i<n; i++) 
	{// Apply filter to input data.
		s[i]=0.0;
		for (j=1; j<=np; j++)
		{
			int it = i+index[j];
			if (it >=0 && it < n)//skip left points that do not exist.
				s[i] += c[j]*y[i+index[j]];
		}
	}

	free_vector(y,0,n-1);
	free_vector(c,1,np);
	free_intvector(index,1,np);

	addResultCurve(n, x, s, colIndex, "Smoothed"+QString::number(++fitID), 
			tr("Savitzky-Golay smoothing of ")+curve->title().text());

	QApplication::restoreOverrideCursor();
}

void Graph::smoothFFT(long curveKey, int points, int colIndex)
{
	QwtPlotCurve *curve=d_plot->curve(curveKey);
	if (!curve)
		return;

	QApplication::setOverrideCursor(Qt::waitCursor);

	int i, n=curve->dataSize();
	double *x = vector(0,n-1);
	double *y = vector(0,n-1);
	for (i = 0; i<n; i++)
	{// The data to be filtered 
		x[i]=curve->x(i);
		y[i]=curve->y(i);
	}

	//smoothFT(y, n, points); //Numerical Receipes method	

	gsl_fft_real_workspace *work = gsl_fft_real_workspace_alloc(n);
	gsl_fft_real_wavetable *real = gsl_fft_real_wavetable_alloc(n);
	gsl_fft_real_transform (y, 1, n, real, work);//FFT forward
	gsl_fft_real_wavetable_free (real);

	double df = 1.0/(double)(x[1]-x[0]);
	double lf = df/(double)points;//frequency cutoff
	df= 0.5*df/(double)n;

	for (i = 0; i < n; i++)
		y[i] = i*df > lf ? 0 : y[i];//filtering frequencies

	gsl_fft_halfcomplex_wavetable *hc = gsl_fft_halfcomplex_wavetable_alloc (n);
	gsl_fft_halfcomplex_inverse (y, 1, n, hc, work);//FFT inverse
	gsl_fft_halfcomplex_wavetable_free (hc);
	gsl_fft_real_workspace_free (work);

	addResultCurve(n, x, y, colIndex, "Smoothed"+QString::number(++fitID), 
			QString::number(points) + " points FFT Smoothing of "+curve->title().text());
	QApplication::restoreOverrideCursor();
}

void Graph::smoothAverage(long curveKey, int points, int colIndex)
{
	QwtPlotCurve *curve=d_plot->curve(curveKey);
	if (!curve)
		return;

	QApplication::setOverrideCursor(Qt::waitCursor);

	int i,j, n=curve->dataSize();
	double *x = vector(0,n-1);
	double *y = vector(0,n-1);
	double *s = vector(0,n-1);

	for (i = 0; i < n; i++)
	{// The data to be smoothed 
		x[i]=curve->x(i);
		y[i] = curve->y(i);
	}

	int p2 = points/2;
	double m = double(2*p2+1);
	double aux = 0.0;

	s[0]=y[0];
	for (i=1; i<p2; i++) 
	{
		aux = 0.0;
		for (j=-i; j<=i; j++)
			aux+= y[i+j];

		s[i]=aux/(double)(2*i+1);
	}
	for (i=p2; i<n-p2; i++) 
	{
		aux = 0.0;
		for (j=-p2; j<=p2; j++)
			aux+= y[i+j];

		s[i]=aux/m;
	}
	for (i=n-p2; i<n-1; i++) 
	{
		aux = 0.0;
		for (j=n-i-1; j>=i-n+1; j--)
			aux+= y[i+j];

		s[i]=aux/(double)(2*(n-i-1)+1);
	}
	s[n-1]=y[n-1];
	free_vector(y,0,n-1);	
	addResultCurve(n, x, s, colIndex, "Smoothed"+QString::number(++fitID), 
			QString::number(points)+" Points Average Smoothing of "+curve->title().text());
	QApplication::restoreOverrideCursor();
}

void Graph::filterFFT(long curveKey, int filter_type, double lf, double hf, bool DCOffset, int colIndex)
{
	QwtPlotCurve *curve=d_plot->curve(curveKey);
	if (!curve)
		return;

	QApplication::setOverrideCursor(Qt::waitCursor);

	int i, n=curve->dataSize();
	double *x = vector(0,n-1);
	double *y = vector(0,n-1);

	for (i = 0; i<n; i++)
	{// The data to be filtered 
		x[i]=curve->x(i);
		y[i]=curve->y(i);
	}

	double df = 0.5/(double)(n*(x[1]-x[0]));//half frequency sampling due to GSL storing

	gsl_fft_real_workspace *work = gsl_fft_real_workspace_alloc(n);
	gsl_fft_real_wavetable * real = gsl_fft_real_wavetable_alloc(n);

	gsl_fft_real_transform (y, 1, n, real, work);
	gsl_fft_real_wavetable_free (real);

	QString label;
	if (filter_type <= 2)
		label = QString::number(lf) + " Hz ";
	else
		label = QString::number(lf)+" to "+QString::number(hf) + " Hz ";

	switch (filter_type)
	{
		case 1://low pass
			label+= tr("Low Pass FFT Filter of ");
			for (i = 0; i < n; i++)
				y[i] = i*df > lf ? 0 : y[i];
			break;

		case 2://high pass
			label+= tr("High Pass FFT Filter of ");
			for (i = 0; i < n; i++)
				y[i] = i*df < lf ? 0 : y[i];
			break;

		case 3://band pass
			label+= tr("Band Pass FFT Filter of ");
			if(DCOffset)
			{
				for (i = 1; i < n; i++)
					y[i] = ((i*df > lf ) && (i*df < hf )) ? y[i] : 0;
			}
			else
			{
				for (i = 0; i < n; i++)
					y[i] = ((i*df > lf ) && (i*df < hf )) ? y[i] : 0;
			}
			break;

		case 4://band block
			label+= tr("Band Block FFT Filter of ");

			if(DCOffset)
				y[0] = 0;//substract DC offset

			for (i = 1; i < n; i++)
				y[i] = ((i*df > lf ) && (i*df < hf )) ? 0 : y[i];
			break;
	}

	gsl_fft_halfcomplex_wavetable *hc = gsl_fft_halfcomplex_wavetable_alloc (n);
	gsl_fft_halfcomplex_inverse (y, 1, n, hc, work);
	gsl_fft_halfcomplex_wavetable_free (hc);
	gsl_fft_real_workspace_free (work);

	addResultCurve(n, x, y, colIndex, "FilteredFFT"+QString::number(++fitID), label+curve->title().text());
	QApplication::restoreOverrideCursor();
}

bool Graph::validCurvesDataSize()
{
	if (!n_curves)
	{
		QMessageBox::warning(this,tr("QtiPlot - Warning"),
				tr("There are no curves available on this plot!"));
		return false;
	}
	else 
	{
		int check=0;
		while(curveDataSize(check)<2)
		{
			if(check==n_curves)
			{
				QMessageBox::warning(this, tr("QtiPlot - Error"),
						tr("There are no curves with more than two points on this plot. Operation aborted!"));
				return false;
			}
			check++;
		}
	}
	return true;
}

QString Graph::fitBoltzmann(const QString& curveTitle)
{ 
	int n, start, end;
	QwtPlotCurve *c= getValidCurve(curveTitle, 4, n, start, end);
	if (!c)
		return QString::null;

	gsl_vector *X = gsl_vector_alloc (n);
	gsl_vector *Y = gsl_vector_alloc (n);
	int aux = 0;
	for (int i = start; i <= end; i++)
	{// This is the data to be fitted 
		gsl_vector_set (X, aux, c->x(i));
		gsl_vector_set (Y, aux, c->y(i));
		aux++;
	}

	double min_out, max_out;
	gsl_vector_minmax (Y, &min_out, &max_out);
	double x0_init = gsl_vector_get (X, start+n/2);
	gsl_vector_free (X);
	gsl_vector_free (Y);

	return fitBoltzmann(c, min_out, max_out, x0_init, 1.0, start, end, 1000, 0, 1e-4, 1);
}

QString Graph::fitBoltzmann(const QString& name, double A1, double A2, double x0, double dx,
		double from, double to, int iterations, int solver, double tolerance, int colorIndex)
{
	int start, end;
	QwtPlotCurve* c = getFitLimits(name, from, to, 4, start, end);
	if (!c)
		return "";
	return fitBoltzmann(c, A1, A2, x0, dx, start, end, iterations, solver, tolerance, colorIndex);
}

QString Graph::fitBoltzmann(QwtPlotCurve *curve, double A1, double A2, double x0, double dx,
		int start, int end, int iterations, int solver, double tolerance, int colorIndex)
{
	int n = end - start + 1;
	double *X = vector(0, n-1);
	double *Y = vector(0, n-1);
	int i, aux = 0;
	for (i = start; i <= end; i++)
	{// This is the data to be fitted 
		X[aux]=curve->x(i);
		Y[aux]=curve->y(i);
		aux++;
	}
	const size_t p = 4;
	struct FitData d = {n, X, Y};
	double x_init[4] = {A1, A2, x0, dx};
	QString result;
	int status;
	double *par=vector(0,p-1);
	QStringList params;
	params << tr("A1 (init value)") << tr("A2 (final value)") << tr("x0 (center)") << tr("dx (time constant)");
	double X0 =	X[0];
	double XN =	X[n-1];

	if(solver==2)
	{
		gsl_multimin_function f;
		f.f = &boltzmann_d;
		f.n = p;
		f.params = &d;
		gsl_multimin_fminimizer *s =  fitSimplex(f, x_init, tolerance, iterations, status);	

		//allocate and evaluate jacobian of residuals
		gsl_matrix *J = gsl_matrix_alloc(n,p);
		boltzmann_df(s->x,(void*)f.params,J);

		for (i=0;i<(int)p;i++)
			par[i]=gsl_vector_get(s->x,i); 

		result = outputFitString(n, tolerance, X0, XN, iterations, J, status, par, s, params,
				curve->title().text(), "y=(A1-A2)/(1+exp((x-x0)/dx))+A2", tr("Boltzmann (Sigmoidal)"));
	}
	else
	{
		gsl_multifit_function_fdf f;
		f.f = &boltzmann_f;
		f.df = &boltzmann_df;
		f.fdf = &boltzmann_fdf;
		f.n = n;
		f.p = p;
		f.params = &d;

		gsl_multifit_fdfsolver *s = fitGSL(f, p, n, x_init, solver, tolerance, iterations, status);	

		for (i=0;i<(int)p;i++)
			par[i]=gsl_vector_get(s->x,i); 

		result = outputFitString(n, tolerance, X0, XN, iterations, solver, status, par, s, params,
				curve->title().text(), "y=(A1-A2)/(1+exp((x-x0)/dx))+A2", tr("Boltzmann (Sigmoidal)"));
	}

	int n1 = (n<100)?100:n;
	double step=(XN-X0)/(n1-1);
	free_vector(X,0,n-1); free_vector(Y,0,n-1);
	X=vector(0,n1-1); Y=vector(0,n1-1);

	for (i=0;i<n1;i++)
	{
		X[i]=X0+i*step;
		Y[i]=(par[0]-par[1])/(1+exp((X[i]-par[2])/par[3]))+par[1];
	}
	addResultCurve(n1, X, Y, colorIndex, "Fit"+QString::number(fitID), tr("Boltzmann (Sigmoidal) fit of ")+ curve->title().text());
	return result;
}
