/***************************************************************************
    File                 : FFTFilter.h
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
#ifndef FFTFILTER_H
#define FFTFILTER_H

#include "Filter.h"

class QwtPlotCurve;
class FFTFilter : public Filter
{
Q_OBJECT

public:
	FFTFilter(ApplicationWindow *parent, Graph *g, const QString& curveTitle, int m = 1);
	FFTFilter(ApplicationWindow *parent, Graph *g, const QString& curveTitle, double start, double end, int m = 1);

    enum FilterType{LowPass = 1, HighPass = 2, BandPass = 3, BandBlock = 4};

    void setFilterType(int type);
    void setFilterType(FilterType type){setFilterType((int)type);};

    //! Sets the cutoff frequency. To be used only for Low Pass and High Pass filters.
    void setCutoff(double f);

    //! Sets the cutoff frequencies. To be used only for the Band Pass and Band block filters.
    void setBand(double lowFreq, double highFreq);

    //! Enables/Disables the DC offset when applying a Band Pass/Band block filter.
    void enableOffset(bool offset = true){d_offset = offset;};

private:
    void init(int m);
    void calculateOutputData(double *x, double *y);

    //! The filter type. 
    FilterType d_filter_type;

    //! Cutoff frequency for Low Pass and High Pass filters. Lower edge of the band for Band Pass and Band block filters.
    double d_low_freq;

    //! Upper edge of the band for Band Pass and Band block filters.
    double d_high_freq;

    //! Flag telling if the DC offset must be added/substracted when applying a Band Pass/Band block filter respectively.
    bool d_offset;
};

#endif
