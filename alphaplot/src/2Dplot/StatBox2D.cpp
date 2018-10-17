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
      whiskerstyle_(StatBox2D::BoxWhiskerStyle::Perc_5_95),
      picker_(Graph2DCommon::Picker::None) {
  layer()->setMode(QCPLayer::LayerMode::lmBuffered);
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

Graph2DCommon::ScatterStyle StatBox2D::getscattershape_statbox() const {
  Graph2DCommon::ScatterStyle scatterstyle;
  switch (outlierStyle().shape()) {
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

void StatBox2D::setscattershape_statbox(
    const Graph2DCommon::ScatterStyle &shape) {
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

void StatBox2D::setpicker_statbox(const Graph2DCommon::Picker picker) {
  picker_ = picker;
}

void StatBox2D::mousePressEvent(QMouseEvent *event, const QVariant &details) {
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
  QCPStatisticalBox::mousePressEvent(event, details);
}

void StatBox2D::datapicker(QMouseEvent *event, const QVariant &details) {
  QCPStatisticalBoxDataContainer::const_iterator it = data()->constEnd();
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

void StatBox2D::graphpicker(QMouseEvent *event, const QVariant &details) {
  double xvalue, yvalue;
  pixelsToCoords(event->localPos(), xvalue, yvalue);
  emit showtooltip(event->localPos(), xvalue, yvalue);
}

void StatBox2D::movepicker(QMouseEvent *event, const QVariant &details) {}

void StatBox2D::removepicker(QMouseEvent *event, const QVariant &details) {}
