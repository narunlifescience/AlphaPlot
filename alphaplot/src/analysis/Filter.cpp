/***************************************************************************
    File                 : Fit.cpp
    Project              : AlphaPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Ion Vasilief
    Email (use @ for *)  : ion_vasilief*yahoo.fr
    Description          : Abstract base class for data analysis operations

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
#include "Filter.h"
#include "2Dplot/AxisRect2D.h"
#include "ColorBox.h"
#include "Table.h"
#include "core/column/Column.h"

#include <QApplication>
#include <QLocale>
#include <QMessageBox>

#include <gsl/gsl_sort.h>

#include <algorithm>
using namespace std;

Filter::Filter(ApplicationWindow *parent, AxisRect2D *axisrect, QString name)
    : QObject(parent), app_(parent), axisrect_(axisrect) {
  QObject::setObjectName(name);
  init();
}

Filter::Filter(ApplicationWindow *parent, Table *table, QString name)
    : QObject(parent), app_(parent), d_table(table) {
  QObject::setObjectName(name);
  init();
  d_table = table;
}

void Filter::init() {
  d_n = 0;
  d_curveColorIndex = 1;
  d_tolerance = 1e-4;
  d_points = 100;
  d_max_iterations = 1000;
  associateddata_ = nullptr;
  d_prec = app_->fit_output_precision;
  d_init_err = false;
  d_sort_data = false;
  d_min_points = 2;
  d_explanation = objectName();
}

void Filter::setInterval(double from, double to) {
  if (!associateddata_) {
    QMessageBox::critical(app_, tr("AlphaPlot") + " - " + tr("Error"),
                          tr("Please assign a curve first!"));
    return;
  }
  setDataFromCurve(associateddata_, from, to);
}

void Filter::setDataCurve(PlotData::AssociatedData *associateddata,
                          double start, double end) {
  if (start > end) qSwap(start, end);

  if (d_n > 0) {  // delete previousely allocated memory
    delete[] d_x;
    delete[] d_y;
  }

  d_init_err = false;
  associateddata_ = associateddata;
  if (d_sort_data)
    d_n = sortedCurveData(start, end, &d_x, &d_y);
  else
    d_n = curveData(start, end, &d_x, &d_y);

  if (!isDataAcceptable()) {
    d_init_err = true;
    return;
  }

  // ensure range is within data range
  if (d_n > 0) {
    d_from = max(start, *min_element(d_x, d_x + d_n));
    d_to = min(end, *max_element(d_x, d_x + d_n));
  }
}

bool Filter::isDataAcceptable() {
  if (d_n < d_min_points) {
    QMessageBox::critical(
        app_, tr("AlphaPlot") + " - " + tr("Error"),
        tr("You need at least %1 points in order to perform this operation!")
            .arg(d_min_points));
    return false;
  }
  return true;
}

bool Filter::setDataFromCurve(PlotData::AssociatedData *associateddata,
                              AxisRect2D *axisrect) {
  if (!associateddata) {
    d_init_err = true;
    return false;
  }

  setDataCurve(associateddata, d_from, d_to);
  return true;
}

bool Filter::setDataFromCurve(PlotData::AssociatedData *associateddata,
                              double from, double to, AxisRect2D *axisrect) {
  if (!associateddata) {
    d_init_err = true;
    return false;
  }

  setDataCurve(associateddata, from, to);
  return true;
}

void Filter::setColor(const QString &colorName) {
  QColor c = QColor(colorName);
  if (colorName == "green")
    c = QColor(Qt::green);
  else if (colorName == "darkYellow")
    c = QColor(Qt::darkYellow);
  if (!ColorBox::isValidColor(c)) {
    QMessageBox::critical(app_, tr("Color Name Error"),
                          tr("The color name '%1' is not valid, a default "
                             "color (red) will be used instead!")
                              .arg(colorName));
    d_curveColorIndex = 1;
    return;
  }

  d_curveColorIndex = ColorBox::colorIndex(c);
}

bool Filter::run() {
  if (d_init_err) return false;

  if (d_n < 0) {
    QMessageBox::critical(
        app_, tr("AlphaPlot") + " - " + tr("Error"),
        tr("You didn't specify a valid data set for this operation!"));
    return false;
  }

  QApplication::setOverrideCursor(Qt::WaitCursor);

  output();  // data analysis and output
  app_->updateLog(logInfo());

  QApplication::restoreOverrideCursor();
  return true;
}

void Filter::output() {
  double *X = new double[d_points];
  double *Y = new double[d_points];

  // do the data analysis
  calculateOutputData(X, Y);

  addResultCurve(X, Y);
}

int Filter::sortedCurveData(double start, double end, double **x, double **y) {
  if (!associateddata_) return 0;

  // start/end finding only works on nondecreasing data, so sort first
  int from = associateddata_->from;
  int to = associateddata_->to;
  int datasize = (to - from) + 1;

  double *xtemp = new double[static_cast<size_t>(datasize)];
  for (int i = 0, j = from; i < datasize; i++) {
    xtemp[i] = associateddata_->xcol->valueAt(j);
    j++;
  }

  size_t *p = new size_t[static_cast<size_t>(datasize)];
  gsl_sort_index(p, xtemp, 1, static_cast<size_t>(datasize));
  delete[] xtemp;

  // make result arrays
  int n = datasize;
  (*x) = new double[n];
  (*y) = new double[n];
  for (int j = 0, i = from; i <= to + 1; i++, j++) {
    (*x)[j] = associateddata_->xcol->valueAt(p[j] + from);
    (*y)[j] = associateddata_->ycol->valueAt(p[j] + from);
  }
  delete[] p;
  return n;
}

int Filter::curveData(double start, double end, double **x, double **y) {
  if (!associateddata_) return 0;

  int from = associateddata_->from;
  int to = associateddata_->to;
  int datasize = (to - from) + 1;
  int i_start = from, i_end = to;
  /*for (i_start = from; i_start < datasize; i_start++)
    if (associateddata_->xcol->valueAt(i_start) >= start) break;
  for (i_end = datasize - 1; i_end >= 0; i_end--)
    if (associateddata_->xcol->valueAt(i_end) <= end) break;*/

  int n = i_end - i_start + 1;
  (*x) = new double[n];
  (*y) = new double[n];

  for (int j = 0, i = i_start; i <= i_end; i++, j++) {
    (*x)[j] = associateddata_->xcol->valueAt(i);
    (*y)[j] = associateddata_->ycol->valueAt(i);
  }
  return n;
}

Curve2D *Filter::addResultCurve(double *xdata, double *ydata) {
  const QString tableName = app_->generateUniqueName(this->objectName());
  Column *xCol =
      new Column(tr("1", "filter table x column name"), AlphaPlot::Numeric);
  Column *yCol =
      new Column(tr("2", "filter table y column name"), AlphaPlot::Numeric);
  xCol->setPlotDesignation(AlphaPlot::X);
  yCol->setPlotDesignation(AlphaPlot::Y);
  for (int i = 0; i < d_points; i++) {
    xCol->setValueAt(i, xdata[i]);
    yCol->setValueAt(i, ydata[i]);
  }
  // first set the values, then add the columns to the table, otherwise, we
  // generate too many undo commands
  QString label = associateddata_->table->name() + "_" +
                  associateddata_->xcol->name() + "_" +
                  associateddata_->ycol->name();
  Table *table = app_->newHiddenTable(
      tableName, d_explanation + " " + tr("of") + " " + label,
      QList<Column *>() << xCol << yCol);
  QList<Axis2D *> xaxes = axisrect_->getXAxes2D();
  QList<Axis2D *> yaxes = axisrect_->getYAxes2D();
  Curve2D *curve = axisrect_->addCurve2DPlot(
      AxisRect2D::LineScatterType::Line2D, table, xCol, yCol, 0,
      xCol->rowCount() - 1, xaxes.at(0), yaxes.at(0));
  xaxes.at(0)->rescale();
  yaxes.at(0)->rescale();

  delete[] xdata;
  delete[] ydata;
  return curve;
}

Filter::~Filter() {
  if (d_n > 0) {  // delete the memory allocated for the data
    delete[] d_x;
    delete[] d_y;
  }
}
