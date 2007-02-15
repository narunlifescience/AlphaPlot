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
#include "colorBox.h"
#include "ImageMarker.h"
#include "Histogram.h"
#include "nrutil.h"

#include <qapplication.h>
#include <qdatetime.h> 
#include <qimage.h>
#include <qmessagebox.h>


#include <gsl/gsl_sort.h>
#include <gsl/gsl_spline.h>
#include <gsl/gsl_interp.h>
#include <gsl/gsl_fft_halfcomplex.h>
#include <gsl/gsl_fft_complex.h>
#include <gsl/gsl_histogram.h>

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
	QwtPlotCurve *curve = (QwtPlotCurve *)d_plot->curve(curveKey);
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
	QString name, text;
	if(forward)
	{
		name="ForwardFFT"+QString::number(++fitID);
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
		name="InverseFFT"+QString::number(++fitID);
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

	emit createHiddenTable(name, "", n, 5, text);	
	QApplication::restoreOverrideCursor();
}

void Graph::interpolate(int cindex, int spline, double start, double end,
		int points, int colorIndex)
{
	double *x, *y;
	size_t n = sortedCurveData(cindex, start, end, &x, &y);
	
	gsl_interp_accel *acc= gsl_interp_accel_alloc ();
	const gsl_interp_type *method;
	QString label, wlabel;
	switch(spline)
	{
		case 0:
			method=gsl_interp_linear;
			label="LinearInt";
			wlabel = tr("Linear interpolation of ")+curve(cindex)->title().text();
			break;
		case 1:
			method=gsl_interp_cspline;
			label="CubicInt";
			wlabel = tr("Cubic interpolation of ")+curve(cindex)->title().text();
			break;
		case 2:
			method=gsl_interp_akima;
			label="AkimaInt";
			wlabel = tr("Akima interpolation of ")+curve(cindex)->title().text();
			break;
	}

	gsl_spline *interp = gsl_spline_alloc (method, n);     
	gsl_spline_init (interp, x, y, n);	

	double origin = x[0];
	double step=(x[n-1]-x[0])/(double)(points-1);
	delete[] x;
	delete[] y;

	x = new double[points];
	y = new double[points];
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
	int cindex = curveIndex(curveTitle);
	if (cindex < 0) return false;
	double start, end;
  	range(cindex, &start, &end);
  	double *x, *y;
  	int n = curveData(cindex, start, end, &x, &y);

  	if (n < 4)
  	        {
  	        QMessageBox::critical(this,tr("QtiPlot - Error"),
  	        tr("You need at least %1 points to perform this operation! Operation aborted!").arg(QString::number(4)));
  	        delete[] x;
  	        delete[] y;
            return false;
  	        }

	double *result = new double[n-1];
	for (int i = 1; i < n-1; i++)
		result[i]=0.5*((y[i+1]-y[i])/(x[i+1]-x[i]) + (y[i]-y[i-1])/(x[i]-x[i-1]));

	QString text="x\tderivative\n";
	for (int i = 1; i < n-1; i++)
	{
		text+=QString::number(x[i]);
		text+="\t";
		text+=QString::number(result[i]);
		text+="\n";
	}

	emit createHiddenTable(curve(cindex)->title().text(), tr("Derivative of")+" "+curve(cindex)->title().text(),n-2,2,text);
	delete[] x;
	delete[] y;
	delete[] result;
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

void Graph::calculateLineProfile(const QPoint& start, const QPoint& end)
{	
	ImageMarker* mrk=(ImageMarker*) d_plot->marker(selectedMarker);
	if (!mrk)
	{
		QMessageBox::warning(0,tr("QtiPlot - Pixel selection warning"),  
				"Please select the start line point inside the image rectangle!");

		int linesOnPlot = (int)d_lines.size();
        d_plot->removeMarker(d_lines[--linesOnPlot]);
		d_lines.resize(linesOnPlot);
		return;
	}

	QRect rect=mrk->rect();
	if (!rect.contains(start) || !rect.contains(end))
	{
		QMessageBox::warning(0,tr("QtiPlot - Pixel selection warning"),  
				"Please select the end line point inside the image rectangle!");
		
        int linesOnPlot = (int)d_lines.size();
	    d_plot->removeMarker(d_lines[--linesOnPlot]);
		d_lines.resize(linesOnPlot);		
		return;		
	}

	QPoint o=mrk->origin();
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

void Graph::addResultCurve(int n, double *x, double *y, int colorIndex,
		const QString& tableName, const QString& legend)
{
	QString label=tableName+"_2";
	QwtPlotCurve *c = new QwtPlotCurve(label);
	long curveID = d_plot->insertCurve(c);
	c->setPen(QPen(ColorBox::color(colorIndex),1)); 

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
	delete[] x;
	delete[] y;

	emit createHiddenTable(tableName, legend, n, 2, text);
	updatePlot();
}

void Graph::smoothSavGol(long curveKey, int order, int nl, int nr, int colIndex)
{
	QwtPlotCurve *curve = (QwtPlotCurve *)d_plot->curve(curveKey);
	if (!curve || curve->rtti() != QwtPlotItem::Rtti_PlotCurve)
		return;

	QApplication::setOverrideCursor(Qt::waitCursor);

	int i,n=curve->dataSize();
	double *x = new double[n];
	double *y = new double[n];
	double *s = new double[n];
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

	delete[] y;
	free_vector(c,1,np);

	free_intvector(index,1,np);

	addResultCurve(n, x, s, colIndex, tr("Smoothed")+QString::number(++fitID),
			 tr("Savitzky-Golay smoothing of ")+curve->title().text());

	QApplication::restoreOverrideCursor();
}

void Graph::smoothFFT(long curveKey, int points, int colIndex)
{
	QwtPlotCurve *curve = (QwtPlotCurve *)d_plot->curve(curveKey);
	if (!curve || curve->rtti() != QwtPlotItem::Rtti_PlotCurve)
		return;

	QApplication::setOverrideCursor(Qt::waitCursor);

	int i, n = curve->dataSize();
	double *x = new double[n];
	double *y = new double[n];
	for (i = 0; i<n; i++)
	{// The data to be filtered 
		x[i]=curve->x(i);
		y[i]=curve->y(i);
	}

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

	addResultCurve(n, x, y, colIndex, tr("Smoothed")+QString::number(++fitID),
					QString::number(points)+" "+tr("points")+" "+tr("FFT Smoothing of")+" "+curve->title().text());
	QApplication::restoreOverrideCursor();
}

void Graph::smoothAverage(long curveKey, int points, int colIndex)
{
	QwtPlotCurve *curve = (QwtPlotCurve *)d_plot->curve(curveKey);
	if (!curve || curve->rtti() != QwtPlotItem::Rtti_PlotCurve)
		return;

	QApplication::setOverrideCursor(Qt::waitCursor);

	int i,j, n = curve->dataSize();
	double *x = new double[n];
	double *y = new double[n]; 
	double *s = new double[n];

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
	addResultCurve(n, x, s, colIndex, tr("Smoothed") + QString::number(++fitID),
		QString::number(points)+" "+tr("points")+" "+tr("average smoothing of")+" "+curve->title().text());
	QApplication::restoreOverrideCursor();
}

void Graph::filterFFT(long curveKey, int filter_type, double lf, double hf, bool DCOffset, int colIndex)
{
	QwtPlotCurve *curve = (QwtPlotCurve *)d_plot->curve(curveKey);
	if (!curve || curve->rtti() != QwtPlotItem::Rtti_PlotCurve)
		return;

	QApplication::setOverrideCursor(Qt::waitCursor);

	int i,j, n = curve->dataSize();
	double *x = new double[n];
	double *y = new double[n]; 

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
		for (int i=0; i < n_curves; i++)
		{
			 QwtPlotCurve *c = curve(i);
  	         if(c && c->rtti() == QwtPlotItem::Rtti_PlotCurve && c->dataSize() > 2)
  	         	return true;
  	    }
		QMessageBox::warning(this, tr("QtiPlot - Error"),
		tr("There are no curves with more than two points on this plot. Operation aborted!"));
		return false;
	}
}
