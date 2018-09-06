#include "Spline2D.h"
#include "../future/core/column/Column.h"
#include "core/Utilities.h"

#include <gsl/gsl_interp.h>
#include <gsl/gsl_sort.h>
#include <gsl/gsl_spline.h>
#include <QMessageBox>

Spline2D::Spline2D(Axis2D *xAxis, Axis2D *yAxis)
    : QCPGraph(xAxis, yAxis), xAxis_(xAxis), yAxis_(yAxis) {
  setLineStyle(QCPGraph::lsLine);

  setlinestrokecolor_splot(
      Utilities::getRandColorGoldenRatio(Utilities::ColorPal::Dark));
}

Spline2D::~Spline2D() {}

void Spline2D::setGraphData(Column *xData, Column *yData, int from, int to) {
  int d_n = (to - from) + 1;

  if (d_n < 4) {
    QMessageBox::critical(
        this->parentPlot(), tr("AlphaPlot") + " - " + tr("Error"),
        tr("You need at least 4 points in order to draw a cubic spline!"));
    return;
  }

  double *d_x = new double[d_n];
  double *d_y = new double[d_n];
  int xi = from;
  for (int i = 0, xi = from; i < d_n; i++, xi++) {
    d_x[i] = xData->valueAt(xi);
    d_y[i] = yData->valueAt(xi);
  }

  // sort required for interpolation
  gsl_sort2(d_x, 1, d_y, 1, static_cast<size_t>(d_n));

  for (int i = 1; i < d_n; i++)
    if (d_x[i - 1] == d_x[i]) {
      QMessageBox::critical(
            this->parentPlot(), tr("AlphaPlot") + " - " + tr("Error"),
          tr("Several data points have the same x value causing divisions by "
             "zero, operation aborted!"));
      delete[] d_x;
      delete[] d_y;
      return;
    }

  gsl_interp_accel *acc = gsl_interp_accel_alloc();
  gsl_spline *interp =
      gsl_spline_alloc(gsl_interp_cspline, static_cast<size_t>(d_n));
  double d_to = d_x[d_n - 1];
  double d_from = d_x[0];
  int d_points = d_n * 20;
  QVector<double> *xdata = new QVector<double>();
  QVector<double> *ydata = new QVector<double>();

  gsl_spline_init(interp, d_x, d_y, static_cast<size_t>(d_n));

  double step = (d_to - d_from) / static_cast<double>(d_points - 1);
  for (int j = 0; j < d_points; j++) {
    xdata->append(d_from + j * step);
    ydata->append(gsl_spline_eval(interp, xdata->at(j), acc));
  }

  // frtee used up resources
  gsl_spline_free(interp);
  gsl_interp_accel_free(acc);
  delete[] d_x;
  delete[] d_y;

  QSharedPointer<QCPGraphDataContainer> functionData(new QCPGraphDataContainer);
  for (int i = 0; i < xdata->size(); i++) {
    QCPGraphData fd;
    fd.key = xdata->at(i);
    fd.value = ydata->at(i);
    functionData->add(fd);
  }
  setData(functionData);
  // free those containers
  delete xdata;
  delete ydata;
}

Qt::PenStyle Spline2D::getlinestrokestyle_splot() const {
  return pen().style();
}

QColor Spline2D::getlinestrokecolor_splot() const { return pen().color(); }

double Spline2D::getlinestrokethickness_splot() const { return pen().widthF(); }

QColor Spline2D::getlinefillcolor_splot() const { return brush().color(); }

bool Spline2D::getlineantialiased_splot() const { return antialiased(); }

void Spline2D::setlinestrokestyle_splot(const Qt::PenStyle &style) {
  QPen p = pen();
  p.setStyle(style);
  setPen(p);
}

void Spline2D::setlinestrokecolor_splot(const QColor &color) {
  QPen p = pen();
  p.setColor(color);
  setPen(p);
}

void Spline2D::setlinestrokethickness_splot(const double value) {
  QPen p = pen();
  p.setWidthF(value);
  setPen(p);
}

void Spline2D::setlinefillcolor_splot(const QColor &color) {
  QBrush b = brush();
  b.setColor(color);
  setBrush(b);
}

void Spline2D::setlineantialiased_splot(const bool value) {
  setAntialiased(value);
}
