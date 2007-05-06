/***************************************************************************
    File                 : FFTFilter.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Ion Vasilief
    Email (use @ for *)  : ion_vasilief*yahoo.fr
    Description          : Numerical FFT filtering of data sets

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
#include "FFTFilter.h"

#include <QMessageBox>
#include <QLocale>

#include <gsl/gsl_fft_halfcomplex.h>

FFTFilter::FFTFilter(ApplicationWindow *parent, Graph *g, const QString& curveTitle, int m)
: Filter(parent, g)
{
	setDataFromCurve(curveTitle);
	init(m);
}

FFTFilter::FFTFilter(ApplicationWindow *parent, Graph *g, const QString& curveTitle,
                             double start, double end, int m)
: Filter(parent, g)
{
	setDataFromCurve(curveTitle, start, end);
    init(m);
}

void FFTFilter::init (int m)
{
    setName(tr("FFT") + tr("Filtered"));
    setFilterType(m);
    d_points = d_n;
    d_offset = true;
    d_low_freq = 0;
    d_high_freq = 0;
}


void FFTFilter::setFilterType(int type)
{
    if (type < 1 || type > 4)
    {
        QMessageBox::critical((ApplicationWindow *)parent(), tr("QtiPlot") + " - " + tr("Error"),
        tr("Unknown filter type. Valid values are: 1 - Low pass, 2 - High Pass, 3 - Band Pass, 4 - Band block."));
        d_init_err = true;
        return;
    }
    d_filter_type = (FilterType)type;
}

void FFTFilter::setCutoff(double f)
{
    if (d_filter_type > 2 && d_low_freq == f)
        return;

    d_low_freq = f;
}

void FFTFilter::setBand(double lowFreq, double highFreq)
{
    if (d_filter_type < 3)
        return;
    else if (lowFreq == highFreq)
    {
        QMessageBox::critical((ApplicationWindow *)parent(), tr("QtiPlot") + " - " + tr("Error"),
        tr("Please enter different values for the band limits."));
        d_init_err = true;
        return;
    }

    if (lowFreq > highFreq)
    {
        d_low_freq = highFreq;
        d_high_freq = lowFreq;
    }
    else
    {
        d_low_freq = lowFreq;
        d_high_freq = highFreq;
    }
}

void FFTFilter::calculateOutputData(double *x, double *y)
{
    for (int i = 0; i < d_points; i++)
	{
	   x[i] = d_x[i];
	   y[i] = d_y[i];
	}

    double df = 0.5/(double)(d_n*(x[1]-x[0]));//half frequency sampling due to GSL storing

	gsl_fft_real_workspace *work = gsl_fft_real_workspace_alloc(d_n);
	gsl_fft_real_wavetable *real = gsl_fft_real_wavetable_alloc(d_n);

	gsl_fft_real_transform (y, 1, d_n, real, work);
	gsl_fft_real_wavetable_free (real);

    d_explanation = QLocale().toString(d_low_freq) + " ";
	if (d_filter_type > 2)
	   d_explanation += tr("to") + " " + QLocale().toString(d_high_freq) + " ";
	d_explanation += tr("Hz") + " ";

	switch ((int)d_filter_type)
	{
		case 1://low pass
			d_explanation += tr("Low Pass FFT Filter");
			for (int i = 0; i < d_n; i++)
				y[i] = i*df > d_low_freq ? 0 : y[i];
			break;

		case 2://high pass
			d_explanation += tr("High Pass FFT Filter");
			for (int i = 0; i < d_n; i++)
				y[i] = i*df < d_low_freq ? 0 : y[i];
			break;

		case 3://band pass
			d_explanation += tr("Band Pass FFT Filter");
			if(d_offset)
			{// keep DC offset
				for (int i = 1; i < d_n; i++)
					y[i] = ((i*df > d_low_freq ) && (i*df < d_high_freq )) ? y[i] : 0;
			}
			else
			{
				for (int i = 0; i < d_n; i++)
					y[i] = ((i*df > d_low_freq ) && (i*df < d_high_freq )) ? y[i] : 0;
			}
			break;

		case 4://band block
			d_explanation += tr("Band Block FFT Filter");

			if(!d_offset)
				y[0] = 0;//substract DC offset

			for (int i = 1; i < d_n; i++)
				y[i] = ((i*df > d_low_freq ) && (i*df < d_high_freq )) ? 0 : y[i];
			break;
	}

	gsl_fft_halfcomplex_wavetable *hc = gsl_fft_halfcomplex_wavetable_alloc (d_n);
	gsl_fft_halfcomplex_inverse (y, 1, d_n, hc, work);
	gsl_fft_halfcomplex_wavetable_free (hc);
	gsl_fft_real_workspace_free (work);
}
