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

#include <QMenu>

#include "ColorMap2D.h"
#include "ErrorBar2D.h"
#include "ImageItem2D.h"
#include "Legend2D.h"
#include "LineItem2D.h"
#include "Matrix.h"
#include "Pie2D.h"
#include "QMessageBox"
#include "Table.h"
#include "TextItem2D.h"
#include "core/Utilities.h"
#include "future/core/column/Column.h"
#include "future/lib/XmlStreamWriter.h"

AxisRect2D::AxisRect2D(Plot2D *parent, bool setupDefaultAxis)
    : QCPAxisRect(parent, setupDefaultAxis),
      plot2d_(parent),
      axisRectBackGround_(Qt::white),
      axisRectLegend_(new Legend2D(this)),
      isAxisRectSelected_(false),
      printorexportjob_(false) {
  gridpair_.first.first = nullptr;
  gridpair_.first.second = nullptr;
  gridpair_.second.first = nullptr;
  gridpair_.second.second = nullptr;
  setAxisRectBackground(axisRectBackGround_);
  insetLayout()->addElement(axisRectLegend_, Qt::AlignTop | Qt::AlignLeft);
  insetLayout()->setInsetPlacement(0, QCPLayoutInset::ipFree);
  axisRectLegend_->setLayer(plot2d_->getLegend2DLayerName());
  connect(axisRectLegend_, SIGNAL(legendClicked()), SLOT(legendClick()));
}

AxisRect2D::~AxisRect2D() {
  insetLayout()->take(axisRectLegend_);
  delete axisRectLegend_;
}

void AxisRect2D::setAxisRectBackground(const QBrush &brush) {
  axisRectBackGround_ = brush;
  setBackground(brush);
}

Axis2D *AxisRect2D::addAxis2D(const Axis2D::AxisOreantation &orientation,
                              const Axis2D::TickerType &tickertype) {
  Axis2D *axis2D = nullptr;
  switch (orientation) {
    case Axis2D::AxisOreantation::Left:
      axis2D = new Axis2D(this, QCPAxis::atLeft, tickertype);
      addAxis(QCPAxis::atLeft, axis2D);
      break;
    case Axis2D::AxisOreantation::Bottom:
      axis2D = new Axis2D(this, QCPAxis::atBottom, tickertype);
      addAxis(QCPAxis::atBottom, axis2D);
      break;
    case Axis2D::AxisOreantation::Right:
      axis2D = new Axis2D(this, QCPAxis::atRight, tickertype);
      addAxis(QCPAxis::atRight, axis2D);
      break;
    case Axis2D::AxisOreantation::Top:
      axis2D = new Axis2D(this, QCPAxis::atTop, tickertype);
      addAxis(QCPAxis::atTop, axis2D);
      break;
  }

  axes_.append(axis2D);
  emit Axis2DCreated(axis2D);
  return axis2D;
}

Axis2D *AxisRect2D::addAxis2DifNeeded(Column *col) {
  Axis2D *axis = nullptr;
  Axis2D::TickerType tickertype = Axis2D::TickerType::Value;
  Axis2D::AxisOreantation orientation = Axis2D::AxisOreantation::Bottom;
  switch (col->dataType()) {
    case AlphaPlot::ColumnDataType::TypeDouble:
      tickertype = Axis2D::TickerType::Value;
      break;
    case AlphaPlot::ColumnDataType::TypeString:
      tickertype = Axis2D::TickerType::Text;
      break;
    case AlphaPlot::ColumnDataType::TypeDateTime:
      tickertype = Axis2D::TickerType::DateTime;
      break;
  }

  if (col->plotDesignation() == AlphaPlot::X) {
    QList<Axis2D *> haxes = getAxes2D(Axis2D::Axis2D::AxisOreantation::Bottom);
    haxes.append(getAxes2D(Axis2D::Axis2D::AxisOreantation::Top));
    foreach (Axis2D *axis2d, haxes) {
      if (axis2d->gettickertype_axis() == tickertype) {
        axis = axis2d;
        break;
      }
    }
    if (!axis) {
      int b = 0, t = 0;
      foreach (Axis2D *axis2d, haxes) {
        (axis2d->getorientation_axis() == Axis2D::AxisOreantation::Bottom)
            ? b++
            : t++;
      }
      (t < b) ? axis = addAxis2D(Axis2D::AxisOreantation::Top, tickertype)
              : axis = addAxis2D(Axis2D::AxisOreantation::Bottom, tickertype);
      axis = addAxis2D(Axis2D::AxisOreantation::Bottom, tickertype);
    }
  } else if (col->plotDesignation() == AlphaPlot::Y) {
    QList<Axis2D *> vaxes = getAxes2D(Axis2D::AxisOreantation::Left);
    vaxes.append(getAxes2D(Axis2D::AxisOreantation::Right));
    foreach (Axis2D *axis2d, vaxes) {
      if (axis2d->gettickertype_axis() == tickertype) {
        axis = axis2d;
        break;
      }
    }
    if (!axis) {
      int l = 0, r = 0;
      foreach (Axis2D *axis2d, vaxes) {
        (axis2d->getorientation_axis() == Axis2D::AxisOreantation::Left) ? l++
                                                                         : r++;
      }
      (r < l) ? axis = addAxis2D(Axis2D::AxisOreantation::Right, tickertype)
              : axis = addAxis2D(Axis2D::AxisOreantation::Left, tickertype);
    }
  }
  return axis;
}

bool AxisRect2D::removeAxis2D(Axis2D *axis, bool force) {
  bool status = true;
  foreach (LineSpecial2D *ls, lsvec_) {
    if (ls->getxaxis() == axis || ls->getyaxis() == axis) status = false;
  }

  foreach (Curve2D *curve, curvevec_) {
    if (curve->getxaxis() == axis || curve->getyaxis() == axis) status = false;
  }

  foreach (StatBox2D *statbox, statboxvec_) {
    if (statbox->getxaxis() == axis || statbox->getyaxis() == axis)
      status = false;
  }

  foreach (Vector2D *vec, vectorvec_) {
    if (vec->getxaxis() == axis || vec->getyaxis() == axis) status = false;
  }

  foreach (Bar2D *bar, barvec_) {
    if (bar->getxaxis() == axis || bar->getyaxis() == axis) status = false;
  }

  if (!status) {
    QMessageBox::warning(
        nullptr, tr("Axis associated with plot"),
        tr("This axis is associated with a plot! eithor remove the plot or "
           "change the plot to anothor axis"));
    return false;
  }

  if (gridpair_.first.second == axis) {
    if (!force) {
      status = false;
    } else {
      gridpair_.first.second = nullptr;
    }
  }
  if (gridpair_.second.second == axis) {
    if (!force) {
      status = false;
    } else {
      gridpair_.second.second = nullptr;
    }
  }

  if (!status) {
    QMessageBox::warning(nullptr, tr("Axis associated with grid"),
                         tr("This axis is associated with axis grid! please "
                            "change the associated grid to anothor axis"));
    return false;
  }

  status = removeAxis(static_cast<QCPAxis *>(axis));
  if (status) {
    for (int i = 0; i < axes_.size(); i++) {
      if (axes_.at(i) == axis) axes_.removeAt(i);
    }
    emit Axis2DRemoved(this);
  }
  return status;
}

QBrush AxisRect2D::getAxisRectBackground() const { return axisRectBackGround_; }

Grid2D *AxisRect2D::bindGridTo(Axis2D *axis) {
  Grid2D *grid = nullptr;
  switch (axis->getorientation_axis()) {
    case Axis2D::AxisOreantation::Bottom:
    case Axis2D::AxisOreantation::Top:
      if (gridpair_.first.second == axis) return gridpair_.first.first;
      if (gridpair_.first.first != nullptr) delete gridpair_.first.first;
      gridpair_.first.first = nullptr;
      gridpair_.first.second = nullptr;
      gridpair_.first.first = new Grid2D(axis);
      gridpair_.first.second = axis;
      grid = gridpair_.first.first;
      break;
    case Axis2D::AxisOreantation::Left:
    case Axis2D::AxisOreantation::Right:
      if (gridpair_.second.second == axis) return gridpair_.second.first;
      if (gridpair_.second.first != nullptr) delete gridpair_.second.first;
      gridpair_.second.first = nullptr;
      gridpair_.second.second = nullptr;
      gridpair_.second.first = new Grid2D(axis);
      gridpair_.second.second = axis;
      grid = gridpair_.second.first;
      break;
  }

  if (gridpair_.first.second && gridpair_.second.second)
    setItemAxes(gridpair_.first.second, gridpair_.second.second);

  return grid;
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

int AxisRect2D::getXAxisNo(Axis2D *axis) {
  QList<Axis2D *> xaxes = getXAxes2D();
  if (axis && (axis->getorientation_axis() == Axis2D::AxisOreantation::Bottom ||
               axis->getorientation_axis() == Axis2D::AxisOreantation::Top)) {
    return xaxes.indexOf(axis);
  } else {
    return -1;
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

int AxisRect2D::getYAxisNo(Axis2D *axis) {
  QList<Axis2D *> yaxes = getYAxes2D();
  if (axis && (axis->getorientation_axis() == Axis2D::AxisOreantation::Left ||
               axis->getorientation_axis() == Axis2D::AxisOreantation::Right)) {
    return yaxes.indexOf(axis);
  } else {
    return -1;
  }
}

LineSpecial2D *AxisRect2D::addLineSpecial2DPlot(
    const LineScatterSpecialType &type, Table *table, Column *xData,
    Column *yData, int from, int to, Axis2D *xAxis, Axis2D *yAxis) {
  LineSpecial2D *lineSpecial =
      new LineSpecial2D(table, xData, yData, from, to, xAxis, yAxis);
  lineSpecial->setlinefillcolor_lsplot(
      Utilities::getRandColorGoldenRatio(Utilities::ColorPal::Light));

  switch (type) {
    case LineScatterSpecialType::Area2D:
      lineSpecial->setlinetype_lsplot(Graph2DCommon::LineStyleType::Line);
      lineSpecial->setscattershape_lsplot(Graph2DCommon::ScatterStyle::None);
      lineSpecial->setlinefillstatus_lsplot(true);
      lineSpecial->setlineantialiased_lsplot(true);
      lineSpecial->setscatterantialiased_lsplot(true);
      break;
    case LineScatterSpecialType::VerticalDropLine2D:
      lineSpecial->setlinetype_lsplot(Graph2DCommon::LineStyleType::Impulse);
      lineSpecial->setscattershape_lsplot(Graph2DCommon::ScatterStyle::Disc);
      lineSpecial->setlinefillstatus_lsplot(false);
      lineSpecial->setlineantialiased_lsplot(false);
      lineSpecial->setscatterantialiased_lsplot(true);
      break;
    case LineScatterSpecialType::CentralStepAndScatter2D:
      lineSpecial->setlinetype_lsplot(Graph2DCommon::LineStyleType::StepCenter);
      lineSpecial->setscattershape_lsplot(Graph2DCommon::ScatterStyle::Disc);
      lineSpecial->setlinefillstatus_lsplot(false);
      lineSpecial->setlineantialiased_lsplot(false);
      lineSpecial->setscatterantialiased_lsplot(true);
      break;
    case LineScatterSpecialType::HorizontalStep2D:
      lineSpecial->setlinetype_lsplot(Graph2DCommon::LineStyleType::StepRight);
      lineSpecial->setscattershape_lsplot(Graph2DCommon::ScatterStyle::None);
      lineSpecial->setlinefillstatus_lsplot(false);
      lineSpecial->setlineantialiased_lsplot(false);
      lineSpecial->setscatterantialiased_lsplot(true);
      break;
    case LineScatterSpecialType::VerticalStep2D:
      lineSpecial->setlinetype_lsplot(Graph2DCommon::LineStyleType::StepLeft);
      lineSpecial->setscattershape_lsplot(Graph2DCommon::ScatterStyle::None);
      lineSpecial->setlinefillstatus_lsplot(false);
      lineSpecial->setlineantialiased_lsplot(false);
      lineSpecial->setscatterantialiased_lsplot(true);
      break;
  }

  LegendItem2D *legendItem = new LegendItem2D(axisRectLegend_, lineSpecial);
  axisRectLegend_->addItem(legendItem);
  connect(legendItem, SIGNAL(legendItemClicked()), SLOT(legendClick()));
  lineSpecial->setName(table->name() + "_" + xData->name() + "_" +
                       yData->name());
  lsvec_.append(lineSpecial);
  layers_.append(lineSpecial->layer());
  connect(lineSpecial, SIGNAL(showtooltip(QPointF, double, double)), this,
          SIGNAL(showtooltip(QPointF, double, double)));

  emit LineSpecial2DCreated(lineSpecial);
  return lineSpecial;
}

QPair<LineSpecial2D *, LineSpecial2D *> AxisRect2D::addLineSpecialChannel2DPlot(
    Table *table, Column *xData, Column *yData1, Column *yData2, int from,
    int to, Axis2D *xAxis, Axis2D *yAxis) {
  LineSpecial2D *lineSpecial1 =
      new LineSpecial2D(table, xData, yData1, from, to, xAxis, yAxis);
  QColor color = Utilities::getRandColorGoldenRatio(Utilities::ColorPal::Light);
  color.setAlpha(155);
  lineSpecial1->setlinefillcolor_lsplot(color);
  lineSpecial1->setlinetype_lsplot(Graph2DCommon::LineStyleType::Line);
  lineSpecial1->setscattershape_lsplot(Graph2DCommon::ScatterStyle::None);
  lineSpecial1->setlinefillstatus_lsplot(true);
  lineSpecial1->setlineantialiased_lsplot(true);
  lineSpecial1->setscatterantialiased_lsplot(true);
  LineSpecial2D *lineSpecial2 =
      new LineSpecial2D(table, xData, yData2, from, to, xAxis, yAxis);
  lineSpecial2->setlinetype_lsplot(Graph2DCommon::LineStyleType::Line);
  lineSpecial2->setscattershape_lsplot(Graph2DCommon::ScatterStyle::None);
  lineSpecial2->setlinefillstatus_lsplot(false);
  lineSpecial2->setlineantialiased_lsplot(true);
  lineSpecial2->setscatterantialiased_lsplot(true);
  lineSpecial1->setChannelFillGraph(lineSpecial2);
  lineSpecial1->setlinestrokecolor_lsplot(Qt::darkGray);
  lineSpecial1->setlinestrokestyle_lsplot(Qt::PenStyle::DotLine);
  lineSpecial2->setlinestrokecolor_lsplot(Qt::darkGray);
  lineSpecial2->setlinestrokestyle_lsplot(Qt::PenStyle::DotLine);
  LegendItem2D *legendItem = new LegendItem2D(axisRectLegend_, lineSpecial1);
  axisRectLegend_->addItem(legendItem);
  connect(legendItem, SIGNAL(legendItemClicked()), SLOT(legendClick()));
  lineSpecial1->setName(table->name() + "_" + xData->name() + "_" +
                        yData1->name() + "_" + yData2->name());
  auto pair =
      QPair<LineSpecial2D *, LineSpecial2D *>(lineSpecial1, lineSpecial2);
  channelvec_.append(pair);
  layers_.append(lineSpecial1->layer());
  emit LineSpecialChannel2DCreated(pair);
  return pair;
}

Curve2D *AxisRect2D::addCurve2DPlot(const AxisRect2D::LineScatterType &type,
                                    Table *table, Column *xcol, Column *ycol,
                                    int from, int to, Axis2D *xAxis,
                                    Axis2D *yAxis) {
  Curve2D *curve = nullptr;
  switch (type) {
    case LineScatterType::Line2D:
    case LineScatterType::Scatter2D:
    case LineScatterType::LineAndScatter2D:
      curve = new Curve2D(Curve2D::Curve2DType::Curve, table, xcol, ycol, from,
                          to, xAxis, yAxis);
      break;
    case LineScatterType::Spline2D:
      curve = new Curve2D(Curve2D::Curve2DType::Spline, table, xcol, ycol, from,
                          to, xAxis, yAxis);
      break;
  }

  switch (type) {
    case LineScatterType::Line2D:
      curve->setlinetype_cplot(1);
      curve->setscattershape_cplot(Graph2DCommon::ScatterStyle::None);
      curve->setlinefillstatus_cplot(false);
      break;
    case LineScatterType::Scatter2D:
      curve->setlinetype_cplot(0);
      curve->setscattershape_cplot(Graph2DCommon::ScatterStyle::Disc);
      curve->setlinefillstatus_cplot(false);
      break;
    case LineScatterType::LineAndScatter2D:
      curve->setlinetype_cplot(1);
      curve->setscattershape_cplot(Graph2DCommon::ScatterStyle::Disc);
      curve->setlinefillstatus_cplot(false);
      break;
    case LineScatterType::Spline2D:
      curve->setscattershape_cplot(Graph2DCommon::ScatterStyle::Disc);
      curve->setlinefillstatus_cplot(false);
      break;
  }
  SplineLegendItem2D *legendItem =
      new SplineLegendItem2D(axisRectLegend_, curve);
  axisRectLegend_->addItem(legendItem);
  connect(legendItem, SIGNAL(legendItemClicked()), SLOT(legendClick()));
  curve->setName(table->name() + "_" + xcol->name() + "_" + ycol->name());
  curvevec_.append(curve);
  layers_.append(curve->layer());
  connect(curve, SIGNAL(showtooltip(QPointF, double, double)), this,
          SIGNAL(showtooltip(QPointF, double, double)));

  emit Curve2DCreated(curve);
  return curve;
}

Curve2D *AxisRect2D::addFunction2DPlot(QVector<double> *xdata,
                                       QVector<double> *ydata, Axis2D *xAxis,
                                       Axis2D *yAxis, const QString &name) {
  Curve2D *curve = new Curve2D(xdata, ydata, xAxis, yAxis);
  curve->setlinetype_cplot(1);
  curve->setscattershape_cplot(Graph2DCommon::ScatterStyle::None);

  curve->setName(name);
  LegendItem2D *legendItem = new LegendItem2D(axisRectLegend_, curve);
  axisRectLegend_->addItem(legendItem);
  connect(legendItem, SIGNAL(legendItemClicked()), SLOT(legendClick()));
  layers_.append(curve->layer());
  curvevec_.append(curve);
  connect(curve, SIGNAL(showtooltip(QPointF, double, double)), this,
          SIGNAL(showtooltip(QPointF, double, double)));

  emit Curve2DCreated(curve);
  return curve;
}

Bar2D *AxisRect2D::addBox2DPlot(const AxisRect2D::BarType &type, Table *table,
                                Column *xData, Column *yData, int from, int to,
                                Axis2D *xAxis, Axis2D *yAxis) {
  Bar2D *bar;
  switch (type) {
    case AxisRect2D::BarType::HorizontalBars:
      bar = new Bar2D(table, xData, yData, from, to, yAxis, xAxis);
      break;
    case AxisRect2D::BarType::VerticalBars:
      bar = new Bar2D(table, xData, yData, from, to, xAxis, yAxis);
      break;
  }

  bar->setWidth(1);
  bar->setAntialiased(false);
  bar->setAntialiasedFill(false);
  LegendItem2D *legendItem = new LegendItem2D(axisRectLegend_, bar);
  axisRectLegend_->addItem(legendItem);
  connect(legendItem, SIGNAL(legendItemClicked()), SLOT(legendClick()));
  bar->setName(table->name() + "_" + xData->name() + "_" + yData->name());
  layers_.append(bar->layer());
  barvec_.append(bar);
  connect(bar, SIGNAL(showtooltip(QPointF, double, double)), this,
          SIGNAL(showtooltip(QPointF, double, double)));

  emit Bar2DCreated(bar);
  return bar;
}

Vector2D *AxisRect2D::addVectorPlot(const Vector2D::VectorPlot &vectorplot,
                                    Table *table, Column *x1Data,
                                    Column *y1Data, Column *x2Data,
                                    Column *y2Data, int from, int to,
                                    Axis2D *xAxis, Axis2D *yAxis) {
  Vector2D *vec = new Vector2D(vectorplot, table, x1Data, y1Data, x2Data,
                               y2Data, from, to, xAxis, yAxis);
  VectorLegendItem2D *legendItem = new VectorLegendItem2D(axisRectLegend_, vec);
  axisRectLegend_->addItem(legendItem);
  connect(legendItem, SIGNAL(legendItemClicked()), SLOT(legendClick()));
  vec->setName(table->name() + "_" + x1Data->name() + "_" + y1Data->name() +
               "_" + x2Data->name() + "_" + y2Data->name());
  layers_.append(vec->layer());
  vectorvec_.append(vec);

  emit Vector2DCreated(vec);
  return vec;
}

StatBox2D *AxisRect2D::addStatBox2DPlot(StatBox2D::BoxWhiskerData data,
                                        Axis2D *xAxis, Axis2D *yAxis) {
  StatBox2D *statbox = new StatBox2D(data, xAxis, yAxis);
  LegendItem2D *legendItem = new LegendItem2D(axisRectLegend_, statbox);
  axisRectLegend_->addItem(legendItem);
  connect(legendItem, SIGNAL(legendItemClicked()), SLOT(legendClick()));
  statbox->setName(data.name);
  layers_.append(statbox->layer());
  statboxvec_.append(statbox);
  connect(statbox, SIGNAL(showtooltip(QPointF, double, double)), this,
          SIGNAL(showtooltip(QPointF, double, double)));

  emit StatBox2DCreated(statbox);
  return statbox;
}

Bar2D *AxisRect2D::addHistogram2DPlot(const AxisRect2D::BarType &type,
                                      Table *table, Column *yData, int from,
                                      int to, Axis2D *xAxis, Axis2D *yAxis) {
  Bar2D *bar;
  switch (type) {
    case AxisRect2D::BarType::HorizontalBars:
      bar = new Bar2D(table, yData, from, to, yAxis, xAxis);
      break;
    case AxisRect2D::BarType::VerticalBars:
      bar = new Bar2D(table, yData, from, to, xAxis, yAxis);
      break;
  }
  bar->setAntialiased(false);
  bar->setAntialiasedFill(false);
  LegendItem2D *legendItem = new LegendItem2D(axisRectLegend_, bar);
  axisRectLegend_->addItem(legendItem);
  connect(legendItem, SIGNAL(legendItemClicked()), SLOT(legendClick()));
  bar->setName(table->name() + "_" + yData->name());
  layers_.append(bar->layer());
  barvec_.append(bar);
  connect(bar, SIGNAL(showtooltip(QPointF, double, double)), this,
          SIGNAL(showtooltip(QPointF, double, double)));

  emit Bar2DCreated(bar);
  return bar;
}

Pie2D *AxisRect2D::addPie2DPlot(Table *table, Column *xData, int from, int to) {
  Pie2D *pie = new Pie2D(this, table, xData, from, to);
  pie->setGraphData(table, xData, from, to);
  getLegend()->setVisible(false);
  // connect(legendItem, SIGNAL(legendItemClicked()), SLOT(legendClick()));
  layers_.append(pie->layer());
  pievec_.append(pie);
  emit Pie2DCreated(pie);
  return pie;
}

ColorMap2D *AxisRect2D::addColorMap2DPlot(Matrix *matrix, Axis2D *xAxis,
                                          Axis2D *yAxis) {
  ColorMap2D *colormap = new ColorMap2D(matrix, xAxis, yAxis);
  LegendItem2D *legendItem = new LegendItem2D(axisRectLegend_, colormap);
  axisRectLegend_->addItem(legendItem);
  getLegend()->setVisible(false);
  layers_.append(colormap->layer());
  colormapvec_.append(colormap);
  colormap->setname_colormap(matrix->name());
  emit ColorMap2DCreated(colormap);
  return colormap;
}

TextItem2D *AxisRect2D::addTextItem2D(QString text) {
  TextItem2D *textitem = new TextItem2D(this, plot2d_);
  textitem->position->setAxes(gridpair_.first.second, gridpair_.second.second);
  textitem->setText(text);
  textitem->setpixelposition_textitem(this->rect().center());
  layers_.append(textitem->layer());
  textvec_.append(textitem);
  emit TextItem2DCreated(textitem);
  return textitem;
}

LineItem2D *AxisRect2D::addLineItem2D() {
  LineItem2D *lineitem = new LineItem2D(this, plot2d_);
  foreach (QCPItemPosition *position, lineitem->positions()) {
    position->setAxes(gridpair_.first.second, gridpair_.second.second);
  }
  QRectF rect = this->rect();
  int widthpercent = static_cast<int>((rect.width() * 20) / 100);
  int heightpercent = static_cast<int>((rect.height() * 20) / 100);
  rect.adjust(widthpercent, heightpercent, -widthpercent, -heightpercent);
  lineitem->start->setPixelPosition(rect.topLeft());
  lineitem->end->setPixelPosition(rect.bottomRight());
  layers_.append(lineitem->layer());
  linevec_.append(lineitem);
  emit LineItem2DCreated(lineitem);
  return lineitem;
}

LineItem2D *AxisRect2D::addArrowItem2D() {
  LineItem2D *lineitem = addLineItem2D();
  lineitem->setendstyle_lineitem(LineItem2D::LineEndLocation::Stop,
                                 QCPLineEnding::EndingStyle::esFlatArrow);
  return lineitem;
}

ImageItem2D *AxisRect2D::addImageItem2D(const QString &filename) {
  ImageItem2D *imageitem = new ImageItem2D(this, plot2d_, filename);
  foreach (QCPItemPosition *position, imageitem->positions()) {
    position->setAxes(gridpair_.first.second, gridpair_.second.second);
  }
  layers_.append(imageitem->layer());
  imagevec_.append(imageitem);
  emit ImageItem2DCreated(imageitem);
  return imageitem;
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

void AxisRect2D::selectAxisRect() { emit AxisRectClicked(this); }

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

bool AxisRect2D::removeTextItem2D(TextItem2D *textitem) {
  for (int i = 0; i < textvec_.size(); i++) {
    if (textvec_.at(i) == textitem) {
      textvec_.remove(i);
      layers_.removeOne(textitem->layer());
    }
  }
  bool result = false;
  result = plot2d_->removeItem(textitem);
  if (!result) return result;

  emit TextItem2DRemoved(this);
  return result;
}

bool AxisRect2D::removeLineItem2D(LineItem2D *lineitem) {
  for (int i = 0; i < linevec_.size(); i++) {
    if (linevec_.at(i) == lineitem) {
      linevec_.remove(i);
      layers_.removeOne(lineitem->layer());
    }
  }
  bool result = false;
  result = plot2d_->removeItem(lineitem);
  if (!result) return result;

  emit LineItem2DRemoved(this);
  return result;
}

bool AxisRect2D::removeImageItem2D(ImageItem2D *imageitem) {
  for (int i = 0; i < imagevec_.size(); i++) {
    if (imagevec_.at(i) == imageitem) {
      imagevec_.remove(i);
      layers_.removeOne(imageitem->layer());
    }
  }
  bool result = false;
  result = plot2d_->removeItem(imageitem);
  if (!result) return result;

  emit ImageItem2DRemoved(this);
  return result;
}

bool AxisRect2D::removeLineSpecial2D(LineSpecial2D *ls) {
  for (int i = 0; i < lsvec_.size(); i++) {
    if (lsvec_.at(i) == ls) {
      lsvec_.remove(i);
      layers_.removeOne(ls->layer());
    }
  }
  axisRectLegend_->removeItem(axisRectLegend_->itemWithPlottable(ls));
  bool result = false;
  ls->removeXerrorBar();
  ls->removeYerrorBar();
  result = plot2d_->removeGraph(ls);
  // result = plot2d_->removePlottable(ls);
  if (!result) return result;
  emit LineSpecial2DRemoved(this);
  return result;
}

bool AxisRect2D::removeChannel2D(
    QPair<LineSpecial2D *, LineSpecial2D *> channel) {
  for (int i = 0; i < channelvec_.size(); i++) {
    if (channelvec_.at(i) == channel) {
      channelvec_.remove(i);
      layers_.removeOne(channel.first->layer());
    }
  }
  axisRectLegend_->removeItem(
      axisRectLegend_->itemWithPlottable(channel.first));
  bool result = false;
  result = plot2d_->removeGraph(channel.second);
  if (!result) return result;
  result = plot2d_->removeGraph(channel.first);
  if (!result) return result;
  emit LineSpecialChannel2DRemoved(this);
  return result;
}

bool AxisRect2D::removeStatBox2D(StatBox2D *statbox) {
  for (int i = 0; i < statboxvec_.size(); i++) {
    if (statboxvec_.at(i) == statbox) {
      statboxvec_.remove(i);
      layers_.removeOne(statbox->layer());
    }
  }
  axisRectLegend_->removeItem(axisRectLegend_->itemWithPlottable(statbox));
  bool result = false;
  result = plot2d_->removePlottable(statbox);
  emit StatBox2DRemoved(this);
  return result;
}

bool AxisRect2D::removeVector2D(Vector2D *vector) {
  for (int i = 0; i < vectorvec_.size(); i++) {
    if (vectorvec_.at(i) == vector) {
      vectorvec_.remove(i);
      layers_.removeOne(vector->layer());
    }
  }
  axisRectLegend_->removeItem(axisRectLegend_->itemWithPlottable(vector));
  bool result = false;
  result = plot2d_->removePlottable(vector);
  emit Vector2DRemoved(this);
  return result;
}

bool AxisRect2D::removeCurve2D(Curve2D *curve) {
  for (int i = 0; i < curvevec_.size(); i++) {
    if (curvevec_.at(i) == curve) {
      curvevec_.remove(i);
      layers_.removeOne(curve->layer());
    }
  }
  axisRectLegend_->removeItem(axisRectLegend_->itemWithPlottable(curve));
  bool result = false;
  curve->removeXerrorBar();
  curve->removeYerrorBar();
  result = plot2d_->removePlottable(curve);
  emit Curve2DRemoved(this);
  return result;
}

bool AxisRect2D::removeBar2D(Bar2D *bar) {
  for (int i = 0; i < barvec_.size(); i++) {
    if (barvec_.at(i) == bar) {
      barvec_.remove(i);
      layers_.removeOne(bar->layer());
    }
  }
  axisRectLegend_->removeItem(axisRectLegend_->itemWithPlottable(bar));
  bool result = false;
  bar->removeXerrorBar();
  bar->removeYerrorBar();
  result = plot2d_->removePlottable(bar);
  emit Bar2DRemoved(this);
  return result;
}

bool AxisRect2D::removePie2D(Pie2D *pie) {
  for (int i = 0; i < pievec_.size(); i++) {
    if (pievec_.at(i) == pie) {
      pievec_.remove(i);
      layers_.removeOne(pie->layer());
    }
  }
  bool result = false;
  result = plot2d_->removeItem(pie);
  emit Pie2DRemoved(this);
  return result;
}

bool AxisRect2D::removeColorMap2D(ColorMap2D *colormap) {
  for (int i = 0; i < colormapvec_.size(); i++) {
    if (colormapvec_.at(i) == colormap) {
      colormapvec_.remove(i);
      layers_.removeOne(colormap->layer());
    }
  }
  bool result = false;
  result = plot2d_->removePlottable(colormap);
  emit ColorMap2DRemoved(this);
  return result;
}

bool AxisRect2D::moveLayer(QCPLayer *layer,
                           const QCustomPlot::LayerInsertMode &mode) {
  bool layermoved = false;
  QCPLayer *layerswamped = nullptr;
  for (int i = 0; i < layers_.size(); i++) {
    if (layers_.at(i) == layer) {
      switch (mode) {
        case QCustomPlot::LayerInsertMode::limAbove:
          if (i + 1 < layers_.size()) {
            layermoved =
                parentPlot()->moveLayer(layers_.at(i), layers_.at(i + 1),
                                        QCustomPlot::LayerInsertMode::limAbove);
            if (layermoved) {
              layerswamped = layers_.at(i + 1);
              layers_.swapItemsAt(i, i + 1);
            }
          } else {
            qDebug() << "unable to move layer(s). this layer is already the "
                        "top layer";
          }
          break;
        case QCustomPlot::LayerInsertMode::limBelow:
          if (i > 0) {
            layermoved =
                parentPlot()->moveLayer(layers_.at(i), layers_.at(i - 1),
                                        QCustomPlot::LayerInsertMode::limBelow);
            if (layermoved) {
              layerswamped = layers_.at(i - 1);
              layers_.swapItemsAt(i, i - 1);
            }
          } else {
            qDebug() << "unable to move layer(s). this layer is already the "
                        "bottom layer";
          }
          break;
      }
      break;
    }
  }
  if (layermoved) {
    layermoved = movechannellayer(layer, layerswamped);
    emit LayerMoved(this);
  }
  return layermoved;
}

bool AxisRect2D::movechannellayer(QCPLayer *layer, QCPLayer *layerswap) {
  if (!layerswap) return false;
  bool layermoved = true;
  for (int i = 0; i < layers_.size(); i++) {
    foreach (auto channel, channelvec_) {
      if (channel.first->layer() == layer) {
        layermoved =
            parentPlot()->moveLayer(channel.second->layer(), layer,
                                    QCustomPlot::LayerInsertMode::limBelow);
        if (!layermoved) qDebug() << "unable to move channel layer(s). error ";
      } else if (channel.first->layer() == layerswap) {
        layermoved =
            parentPlot()->moveLayer(channel.second->layer(), layerswap,
                                    QCustomPlot::LayerInsertMode::limBelow);
        if (!layermoved) qDebug() << "unable to move channel layer(s). error ";
      }
    }
  }
  return layermoved;
}

void AxisRect2D::replotBareBones() const {
  plot2d_->layer(plot2d_->getBackground2DLayerName())->replot();
  plot2d_->layer(plot2d_->getGrid2DLayerName())->replot();
  plot2d_->layer(plot2d_->getAxis2DLayerName())->replot();
  plot2d_->layer(plot2d_->getLegend2DLayerName())->replot();
}

void AxisRect2D::setGraphTool(const Graph2DCommon::Picker &picker) {
  foreach (LineSpecial2D *ls, lsvec_) { ls->setpicker_lsplot(picker); }
  foreach (Curve2D *curve, curvevec_) { curve->setpicker_cplot(picker); }
  foreach (Bar2D *bar, barvec_) { bar->setpicker_barplot(picker); }
  foreach (StatBox2D *statbox, statboxvec_) {
    statbox->setpicker_statbox(picker);
  }
}

void AxisRect2D::setGridPairToNullptr() {
  gridpair_.first.first = nullptr;
  gridpair_.first.second = nullptr;
  gridpair_.second.first = nullptr;
  gridpair_.second.second = nullptr;
}

void AxisRect2D::setItemAxes(Axis2D *xaxis, Axis2D *yaxis) {
  // change to new axis before you delete the associated axis
  // we always keep grid axis associated with Items
  if (!xaxis && !yaxis) {
    qDebug() << "QCPAbstractItem unable to set to new Axis2D";
    return;
  }

  foreach (TextItem2D *textitem, textvec_) {
    textitem->position->setAxes(xaxis, yaxis);
  }
  foreach (LineItem2D *lineitem, linevec_) {
    foreach (QCPItemPosition *position, lineitem->positions()) {
      position->setAxes(xaxis, yaxis);
    }
  }
  foreach (ImageItem2D *imageitem, imagevec_) {
    foreach (QCPItemPosition *position, imageitem->positions()) {
      position->setAxes(xaxis, yaxis);
    }
  }
}

Table *AxisRect2D::getTableByName(QList<Table *> tabs, const QString name) {
  Table *table = nullptr;
  foreach (Table *tab, tabs) {
    if (tab->name() == name) table = tab;
  }
  return table;
}

Matrix *AxisRect2D::getMatrixByName(QList<Matrix *> mats, const QString name) {
  Matrix *matrix = nullptr;
  foreach (Matrix *mat, mats) {
    if (mat->name() == name) matrix = mat;
  }
  return matrix;
}

void AxisRect2D::save(XmlStreamWriter *xmlwriter, const int index) {
  xmlwriter->writeStartElement("layout");
  xmlwriter->writeAttribute("index", QString::number(index + 1));
  xmlwriter->writeAttribute("row", QString::number(0));
  xmlwriter->writeAttribute("column", QString::number(index + 1));
  xmlwriter->writeBrush(backgroundBrush());
  foreach (Axis2D *axis, getAxes2D()) { axis->save(xmlwriter); }
  gridpair_.first.first->save(xmlwriter, "xgrid");
  gridpair_.second.first->save(xmlwriter, "ygrid");
  foreach (TextItem2D *textitem, textvec_) { textitem->save(xmlwriter); }
  foreach (LineItem2D *lineitem, linevec_) { lineitem->save(xmlwriter); }
  foreach (ImageItem2D *imageitem, imagevec_) { imageitem->save(xmlwriter); }
  foreach (Curve2D *curve, curvevec_) {
    curve->save(xmlwriter, getXAxisNo(curve->getxaxis()),
                getYAxisNo(curve->getyaxis()));
  }
  foreach (Vector2D *vector, vectorvec_) {
    vector->save(xmlwriter, getXAxisNo(vector->getxaxis()),
                 getYAxisNo(vector->getyaxis()));
  }
  foreach (Pie2D *pie, pievec_) { pie->save(xmlwriter); }
  getLegend()->save(xmlwriter);
  xmlwriter->writeEndElement();
}

bool AxisRect2D::load(XmlStreamReader *xmlreader, QList<Table *> tabs,
                      QList<Matrix *> mats) {
  if (xmlreader->isStartElement() && xmlreader->name() == "layout") {
    bool ok = false;
    while (!xmlreader->atEnd()) {
      xmlreader->readNext();
      if (xmlreader->isEndElement() && xmlreader->name() == "brush") break;
      // brush
      if (xmlreader->isStartElement() && xmlreader->name() == "brush") {
        QBrush b = xmlreader->readBrush(&ok);
        if (ok)
          setBackground(b);
        else
          xmlreader->raiseWarning(tr("Layout brush property setting error"));
      }
    }
    while (!xmlreader->atEnd()) {
      xmlreader->readNext();
      if (xmlreader->isEndElement() && xmlreader->name() == "layout") break;
      if (xmlreader->isStartElement() && xmlreader->name() == "axis") {
        Axis2D::AxisOreantation type = Axis2D::AxisOreantation::Left;
        Axis2D::TickerType tickertype = Axis2D::TickerType::Value;
        // Type property
        QString position = xmlreader->readAttributeString("position", &ok);
        if (ok) {
          if (position == "left")
            type = Axis2D::AxisOreantation::Left;
          else if (position == "bottom")
            type = Axis2D::AxisOreantation::Bottom;
          else if (position == "right")
            type = Axis2D::AxisOreantation::Right;
          else if (position == "top")
            type = Axis2D::AxisOreantation::Top;
          else
            xmlreader->raiseError(
                tr("(critical) Axis2D Position property setting error"));
        } else
          xmlreader->raiseError(
              tr("(critical) Axis2D Position property setting error"));
        // Tickertype property
        QString ticker = xmlreader->readAttributeString("tickertype", &ok);
        if (ok) {
          if (ticker == "value")
            tickertype = Axis2D::TickerType::Value;
          else if (ticker == "log")
            tickertype = Axis2D::TickerType::Log;
          else if (ticker == "pi")
            tickertype = Axis2D::TickerType::Pi;
          else if (ticker == "time")
            tickertype = Axis2D::TickerType::Time;
          else if (ticker == "datetime")
            tickertype = Axis2D::TickerType::DateTime;
          else if (ticker == "text")
            tickertype = Axis2D::TickerType::Text;
          else {
            xmlreader->raiseError(
                tr("(critical) Axis2D Tickertype property setting error"));
          }
        } else
          xmlreader->raiseError(
              tr("(critical) Axis2D Tickertype property setting error"));
        // create axis
        Axis2D *axis = addAxis2D(type, tickertype);
        bool associatedgrid = xmlreader->readAttributeBool("grid", &ok);
        if (ok && associatedgrid) {
          bindGridTo(axis);
        } else
          xmlreader->raiseWarning(tr("Axis2D unable to set Grid2D error"));
        axis->load(xmlreader);
      } else
          // xgrid
          if (xmlreader->isStartElement() && xmlreader->name() == "xgrid") {
        gridpair_.first.first->load(xmlreader, "xgrid");
      } else
          // ygrid
          if (xmlreader->isStartElement() && xmlreader->name() == "ygrid") {
        gridpair_.second.first->load(xmlreader, "ygrid");
      } else
          // textitem
          if (xmlreader->isStartElement() && xmlreader->name() == "textitem") {
        TextItem2D *textitem = addTextItem2D("Text");
        textitem->load(xmlreader);
      } else
          // lineitem
          if (xmlreader->isStartElement() && xmlreader->name() == "lineitem") {
        LineItem2D *lineitem = addLineItem2D();
        lineitem->load(xmlreader);
      } else
          // imageitem
          if (xmlreader->isStartElement() && xmlreader->name() == "imageitem") {
        // source property
        QString file = xmlreader->readAttributeString("file", &ok);
        if (ok && QFile(file).exists()) {
          ImageItem2D *imageitem = addImageItem2D(file);
          imageitem->load(xmlreader);
        } else
          xmlreader->raiseWarning(
              tr("ImageItem2D file property setting error"));
      } else
          // curve
          if (xmlreader->isStartElement() && xmlreader->name() == "curve") {
        Curve2D *curve = nullptr;
        Axis2D *xaxis = nullptr;
        Axis2D *yaxis = nullptr;
        LineScatterType ctype = LineScatterType::Scatter2D;

        int xax = xmlreader->readAttributeInt("xaxis", &ok);
        if (ok) {
          xaxis = getXAxis(xax);
        } else
          xmlreader->raiseError(tr("Curve2D X axis not found error"));
        int yax = xmlreader->readAttributeInt("yaxis", &ok);
        if (ok) {
          yaxis = getYAxis(yax);
        } else
          xmlreader->raiseError(tr("Curve2D Y axis not found error"));

        QString curvetype = xmlreader->readAttributeString("type", &ok);
        if (curvetype == "curve" && ok) {
          ctype = LineScatterType::Scatter2D;
        } else if (curvetype == "spline" && ok) {
          ctype = LineScatterType::Spline2D;
        }

        // legend
        QString legend = xmlreader->readAttributeString("legend", &ok);
        if (!ok) xmlreader->raiseWarning(tr("Curve2D legendtext not found"));

        QString datatype = xmlreader->readAttributeString("data", &ok);
        if (ok && datatype == "table") {
          Table *table = nullptr;
          Column *xcolumn = nullptr;
          Column *ycolumn = nullptr;

          QString tablename = xmlreader->readAttributeString("table", &ok);
          if (ok) {
            table = getTableByName(tabs, tablename);
          } else
            xmlreader->raiseError(tr("Curve2D Table not found error"));
          QString xcolname = xmlreader->readAttributeString("xcolumn", &ok);
          if (ok) {
            (table) ? xcolumn = table->column(xcolname) : xcolumn = nullptr;
          } else
            xmlreader->raiseError(tr("Curve2D Table X column not found error"));
          QString ycolname = xmlreader->readAttributeString("ycolumn", &ok);
          if (ok) {
            (table) ? ycolumn = table->column(ycolname) : ycolumn = nullptr;
          } else
            xmlreader->raiseError(tr("Curve2D Table Y column not found error"));
          int from = xmlreader->readAttributeInt("from", &ok);
          if (!ok) xmlreader->raiseError(tr("Curve2D from not found error"));
          int to = xmlreader->readAttributeInt("to", &ok);
          if (!ok) xmlreader->raiseError(tr("Curve2D to not found error"));

          if (table && xcolumn && ycolumn && xaxis && yaxis) {
            curve = addCurve2DPlot(ctype, table, xcolumn, ycolumn, from, to,
                                   xaxis, yaxis);
            // curve->load(xmlreader);
          }
        } else if (ok && datatype == "function") {
          QVector<double> *xdata = new QVector<double>();
          QVector<double> *ydata = new QVector<double>();

          while (!xmlreader->atEnd()) {
            xmlreader->readNext();
            if (xmlreader->isEndElement() &&
                xmlreader->name() == "functiondata")
              break;
            // pen
            if (xmlreader->isStartElement() && xmlreader->name() == "data") {
              double xval = xmlreader->readAttributeDouble("xdata", &ok);
              bool xok = ok;
              double yval = xmlreader->readAttributeDouble("ydata", &ok);
              if (xok && ok) {
                xdata->append(xval);
                ydata->append(yval);
              } else
                xmlreader->raiseWarning(tr("Curve2D data generation error"));
            }
          }
          if (xdata->size() > 0 && ydata->size() > 0 &&
              xdata->size() == ydata->size() && xaxis && yaxis) {
            curve = addFunction2DPlot(xdata, ydata, xaxis, yaxis, legend);
            curve->setlegendtext_cplot(legend);
            // curve->load(xmlreader);
          } else {
            xmlreader->raiseError(tr("Curve2D function skipped due to error"));
            delete xdata;
            delete ydata;
          }

        } else
          xmlreader->raiseError(tr("Curve2D data not found error"));

        while (!xmlreader->atEnd()) {
          xmlreader->readNextStartElement();
          if (xmlreader->isStartElement() && xmlreader->name() != "errorbar") {
            curve->load(xmlreader);
            break;
          }

          if (xmlreader->isStartElement() && xmlreader->name() == "errorbar") {
            Table *table = nullptr;
            Column *column = nullptr;
            QString type = xmlreader->readAttributeString("type", &ok);
            if (!ok) {
              xmlreader->raiseError(tr("ErrorBar2D type not found error"));
            }
            QString tablename = xmlreader->readAttributeString("table", &ok);
            if (ok) {
              table = getTableByName(tabs, tablename);
            } else
              xmlreader->raiseError(tr("ErrorBar2D Table not found error"));
            QString colname = xmlreader->readAttributeString("errcolumn", &ok);
            if (ok) {
              (table) ? column = table->column(colname) : column = nullptr;
            } else
              xmlreader->raiseError(
                  tr("ErrorBar2D Table column not found error"));
            int from = xmlreader->readAttributeInt("from", &ok);
            if (!ok)
              xmlreader->raiseError(tr("ErrorBar2D from not found error"));
            int to = xmlreader->readAttributeInt("to", &ok);
            if (!ok) xmlreader->raiseError(tr("ErrorBar2D to not found error"));

            if (table && column && !type.isEmpty() &&
                curve->getcurvetype_cplot() == Curve2D::Curve2DType::Curve) {
              if (type == "x") {
                curve->setXerrorBar(table, column, from, to);
                curve->getxerrorbar_curveplot()->load(xmlreader);
              } else if (type == "y") {
                curve->setYerrorBar(table, column, from, to);
                curve->getyerrorbar_curveplot()->load(xmlreader);
              }
            }
          }
        }
      } else

          // vector
          if (xmlreader->isStartElement() && xmlreader->name() == "vector") {
        Axis2D *xaxis = nullptr;
        Axis2D *yaxis = nullptr;
        Vector2D::VectorPlot type;

        // axis
        int xax = xmlreader->readAttributeInt("xaxis", &ok);
        if (ok) {
          xaxis = getXAxis(xax);
        } else
          xmlreader->raiseError(tr("Vector2D X axis not found error"));
        int yax = xmlreader->readAttributeInt("yaxis", &ok);
        if (ok) {
          yaxis = getYAxis(yax);
        } else
          xmlreader->raiseError(tr("Vector2D Y axis not found error"));

        // vector type
        QString vectortype = xmlreader->readAttributeString("type", &ok);
        if (vectortype == "xyam" && ok) {
          type = Vector2D::VectorPlot::XYAM;
        } else if (vectortype == "xyxy" && ok) {
          type = Vector2D::VectorPlot::XYXY;
        } else {
          xmlreader->raiseError(tr("Vector2D type not found error"));
        }

        Table *table = nullptr;
        Column *x1column = nullptr;
        Column *y1column = nullptr;
        Column *x2column = nullptr;
        Column *y2column = nullptr;

        QString tablename = xmlreader->readAttributeString("table", &ok);
        if (ok) {
          table = getTableByName(tabs, tablename);
        } else
          xmlreader->raiseError(tr("Vector2D Table not found error"));
        QString x1colname = xmlreader->readAttributeString("x1column", &ok);
        if (ok) {
          (table) ? x1column = table->column(x1colname) : x1column = nullptr;
        } else
          xmlreader->raiseError(tr("Vector2D Table X1 column not found error"));
        QString y1colname = xmlreader->readAttributeString("y1column", &ok);
        if (ok) {
          (table) ? y1column = table->column(y1colname) : y1column = nullptr;
        } else
          xmlreader->raiseError(tr("Vector2D Table Y1 column not found error"));

        QString x2colname = xmlreader->readAttributeString("x2column", &ok);
        if (ok) {
          (table) ? x2column = table->column(x2colname) : x2column = nullptr;
        } else
          xmlreader->raiseError(tr("Vector2D Table X2 column not found error"));
        QString y2colname = xmlreader->readAttributeString("y2column", &ok);
        if (ok) {
          (table) ? y2column = table->column(y2colname) : y2column = nullptr;
        } else
          xmlreader->raiseError(tr("Vector2D Table Y2 column not found error"));

        int from = xmlreader->readAttributeInt("from", &ok);
        if (!ok) xmlreader->raiseError(tr("Vector2D from not found error"));
        int to = xmlreader->readAttributeInt("to", &ok);
        if (!ok) xmlreader->raiseError(tr("Vector2D to not found error"));

        if (table && x1column && y1column && x2column && y2column && xaxis &&
            yaxis) {
          Vector2D *curve =
              addVectorPlot(type, table, x1column, y1column, x2column, y2column,
                            from, to, xaxis, yaxis);
          curve->load(xmlreader);
        }
      } else

          // pie
          if (xmlreader->isStartElement() && xmlreader->name() == "pie") {
        Table *table = nullptr;
        Column *xcolumn = nullptr;
        QString tablename = xmlreader->readAttributeString("table", &ok);
        if (ok) {
          table = getTableByName(tabs, tablename);
        } else
          xmlreader->raiseError(tr("Pie2D Table not found error"));
        QString xcolname = xmlreader->readAttributeString("xcolumn", &ok);
        if (ok) {
          (table) ? xcolumn = table->column(xcolname) : xcolumn = nullptr;
        } else
          xmlreader->raiseError(tr("Pie2D Table X column not found error"));
        int from = xmlreader->readAttributeInt("from", &ok);
        if (!ok) xmlreader->raiseError(tr("Pie2D from not found error"));
        int to = xmlreader->readAttributeInt("to", &ok);
        if (!ok) xmlreader->raiseError(tr("Pie2D to not found error"));
        if (table && xcolumn) {
          Pie2D *pie = addPie2DPlot(table, xcolumn, from, to);
          pie->load(xmlreader);
        }
      }
      if (xmlreader->isStartElement() && xmlreader->name() == "legend") {
        getLegend()->load(xmlreader);
      }
    }
  } else  // no plot2d element
    xmlreader->raiseError(tr("unknown element %1").arg(xmlreader->name()));

  return !xmlreader->hasError();
}

void AxisRect2D::mousePressEvent(QMouseEvent *event, const QVariant &variant) {
  emit AxisRectClicked(this);
  QCPAxisRect::mousePressEvent(event, variant);
}

void AxisRect2D::draw(QCPPainter *painter) {
  if (printorexportjob_) return;
  QCPAxisRect::draw(painter);
  if (isAxisRectSelected_) drawSelection(painter);
}

void AxisRect2D::legendClick() { emit AxisRectClicked(this); }
