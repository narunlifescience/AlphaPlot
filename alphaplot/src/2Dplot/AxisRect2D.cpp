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
      break;
    case Axis2D::Bottom:
      axis2D = new Axis2D(this, QCPAxis::atBottom);
      addAxis(QCPAxis::atBottom, axis2D);
      break;
    case Axis2D::Right:
      axis2D = new Axis2D(this, QCPAxis::atRight);
      addAxis(QCPAxis::atRight, axis2D);
      break;
    case Axis2D::Top:
      axis2D = new Axis2D(this, QCPAxis::atTop);
      addAxis(QCPAxis::atTop, axis2D);
      break;
  }
  return axis2D;
}

bool AxisRect2D::removeAxis2D(Axis2D *axis) {
  return removeAxis(static_cast<QCPAxis *>(axis));
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
  QList<QCPAxis *> qcpAxes = axes();
  QList<Axis2D *> axes2D = QList<Axis2D *>();
  foreach (QCPAxis *qcpAxis, qcpAxes) {
    axes2D << static_cast<Axis2D *>(qcpAxis);
  }
  return axes2D;
}

QList<Axis2D *> AxisRect2D::getAxes2D(
    const Axis2D::AxisOreantation &orientation) const {
  QList<QCPAxis *> qcpAxes = QList<QCPAxis *>();
  QList<Axis2D *> axes2D = QList<Axis2D *>();
  switch (orientation) {
    case Axis2D::Left:
      qcpAxes = axes(QCPAxis::atLeft);
      break;
    case Axis2D::Bottom:
      qcpAxes = axes(QCPAxis::atBottom);
      break;
    case Axis2D::Right:
      qcpAxes = axes(QCPAxis::atRight);
      break;
    case Axis2D::Top:
      qcpAxes = axes(QCPAxis::atTop);
      break;
  }

  foreach (QCPAxis *qcpAxis, qcpAxes) {
    axes2D << static_cast<Axis2D *>(qcpAxis);
  }

  return axes2D;
}

LineScatter2D *AxisRect2D::addLineScatter2DPlot(
    const AxisRect2D::LineScatterType &type, QCPDataMap *dataMap, Axis2D *xAxis,
    Axis2D *yAxis) {
  LineScatter2D *lineScatter = new LineScatter2D(xAxis, yAxis);

  switch (type) {
    case Line2D:
      lineScatter->setLineScatter2DPlot(LineScatter2D::LinePlot,
                                        LineScatter2D::ScatterHidden);
      break;
    case Scatter2D:
      lineScatter->setLineScatter2DPlot(LineScatter2D::NonePlot,
                                        LineScatter2D::ScatterVisible);
      break;
    case LineAndScatter2D:
      lineScatter->setLineScatter2DPlot(LineScatter2D::LinePlot,
                                        LineScatter2D::ScatterVisible);
      break;
    case VerticalDropLine2D:
      lineScatter->setLineScatter2DPlot(LineScatter2D::VerticalDropLinePlot,
                                        LineScatter2D::ScatterVisible);
      break;
    case Spline2D:
      break;
    case CentralStepAndScatter2D:
      lineScatter->setLineScatter2DPlot(
          LineScatter2D::CentralStepAndScatterPlot,
          LineScatter2D::ScatterVisible);
      break;
    case HorizontalStep2D:
      lineScatter->setLineScatter2DPlot(LineScatter2D::VerticalStepPlot,
                                        LineScatter2D::ScatterHidden);
      break;
    case VerticalStep2D:
      lineScatter->setLineScatter2DPlot(LineScatter2D::HorizontalStepPlot,
                                        LineScatter2D::ScatterHidden);
      break;
    case Area2D:
      lineScatter->setLineScatter2DPlot(LineScatter2D::AreaPlot,
                                        LineScatter2D::ScatterHidden);
      break;
  }

  lineScatter->setData(dataMap);
  LegendItem2D *legendItem = new LegendItem2D(axisRectLegend_, lineScatter);
  axisRectLegend_->addItem(legendItem);
  connect(legendItem, SIGNAL(legendItemClicked()), SLOT(legendClick()));
  return lineScatter;
}

Bar2D *AxisRect2D::addBox2DPlot(const AxisRect2D::BarType &type,
                                QCPBarDataMap *barDataMap, Axis2D *xAxis,
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

  bar->setData(barDataMap);
  bar->setAntialiased(false);
  bar->setAntialiasedFill(false);
  LegendItem2D *legendItem = new LegendItem2D(axisRectLegend_, bar);
  axisRectLegend_->addItem(legendItem);
  connect(legendItem, SIGNAL(legendItemClicked()), SLOT(legendClick()));
  return bar;
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
  QCPAxisRect::draw(painter);
  if (isAxisRectSelected_) drawSelection(painter);
}

void AxisRect2D::legendClick() { emit AxisRectClicked(this); }
