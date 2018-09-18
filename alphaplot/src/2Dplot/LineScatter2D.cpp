#include "LineScatter2D.h"
#include <QApplication>
#include <QCursor>
#include <QCursorShape>
#include <QCustomEvent>
#include "../future/core/column/Column.h"
#include "PlotPoint.h"
#include "core/Utilities.h"

LineScatter2D::LineScatter2D(Axis2D *xAxis, Axis2D *yAxis)
    : QCPGraph(xAxis, yAxis),
      xAxis_(xAxis),
      yAxis_(yAxis),
      scatterstyle_(new QCPScatterStyle(
          QCPScatterStyle::ssDisc,
          Utilities::getRandColorGoldenRatio(Utilities::ColorPal::Dark),
          Utilities::getRandColorGoldenRatio(Utilities::ColorPal::Dark), 6.0))
// mPointUnderCursor(new PlotPoint(parentPlot(), 5))
{
  setlinestrokecolor_lsplot(
      Utilities::getRandColorGoldenRatio(Utilities::ColorPal::Dark));
}

LineScatter2D::~LineScatter2D() { delete scatterstyle_; }

void LineScatter2D::setGraphData(Column *xData, Column *yData, int from,
                                 int to) {
  QSharedPointer<QCPGraphDataContainer> graphData(new QCPGraphDataContainer);
  QVector<QCPGraphData> *gdvector = new QVector<QCPGraphData>();
  double xdata = 0, ydata = 0;

  // strip unused end rows
  int end_row = to;
  if (end_row >= xData->rowCount()) end_row = xData->rowCount() - 1;
  if (end_row >= yData->rowCount()) end_row = yData->rowCount() - 1;

  // determine rows for which all columns have valid content
  QList<int> valid_rows;
  for (int row = from; row <= end_row; row++) {
    bool all_valid = true;

    if (xData->isInvalid(row) || yData->isInvalid(row)) {
      all_valid = false;
    }

    if (all_valid) valid_rows.push_back(row);
  }

  for (int i = 0; i < valid_rows.size(); i++) {
    xdata = xData->valueAt(valid_rows.at(i));
    ydata = yData->valueAt(valid_rows.at(i));
    QCPGraphData gd(xdata, ydata);
    gdvector->append(gd);
  }
  graphData->add(*gdvector, true);
  gdvector->clear();
  delete gdvector;
  setData(graphData);
}

void LineScatter2D::setGraphData(QVector<double> *xdata,
                                 QVector<double> *ydata) {
  Q_ASSERT(xdata->size() == ydata->size());

  QSharedPointer<QCPGraphDataContainer> functionData(new QCPGraphDataContainer);
  for (int i = 0; i < xdata->size(); i++) {
    QCPGraphData fd;
    fd.key = xdata->at(i);
    fd.value = ydata->at(i);
    functionData->add(fd);
  }
  setData(functionData);
  // free those containers
  delete xdata;
  delete ydata;
}

LSCommon::LineStyleType LineScatter2D::getlinetype_lsplot() const {
  LSCommon::LineStyleType linestyletype;

  switch (lineStyle()) {
    case lsNone:
      linestyletype = LSCommon::LineStyleType::None;
      break;
    case lsLine:
      linestyletype = LSCommon::LineStyleType::Line;
      break;
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
  }
  return linestyletype;
}

Qt::PenStyle LineScatter2D::getlinestrokestyle_lsplot() const {
  return pen().style();
}

QColor LineScatter2D::getlinestrokecolor_lsplot() const {
  return pen().color();
}

double LineScatter2D::getlinestrokethickness_lsplot() const {
  return pen().widthF();
}

bool LineScatter2D::getlinefillstatus_lsplot() const {
  if (brush().style() == Qt::NoBrush) {
    return false;
  } else {
    return true;
  }
}

QColor LineScatter2D::getlinefillcolor_lsplot() const {
  return brush().color();
}

bool LineScatter2D::getlineantialiased_lsplot() const { return antialiased(); }

LSCommon::ScatterStyle LineScatter2D::getscattershape_lsplot() const {
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

QColor LineScatter2D::getscatterfillcolor_lsplot() const {
  return scatterStyle().brush().color();
}

double LineScatter2D::getscattersize_lsplot() const {
  return scatterStyle().size();
}

Qt::PenStyle LineScatter2D::getscatterstrokestyle_lsplot() const {
  return scatterStyle().pen().style();
}

QColor LineScatter2D::getscatterstrokecolor_lsplot() const {
  return scatterStyle().pen().color();
}

double LineScatter2D::getscatterstrokethickness_lsplot() const {
  return scatterStyle().pen().widthF();
}

bool LineScatter2D::getscatterantialiased_lsplot() const {
  return antialiasedScatters();
}

QString LineScatter2D::getlegendtext_lsplot() const { return name(); }

Axis2D *LineScatter2D::getxaxis_lsplot() const { return xAxis_; }

Axis2D *LineScatter2D::getyaxis_lsplot() const { return yAxis_; }

void LineScatter2D::setlinetype_lsplot(const LSCommon::LineStyleType &line) {
  switch (line) {
    case LSCommon::LineStyleType::Line:
      setLineStyle(QCPGraph::lsLine);
      break;
    case LSCommon::LineStyleType::None:
      setLineStyle(QCPGraph::lsNone);
      break;
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

void LineScatter2D::setlinestrokestyle_lsplot(const Qt::PenStyle &style) {
  QPen p = pen();
  p.setStyle(style);
  setPen(p);
}

void LineScatter2D::setlinestrokecolor_lsplot(const QColor &color) {
  QPen p = pen();
  p.setColor(color);
  setPen(p);
}

void LineScatter2D::setlinestrokethickness_lsplot(const double value) {
  QPen p = pen();
  p.setWidthF(value);
  setPen(p);
}

void LineScatter2D::setlinefillstatus_lsplot(bool status) {
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

void LineScatter2D::setlinefillcolor_lsplot(const QColor &color) {
  QBrush b = brush();
  b.setColor(color);
  setBrush(b);
}

void LineScatter2D::setlineantialiased_lsplot(const bool value) {
  setAntialiased(value);
}

void LineScatter2D::setscattershape_lsplot(
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

void LineScatter2D::setscatterfillcolor_lsplot(const QColor &color) {
  QBrush b = scatterstyle_->brush();
  b.setColor(color);
  scatterstyle_->setBrush(b);
  setScatterStyle(*scatterstyle_);
}

void LineScatter2D::setscattersize_lsplot(const double value) {
  scatterstyle_->setSize(value);
  setScatterStyle(*scatterstyle_);
}

void LineScatter2D::setscatterstrokestyle_lsplot(const Qt::PenStyle &style) {
  QPen p = scatterstyle_->pen();
  p.setStyle(style);
  scatterstyle_->setPen(p);
  setScatterStyle(*scatterstyle_);
}

void LineScatter2D::setscatterstrokecolor_lsplot(const QColor &color) {
  QPen p = scatterstyle_->pen();
  p.setColor(color);
  scatterstyle_->setPen(p);
  setScatterStyle(*scatterstyle_);
}

void LineScatter2D::setscatterstrokethickness_lsplot(const double value) {
  QPen p = scatterstyle_->pen();
  p.setWidthF(value);
  scatterstyle_->setPen(p);
  setScatterStyle(*scatterstyle_);
}

void LineScatter2D::setscatterantialiased_lsplot(const bool value) {
  setAntialiasedScatters(value);
}

void LineScatter2D::setlegendtext_lsplot(const QString &legendtext) {
  setName(legendtext);
}

void LineScatter2D::setxaxis_lsplot(Axis2D *axis) {
  Q_ASSERT(axis->getorientation_axis() == Axis2D::AxisOreantation::Bottom ||
           axis->getorientation_axis() == Axis2D::AxisOreantation::Top);
  if (axis == getxaxis_lsplot()) return;

  xAxis_ = axis;
  setKeyAxis(axis);
}

void LineScatter2D::setyaxis_lsplot(Axis2D *axis) {
  Q_ASSERT(axis->getorientation_axis() == Axis2D::AxisOreantation::Left ||
           axis->getorientation_axis() == Axis2D::AxisOreantation::Right);
  if (axis == getyaxis_lsplot()) return;

  yAxis_ = axis;
  setValueAxis(axis);
}

void LineScatter2D::mousePressEvent(QMouseEvent *event,
                                    const QVariant &details) {
  /*if (event->button() == Qt::LeftButton && mPointUnderCursor) {
    // localpos()
    mPointUnderCursor->startMoving(
        event->pos(), event->modifiers().testFlag(Qt::ShiftModifier));
    return;
  }*/

  QCPGraph::mousePressEvent(event, details);
}

void LineScatter2D::mouseMoveEvent(QMouseEvent *event,
                                   const QPointF &startPos) {
  /*if (event->buttons() == Qt::NoButton) {
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
   }*/
  QCPGraph::mouseMoveEvent(event, event->pos());
}
