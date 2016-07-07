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
#include "Legend2D.h"

AxisRect2D::AxisRect2D(QCustomPlot *parent, bool setupDefaultAxis)
    : QCPAxisRect(parent, setupDefaultAxis),
      axisRectBackGround_(Qt::white),
      axisRectLegend_(new Legend2D()),
      isAxisRectSelected_(false) {
  setAxisRectBackground(axisRectBackGround_);
  insetLayout()->addElement(axisRectLegend_, Qt::AlignTop | Qt::AlignLeft);
  insetLayout()->setInsetPlacement(0, QCPLayoutInset::ipFree);
  axisRectLegend_->setLayer("legend");

  // initialize axis2d map
  axises_.insert(Axis2D::Left, QList<Axis2D *>());
  axises_.insert(Axis2D::Bottom, QList<Axis2D *>());
  axises_.insert(Axis2D::Right, QList<Axis2D *>());
  axises_.insert(Axis2D::Top, QList<Axis2D *>());

  // initialize linescatter2D map
  lineScatter_.insert(Line2D, QList<LineScatter2D *>());
  lineScatter_.insert(Scatter2D, QList<LineScatter2D *>());
  lineScatter_.insert(LineAndScatter2D, QList<LineScatter2D *>());
  lineScatter_.insert(VerticalDropLine2D, QList<LineScatter2D *>());
  lineScatter_.insert(Spline2D, QList<LineScatter2D *>());
  lineScatter_.insert(CentralStepAndScatter2D, QList<LineScatter2D *>());
  lineScatter_.insert(HorizontalStep2D, QList<LineScatter2D *>());
  lineScatter_.insert(VerticalStep2D, QList<LineScatter2D *>());

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
    case Axis2D::Left:
      axis2D = new Axis2D(this, QCPAxis::atLeft);
      addAxis(QCPAxis::atLeft, axis2D);
      axises_.insert(Axis2D::Left, getAxesOrientedTo(Axis2D::Left));
      break;
    case Axis2D::Bottom:
      axis2D = new Axis2D(this, QCPAxis::atBottom);
      addAxis(QCPAxis::atBottom, axis2D);
      axises_.insert(Axis2D::Bottom, getAxesOrientedTo(Axis2D::Bottom));
      break;
    case Axis2D::Right:
      axis2D = new Axis2D(this, QCPAxis::atRight);
      addAxis(QCPAxis::atRight, axis2D);
      axises_.insert(Axis2D::Right, getAxesOrientedTo(Axis2D::Right));
      break;
    case Axis2D::Top:
      axis2D = new Axis2D(this, QCPAxis::atTop);
      addAxis(QCPAxis::atTop, axis2D);
      axises_.insert(Axis2D::Top, getAxesOrientedTo(Axis2D::Top));
      break;
  }
  return axis2D;
}

bool AxisRect2D::removeAxis2D(Axis2D *axis) {
  bool status = removeAxis(static_cast<QCPAxis *>(axis));

  if (!status) return false;

  switch (axis->getOrientation()) {
    case Axis2D::Left:
      axises_.insert(Axis2D::Left, getAxesOrientedTo(Axis2D::Left));
      break;
    case Axis2D::Bottom:
      axises_.insert(Axis2D::Bottom, getAxesOrientedTo(Axis2D::Bottom));
      break;
    case Axis2D::Right:
      axises_.insert(Axis2D::Right, getAxesOrientedTo(Axis2D::Right));
      break;
    case Axis2D::Top:
      axises_.insert(Axis2D::Top, getAxesOrientedTo(Axis2D::Top));
      break;
  }
  return status;
}

QBrush AxisRect2D::getAxisRectBackground() const { return axisRectBackGround_; }

Grid2D *AxisRect2D::bindGridTo(Axis2D *axis) {
  switch (axis->getOrientation()) {
    case Axis2D::Bottom:
    case Axis2D::Top:
      delete grids_.first;
      grids_.first = new Grid2D(axis);
      return grids_.first;
    case Axis2D::Left:
    case Axis2D::Right:
      delete grids_.second;
      grids_.second = new Grid2D(axis);
      return grids_.second;
  }
  return nullptr;
}

QList<Axis2D *> AxisRect2D::getAxes2D() const {
  QList<Axis2D *> axes2D = QList<Axis2D *>();
  axes2D << axises_.value(Axis2D::Left) << axises_.value(Axis2D::Bottom)
         << axises_.value(Axis2D::Right) << axises_.value(Axis2D::Top);
  return axes2D;
}

QList<Axis2D *> AxisRect2D::getAxes2D(
    const Axis2D::AxisOreantation &orientation) const {
  QList<Axis2D *> axes2D = QList<Axis2D *>();
  switch (orientation) {
    case Axis2D::Left:
      axes2D = axises_.value(Axis2D::Left);
      break;
    case Axis2D::Bottom:
      axes2D = axises_.value(Axis2D::Bottom);
      break;
    case Axis2D::Right:
      axes2D = axises_.value(Axis2D::Right);
      break;
    case Axis2D::Top:
      axes2D = axises_.value(Axis2D::Top);
      break;
  }
  return axes2D;
}

LineScatter2D *AxisRect2D::addLineScatter2DPlot(
    const AxisRect2D::LineScatterType &type, QCPDataMap *dataMap, Axis2D *xAxis,
    Axis2D *yAxis) {
  LineScatter2D *lineScatter = new LineScatter2D(xAxis, yAxis);
  QList<LineScatter2D *> list;

  switch (type) {
    case Line2D:
      lineScatter->setLineScatter2DPlot(LineScatter2D::LinePlot,
                                        LineScatter2D::ScatterHidden);
      list = lineScatter_.value(Line2D);
      break;
    case Scatter2D:
      lineScatter->setLineScatter2DPlot(LineScatter2D::NonePlot,
                                        LineScatter2D::ScatterVisible);
      list = lineScatter_.value(Scatter2D);
      break;
    case LineAndScatter2D:
      lineScatter->setLineScatter2DPlot(LineScatter2D::LinePlot,
                                        LineScatter2D::ScatterVisible);
      list = lineScatter_.value(LineAndScatter2D);
      break;
    case VerticalDropLine2D:
      lineScatter->setLineScatter2DPlot(LineScatter2D::VerticalDropLinePlot,
                                        LineScatter2D::ScatterVisible);
      list = lineScatter_.value(VerticalDropLine2D);
      break;
    case Spline2D:
      break;
    case CentralStepAndScatter2D:
      lineScatter->setLineScatter2DPlot(
          LineScatter2D::CentralStepAndScatterPlot,
          LineScatter2D::ScatterVisible);
      list = lineScatter_.value(CentralStepAndScatter2D);
      break;
    case HorizontalStep2D:
      lineScatter->setLineScatter2DPlot(LineScatter2D::VerticalStepPlot,
                                        LineScatter2D::ScatterHidden);
      list = lineScatter_.value(HorizontalStep2D);
      break;
    case VerticalStep2D:
      lineScatter->setLineScatter2DPlot(LineScatter2D::HorizontalStepPlot,
                                        LineScatter2D::ScatterHidden);
      list = lineScatter_.value(VerticalStep2D);
      break;
  }

  lineScatter->setData(dataMap);
  LegendItem2D *legendItem = new LegendItem2D(axisRectLegend_, lineScatter);
  axisRectLegend_->addItem(legendItem);
  connect(legendItem, SIGNAL(legendItemClicked()), SLOT(legendClick()));
  list.append(lineScatter);
  lineScatter_.insert(type, list);
  return lineScatter;
}

// Should not use for other than populating axis map
QList<Axis2D *> AxisRect2D::getAxesOrientedTo(
    const Axis2D::AxisOreantation &orientation) const {
  QList<QCPAxis *> axesQCP = QList<QCPAxis *>();
  QList<Axis2D *> axes2D = QList<Axis2D *>();

  switch (orientation) {
    case Axis2D::Left:
      axesQCP = axes(QCPAxis::atLeft);
      break;
    case Axis2D::Bottom:
      axesQCP = axes(QCPAxis::atBottom);
      break;
    case Axis2D::Right:
      axesQCP = axes(QCPAxis::atRight);
      break;
    case Axis2D::Top:
      axesQCP = axes(QCPAxis::atTop);
      break;
  }

  foreach (QCPAxis *axisQCP, axesQCP) {
    axes2D.append(static_cast<Axis2D *>(axisQCP));
  }
  return axes2D;
}

void AxisRect2D::updateLegendRect() {
  axisRectLegend_->setMaximumSize(axisRectLegend_->minimumSizeHint());
}

void AxisRect2D::setSelected(const bool status) {
  isAxisRectSelected_ = status;
}

void AxisRect2D::drawSelection(QCPPainter *painter) {
  QPolygon poly;
  poly << QPoint(topRight().x() - 16, topRight().y())
       << QPoint(topRight().x(), topRight().y())
       << QPoint(topRight().x(), topRight().y() + 16);
  painter->setBrush(QBrush(QColor(255, 0, 0, 150)));
  painter->setPen(QPen(Qt::NoPen));
  painter->drawPolygon(poly);
}

void AxisRect2D::mousePressEvent(QMouseEvent *) { emit AxisRectClicked(this); }

void AxisRect2D::draw(QCPPainter *painter) {
  if (isAxisRectSelected_) drawSelection(painter);
}

void AxisRect2D::legendClick() { emit AxisRectClicked(this); }
