/***************************************************************************
    File                 : analysis.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
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

#include <QApplication>
#include <QDateTime>
#include <QImage>
#include <QMessageBox>

#include <gsl/gsl_sort.h>
#include <gsl/gsl_spline.h>
#include <gsl/gsl_interp.h>
#include <gsl/gsl_fft_halfcomplex.h>
#include <gsl/gsl_fft_complex.h>
#include <gsl/gsl_histogram.h>
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
