#include "Spline2D.h"
#include "../future/core/column/Column.h"
#include "DataManager2D.h"
#include "analysis/spline.h"
#include "core/Utilities.h"

#include <gsl/gsl_interp.h>
#include <gsl/gsl_sort.h>
#include <gsl/gsl_spline.h>
#include <QMessageBox>

#include <gsl/gsl_bspline.h>
#include <gsl/gsl_multifit.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_statistics.h>

Spline2D::Spline2D(Table *table, Column *xcol, Column *ycol, int from, int to,
                   Axis2D *xAxis, Axis2D *yAxis)
    : QCPCurve(xAxis, yAxis),
      xAxis_(xAxis),
      yAxis_(yAxis),
      scatterstyle_(new QCPScatterStyle),
      curvedata_(new DataBlockCurve(table, xcol, ycol, from, to)),
      picker_(Graph2DCommon::Picker::None) {
  setSelectable(QCP::SelectionType::stSingleData);
  layer()->setMode(QCPLayer::LayerMode::lmBuffered);
  setlinestrokecolor_splot(
      Utilities::getRandColorGoldenRatio(Utilities::ColorPal::Dark));
  setscattershape_splot(Graph2DCommon::ScatterStyle::None);
  testData(table, xcol, ycol, from, to);
}

Spline2D::~Spline2D() {
  delete scatterstyle_;
  delete curvedata_;
}

void Spline2D::setSplineData(Table *table, Column *xData, Column *yData,
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

void Spline2D::testData(Table *table, Column *xData, Column *yData, int from,
                        int to) {
  QPolygonF poly;
  for (int i = 0, xi = from; i < to; i++, xi++) {
    poly.append(QPointF(xData->valueAt(xi), yData->valueAt(xi)));
  }
  Spline spline;
  spline.setPoints(poly);

  for (double i = poly.at(0).x(); i < poly.at(poly.size() - 1).x(); i++) {
    qDebug() << QPointF(i, spline.value(i));
  }
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

Graph2DCommon::ScatterStyle Spline2D::getscattershape_splot() const {
  Graph2DCommon::ScatterStyle scatterstyle;
  switch (scatterStyle().shape()) {
    case QCPScatterStyle::ssNone:
      scatterstyle = Graph2DCommon::ScatterStyle::None;
      break;
    case QCPScatterStyle::ssDot:
      scatterstyle = Graph2DCommon::ScatterStyle::Dot;
      break;
    case QCPScatterStyle::ssCross:
      scatterstyle = Graph2DCommon::ScatterStyle::Cross;
      break;
    case QCPScatterStyle::ssPlus:
      scatterstyle = Graph2DCommon::ScatterStyle::Plus;
      break;
    case QCPScatterStyle::ssCircle:
      scatterstyle = Graph2DCommon::ScatterStyle::Circle;
      break;
    case QCPScatterStyle::ssDisc:
      scatterstyle = Graph2DCommon::ScatterStyle::Disc;
      break;
    case QCPScatterStyle::ssSquare:
      scatterstyle = Graph2DCommon::ScatterStyle::Square;
      break;
    case QCPScatterStyle::ssDiamond:
      scatterstyle = Graph2DCommon::ScatterStyle::Diamond;
      break;
    case QCPScatterStyle::ssStar:
      scatterstyle = Graph2DCommon::ScatterStyle::Star;
      break;
    case QCPScatterStyle::ssTriangle:
      scatterstyle = Graph2DCommon::ScatterStyle::Triangle;
      break;
    case QCPScatterStyle::ssTriangleInverted:
      scatterstyle = Graph2DCommon::ScatterStyle::TriangleInverted;
      break;
    case QCPScatterStyle::ssCrossSquare:
      scatterstyle = Graph2DCommon::ScatterStyle::CrossSquare;
      break;
    case QCPScatterStyle::ssPlusSquare:
      scatterstyle = Graph2DCommon::ScatterStyle::PlusSquare;
      break;
    case QCPScatterStyle::ssCrossCircle:
      scatterstyle = Graph2DCommon::ScatterStyle::CrossCircle;
      break;
    case QCPScatterStyle::ssPlusCircle:
      scatterstyle = Graph2DCommon::ScatterStyle::PlusCircle;
      break;
    case QCPScatterStyle::ssPeace:
      scatterstyle = Graph2DCommon::ScatterStyle::Peace;
      break;
    case QCPScatterStyle::ssCustom:
    case QCPScatterStyle::ssPixmap:
      qDebug() << "QCPScatterStyle::ssCustom & QCPScatterStyle::ssPixmap "
                  "unsupported! using QCPScatterStyle::ssDisc insted";
      scatterstyle = Graph2DCommon::ScatterStyle::Disc;
      break;
  }
  return scatterstyle;
}

QColor Spline2D::getscatterfillcolor_splot() const {
  return scatterStyle().brush().color();
}

double Spline2D::getscattersize_splot() const { return scatterStyle().size(); }

Qt::PenStyle Spline2D::getscatterstrokestyle_splot() const {
  return scatterStyle().pen().style();
}

QColor Spline2D::getscatterstrokecolor_splot() const {
  return scatterStyle().pen().color();
}

double Spline2D::getscatterstrokethickness_splot() const {
  return scatterStyle().pen().widthF();
}

bool Spline2D::getscatterantialiased_splot() const {
  return antialiasedScatters();
}

QString Spline2D::getlegendtext_splot() const { return name(); }

Axis2D *Spline2D::getxaxis_splot() const { return xAxis_; }

Axis2D *Spline2D::getyaxis_splot() const { return yAxis_; }

DataBlockCurve *Spline2D::getdatablock_splot() const { return curvedata_; }

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

void Spline2D::setscattershape_splot(const Graph2DCommon::ScatterStyle &shape) {
  switch (shape) {
    case Graph2DCommon::ScatterStyle::None:
      scatterstyle_->setShape(QCPScatterStyle::ssNone);
      break;
    case Graph2DCommon::ScatterStyle::Dot:
      scatterstyle_->setShape(QCPScatterStyle::ssDot);
      break;
    case Graph2DCommon::ScatterStyle::Cross:
      scatterstyle_->setShape(QCPScatterStyle::ssCross);
      break;
    case Graph2DCommon::ScatterStyle::Plus:
      scatterstyle_->setShape(QCPScatterStyle::ssPlus);
      break;
    case Graph2DCommon::ScatterStyle::Circle:
      scatterstyle_->setShape(QCPScatterStyle::ssCircle);
      break;
    case Graph2DCommon::ScatterStyle::Disc:
      scatterstyle_->setShape(QCPScatterStyle::ssDisc);
      break;
    case Graph2DCommon::ScatterStyle::Square:
      scatterstyle_->setShape(QCPScatterStyle::ssSquare);
      break;
    case Graph2DCommon::ScatterStyle::Diamond:
      scatterstyle_->setShape(QCPScatterStyle::ssDiamond);
      break;
    case Graph2DCommon::ScatterStyle::Star:
      scatterstyle_->setShape(QCPScatterStyle::ssStar);
      break;
    case Graph2DCommon::ScatterStyle::Triangle:
      scatterstyle_->setShape(QCPScatterStyle::ssTriangle);
      break;
    case Graph2DCommon::ScatterStyle::TriangleInverted:
      scatterstyle_->setShape(QCPScatterStyle::ssTriangleInverted);
      break;
    case Graph2DCommon::ScatterStyle::CrossSquare:
      scatterstyle_->setShape(QCPScatterStyle::ssCrossSquare);
      break;
    case Graph2DCommon::ScatterStyle::PlusSquare:
      scatterstyle_->setShape(QCPScatterStyle::ssPlusSquare);
      break;
    case Graph2DCommon::ScatterStyle::CrossCircle:
      scatterstyle_->setShape(QCPScatterStyle::ssCrossCircle);
      break;
    case Graph2DCommon::ScatterStyle::PlusCircle:
      scatterstyle_->setShape(QCPScatterStyle::ssPlusCircle);
      break;
    case Graph2DCommon::ScatterStyle::Peace:
      scatterstyle_->setShape(QCPScatterStyle::ssPeace);
      break;
  }
  setScatterStyle(*scatterstyle_);
}

void Spline2D::setscatterfillcolor_splot(const QColor &color) {
  QBrush b = scatterstyle_->brush();
  b.setColor(color);
  scatterstyle_->setBrush(b);
  setScatterStyle(*scatterstyle_);
}

void Spline2D::setscattersize_splot(const double value) {
  scatterstyle_->setSize(value);
  setScatterStyle(*scatterstyle_);
}

void Spline2D::setscatterstrokestyle_splot(const Qt::PenStyle &style) {
  QPen p = scatterstyle_->pen();
  p.setStyle(style);
  scatterstyle_->setPen(p);
  setScatterStyle(*scatterstyle_);
}

void Spline2D::setscatterstrokecolor_splot(const QColor &color) {
  QPen p = scatterstyle_->pen();
  p.setColor(color);
  scatterstyle_->setPen(p);
  setScatterStyle(*scatterstyle_);
}

void Spline2D::setscatterstrokethickness_splot(const double value) {
  QPen p = scatterstyle_->pen();
  p.setWidthF(value);
  scatterstyle_->setPen(p);
  setScatterStyle(*scatterstyle_);
}

void Spline2D::setscatterantialiased_splot(const bool value) {
  setAntialiasedScatters(value);
}

void Spline2D::setlegendtext_splot(const QString &text) { setName(text); }

void Spline2D::setpicker_splot(const Graph2DCommon::Picker picker) {
  picker_ = picker;
}

void Spline2D::draw(QCPPainter *painter) { QCPCurve::draw(painter); }

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
    if (point.x() > event->localPos().x() - 10 &&
        point.x() < event->localPos().x() + 10 &&
        point.y() > event->localPos().y() - 10 &&
        point.y() < event->localPos().y() + 10) {
      emit showtooltip(point, it->mainKey(), it->mainValue());
    }
  }
}

void Spline2D::graphpicker(QMouseEvent *event, const QVariant &details) {
  double xvalue, yvalue;
  pixelsToCoords(event->localPos(), xvalue, yvalue);
  emit showtooltip(event->localPos(), xvalue, yvalue);
}

void Spline2D::movepicker(QMouseEvent *event, const QVariant &details) {}

void Spline2D::removepicker(QMouseEvent *event, const QVariant &details) {}
