/***************************************************************************
    File                 : SmoothFilter.cpp
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Ion Vasilief
                           (C) 2010 by Knut Franke
    Email (use @ for *)  : ion_vasilief*yahoo.fr
                           knut.franke*gmx.de
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

#include <QApplication>
#include <QMessageBox>

#include <gsl/gsl_fft_halfcomplex.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_poly.h>

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
    setObjectName(tr("Smoothed"));
    setMethod(m);
    d_points = d_n;
    d_right_points = 2;
    d_left_points = 2;
    d_polynom_order = 2;
}


void SmoothFilter::setMethod(int m)
{
if (m < 1 || m > 3)
    {
        QMessageBox::critical((ApplicationWindow *)parent(), tr("SciDAVis") + " - " + tr("Error"),
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
            d_explanation = QString::number(d_right_points) + " " + tr("points") + " " + tr("Savitzky-Golay smoothing");
            smoothSavGol(x, y);
			break;
		case 2:
            d_explanation = QString::number(d_right_points) + " " + tr("points") + " " + tr("FFT smoothing");
    		smoothFFT(x, y);
			break;
		case 3:
            d_explanation = QString::number(d_right_points) + " " + tr("points") + " " + tr("average smoothing");
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
	double lf = df/(double)d_right_points;//frequency cutoff
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
	int p2 = d_right_points/2;
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

/**
 * \brief Compute Savitzky-Golay coefficients and store them into #h.
 *
 * This function follows GSL conventions in that it writes its result into a matrix allocated by
 * the caller and returns a non-zero result on error.
 *
 * The coefficient matrix is defined as the matrix H mapping a set of input values to the values
 * of the polynomial of order #polynom_order which minimizes squared deviations from the input
 * values. It is computed using the formula \$H=V(V^TV)^(-1)V^T\$, where \$V\$ is the Vandermonde
 * matrix of the point indices.
 *
 * For a short description of the mathematical background, see
 * http://www.statistics4u.info/fundstat_eng/cc_filter_savgol_math.html
 */
int SmoothFilter::savitzkyGolayCoefficients(int points, int polynom_order, gsl_matrix *h) {
	int error = 0; // catch GSL error codes

	// compute Vandermonde matrix
	gsl_matrix *vandermonde = gsl_matrix_alloc(points, polynom_order+1);
	for (int i = 0; i < points; ++i) {
		gsl_matrix_set(vandermonde, i, 0, 1.0);
		for (int j = 1; j <= polynom_order; ++j)
			gsl_matrix_set(vandermonde, i, j, gsl_matrix_get(vandermonde,i,j-1) * i);
	}

	// compute V^TV
	gsl_matrix *vtv = gsl_matrix_alloc(polynom_order+1, polynom_order+1);
	error = gsl_blas_dgemm(CblasTrans, CblasNoTrans, 1.0, vandermonde, vandermonde, 0.0, vtv);

	if (!error) {
		// compute (V^TV)^(-1) using LU decomposition
		gsl_permutation *p = gsl_permutation_alloc(polynom_order+1);
		int signum;
		error = gsl_linalg_LU_decomp(vtv, p, &signum);

		if (!error) {
			gsl_matrix *vtv_inv = gsl_matrix_alloc(polynom_order+1, polynom_order+1);
			error = gsl_linalg_LU_invert(vtv, p, vtv_inv);
			if (!error) {
				// compute (V^TV)^(-1)V^T
				gsl_matrix *vtv_inv_vt = gsl_matrix_alloc(polynom_order+1, points);
				error = gsl_blas_dgemm(CblasNoTrans, CblasTrans, 1.0, vtv_inv, vandermonde, 0.0, vtv_inv_vt);

				if (!error) {
					// finally, compute H = V(V^TV)^(-1)V^T
					error = gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, vandermonde, vtv_inv_vt, 0.0, h);
				}
				gsl_matrix_free(vtv_inv_vt);
			}
			gsl_matrix_free(vtv_inv);
		}
		gsl_permutation_free(p);
	}
	gsl_matrix_free(vtv);
	gsl_matrix_free(vandermonde);

	return error;
}

/**
 * \brief Savitzky-Golay smoothing of (uniformly distributed) data.
 *
 * When the data is not uniformly distributed, Savitzky-Golay looses its interesting conservation
 * properties. On the other hand, a central point of the algorithm is that for uniform data, the
 * operation can be implemented as a convolution. This is considerably more efficient than a more
 * generic method (see smoothModifiedSavGol()) able to handle non-uniform input data.
 *
 * There are at least three possible approaches to handling edges of the data vector (cutting them
 * off, zero padding and using the left-/rightmost smoothing polynomial for computing smoothed
 * values near the edges). Zero-padding is a particularly bad choice for signals with a distinctly
 * non-zero baseline and cutting off edges makes further computations on the original and smoothed
 * signals more difficult; therefore, deviating from the user-specified number of left/right
 * adjacent points (by smoothing over a fixed window at the edges) would be the least annoying
 * alternative; if it were not for the fact that previous versions of SciDAVis had a different
 * behaviour and such a subtle change to the behaviour would be even more annoying, especially
 * between bugfix releases. TODO: reconsider issue for next minor release (also: would it help
 * to add an "edge behaviour" option to the UI?)
 */
void SmoothFilter::smoothSavGol(double *, double *y_inout) {
	// total number of points in smoothing window
	int points = d_left_points + d_right_points + 1;

	if (points < d_polynom_order+1) {
		QMessageBox::critical((ApplicationWindow *)parent(), tr("SciDAVis") + " - " + tr("Error"),
				tr("The polynomial order must be lower than the number of left points plus the number of right points!"));
		return;
	}

	if (d_n < points) {
		QMessageBox::critical((ApplicationWindow *)parent(), tr("SciDAVis") + " - " + tr("Error"),
				tr("Tried to smooth over more points (left+right+1=%1) than given as input (%2).").arg(points).arg(d_n));
		return;
	}

	// Savitzky-Golay coefficient matrix, y' = H y
	gsl_matrix *h = gsl_matrix_alloc(points, points);
	if (int error = savitzkyGolayCoefficients(points, d_polynom_order, h)) {
		QMessageBox::critical((ApplicationWindow *)parent(), tr("SciDAVis") + " - " + tr("Error"),
				tr("Internal error in Savitzky-Golay algorithm.\n")
				+ gsl_strerror(error));
		gsl_matrix_free(h);
		return;
	}

	// allocate memory for the result (temporary; don't overwrite y_inout while we still read from it)
	QVector<double> result(d_n);

	// near left edge: use interpolation of (points) left-most input values
	// i.e. we deviate from the specified left/right points to use
	/*
	for (int i=0; i<d_left_points; i++) {
		double convolution = 0.0;
		for (int k=0; k<points; k++)
			convolution += gsl_matrix_get(h, i, k) * y_inout[k];
		result[i] =  convolution;
	}
	*/
	// legacy behaviour: handle left edge by zero padding
	for (int i=0; i<d_left_points; i++) {
		double convolution = 0.0;
		for (int k=d_left_points-i; k<points; k++)
			convolution += gsl_matrix_get(h, d_left_points, k) * y_inout[i-d_left_points+k];
		result[i] = convolution;
	}
	// central part: convolve with fixed row of h (as given by number of left points to use)
	for (int i=d_left_points; i<d_n-d_right_points; i++) {
		double convolution = 0.0;
		for (int k=0; k<points; k++)
			convolution += gsl_matrix_get(h, d_left_points, k) * y_inout[i-d_left_points+k];
		result[i] = convolution;
	}
	// near right edge: use interpolation of (points) right-most input values
	// i.e. we deviate from the specified left/right points to use
	/*
	for (int i=d_n-d_right_points; i<d_n; i++) {
		double convolution = 0.0;
		for (int k=0; k<points; k++)
			convolution += gsl_matrix_get(h, points-d_n+i, k) * y_inout[d_n-points+k];
		result[i] = convolution;
	}
	*/
	// legacy behaviour: handle right edge by zero padding
	for (int i=d_n-d_right_points; i<d_n; i++) {
		double convolution = 0.0;
		for (int k=0; i-d_left_points+k < d_n; k++)
			convolution += gsl_matrix_get(h, d_left_points, k) * y_inout[i-d_left_points+k];
		result[i] = convolution;
	}

	// deallocate memory
	gsl_matrix_free(h);

	// write result into *y_inout
	qCopy(result.begin(), result.end(), y_inout);
}

/**
 * \brief A variant of the Savitzky-Golay algorithm able to handle non-uniformly distributed data.
 *
 * In comparison to smoothSavGol(), this method trades proper handling of the X coordinates for
 * runtime efficiency by abandoning a central idea of Savitzky-Golay algorithm, namely that
 * polynomial smoothing can be expressed as a convolution.
 *
 * TODO: integrate this option into the GUI.
 */
void SmoothFilter::smoothModifiedSavGol(double *x_in, double *y_inout)
{
	// total number of points in smoothing window
	int points = d_left_points + d_right_points + 1;

	if (points < d_polynom_order+1) {
		QMessageBox::critical((ApplicationWindow *)parent(), tr("SciDAVis") + " - " + tr("Error"),
				tr("The polynomial order must be lower than the number of left points plus the number of right points!"));
		return;
	}

	// allocate memory for the result
	QVector<double> result(d_n);

	// allocate memory for the linear algegra computations
	// Vandermonde matrix for x values of points in the current smoothing window
	gsl_matrix *vandermonde = gsl_matrix_alloc(points, d_polynom_order+1);
	// stores part of the QR decomposition of vandermonde
	gsl_vector *tau = gsl_vector_alloc(qMin(points, d_polynom_order+1));
	// coefficients of polynomial approximation computed for each smoothing window
	gsl_vector *poly = gsl_vector_alloc(d_polynom_order+1);
	// residual of the (least-squares) approximation (by-product of GSL's algorithm)
	gsl_vector *residual = gsl_vector_alloc(points);

	for (int target_index = 0; target_index < d_n; target_index++) {
		int offset = target_index - d_left_points;
		// use a fixed number of points; near left/right borders, use offset to change
		// effective number of left/right points considered
		if (target_index < d_left_points)
			offset += d_left_points - target_index;
		else if (target_index + d_right_points >= d_n)
			offset += d_n - 1 - (target_index + d_right_points);

		// fill Vandermonde matrix
		for (int i = 0; i < points; ++i) {
			gsl_matrix_set(vandermonde, i, 0, 1.0);
			for (int j = 1; j <= d_polynom_order; ++j)
				gsl_matrix_set(vandermonde, i, j, gsl_matrix_get(vandermonde,i,j-1) * x_in[offset + i]);
		}

		// Y values within current smoothing window
		gsl_vector_view y_slice = gsl_vector_view_array(y_inout+offset, points);

		// compute QR decomposition of Vandermonde matrix
		if (int error=gsl_linalg_QR_decomp(vandermonde, tau))
			QMessageBox::critical((ApplicationWindow *)parent(), tr("SciDAVis") + " - " + tr("Error"),
				tr("Internal error in Savitzky-Golay algorithm: QR decomposition failed.\n")
				+ gsl_strerror(error));
		// least-squares-solve vandermonde*poly=y_slice using the QR decomposition now stored in
		// vandermonde and tau
		else if (int error=gsl_linalg_QR_lssolve(vandermonde, tau, &y_slice.vector, poly, residual))
			QMessageBox::critical((ApplicationWindow *)parent(), tr("SciDAVis") + " - " + tr("Error"),
				tr("Internal error in Savitzky-Golay algorithm: least-squares solution failed.\n")
				+ gsl_strerror(error));
		else
			result[target_index] = gsl_poly_eval(poly->data, d_polynom_order+1, x_in[target_index]);
	}

	// deallocate memory
	gsl_vector_free(residual);
	gsl_vector_free(poly);
	gsl_vector_free(tau);
	gsl_matrix_free(vandermonde);

	// write result into *y_inout
	qCopy(result.begin(), result.end(), y_inout);
}

void SmoothFilter::setSmoothPoints(int points, int left_points)
{
    if (points < 0 || left_points < 0)
    {
        QMessageBox::critical((ApplicationWindow *)parent(), tr("SciDAVis") + " - " + tr("Error"),
				tr("The number of points must be positive!"));
		d_init_err = true;
		return;
    }
    else if (d_polynom_order > points + left_points)
    {
        QMessageBox::critical((ApplicationWindow *)parent(), tr("SciDAVis") + " - " + tr("Error"),
				tr("The polynomial order must be lower than the number of left points plus the number of right points!"));
		d_init_err = true;
		return;
    }

    d_right_points = points;
    d_left_points = left_points;
}

void SmoothFilter::setPolynomOrder(int order)
{
	if (d_method != SavitzkyGolay)
    {
        QMessageBox::critical((ApplicationWindow *)parent(), tr("SciDAVis") + " - " + tr("Error"),
				tr("Setting polynomial order is only available for Savitzky-Golay smooth filters! Ignored option!"));
		return;
    }
	
    if (order > d_right_points + d_left_points)
    {
        QMessageBox::critical((ApplicationWindow *)parent(), tr("SciDAVis") + " - " + tr("Error"),
				tr("The polynomial order must be lower than the number of left points plus the number of right points!"));
		d_init_err = true;
		return;
    }
    d_polynom_order = order;
}
