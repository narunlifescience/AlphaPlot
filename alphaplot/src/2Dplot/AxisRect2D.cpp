/* This file is part of AlphaPlot.
   Copyright 2016, Arun Narayanankutty <n.arun.lifescience@gmail.com>

   AlphaPlot is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   AlphaPlot is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   You should have received a copy of the GNU General Public License
   along with AlphaPlot.  If not, see <http://www.gnu.org/licenses/>.

   Description : axis rect related stuff */

#include "AxisRect2D.h"
#include "../future/core/column/Column.h"
//#include "FunctionDialog.h"
#include "Legend2D.h"
#include "core/Utilities.h"

#include <QMenu>

AxisRect2D::AxisRect2D(QCustomPlot *parent, bool setupDefaultAxis)
    : QCPAxisRect(parent, setupDefaultAxis),
      axisRectBackGround_(Qt::white),
      axisRectLegend_(new Legend2D()),
      isAxisRectSelected_(false) {
  setAxisRectBackground(axisRectBackGround_);
  insetLayout()->addElement(axisRectLegend_, Qt::AlignTop | Qt::AlignLeft);
  insetLayout()->setInsetPlacement(0, QCPLayoutInset::ipFree);
  axisRectLegend_->setLayer("legend");
  connect(axisRectLegend_, SIGNAL(legendClicked()), SLOT(legendClick()));
}

AxisRect2D::~AxisRect2D() {}

void AxisRect2D::setAxisRectBackground(const QBrush &brush) {
  axisRectBackGround_ = brush;
  setBackground(brush);
}

Axis2D *AxisRect2D::addAxis2D(const Axis2D::AxisOreantation &orientation) {
  Axis2D *axis2D = nullptr;
  switch (orientation) {
    case Axis2D::AxisOreantation::Left:
      axis2D = new Axis2D(this, QCPAxis::atLeft);
      addAxis(QCPAxis::atLeft, axis2D);
      break;
    case Axis2D::AxisOreantation::Bottom:
      axis2D = new Axis2D(this, QCPAxis::atBottom);
      addAxis(QCPAxis::atBottom, axis2D);
      break;
    case Axis2D::AxisOreantation::Right:
      axis2D = new Axis2D(this, QCPAxis::atRight);
      addAxis(QCPAxis::atRight, axis2D);
      break;
    case Axis2D::AxisOreantation::Top:
      axis2D = new Axis2D(this, QCPAxis::atTop);
      addAxis(QCPAxis::atTop, axis2D);
      break;
  }
  axes_.append(axis2D);
  emit AxisCreated(axis2D);
  return axis2D;
}

bool AxisRect2D::removeAxis2D(Axis2D *axis) {
  bool status = removeAxis(static_cast<QCPAxis *>(axis));
  if (status) {
    for (int i = 0; i < axes_.size(); i++) {
      if (axes_.at(i) == axis) axes_.removeAt(i);
    }
    emit AxisRemoved(this);
  }
  return status;
}

QBrush AxisRect2D::getAxisRectBackground() const { return axisRectBackGround_; }

Grid2D *AxisRect2D::bindGridTo(Axis2D *axis) {
  switch (axis->getorientation_axis()) {
    case Axis2D::AxisOreantation::Bottom:
    case Axis2D::AxisOreantation::Top:
      delete gridpair_.first.first;
      gridpair_.first.first = nullptr;
      gridpair_.first.second = nullptr;
      gridpair_.first.first = new Grid2D(axis);
      gridpair_.first.second = axis;
      return gridpair_.first.first;
    case Axis2D::AxisOreantation::Left:
    case Axis2D::AxisOreantation::Right:
      delete gridpair_.second.first;
      gridpair_.second.first = nullptr;
      gridpair_.second.second = nullptr;
      gridpair_.second.first = new Grid2D(axis);
      gridpair_.second.second = axis;
      return gridpair_.second.first;
  }
  return nullptr;
}

QList<Axis2D *> AxisRect2D::getAxes2D() const { return axes_; }

QList<Axis2D *> AxisRect2D::getAxes2D(
    const Axis2D::AxisOreantation &orientation) const {
  QList<QCPAxis *> qcpAxes = QList<QCPAxis *>();
  QList<Axis2D *> axes2D = QList<Axis2D *>();
  switch (orientation) {
    case Axis2D::AxisOreantation::Left:
      qcpAxes = axes(QCPAxis::atLeft);
      break;
    case Axis2D::AxisOreantation::Bottom:
      qcpAxes = axes(QCPAxis::atBottom);
      break;
    case Axis2D::AxisOreantation::Right:
      qcpAxes = axes(QCPAxis::atRight);
      break;
    case Axis2D::AxisOreantation::Top:
      qcpAxes = axes(QCPAxis::atTop);
      break;
  }

  foreach (QCPAxis *qcpAxis, qcpAxes) {
    axes2D << static_cast<Axis2D *>(qcpAxis);
  }

  return axes2D;
}

QList<Axis2D *> AxisRect2D::getXAxes2D() const {
  QList<Axis2D *> axes2d = QList<Axis2D *>();
  for (int i = 0; i < axes_.size(); i++) {
    if (axes_.at(i)->getorientation_axis() == Axis2D::AxisOreantation::Bottom ||
        axes_.at(i)->getorientation_axis() == Axis2D::AxisOreantation::Top) {
      axes2d.append(axes_.at(i));
    }
  }
  return axes2d;
}

QList<Axis2D *> AxisRect2D::getYAxes2D() const {
  QList<Axis2D *> axes2d = QList<Axis2D *>();
  for (int i = 0; i < axes_.size(); i++) {
    if (axes_.at(i)->getorientation_axis() == Axis2D::AxisOreantation::Left ||
        axes_.at(i)->getorientation_axis() == Axis2D::AxisOreantation::Right) {
      axes2d.append(axes_.at(i));
    }
  }
  return axes2d;
}

Axis2D *AxisRect2D::getXAxis(int value) {
  QList<Axis2D *> xaxes = getXAxes2D();
  if (value > -1 && value < xaxes.size()) {
    return xaxes.at(value);

  } else {
    return nullptr;
  }
}

Axis2D *AxisRect2D::getYAxis(int value) {
  QList<Axis2D *> yaxes = getYAxes2D();
  if (value > -1 && value < yaxes.size()) {
    return yaxes.at(value);
  } else {
    return nullptr;
  }
}

LineScatter2D *AxisRect2D::addLineScatter2DPlot(
    const AxisRect2D::LineScatterType &type, Column *xData, Column *yData,
    int from, int to, Axis2D *xAxis, Axis2D *yAxis) {
  LineScatter2D *lineScatter = new LineScatter2D(xAxis, yAxis);
  lineScatter->setlinefillcolor_lsplot(
      Utilities::getRandColorGoldenRatio(Utilities::ColorPal::Light));

  switch (type) {
    case Line2D:
      lineScatter->setlinetype_lsplot(LineScatter2D::LineStyleType::Line);
      lineScatter->setscattershape_lsplot(LineScatter2D::ScatterStyle::None);
      lineScatter->setlinefillstatus_lsplot(false);
      lineScatter->setlineantialiased_lsplot(true);
      lineScatter->setscatterantialiased_lsplot(true);
      break;
    case Scatter2D:
      lineScatter->setlinetype_lsplot(LineScatter2D::LineStyleType::None);
      lineScatter->setscattershape_lsplot(LineScatter2D::ScatterStyle::Disc);
      lineScatter->setlinefillstatus_lsplot(false);
      lineScatter->setlineantialiased_lsplot(false);
      lineScatter->setscatterantialiased_lsplot(true);
      break;
    case LineAndScatter2D:
      lineScatter->setlinetype_lsplot(LineScatter2D::LineStyleType::Line);
      lineScatter->setscattershape_lsplot(LineScatter2D::ScatterStyle::Disc);
      lineScatter->setlinefillstatus_lsplot(false);
      lineScatter->setlineantialiased_lsplot(true);
      lineScatter->setscatterantialiased_lsplot(true);
      break;
    case VerticalDropLine2D:
      lineScatter->setlinetype_lsplot(LineScatter2D::LineStyleType::Impulse);
      lineScatter->setscattershape_lsplot(LineScatter2D::ScatterStyle::Disc);
      lineScatter->setlinefillstatus_lsplot(false);
      lineScatter->setlineantialiased_lsplot(false);
      lineScatter->setscatterantialiased_lsplot(true);
      break;
    case CentralStepAndScatter2D:
      lineScatter->setlinetype_lsplot(LineScatter2D::LineStyleType::StepCenter);
      lineScatter->setscattershape_lsplot(LineScatter2D::ScatterStyle::Disc);
      lineScatter->setlinefillstatus_lsplot(false);
      lineScatter->setlineantialiased_lsplot(false);
      lineScatter->setscatterantialiased_lsplot(true);
      break;
    case HorizontalStep2D:
      lineScatter->setlinetype_lsplot(LineScatter2D::LineStyleType::StepRight);
      lineScatter->setscattershape_lsplot(LineScatter2D::ScatterStyle::None);
      lineScatter->setlinefillstatus_lsplot(false);
      lineScatter->setlineantialiased_lsplot(false);
      lineScatter->setscatterantialiased_lsplot(true);
      break;
    case VerticalStep2D:
      lineScatter->setlinetype_lsplot(LineScatter2D::LineStyleType::StepLeft);
      lineScatter->setscattershape_lsplot(LineScatter2D::ScatterStyle::None);
      lineScatter->setlinefillstatus_lsplot(false);
      lineScatter->setlineantialiased_lsplot(false);
      lineScatter->setscatterantialiased_lsplot(true);
      break;
    case Area2D:
      lineScatter->setlinetype_lsplot(LineScatter2D::LineStyleType::Line);
      lineScatter->setscattershape_lsplot(LineScatter2D::ScatterStyle::None);
      lineScatter->setlinefillstatus_lsplot(true);
      lineScatter->setlineantialiased_lsplot(true);
      lineScatter->setscatterantialiased_lsplot(true);
      break;
  }

  lineScatter->setGraphData(xData, yData, from, to);
  LegendItem2D *legendItem = new LegendItem2D(axisRectLegend_, lineScatter);
  axisRectLegend_->addItem(legendItem);
  connect(legendItem, SIGNAL(legendItemClicked()), SLOT(legendClick()));
  lsvec_.append(lineScatter);

  emit LineScatterCreated(lineScatter);
  return lineScatter;
}

Spline2D *AxisRect2D::addSpline2DPlot(Column *xData, Column *yData, int from,
                                      int to, Axis2D *xAxis, Axis2D *yAxis) {
  Spline2D *spline = new Spline2D(xAxis, yAxis);
  spline->setGraphData(xData, yData, from, to);
  LegendItem2D *legendItem = new LegendItem2D(axisRectLegend_, spline);
  axisRectLegend_->addItem(legendItem);
  connect(legendItem, SIGNAL(legendItemClicked()), SLOT(legendClick()));
  return spline;
}

LineScatter2D *AxisRect2D::addLineFunction2DPlot(QVector<double> *xdata,
                                                 QVector<double> *ydata,
                                                 Axis2D *xAxis, Axis2D *yAxis) {
  LineScatter2D *lineScatter = new LineScatter2D(xAxis, yAxis);
  lineScatter->setlinetype_lsplot(LineScatter2D::LineStyleType::Line);
  lineScatter->setscattershape_lsplot(LineScatter2D::ScatterStyle::None);

  lineScatter->setGraphData(xdata, ydata);
  LegendItem2D *legendItem = new LegendItem2D(axisRectLegend_, lineScatter);
  axisRectLegend_->addItem(legendItem);
  connect(legendItem, SIGNAL(legendItemClicked()), SLOT(legendClick()));
  lsvec_.append(lineScatter);

  emit LineScatterCreated(lineScatter);
  return lineScatter;
}

Curve2D *AxisRect2D::addCurveFunction2DPlot(QVector<double> *xdata,
                                            QVector<double> *ydata,
                                            Axis2D *xAxis, Axis2D *yAxis) {
  Curve2D *curve = new Curve2D(xAxis, yAxis);

  curve->setGraphData(xdata, ydata);
  LegendItem2D *legendItem = new LegendItem2D(axisRectLegend_, curve);
  axisRectLegend_->addItem(legendItem);
  connect(legendItem, SIGNAL(legendItemClicked()), SLOT(legendClick()));
  return curve;
}

Bar2D *AxisRect2D::addBox2DPlot(const AxisRect2D::BarType &type, Column *xData,
                                Column *yData, int from, int to, Axis2D *xAxis,
                                Axis2D *yAxis) {
  Bar2D *bar;
  switch (type) {
    case HorizontalBars:
      bar = new Bar2D(yAxis, xAxis);
      break;
    case VerticalBars:
      bar = new Bar2D(xAxis, yAxis);
      break;
  }

  bar->setBarData(xData, yData, from, to);
  bar->setBarWidth(1);
  bar->setAntialiased(false);
  bar->setAntialiasedFill(false);
  LegendItem2D *legendItem = new LegendItem2D(axisRectLegend_, bar);
  axisRectLegend_->addItem(legendItem);
  connect(legendItem, SIGNAL(legendItemClicked()), SLOT(legendClick()));
  barvec_.append(bar);

  BarCreated(bar);
  return bar;
}

Vector2D *AxisRect2D::addVectorPlot(const Vector2D::VectorPlot &vectorplot,
                                    Column *x1Data, Column *y1Data,
                                    Column *x2Data, Column *y2Data, int from,
                                    int to, Axis2D *xAxis, Axis2D *yAxis) {
  Vector2D *vec = new Vector2D(xAxis, yAxis);
  vec->setGraphData(vectorplot, x1Data, y1Data, x2Data, y2Data, from, to);
  LegendItem2D *legendItem = new LegendItem2D(axisRectLegend_, vec);
  axisRectLegend_->addItem(legendItem);
  connect(legendItem, SIGNAL(legendItemClicked()), SLOT(legendClick()));
  return vec;
}

Pie2D *AxisRect2D::addPie2DPlot(Column *xData, int from, int to) {
  // remove all axis
  /*for (int i = 0; i < axes_.size(); i++) {
    removeAxis2D(axes_.at(i));
    qDebug() << axes_.size();
  }*/
  Pie2D *pie = new Pie2D(this);

  pie->setGraphData(xData, from, to);
  // LegendItem2D *legendItem = new LegendItem2D(axisRectLegend_, pie);
  // axisRectLegend_->addItem(legendItem);
  // connect(legendItem, SIGNAL(legendItemClicked()), SLOT(legendClick()));
  return pie;
}

// Should not use for other than populating axis map
QList<Axis2D *> AxisRect2D::getAxesOrientedTo(
    const Axis2D::AxisOreantation &orientation) const {
  QList<QCPAxis *> axesQCP = QList<QCPAxis *>();
  QList<Axis2D *> axes2D = QList<Axis2D *>();

  switch (orientation) {
    case Axis2D::AxisOreantation::Left:
      axesQCP = axes(QCPAxis::atLeft);
      break;
    case Axis2D::AxisOreantation::Bottom:
      axesQCP = axes(QCPAxis::atBottom);
      break;
    case Axis2D::AxisOreantation::Right:
      axesQCP = axes(QCPAxis::atRight);
      break;
    case Axis2D::AxisOreantation::Top:
      axesQCP = axes(QCPAxis::atTop);
      break;
  }

  foreach (QCPAxis *axisQCP, axesQCP) {
    axes2D.append(static_cast<Axis2D *>(axisQCP));
  }
  return axes2D;
}

void AxisRect2D::updateLegendRect() {
  axisRectLegend_->setMaximumSize(axisRectLegend_->minimumOuterSizeHint());
}

void AxisRect2D::setSelected(const bool status) {
  isAxisRectSelected_ = status;
}

void AxisRect2D::drawSelection(QCPPainter *painter) {
  QPolygon poly;
  poly << QPoint(topRight().x() - 16, topRight().y())
       << QPoint(topRight().x(), topRight().y())
       << QPoint(topRight().x(), topRight().y() + 16);
  painter->setBrush(QBrush(QColor(255, 0, 0, 100)));
  painter->setPen(QPen(Qt::NoPen));
  painter->drawPolygon(poly);
}

void AxisRect2D::mousePressEvent(QMouseEvent *, const QVariant &) {
  emit AxisRectClicked(this);
}

void AxisRect2D::mouseReleaseEvent(QMouseEvent *event,
                                   const QPointF &startPos) {
  if (event->button() == Qt::RightButton) {
    QMenu *menu = new QMenu();
    menu->setAttribute(Qt::WA_DeleteOnClose);
    menu->addAction("Add/Remove plot...", this, SLOT(removeSelectedGraph()));
    menu->addAction("Add Function plot...", this, SLOT(addfunctionplot()));
    menu->addAction("Analyze", this, SLOT(addfunctionplot()));
    menu->addSeparator();
    menu->addAction("Copy", this, SLOT(addfunctionplot()));
    menu->addAction("Export", this, SLOT(addfunctionplot()));
    menu->addAction("Print", this, SLOT(addfunctionplot()));
    menu->popup(parentPlot()->mapToGlobal(QPoint(
        static_cast<int>(startPos.x()), static_cast<int>(startPos.y()))));
  }
}

void AxisRect2D::draw(QCPPainter *painter) {
  QCPAxisRect::draw(painter);
  if (isAxisRectSelected_) drawSelection(painter);
}

void AxisRect2D::legendClick() { emit AxisRectClicked(this); }

void AxisRect2D::addfunctionplot() {
  /*FunctionDialog *fd = new FunctionDialog();
  fd->setAttribute(Qt::WA_DeleteOnClose);
  fd->setWindowTitle(tr("Edit function"));
  fd->show();
  fd->activateWindow();*/
}

void AxisRect2D::addplot() {}
