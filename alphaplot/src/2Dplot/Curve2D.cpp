#include "Curve2D.h"
#include "DataManager2D.h"
#include "core/Utilities.h"

Curve2D::Curve2D(Axis2D *xAxis, Axis2D *yAxis)
    : QCPCurve(xAxis, yAxis),
      xAxis_(xAxis),
      yAxis_(yAxis),
      scatterstyle_(new QCPScatterStyle(
          QCPScatterStyle::ssNone,
          Utilities::getRandColorGoldenRatio(Utilities::ColorPal::Dark),
          Utilities::getRandColorGoldenRatio(Utilities::ColorPal::Dark), 6.0)),
      curvedata_(nullptr) {
  setlinestrokecolor_cplot(
      Utilities::getRandColorGoldenRatio(Utilities::ColorPal::Dark));
}

Curve2D::~Curve2D() {
  delete scatterstyle_;
  delete curvedata_;
}

void Curve2D::setGraphData(QVector<double> *xdata, QVector<double> *ydata) {
  Q_ASSERT(xdata->size() == ydata->size());

  QSharedPointer<QCPCurveDataContainer> functionData(new QCPCurveDataContainer);
  for (int i = 0; i < xdata->size(); i++) {
    QCPCurveData fd;
    fd.key = xdata->at(i);
    fd.value = ydata->at(i);
    functionData->add(fd);
  }
  setData(functionData);
  // free those containers
  delete xdata;
  delete ydata;
}

void Curve2D::setGraphData(Table *table, QString xcolname, QString ycolname,
                           int from, int to) {
  if (curvedata_) {
    qDebug() << "DataBlockCurveData already set";
    return;
  }
  curvedata_ = new DataBlockCurveData(table, xcolname, ycolname, from, to);
  setData(curvedata_->data());
}

int Curve2D::getlinetype_cplot() const {
  switch (lineStyle()) {
    case LineStyle::lsNone:
      return 0;
    case LineStyle::lsLine:
      return 1;
    default:
      qDebug() << "unknown LineStyle";
      return 0;
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

LSCommon::ScatterStyle Curve2D::getscattershape_cplot() const {
  LSCommon::ScatterStyle scatterstyle;
  switch (scatterStyle().shape()) {
    case QCPScatterStyle::ssNone:
      scatterstyle = LSCommon::ScatterStyle::None;
      break;
    case QCPScatterStyle::ssDot:
      scatterstyle = LSCommon::ScatterStyle::Dot;
      break;
    case QCPScatterStyle::ssCross:
      scatterstyle = LSCommon::ScatterStyle::Cross;
      break;
    case QCPScatterStyle::ssPlus:
      scatterstyle = LSCommon::ScatterStyle::Plus;
      break;
    case QCPScatterStyle::ssCircle:
      scatterstyle = LSCommon::ScatterStyle::Circle;
      break;
    case QCPScatterStyle::ssDisc:
      scatterstyle = LSCommon::ScatterStyle::Disc;
      break;
    case QCPScatterStyle::ssSquare:
      scatterstyle = LSCommon::ScatterStyle::Square;
      break;
    case QCPScatterStyle::ssDiamond:
      scatterstyle = LSCommon::ScatterStyle::Diamond;
      break;
    case QCPScatterStyle::ssStar:
      scatterstyle = LSCommon::ScatterStyle::Star;
      break;
    case QCPScatterStyle::ssTriangle:
      scatterstyle = LSCommon::ScatterStyle::Triangle;
      break;
    case QCPScatterStyle::ssTriangleInverted:
      scatterstyle = LSCommon::ScatterStyle::TriangleInverted;
      break;
    case QCPScatterStyle::ssCrossSquare:
      scatterstyle = LSCommon::ScatterStyle::CrossSquare;
      break;
    case QCPScatterStyle::ssPlusSquare:
      scatterstyle = LSCommon::ScatterStyle::PlusSquare;
      break;
    case QCPScatterStyle::ssCrossCircle:
      scatterstyle = LSCommon::ScatterStyle::CrossCircle;
      break;
    case QCPScatterStyle::ssPlusCircle:
      scatterstyle = LSCommon::ScatterStyle::PlusCircle;
      break;
    case QCPScatterStyle::ssPeace:
      scatterstyle = LSCommon::ScatterStyle::Peace;
      break;
    case QCPScatterStyle::ssCustom:
    case QCPScatterStyle::ssPixmap:
      qDebug() << "QCPScatterStyle::ssCustom & QCPScatterStyle::ssPixmap "
                  "unsupported! using QCPScatterStyle::ssDisc insted";
      scatterstyle = LSCommon::ScatterStyle::Disc;
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

void Curve2D::setscattershape_cplot(const LSCommon::ScatterStyle &shape) {
  switch (shape) {
    case LSCommon::ScatterStyle::None:
      scatterstyle_->setShape(QCPScatterStyle::ssNone);
      break;
    case LSCommon::ScatterStyle::Dot:
      scatterstyle_->setShape(QCPScatterStyle::ssDot);
      break;
    case LSCommon::ScatterStyle::Cross:
      scatterstyle_->setShape(QCPScatterStyle::ssCross);
      break;
    case LSCommon::ScatterStyle::Plus:
      scatterstyle_->setShape(QCPScatterStyle::ssPlus);
      break;
    case LSCommon::ScatterStyle::Circle:
      scatterstyle_->setShape(QCPScatterStyle::ssCircle);
      break;
    case LSCommon::ScatterStyle::Disc:
      scatterstyle_->setShape(QCPScatterStyle::ssDisc);
      break;
    case LSCommon::ScatterStyle::Square:
      scatterstyle_->setShape(QCPScatterStyle::ssSquare);
      break;
    case LSCommon::ScatterStyle::Diamond:
      scatterstyle_->setShape(QCPScatterStyle::ssDiamond);
      break;
    case LSCommon::ScatterStyle::Star:
      scatterstyle_->setShape(QCPScatterStyle::ssStar);
      break;
    case LSCommon::ScatterStyle::Triangle:
      scatterstyle_->setShape(QCPScatterStyle::ssTriangle);
      break;
    case LSCommon::ScatterStyle::TriangleInverted:
      scatterstyle_->setShape(QCPScatterStyle::ssTriangleInverted);
      break;
    case LSCommon::ScatterStyle::CrossSquare:
      scatterstyle_->setShape(QCPScatterStyle::ssCrossSquare);
      break;
    case LSCommon::ScatterStyle::PlusSquare:
      scatterstyle_->setShape(QCPScatterStyle::ssPlusSquare);
      break;
    case LSCommon::ScatterStyle::CrossCircle:
      scatterstyle_->setShape(QCPScatterStyle::ssCrossCircle);
      break;
    case LSCommon::ScatterStyle::PlusCircle:
      scatterstyle_->setShape(QCPScatterStyle::ssPlusCircle);
      break;
    case LSCommon::ScatterStyle::Peace:
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
