#include "LineSpecial2D.h"

#include <QApplication>
#include <QCursor>

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

LineSpecial2D::LineSpecial2D(Table *table, Column *xcol, Column *ycol, int from,
                             int to, Axis2D *xAxis, Axis2D *yAxis)
    : QCPGraph(xAxis, yAxis),
      xAxis_(xAxis),
      yAxis_(yAxis),
      scatterstyle_(new QCPScatterStyle(
          QCPScatterStyle::ssDisc,
          Utilities::getRandColorGoldenRatio(Utilities::ColorPal::Dark),
          Utilities::getRandColorGoldenRatio(Utilities::ColorPal::Dark), 6.0)),
      graphdata_(new DataBlockGraph(table, xcol, ycol, from, to)),
      functionData_(nullptr),
      xerrorbar_(nullptr),
      yerrorbar_(nullptr),
      layername_(
          QString("<LineSpecial2D>") +
          QDateTime::currentDateTime().toString("yyyy:MM:dd:hh:mm:ss:zzz")),
      xerroravailable_(false),
      yerroravailable_(false) {
  reloadIcon();
  QThread::msleep(1);
  parentPlot()->addLayer(layername_, xAxis_->layer(), QCustomPlot::limBelow);
  setLayer(layername_);
  layer()->setMode(QCPLayer::LayerMode::lmBuffered);
  setSelectable(QCP::SelectionType::stSingleData);
  setlinestrokecolor_lsplot(
      Utilities::getRandColorGoldenRatio(Utilities::ColorPal::Dark));
  setData(graphdata_->data());
}

LineSpecial2D::~LineSpecial2D() {
  delete scatterstyle_;
  delete graphdata_;
  parentPlot()->removeLayer(layer());
}

void LineSpecial2D::setXerrorBar(Table *table, Column *errorcol, int from,
                                 int to) {
  if (xerroravailable_) {
    qDebug() << "X error bar already defined";
    return;
  }
  xerrorbar_ = new ErrorBar2D(table, errorcol, from, to, xAxis_, yAxis_,
                              QCPErrorBars::ErrorType::etKeyError, this);
  xerroravailable_ = true;
  emit xAxis_->getaxisrect_axis()->ErrorBar2DCreated(xerrorbar_);
}

void LineSpecial2D::setYerrorBar(Table *table, Column *errorcol, int from,
                                 int to) {
  if (yerroravailable_) {
    qDebug() << "Y error bar already defined";
    return;
  }
  yerrorbar_ = new ErrorBar2D(table, errorcol, from, to, xAxis_, yAxis_,
                              QCPErrorBars::ErrorType::etValueError, this);
  yerroravailable_ = true;
  emit yAxis_->getaxisrect_axis()->ErrorBar2DCreated(yerrorbar_);
}

void LineSpecial2D::setGraphData(Table *table, Column *xcol, Column *ycol,
                                 int from, int to) {
  graphdata_->regenerateDataBlock(table, xcol, ycol, from, to);
  setData(graphdata_->data());
}

void LineSpecial2D::removeXerrorBar() {
  if (!xerroravailable_) return;

  parentPlot()->removePlottable(xerrorbar_);
  xerrorbar_ = nullptr;
  xerroravailable_ = false;
  emit xAxis_->getaxisrect_axis()->ErrorBar2DRemoved(
      xAxis_->getaxisrect_axis());
}

void LineSpecial2D::removeYerrorBar() {
  if (!yerroravailable_) return;

  parentPlot()->removePlottable(yerrorbar_);
  yerrorbar_ = nullptr;
  yerroravailable_ = false;
  emit yAxis_->getaxisrect_axis()->ErrorBar2DRemoved(
      yAxis_->getaxisrect_axis());
}

Graph2DCommon::LineStyleType LineSpecial2D::getlinetype_lsplot() const {
  Graph2DCommon::LineStyleType linestyletype;

  switch (lineStyle()) {
    case lsStepLeft:
      linestyletype = Graph2DCommon::LineStyleType::StepLeft;
      break;
    case lsStepRight:
      linestyletype = Graph2DCommon::LineStyleType::StepRight;
      break;
    case lsStepCenter:
      linestyletype = Graph2DCommon::LineStyleType::StepCenter;
      break;
    case lsImpulse:
      linestyletype = Graph2DCommon::LineStyleType::Impulse;
      break;
    case lsLine:
      linestyletype = Graph2DCommon::LineStyleType::Line;
      break;
    case lsNone:
      linestyletype = Graph2DCommon::LineStyleType::StepLeft;
  }
  return linestyletype;
}

Qt::PenStyle LineSpecial2D::getlinestrokestyle_lsplot() const {
  return pen().style();
}

QColor LineSpecial2D::getlinestrokecolor_lsplot() const {
  return pen().color();
}

double LineSpecial2D::getlinestrokethickness_lsplot() const {
  return pen().widthF();
}

bool LineSpecial2D::getlinefillstatus_lsplot() const {
  if (brush().style() == Qt::NoBrush) {
    return false;
  } else {
    return true;
  }
}

QColor LineSpecial2D::getlinefillcolor_lsplot() const {
  return brush().color();
}

Qt::BrushStyle LineSpecial2D::getlinefillstyle_lsplot() const {
  return brush().style();
}

bool LineSpecial2D::getlineantialiased_lsplot() const { return antialiased(); }

Graph2DCommon::ScatterStyle LineSpecial2D::getscattershape_lsplot() const {
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

QColor LineSpecial2D::getscatterfillcolor_lsplot() const {
  return scatterStyle().brush().color();
}

double LineSpecial2D::getscattersize_lsplot() const {
  return scatterStyle().size();
}

Qt::PenStyle LineSpecial2D::getscatterstrokestyle_lsplot() const {
  return scatterStyle().pen().style();
}

QColor LineSpecial2D::getscatterstrokecolor_lsplot() const {
  return scatterStyle().pen().color();
}

double LineSpecial2D::getscatterstrokethickness_lsplot() const {
  return scatterStyle().pen().widthF();
}

bool LineSpecial2D::getscatterantialiased_lsplot() const {
  return antialiasedScatters();
}

bool LineSpecial2D::getlegendvisible_lsplot() const {
  return mParentPlot->legend->hasItemWithPlottable(this);
}

QString LineSpecial2D::getlegendtext_lsplot() const { return name(); }

Axis2D *LineSpecial2D::getxaxis() const { return xAxis_; }

Axis2D *LineSpecial2D::getyaxis() const { return yAxis_; }

void LineSpecial2D::setlinetype_lsplot(
    const Graph2DCommon::LineStyleType &line) {
  switch (line) {
    case Graph2DCommon::LineStyleType::Impulse:
      setLineStyle(QCPGraph::lsImpulse);
      break;
    case Graph2DCommon::LineStyleType::StepCenter:
      setLineStyle(QCPGraph::lsStepCenter);
      break;
    case Graph2DCommon::LineStyleType::StepLeft:
      setLineStyle(QCPGraph::lsStepLeft);
      break;
    case Graph2DCommon::LineStyleType::StepRight:
      setLineStyle(QCPGraph::lsStepRight);
      break;
    case Graph2DCommon::LineStyleType::Line:
      setLineStyle(QCPGraph::lsLine);
      break;
  }
  reloadIcon();
}

void LineSpecial2D::setlinestrokestyle_lsplot(const Qt::PenStyle &style) {
  QPen p = pen();
  p.setStyle(style);
  setPen(p);
}

void LineSpecial2D::setlinestrokecolor_lsplot(const QColor &color) {
  QPen p = pen();
  p.setColor(color);
  setPen(p);
}

void LineSpecial2D::setlinestrokethickness_lsplot(const double value) {
  QPen p = pen();
  p.setWidthF(value);
  setPen(p);
}

void LineSpecial2D::setlinefillstatus_lsplot(bool status) {
  if (status) {
    QBrush b = brush();
    b.setStyle(Qt::SolidPattern);
    setBrush(b);
  } else {
    QBrush b = brush();
    b.setStyle(Qt::NoBrush);
    setBrush(b);
  }
  reloadIcon();
}

void LineSpecial2D::setlinefillcolor_lsplot(const QColor &color) {
  QBrush b = brush();
  b.setColor(color);
  setBrush(b);
}

void LineSpecial2D::setlinefillstyle_lsplot(const Qt::BrushStyle &style) {
  QBrush b = brush();
  if (b.style() != Qt::BrushStyle::NoBrush) {
    b.setStyle(style);
    setBrush(b);
  }
}

void LineSpecial2D::setlineantialiased_lsplot(const bool value) {
  setAntialiased(value);
}

void LineSpecial2D::setscattershape_lsplot(
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
  setScatterStyle(*scatterstyle_);
}

void LineSpecial2D::setscatterfillcolor_lsplot(const QColor &color) {
  QBrush b = scatterstyle_->brush();
  b.setColor(color);
  scatterstyle_->setBrush(b);
  setScatterStyle(*scatterstyle_);
}

void LineSpecial2D::setscattersize_lsplot(const double value) {
  scatterstyle_->setSize(value);
  setScatterStyle(*scatterstyle_);
}

void LineSpecial2D::setscatterstrokestyle_lsplot(const Qt::PenStyle &style) {
  QPen p = scatterstyle_->pen();
  p.setStyle(style);
  scatterstyle_->setPen(p);
  setScatterStyle(*scatterstyle_);
}

void LineSpecial2D::setscatterstrokecolor_lsplot(const QColor &color) {
  QPen p = scatterstyle_->pen();
  p.setColor(color);
  scatterstyle_->setPen(p);
  setScatterStyle(*scatterstyle_);
}

void LineSpecial2D::setscatterstrokethickness_lsplot(const double value) {
  QPen p = scatterstyle_->pen();
  p.setWidthF(value);
  scatterstyle_->setPen(p);
  setScatterStyle(*scatterstyle_);
}

void LineSpecial2D::setscatterantialiased_lsplot(const bool value) {
  setAntialiasedScatters(value);
}

void LineSpecial2D::setlegendvisible_lsplot(const bool value) {
  (value) ? addToLegend() : removeFromLegend();
}

void LineSpecial2D::setlegendtext_lsplot(const QString &legendtext) {
  setName(legendtext);
}

void LineSpecial2D::setxaxis_lsplot(Axis2D *axis) {
  Q_ASSERT(axis->getorientation_axis() == Axis2D::AxisOreantation::Bottom ||
           axis->getorientation_axis() == Axis2D::AxisOreantation::Top);
  if (axis == getxaxis()) return;

  xAxis_ = axis;
  setKeyAxis(axis);
}

void LineSpecial2D::setyaxis_lsplot(Axis2D *axis) {
  Q_ASSERT(axis->getorientation_axis() == Axis2D::AxisOreantation::Left ||
           axis->getorientation_axis() == Axis2D::AxisOreantation::Right);
  if (axis == getyaxis()) return;

  yAxis_ = axis;
  setValueAxis(axis);
}

void LineSpecial2D::save(XmlStreamWriter *xmlwriter, int xaxis, int yaxis) {
  xmlwriter->writeStartElement("linespecial");
  // axis
  xmlwriter->writeAttribute("xaxis", QString::number(xaxis));
  xmlwriter->writeAttribute("yaxis", QString::number(yaxis));

  (getlegendvisible_lsplot())
      ? xmlwriter->writeAttribute("legendvisible", "true")
      : xmlwriter->writeAttribute("legendvisible", "false");
  xmlwriter->writeAttribute("legend", getlegendtext_lsplot());
  // data
  xmlwriter->writeAttribute("table", graphdata_->gettable()->name());
  xmlwriter->writeAttribute("xcolumn", graphdata_->getxcolumn()->name());
  xmlwriter->writeAttribute("ycolumn", graphdata_->getycolumn()->name());
  xmlwriter->writeAttribute("from", QString::number(graphdata_->getfrom()));
  xmlwriter->writeAttribute("to", QString::number(graphdata_->getto()));
  // error bar
  if (xerroravailable_) xerrorbar_->save(xmlwriter);
  if (yerroravailable_) yerrorbar_->save(xmlwriter);
  // line
  xmlwriter->writeStartElement("line");
  switch (getlinetype_lsplot()) {
    case Graph2DCommon::LineStyleType::Line:
      xmlwriter->writeAttribute("style", "line");
      break;
    case Graph2DCommon::LineStyleType::Impulse:
      xmlwriter->writeAttribute("style", "impulse");
      break;
    case Graph2DCommon::LineStyleType::StepLeft:
      xmlwriter->writeAttribute("style", "stepleft");
      break;
    case Graph2DCommon::LineStyleType::StepRight:
      xmlwriter->writeAttribute("style", "stepright");
      break;
    case Graph2DCommon::LineStyleType::StepCenter:
      xmlwriter->writeAttribute("style", "stepcenter");
      break;
  }
  (getlinefillstatus_lsplot()) ? xmlwriter->writeAttribute("fill", "true")
                               : xmlwriter->writeAttribute("fill", "false");
  (getlineantialiased_lsplot())
      ? xmlwriter->writeAttribute("antialias", "true")
      : xmlwriter->writeAttribute("antialias", "false");
  xmlwriter->writePen(pen());
  xmlwriter->writeBrush(brush());
  xmlwriter->writeEndElement();

  // scatter
  xmlwriter->writeStartElement("scatter");
  switch (getscattershape_lsplot()) {
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
  xmlwriter->writeAttribute("size", QString::number(getscattersize_lsplot()));
  (getscatterantialiased_lsplot())
      ? xmlwriter->writeAttribute("antialias", "true")
      : xmlwriter->writeAttribute("antialias", "false");
  xmlwriter->writePen(scatterstyle_->pen());
  xmlwriter->writeBrush(scatterstyle_->brush());
  xmlwriter->writeEndElement();
  xmlwriter->writeEndElement();
}

bool LineSpecial2D::load(XmlStreamReader *xmlreader) {
  bool ok;
  while (!xmlreader->atEnd()) {
    if (xmlreader->isEndElement() && xmlreader->name() == "linespecial") break;

    // line
    if (xmlreader->isStartElement() && xmlreader->name() == "line") {
      // line style
      QString style = xmlreader->readAttributeString("style", &ok);
      if (ok) {
        if (style == "line") {
          setlinetype_lsplot(Graph2DCommon::LineStyleType::Line);
        } else if (style == "impulse") {
          setlinetype_lsplot(Graph2DCommon::LineStyleType::Impulse);
        } else if (style == "stepleft") {
          setlinetype_lsplot(Graph2DCommon::LineStyleType::StepLeft);
        } else if (style == "stepright") {
          setlinetype_lsplot(Graph2DCommon::LineStyleType::StepRight);
        } else if (style == "stepcenter") {
          setlinetype_lsplot(Graph2DCommon::LineStyleType::StepCenter);
        }
      } else
        xmlreader->raiseWarning(
            tr("LineSpecial2D line style property setting error"));

      // line fill status
      bool fill = xmlreader->readAttributeBool("fill", &ok);
      (ok) ? setlinefillstatus_lsplot(fill)
           : xmlreader->raiseWarning(
                 tr("LineSpecial2D line fill status property setting error"));

      // line antialias
      bool lineantialias = xmlreader->readAttributeBool("antialias", &ok);
      (ok) ? setAntialiased(lineantialias)
           : xmlreader->raiseWarning(
                 tr("LineSpecial2D line antialias property setting error"));

      // line pen property
      while (!xmlreader->atEnd()) {
        xmlreader->readNext();
        if (xmlreader->isEndElement() && xmlreader->name() == "pen") break;
        // pen
        if (xmlreader->isStartElement() && xmlreader->name() == "pen") {
          QPen strokep = xmlreader->readPen(&ok);
          if (ok) {
            setlinestrokecolor_lsplot(strokep.color());
            setlinestrokestyle_lsplot(strokep.style());
            setlinestrokethickness_lsplot(strokep.widthF());
          } else
            xmlreader->raiseWarning(
                tr("LineSpecial2D line pen property setting error"));
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
            setlinefillcolor_lsplot(b.color());
            setlinefillstyle_lsplot(b.style());
          } else
            xmlreader->raiseWarning(
                tr("LineSpecial2D linebrush property setting error"));
        }
      }
    }

    // scatter
    if (xmlreader->isStartElement() && xmlreader->name() == "scatter") {
      // scatter shape
      QString scattershape = xmlreader->readAttributeString("style", &ok);
      if (ok) {
        if (scattershape == "dot") {
          setscattershape_lsplot(Graph2DCommon::ScatterStyle::Dot);
        } else if (scattershape == "disc") {
          setscattershape_lsplot(Graph2DCommon::ScatterStyle::Disc);
        } else if (scattershape == "none") {
          setscattershape_lsplot(Graph2DCommon::ScatterStyle::None);
        } else if (scattershape == "plus") {
          setscattershape_lsplot(Graph2DCommon::ScatterStyle::Plus);
        } else if (scattershape == "star") {
          setscattershape_lsplot(Graph2DCommon::ScatterStyle::Star);
        } else if (scattershape == "cross") {
          setscattershape_lsplot(Graph2DCommon::ScatterStyle::Cross);
        } else if (scattershape == "peace") {
          setscattershape_lsplot(Graph2DCommon::ScatterStyle::Peace);
        } else if (scattershape == "circle") {
          setscattershape_lsplot(Graph2DCommon::ScatterStyle::Circle);
        } else if (scattershape == "square") {
          setscattershape_lsplot(Graph2DCommon::ScatterStyle::Square);
        } else if (scattershape == "diamond") {
          setscattershape_lsplot(Graph2DCommon::ScatterStyle::Diamond);
        } else if (scattershape == "triangle") {
          setscattershape_lsplot(Graph2DCommon::ScatterStyle::Triangle);
        } else if (scattershape == "pluscircle") {
          setscattershape_lsplot(Graph2DCommon::ScatterStyle::PlusCircle);
        } else if (scattershape == "plussquare") {
          setscattershape_lsplot(Graph2DCommon::ScatterStyle::PlusSquare);
        } else if (scattershape == "crosscircle") {
          setscattershape_lsplot(Graph2DCommon::ScatterStyle::CrossCircle);
        } else if (scattershape == "crosssquare") {
          setscattershape_lsplot(Graph2DCommon::ScatterStyle::CrossSquare);
        } else if (scattershape == "triangleinverted") {
          setscattershape_lsplot(Graph2DCommon::ScatterStyle::TriangleInverted);
        }
      } else
        xmlreader->raiseWarning(
            tr("LineSpecial2D scatter shape property setting error"));

      // scatter size
      int scattersize = xmlreader->readAttributeInt("size", &ok);
      (ok) ? setscattersize_lsplot(scattersize)
           : xmlreader->raiseWarning(
                 tr("LineSpecial2D scatter size property setting error"));

      // scatter antialias
      bool scatterantialias = xmlreader->readAttributeBool("antialias", &ok);
      (ok) ? setscatterantialiased_lsplot(scatterantialias)
           : xmlreader->raiseWarning(
                 tr("LineSpecialD scatter antialias property setting error"));

      // scatter pen property
      while (!xmlreader->atEnd()) {
        xmlreader->readNext();
        if (xmlreader->isEndElement() && xmlreader->name() == "pen") break;
        // pen
        if (xmlreader->isStartElement() && xmlreader->name() == "pen") {
          QPen strokep = xmlreader->readPen(&ok);
          if (ok) {
            setscatterstrokecolor_lsplot(strokep.color());
            setscatterstrokestyle_lsplot(strokep.style());
            setscatterstrokethickness_lsplot(strokep.widthF());
          } else
            xmlreader->raiseWarning(
                tr("LineSpecial2D scatter pen property setting error"));
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
            setscatterfillcolor_lsplot(b.color());
          } else
            xmlreader->raiseWarning(
                tr("LineSpecial2D scatterbrush property setting error"));
        }
      }
    }
    xmlreader->readNext();
  }

  return !xmlreader->hasError();
}

void LineSpecial2D::mousePressEvent(QMouseEvent *event,
                                    const QVariant &details) {
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
  QCPGraph::mousePressEvent(event, details);
}

void LineSpecial2D::datapicker(QMouseEvent *event, const QVariant &details) {
  QCPGraphDataContainer::const_iterator it;
  QCPDataSelection dataPoints = details.value<QCPDataSelection>();
  if (dataPoints.dataPointCount() > 0) {
    dataPoints.dataRange();
    it = data()->at(dataPoints.dataRange().begin());
    QPointF point = coordsToPixels(it->mainKey(), it->mainValue());
    if (point.x() > event->localPos().x() - 10 &&
        point.x() < event->localPos().x() + 10 &&
        point.y() > event->localPos().y() - 10 &&
        point.y() < event->localPos().y() + 10)
      xAxis_->getaxisrect_axis()->getPickerTool()->showtooltip(
          point, it->mainKey(), it->mainValue(), getxaxis(), getyaxis());
  }
}

void LineSpecial2D::movepicker(QMouseEvent *event, const QVariant &details) {
  QCPGraphDataContainer::const_iterator it;
  QCPDataSelection dataPoints = details.value<QCPDataSelection>();
  if (dataPoints.dataPointCount() > 0) {
    dataPoints.dataRange();
    it = data()->at(dataPoints.dataRange().begin());
    QPointF point = coordsToPixels(it->mainKey(), it->mainValue());
    if (point.x() > event->localPos().x() - 10 &&
        point.x() < event->localPos().x() + 10 &&
        point.y() > event->localPos().y() - 10 &&
        point.y() < event->localPos().y() + 10) {
      xAxis_->getaxisrect_axis()->getPickerTool()->movepickermouspressls(
          this, it->mainKey(), it->mainValue(), getxaxis(), getyaxis());
    }
  }
}

void LineSpecial2D::removepicker(QMouseEvent *event, const QVariant &details) {
  QCPGraphDataContainer::const_iterator it;
  QCPDataSelection dataPoints = details.value<QCPDataSelection>();
  if (dataPoints.dataPointCount() > 0) {
    dataPoints.dataRange();
    it = data()->at(dataPoints.dataRange().begin());
    QPointF point = coordsToPixels(it->mainKey(), it->mainValue());
    if (point.x() > event->localPos().x() - 10 &&
        point.x() < event->localPos().x() + 10 &&
        point.y() > event->localPos().y() - 10 &&
        point.y() < event->localPos().y() + 10) {
      graphdata_->removedatafromtable(it->mainKey(), it->mainValue());
    }
  }
}

void LineSpecial2D::reloadIcon() {
  switch (getlinetype_lsplot()) {
    case Graph2DCommon::LineStyleType::Impulse:
      icon_ = IconLoader::load("graph2d-vertical-drop", IconLoader::LightDark);
      break;
    case Graph2DCommon::LineStyleType::StepCenter:
      (getlinefillstatus_lsplot())
          ? icon_ = IconLoader::load("graph2d-area", IconLoader::LightDark)
          : icon_ = IconLoader::load("graph2d-vertical-step",
                                     IconLoader::LightDark);
      break;
    case Graph2DCommon::LineStyleType::StepLeft:
      (getlinefillstatus_lsplot())
          ? icon_ = IconLoader::load("graph2d-area", IconLoader::LightDark)
          : icon_ = IconLoader::load("graph2d-horizontal-step",
                                     IconLoader::LightDark);
      break;
    case Graph2DCommon::LineStyleType::StepRight:
      (getlinefillstatus_lsplot())
          ? icon_ = IconLoader::load("graph2d-area", IconLoader::LightDark)
          : icon_ = IconLoader::load("graph2d-vertical-step",
                                     IconLoader::LightDark);
      break;
    case Graph2DCommon::LineStyleType::Line:
      (getlinefillstatus_lsplot())
          ? icon_ = IconLoader::load("graph2d-area", IconLoader::LightDark)
          : icon_ = IconLoader::load("graph2d-line", IconLoader::LightDark);
      break;
  }
}
