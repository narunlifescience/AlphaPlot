/***************************************************************************
    File                 : FFTFilter.cpp
    Project              : SciDAVis
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

FFTFilter::FFTFilter(ApplicationWindow *parent, Layer *layer, const QString& curveTitle, int m)
: Filter(parent, layer)
{
	setDataFromCurve(curveTitle);
	init(m);
}

FFTFilter::FFTFilter(ApplicationWindow *parent, Layer *layer, const QString& curveTitle,
                             double start, double end, int m)
: Filter(parent, layer)
{
	setDataFromCurve(curveTitle, start, end);
    init(m);
}

void FFTFilter::init (int m)
{
    setName(tr("FFT") + tr("Filtered"));
    setFilterType(m);
    m_points = m_n;
    m_offset = true;
    m_low_freq = 0;
    m_high_freq = 0;
}


void FFTFilter::setFilterType(int type)
{
    if (type < 1 || type > 4)
    {
        QMessageBox::critical((ApplicationWindow *)parent(), tr("SciDAVis") + " - " + tr("Error"),
        tr("Unknown filter type. Valid values are: 1 - Low pass, 2 - High Pass, 3 - Band Pass, 4 - Band block."));
        m_init_err = true;
        return;
    }
    m_filter_type = (FilterType)type;
}

void FFTFilter::setCutoff(double f)
{
    if (m_filter_type > 2 && m_low_freq == f)
        return;

    m_low_freq = f;
}

void FFTFilter::setBand(double lowFreq, double highFreq)
{
    if (m_filter_type < 3)
        return;
    else if (lowFreq == highFreq)
    {
        QMessageBox::critical((ApplicationWindow *)parent(), tr("SciDAVis") + " - " + tr("Error"),
        tr("Please enter different values for the band limits."));
        m_init_err = true;
        return;
    }

    if (lowFreq > highFreq)
    {
        m_low_freq = highFreq;
        m_high_freq = lowFreq;
    }
    else
    {
        m_low_freq = lowFreq;
        m_high_freq = highFreq;
    }
}

void FFTFilter::calculateOutputData(double *x, double *y)
{
    for (int i = 0; i < m_points; i++)
	{
	   x[i] = m_x[i];
	   y[i] = m_y[i];
	}

    double df = 0.5/(double)(m_n*(x[1]-x[0]));//half frequency sampling due to GSL storing

	gsl_fft_real_workspace *work = gsl_fft_real_workspace_alloc(m_n);
	gsl_fft_real_wavetable *real = gsl_fft_real_wavetable_alloc(m_n);

	gsl_fft_real_transform (y, 1, m_n, real, work);
	gsl_fft_real_wavetable_free (real);

    m_explanation = QLocale().toString(m_low_freq) + " ";
	if (m_filter_type > 2)
	   m_explanation += tr("to") + " " + QLocale().toString(m_high_freq) + " ";
	m_explanation += tr("Hz") + " ";

	switch ((int)m_filter_type)
	{
		case 1://low pass
			m_explanation += tr("Low Pass FFT Filter");
			for (int i = 0; i < m_n; i++)
				y[i] = i*df > m_low_freq ? 0 : y[i];
			break;

		case 2://high pass
			m_explanation += tr("High Pass FFT Filter");
			for (int i = 0; i < m_n; i++)
				y[i] = i*df < m_low_freq ? 0 : y[i];
			break;

		case 3://band pass
			m_explanation += tr("Band Pass FFT Filter");
			if(m_offset)
			{// keep DC offset
				for (int i = 1; i < m_n; i++)
					y[i] = ((i*df > m_low_freq ) && (i*df < m_high_freq )) ? y[i] : 0;
			}
			else
			{
				for (int i = 0; i < m_n; i++)
					y[i] = ((i*df > m_low_freq ) && (i*df < m_high_freq )) ? y[i] : 0;
			}
			break;

		case 4://band block
			m_explanation += tr("Band Block FFT Filter");

			if(!m_offset)
				y[0] = 0;//substract DC offset

			for (int i = 1; i < m_n; i++)
				y[i] = ((i*df > m_low_freq ) && (i*df < m_high_freq )) ? 0 : y[i];
			break;
	}

	gsl_fft_halfcomplex_wavetable *hc = gsl_fft_halfcomplex_wavetable_alloc (m_n);
	gsl_fft_halfcomplex_inverse (y, 1, m_n, hc, work);
	gsl_fft_halfcomplex_wavetable_free (hc);
	gsl_fft_real_workspace_free (work);
}
