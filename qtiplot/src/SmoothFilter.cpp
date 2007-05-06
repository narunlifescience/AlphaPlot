/***************************************************************************
    File                 : SmoothFilter.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Ion Vasilief
    Email (use @ for *)  : ion_vasilief*yahoo.fr
    Description          : Numerical smoothing of data sets

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
#include "SmoothFilter.h"
#include "nrutil.h"

#include <QApplication>
#include <QMessageBox>

#include <gsl/gsl_fft_halfcomplex.h>

SmoothFilter::SmoothFilter(ApplicationWindow *parent, Graph *g, const QString& curveTitle, int m)
: Filter(parent, g)
{
	setDataFromCurve(curveTitle);
	init(m);
}

SmoothFilter::SmoothFilter(ApplicationWindow *parent, Graph *g, const QString& curveTitle,
                             double start, double end, int m)
: Filter(parent, g)
{
	setDataFromCurve(curveTitle, start, end);
    init(m);
}

void SmoothFilter::init (int m)
{
    setName(tr("Smoothed"));
    setMethod(m);
    d_points = d_n;
    d_smooth_points = 2;
    d_sav_gol_points = 2;
    d_polynom_order = 2;
}


void SmoothFilter::setMethod(int m)
{
if (m < 1 || m > 3)
    {
        QMessageBox::critical((ApplicationWindow *)parent(), tr("QtiPlot") + " - " + tr("Error"),
        tr("Unknown smooth filter. Valid values are: 1 - Savitky-Golay, 2 - FFT, 3 - Moving Window Average."));
        d_init_err = true;
        return;
    }
d_method = (SmoothMethod)m;
}

void SmoothFilter::calculateOutputData(double *x, double *y)
{
    for (int i = 0; i < d_points; i++)
	{
	   x[i] = d_x[i];
	   y[i] = d_y[i];//filtering frequencies
	}

	switch((int)d_method)
	{
		case 1:
            d_explanation = QString::number(d_smooth_points) + " " + tr("points") + " " + tr("Savitzky-Golay smoothing");
            smoothSavGol(x, y);
			break;
		case 2:
            d_explanation = QString::number(d_smooth_points) + " " + tr("points") + " " + tr("FFT smoothing");
    		smoothFFT(x, y);
			break;
		case 3:
            d_explanation = QString::number(d_smooth_points) + " " + tr("points") + " " + tr("average smoothing");
    		smoothAverage(x, y);
			break;
	}
}

void SmoothFilter::smoothFFT(double *x, double *y)
{
	gsl_fft_real_workspace *work = gsl_fft_real_workspace_alloc(d_n);
	gsl_fft_real_wavetable *real = gsl_fft_real_wavetable_alloc(d_n);
	gsl_fft_real_transform (y, 1, d_n, real, work);//FFT forward
	gsl_fft_real_wavetable_free (real);

	double df = 1.0/(double)(x[1] - x[0]);
	double lf = df/(double)d_smooth_points;//frequency cutoff
	df = 0.5*df/(double)d_n;

    for (int i = 0; i < d_n; i++)
	{
	   x[i] = d_x[i];
	   y[i] = i*df > lf ? 0 : y[i];//filtering frequencies
	}

	gsl_fft_halfcomplex_wavetable *hc = gsl_fft_halfcomplex_wavetable_alloc (d_n);
	gsl_fft_halfcomplex_inverse (y, 1, d_n, hc, work);//FFT inverse
	gsl_fft_halfcomplex_wavetable_free (hc);
	gsl_fft_real_workspace_free (work);
}

void SmoothFilter::smoothAverage(double *, double *y)
{
	int p2 = d_smooth_points/2;
	double m = double(2*p2+1);
	double aux = 0.0;
    double *s = new double[d_n];

	s[0] = y[0];
	for (int i=1; i<p2; i++)
	{
		aux = 0.0;
		for (int j=-i; j<=i; j++)
			aux += y[i+j];

		s[i] = aux/(double)(2*i+1);
	}
	for (int i=p2; i<d_n-p2; i++)
	{
		aux = 0.0;
		for (int j=-p2; j<=p2; j++)
			aux += y[i+j];

		s[i] = aux/m;
	}
	for (int i=d_n-p2; i<d_n-1; i++)
	{
		aux = 0.0;
		for (int j=d_n-i-1; j>=i-d_n+1; j--)
			aux += y[i+j];

		s[i] = aux/(double)(2*(d_n-i-1)+1);
	}
	s[d_n-1] = y[d_n-1];

    for (int i = 0; i<d_n; i++)
        y[i] = s[i];

    delete[] s;
}

void SmoothFilter::smoothSavGol(double *, double *y)
{
	double *s = new double[d_n];
    int nl = d_smooth_points;
    int nr = d_sav_gol_points;
	int np = nl+nr+1;
	double *c = vector(1, np);

	//seek shift index for given case nl, nr, m (see savgol).
	int *index = intvector(1, np);
	index[1]=0;
	int i, j=3;
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
	savgol(c, np, nl, nr, 0, d_polynom_order);

	for (i=0; i<d_n; i++)
	{// Apply filter to input data.
		s[i]=0.0;
		for (j=1; j<=np; j++)
		{
			int it = i+index[j];
			if (it >=0 && it < d_n)//skip left points that do not exist.
				s[i] += c[j]*y[i+index[j]];
		}
	}

    for (i = 0; i<d_n; i++)
        y[i] = s[i];

	delete[] s;
	free_vector(c, 1, np);
	free_intvector(index, 1, np);
}

void SmoothFilter::setSmoothPoints(int points, int left_points)
{
    if (points < 0 || left_points < 0)
    {
        QMessageBox::critical((ApplicationWindow *)parent(), tr("QtiPlot") + " - " + tr("Error"),
				tr("The number of points must be positive!"));
		d_init_err = true;
		return;
    }
    else if (d_polynom_order > points + left_points)
    {
        QMessageBox::critical((ApplicationWindow *)parent(), tr("QtiPlot") + " - " + tr("Error"),
				tr("The polynomial order must be lower than the number of left points plus the number of right points!"));
		d_init_err = true;
		return;
    }

    d_smooth_points = points;
    d_sav_gol_points = left_points;
}

void SmoothFilter::setPolynomOrder(int order)
{
	if (d_method != SavitzkyGolay)
    {
        QMessageBox::critical((ApplicationWindow *)parent(), tr("QtiPlot") + " - " + tr("Error"),
				tr("Setting polynomial order is only available for Savitzky-Golay smooth filters! Ignored option!"));
		return;
    }
	
    if (order > d_smooth_points + d_sav_gol_points)
    {
        QMessageBox::critical((ApplicationWindow *)parent(), tr("QtiPlot") + " - " + tr("Error"),
				tr("The polynomial order must be lower than the number of left points plus the number of right points!"));
		d_init_err = true;
		return;
    }
    d_polynom_order = order;
}
