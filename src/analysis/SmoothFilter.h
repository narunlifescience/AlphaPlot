/***************************************************************************
    File                 : SmoothFilter.h
    Project              : AlphaPlot
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
#ifndef SMOOTHFILTER_H
#define SMOOTHFILTER_H

#include <gsl/gsl_matrix_double.h>
#include "Filter.h"

class AxisRect2D;

class SmoothFilter : public Filter
{
    Q_OBJECT

public:
    SmoothFilter(ApplicationWindow *parent, AxisRect2D *axisrect,
                 PlotData::AssociatedData *associateddata, int m = 3);
    SmoothFilter(ApplicationWindow *parent, AxisRect2D *axisrect,
                 PlotData::AssociatedData *associateddata, double start,
                 double end, int m = 3);

    enum SmoothMethod { SavitzkyGolay = 1, FFT = 2, Average = 3 };

    int method() { return static_cast<int>(d_method); }
    void setMethod(int m);
    void setMethod(SmoothMethod m) { setMethod(static_cast<int>(m)); }

    void setSmoothPoints(int points, int left_points = 0);
    //! Sets the polynomial order in the Savitky-Golay algorithm.
    void setPolynomOrder(int order);

private:
    void init(int m);
    void calculateOutputData(double *x, double *y);
    void smoothFFT(double *x, double *y);
    void smoothAverage(double *x, double *y);
    void smoothSavGol(double *x, double *y);
    void smoothModifiedSavGol(double *x, double *y);
    static int savitzkyGolayCoefficients(int points, int polynom_order,
                                         gsl_matrix *h);

    //! The smooth method.
    SmoothMethod d_method;

    //! The number of (right) adjacents points used to smooth the data set.
    int d_right_points;

    //! The number of left adjacents points used by the Savitzky-Golay algorithm.
    int d_left_points;

    //! Polynomial order in the Savitzky-Golay algorithm.
    int d_polynom_order;
};

#endif // SMOOTHFILTER_H
