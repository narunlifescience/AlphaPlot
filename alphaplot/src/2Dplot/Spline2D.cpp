#include "Spline2D.h"
#include "../future/core/column/Column.h"
#include "core/Utilities.h"

#include <gsl/gsl_interp.h>
#include <gsl/gsl_sort.h>
#include <gsl/gsl_spline.h>
#include <QMessageBox>

#include <gsl/gsl_bspline.h>
#include <gsl/gsl_multifit.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_statistics.h>

Spline2D::Spline2D(Axis2D *xAxis, Axis2D *yAxis)
    : QCPCurve(xAxis, yAxis),
      xAxis_(xAxis),
      yAxis_(yAxis),
      picker_(Graph2DCommon::Picker::None) {
  layer()->setMode(QCPLayer::LayerMode::lmBuffered);
  setlinestrokecolor_splot(
      Utilities::getRandColorGoldenRatio(Utilities::ColorPal::Dark));
}

Spline2D::~Spline2D() {}

void Spline2D::setGraphData(Table *table, Column *xData, Column *yData,
                            int from, int to) {
  table_ = table;
  xcol_ = xData;
  ycol_ = yData;
  from_ = from;
  to_ = to;
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

  QSharedPointer<QCPCurveDataContainer> functionData(new QCPCurveDataContainer);
  QVector<QCPCurveData> *gdvector = new QVector<QCPCurveData>();
  for (int i = 0; i < xdata->size(); i++) {
    QCPCurveData fd;
    fd.key = xdata->at(i);
    fd.value = ydata->at(i);
    gdvector->append(fd);
  }
  functionData->add(*gdvector, true);
  gdvector->clear();
  delete gdvector;
  setData(functionData);
  // free those containers
  delete xdata;
  delete ydata;
}

void Spline2D::setSplineData(Column *xData, Column *yData, int from, int to) {
  int start_row = from;
  int end_row = to;
  if (end_row >= xData->rowCount()) end_row = xData->rowCount() - 1;
  if (end_row >= yData->rowCount()) end_row = yData->rowCount() - 1;
  QVector<QPair<double, double>> *points = new QVector<QPair<double, double>>();
  for (int row = start_row; row <= end_row; row++) {
    if (!xData->isInvalid(row) && !yData->isInvalid(row)) {
      QPair<double, double> point;
      point.first = xData->valueAt(row);
      point.second = yData->valueAt(row);
      points->append(point);
    }
  }

  QVector<double> *xdat = new QVector<double>();
  QVector<double> *ydat = new QVector<double>();

  const size_t n = points->size();
  const size_t ncoeffs = 12;
  const size_t nbreak = 10;
  size_t i, j;
  gsl_bspline_workspace *bw;
  gsl_vector *B;
  double dy;
  gsl_vector *c, *w;
  gsl_vector *x, *y;
  gsl_matrix *X, *cov;
  gsl_multifit_linear_workspace *mw;
  double chisq, Rsq, dof, tss;

  gsl_rng_env_setup();

  /* allocate a cubic bspline workspace (k = 4) */
  bw = gsl_bspline_alloc(4, nbreak);
  B = gsl_vector_alloc(ncoeffs);

  x = gsl_vector_alloc(n);
  y = gsl_vector_alloc(n);
  X = gsl_matrix_alloc(n, ncoeffs);
  c = gsl_vector_alloc(ncoeffs);
  w = gsl_vector_alloc(n);
  cov = gsl_matrix_alloc(ncoeffs, ncoeffs);
  mw = gsl_multifit_linear_alloc(n, ncoeffs);

  /* this is the data to be fitted */
  for (i = 0; i < n; ++i) {
    double sigma;
    double xi = points->at(i).first;
    double yi = points->at(i).second;

    sigma = 0.1 * yi;

    gsl_vector_set(x, i, xi);
    gsl_vector_set(y, i, yi);
    gsl_vector_set(w, i, 1.0 / (sigma * sigma));
  }

  /* use uniform breakpoints on [0, 15] */
  gsl_bspline_knots_uniform(0.0, 15.0, bw);

  /* construct the fit matrix X */
  for (i = 0; i < n; ++i) {
    double xi = gsl_vector_get(x, i);

    /* compute B_j(xi) for all j */
    gsl_bspline_eval(xi, B, bw);

    /* fill in row i of X */
    for (j = 0; j < ncoeffs; ++j) {
      double Bj = gsl_vector_get(B, j);
      gsl_matrix_set(X, i, j, Bj);
    }
  }

  /* do the fit */
  gsl_multifit_wlinear(X, w, y, c, cov, &chisq, mw);

  dof = n - ncoeffs;
  tss = gsl_stats_wtss(w->data, 1, y->data, 1, y->size);
  Rsq = 1.0 - chisq / tss;

  /* output the smoothed curve */
  {
    double xi, yi, yerr;

    for (xi = 0.0; xi < 1; xi += 0.1) {
      gsl_bspline_eval(xi, B, bw);
      gsl_multifit_linear_est(B, c, cov, &yi, &yerr);
      xdat->append(xi);
      ydat->append(yi);
    }
  }

  QSharedPointer<QCPCurveDataContainer> functionData(new QCPCurveDataContainer);
  for (int i = 0; i < xdat->size(); i++) {
    QCPCurveData fd;
    fd.key = xdat->at(i);
    fd.value = ydat->at(i);
    functionData.data()->add(fd);
  }

  setData(functionData);

  // free those containers
  gsl_bspline_free(bw);
  gsl_vector_free(B);
  gsl_vector_free(x);
  gsl_vector_free(y);
  gsl_matrix_free(X);
  gsl_vector_free(c);
  gsl_vector_free(w);
  gsl_matrix_free(cov);
  gsl_multifit_linear_free(mw);
}

Qt::PenStyle Spline2D::getlinestrokestyle_splot() const {
  return pen().style();
}

QColor Spline2D::getlinestrokecolor_splot() const { return pen().color(); }

double Spline2D::getlinestrokethickness_splot() const { return pen().widthF(); }

QColor Spline2D::getlinefillcolor_splot() const { return brush().color(); }

bool Spline2D::getlineantialiased_splot() const { return antialiased(); }

bool Spline2D::getlinefillstatus_splot() const {
  if (brush().style() == Qt::NoBrush) {
    return false;
  } else {
    return true;
  }
}

QString Spline2D::getlegendtext_splot() const { return name(); }

Axis2D *Spline2D::getxaxis_splot() const { return xAxis_; }

Axis2D *Spline2D::getyaxis_splot() const { return yAxis_; }

void Spline2D::setxaxis_splot(Axis2D *axis) {
  Q_ASSERT(axis->getorientation_axis() == Axis2D::AxisOreantation::Bottom ||
           axis->getorientation_axis() == Axis2D::AxisOreantation::Top);
  if (axis == getxaxis_splot()) return;

  xAxis_ = axis;
  setKeyAxis(axis);
}

void Spline2D::setyaxis_splot(Axis2D *axis) {
  Q_ASSERT(axis->getorientation_axis() == Axis2D::AxisOreantation::Left ||
           axis->getorientation_axis() == Axis2D::AxisOreantation::Right);
  if (axis == getyaxis_splot()) return;

  yAxis_ = axis;
  setValueAxis(axis);
}

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

void Spline2D::setlinefillstatus_splot(const bool value) {
  if (value) {
    QBrush b = brush();
    b.setStyle(Qt::SolidPattern);
    setBrush(b);
  } else {
    QBrush b = brush();
    b.setStyle(Qt::NoBrush);
    setBrush(b);
  }
}

void Spline2D::setlegendtext_splot(const QString &text) { setName(text); }

void Spline2D::setpicker_splot(const Graph2DCommon::Picker picker) {
  picker_ = picker;
}

void Spline2D::mousePressEvent(QMouseEvent *event, const QVariant &details) {
  if (event->button() == Qt::LeftButton) {
    switch (picker_) {
      case Graph2DCommon::Picker::None:
        break;
      case Graph2DCommon::Picker::DataPoint:
        datapicker(event, details);
        break;
      case Graph2DCommon::Picker::DataGraph:
        graphpicker(event, details);
        break;
      case Graph2DCommon::Picker::DataMove:
        movepicker(event, details);
        break;
      case Graph2DCommon::Picker::DataRemove:
        removepicker(event, details);
        break;
    }
  }
  QCPCurve::mousePressEvent(event, details);
}

void Spline2D::datapicker(QMouseEvent *event, const QVariant &details) {
  QCPCurveDataContainer::const_iterator it = data()->constEnd();
  QCPDataSelection dataPoints = details.value<QCPDataSelection>();
  if (dataPoints.dataPointCount() > 0) {
    dataPoints.dataRange();
    it = data()->at(dataPoints.dataRange().begin());
    QPointF point = coordsToPixels(it->mainKey(), it->mainValue());
    if (point.x() > event->posF().x() - 10 &&
        point.x() < event->posF().x() + 10 &&
        point.y() > event->posF().y() - 10 &&
        point.y() < event->posF().y() + 10) {
      emit showtooltip(point, it->mainKey(), it->mainValue());
    }
  }
}

void Spline2D::graphpicker(QMouseEvent *event, const QVariant &details) {
  double xvalue, yvalue;
  pixelsToCoords(event->posF(), xvalue, yvalue);
  emit showtooltip(event->posF(), xvalue, yvalue);
}

void Spline2D::movepicker(QMouseEvent *event, const QVariant &details) {}

void Spline2D::removepicker(QMouseEvent *event, const QVariant &details) {}
