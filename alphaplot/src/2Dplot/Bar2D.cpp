#include "Bar2D.h"

#include "AxisRect2D.h"
#include "DataManager2D.h"
#include "ErrorBar2D.h"
#include "PickerTool2D.h"
#include "Table.h"
#include "core/IconLoader.h"
#include "core/Utilities.h"
#include "future/core/column/Column.h"
#include "future/lib/XmlStreamReader.h"
#include "future/lib/XmlStreamWriter.h"

Bar2D::Bar2D(Table *table, Column *xcol, Column *ycol, int from, int to,
             Axis2D *xAxis, Axis2D *yAxis, const BarStyle &style,
             int stackposition)
    : QCPBars(xAxis, yAxis),
      barwidth_(1),
      xaxis_(xAxis),
      yaxis_(yAxis),
      ishistogram_(false),
      style_(style),
      group_(nullptr),
      bardata_(new DataBlockBar(table, xcol, ycol, from, to)),
      histdata_(nullptr),
      xerrorbar_(nullptr),
      yerrorbar_(nullptr),
      xerroravailable_(false),
      yerroravailable_(false),
      stackposition_(stackposition) {
  reloadIcon();
  init();
  setSelectable(QCP::SelectionType::stSingleData);
  QColor color = Utilities::getRandColorGoldenRatio(Utilities::ColorPal::Dark);
  setstrokecolor_barplot(color);
  color.setAlpha(100);
  setfillcolor_barplot(color);
  setData(bardata_->data());
  if (bardata_ && bardata_->data()->size() > 1)
    setWidth(bardata_->data()->at(1)->mainKey() -
             bardata_->data()->at(0)->mainKey());
}

Bar2D::Bar2D(Table *table, Column *col, int from, int to, Axis2D *xAxis,
             Axis2D *yAxis)
    : QCPBars(xAxis, yAxis),
      barwidth_(1),
      xaxis_(xAxis),
      yaxis_(yAxis),
      ishistogram_(true),
      style_(BarStyle::Individual),
      group_(nullptr),
      histdata_(new DataBlockHist(table, col, from, to)),
      xerrorbar_(nullptr),
      yerrorbar_(nullptr),
      xerroravailable_(false),
      yerroravailable_(false),
      stackposition_(-1) {
  reloadIcon();
  init();
  setSelectable(QCP::SelectionType::stSingleData);
  QColor color = Utilities::getRandColorGoldenRatio(Utilities::ColorPal::Dark);
  setstrokecolor_barplot(color);
  color.setAlpha(100);
  setfillcolor_barplot(color);
  setBarData(table, col, from, to);
}

void Bar2D::init() {
  layername_ = QString("<Bar2D>") +
               QDateTime::currentDateTime().toString("yyyy:MM:dd:hh:mm:ss:zzz");
  QThread::msleep(1);
  parentPlot()->addLayer(layername_, xaxis_->layer(), QCustomPlot::limBelow);
  setLayer(layername_);
  layer()->setMode(QCPLayer::LayerMode::lmBuffered);
}

Bar2D::~Bar2D() {
  if (!ishistogram_) delete bardata_;
  parentPlot()->removeLayer(layer());
}

void Bar2D::setXerrorBar(Table *table, Column *errorcol, int from, int to) {
  if (xerroravailable_ || ishistogram_) {
    qDebug() << "X error bar already defined or unsupported plot type";
    return;
  }
  xerrorbar_ = new ErrorBar2D(table, errorcol, from, to, xaxis_, yaxis_,
                              QCPErrorBars::ErrorType::etKeyError, this);
  parentPlot()->moveLayer(layer(), xerrorbar_->layer(),
                          QCustomPlot::LayerInsertMode::limAbove);
  xerroravailable_ = true;
  emit xaxis_->getaxisrect_axis()->ErrorBar2DCreated(xerrorbar_);
}

void Bar2D::setYerrorBar(Table *table, Column *errorcol, int from, int to) {
  if (yerroravailable_ || ishistogram_) {
    qDebug() << "Y error bar already defined or unsupported plot type";
    return;
  }
  yerrorbar_ = new ErrorBar2D(table, errorcol, from, to, xaxis_, yaxis_,
                              QCPErrorBars::ErrorType::etValueError, this);
  parentPlot()->moveLayer(layer(), yerrorbar_->layer(),
                          QCustomPlot::LayerInsertMode::limAbove);
  yerroravailable_ = true;
  emit yaxis_->getaxisrect_axis()->ErrorBar2DCreated(yerrorbar_);
}

void Bar2D::removeXerrorBar() {
  if (!xerroravailable_) return;

  parentPlot()->removePlottable(xerrorbar_);
  xerrorbar_ = nullptr;
  xerroravailable_ = false;
  emit xaxis_->getaxisrect_axis()->ErrorBar2DRemoved(
      xaxis_->getaxisrect_axis());
}

void Bar2D::removeYerrorBar() {
  if (!yerroravailable_) return;

  parentPlot()->removePlottable(yerrorbar_);
  yerrorbar_ = nullptr;
  yerroravailable_ = false;
  emit yaxis_->getaxisrect_axis()->ErrorBar2DRemoved(
      yaxis_->getaxisrect_axis());
}

Axis2D *Bar2D::getxaxis() const { return xaxis_; }

Axis2D *Bar2D::getyaxis() const { return yaxis_; }

Qt::PenStyle Bar2D::getstrokestyle_barplot() const { return pen().style(); }

QColor Bar2D::getstrokecolor_barplot() const { return pen().color(); }

double Bar2D::getstrokethickness_barplot() const { return pen().widthF(); }

QColor Bar2D::getfillcolor_barplot() const { return brush().color(); }

Qt::BrushStyle Bar2D::getfillstyle_barplot() const { return brush().style(); }

DataBlockBar *Bar2D::getdatablock_barplot() const { return bardata_; }

bool Bar2D::ishistogram_barplot() const {
  if (ishistogram_)
    return true;
  else
    return false;
}

DataBlockHist *Bar2D::getdatablock_histplot() const { return histdata_; }

void Bar2D::setxaxis_barplot(Axis2D *axis, bool override) {
  if (!override)
    Q_ASSERT(axis->getorientation_axis() == Axis2D::AxisOreantation::Bottom ||
             axis->getorientation_axis() == Axis2D::AxisOreantation::Top);
  if (axis == getxaxis()) return;

  xaxis_ = axis;
  setKeyAxis(axis);
}

void Bar2D::setyaxis_barplot(Axis2D *axis, bool override) {
  if (!override)
    Q_ASSERT(axis->getorientation_axis() == Axis2D::AxisOreantation::Left ||
             axis->getorientation_axis() == Axis2D::AxisOreantation::Right);
  if (axis == getyaxis()) return;

  yaxis_ = axis;
  setValueAxis(axis);
}

void Bar2D::setstrokestyle_barplot(const Qt::PenStyle &style) {
  QPen p = pen();
  p.setStyle(style);
  setPen(p);
}

void Bar2D::setstrokecolor_barplot(const QColor &color) {
  QPen p = pen();
  p.setColor(color);
  setPen(p);
}

void Bar2D::setstrokethickness_barplot(const double value) {
  QPen p = pen();
  p.setWidthF(value);
  setPen(p);
}

void Bar2D::setfillcolor_barplot(const QColor &color) {
  QBrush b = brush();
  b.setColor(color);
  setBrush(b);
}

void Bar2D::setfillstyle_barplot(const Qt::BrushStyle &style) {
  QBrush b = brush();
  b.setStyle(style);
  setBrush(b);
}

void Bar2D::setHistAutoBin(const bool status) {
  if (histdata_->getautobin() == status) return;
  histdata_->setautobin(status);
  setBarData(histdata_->gettable(), histdata_->getcolumn(),
             histdata_->getfrom(), histdata_->getto());
}

void Bar2D::setHistBinSize(const double binsize) {
  if (histdata_->getbinsize() == binsize) return;
  histdata_->setbinsize(binsize);
  setBarData(histdata_->gettable(), histdata_->getcolumn(),
             histdata_->getfrom(), histdata_->getto());
}

void Bar2D::setHistBegin(const double begin) {
  if (histdata_->getbegin() == begin) return;
  histdata_->setbegin(begin);
  setBarData(histdata_->gettable(), histdata_->getcolumn(),
             histdata_->getfrom(), histdata_->getto());
}

void Bar2D::setHistEnd(const double end) {
  if (histdata_->getend() == end) return;
  histdata_->setend(end);
  setBarData(histdata_->gettable(), histdata_->getcolumn(),
             histdata_->getfrom(), histdata_->getto());
}

void Bar2D::setBarData(Table *table, Column *xcol, Column *ycol, int from,
                       int to) {
  bardata_->regenerateDataBlock(table, xcol, ycol, from, to);
  setData(bardata_->data());
}

void Bar2D::setBarData(Table *table, Column *col, int from, int to) {
  histdata_->regenerateDataBlock(table, col, from, to);
  setData(histdata_->data());
  if (!histdata_->data().isNull() && histdata_->data()->size() > 1)
    setWidth(histdata_->data()->at(1)->mainKey() -
             histdata_->data()->at(0)->mainKey());
}

void Bar2D::save(XmlStreamWriter *xmlwriter, int xaxis, int yaxis) {
  xmlwriter->writeStartElement("bar");
  // axis
  (getxaxis()->getorientation_axis() == Axis2D::AxisOreantation::Top ||
   getxaxis()->getorientation_axis() == Axis2D::AxisOreantation::Bottom)
      ? xmlwriter->writeAttribute("orientation", "vertical")
      : xmlwriter->writeAttribute("orientation", "horizontal");
  xmlwriter->writeAttribute("xaxis", QString::number(xaxis));
  xmlwriter->writeAttribute("yaxis", QString::number(yaxis));
  xmlwriter->writeAttribute("legend", name());
  (ishistogram_) ? xmlwriter->writeAttribute("type", "histogram")
                 : xmlwriter->writeAttribute("type", "barxy");
  if (ishistogram_) {
    xmlwriter->writeAttribute("table", histdata_->gettable()->name());
    xmlwriter->writeAttribute("column", histdata_->getcolumn()->name());
    xmlwriter->writeAttribute("from", QString::number(histdata_->getfrom()));
    xmlwriter->writeAttribute("to", QString::number(histdata_->getto()));
    (histdata_->getautobin()) ? xmlwriter->writeAttribute("autobin", "true")
                              : xmlwriter->writeAttribute("autobin", "false");
    xmlwriter->writeAttribute("binsize",
                              QString::number(histdata_->getbinsize()));
    xmlwriter->writeAttribute("begin", QString::number(histdata_->getbegin()));
    xmlwriter->writeAttribute("end", QString::number(histdata_->getend()));
  } else {
    xmlwriter->writeAttribute("table", bardata_->gettable()->name());
    xmlwriter->writeAttribute("xcolumn", bardata_->getxcolumn()->name());
    xmlwriter->writeAttribute("ycolumn", bardata_->getycolumn()->name());
    xmlwriter->writeAttribute("from", QString::number(bardata_->getfrom()));
    xmlwriter->writeAttribute("to", QString::number(bardata_->getto()));
    switch (style_) {
      case Bar2D::BarStyle::Individual:
        xmlwriter->writeAttribute("style", "individual");
        break;
      case Bar2D::BarStyle::Grouped:
        xmlwriter->writeAttribute("style", "grouped");
        break;
      case Bar2D::BarStyle::Stacked:
        xmlwriter->writeAttribute("style", "stacked");
        break;
    }
    xmlwriter->writeAttribute("stackorder",
                              QString::number(getstackposition_barplot()));
  }
  double stackorgroupgap;
  (style_ == BarStyle::Grouped) ? stackorgroupgap = getBarGroup()->spacing()
                                : stackorgroupgap = stackingGap();
  xmlwriter->writeAttribute("stackgap", QString::number(stackorgroupgap));
  // error bar
  if (xerroravailable_) xerrorbar_->save(xmlwriter);
  if (yerroravailable_) yerrorbar_->save(xmlwriter);

  // line
  xmlwriter->writeStartElement("box");
  xmlwriter->writeAttribute("width", QString::number(width()));
  (antialiased()) ? xmlwriter->writeAttribute("antialias", "true")
                  : xmlwriter->writeAttribute("antialias", "false");
  (antialiasedFill()) ? xmlwriter->writeAttribute("antialiasfill", "true")
                      : xmlwriter->writeAttribute("antialiasfill", "false");
  xmlwriter->writePen(pen());
  xmlwriter->writeBrush(brush());
  xmlwriter->writeEndElement();
  xmlwriter->writeEndElement();
}

bool Bar2D::load(XmlStreamReader *xmlreader) {
  bool ok;
  while (!xmlreader->atEnd()) {
    if (xmlreader->isEndElement() && xmlreader->name() == "bar") break;

    if (xmlreader->isStartElement() && xmlreader->name() == "box") {
      // width
      double w = xmlreader->readAttributeDouble("width", &ok);
      (ok) ? setWidth(w)
           : xmlreader->raiseWarning(tr("Bar2D width property setting error"));

      // antialias
      bool ant = xmlreader->readAttributeBool("antialias", &ok);
      (ok) ? setAntialiased(ant)
           : xmlreader->raiseWarning(
                 tr("Bar2D antialias property setting error"));

      // antialias fill
      bool antfill = xmlreader->readAttributeBool("antialiasfill", &ok);
      (ok) ? setAntialiasedFill(antfill)
           : xmlreader->raiseWarning(
                 tr("Bar2D antialias fill property setting error"));

      // pen property
      while (!xmlreader->atEnd()) {
        xmlreader->readNext();
        if (xmlreader->isEndElement() && xmlreader->name() == "pen") break;
        // pen
        if (xmlreader->isStartElement() && xmlreader->name() == "pen") {
          QPen strokep = xmlreader->readPen(&ok);
          if (ok) {
            setPen(strokep);
          } else
            xmlreader->raiseWarning(tr("Bar2D pen property setting error"));
        }
      }

      // brush property
      while (!xmlreader->atEnd()) {
        xmlreader->readNext();
        if (xmlreader->isEndElement() && xmlreader->name() == "brush") break;
        // brush
        if (xmlreader->isStartElement() && xmlreader->name() == "brush") {
          QBrush b = xmlreader->readBrush(&ok);
          if (ok) {
            setBrush(b);
          } else
            xmlreader->raiseWarning(tr("Bar2D brush property setting error"));
        }
      }
    }

    xmlreader->readNext();
  }

  return !xmlreader->hasError();
}

void Bar2D::mousePressEvent(QMouseEvent *event, const QVariant &details) {
  if (event->button() == Qt::LeftButton) {
    switch (xaxis_->getaxisrect_axis()->getPickerTool()->getPicker()) {
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
  QCPBars::mousePressEvent(event, details);
}

void Bar2D::datapicker(QMouseEvent *, const QVariant &details) {
  QCPBarsDataContainer::const_iterator it;
  QCPDataSelection dataPoints = details.value<QCPDataSelection>();
  if (dataPoints.dataPointCount() > 0) {
    dataPoints.dataRange();
    it = data()->at(dataPoints.dataRange().begin());
    QPointF point = coordsToPixels(it->mainKey(), it->mainValue());
    xaxis_->getaxisrect_axis()->getPickerTool()->showtooltip(
        point, it->mainKey(), it->mainValue(), getxaxis(), getyaxis());
  }
}

void Bar2D::movepicker(QMouseEvent *event, const QVariant &details) {
  QCPBarsDataContainer::const_iterator it;
  QCPDataSelection dataPoints = details.value<QCPDataSelection>();
  if (dataPoints.dataPointCount() > 0) {
    dataPoints.dataRange();
    it = data()->at(dataPoints.dataRange().begin());
    QPointF point = coordsToPixels(it->mainKey(), it->mainValue());
    if (point.x() > event->localPos().x() - 10 &&
        point.x() < event->localPos().x() + 10 &&
        point.y() > event->localPos().y() - 10 &&
        point.y() < event->localPos().y() + 10) {
      xaxis_->getaxisrect_axis()->getPickerTool()->movepickermouspressbar(
          this, it->mainKey(), it->mainValue(), getxaxis(), getyaxis());
    }
  }
}

void Bar2D::removepicker(QMouseEvent *, const QVariant &details) {
  QCPBarsDataContainer::const_iterator it;
  QCPDataSelection dataPoints = details.value<QCPDataSelection>();
  if (dataPoints.dataPointCount() > 0) {
    dataPoints.dataRange();
    it = data()->at(dataPoints.dataRange().begin());
    bardata_->removedatafromtable(it->mainKey(), it->mainValue());
  }
}

void Bar2D::reloadIcon() {
  if (ishistogram_) {
    icon_ = IconLoader::load("graph2d-histogram", IconLoader::LightDark);
    return;
  }

  ((xaxis_->getorientation_axis() == Axis2D::AxisOreantation::Top ||
    xaxis_->getorientation_axis() == Axis2D::AxisOreantation::Bottom) &&
   style_ == BarStyle::Individual)
      ? icon_ = IconLoader::load("graph2d-vertical-bar", IconLoader::LightDark)
  : ((xaxis_->getorientation_axis() != Axis2D::AxisOreantation::Top &&
      xaxis_->getorientation_axis() != Axis2D::AxisOreantation::Bottom) &&
     style_ == BarStyle::Individual)
      ? icon_ =
            IconLoader::load("graph2d-horizontal-bar", IconLoader::LightDark)
  : ((xaxis_->getorientation_axis() == Axis2D::AxisOreantation::Top ||
      xaxis_->getorientation_axis() == Axis2D::AxisOreantation::Bottom) &&
     style_ == BarStyle::Grouped)
      ? icon_ = IconLoader::load("graph2d-vertical-group-bar",
                                 IconLoader::LightDark)
  : ((xaxis_->getorientation_axis() != Axis2D::AxisOreantation::Top &&
      xaxis_->getorientation_axis() != Axis2D::AxisOreantation::Bottom) &&
     style_ == BarStyle::Grouped)
      ? icon_ = IconLoader::load("graph2d-horizontal-group-bar",
                                 IconLoader::LightDark)
  : ((xaxis_->getorientation_axis() == Axis2D::AxisOreantation::Top ||
      xaxis_->getorientation_axis() == Axis2D::AxisOreantation::Bottom) &&
     style_ == BarStyle::Stacked)
      ? icon_ = IconLoader::load("graph2d-vertical-stack-bar",
                                 IconLoader::LightDark)
  : ((xaxis_->getorientation_axis() != Axis2D::AxisOreantation::Top &&
      xaxis_->getorientation_axis() != Axis2D::AxisOreantation::Bottom) &&
     style_ == BarStyle::Stacked)
      ? icon_ = IconLoader::load("graph2d-horizontal-stack-bar",
                                 IconLoader::LightDark)
      : icon_ = QIcon();
}
