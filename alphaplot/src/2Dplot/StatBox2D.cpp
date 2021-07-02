#include "StatBox2D.h"

#include <gsl/gsl_sort.h>
#include <gsl/gsl_statistics.h>

#include "Axis2D.h"
#include "PickerTool2D.h"
#include "Table.h"
#include "core/IconLoader.h"
#include "core/Utilities.h"
#include "future/core/column/Column.h"
#include "future/lib/XmlStreamReader.h"
#include "future/lib/XmlStreamWriter.h"

StatBox2D::StatBox2D(BoxWhiskerData boxWhiskerData, Axis2D *xAxis,
                     Axis2D *yAxis)
    : QCPStatisticalBox(xAxis, yAxis),
      xAxis_(xAxis),
      yAxis_(yAxis),
      layername_(QString("<StatBox2D>") + QDateTime::currentDateTime().toString(
                                              "yyyy:MM:dd:hh:mm:ss:zzz")),
      boxwhiskerdata_(boxWhiskerData),
      scatterstyle_(new QCPScatterStyle(
          QCPScatterStyle::ssDisc,
          Utilities::getRandColorGoldenRatio(Utilities::ColorPal::Dark),
          Utilities::getRandColorGoldenRatio(Utilities::ColorPal::Dark), 6.0)),
      boxstyle_(StatBox2D::BoxWhiskerStyle::Perc_25_75),
      whiskerstyle_(StatBox2D::BoxWhiskerStyle::Perc_5_95) {
  // setting icon
  icon_ = IconLoader::load("graph2d-box", IconLoader::LightDark);

  QThread::msleep(1);
  parentPlot()->addLayer(layername_, xAxis_->layer(), QCustomPlot::limBelow);
  setLayer(layername_);
  layer()->setMode(QCPLayer::LayerMode::lmBuffered);
  setWhiskerAntialiased(false);
  setAntialiasedFill(false);
  setAntialiased(false);
  setOutlierStyle(*scatterstyle_);
  setfillcolor_statbox(
      Utilities::getRandColorGoldenRatio(Utilities::ColorPal::Light));
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

StatBox2D::~StatBox2D() {
  delete scatterstyle_;
  parentPlot()->removeLayer(layer());
}

Axis2D *StatBox2D::getxaxis() const { return xAxis_; }

Axis2D *StatBox2D::getyaxis() const { return yAxis_; }

StatBox2D::BoxWhiskerStyle StatBox2D::getboxstyle_statbox() const {
  return boxstyle_;
}

StatBox2D::BoxWhiskerStyle StatBox2D::getwhiskerstyle_statbox() const {
  return whiskerstyle_;
}

QColor StatBox2D::getfillcolor_statbox() const { return brush().color(); }

Qt::BrushStyle StatBox2D::getfillstyle_statbox() const {
  return brush().style();
}

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
  if (axis == getxaxis()) return;

  xAxis_ = axis;
  setKeyAxis(axis);
}

void StatBox2D::setyaxis_statbox(Axis2D *axis) {
  Q_ASSERT(axis->getorientation_axis() == Axis2D::AxisOreantation::Left ||
           axis->getorientation_axis() == Axis2D::AxisOreantation::Right);
  if (axis == getyaxis()) return;

  yAxis_ = axis;
  setValueAxis(axis);
}

void StatBox2D::setboxwhiskerdata(const BoxWhiskerData boxWhiskerData) {
  boxwhiskerdata_ = boxWhiskerData;
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

void StatBox2D::setfillstyle_statbox(const Qt::BrushStyle &style) {
  QBrush b = brush();
  if (b.style() != Qt::BrushStyle::NoBrush) {
    b.setStyle(style);
    setBrush(b);
  }
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

void StatBox2D::setlegendtext_statbox(const QString name) {
  QSharedPointer<QCPAxisTickerText> textTicker =
      qSharedPointerCast<QCPAxisTickerText>(getxaxis()->getticker_axis());
  boxwhiskerdata_.name = name;
  textTicker->addTick(boxwhiskerdata_.key, boxwhiskerdata_.name);
  getxaxis()->setTicker(textTicker);
  setName(boxwhiskerdata_.name);
}

void StatBox2D::save(XmlStreamWriter *xmlwriter, int xaxis, int yaxis) {
  xmlwriter->writeStartElement("statbox");
  // axis
  xmlwriter->writeAttribute("xaxis", QString::number(xaxis));
  xmlwriter->writeAttribute("yaxis", QString::number(yaxis));
  xmlwriter->writeAttribute("legend", name());
  // data
  xmlwriter->writeAttribute("table", boxwhiskerdata_.table_->name());
  xmlwriter->writeAttribute("column", boxwhiskerdata_.column_->name());
  xmlwriter->writeAttribute("from", QString::number(boxwhiskerdata_.from_));
  xmlwriter->writeAttribute("to", QString::number(boxwhiskerdata_.to_));
  xmlwriter->writeAttribute("key", QString::number(boxwhiskerdata_.key));

  // box
  xmlwriter->writeStartElement("box");
  (getfillstatus_statbox()) ? xmlwriter->writeAttribute("fill", "true")
                            : xmlwriter->writeAttribute("fill", "true");
  (antialiased()) ? xmlwriter->writeAttribute("antialias", "true")
                  : xmlwriter->writeAttribute("antialias", "false");
  (antialiasedFill()) ? xmlwriter->writeAttribute("antialiasfill", "true")
                      : xmlwriter->writeAttribute("antialiasfill", "false");
  xmlwriter->writeAttribute("width", QString::number(width()));

  switch (getboxstyle_statbox()) {
    case StatBox2D::BoxWhiskerStyle::SE:
      xmlwriter->writeAttribute("boxstyle", "se");
      break;
    case StatBox2D::BoxWhiskerStyle::SD:
      xmlwriter->writeAttribute("boxstyle", "sd");
      break;
    case StatBox2D::BoxWhiskerStyle::MinMax:
      xmlwriter->writeAttribute("boxstyle", "minmax");
      break;
    case StatBox2D::BoxWhiskerStyle::Constant:
      xmlwriter->writeAttribute("boxstyle", "constant");
      break;
    case StatBox2D::BoxWhiskerStyle::Perc_1_99:
      xmlwriter->writeAttribute("boxstyle", "perc_1_99");
      break;
    case StatBox2D::BoxWhiskerStyle::Perc_5_95:
      xmlwriter->writeAttribute("boxstyle", "perc_5_95");
      break;
    case StatBox2D::BoxWhiskerStyle::Perc_10_90:
      xmlwriter->writeAttribute("boxstyle", "perc_10_90");
      break;
    case StatBox2D::BoxWhiskerStyle::Perc_25_75:
      xmlwriter->writeAttribute("boxstyle", "perc_25_75");
      break;
  }
  xmlwriter->writePen(pen());
  xmlwriter->writeBrush(brush());
  xmlwriter->writeEndElement();
  // median
  xmlwriter->writeStartElement("median");
  xmlwriter->writePen(medianPen());
  xmlwriter->writeEndElement();
  // whisker
  xmlwriter->writeStartElement("whisker");
  (whiskerAntialiased()) ? xmlwriter->writeAttribute("antialias", "true")
                         : xmlwriter->writeAttribute("antialias", "false");
  xmlwriter->writeAttribute("width", QString::number(whiskerWidth()));
  xmlwriter->writePen(whiskerPen());
  xmlwriter->writeEndElement();
  // whisker bar
  xmlwriter->writeStartElement("whiskerbar");
  xmlwriter->writePen(whiskerBarPen());
  xmlwriter->writeEndElement();
  // scatter
  xmlwriter->writeStartElement("scatter");
  switch (getscattershape_statbox()) {
    case Graph2DCommon::ScatterStyle::None:
      xmlwriter->writeAttribute("style", "none");
      break;
    case Graph2DCommon::ScatterStyle::Dot:
      xmlwriter->writeAttribute("style", "dot");
      break;
    case Graph2DCommon::ScatterStyle::Disc:
      xmlwriter->writeAttribute("style", "disc");
      break;
    case Graph2DCommon::ScatterStyle::Plus:
      xmlwriter->writeAttribute("style", "plus");
      break;
    case Graph2DCommon::ScatterStyle::Star:
      xmlwriter->writeAttribute("style", "star");
      break;
    case Graph2DCommon::ScatterStyle::Cross:
      xmlwriter->writeAttribute("style", "cross");
      break;
    case Graph2DCommon::ScatterStyle::Peace:
      xmlwriter->writeAttribute("style", "peace");
      break;
    case Graph2DCommon::ScatterStyle::Circle:
      xmlwriter->writeAttribute("style", "circle");
      break;
    case Graph2DCommon::ScatterStyle::Square:
      xmlwriter->writeAttribute("style", "square");
      break;
    case Graph2DCommon::ScatterStyle::Diamond:
      xmlwriter->writeAttribute("style", "diamond");
      break;
    case Graph2DCommon::ScatterStyle::Triangle:
      xmlwriter->writeAttribute("style", "triangle");
      break;
    case Graph2DCommon::ScatterStyle::PlusCircle:
      xmlwriter->writeAttribute("style", "pluscircle");
      break;
    case Graph2DCommon::ScatterStyle::PlusSquare:
      xmlwriter->writeAttribute("style", "plussquare");
      break;
    case Graph2DCommon::ScatterStyle::CrossCircle:
      xmlwriter->writeAttribute("style", "crosscircle");
      break;
    case Graph2DCommon::ScatterStyle::CrossSquare:
      xmlwriter->writeAttribute("style", "crosssquare");
      break;
    case Graph2DCommon::ScatterStyle::TriangleInverted:
      xmlwriter->writeAttribute("style", "triangleinverted");
      break;
  }
  xmlwriter->writeAttribute("size", QString::number(scatterstyle_->size()));
  (antialiasedScatters()) ? xmlwriter->writeAttribute("antialias", "true")
                          : xmlwriter->writeAttribute("antialias", "false");
  xmlwriter->writePen(scatterstyle_->pen());
  xmlwriter->writeBrush(scatterstyle_->brush());
  xmlwriter->writeEndElement();
  xmlwriter->writeEndElement();
}

bool StatBox2D::load(XmlStreamReader *xmlreader) {
  bool ok;
  while (!xmlreader->atEnd()) {
    if (xmlreader->isEndElement() && xmlreader->name() == "statbox") break;

    // box
    if (xmlreader->isStartElement() && xmlreader->name() == "box") {
      // box fill status
      bool fill = xmlreader->readAttributeBool("fill", &ok);
      (ok) ? setfillstatus_statbox(fill)
           : xmlreader->raiseWarning(
                 tr("StatBox2D box fill status property setting error"));

      // box antialias
      bool boxantialias = xmlreader->readAttributeBool("antialias", &ok);
      (ok) ? setAntialiased(boxantialias)
           : xmlreader->raiseWarning(
                 tr("StatBox2D box antialias property setting error"));

      // box antialias fill
      bool boxantialiasfill =
          xmlreader->readAttributeBool("antialiasfill", &ok);
      (ok) ? setAntialiasedFill(boxantialiasfill)
           : xmlreader->raiseWarning(
                 tr("StatBox2D box antialias propfill erty setting error"));

      // box width
      double boxwidth = xmlreader->readAttributeDouble("width", &ok);
      if (ok) {
        setWidth(boxwidth);
      } else
        xmlreader->raiseWarning(
            tr("StatBox2D box width property setting error"));

      // box style
      QString boxstyle = xmlreader->readAttributeString("boxstyle", &ok);
      if (ok) {
        if (boxstyle == "sd") {
          setboxstyle_statbox(StatBox2D::BoxWhiskerStyle::SD);
        } else if (boxstyle == "se") {
          setboxstyle_statbox(StatBox2D::BoxWhiskerStyle::SE);
        } else if (boxstyle == "minmax") {
          setboxstyle_statbox(StatBox2D::BoxWhiskerStyle::MinMax);
        } else if (boxstyle == "constant") {
          setboxstyle_statbox(StatBox2D::BoxWhiskerStyle::Constant);
        } else if (boxstyle == "perc_1_99") {
          setboxstyle_statbox(StatBox2D::BoxWhiskerStyle::Perc_1_99);
        } else if (boxstyle == "perc_5_95") {
          setboxstyle_statbox(StatBox2D::BoxWhiskerStyle::Perc_5_95);
        } else if (boxstyle == "perc_10_90") {
          setboxstyle_statbox(StatBox2D::BoxWhiskerStyle::Perc_10_90);
        } else if (boxstyle == "perc_25_75") {
          setboxstyle_statbox(StatBox2D::BoxWhiskerStyle::Perc_25_75);
        }
      } else
        xmlreader->raiseWarning(
            tr("StatBox2D box style property setting error"));

      // box pen property
      while (!xmlreader->atEnd()) {
        xmlreader->readNext();
        if (xmlreader->isEndElement() && xmlreader->name() == "pen") break;
        // pen
        if (xmlreader->isStartElement() && xmlreader->name() == "pen") {
          QPen boxpen = xmlreader->readPen(&ok);
          (ok) ? setPen(boxpen)
               : xmlreader->raiseWarning(
                     tr("StatBox2D box pen property setting error"));
        }
      }

      // box brush property
      while (!xmlreader->atEnd()) {
        xmlreader->readNext();
        if (xmlreader->isEndElement() && xmlreader->name() == "brush") break;
        // brush
        if (xmlreader->isStartElement() && xmlreader->name() == "brush") {
          QBrush boxbrush = xmlreader->readBrush(&ok);
          (ok) ? setBrush(boxbrush)
               : xmlreader->raiseWarning(
                     tr("Curve2D linebrush property setting error"));
        }
      }
    }

    // median
    if (xmlreader->isStartElement() && xmlreader->name() == "median") {
      // box pen property
      while (!xmlreader->atEnd()) {
        xmlreader->readNext();
        if (xmlreader->isEndElement() && xmlreader->name() == "pen") break;
        // pen
        if (xmlreader->isStartElement() && xmlreader->name() == "pen") {
          QPen mpen = xmlreader->readPen(&ok);
          (ok) ? setMedianPen(mpen)
               : xmlreader->raiseWarning(
                     tr("StatBox2D median pen property setting error"));
        }
      }
    }

    // whiskerbar
    if (xmlreader->isStartElement() && xmlreader->name() == "whisker") {
      // whisker antialias
      bool boxantialias = xmlreader->readAttributeBool("antialias", &ok);
      (ok) ? setWhiskerAntialiased(boxantialias)
           : xmlreader->raiseWarning(
                 tr("StatBox2D whisker antialias property setting error"));

      // whisker width
      double whiskerwidth = xmlreader->readAttributeDouble("width", &ok);
      (ok) ? setWhiskerWidth(whiskerwidth)
           : xmlreader->raiseWarning(
                 tr("StatBox2D whisker width property setting error"));

      // pen property
      while (!xmlreader->atEnd()) {
        xmlreader->readNext();
        if (xmlreader->isEndElement() && xmlreader->name() == "pen") break;
        // pen
        if (xmlreader->isStartElement() && xmlreader->name() == "pen") {
          QPen wpen = xmlreader->readPen(&ok);
          (ok) ? setWhiskerPen(wpen)
               : xmlreader->raiseWarning(
                     tr("StatBox2D whisker pen property setting error"));
        }
      }
    }

    // whiskerbar
    if (xmlreader->isStartElement() && xmlreader->name() == "whiskerbar") {
      // pen property
      while (!xmlreader->atEnd()) {
        xmlreader->readNext();
        if (xmlreader->isEndElement() && xmlreader->name() == "pen") break;
        // pen
        if (xmlreader->isStartElement() && xmlreader->name() == "pen") {
          QPen wbpen = xmlreader->readPen(&ok);
          (ok) ? setWhiskerBarPen(wbpen)
               : xmlreader->raiseWarning(
                     tr("StatBox2D whiskerbar pen property setting error"));
        }
      }
    }

    // scatter
    if (xmlreader->isStartElement() && xmlreader->name() == "scatter") {
      // scatter shape
      QString scattershape = xmlreader->readAttributeString("style", &ok);
      if (ok) {
        if (scattershape == "dot") {
          setscattershape_statbox(Graph2DCommon::ScatterStyle::Dot);
        } else if (scattershape == "disc") {
          setscattershape_statbox(Graph2DCommon::ScatterStyle::Disc);
        } else if (scattershape == "none") {
          setscattershape_statbox(Graph2DCommon::ScatterStyle::None);
        } else if (scattershape == "plus") {
          setscattershape_statbox(Graph2DCommon::ScatterStyle::Plus);
        } else if (scattershape == "star") {
          setscattershape_statbox(Graph2DCommon::ScatterStyle::Star);
        } else if (scattershape == "cross") {
          setscattershape_statbox(Graph2DCommon::ScatterStyle::Cross);
        } else if (scattershape == "peace") {
          setscattershape_statbox(Graph2DCommon::ScatterStyle::Peace);
        } else if (scattershape == "circle") {
          setscattershape_statbox(Graph2DCommon::ScatterStyle::Circle);
        } else if (scattershape == "square") {
          setscattershape_statbox(Graph2DCommon::ScatterStyle::Square);
        } else if (scattershape == "diamond") {
          setscattershape_statbox(Graph2DCommon::ScatterStyle::Diamond);
        } else if (scattershape == "triangle") {
          setscattershape_statbox(Graph2DCommon::ScatterStyle::Triangle);
        } else if (scattershape == "pluscircle") {
          setscattershape_statbox(Graph2DCommon::ScatterStyle::PlusCircle);
        } else if (scattershape == "plussquare") {
          setscattershape_statbox(Graph2DCommon::ScatterStyle::PlusSquare);
        } else if (scattershape == "crosscircle") {
          setscattershape_statbox(Graph2DCommon::ScatterStyle::CrossCircle);
        } else if (scattershape == "crosssquare") {
          setscattershape_statbox(Graph2DCommon::ScatterStyle::CrossSquare);
        } else if (scattershape == "triangleinverted") {
          setscattershape_statbox(
              Graph2DCommon::ScatterStyle::TriangleInverted);
        }
      } else
        xmlreader->raiseWarning(
            tr("StatBox2D scatter shape property setting error"));

      // scatter size
      int scattersize = xmlreader->readAttributeInt("size", &ok);
      (ok) ? setscattersize_statbox(scattersize)
           : xmlreader->raiseWarning(
                 tr("Scatter2D scatter size property setting error"));

      // scatter antialias
      bool scatterantialias = xmlreader->readAttributeBool("antialias", &ok);
      (ok) ? setAntialiasedScatters(scatterantialias)
           : xmlreader->raiseWarning(
                 tr("ScatterBox2D scatter antialias property setting error"));

      // scatter pen property
      while (!xmlreader->atEnd()) {
        xmlreader->readNext();
        if (xmlreader->isEndElement() && xmlreader->name() == "pen") break;
        // pen
        if (xmlreader->isStartElement() && xmlreader->name() == "pen") {
          QPen strokep = xmlreader->readPen(&ok);
          if (ok) {
            setscatterstrokecolor_statbox(strokep.color());
            setscatterstrokestyle_statbox(strokep.style());
            setscatterstrokethickness_statbox(strokep.widthF());
          } else
            xmlreader->raiseWarning(
                tr("ScatterBox2D scatter pen property setting error"));
        }
      }
    }
    xmlreader->readNext();
  }
  return !xmlreader->hasError();
}

void StatBox2D::mousePressEvent(QMouseEvent *event, const QVariant &details) {
  if (event->button() == Qt::LeftButton) {
    switch (xAxis_->getaxisrect_axis()->getPickerTool()->getPicker()) {
      case Graph2DCommon::Picker::None:
      case Graph2DCommon::Picker::DataGraph:
      case Graph2DCommon::Picker::DragRange:
      case Graph2DCommon::Picker::ZoomRange:
      case Graph2DCommon::Picker::DataRange:
        break;
      case Graph2DCommon::Picker::DataPoint:
        datapicker(event, details);
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
      xAxis_->getaxisrect_axis()->getPickerTool()->showtooltip(
          point, it->mainKey(), it->mainValue(), getxaxis(), getyaxis());
    }
  }
}

void StatBox2D::movepicker(QMouseEvent *, const QVariant &) {
  qDebug() << "move picker unavailable for StatBox2D plots";
}

void StatBox2D::removepicker(QMouseEvent *, const QVariant &) {
  qDebug() << "remove picker unavailable for StatBox2D plots";
}
