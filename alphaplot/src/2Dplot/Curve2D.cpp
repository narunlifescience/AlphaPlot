#include "Curve2D.h"

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

Curve2D::Curve2D(Curve2D::Curve2DType curve2dtype, Table *table, Column *xcol,
                 Column *ycol, int from, int to, Axis2D *xAxis, Axis2D *yAxis)
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
      curve2dtype_(curve2dtype),
      xerrorbar_(nullptr),
      yerrorbar_(nullptr),
      xerroravailable_(false),
      yerroravailable_(false) {
  reloadIcon();
  init();
  setSelectable(QCP::SelectionType::stSingleData);
  setlinestrokecolor_cplot(
      Utilities::getRandColorGoldenRatio(Utilities::ColorPal::Dark));
  if (curve2dtype_ == Curve2DType::Spline) {
    setPen(Qt::NoPen);
    splinePen_.setColor(
        Utilities::getRandColorGoldenRatio(Utilities::ColorPal::Light));
    splinepoints_ = new QVector<QPointF>();
    splinecontrolpoints_ = new QVector<QPointF>();
    loadSplineData();
  }
  setData(curvedata_->data());
}

Curve2D::Curve2D(const PlotData::FunctionData funcdata, QVector<double> *xdata,
                 QVector<double> *ydata, Axis2D *xAxis, Axis2D *yAxis)
    : QCPCurve(xAxis, yAxis),
      xAxis_(xAxis),
      yAxis_(yAxis),
      scatterstyle_(new QCPScatterStyle(
          QCPScatterStyle::ssNone,
          Utilities::getRandColorGoldenRatio(Utilities::ColorPal::Dark),
          Utilities::getRandColorGoldenRatio(Utilities::ColorPal::Dark), 6.0)),
      curvedata_(nullptr),
      funcdata_(funcdata),
      functionData_(new QCPCurveDataContainer),
      type_(Graph2DCommon::PlotType::Function),
      xerrorbar_(nullptr),
      yerrorbar_(nullptr),
      xerroravailable_(false),
      yerroravailable_(false) {
  Q_ASSERT(xdata->size() == ydata->size());
  init();
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

void Curve2D::init() {
  layername_ = QString("<Curve2D>") +
               QDateTime::currentDateTime().toString("yyyy:MM:dd:hh:mm:ss:zzz");
  QThread::msleep(1);
  parentPlot()->addLayer(layername_, xAxis_->layer(), QCustomPlot::limBelow);
  setLayer(layername_);
  layer()->setMode(QCPLayer::LayerMode::lmBuffered);
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

  switch (curve2dtype_) {
    case Curve2D::Curve2DType::Curve:
      break;
    case Curve2D::Curve2DType::Spline:
      delete splinepoints_;
      delete splinecontrolpoints_;
      break;
  }
  parentPlot()->removeLayer(layer());
}

void Curve2D::setXerrorBar(Table *table, Column *errorcol, int from, int to) {
  if (xerroravailable_ || type_ == Graph2DCommon::PlotType::Function) {
    qDebug() << "X error bar already defined or unsupported plot type";
    return;
  }
  xerrorbar_ = new ErrorBar2D(table, errorcol, from, to, xAxis_, yAxis_,
                              QCPErrorBars::ErrorType::etKeyError, this);
  xerrorbar_->setLayer(layername_);
  xerroravailable_ = true;
  emit xAxis_->getaxisrect_axis()->ErrorBar2DCreated(xerrorbar_);
}

void Curve2D::setYerrorBar(Table *table, Column *errorcol, int from, int to) {
  if (yerroravailable_ || type_ == Graph2DCommon::PlotType::Function) {
    qDebug() << "Y error bar already defined or unsupported plot type";
    return;
  }
  yerrorbar_ = new ErrorBar2D(table, errorcol, from, to, xAxis_, yAxis_,
                              QCPErrorBars::ErrorType::etValueError, this);
  yerrorbar_->setLayer(layername_);
  yerroravailable_ = true;
  emit yAxis_->getaxisrect_axis()->ErrorBar2DCreated(yerrorbar_);
}

void Curve2D::removeXerrorBar() {
  if (!xerroravailable_) return;

  parentPlot()->removePlottable(xerrorbar_);
  xerrorbar_ = nullptr;
  xerroravailable_ = false;
  emit xAxis_->getaxisrect_axis()->ErrorBar2DRemoved(
      xAxis_->getaxisrect_axis());
}

void Curve2D::removeYerrorBar() {
  if (!yerroravailable_) return;

  parentPlot()->removePlottable(yerrorbar_);
  yerrorbar_ = nullptr;
  yerroravailable_ = false;
  emit yAxis_->getaxisrect_axis()->ErrorBar2DRemoved(
      yAxis_->getaxisrect_axis());
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
  if (curve2dtype_ == Curve2DType::Spline) loadSplineData();
}

int Curve2D::getlinetype_cplot() const {
  switch (lineStyle()) {
    case LineStyle::lsNone:
      return 0;
    case LineStyle::lsLine:
      return 1;
  }
  return 0;
}

Qt::PenStyle Curve2D::getlinestrokestyle_cplot() const {
  switch (curve2dtype_) {
    case Curve2D::Curve2DType::Curve:
      return pen().style();
    case Curve2D::Curve2DType::Spline:
      return splinePen_.style();
  }
  return QPen().style();
}

QColor Curve2D::getlinestrokecolor_cplot() const {
  switch (curve2dtype_) {
    case Curve2D::Curve2DType::Curve:
      return pen().color();
    case Curve2D::Curve2DType::Spline:
      return splinePen_.color();
  }
  return QPen().color();
}

double Curve2D::getlinestrokethickness_cplot() const {
  switch (curve2dtype_) {
    case Curve2D::Curve2DType::Curve:
      return pen().widthF();
    case Curve2D::Curve2DType::Spline:
      return splinePen_.widthF();
  }
  return QPen().widthF();
}

QPen Curve2D::getlinepen_cplot() const {
  switch (curve2dtype_) {
    case Curve2D::Curve2DType::Curve:
      return pen();
    case Curve2D::Curve2DType::Spline:
      return splinePen_;
  }
  return QPen();
}

QColor Curve2D::getlinefillcolor_cplot() const {
  switch (curve2dtype_) {
    case Curve2D::Curve2DType::Curve:
      return brush().color();
    case Curve2D::Curve2DType::Spline:
      return splineBrush_.color();
  }
  return QBrush().color();
}

Qt::BrushStyle Curve2D::getlinefillstyle_cplot() const {
  switch (curve2dtype_) {
    case Curve2D::Curve2DType::Curve:
      return brush().style();
    case Curve2D::Curve2DType::Spline:
      return splineBrush_.style();
  }
  return QBrush().style();
}

QBrush Curve2D::getlinebrush_cplot() const {
  switch (curve2dtype_) {
    case Curve2D::Curve2DType::Curve:
      return brush();
    case Curve2D::Curve2DType::Spline:
      return splineBrush_;
  }
  return QBrush();
}

bool Curve2D::getlineantialiased_cplot() const { return antialiased(); }

bool Curve2D::getlinefillstatus_cplot() const {
  if (curve2dtype_ == Curve2D::Curve2DType::Curve) {
    if (brush().style() == Qt::NoBrush) {
      return false;
    } else {
      return true;
    }
  } else {
    if (splineBrush_.style() == Qt::NoBrush) {
      return false;
    } else {
      return true;
    }
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

QBrush Curve2D::getscatterbrush_cplot() const { return scatterStyle().brush(); }

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

QPen Curve2D::getscatterpen_cplot() const { return scatterStyle().pen(); }

bool Curve2D::getscatterantialiased_cplot() const {
  return antialiasedScatters();
}

bool Curve2D::getlegendvisible_cplot() const {
  return mParentPlot->legend->hasItemWithPlottable(this);
}

QString Curve2D::getlegendtext_cplot() const { return name(); }

Axis2D *Curve2D::getxaxis() const { return xAxis_; }

Axis2D *Curve2D::getyaxis() const { return yAxis_; }

void Curve2D::setxaxis_cplot(Axis2D *axis) {
  Q_ASSERT(axis->getorientation_axis() == Axis2D::AxisOreantation::Bottom ||
           axis->getorientation_axis() == Axis2D::AxisOreantation::Top);
  if (axis == getxaxis()) return;

  xAxis_ = axis;
  setKeyAxis(axis);
}

void Curve2D::setyaxis_cplot(Axis2D *axis) {
  Q_ASSERT(axis->getorientation_axis() == Axis2D::AxisOreantation::Left ||
           axis->getorientation_axis() == Axis2D::AxisOreantation::Right);
  if (axis == getyaxis()) return;

  yAxis_ = axis;
  setValueAxis(axis);
}

void Curve2D::setlinetype_cplot(const int type) {
  switch (type) {
    case 0:
      if (curve2dtype_ == Curve2D::Curve2DType::Curve) {
        setLineStyle(LineStyle::lsNone);
      } else {
        splinePen_.setStyle(Qt::PenStyle::NoPen);
      }
      break;
    case 1:
      if (curve2dtype_ == Curve2D::Curve2DType::Curve) {
        setLineStyle(LineStyle::lsLine);
      } else {
        splinePen_.setStyle(Qt::PenStyle::SolidLine);
      }
      break;
  }
  reloadIcon();
}

void Curve2D::setlinestrokestyle_cplot(const Qt::PenStyle &style) {
  if (curve2dtype_ == Curve2D::Curve2DType::Curve) {
    QPen p = pen();
    p.setStyle(style);
    setPen(p);
  } else {
    splinePen_.setStyle(style);
  }
}

void Curve2D::setlinestrokecolor_cplot(const QColor &color) {
  if (curve2dtype_ == Curve2D::Curve2DType::Curve) {
    QPen p = pen();
    p.setColor(color);
    setPen(p);
  } else {
    splinePen_.setColor(color);
  }
}

void Curve2D::setlinestrokethickness_cplot(const double value) {
  if (curve2dtype_ == Curve2D::Curve2DType::Curve) {
    QPen p = pen();
    p.setWidthF(value);
    setPen(p);
  } else {
    splinePen_.setWidthF(value);
  }
}

void Curve2D::setlinefillcolor_cplot(const QColor &color) {
  if (curve2dtype_ == Curve2D::Curve2DType::Curve) {
    QBrush b = brush();
    b.setColor(color);
    setBrush(b);
  } else {
    splineBrush_.setColor(color);
  }
}

void Curve2D::setlinefillstyle_cplot(const Qt::BrushStyle &style) {
  if (curve2dtype_ == Curve2D::Curve2DType::Curve) {
    QBrush b = brush();
    if (b.style() != Qt::BrushStyle::NoBrush) {
      b.setStyle(style);
      setBrush(b);
    }
  } else {
    if (splineBrush_.style() != Qt::BrushStyle::NoBrush)
      splineBrush_.setStyle(style);
  }
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
  reloadIcon();
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
  if (curve2dtype_ == Curve2D::Curve2DType::Curve) {
    if (value) {
      QBrush b = brush();
      b.setStyle(Qt::SolidPattern);
      setBrush(b);
    } else {
      QBrush b = brush();
      b.setStyle(Qt::NoBrush);
      setBrush(b);
    }
  } else {
    if (value) {
      splineBrush_.setStyle(Qt::SolidPattern);
    } else {
      splineBrush_.setStyle(Qt::NoBrush);
    }
  }
  reloadIcon();
}

void Curve2D::setlegendvisible_cplot(const bool value) {
  (value) ? addToLegend() : removeFromLegend();
}

void Curve2D::setlegendtext_cplot(const QString &text) { setName(text); }

void Curve2D::save(XmlStreamWriter *xmlwriter, int xaxis, int yaxis) {
  xmlwriter->writeStartElement("curve");
  // axis
  xmlwriter->writeAttribute("xaxis", QString::number(xaxis));
  xmlwriter->writeAttribute("yaxis", QString::number(yaxis));

  switch (getcurvetype_cplot()) {
    case Curve2D::Curve2DType::Curve:
      xmlwriter->writeAttribute("type", "curve");
      break;
    case Curve2D::Curve2DType::Spline:
      xmlwriter->writeAttribute("type", "spline");
      break;
  }

  (getlegendvisible_cplot())
      ? xmlwriter->writeAttribute("legendvisible", "true")
      : xmlwriter->writeAttribute("legendvisible", "false");
  xmlwriter->writeAttribute("legend", getlegendtext_cplot());
  // data
  if (curvedata_) {
    xmlwriter->writeAttribute("data", "table");
    xmlwriter->writeAttribute("table", curvedata_->gettable()->name());
    xmlwriter->writeAttribute("xcolumn", curvedata_->getxcolumn()->name());
    xmlwriter->writeAttribute("ycolumn", curvedata_->getycolumn()->name());
    xmlwriter->writeAttribute("from", QString::number(curvedata_->getfrom()));
    xmlwriter->writeAttribute("to", QString::number(curvedata_->getto()));
  } else if (functionData_) {
    xmlwriter->writeAttribute("data", "function");
    switch (funcdata_.type) {
      case 0:
        xmlwriter->writeAttribute("functiontype", "normal");
        (funcdata_.functions.size() == 1)
            ? xmlwriter->writeAttribute("function", funcdata_.functions.at(0))
            : xmlwriter->writeAttribute("function", "unknown");
        break;
      case 1:
        xmlwriter->writeAttribute("functiontype", "parametric");
        if (funcdata_.functions.size() == 2) {
          xmlwriter->writeAttribute("functionx", funcdata_.functions.at(0));
          xmlwriter->writeAttribute("functiony", funcdata_.functions.at(1));
        } else {
          xmlwriter->writeAttribute("functionx", "unknown");
          xmlwriter->writeAttribute("functiony", "unknown");
        }
        xmlwriter->writeAttribute("parameter", funcdata_.parameter);
        break;
      case 2:
        xmlwriter->writeAttribute("functiontype", "polar");
        if (funcdata_.functions.size() == 2) {
          xmlwriter->writeAttribute("functionr", funcdata_.functions.at(0));
          xmlwriter->writeAttribute("functiontheta", funcdata_.functions.at(1));
        } else {
          xmlwriter->writeAttribute("functionr", "unknown");
          xmlwriter->writeAttribute("functiontheta", "unknown");
        }
        xmlwriter->writeAttribute("parameter", funcdata_.parameter);
        break;
    }
    xmlwriter->writeAttribute("from", QString::number(funcdata_.from));
    xmlwriter->writeAttribute("to", QString::number(funcdata_.to));
    xmlwriter->writeAttribute("points", QString::number(funcdata_.points));
    xmlwriter->writeStartElement("functiondata");

    for (int i = 0; i < functionData_.data()->size(); ++i) {
      xmlwriter->writeStartElement("data");
      xmlwriter->writeAttribute(
          "xdata", QString::number(functionData_.data()->at(i)->key));
      xmlwriter->writeAttribute(
          "ydata", QString::number(functionData_.data()->at(i)->value));
      xmlwriter->writeEndElement();
    }
    xmlwriter->writeEndElement();
  }
  // error bar
  if (xerroravailable_) xerrorbar_->save(xmlwriter);
  if (yerroravailable_) yerrorbar_->save(xmlwriter);
  // line
  xmlwriter->writeStartElement("line");
  switch (getlinetype_cplot()) {
    case 0:
      xmlwriter->writeAttribute("style", "none");
      break;
    case 1:
      xmlwriter->writeAttribute("style", "line");
      break;
  }
  (getlinefillstatus_cplot()) ? xmlwriter->writeAttribute("fill", "true")
                              : xmlwriter->writeAttribute("fill", "false");
  (getlineantialiased_cplot())
      ? xmlwriter->writeAttribute("antialias", "true")
      : xmlwriter->writeAttribute("antialias", "false");
  xmlwriter->writePen(getlinepen_cplot());
  xmlwriter->writeBrush(getlinebrush_cplot());
  xmlwriter->writeEndElement();

  // scatter
  xmlwriter->writeStartElement("scatter");
  switch (getscattershape_cplot()) {
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
  xmlwriter->writeAttribute("size", QString::number(getscattersize_cplot()));
  (getscatterantialiased_cplot())
      ? xmlwriter->writeAttribute("antialias", "true")
      : xmlwriter->writeAttribute("antialias", "false");
  xmlwriter->writePen(getscatterpen_cplot());
  xmlwriter->writeBrush(getscatterbrush_cplot());
  xmlwriter->writeEndElement();
  xmlwriter->writeEndElement();
}

bool Curve2D::load(XmlStreamReader *xmlreader) {
  bool ok;
  while (!xmlreader->atEnd()) {
    if (xmlreader->isEndElement() && xmlreader->name() == "curve") break;

    // line
    if (xmlreader->isStartElement() && xmlreader->name() == "line") {
      // line style
      QString style = xmlreader->readAttributeString("style", &ok);
      if (ok) {
        if (style == "none") {
          setlinetype_cplot(0);
        } else if (style == "line") {
          setlinetype_cplot(1);
        }
      } else
        xmlreader->raiseWarning(
            tr("Curve2D line style property setting error"));

      // line fill status
      bool fill = xmlreader->readAttributeBool("fill", &ok);
      (ok) ? setlinefillstatus_cplot(fill)
           : xmlreader->raiseWarning(
                 tr("Curve2D line fill status property setting error"));

      // line antialias
      bool lineantialias = xmlreader->readAttributeBool("antialias", &ok);
      (ok) ? setAntialiased(lineantialias)
           : xmlreader->raiseWarning(
                 tr("Curve2D line antialias property setting error"));

      // line pen property
      while (!xmlreader->atEnd()) {
        xmlreader->readNext();
        if (xmlreader->isEndElement() && xmlreader->name() == "pen") break;
        // pen
        if (xmlreader->isStartElement() && xmlreader->name() == "pen") {
          QPen strokep = xmlreader->readPen(&ok);
          if (ok) {
            setlinestrokecolor_cplot(strokep.color());
            setlinestrokestyle_cplot(strokep.style());
            setlinestrokethickness_cplot(strokep.widthF());
          } else
            xmlreader->raiseWarning(
                tr("Curve2D line pen property setting error"));
        }
      }

      // line brush property
      while (!xmlreader->atEnd()) {
        xmlreader->readNext();
        if (xmlreader->isEndElement() && xmlreader->name() == "brush") break;
        // brush
        if (xmlreader->isStartElement() && xmlreader->name() == "brush") {
          QBrush b = xmlreader->readBrush(&ok);
          if (ok) {
            setlinefillcolor_cplot(b.color());
            setlinefillstyle_cplot(b.style());
          } else
            xmlreader->raiseWarning(
                tr("Curve2D linebrush property setting error"));
        }
      }
    }

    // scatter
    if (xmlreader->isStartElement() && xmlreader->name() == "scatter") {
      // scatter shape
      QString scattershape = xmlreader->readAttributeString("style", &ok);
      if (ok) {
        if (scattershape == "dot") {
          setscattershape_cplot(Graph2DCommon::ScatterStyle::Dot);
        } else if (scattershape == "disc") {
          setscattershape_cplot(Graph2DCommon::ScatterStyle::Disc);
        } else if (scattershape == "none") {
          setscattershape_cplot(Graph2DCommon::ScatterStyle::None);
        } else if (scattershape == "plus") {
          setscattershape_cplot(Graph2DCommon::ScatterStyle::Plus);
        } else if (scattershape == "star") {
          setscattershape_cplot(Graph2DCommon::ScatterStyle::Star);
        } else if (scattershape == "cross") {
          setscattershape_cplot(Graph2DCommon::ScatterStyle::Cross);
        } else if (scattershape == "peace") {
          setscattershape_cplot(Graph2DCommon::ScatterStyle::Peace);
        } else if (scattershape == "circle") {
          setscattershape_cplot(Graph2DCommon::ScatterStyle::Circle);
        } else if (scattershape == "square") {
          setscattershape_cplot(Graph2DCommon::ScatterStyle::Square);
        } else if (scattershape == "diamond") {
          setscattershape_cplot(Graph2DCommon::ScatterStyle::Diamond);
        } else if (scattershape == "triangle") {
          setscattershape_cplot(Graph2DCommon::ScatterStyle::Triangle);
        } else if (scattershape == "pluscircle") {
          setscattershape_cplot(Graph2DCommon::ScatterStyle::PlusCircle);
        } else if (scattershape == "plussquare") {
          setscattershape_cplot(Graph2DCommon::ScatterStyle::PlusSquare);
        } else if (scattershape == "crosscircle") {
          setscattershape_cplot(Graph2DCommon::ScatterStyle::CrossCircle);
        } else if (scattershape == "crosssquare") {
          setscattershape_cplot(Graph2DCommon::ScatterStyle::CrossSquare);
        } else if (scattershape == "triangleinverted") {
          setscattershape_cplot(Graph2DCommon::ScatterStyle::TriangleInverted);
        }
      } else
        xmlreader->raiseWarning(
            tr("Curve2D scatter shape property setting error"));

      // scatter size
      int scattersize = xmlreader->readAttributeInt("size", &ok);
      (ok) ? setscattersize_cplot(scattersize)
           : xmlreader->raiseWarning(
                 tr("Curve2D scatter size property setting error"));

      // scatter antialias
      bool scatterantialias = xmlreader->readAttributeBool("antialias", &ok);
      (ok) ? setscatterantialiased_cplot(scatterantialias)
           : xmlreader->raiseWarning(
                 tr("Curve2D scatter antialias property setting error"));

      // scatter pen property
      while (!xmlreader->atEnd()) {
        xmlreader->readNext();
        if (xmlreader->isEndElement() && xmlreader->name() == "pen") break;
        // pen
        if (xmlreader->isStartElement() && xmlreader->name() == "pen") {
          QPen strokep = xmlreader->readPen(&ok);
          if (ok) {
            setscatterstrokecolor_cplot(strokep.color());
            setscatterstrokestyle_cplot(strokep.style());
            setscatterstrokethickness_cplot(strokep.widthF());
          } else
            xmlreader->raiseWarning(
                tr("Curve2D scatter pen property setting error"));
        }
      }

      // scatter brush property
      while (!xmlreader->atEnd()) {
        xmlreader->readNext();
        if (xmlreader->isEndElement() && xmlreader->name() == "brush") break;
        // brush
        if (xmlreader->isStartElement() && xmlreader->name() == "brush") {
          QBrush b = xmlreader->readBrush(&ok);
          if (ok) {
            setscatterfillcolor_cplot(b.color());
          } else
            xmlreader->raiseWarning(
                tr("Curve2D scatterbrush property setting error"));
        }
      }
    }
    xmlreader->readNext();
  }

  return !xmlreader->hasError();
}

void Curve2D::draw(QCPPainter *painter) {
  if (curve2dtype_ == Curve2D::Curve2DType::Spline) drawSpline(painter);
  QCPCurve::draw(painter);
}

void Curve2D::drawCurveLine(QCPPainter *painter,
                            const QVector<QPointF> &lines) const {
  QCPCurve::drawCurveLine(painter, lines);

  // draw base fill under graph, fill goes all the way to the zero-value-line:
  /*painter->setBrush(QBrush(Qt::red));
  QPolygonF result(lines.size() + 2);
  result[0] = QPointF(0, lines.at(0).y());
  result.append(lines);
  result.append(QPointF(0, lines.at(lines.size() - 1).y()));
  painter->drawPolygon(result);*/
}

void Curve2D::mousePressEvent(QMouseEvent *event, const QVariant &details) {
  if (event->button() == Qt::LeftButton) {
    switch (xAxis_->getaxisrect_axis()->getPickerTool()->getPicker()) {
      case Graph2DCommon::Picker::None:
      case Graph2DCommon::Picker::DataGraph:
      case Graph2DCommon::Picker::DragRange:
      case Graph2DCommon::Picker::ZoomRange:
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
      case Graph2DCommon::Picker::DataRange:
        dataRangePicker(event, details);
        break;
      default:
        break;
    }
  }
  QCPCurve::mousePressEvent(event, details);
}

void Curve2D::drawSpline(QCPPainter *painter) {
  if (splinepoints_->size() < 2 && splinecontrolpoints_->size() < 2) return;

  QPainterPath path;
  QPointF point = QPointF(xAxis_->coordToPixel(splinepoints_->at(0).x()),
                          yAxis_->coordToPixel(splinepoints_->at(0).y()));
  path.moveTo(point);
  for (int i = 0, j = 0; i < splinepoints_->size() - 1; i++, j++) {
    QPointF ctrlpoint1 =
        QPointF(xAxis_->coordToPixel(splinecontrolpoints_->at(i + j).x()),
                yAxis_->coordToPixel(splinecontrolpoints_->at(i + j).y()));
    QPointF ctrlpoint2 =
        QPointF(xAxis_->coordToPixel(splinecontrolpoints_->at(i + 1 + j).x()),
                yAxis_->coordToPixel(splinecontrolpoints_->at(i + 1 + j).y()));
    QPointF sourcepoint =
        QPointF(xAxis_->coordToPixel(splinepoints_->at(i + 1).x()),
                yAxis_->coordToPixel(splinepoints_->at(i + 1).y()));
    path.cubicTo(ctrlpoint1, ctrlpoint2, sourcepoint);
  }
  painter->setPen(splinePen_);
  painter->setBrush(splineBrush_);
  painter->drawPath(path);
}

void Curve2D::loadSplineData() {
  splinepoints_->clear();
  splinecontrolpoints_->clear();
  for (int i = 0; i < curvedata_->size(); i++) {
    splinepoints_->append(QPointF(curvedata_->data()->at(i)->mainKey(),
                                  curvedata_->data()->at(i)->mainValue()));
  }
  *splinecontrolpoints_ = calculateControlPoints(*splinepoints_);
}

QVector<QPointF> Curve2D::calculateControlPoints(
    const QVector<QPointF> &points) {
  QVector<QPointF> controlPoints;
  controlPoints.resize(points.count() * 2 - 2);
  int n = points.count() - 1;

  if (n == 1) {
    // for n==1
    controlPoints[0].setX((2 * points[0].x() + points[1].x()) / 3);
    controlPoints[0].setY((2 * points[0].y() + points[1].y()) / 3);
    controlPoints[1].setX(2 * controlPoints[0].x() - points[0].x());
    controlPoints[1].setY(2 * controlPoints[0].y() - points[0].y());
    return controlPoints;
  }

  // Calculate first Bezier control points
  // Set of equations for P0 to Pn points.
  QVector<qreal> vector;
  vector.resize(n);
  vector[0] = points[0].x() + 2 * points[1].x();
  for (int i = 1; i < n - 1; ++i)
    vector[i] = 4 * points[i].x() + 2 * points[i + 1].x();
  vector[n - 1] = (8 * points[n - 1].x() + points[n].x()) / 2.0;
  QVector<qreal> xControl = firstControlPoints(vector);
  vector[0] = points[0].y() + 2 * points[1].y();
  for (int i = 1; i < n - 1; ++i)
    vector[i] = 4 * points[i].y() + 2 * points[i + 1].y();
  vector[n - 1] = (8 * points[n - 1].y() + points[n].y()) / 2.0;
  QVector<qreal> yControl = firstControlPoints(vector);
  for (int i = 0, j = 0; i < n; ++i, ++j) {
    controlPoints[j].setX(xControl[i]);
    controlPoints[j].setY(yControl[i]);
    j++;
    if (i < n - 1) {
      controlPoints[j].setX(2 * points[i + 1].x() - xControl[i + 1]);
      controlPoints[j].setY(2 * points[i + 1].y() - yControl[i + 1]);
    } else {
      controlPoints[j].setX((points[n].x() + xControl[n - 1]) / 2);
      controlPoints[j].setY((points[n].y() + yControl[n - 1]) / 2);
    }
  }
  return controlPoints;
}

QVector<qreal> Curve2D::firstControlPoints(const QVector<qreal> &vector) {
  QVector<qreal> result;
  int count = vector.count();
  result.resize(count);
  result[0] = vector[0] / 2.0;
  QVector<qreal> temp;
  temp.resize(count);
  temp[0] = 0;
  qreal b = 2.0;
  for (int i = 1; i < count; i++) {
    temp[i] = 1 / b;
    b = (i < count - 1 ? 4.0 : 3.5) - temp[i];
    result[i] = (vector[i] - result[i - 1]) / b;
  }
  for (int i = 1; i < count; i++)
    result[count - i - 1] -= temp[count - i] * result[count - i];
  return result;
}

void Curve2D::datapicker(QMouseEvent *event, const QVariant &details) {
  QCPCurveDataContainer::const_iterator it;
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
      xAxis_->getaxisrect_axis()->getPickerTool()->datapoint(
          this, it->mainKey(), it->mainValue());
    }
  }
}

void Curve2D::movepicker(QMouseEvent *event, const QVariant &details) {
  QCPCurveDataContainer::const_iterator it;
  QCPDataSelection dataPoints = details.value<QCPDataSelection>();
  if (dataPoints.dataPointCount() > 0) {
    dataPoints.dataRange();
    it = data()->at(dataPoints.dataRange().begin());
    QPointF point = coordsToPixels(it->mainKey(), it->mainValue());
    if (point.x() > event->localPos().x() - 10 &&
        point.x() < event->localPos().x() + 10 &&
        point.y() > event->localPos().y() - 10 &&
        point.y() < event->localPos().y() + 10) {
      xAxis_->getaxisrect_axis()->getPickerTool()->movepickermouspresscurve(
          this, it->mainKey(), it->mainValue(), getxaxis(), getyaxis());
    }
  }
}

void Curve2D::removepicker(QMouseEvent *event, const QVariant &details) {
  QCPCurveDataContainer::const_iterator it;
  QCPDataSelection dataPoints = details.value<QCPDataSelection>();
  if (dataPoints.dataPointCount() > 0) {
    dataPoints.dataRange();
    it = data()->at(dataPoints.dataRange().begin());
    QPointF point = coordsToPixels(it->mainKey(), it->mainValue());
    if (point.x() > event->localPos().x() - 10 &&
        point.x() < event->localPos().x() + 10 &&
        point.y() > event->localPos().y() - 10 &&
        point.y() < event->localPos().y() + 10) {
      if (curvedata_->removedatafromtable(it->mainKey(), it->mainValue())) {
        if (curve2dtype_ == Curve2D::Curve2DType::Spline) loadSplineData();
      }
    }
  }
}

void Curve2D::dataRangePicker(QMouseEvent *event, const QVariant &details) {
  QCPCurveDataContainer::const_iterator it;
  QCPDataSelection dataPoints = details.value<QCPDataSelection>();
  if (dataPoints.dataPointCount() > 0) {
    dataPoints.dataRange();
    it = data()->at(dataPoints.dataRange().begin());
    QPointF point = coordsToPixels(it->mainKey(), it->mainValue());
    if (point.x() > event->localPos().x() - 10 &&
        point.x() < event->localPos().x() + 10 &&
        point.y() > event->localPos().y() - 10 &&
        point.y() < event->localPos().y() + 10) {
      xAxis_->getaxisrect_axis()->getPickerTool()->rangepickermousepress(
          this, it->mainKey(), it->mainValue());
    }
  }
}

void Curve2D::reloadIcon() {
  if (curve2dtype_ == Curve2DType::Spline) {
    icon_ = IconLoader::load("graph2d-spline", IconLoader::LightDark);
    return;
  }
  if (type_ == Graph2DCommon::PlotType::Function) {
    icon_ = IconLoader::load("graph2d-function-xy", IconLoader::LightDark);
    return;
  }
  (getlinetype_cplot() == 0 &&
   getscattershape_cplot() == Graph2DCommon::ScatterStyle::None &&
   getlinefillstatus_cplot() == false)
      ? icon_ = IconLoader::load("graph2d-scatter", IconLoader::LightDark)
  : (getlinetype_cplot() == 0 &&
     getscattershape_cplot() == Graph2DCommon::ScatterStyle::None &&
     getlinefillstatus_cplot() == true)
      ? icon_ = IconLoader::load("graph2d-scatter", IconLoader::LightDark)
  : (getlinetype_cplot() == 0 &&
     getscattershape_cplot() != Graph2DCommon::ScatterStyle::None &&
     getlinefillstatus_cplot() == false)
      ? icon_ = IconLoader::load("graph2d-scatter", IconLoader::LightDark)
  : (getlinetype_cplot() == 0 &&
     getscattershape_cplot() != Graph2DCommon::ScatterStyle::None &&
     getlinefillstatus_cplot() == true)
      ? icon_ = IconLoader::load("graph2d-scatter", IconLoader::LightDark)
  : (getlinetype_cplot() == 1 &&
     getscattershape_cplot() == Graph2DCommon::ScatterStyle::None &&
     getlinefillstatus_cplot() == false)
      ? icon_ = IconLoader::load("graph2d-line", IconLoader::LightDark)
  : (getlinetype_cplot() == 1 &&
     getscattershape_cplot() == Graph2DCommon::ScatterStyle::None &&
     getlinefillstatus_cplot() == true)
      ? icon_ = IconLoader::load("graph2d-area", IconLoader::LightDark)
  : (getlinetype_cplot() == 1 &&
     getscattershape_cplot() != Graph2DCommon::ScatterStyle::None &&
     getlinefillstatus_cplot() == false)
      ? icon_ = IconLoader::load("graph2d-line-scatter", IconLoader::LightDark)
  : (getlinetype_cplot() == 1 &&
     getscattershape_cplot() != Graph2DCommon::ScatterStyle::None &&
     getlinefillstatus_cplot() == true)
      ? icon_ = IconLoader::load("graph2d-area", IconLoader::LightDark)
      : IconLoader::load("graph2d-line-scatter", IconLoader::LightDark);
}
