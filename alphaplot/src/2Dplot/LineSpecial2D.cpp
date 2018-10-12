#include "LineSpecial2D.h"
#include <QApplication>
#include <QCursor>
#include <QCursorShape>
#include <QCustomEvent>
#include "../future/core/column/Column.h"
#include "DataManager2D.h"
#include "ErrorBar2D.h"
//#include "PlotPoint.h"
#include "core/Utilities.h"

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
      xerroravailable_(false),
      yerroravailable_(false),
      picker_(Graph2DCommon::Picker::None)
// mPointUnderCursor(new PlotPoint(parentPlot(), 5))
{
  layer()->setMode(QCPLayer::LayerMode::lmBuffered);
  setSelectable(QCP::SelectionType::stSingleData);
  setlinestrokecolor_lsplot(
      Utilities::getRandColorGoldenRatio(Utilities::ColorPal::Dark));
  setData(graphdata_->data());
}

LineSpecial2D::~LineSpecial2D() {
  delete scatterstyle_;
  delete graphdata_;
  if (xerroravailable_) removeXerrorBar();
  if (yerroravailable_) removeYerrorBar();
}

void LineSpecial2D::setXerrorBar(Table *table, Column *errorcol, int from,
                                 int to) {
  xerrorbar_ = new ErrorBar2D(table, errorcol, from, to, xAxis_, yAxis_,
                              QCPErrorBars::ErrorType::etKeyError, this);
  xerroravailable_ = true;
}

void LineSpecial2D::setYerrorBar(Table *table, Column *errorcol, int from,
                                 int to) {
  yerrorbar_ = new ErrorBar2D(table, errorcol, from, to, xAxis_, yAxis_,
                              QCPErrorBars::ErrorType::etValueError, this);
  yerroravailable_ = true;
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
}

void LineSpecial2D::removeYerrorBar() {
  if (!yerroravailable_) return;

  parentPlot()->removePlottable(yerrorbar_);
  yerrorbar_ = nullptr;
  yerroravailable_ = false;
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
    case lsNone:
    case lsLine:
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

QString LineSpecial2D::getlegendtext_lsplot() const { return name(); }

Axis2D *LineSpecial2D::getxaxis_lsplot() const { return xAxis_; }

Axis2D *LineSpecial2D::getyaxis_lsplot() const { return yAxis_; }

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
  }
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
}

void LineSpecial2D::setlinefillcolor_lsplot(const QColor &color) {
  QBrush b = brush();
  b.setColor(color);
  setBrush(b);
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

void LineSpecial2D::setlegendtext_lsplot(const QString &legendtext) {
  setName(legendtext);
}

void LineSpecial2D::setxaxis_lsplot(Axis2D *axis) {
  Q_ASSERT(axis->getorientation_axis() == Axis2D::AxisOreantation::Bottom ||
           axis->getorientation_axis() == Axis2D::AxisOreantation::Top);
  if (axis == getxaxis_lsplot()) return;

  xAxis_ = axis;
  setKeyAxis(axis);
}

void LineSpecial2D::setyaxis_lsplot(Axis2D *axis) {
  Q_ASSERT(axis->getorientation_axis() == Axis2D::AxisOreantation::Left ||
           axis->getorientation_axis() == Axis2D::AxisOreantation::Right);
  if (axis == getyaxis_lsplot()) return;

  yAxis_ = axis;
  setValueAxis(axis);
}

void LineSpecial2D::setpicker_lsplot(const Graph2DCommon::Picker picker) {
  picker_ = picker;
}

void LineSpecial2D::mousePressEvent(QMouseEvent *event,
                                    const QVariant &details) {
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
  QCPGraph::mousePressEvent(event, details);
}

void LineSpecial2D::datapicker(QMouseEvent *event, const QVariant &details) {
  QCPGraphDataContainer::const_iterator it = data()->constEnd();
  QCPDataSelection dataPoints = details.value<QCPDataSelection>();
  if (dataPoints.dataPointCount() > 0) {
    dataPoints.dataRange();
    it = data()->at(dataPoints.dataRange().begin());
    QPointF point = coordsToPixels(it->mainKey(), it->mainValue());
    if (point.x() > event->posF().x() - 10 &&
        point.x() < event->posF().x() + 10 &&
        point.y() > event->posF().y() - 10 &&
        point.y() < event->posF().y() + 10)
      emit showtooltip(point, it->mainKey(), it->mainValue());
  }
}

void LineSpecial2D::graphpicker(QMouseEvent *event, const QVariant &details) {
  double xvalue, yvalue;
  pixelsToCoords(event->posF(), xvalue, yvalue);
  emit showtooltip(event->posF(), xvalue, yvalue);
}

void LineSpecial2D::movepicker(QMouseEvent *event, const QVariant &details) {}

void LineSpecial2D::removepicker(QMouseEvent *event, const QVariant &details) {
  QCPGraphDataContainer::const_iterator it = data()->constEnd();
  QCPDataSelection dataPoints = details.value<QCPDataSelection>();
  if (dataPoints.dataPointCount() > 0) {
    dataPoints.dataRange();
    it = data()->at(dataPoints.dataRange().begin());
    QPointF point = coordsToPixels(it->mainKey(), it->mainValue());
    if (point.x() > event->posF().x() - 10 &&
        point.x() < event->posF().x() + 10 &&
        point.y() > event->posF().y() - 10 &&
        point.y() < event->posF().y() + 10) {
      graphdata_->data()->remove(it->mainKey());
      layer()->replot();
    }
  }
}

/*void LineScatter2D::mousePressEvent(QMouseEvent *event,
                                    const QVariant &details) {
  if (event->button() == Qt::LeftButton && mPointUnderCursor) {
    // localpos()
    mPointUnderCursor->startMoving(
        event->pos(), event->modifiers().testFlag(Qt::ShiftModifier));
    return;
  }

  QCPGraph::mousePressEvent(event, details);
}*/

/*void LineSpecial2D::mouseMoveEvent(QMouseEvent *event,
                                   const QPointF &startPos) {
  if (event->buttons() == Qt::NoButton) {
    PlotPoint *plotPoint =
        qobject_cast<PlotPoint *>(parentPlot()->itemAt(event->pos(), true));
    if (plotPoint != mPointUnderCursor) {
      if (mPointUnderCursor == nullptr) {
        // cursor moved from empty space to item
        plotPoint->setActive(true);
        parentPlot()->setCursor(Qt::CursorShape::CrossCursor);
      } else if (plotPoint == nullptr) {
        // cursor move from item to empty space
        qDebug() << "elipse not active";
        mPointUnderCursor->setActive(false);
        parentPlot()->unsetCursor();
      } else {
        // cursor moved from item to item
        qDebug() << "point under cursor";
        mPointUnderCursor->setActive(false);
        plotPoint->setActive(true);
      }
      mPointUnderCursor = plotPoint;
      parentPlot()->replot(QCustomPlot::RefreshPriority::rpImmediateRefresh);
    }
  }
  QCPGraph::mouseMoveEvent(event, event->pos());
}*/
