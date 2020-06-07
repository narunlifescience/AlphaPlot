/*This file is part of AlphaPlot.

   Copyright 2016 - 2020, Arun Narayanankutty <n.arun.lifescience@gmail.com>
   Copyright 2006 - 2007, Ion Vasilief <ion_vasilief@yahoo.fr>

   AlphaPlot is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   AlphaPlot is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   You should have received a copy of the GNU General Public License
   along with AlphaPlot.  If not, see <http://www.gnu.org/licenses/>.

   Description : Numerical interpolation of data sets*/

#include "Interpolation.h"

#include <gsl/gsl_interp.h>
#include <gsl/gsl_sort.h>
#include <gsl/gsl_spline.h>

#include <QMessageBox>

Interpolation::Interpolation(ApplicationWindow *parent, AxisRect2D *axisrect,
                             PlotData::AssociatedData *associateddata,
                             const InterpolationMethod &method)
    : Filter(parent, axisrect) {
  init(method);
  setDataFromCurve(associateddata);
}

Interpolation::Interpolation(ApplicationWindow *parent, AxisRect2D *axisrect,
                             PlotData::AssociatedData *associateddata,
                             double start, double end,
                             const InterpolationMethod &method)
    : Filter(parent, axisrect) {
  init(method);
  setDataFromCurve(associateddata, start, end);
}

void Interpolation::init(const InterpolationMethod &method) {
  d_method = method;
  switch (d_method) {
    case Interpolation::InterpolationMethod::Linear:
      setObjectName(tr("Linear") + tr("Int"));
      d_explanation = tr("Linear") + " " + tr("Interpolation");
      d_min_points = 3;
      break;
    case Interpolation::InterpolationMethod::Polynomial:
      setObjectName(tr("Polynomial") + tr("Int"));
      d_explanation = tr("Polynomial") + " " + tr("Interpolation");
      d_min_points = 4;
      break;
    case Interpolation::InterpolationMethod::Cubic:
      setObjectName(tr("Cubic") + tr("Int"));
      d_explanation = tr("Cubic") + " " + tr("Interpolation");
      d_min_points = 4;
      break;
    case Interpolation::InterpolationMethod::CubicPeriodic:
      setObjectName(tr("CubicPeriodic") + tr("Int"));
      d_explanation = tr("CubicPeriodic") + " " + tr("Interpolation");
      d_min_points = 4;
      break;
    case Interpolation::InterpolationMethod::Akima:
      setObjectName(tr("Akima") + tr("Int"));
      d_explanation = tr("Akima") + " " + tr("Interpolation");
      d_min_points = 5;
      break;
    case Interpolation::InterpolationMethod::AkimaPeriodic:
      setObjectName(tr("AkimaPeriodic") + tr("Int"));
      d_explanation = tr("AkimaPeriodic") + " " + tr("Interpolation");
      d_min_points = 5;
      break;
    case Interpolation::InterpolationMethod::Steffen:
      setObjectName(tr("Steffen") + tr("Int"));
      d_explanation = tr("Steffen") + " " + tr("Interpolation");
      d_min_points = 4;
      break;
  }
  d_sort_data = true;
}

void Interpolation::setMethod(const InterpolationMethod &method) {
  int min_points = 3;
  switch (method) {
    case Interpolation::InterpolationMethod::Linear:
      min_points = 3;
      break;
    case Interpolation::InterpolationMethod::Polynomial:
    case Interpolation::InterpolationMethod::Cubic:
    case Interpolation::InterpolationMethod::CubicPeriodic:
    case Interpolation::InterpolationMethod::Steffen:
      min_points = 4;
      break;
    case Interpolation::InterpolationMethod::Akima:
    case Interpolation::InterpolationMethod::AkimaPeriodic:
      min_points = 5;
      break;
  }

  if (d_n < min_points) {
    QMessageBox::critical(
        app_, tr("AlphaPlot") + " - " + tr("Error"),
        tr("You need at least %1 points in order to perform this operation!")
            .arg(min_points));
    d_init_err = true;
    return;
  }
  d_method = method;
  d_min_points = min_points;
}

void Interpolation::calculateOutputData(double *x, double *y) {
  gsl_interp_accel *acc = gsl_interp_accel_alloc();
  const gsl_interp_type *method = nullptr;
  switch (d_method) {
    case Interpolation::InterpolationMethod::Linear:
      method = gsl_interp_linear;
      break;
    case Interpolation::InterpolationMethod::Polynomial:
      method = gsl_interp_polynomial;
      break;
    case Interpolation::InterpolationMethod::Cubic:
      method = gsl_interp_cspline;
      break;
    case Interpolation::InterpolationMethod::CubicPeriodic:
      method = gsl_interp_cspline_periodic;
      break;
    case Interpolation::InterpolationMethod::Akima:
      method = gsl_interp_akima;
      break;
    case Interpolation::InterpolationMethod::AkimaPeriodic:
      method = gsl_interp_akima_periodic;
      break;
    case Interpolation::InterpolationMethod::Steffen:
      method = gsl_interp_steffen;
      break;
  }

  gsl_spline *interp = gsl_spline_alloc(method, static_cast<size_t>(d_n));
  gsl_spline_init(interp, d_x, d_y, static_cast<size_t>(d_n));

  double step = (d_to - d_from) / static_cast<double>(d_points - 1);
  for (int j = 0; j < d_points; j++) {
    x[j] = d_from + j * step;
    y[j] = gsl_spline_eval(interp, x[j], acc);
  }

  gsl_spline_free(interp);
  gsl_interp_accel_free(acc);
}

bool Interpolation::isDataAcceptable() {
  // GSL interpolation routines fail with division by zero on such data
  for (int i = 1; i < d_n; i++)
    if (d_x[i - 1] == d_x[i]) {
      QMessageBox::critical(
          app_, tr("AlphaPlot") + " - " + tr("Error"),
          tr("Several data points have the same x value causing divisions by "
             "zero, operation aborted!"));
      return false;
    }

  // y(0) and y(n) should be same for periodic data
  if (d_method == Interpolation::InterpolationMethod::CubicPeriodic ||
      d_method == Interpolation::InterpolationMethod::AkimaPeriodic) {
    if (d_y[0] != d_y[d_n - 1]) {
      QMessageBox::critical(app_, tr("AlphaPlot") + " - " + tr("Error"),
                            tr("Periodic data should have "
                               "y(0) = y(n), operation aborted!"));
      return false;
    }
  }

  return Filter::isDataAcceptable();
}
