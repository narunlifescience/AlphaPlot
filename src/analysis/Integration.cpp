/***************************************************************************
    File                 : Integration.cpp
    Project              : AlphaPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Ion Vasilief
    Email (use @ for *)  : ion_vasilief*yahoo.fr
    Description          : Numerical integration of data sets

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
#include "Integration.h"
#include "core/column/Column.h"

#include <QDateTime>
#include <QLocale>
#include <QMessageBox>

#include <gsl/gsl_interp.h>
#include <gsl/gsl_spline.h>
#include <gsl/gsl_vector.h>

Integration::Integration(ApplicationWindow *parent, AxisRect2D *axisrect)
    : Filter(parent, axisrect) {
  init();
}

Integration::Integration(ApplicationWindow *parent, AxisRect2D *axisrect,
                         PlotData::AssociatedData *associateddata)
    : Filter(parent, axisrect) {
  init();
  setDataFromCurve(associateddata);
}

Integration::Integration(ApplicationWindow *parent, AxisRect2D *axisrect,
                         PlotData::AssociatedData *associateddata, double start,
                         double end)
    : Filter(parent, axisrect) {
  init();
  setDataFromCurve(associateddata, start, end);
}

void Integration::init() {
  setObjectName(tr("Integration"));
  d_method = Linear;
  d_sort_data = true;
  d_result = std::numeric_limits<double>::quiet_NaN();
}

bool Integration::isDataAcceptable() {
  const gsl_interp_type *method_t;
  switch (d_method) {
    case Linear:
      method_t = gsl_interp_linear;
      break;
    case Cubic:
      method_t = gsl_interp_cspline;
      break;
    case Akima:
      method_t = gsl_interp_akima;
      break;
    default:
      QMessageBox::critical(app_, tr("AlphaPlot") + " - " + tr("Error"),
                            tr("Unknown interpolation method. Valid values "
                               "are: 0 - Linear, 1 - Cubic, 2 - Akima."));
      d_init_err = true;
      return true;
  }
  Q_UNUSED(method_t);
  // GSL interpolation routines fail with division by zero on such data
  for (int i = 1; i < d_n; i++)
    if (d_x[i - 1] == d_x[i]) {
      QMessageBox::critical(
          app_, tr("AlphaPlot") + " - " + tr("Error"),
          tr("Several data points have the same x value causing divisions by "
             "zero, operation aborted!"));
      return false;
    }

  return Filter::isDataAcceptable();
}

QString Integration::logInfo() {
  const gsl_interp_type *method_t{};
  QString method_name;
  switch (d_method) {
    case Linear:
      method_t = gsl_interp_linear;
      method_name = tr("Linear");
      break;
    case Cubic:
      method_t = gsl_interp_cspline;
      method_name = tr("Cubic");
      break;
    case Akima:
      method_t = gsl_interp_akima;
      method_name = tr("Akima");
      break;
  }

  gsl_interp *interpolation =
      gsl_interp_alloc(method_t, static_cast<size_t>(d_n));
  gsl_interp_init(interpolation, d_x, d_y, static_cast<size_t>(d_n));

  if (static_cast<size_t>(d_n) < gsl_interp_min_size(interpolation)) {
    QMessageBox::critical(
        app_, tr("AlphaPlot") + " - " + tr("Error"),
        tr("You need at least %1 points in order to perform this operation!")
            .arg(gsl_interp_min_size(interpolation)));
    d_init_err = true;
    return "";
  }

  QString curvename = associateddata_->table->name() + "_" +
                      associateddata_->xcol->name() + "_" +
                      associateddata_->ycol->name();
  QString logInfo = "[" + QDateTime::currentDateTime().toString(Qt::LocalDate) +
                    "\t" + tr("Plot") + ": ''" + curvename + "'']\n";
  logInfo += "\n" + tr("Numerical integration of") + ": " + curvename +
             tr(" using ") + method_name + tr("Interpolation") + "\n";

  int prec = app_->d_decimal_digits;
  logInfo += tr("Points") + ": " + QString::number(d_n) + " " + tr("from") +
             " x = " + QLocale().toString(d_from, 'g', prec) + " ";
  logInfo += tr("to") + " x = " + QLocale().toString(d_to, 'g', prec) + "\n";

  // using GSL to find maximum value of data set
  gsl_vector *aux = gsl_vector_alloc(static_cast<size_t>(d_n));
  for (int i = 0; i < d_n; i++)
    gsl_vector_set(aux, static_cast<size_t>(i), d_y[i]);
  size_t maxID = gsl_vector_max_index(aux);
  gsl_vector_free(aux);

  // calculate result
  d_result =
      gsl_interp_eval_integ(interpolation, d_x, d_y, d_from, d_to, nullptr);

  logInfo += tr("Peak at") +
             " x = " + QLocale().toString(d_x[maxID], 'g', prec) + "\t";
  logInfo += "y = " + QLocale().toString(d_y[maxID], 'g', prec) + "\n";

  logInfo += tr("Area") + "=";
  logInfo += QLocale().toString(d_result, 'g', prec);
  logInfo +=
      "\n-------------------------------------------------------------\n";

  gsl_interp_free(interpolation);

  return logInfo;
}
