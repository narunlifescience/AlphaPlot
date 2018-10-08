#include "LineSpecial2D.h"
#include <QApplication>
#include <QCursor>
#include <QCursorShape>
#include <QCustomEvent>
#include "../future/core/column/Column.h"
#include "DataManager2D.h"
#include "ErrorBar2D.h"
#include "PlotPoint.h"
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
      yerroravailable_(false)
// mPointUnderCursor(new PlotPoint(parentPlot(), 5))
{
  layer()->setMode(QCPLayer::LayerMode::lmBuffered);
  setlinestrokecolor_lsplot(
      Utilities::getRandColorGoldenRatio(Utilities::ColorPal::Dark));
  setData(graphdata_->data());
}

LineSpecial2D::~LineSpecial2D() {
  delete scatterstyle_;
  delete graphdata_;
  if (xerroravailable_) delete xerrorbar_;
  if (yerroravailable_) delete yerrorbar_;
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
  if (xerroravailable_) return;

  delete xerrorbar_;
  xerrorbar_ = nullptr;
  xerroravailable_ = false;
}

void LineSpecial2D::removeYerrorBar() {
  if (yerroravailable_) return;

  delete yerrorbar_;
  yerrorbar_ = nullptr;
  yerroravailable_ = false;
}

LSCommon::LineStyleType LineSpecial2D::getlinetype_lsplot() const {
  LSCommon::LineStyleType linestyletype;

  switch (lineStyle()) {
    case lsStepLeft:
      linestyletype = LSCommon::LineStyleType::StepLeft;
      break;
    case lsStepRight:
      linestyletype = LSCommon::LineStyleType::StepRight;
      break;
    case lsStepCenter:
      linestyletype = LSCommon::LineStyleType::StepCenter;
      break;
    case lsImpulse:
      linestyletype = LSCommon::LineStyleType::Impulse;
      break;
    case lsNone:
    case lsLine:
      linestyletype = LSCommon::LineStyleType::StepLeft;
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

LSCommon::ScatterStyle LineSpecial2D::getscattershape_lsplot() const {
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

void LineSpecial2D::setlinetype_lsplot(const LSCommon::LineStyleType &line) {
  switch (line) {
    case LSCommon::LineStyleType::Impulse:
      setLineStyle(QCPGraph::lsImpulse);
      break;
    case LSCommon::LineStyleType::StepCenter:
      setLineStyle(QCPGraph::lsStepCenter);
      break;
    case LSCommon::LineStyleType::StepLeft:
      setLineStyle(QCPGraph::lsStepLeft);
      break;
    case LSCommon::LineStyleType::StepRight:
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
    const LSCommon::ScatterStyle &shape) {
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

/*void LineScatter2D::mouseMoveEvent(QMouseEvent *event,
                                   const QPointF &startPos) {
  if (event->buttons() == Qt::NoButton) {
     PlotPoint *plotPoint =
         qobject_cast<PlotPoint *>(parentPlot()->itemAt(event->pos(), true));
     if (plotPoint != mPointUnderCursor) {
       if (mPointUnderCursor == nullptr) {
         // cursor moved from empty space to item
         plotPoint->setActive(true);
         parentPlot()->setCursor(Qt::OpenHandCursor);
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
       parentPlot()->replot();
     }
   }
  QCPGraph::mouseMoveEvent(event, event->pos());
}*/
