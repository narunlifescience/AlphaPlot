/***************************************************************************
    File                 : Fit.cpp
    Project              : AlphaPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Benkert
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : Fit base class

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
#include "Fit.h"

#include <gsl/gsl_blas.h>
#include <gsl/gsl_deriv.h>
#include <gsl/gsl_statistics.h>
#include <gsl/gsl_version.h>

#include <QApplication>
#include <QDateTime>
#include <QLocale>
#include <QMessageBox>

#include "2Dplot/Curve2D.h"
#include "2Dplot/Legend2D.h"
#include "2Dplot/AxisRect2D.h"
#include "ColorBox.h"
#include "Matrix.h"
#include "Table.h"
#include "core/column/Column.h"
#include "fit_gsl.h"
#include "scripting/Script.h"

Fit::Fit(ApplicationWindow *parent, AxisRect2D *axisrect, QString name)
    : Filter(parent, axisrect, name),
      scripted(ScriptingLangManager::newEnv("muParser", parent)) {
  d_p = 0;
  d_n = 0;
  d_curveColorIndex = 1;
  d_solver = ScaledLevenbergMarquardt;
  d_tolerance = 1e-4;
  d_gen_function = true;
  d_points = 100;
  d_max_iterations = 1000;
  associateddata_ = nullptr;
  d_formula = QString();
  d_explanation = QString();
  d_y_error_source = UnknownErrors;
  d_y_error_dataset = QString();
  is_non_linear = true;
  d_results = nullptr;
  d_result_errors = nullptr;
  d_prec = parent->fit_output_precision;
  d_init_err = false;
  chi_2 = -1;
  d_scale_errors = false;
  d_sort_data = true;
}

double *Fit::fitGslMultifit(int &iterations, int &status) {
  double *result = new double[static_cast<size_t>(d_p)];

  // declare input data
  struct FitData data = {static_cast<size_t>(d_n),
                         static_cast<size_t>(d_p),
                         d_x,
                         d_y,
                         d_y_errors,
                         this};
  gsl_multifit_function_fdf f;
  f.f = d_f;
  f.df = d_df;
  f.fdf = d_fdf;
  f.n = static_cast<size_t>(d_n);
  f.p = static_cast<size_t>(d_p);
  f.params = &data;

  // initialize solver
  const gsl_multifit_fdfsolver_type *T;
  switch (d_solver) {
    case ScaledLevenbergMarquardt:
      T = gsl_multifit_fdfsolver_lmsder;
      break;
    case UnscaledLevenbergMarquardt:
      T = gsl_multifit_fdfsolver_lmder;
      break;
    default:
      break;
  }
  gsl_multifit_fdfsolver *s = gsl_multifit_fdfsolver_alloc(
      T, static_cast<size_t>(d_n), static_cast<size_t>(d_p));
  gsl_multifit_fdfsolver_set(s, &f, d_param_init);

  // iterate solver algorithm
  for (iterations = 0; iterations < d_max_iterations; iterations++) {
    status = gsl_multifit_fdfsolver_iterate(s);
    if (status) break;

    status = gsl_multifit_test_delta(s->dx, s->x, d_tolerance, d_tolerance);
    if (status != GSL_CONTINUE) break;
  }

  // grab results
  for (int i = 0; i < d_p; i++)
    result[i] = gsl_vector_get(s->x, static_cast<size_t>(i));
  gsl_blas_ddot(s->f, s->f, &chi_2);
#if GSL_MAJOR_VERSION < 2
  gsl_multifit_covar(s->J, 0.0, covar);
#else
  {
    gsl_matrix *J = gsl_matrix_alloc(d_n, d_p);
    gsl_multifit_fdfsolver_jac(s,J);
    gsl_multifit_covar (J, 0.0, covar);
    gsl_matrix_free (J);
  }
#endif
  if (d_y_error_source == UnknownErrors) {
    // multiply covar by variance of residuals, which is used as an estimate for
    // the
    // statistical errors (this relies on the Y errors being set to 1.0, so that
    // s->f is properly normalized)
    gsl_matrix_scale(covar, chi_2 / (d_n - d_p));
  }

  // free memory allocated for fitting
  gsl_multifit_fdfsolver_free(s);

  return result;
}

double *Fit::fitGslMultimin(int &iterations, int &status) {
  double *result = new double[static_cast<size_t>(d_p)];

  // declare input data
  struct FitData data = {static_cast<size_t>(d_n),
                         static_cast<size_t>(d_p),
                         d_x,
                         d_y,
                         d_y_errors,
                         this};
  gsl_multimin_function f;
  f.f = d_fsimplex;
  f.n = static_cast<size_t>(d_p);
  f.params = &data;

  // step size (size of the simplex)
  // can be increased for faster convergence
  gsl_vector *ss = gsl_vector_alloc(f.n);
  gsl_vector_set_all(ss, 10.0);

  // initialize minimizer
  const gsl_multimin_fminimizer_type *T = gsl_multimin_fminimizer_nmsimplex;
  gsl_multimin_fminimizer *s_min = gsl_multimin_fminimizer_alloc(T, f.n);
  gsl_multimin_fminimizer_set(s_min, &f, d_param_init, ss);

  // iterate minimization algorithm
  for (iterations = 0; iterations < d_max_iterations; iterations++) {
    status = gsl_multimin_fminimizer_iterate(s_min);
    if (status) break;

    double size = gsl_multimin_fminimizer_size(s_min);
    status = gsl_multimin_test_size(size, d_tolerance);
    if (status != GSL_CONTINUE) break;
  }

  // grab results
  for (int i = 0; i < d_p; i++)
    result[i] = gsl_vector_get(s_min->x, static_cast<size_t>(i));
  chi_2 = s_min->fval;
  gsl_matrix *J =
      gsl_matrix_alloc(static_cast<size_t>(d_n), static_cast<size_t>(d_p));
  d_df(s_min->x, static_cast<void *>(f.params), J);
  gsl_multifit_covar(J, 0.0, covar);
  if (d_y_error_source == UnknownErrors) {
    // multiply covar by variance of residuals, which is used as an estimate for
    // the
    // statistical errors (this relies on the Y errors being set to 1.0)
    gsl_matrix_scale(covar, chi_2 / (d_n - d_p));
  }

  // free previously allocated memory
  gsl_matrix_free(J);
  gsl_multimin_fminimizer_free(s_min);
  gsl_vector_free(ss);

  return result;
}

void Fit::setDataCurve(PlotData::AssociatedData *associateddata, double start,
                       double end) {
  if (d_n > 0) delete[] d_y_errors;
  Filter::setDataCurve(associateddata, start, end);

  d_y_errors = new double[static_cast<size_t>(d_n)];
  // if (!setYErrorSource(AssociatedErrors, QString::null, true))
  setYErrorSource(UnknownErrors);
}

void Fit::setInitialGuesses(double *x_init) {
  for (int i = 0; i < d_p; i++)
    gsl_vector_set(d_param_init, static_cast<size_t>(i), x_init[i]);
}

void Fit::generateFunction(bool yes, int points) {
  d_gen_function = yes;
  if (d_gen_function) d_points = points;
}

QString Fit::logFitInfo(double *par, int iterations, int status,
                        const QString &plotName) {
  QDateTime dt = QDateTime::currentDateTime();
  QString info = "[" + dt.toString(Qt::LocalDate) + "\t" + tr("Plot") + ": ''" +
                 plotName + "'']\n";
  info += d_explanation + " " + tr("fit of dataset") + ": " +
          associateddata_->table->name() + "_" + associateddata_->xcol->name() +
          "_" + associateddata_->ycol->name();
  if (!d_formula.isEmpty())
    info += ", " + tr("using function") + ": " + d_formula + "\n";
  else
    info += "\n";

  info += tr("Y standard errors") + ": ";
  switch (d_y_error_source) {
    case UnknownErrors:
      info += tr("Unknown");
      break;
    case AssociatedErrors:
      info += tr("Associated dataset (%1)").arg(d_y_error_dataset);
      break;
    case PoissonErrors:
      info += tr("Statistical (assuming Poisson distribution)");
      break;
    case CustomErrors:
      info += tr("Arbitrary Dataset") + ": " + d_y_error_dataset;
      break;
  }
  info += "\n";

  if (is_non_linear) {
    if (d_solver == NelderMeadSimplex)
      info += tr("Nelder-Mead Simplex");
    else if (d_solver == UnscaledLevenbergMarquardt)
      info += tr("Unscaled Levenberg-Marquardt");
    else
      info += tr("Scaled Levenberg-Marquardt");

    info += tr(" algorithm with tolerance = ") +
            QLocale().toString(d_tolerance) + "\n";
  }

  info += tr("From x") + " = " + QLocale().toString(d_x[0], 'g', 15) + " " +
          tr("to x") + " = " + QLocale().toString(d_x[d_n - 1], 'g', 15) + "\n";
  double chi_2_dof = chi_2 / (d_n - d_p);
  for (int i = 0; i < d_p; i++) {
    info += d_param_names[i] + " " + d_param_explain[i] + " = " +
            QLocale().toString(par[i], 'g', d_prec) + " +/- ";
    if (d_scale_errors)
      info += QLocale().toString(
                  sqrt(chi_2_dof * gsl_matrix_get(covar, static_cast<size_t>(i),
                                                  static_cast<size_t>(i))),
                  'g', d_prec) +
              "\n";
    else
      info +=
          QLocale().toString(sqrt(gsl_matrix_get(covar, static_cast<size_t>(i),
                                                 static_cast<size_t>(i))),
                             'g', d_prec) +
          "\n";
  }
  info +=
      "------------------------------------------------------------------------"
      "--------------\n";
  info += "Chi^2/doF = " + QLocale().toString(chi_2_dof, 'g', d_prec) + "\n";

  info += tr("R^2") + " = " + QLocale().toString(rSquare(), 'g', d_prec) + "\n";
  info +=
      "------------------------------------------------------------------------"
      "---------------\n";
  if (is_non_linear) {
    info += tr("Iterations") + " = " + QString::number(iterations) + "\n";
    info += tr("Status") + " = " + gsl_strerror(status) + "\n";
    info +=
        "----------------------------------------------------------------------"
        "-----------------\n";
  }
  return info;
}

double Fit::rSquare() {
  double mean = 0.0, tss = 0.0, weights_sum = 0.0;

  if (d_y_error_source == UnknownErrors) {
    for (int i = 0; i < d_n; i++) mean += d_y[i];
    mean /= d_n;
    for (int i = 0; i < d_n; i++) tss += (d_y[i] - mean) * (d_y[i] - mean);
  } else {
    for (int i = 0; i < d_n; i++) {
      mean += d_y[i] / (d_y_errors[i] * d_y_errors[i]);
      weights_sum += 1.0 / (d_y_errors[i] * d_y_errors[i]);
    }
    mean /= weights_sum;
    for (int i = 0; i < d_n; i++)
      tss +=
          (d_y[i] - mean) * (d_y[i] - mean) / (d_y_errors[i] * d_y_errors[i]);
  }
  return 1 - chi_2 / tss;
}

QString Fit::legendInfo() {
  QString info = tr("Dataset") + ": " + associateddata_->table->name() + "_" +
                 associateddata_->xcol->name() + "_" +
                 associateddata_->ycol->name() + "\n";
  info += tr("Function") + ": " + d_formula + "\n\n";

  double chi_2_dof = chi_2 / (d_n - d_p);
  info += "Chi^2/doF = " + QLocale().toString(chi_2_dof, 'g', d_prec) + "\n";
  info += tr("R^2") + " = " + QLocale().toString(rSquare(), 'g', d_prec) + "\n";

  for (int i = 0; i < d_p; i++) {
    info += d_param_names[i] + " = " +
            QLocale().toString(d_results[i], 'g', d_prec) + " +/- ";
    if (d_scale_errors)
      info += QLocale().toString(
                  sqrt(chi_2_dof * gsl_matrix_get(covar, static_cast<size_t>(i),
                                                  static_cast<size_t>(i))),
                  'g', d_prec) +
              "\n";
    else
      info +=
          QLocale().toString(sqrt(gsl_matrix_get(covar, static_cast<size_t>(i),
                                                 static_cast<size_t>(i))),
                             'g', d_prec) +
          "\n";
  }
  return info;
}

bool Fit::setYErrorSource(ErrorSource err, const QString &colName,
                          bool fail_silently) {
  d_y_error_source = err;
  switch (d_y_error_source) {
    case UnknownErrors: {
      d_y_error_dataset = QString();
      // using 1.0 here is important for correct error estimates,
      // cmp. Fit::fitGslMultifit and Fit::fitGslMultimin
      for (int i = 0; i < d_n; i++) d_y_errors[i] = 1.0;
    } break;
    case AssociatedErrors: {
      /*bool error = true;
      QwtErrorPlotCurve *er = 0;
      if (d_curve && ((PlotCurve *)d_curve)->type() != Graph::Function) {
        QList<DataCurve *> lst = ((DataCurve *)d_curve)->errorBarsList();
        foreach (DataCurve *c, lst) {
          er = (QwtErrorPlotCurve *)c;
          if (!er->xErrors()) {
            d_y_error_dataset = er->title().text();
            error = false;
            break;
          }
        }
      }
      if (error) {
        if (!fail_silently)
          QMessageBox::critical(
              (ApplicationWindow *)parent(), tr("Error"),
              tr("The curve %1 has no associated Y error bars.")
                  .arg(d_curve->title().text()));
        return false;
      }
      if (er) {
        for (int j = 0; j < d_n; j++) d_y_errors[j] = er->errorValue(j);
      }
    */} break;
      case PoissonErrors: {
        d_y_error_dataset = associateddata_->table->name() + "_" +
                            associateddata_->xcol->name() + "_" +
                            associateddata_->ycol->name();

        for (int i = 0; i < d_n; i++) d_y_errors[i] = sqrt(d_y[i]);
      } break;
      case CustomErrors: {  // d_y_errors are equal to the values of the
                            // arbitrary dataset
        if (colName.isEmpty()) return false;

        Table *t = app_->table(colName);
        if (!t) return false;

        if (t->numRows() < d_n) {
          if (!fail_silently)
            QMessageBox::critical(
                app_, tr("Error"),
                tr("The column %1 has less points than the fitted data set. "
                   "Please choose another column!.")
                    .arg(colName));
          return false;
        }

        d_y_error_dataset = colName;

        int col = t->colIndex(colName);
        for (int i = 0; i < d_n; i++) d_y_errors[i] = t->cell(i, col);
      } break;
  }
  return true;
}

Table *Fit::parametersTable(const QString &tableName) {
  Table *t = app_->newTable(tableName, d_p, 3);
  t->setHeader(QStringList() << tr("Parameter") << tr("Value") << tr("Error"));
  t->column(0)->setColumnMode(AlphaPlot::Text);
  t->column(1)->setColumnMode(AlphaPlot::Numeric);
  t->column(2)->setColumnMode(AlphaPlot::Numeric);
  for (int i = 0; i < d_p; i++) {
    t->column(0)->setTextAt(i, d_param_names[i]);
    t->column(1)->setValueAt(i, d_results[i]);
    t->column(2)->setValueAt(i,
                             sqrt(gsl_matrix_get(covar, static_cast<size_t>(i),
                                                 static_cast<size_t>(i))));
  }

  t->column(2)->setPlotDesignation(AlphaPlot::yErr);
// TODO: replace or remove this
#if 0
for (int j=0; j<3; j++)
t->table()->adjustColumn(j);
#endif
  t->showNormal();
  return t;
}

Matrix *Fit::covarianceMatrix(const QString &matrixName) {
  Matrix *m = app_->newMatrix(matrixName, d_p, d_p);
  for (int i = 0; i < d_p; i++) {
    for (int j = 0; j < d_p; j++)
      m->setText(
          i, j,
          QLocale().toString(gsl_matrix_get(covar, static_cast<size_t>(i),
                                            static_cast<size_t>(j)),
                             'g', d_prec));
  }
  m->showNormal();
  return m;
}

double *Fit::errors() {
  if (!d_result_errors) {
    d_result_errors = new double[static_cast<size_t>(d_p)];
    double chi_2_dof = chi_2 / (d_n - d_p);
    for (int i = 0; i < d_p; i++) {
      if (d_scale_errors)
        d_result_errors[i] =
            sqrt(chi_2_dof * gsl_matrix_get(covar, static_cast<size_t>(i),
                                            static_cast<size_t>(i)));
      else
        d_result_errors[i] = sqrt(gsl_matrix_get(covar, static_cast<size_t>(i),
                                                 static_cast<size_t>(i)));
    }
  }
  return d_result_errors;
}

void Fit::storeCustomFitResults(double *par) {
  for (int i = 0; i < d_p; i++) d_results[i] = par[i];
}

void Fit::fit() {
  if (!axisrect_ || d_init_err) return;

  if (!d_n) {
    QMessageBox::critical(app_, tr("Fit Error"),
                          tr("You didn't specify a valid data set for this fit "
                             "operation. Operation aborted!"));
    return;
  }
  if (!d_p) {
    QMessageBox::critical(app_, tr("Fit Error"),
                          tr("There are no parameters specified for this fit "
                             "operation. Operation aborted!"));
    return;
  }
  if (d_p > d_n) {
    QMessageBox::critical(app_, tr("Fit Error"),
                          tr("You need at least %1 data points for this fit "
                             "operation. Operation aborted!")
                              .arg(d_p));
    return;
  }
  if (d_formula.isEmpty()) {
    QMessageBox::critical(
        app_, tr("Fit Error"),
        tr("You must specify a valid fit function first. Operation aborted!"));
    return;
  }

  QApplication::setOverrideCursor(Qt::WaitCursor);

  int status, iterations;
  double *par;
  d_script = scriptEnv->newScript(d_formula, this, metaObject()->className());
  connect(d_script, SIGNAL(error(const QString &, const QString &, int)), this,
          SLOT(scriptError(const QString &, const QString &, int)));

  if (d_solver == NelderMeadSimplex)
    par = fitGslMultimin(iterations, status);
  else
    par = fitGslMultifit(iterations, status);

  storeCustomFitResults(par);
  if (status == GSL_SUCCESS) generateFitCurve(par);

  delete[] par;
  delete d_script;

  if (app_->writeFitResultsToLog)
    app_->updateLog(logFitInfo(d_results, iterations, status,
                               associateddata_->table->name() + "_" +
                                   associateddata_->xcol->name() + "_" +
                                   associateddata_->ycol->name()));

  QApplication::restoreOverrideCursor();
}

void Fit::scriptError(const QString &message, const QString &script_name,
                      int line_number) {
  QMessageBox::critical(
      qobject_cast<QWidget *>(parent()), tr("Input function error"),
      QString("%1:%2\n").arg(script_name).arg(line_number) + message);
}

int Fit::evaluate_f(const gsl_vector *x, gsl_vector *f) {
  for (int i = 0; i < d_p; i++)
    d_script->setDouble(gsl_vector_get(x, static_cast<size_t>(i)),
                        d_param_names[i].toUtf8());
  for (int j = 0; j < d_n; j++) {
    d_script->setDouble(d_x[j], "x");
    bool success;
    gsl_vector_set(
        f, static_cast<size_t>(j),
        (d_script->eval().toDouble(&success) - d_y[j]) / d_y_errors[j]);
    if (!success) return GSL_EINVAL;
  }
  return GSL_SUCCESS;
}

double Fit::evaluate_d(const gsl_vector *x) {
  double result = 0.0;
  for (int i = 0; i < d_p; i++)
    d_script->setDouble(gsl_vector_get(x, static_cast<size_t>(i)),
                        d_param_names[i].toUtf8());
  for (int j = 0; j < d_n; j++) {
    d_script->setDouble(d_x[j], "x");
    bool success;
    result +=
        pow((d_script->eval().toDouble(&success) - d_y[j]) / d_y_errors[j], 2);
    if (!success) return GSL_EINVAL;
  }
  return result;
}

typedef struct {
  Script *script;
  QString param;
  bool success;
} DiffData;

double Fit::evaluate_df_helper(double x, void *params) {
  DiffData *data = static_cast<DiffData *>(params);
  data->script->setDouble(x, data->param.toUtf8());
  bool success;
  double result = data->script->eval().toDouble(&success);
  if (!success) {
    data->script->disconnect(
        SIGNAL(error(const QString &, const QString &, int)));
    data->success = false;
  }
  return result;
}

int Fit::evaluate_df(const gsl_vector *x, gsl_matrix *J) {
  double result, abserr;
  gsl_function F;
  F.function = &evaluate_df_helper;
  DiffData data;
  F.params = &data;
  data.script = d_script;
  data.success = true;
  for (int i = 0; i < d_p; i++)
    d_script->setDouble(gsl_vector_get(x, static_cast<size_t>(i)),
                        d_param_names[i].toUtf8());
  for (int i = 0; i < d_n; i++) {
    d_script->setDouble(d_x[i], "x");
    for (int j = 0; j < d_p; j++) {
      data.param = d_param_names[j];
      gsl_deriv_central(&F, gsl_vector_get(x, static_cast<size_t>(j)), 1e-8,
                        &result, &abserr);
      if (!data.success) return GSL_EINVAL;
      gsl_matrix_set(J, static_cast<size_t>(i), static_cast<size_t>(j),
                     result / d_y_errors[j]);
    }
  }
  return GSL_SUCCESS;
}

void Fit::generateFitCurve(double *par) {
  if (!d_gen_function) d_points = d_n;

  double *X = new double[static_cast<size_t>(d_points)];
  double *Y = new double[static_cast<size_t>(d_points)];

  calculateFitCurveData(par, X, Y);

  if (d_gen_function) {
    insertFitFunctionCurve(objectName() + tr("Fit"), X, Y);
    delete[] X;
    delete[] Y;
  } else
    addResultCurve(X, Y);
}

void Fit::insertFitFunctionCurve(const QString &name, double *x, double *y,
                                 int penWidth) {
  QString formula;
  for (int j = 0; j < d_p; j++)
    formula += QString("%1=%2\n")
                   .arg(d_param_names[j])
                   .arg(d_results[j], 0, 'g', d_prec);
  formula += "\n";
  formula += d_formula;
  Curve2D *curve = app_->addFunctionPlot(
      0, QStringList() << formula, QString("x"),
      QList<double>() << d_x[0] << d_x[d_n - 1], d_points, axisrect_);
  if (curve)
    curve->setlinestrokecolor_cplot(ColorBox::color(d_curveColorIndex));
  curve->layer()->replot();
  axisrect_->getLegend()->layer()->replot();
}

Fit::~Fit() {
  if (!d_p) return;

  if (is_non_linear) gsl_vector_free(d_param_init);

  if (d_results) delete[] d_results;
  if (d_result_errors) delete[] d_result_errors;
  gsl_matrix_free(covar);
}
