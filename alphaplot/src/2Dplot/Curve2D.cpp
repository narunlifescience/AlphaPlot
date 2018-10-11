#include "Curve2D.h"
#include "DataManager2D.h"
#include "core/Utilities.h"

Curve2D::Curve2D(Table *table, Column *xcol, Column *ycol, int from, int to,
                 Axis2D *xAxis, Axis2D *yAxis)
    : QCPCurve(xAxis, yAxis),
      xAxis_(xAxis),
      yAxis_(yAxis),
      scatterstyle_(new QCPScatterStyle(
          QCPScatterStyle::ssNone,
          Utilities::getRandColorGoldenRatio(Utilities::ColorPal::Dark),
          Utilities::getRandColorGoldenRatio(Utilities::ColorPal::Dark), 6.0)),
      curvedata_(new DataBlockCurve(table, xcol, ycol, from, to)),
      functionData_(nullptr),
      type_(Graph2DCommon::PlotType::Associated),
      picker_(Graph2DCommon::Picker::None) {
  setSelectable(QCP::SelectionType::stSingleData);
  layer()->setMode(QCPLayer::LayerMode::lmBuffered);
  setlinestrokecolor_cplot(
      Utilities::getRandColorGoldenRatio(Utilities::ColorPal::Dark));
  setData(curvedata_->data());
}

Curve2D::Curve2D(QVector<double> *xdata, QVector<double> *ydata, Axis2D *xAxis,
                 Axis2D *yAxis)
    : QCPCurve(xAxis, yAxis),
      xAxis_(xAxis),
      yAxis_(yAxis),
      scatterstyle_(new QCPScatterStyle(
          QCPScatterStyle::ssNone,
          Utilities::getRandColorGoldenRatio(Utilities::ColorPal::Dark),
          Utilities::getRandColorGoldenRatio(Utilities::ColorPal::Dark), 6.0)),
      curvedata_(nullptr),
      functionData_(new QCPCurveDataContainer),
      type_(Graph2DCommon::PlotType::Function) {
  Q_ASSERT(xdata->size() == ydata->size());
  setlinestrokecolor_cplot(
      Utilities::getRandColorGoldenRatio(Utilities::ColorPal::Dark));

  for (int i = 0; i < xdata->size(); i++) {
    QCPCurveData fd;
    fd.key = xdata->at(i);
    fd.value = ydata->at(i);
    functionData_->add(fd);
  }
  setData(functionData_);
  // free those containers
  delete xdata;
  delete ydata;
}

Curve2D::~Curve2D() {
  delete scatterstyle_;
  switch (type_) {
    case Graph2DCommon::PlotType::Associated:
      delete curvedata_;
      break;
    case Graph2DCommon::PlotType::Function:
      break;
  }
}

void Curve2D::setGraphData(QVector<double> *xdata, QVector<double> *ydata) {
  if (type_ == Graph2DCommon::PlotType::Associated) {
    qDebug() << "cannot add function data to association plot";
    return;
  }

  Q_ASSERT(xdata->size() == ydata->size());

  functionData_.data()->clear();
  for (int i = 0; i < xdata->size(); i++) {
    QCPCurveData fd;
    fd.key = xdata->at(i);
    fd.value = ydata->at(i);
    functionData_->add(fd);
  }
  setData(functionData_);
  // free those containers
  delete xdata;
  delete ydata;
}

void Curve2D::setCurveData(Table *table, Column *xcol, Column *ycol, int from,
                           int to) {
  if (type_ == Graph2DCommon::PlotType::Function) {
    qDebug() << "cannot associate table with function plot";
    return;
  }
  curvedata_->regenerateDataBlock(table, xcol, ycol, from, to);
  setData(curvedata_->data());
}

int Curve2D::getlinetype_cplot() const {
  switch (lineStyle()) {
    case LineStyle::lsNone:
      return 0;
    case LineStyle::lsLine:
      return 1;
  }
}

Qt::PenStyle Curve2D::getlinestrokestyle_cplot() const { return pen().style(); }

QColor Curve2D::getlinestrokecolor_cplot() const { return pen().color(); }

double Curve2D::getlinestrokethickness_cplot() const { return pen().widthF(); }

QColor Curve2D::getlinefillcolor_cplot() const { return brush().color(); }

bool Curve2D::getlineantialiased_cplot() const { return antialiased(); }

bool Curve2D::getlinefillstatus_cplot() const {
  if (brush().style() == Qt::NoBrush) {
    return false;
  } else {
    return true;
  }
}

Graph2DCommon::ScatterStyle Curve2D::getscattershape_cplot() const {
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

QColor Curve2D::getscatterfillcolor_cplot() const {
  return scatterStyle().brush().color();
}

double Curve2D::getscattersize_cplot() const { return scatterStyle().size(); }

Qt::PenStyle Curve2D::getscatterstrokestyle_cplot() const {
  return scatterStyle().pen().style();
}

QColor Curve2D::getscatterstrokecolor_cplot() const {
  return scatterStyle().pen().color();
}

double Curve2D::getscatterstrokethickness_cplot() const {
  return scatterStyle().pen().widthF();
}

bool Curve2D::getscatterantialiased_cplot() const {
  return antialiasedScatters();
}

QString Curve2D::getlegendtext_cplot() const { return name(); }

Axis2D *Curve2D::getxaxis_cplot() const { return xAxis_; }

Axis2D *Curve2D::getyaxis_cplot() const { return yAxis_; }

void Curve2D::setxaxis_cplot(Axis2D *axis) {
  Q_ASSERT(axis->getorientation_axis() == Axis2D::AxisOreantation::Bottom ||
           axis->getorientation_axis() == Axis2D::AxisOreantation::Top);
  if (axis == getxaxis_cplot()) return;

  xAxis_ = axis;
  setKeyAxis(axis);
}

void Curve2D::setyaxis_cplot(Axis2D *axis) {
  Q_ASSERT(axis->getorientation_axis() == Axis2D::AxisOreantation::Left ||
           axis->getorientation_axis() == Axis2D::AxisOreantation::Right);
  if (axis == getyaxis_cplot()) return;

  yAxis_ = axis;
  setValueAxis(axis);
}

void Curve2D::setlinetype_cplot(const int type) {
  switch (type) {
    case 0:
      setLineStyle(LineStyle::lsNone);
      break;
    case 1:
      setLineStyle(LineStyle::lsLine);
      break;
  }
}

void Curve2D::setlinestrokestyle_cplot(const Qt::PenStyle &style) {
  QPen p = pen();
  p.setStyle(style);
  setPen(p);
}

void Curve2D::setlinestrokecolor_cplot(const QColor &color) {
  QPen p = pen();
  p.setColor(color);
  setPen(p);
}

void Curve2D::setlinestrokethickness_cplot(const double value) {
  QPen p = pen();
  p.setWidthF(value);
  setPen(p);
}

void Curve2D::setlinefillcolor_cplot(const QColor &color) {
  QBrush b = brush();
  b.setColor(color);
  setBrush(b);
}

void Curve2D::setlineantialiased_cplot(const bool value) {
  setAntialiased(value);
}

void Curve2D::setscattershape_cplot(const Graph2DCommon::ScatterStyle &shape) {
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

void Curve2D::setscatterfillcolor_cplot(const QColor &color) {
  QBrush b = scatterstyle_->brush();
  b.setColor(color);
  scatterstyle_->setBrush(b);
  setScatterStyle(*scatterstyle_);
}

void Curve2D::setscattersize_cplot(const double value) {
  scatterstyle_->setSize(value);
  setScatterStyle(*scatterstyle_);
}

void Curve2D::setscatterstrokestyle_cplot(const Qt::PenStyle &style) {
  QPen p = scatterstyle_->pen();
  p.setStyle(style);
  scatterstyle_->setPen(p);
  setScatterStyle(*scatterstyle_);
}

void Curve2D::setscatterstrokecolor_cplot(const QColor &color) {
  QPen p = scatterstyle_->pen();
  p.setColor(color);
  scatterstyle_->setPen(p);
  setScatterStyle(*scatterstyle_);
}

void Curve2D::setscatterstrokethickness_cplot(const double value) {
  QPen p = scatterstyle_->pen();
  p.setWidthF(value);
  scatterstyle_->setPen(p);
  setScatterStyle(*scatterstyle_);
}

void Curve2D::setscatterantialiased_cplot(const bool value) {
  setAntialiasedScatters(value);
}

void Curve2D::setlinefillstatus_cplot(const bool value) {
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

void Curve2D::setlegendtext_cplot(const QString &text) { setName(text); }

void Curve2D::setpicker_cplot(const Graph2DCommon::Picker picker) {
  picker_ = picker;
}

void Curve2D::mousePressEvent(QMouseEvent *event, const QVariant &details) {
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

void Curve2D::datapicker(QMouseEvent *event, const QVariant &details) {
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

void Curve2D::graphpicker(QMouseEvent *event, const QVariant &details) {
  Q_UNUSED(details);
  double xvalue, yvalue;
  pixelsToCoords(event->posF(), xvalue, yvalue);
  emit showtooltip(event->posF(), xvalue, yvalue);
}

void Curve2D::movepicker(QMouseEvent *event, const QVariant &details) {}

void Curve2D::removepicker(QMouseEvent *event, const QVariant &details) {}
