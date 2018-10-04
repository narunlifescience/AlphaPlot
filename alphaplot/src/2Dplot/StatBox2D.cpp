#include "StatBox2D.h"
#include "Table.h"
#include "core/Utilities.h"

StatBox2D::StatBox2D(Axis2D *xAxis, Axis2D *yAxis, Table *table,
                     BoxWhiskerData boxWhiskerData)
    : QCPStatisticalBox(xAxis, yAxis),
      xAxis_(xAxis),
      yAxis_(yAxis),
      boxwhiskerdata_(boxWhiskerData),
      scatterstyle_(new QCPScatterStyle(
          QCPScatterStyle::ssDisc,
          Utilities::getRandColorGoldenRatio(Utilities::ColorPal::Dark),
          Utilities::getRandColorGoldenRatio(Utilities::ColorPal::Dark), 6.0)),
      boxstyle_(StatBox2D::BoxWhiskerStyle::Perc_25_75),
      whiskerstyle_(StatBox2D::BoxWhiskerStyle::Perc_5_95) {
  setWhiskerAntialiased(false);
  setAntialiased(false);
  setOutlierStyle(*scatterstyle_);
  QBrush b = brush();
  b.setColor(Utilities::getRandColorGoldenRatio(Utilities::ColorPal::Light));
  setBrush(b);
  sBoxdata_.key = boxWhiskerData.key;
  sBoxdata_.median = boxWhiskerData.median;
  setboxstyle_statbox(Perc_25_75);
  setwhiskerstyle_statbox(Perc_5_95);
  sBoxdata_.outliers << boxWhiskerData.boxWhiskerDataBounds.min
                     << boxWhiskerData.boxWhiskerDataBounds.max;
  data().data()->clear();
  addData(sBoxdata_.key, sBoxdata_.minimum, sBoxdata_.lowerQuartile,
          sBoxdata_.median, sBoxdata_.upperQuartile, sBoxdata_.maximum,
          sBoxdata_.outliers);
}

StatBox2D::~StatBox2D() { delete scatterstyle_; }

Axis2D *StatBox2D::getxaxis_statbox() const { return xAxis_; }

Axis2D *StatBox2D::getyaxis_statbox() const { return yAxis_; }

StatBox2D::BoxWhiskerStyle StatBox2D::getboxstyle_statbox() const {
  return boxstyle_;
}

StatBox2D::BoxWhiskerStyle StatBox2D::getwhiskerstyle_statbox() const {
  return whiskerstyle_;
}

QColor StatBox2D::getfillcolor_statbox() const { return brush().color(); }

bool StatBox2D::getfillstatus_statbox() const {
  if (brush().style() == Qt::NoBrush) {
    return false;
  } else {
    return true;
  }
}

Qt::PenStyle StatBox2D::getwhiskerstrokestyle_statbox() const {
  return whiskerPen().style();
}

QColor StatBox2D::getwhiskerstrokecolor_statbox() const {
  return whiskerPen().color();
}

double StatBox2D::getwhiskerstrokethickness_statbox() const {
  return whiskerPen().widthF();
}

Qt::PenStyle StatBox2D::getwhiskerbarstrokestyle_statbox() const {
  return whiskerBarPen().style();
}

QColor StatBox2D::getwhiskerbarstrokecolor_statbox() const {
  return whiskerBarPen().color();
}

double StatBox2D::getwhiskerbarstrokethickness_statbox() const {
  return whiskerBarPen().widthF();
}

Qt::PenStyle StatBox2D::getmedianstrokestyle_statbox() const {
  return medianPen().style();
}

QColor StatBox2D::getmedianstrokecolor_statbox() const {
  return medianPen().color();
}

double StatBox2D::getmedianstrokethickness_statbox() const {
  return medianPen().widthF();
}

LSCommon::ScatterStyle StatBox2D::getscattershape_statbox() const {
  LSCommon::ScatterStyle scatterstyle;
  switch (outlierStyle().shape()) {
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

QColor StatBox2D::getscatterfillcolor_statbox() const {
  return outlierStyle().brush().color();
}

double StatBox2D::getscattersize_statbox() const {
  return outlierStyle().size();
}

Qt::PenStyle StatBox2D::getscatterstrokestyle_statbox() const {
  return outlierStyle().pen().style();
}

QColor StatBox2D::getscatterstrokecolor_statbox() const {
  return outlierStyle().pen().color();
}

double StatBox2D::getscatterstrokethickness_statbox() const {
  return outlierStyle().pen().widthF();
}

void StatBox2D::setxaxis_statbox(Axis2D *axis) {
  Q_ASSERT(axis->getorientation_axis() == Axis2D::AxisOreantation::Bottom ||
           axis->getorientation_axis() == Axis2D::AxisOreantation::Top);
  if (axis == getxaxis_statbox()) return;

  xAxis_ = axis;
  setKeyAxis(axis);
}

void StatBox2D::setyaxis_statbox(Axis2D *axis) {
  Q_ASSERT(axis->getorientation_axis() == Axis2D::AxisOreantation::Left ||
           axis->getorientation_axis() == Axis2D::AxisOreantation::Right);
  if (axis == getyaxis_statbox()) return;

  yAxis_ = axis;
  setValueAxis(axis);
}

void StatBox2D::setboxwhiskerdata(const BoxWhiskerData boxWhiskerData) {
  sBoxdata_.key = boxWhiskerData.key;
  sBoxdata_.median = boxWhiskerData.median;
  setboxstyle_statbox(Perc_25_75);
  setwhiskerstyle_statbox(Perc_5_95);
  sBoxdata_.outliers << boxWhiskerData.boxWhiskerDataBounds.min
                     << boxWhiskerData.boxWhiskerDataBounds.max;
  addData(sBoxdata_.key, sBoxdata_.minimum, sBoxdata_.lowerQuartile,
          sBoxdata_.median, sBoxdata_.upperQuartile, sBoxdata_.maximum,
          sBoxdata_.outliers);
}

void StatBox2D::setboxstyle_statbox(
    const StatBox2D::BoxWhiskerStyle &boxStyle) {
  switch (boxStyle) {
    case SD:
      sBoxdata_.lowerQuartile = boxwhiskerdata_.boxWhiskerDataBounds.sd_lower;
      sBoxdata_.upperQuartile = boxwhiskerdata_.boxWhiskerDataBounds.sd_upper;
      break;
    case SE:
      sBoxdata_.lowerQuartile = boxwhiskerdata_.boxWhiskerDataBounds.se_lower;
      sBoxdata_.upperQuartile = boxwhiskerdata_.boxWhiskerDataBounds.se_upper;
      break;
    case Perc_25_75:
      sBoxdata_.lowerQuartile = boxwhiskerdata_.boxWhiskerDataBounds.perc_25;
      sBoxdata_.upperQuartile = boxwhiskerdata_.boxWhiskerDataBounds.perc_75;
      break;
    case Perc_10_90:
      sBoxdata_.lowerQuartile = boxwhiskerdata_.boxWhiskerDataBounds.perc_10;
      sBoxdata_.upperQuartile = boxwhiskerdata_.boxWhiskerDataBounds.perc_90;
      break;
    case Perc_5_95:
      sBoxdata_.lowerQuartile = boxwhiskerdata_.boxWhiskerDataBounds.perc_5;
      sBoxdata_.upperQuartile = boxwhiskerdata_.boxWhiskerDataBounds.perc_95;
      break;
    case Perc_1_99:
      sBoxdata_.lowerQuartile = boxwhiskerdata_.boxWhiskerDataBounds.perc_1;
      sBoxdata_.upperQuartile = boxwhiskerdata_.boxWhiskerDataBounds.perc_99;
      break;
    case MinMax:
      sBoxdata_.lowerQuartile = boxwhiskerdata_.boxWhiskerDataBounds.min;
      sBoxdata_.upperQuartile = boxwhiskerdata_.boxWhiskerDataBounds.max;
      break;
    case Constant:
      sBoxdata_.lowerQuartile =
          boxwhiskerdata_.boxWhiskerDataBounds.constant_lower;
      sBoxdata_.upperQuartile =
          boxwhiskerdata_.boxWhiskerDataBounds.constant_upper;
      break;
  }
  data().data()->clear();
  addData(sBoxdata_.key, sBoxdata_.minimum, sBoxdata_.lowerQuartile,
          sBoxdata_.median, sBoxdata_.upperQuartile, sBoxdata_.maximum,
          sBoxdata_.outliers);
  boxstyle_ = boxStyle;
}

void StatBox2D::setwhiskerstyle_statbox(
    const StatBox2D::BoxWhiskerStyle &whiskerStyle) {
  switch (whiskerStyle) {
    case SD:
      sBoxdata_.minimum = boxwhiskerdata_.boxWhiskerDataBounds.sd_lower;
      sBoxdata_.maximum = boxwhiskerdata_.boxWhiskerDataBounds.sd_upper;
      break;
    case SE:
      sBoxdata_.minimum = boxwhiskerdata_.boxWhiskerDataBounds.se_lower;
      sBoxdata_.maximum = boxwhiskerdata_.boxWhiskerDataBounds.se_upper;
      break;
    case Perc_25_75:
      sBoxdata_.minimum = boxwhiskerdata_.boxWhiskerDataBounds.perc_25;
      sBoxdata_.maximum = boxwhiskerdata_.boxWhiskerDataBounds.perc_75;
      break;
    case Perc_10_90:
      sBoxdata_.minimum = boxwhiskerdata_.boxWhiskerDataBounds.perc_10;
      sBoxdata_.maximum = boxwhiskerdata_.boxWhiskerDataBounds.perc_90;
      break;
    case Perc_5_95:
      sBoxdata_.minimum = boxwhiskerdata_.boxWhiskerDataBounds.perc_5;
      sBoxdata_.maximum = boxwhiskerdata_.boxWhiskerDataBounds.perc_95;
      break;
    case Perc_1_99:
      sBoxdata_.minimum = boxwhiskerdata_.boxWhiskerDataBounds.perc_1;
      sBoxdata_.maximum = boxwhiskerdata_.boxWhiskerDataBounds.perc_99;
      break;
    case MinMax:
      sBoxdata_.minimum = boxwhiskerdata_.boxWhiskerDataBounds.min;
      sBoxdata_.maximum = boxwhiskerdata_.boxWhiskerDataBounds.max;
      break;
    case Constant:
      sBoxdata_.minimum = boxwhiskerdata_.boxWhiskerDataBounds.constant_lower;
      sBoxdata_.maximum = boxwhiskerdata_.boxWhiskerDataBounds.constant_upper;
      break;
  }
  data().data()->clear();
  addData(sBoxdata_.key, sBoxdata_.minimum, sBoxdata_.lowerQuartile,
          sBoxdata_.median, sBoxdata_.upperQuartile, sBoxdata_.maximum,
          sBoxdata_.outliers);
  whiskerstyle_ = whiskerStyle;
}

void StatBox2D::setfillcolor_statbox(const QColor &color) {
  QBrush b = brush();
  b.setColor(color);
  setBrush(b);
}

void StatBox2D::setfillstatus_statbox(const bool status) {
  if (status) {
    QBrush b = brush();
    b.setStyle(Qt::SolidPattern);
    setBrush(b);
  } else {
    QBrush b = brush();
    b.setStyle(Qt::NoBrush);
    setBrush(b);
  }
}

void StatBox2D::setwhiskerstrokestyle_statbox(const Qt::PenStyle &style) {
  QPen p = whiskerPen();
  p.setStyle(style);
  setWhiskerPen(p);
}

void StatBox2D::setwhiskerstrokecolor_statbox(const QColor &color) {
  QPen p = whiskerPen();
  p.setColor(color);
  setWhiskerPen(p);
}

void StatBox2D::setwhiskerstrokethickness_statbox(const double value) {
  QPen p = whiskerPen();
  p.setWidthF(value);
  setWhiskerPen(p);
}

void StatBox2D::setwhiskerbarstrokestyle_statbox(const Qt::PenStyle &style) {
  QPen p = whiskerBarPen();
  p.setStyle(style);
  setWhiskerBarPen(p);
}

void StatBox2D::setwhiskerbarstrokecolor_statbox(const QColor &color) {
  QPen p = whiskerBarPen();
  p.setColor(color);
  setWhiskerBarPen(p);
}

void StatBox2D::setwhiskerbarstrokethickness_statbox(const double value) {
  QPen p = whiskerBarPen();
  p.setWidthF(value);
  setWhiskerBarPen(p);
}

void StatBox2D::setmedianstrokestyle_statbox(const Qt::PenStyle &style) {
  QPen p = medianPen();
  p.setStyle(style);
  setMedianPen(p);
}

void StatBox2D::setmedianstrokecolor_statbox(const QColor &color) {
  QPen p = medianPen();
  p.setColor(color);
  setMedianPen(p);
}

void StatBox2D::setmedianstrokethickness_statbox(const double value) {
  QPen p = medianPen();
  p.setWidthF(value);
  setMedianPen(p);
}

void StatBox2D::setscattershape_statbox(const LSCommon::ScatterStyle &shape) {
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
  setOutlierStyle(*scatterstyle_);
}

void StatBox2D::setscatterfillcolor_statbox(const QColor &color) {
  QBrush b = scatterstyle_->brush();
  b.setColor(color);
  scatterstyle_->setBrush(b);
  setOutlierStyle(*scatterstyle_);
}

void StatBox2D::setscattersize_statbox(const double value) {
  scatterstyle_->setSize(value);
  setOutlierStyle(*scatterstyle_);
}

void StatBox2D::setscatterstrokestyle_statbox(const Qt::PenStyle &style) {
  QPen p = scatterstyle_->pen();
  p.setStyle(style);
  scatterstyle_->setPen(p);
  setOutlierStyle(*scatterstyle_);
}

void StatBox2D::setscatterstrokecolor_statbox(const QColor &color) {
  QPen p = scatterstyle_->pen();
  p.setColor(color);
  scatterstyle_->setPen(p);
  setOutlierStyle(*scatterstyle_);
}

void StatBox2D::setscatterstrokethickness_statbox(const double value) {
  QPen p = scatterstyle_->pen();
  p.setWidthF(value);
  scatterstyle_->setPen(p);
  setOutlierStyle(*scatterstyle_);
}
