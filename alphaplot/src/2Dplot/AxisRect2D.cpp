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
#include "Curve2D.h"
#include "ErrorBar2D.h"
#include "Grid2D.h"
#include "ImageItem2D.h"
#include "Layout2D.h"
#include "LayoutGrid2D.h"
#include "LayoutInset2D.h"
#include "Legend2D.h"
#include "LineItem2D.h"
#include "LineSpecial2D.h"
#include "Matrix.h"
#include "PickerTool2D.h"
#include "Pie2D.h"
#include "Plot2D.h"
#include "QMessageBox"
#include "Table.h"
#include "TextItem2D.h"
#include "core/Utilities.h"
#include "future/core/column/Column.h"
#include "future/lib/XmlStreamWriter.h"

AxisRect2D::AxisRect2D(Plot2D *parent, PickerTool2D *picker,
                       bool setupDefaultAxis)
    : QCPAxisRect(parent, setupDefaultAxis),
      plot2d_(parent),
      axisRectBackGround_(Qt::white),
      axisRectLegend_(new Legend2D(this)),
      isAxisRectSelected_(false),
      printorexportjob_(false),
      picker_(picker) {
  setRangeDrag(Qt::Horizontal | Qt::Vertical);
  setRangeZoom(Qt::Horizontal | Qt::Vertical);
  gridpair_.first.first = nullptr;
  gridpair_.first.second = nullptr;
  gridpair_.second.first = nullptr;
  gridpair_.second.second = nullptr;
  setAxisRectBackground(axisRectBackGround_);
  insetLayout()->addElement(axisRectLegend_, Qt::AlignTop | Qt::AlignLeft);
  insetLayout()->setInsetPlacement(0, QCPLayoutInset::ipFree);
  axisRectLegend_->setlayer_legend(plot2d_->getLegend2DLayerName());
  connect(axisRectLegend_, &Legend2D::legendClicked, this,
          &AxisRect2D::legendClick);
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
    default:
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

void AxisRect2D::setbarsstyle() {
  // set stack
  QList<Bar2D *> bvec;
  QList<Bar2D *> sortedbvec;
  Bar2D::BarStyle style;
  foreach (Bar2D *bar, barvec_) {
    if (bar->getstackposition_barplot() != -1) bvec << bar;
  }

  if (bvec.size() < 2) {
    qDebug() << "less than 2 bars for stacked/grouped port";
    return;
  } else
    style = bvec.first()->getBarStyle();
  // sort the order
  if (!bvec.isEmpty()) {
    int j = 0;
    while (!bvec.isEmpty()) {
      for (int i = 0; i < bvec.size(); i++) {
        if (bvec.at(i)->getstackposition_barplot() == j) {
          if (style != bvec.at(i)->getBarStyle()) return;
          sortedbvec << bvec.at(i);
          bvec.removeOne(bvec.at(i));
          j++;
          break;
        }
      }
    }
  }

  (style == Bar2D::BarStyle::Grouped) ? addBarsToBarsGroup(sortedbvec, false)
                                      : addBarsToStackGroup(sortedbvec);
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

Axis2D *AxisRect2D::getXAxis(const int value) {
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

Axis2D *AxisRect2D::getYAxis(const int value) {
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
    Column *yData, int from, const int to, Axis2D *xAxis, Axis2D *yAxis) {
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
  connect(legendItem, &LegendItem2D::legendItemClicked, this,
          &AxisRect2D::legendClick);
  lineSpecial->setName(table->name() + "_" + xData->name() + "_" +
                       yData->name());
  lsvec_.append(lineSpecial);
  layers_.append(lineSpecial->layer());

  emit LineSpecial2DCreated(lineSpecial);
  return lineSpecial;
}

QPair<LineSpecial2D *, LineSpecial2D *> AxisRect2D::addLineSpecialChannel2DPlot(
    Table *table, Column *xData, Column *yData1, Column *yData2, const int from,
    const int to, Axis2D *xAxis, Axis2D *yAxis) {
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
  connect(legendItem, &LegendItem2D::legendItemClicked, this,
          &AxisRect2D::legendClick);
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
                                    const int from, const int to, Axis2D *xAxis,
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
  connect(legendItem, &LegendItem2D::legendItemClicked, this,
          &AxisRect2D::legendClick);
  curve->setName(table->name() + "_" + xcol->name() + "_" + ycol->name());
  curvevec_.append(curve);
  layers_.append(curve->layer());

  emit Curve2DCreated(curve);
  return curve;
}

Curve2D *AxisRect2D::addFunction2DPlot(const PlotData::FunctionData funcdata,
                                       QVector<double> *xdata,
                                       QVector<double> *ydata, Axis2D *xAxis,
                                       Axis2D *yAxis, const QString &name) {
  Curve2D *curve = new Curve2D(funcdata, xdata, ydata, xAxis, yAxis);
  curve->setlinetype_cplot(1);
  curve->setscattershape_cplot(Graph2DCommon::ScatterStyle::None);

  curve->setName(name);
  LegendItem2D *legendItem = new LegendItem2D(axisRectLegend_, curve);
  axisRectLegend_->addItem(legendItem);
  connect(legendItem, &LegendItem2D::legendItemClicked, this,
          &AxisRect2D::legendClick);
  layers_.append(curve->layer());
  curvevec_.append(curve);

  emit Curve2DCreated(curve);
  return curve;
}

Bar2D *AxisRect2D::addBox2DPlot(const AxisRect2D::BarType &type, Table *table,
                                Column *xData, Column *yData, const int from,
                                const int to, Axis2D *xAxis, Axis2D *yAxis,
                                const Bar2D::BarStyle &style,
                                int stackposition) {
  Bar2D *bar;
  switch (type) {
    case AxisRect2D::BarType::HorizontalBars:
      bar = new Bar2D(table, xData, yData, from, to, yAxis, xAxis, style,
                      stackposition);
      break;
    case AxisRect2D::BarType::VerticalBars:
      bar = new Bar2D(table, xData, yData, from, to, xAxis, yAxis, style,
                      stackposition);
      break;
  }

  bar->setWidth(1);
  bar->setAntialiased(false);
  bar->setAntialiasedFill(false);
  LegendItem2D *legendItem = new LegendItem2D(axisRectLegend_, bar);
  axisRectLegend_->addItem(legendItem);
  connect(legendItem, &LegendItem2D::legendItemClicked, this,
          &AxisRect2D::legendClick);
  bar->setName(table->name() + "_" + xData->name() + "_" + yData->name());
  layers_.append(bar->layer());
  barvec_.append(bar);

  emit Bar2DCreated(bar);
  return bar;
}

Vector2D *AxisRect2D::addVectorPlot(const Vector2D::VectorPlot &vectorplot,
                                    Table *table, Column *x1Data,
                                    Column *y1Data, Column *x2Data,
                                    Column *y2Data, const int from,
                                    const int to, Axis2D *xAxis,
                                    Axis2D *yAxis) {
  Vector2D *vec = new Vector2D(vectorplot, table, x1Data, y1Data, x2Data,
                               y2Data, from, to, xAxis, yAxis);
  VectorLegendItem2D *legendItem = new VectorLegendItem2D(axisRectLegend_, vec);
  axisRectLegend_->addItem(legendItem);
  connect(legendItem, &LegendItem2D::legendItemClicked, this,
          &AxisRect2D::legendClick);
  vec->setName(table->name() + "_" + x1Data->name() + "_" + y1Data->name() +
               "_" + x2Data->name() + "_" + y2Data->name());
  layers_.append(vec->layer());
  vectorvec_.append(vec);

  emit Vector2DCreated(vec);
  return vec;
}

StatBox2D *AxisRect2D::addStatBox2DPlot(const StatBox2D::BoxWhiskerData data,
                                        Axis2D *xAxis, Axis2D *yAxis) {
  StatBox2D *statbox = new StatBox2D(data, xAxis, yAxis);
  LegendItem2D *legendItem = new LegendItem2D(axisRectLegend_, statbox);
  axisRectLegend_->addItem(legendItem);
  connect(legendItem, SIGNAL(legendItemClicked()), SLOT(legendClick()));
  statbox->setName(data.name);
  getLegend()->setVisible(false);
  layers_.append(statbox->layer());
  statboxvec_.append(statbox);
  emit StatBox2DCreated(statbox);
  return statbox;
}

Bar2D *AxisRect2D::addHistogram2DPlot(const AxisRect2D::BarType &type,
                                      Table *table, Column *yData,
                                      const int from, const int to,
                                      Axis2D *xAxis, Axis2D *yAxis) {
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
  emit Bar2DCreated(bar);
  return bar;
}

Pie2D *AxisRect2D::addPie2DPlot(const Graph2DCommon::PieStyle &style,
                                Table *table, Column *xData, Column *yData,
                                const int from, const int to) {
  Pie2D *pie = new Pie2D(this, style, table, xData, yData, from, to);
  pie->setGraphData(table, xData, yData, from, to);
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

TextItem2D *AxisRect2D::addTextItem2D(const QString text) {
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
  LineItem2D *lineitem = new LineItem2D(this, plot2d_);
  lineitem->setendstyle_lineitem(LineItem2D::LineEndLocation::Stop,
                                 QCPLineEnding::EndingStyle::esFlatArrow);
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

ImageItem2D *AxisRect2D::addImageItem2D(const QString &filename) {
  ImageItem2D *imageitem = new ImageItem2D(this, plot2d_, filename);
  foreach (QCPItemPosition *position, imageitem->positions()) {
    position->setAxes(gridpair_.first.second, gridpair_.second.second);
  }
  imageitem->topLeft->setPixelPosition(rect().center());
  // anchor point adjustment
  imageitem->bottomRight->setPixelPosition(
      QPointF(imageitem->topRight->pixelPosition().x(),
              imageitem->bottomLeft->pixelPosition().y()));
  imageitem->setScaled(true, Qt::AspectRatioMode::IgnoreAspectRatio);
  layers_.append(imageitem->layer());
  imagevec_.append(imageitem);
  emit ImageItem2DCreated(imageitem);
  return imageitem;
}

LayoutInset2D *AxisRect2D::addLayoutInset2D() {
  LayoutInset2D *inset = new LayoutInset2D(this);
  return inset;
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

void AxisRect2D::addBarsToBarsGroup(QList<Bar2D *> bars,
                                    bool autowidthsettins) {
  QCPBarsGroup *bargroup = new QCPBarsGroup(plot2d_);
  addBarsGroup(bargroup);
  double spacing = 0.0;
  foreach (Bar2D *bar, bars) {
    if (autowidthsettins) {
      bar->setWidthType(QCPBars::wtPlotCoords);
      spacing = bar->width() * 0.1;
      bar->setWidth((bar->width() / bars.size()) - spacing * 2);
    } else {
      spacing = bar->stackingGap();
    }
    bargroup->append(bar);
    bar->setBarGroup(bargroup);
  }
  bargroup->setSpacingType(QCPBarsGroup::stPlotCoords);
  bargroup->setSpacing(spacing);
}

void AxisRect2D::addBarsToStackGroup(QList<Bar2D *> bars) {
  // create the stack
  Bar2D *basebar = nullptr;
  foreach (Bar2D *bar, bars) {
    bar->setStackingGap(1);
    if (basebar) bar->moveAbove(basebar);
    basebar = bar;
  }
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
  foreach (auto bargroup, bargroupvec_) {
    if (bargroup->size() == 1) {
      bargroupvec_.removeOne(bargroup);
      delete bargroup;
    }
  }
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
#if (QT_VERSION >= QT_VERSION_CHECK(5, 13, 0))
              layers_.swapItemsAt(i, i + 1);
#else
              layers_.swap(i, i + 1);
#endif
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
#if (QT_VERSION >= QT_VERSION_CHECK(5, 13, 0))
              layers_.swapItemsAt(i, i - 1);
#else
              layers_.swap(i, i + 1);
#endif
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

bool AxisRect2D::loadLineSpecialChannel2D(XmlStreamReader *xmlreader,
                                          QList<Table *> tabs) {
  bool ok = false;
  // ls1
  Axis2D *xaxis1 = nullptr;
  Axis2D *yaxis1 = nullptr;
  AxisRect2D::LineScatterSpecialType ltype1;
  bool legendvisible1 = true;
  QString legend1;
  Table *table1 = nullptr;
  Column *xcolumn1 = nullptr;
  Column *ycolumn1 = nullptr;
  int from1 = 0;
  int to1 = 0;
  Graph2DCommon::LineStyleType lstype1;
  bool linefill1 = true;
  bool lineantialias1 = true;
  QPen linepen1;
  QBrush linebrush1;
  Graph2DCommon::ScatterStyle scatterstyle1;
  int scattersize1 = 6;
  bool scatterantialias1 = true;
  QPen scatterpen1;
  QBrush scatterbrush1;
  // ls2
  Axis2D *xaxis2 = nullptr;
  Axis2D *yaxis2 = nullptr;
  AxisRect2D::LineScatterSpecialType ltype2;
  bool legendvisible2 = true;
  QString legend2;
  Table *table2 = nullptr;
  Column *xcolumn2 = nullptr;
  Column *ycolumn2 = nullptr;
  int from2 = 0;
  int to2 = 0;
  Graph2DCommon::LineStyleType lstype2;
  bool linefill2 = true;
  bool lineantialias2 = true;
  QPen linepen2;
  QBrush linebrush2;
  Graph2DCommon::ScatterStyle scatterstyle2;
  int scattersize2 = 6;
  bool scatterantialias2 = true;
  QPen scatterpen2;
  QBrush scatterbrush2;
  // linespecialchannel
  if (xmlreader->isStartElement() && xmlreader->name() == "channel") {
    xmlreader->readNextStartElement();
    while (!xmlreader->atEnd()) {
      if (xmlreader->isEndElement() && xmlreader->name() == "linespecial") {
        break;
      }
      if (xmlreader->isStartElement() && xmlreader->name() == "linespecial") {
        int xax = xmlreader->readAttributeInt("xaxis", &ok);
        if (ok) {
          xaxis1 = getXAxis(xax);
        } else
          xmlreader->raiseError(
              tr("LineSpecialChannel2D X axis not found error"));
        int yax = xmlreader->readAttributeInt("yaxis", &ok);
        if (ok) {
          yaxis1 = getYAxis(yax);
        } else
          xmlreader->raiseError(
              tr("LineSpecialChannel2D Y axis not found error"));

        QString lstype = xmlreader->readAttributeString("type", &ok);
        if (lstype == "line" && ok) {
          ltype1 = AxisRect2D::LineScatterSpecialType::Area2D;
        } else
          xmlreader->raiseWarning(
              tr("LineSpecialChannel2D line type not found"));

        // legend
        legendvisible1 = xmlreader->readAttributeBool("legendvisible", &ok);
        if (!ok)
          xmlreader->raiseWarning(
              tr("LineSpecialChannel2D legend visible property setting error"));
        legend1 = xmlreader->readAttributeString("legend", &ok);
        if (!ok)
          xmlreader->raiseWarning(
              tr("LineSpecialChannel2D legendtext not found"));
        QString tablename = xmlreader->readAttributeString("table", &ok);
        if (ok) {
          table1 = getTableByName(tabs, tablename);
        } else
          xmlreader->raiseError(
              tr("LineSpecialChannel2D Table not found error"));
        QString xcolname = xmlreader->readAttributeString("xcolumn", &ok);
        if (ok) {
          (table1) ? xcolumn1 = table1->column(xcolname) : xcolumn1 = nullptr;
        } else
          xmlreader->raiseError(
              tr("LineSpecialChannel2D Table X column not found error"));
        QString ycolname = xmlreader->readAttributeString("ycolumn", &ok);
        if (ok) {
          (table1) ? ycolumn1 = table1->column(ycolname) : ycolumn1 = nullptr;
        } else
          xmlreader->raiseError(
              tr("LineSpecialChannel2D Table Y column not found error"));
        from1 = xmlreader->readAttributeInt("from", &ok);
        if (!ok)
          xmlreader->raiseError(
              tr("LineSpecialChannel2D from not found error"));
        to1 = xmlreader->readAttributeInt("to", &ok);
        if (!ok)
          xmlreader->raiseError(tr("LineSpecialChannel2D to not found error"));

        xmlreader->readNextStartElement();
        // line
        if (xmlreader->isStartElement() && xmlreader->name() == "line") {
          // line style
          QString style = xmlreader->readAttributeString("style", &ok);
          if (ok) {
            if (style == "line") {
              lstype1 = Graph2DCommon::LineStyleType::Line;
            } else if (style == "impulse") {
              lstype1 = Graph2DCommon::LineStyleType::Impulse;
            } else if (style == "stepleft") {
              lstype1 = Graph2DCommon::LineStyleType::StepLeft;
            } else if (style == "stepright") {
              lstype1 = Graph2DCommon::LineStyleType::StepRight;
            } else if (style == "stepcenter") {
              lstype1 = Graph2DCommon::LineStyleType::StepCenter;
            }
          } else
            xmlreader->raiseWarning(
                tr("LineSpecialChannel2D line style property setting error"));

          // line fill status
          linefill1 = xmlreader->readAttributeBool("fill", &ok);
          if (!ok)
            xmlreader->raiseWarning(
                tr("LineSpecialChannel2D line fill status property setting "
                   "error"));

          // line antialias
          lineantialias1 = xmlreader->readAttributeBool("antialias", &ok);
          if (!ok)
            xmlreader->raiseWarning(
                tr("LineSpecialChannel2D line antialias "
                   "property setting error"));

          // line pen property
          while (!xmlreader->atEnd()) {
            xmlreader->readNext();
            if (xmlreader->isEndElement() && xmlreader->name() == "pen") break;
            // pen
            if (xmlreader->isStartElement() && xmlreader->name() == "pen") {
              linepen1 = xmlreader->readPen(&ok);
              if (!ok)
                xmlreader->raiseWarning(
                    tr("LineSpecialChannel2D line pen property setting error"));
            }
          }

          // line brush property
          while (!xmlreader->atEnd()) {
            xmlreader->readNext();
            if (xmlreader->isEndElement() && xmlreader->name() == "brush")
              break;
            // brush
            if (xmlreader->isStartElement() && xmlreader->name() == "brush") {
              linebrush1 = xmlreader->readBrush(&ok);
              if (!ok)
                xmlreader->raiseWarning(tr(
                    "LineSpecialChannel2D linebrush property setting error"));
            }
          }
        }

        xmlreader->readNext();
        xmlreader->readNext();
        // scatter
        if (xmlreader->isStartElement() && xmlreader->name() == "scatter") {
          // scatter shape
          QString scattershape = xmlreader->readAttributeString("style", &ok);
          if (ok) {
            if (scattershape == "dot") {
              scatterstyle1 = Graph2DCommon::ScatterStyle::Dot;
            } else if (scattershape == "disc") {
              scatterstyle1 = Graph2DCommon::ScatterStyle::Disc;
            } else if (scattershape == "none") {
              scatterstyle1 = Graph2DCommon::ScatterStyle::None;
            } else if (scattershape == "plus") {
              scatterstyle1 = Graph2DCommon::ScatterStyle::Plus;
            } else if (scattershape == "star") {
              scatterstyle1 = Graph2DCommon::ScatterStyle::Star;
            } else if (scattershape == "cross") {
              scatterstyle1 = Graph2DCommon::ScatterStyle::Cross;
            } else if (scattershape == "peace") {
              scatterstyle1 = Graph2DCommon::ScatterStyle::Peace;
            } else if (scattershape == "circle") {
              scatterstyle1 = Graph2DCommon::ScatterStyle::Circle;
            } else if (scattershape == "square") {
              scatterstyle1 = Graph2DCommon::ScatterStyle::Square;
            } else if (scattershape == "diamond") {
              scatterstyle1 = Graph2DCommon::ScatterStyle::Diamond;
            } else if (scattershape == "triangle") {
              scatterstyle1 = Graph2DCommon::ScatterStyle::Triangle;
            } else if (scattershape == "pluscircle") {
              scatterstyle1 = Graph2DCommon::ScatterStyle::PlusCircle;
            } else if (scattershape == "plussquare") {
              scatterstyle1 = Graph2DCommon::ScatterStyle::PlusSquare;
            } else if (scattershape == "crosscircle") {
              scatterstyle1 = Graph2DCommon::ScatterStyle::CrossCircle;
            } else if (scattershape == "crosssquare") {
              scatterstyle1 = Graph2DCommon::ScatterStyle::CrossSquare;
            } else if (scattershape == "triangleinverted") {
              scatterstyle1 = Graph2DCommon::ScatterStyle::TriangleInverted;
            }
          } else
            xmlreader->raiseWarning(tr(
                "LineSpecialChannel2D scatter shape property setting error"));

          // scatter size
          scattersize1 = xmlreader->readAttributeInt("size", &ok);
          if (!ok)
            xmlreader->raiseWarning(
                tr("LineSpecialChannel2D scatter size property setting error"));

          // scatter antialias
          scatterantialias1 = xmlreader->readAttributeBool("antialias", &ok);
          if (!ok)
            xmlreader->raiseWarning(
                tr("LineSpecialChannelD scatter antialias property setting "
                   "error"));

          // scatter pen property
          while (!xmlreader->atEnd()) {
            xmlreader->readNext();
            if (xmlreader->isEndElement() && xmlreader->name() == "pen") break;
            // pen
            if (xmlreader->isStartElement() && xmlreader->name() == "pen") {
              scatterpen1 = xmlreader->readPen(&ok);
              if (!ok)
                xmlreader->raiseWarning(tr(
                    "LineSpecialChannel2D scatter pen property setting error"));
            }
          }

          // scatter brush property
          while (!xmlreader->atEnd()) {
            xmlreader->readNext();
            if (xmlreader->isEndElement() && xmlreader->name() == "brush")
              break;
            // brush
            if (xmlreader->isStartElement() && xmlreader->name() == "brush") {
              scatterbrush1 = xmlreader->readBrush(&ok);
              if (!ok)
                xmlreader->raiseWarning(
                    tr("LineSpecialChannel2D scatterbrush property setting "
                       "error"));
            }
          }
        }
      }
      xmlreader->readNext();
    }

    while (!xmlreader->atEnd()) {
      xmlreader->readNext();
      if (xmlreader->isEndElement() && xmlreader->name() == "linespecial")
        break;
      if (xmlreader->isStartElement() && xmlreader->name() == "linespecial") {
        int xax = xmlreader->readAttributeInt("xaxis", &ok);
        if (ok) {
          xaxis2 = getXAxis(xax);
        } else
          xmlreader->raiseError(
              tr("LineSpecialChannel2D X axis not found error"));
        int yax = xmlreader->readAttributeInt("yaxis", &ok);
        if (ok) {
          yaxis2 = getYAxis(yax);
        } else
          xmlreader->raiseError(
              tr("LineSpecialChannel2D Y axis not found error"));

        QString lstype = xmlreader->readAttributeString("type", &ok);
        if (lstype == "line" && ok) {
          ltype2 = AxisRect2D::LineScatterSpecialType::Area2D;
        } else
          xmlreader->raiseWarning(
              tr("LineSpecialChannel2D line type not found"));

        // legend
        legendvisible2 = xmlreader->readAttributeBool("legendvisible", &ok);
        if (!ok)
          xmlreader->raiseWarning(
              tr("LineSpecialChannel2D legend visible property setting error"));
        legend2 = xmlreader->readAttributeString("legend", &ok);
        if (!ok)
          xmlreader->raiseWarning(
              tr("LineSpecialChannel2D legendtext not found"));
        QString tablename = xmlreader->readAttributeString("table", &ok);
        if (ok) {
          table2 = getTableByName(tabs, tablename);
        } else
          xmlreader->raiseError(
              tr("LineSpecialChannel2D Table not found error"));
        QString xcolname = xmlreader->readAttributeString("xcolumn", &ok);
        if (ok) {
          (table2) ? xcolumn2 = table2->column(xcolname) : xcolumn2 = nullptr;
        } else
          xmlreader->raiseError(
              tr("LineSpecialChannel2D Table X column not found error"));
        QString ycolname = xmlreader->readAttributeString("ycolumn", &ok);
        if (ok) {
          (table2) ? ycolumn2 = table2->column(ycolname) : ycolumn2 = nullptr;
        } else
          xmlreader->raiseError(
              tr("LineSpecialChannel2D Table Y column not found error"));
        from2 = xmlreader->readAttributeInt("from", &ok);
        if (!ok)
          xmlreader->raiseError(
              tr("LineSpecialChannel2D from not found error"));
        to2 = xmlreader->readAttributeInt("to", &ok);
        if (!ok)
          xmlreader->raiseError(tr("LineSpecialChannel2D to not found error"));

        xmlreader->readNextStartElement();
        // line
        if (xmlreader->isStartElement() && xmlreader->name() == "line") {
          // line style
          QString style = xmlreader->readAttributeString("style", &ok);
          if (ok) {
            if (style == "line") {
              lstype2 = Graph2DCommon::LineStyleType::Line;
            } else if (style == "impulse") {
              lstype2 = Graph2DCommon::LineStyleType::Impulse;
            } else if (style == "stepleft") {
              lstype2 = Graph2DCommon::LineStyleType::StepLeft;
            } else if (style == "stepright") {
              lstype2 = Graph2DCommon::LineStyleType::StepRight;
            } else if (style == "stepcenter") {
              lstype2 = Graph2DCommon::LineStyleType::StepCenter;
            }
          } else
            xmlreader->raiseWarning(
                tr("LineSpecialChannel2D line style property setting error"));

          // line fill status
          linefill2 = xmlreader->readAttributeBool("fill", &ok);
          if (!ok)
            xmlreader->raiseWarning(
                tr("LineSpecialChannel2D line fill status property setting "
                   "error"));

          // line antialias
          lineantialias2 = xmlreader->readAttributeBool("antialias", &ok);
          if (!ok)
            xmlreader->raiseWarning(
                tr("LineSpecialChannel2D line antialias "
                   "property setting error"));

          // line pen property
          while (!xmlreader->atEnd()) {
            xmlreader->readNext();
            if (xmlreader->isEndElement() && xmlreader->name() == "pen") break;
            // pen
            if (xmlreader->isStartElement() && xmlreader->name() == "pen") {
              linepen2 = xmlreader->readPen(&ok);
              if (!ok)
                xmlreader->raiseWarning(
                    tr("LineSpecialChannel2D line pen property setting error"));
            }
          }

          // line brush property
          while (!xmlreader->atEnd()) {
            xmlreader->readNext();
            if (xmlreader->isEndElement() && xmlreader->name() == "brush")
              break;
            // brush
            if (xmlreader->isStartElement() && xmlreader->name() == "brush") {
              linebrush2 = xmlreader->readBrush(&ok);
              if (!ok)
                xmlreader->raiseWarning(tr(
                    "LineSpecialChannel2D linebrush property setting error"));
            }
          }
        }

        xmlreader->readNext();
        xmlreader->readNext();
        // scatter
        if (xmlreader->isStartElement() && xmlreader->name() == "scatter") {
          // scatter shape
          QString scattershape = xmlreader->readAttributeString("style", &ok);
          if (ok) {
            if (scattershape == "dot") {
              scatterstyle2 = Graph2DCommon::ScatterStyle::Dot;
            } else if (scattershape == "disc") {
              scatterstyle2 = Graph2DCommon::ScatterStyle::Disc;
            } else if (scattershape == "none") {
              scatterstyle2 = Graph2DCommon::ScatterStyle::None;
            } else if (scattershape == "plus") {
              scatterstyle2 = Graph2DCommon::ScatterStyle::Plus;
            } else if (scattershape == "star") {
              scatterstyle2 = Graph2DCommon::ScatterStyle::Star;
            } else if (scattershape == "cross") {
              scatterstyle2 = Graph2DCommon::ScatterStyle::Cross;
            } else if (scattershape == "peace") {
              scatterstyle2 = Graph2DCommon::ScatterStyle::Peace;
            } else if (scattershape == "circle") {
              scatterstyle2 = Graph2DCommon::ScatterStyle::Circle;
            } else if (scattershape == "square") {
              scatterstyle2 = Graph2DCommon::ScatterStyle::Square;
            } else if (scattershape == "diamond") {
              scatterstyle2 = Graph2DCommon::ScatterStyle::Diamond;
            } else if (scattershape == "triangle") {
              scatterstyle2 = Graph2DCommon::ScatterStyle::Triangle;
            } else if (scattershape == "pluscircle") {
              scatterstyle2 = Graph2DCommon::ScatterStyle::PlusCircle;
            } else if (scattershape == "plussquare") {
              scatterstyle2 = Graph2DCommon::ScatterStyle::PlusSquare;
            } else if (scattershape == "crosscircle") {
              scatterstyle2 = Graph2DCommon::ScatterStyle::CrossCircle;
            } else if (scattershape == "crosssquare") {
              scatterstyle2 = Graph2DCommon::ScatterStyle::CrossSquare;
            } else if (scattershape == "triangleinverted") {
              scatterstyle2 = Graph2DCommon::ScatterStyle::TriangleInverted;
            }
          } else
            xmlreader->raiseWarning(tr(
                "LineSpecialChannel2D scatter shape property setting error"));

          // scatter size
          scattersize2 = xmlreader->readAttributeInt("size", &ok);
          if (!ok)
            xmlreader->raiseWarning(
                tr("LineSpecialChannel2D scatter size property setting error"));

          // scatter antialias
          scatterantialias2 = xmlreader->readAttributeBool("antialias", &ok);
          if (!ok)
            xmlreader->raiseWarning(
                tr("LineSpecialChannelD scatter antialias property setting "
                   "error"));

          // scatter pen property
          while (!xmlreader->atEnd()) {
            xmlreader->readNext();
            if (xmlreader->isEndElement() && xmlreader->name() == "pen") break;
            // pen
            if (xmlreader->isStartElement() && xmlreader->name() == "pen") {
              scatterpen2 = xmlreader->readPen(&ok);
              if (!ok)
                xmlreader->raiseWarning(tr(
                    "LineSpecialChannel2D scatter pen property setting error"));
            }
          }

          // scatter brush property
          while (!xmlreader->atEnd()) {
            xmlreader->readNext();
            if (xmlreader->isEndElement() && xmlreader->name() == "brush")
              break;
            // brush
            if (xmlreader->isStartElement() && xmlreader->name() == "brush") {
              scatterbrush2 = xmlreader->readBrush(&ok);
              if (!ok)
                xmlreader->raiseWarning(
                    tr("LineSpecialChannel2D scatterbrush property setting "
                       "error"));
            }
          }
        }
      }
    }
  }

  if (table1 && table2 && xcolumn1 && xcolumn2 && ycolumn1 && ycolumn2 &&
      xaxis1 && xaxis2 && yaxis1 && yaxis2 && xaxis1 == xaxis2 &&
      yaxis1 == yaxis2 && xcolumn1 == xcolumn2 && table1 == table2 &&
      from1 == from2 && to1 == to2) {
    QPair<LineSpecial2D *, LineSpecial2D *> lspair =
        addLineSpecialChannel2DPlot(table1, xcolumn1, ycolumn1, ycolumn2, from1,
                                    to1, xaxis1, yaxis1);
    // ls1
    lspair.first->setlinetype_lsplot(lstype1);
    lspair.first->setlegendvisible_lsplot(legendvisible1);
    lspair.first->setlegendtext_lsplot(legend1);
    lspair.first->setlinefillstatus_lsplot(linefill1);
    lspair.first->setlineantialiased_lsplot(lineantialias1);
    lspair.first->setlinestrokethickness_lsplot(linepen1.widthF());
    lspair.first->setlinestrokestyle_lsplot(linepen1.style());
    lspair.first->setlinestrokecolor_lsplot(linepen1.color());
    lspair.first->setlinefillcolor_lsplot(linebrush1.color());
    lspair.first->setlinefillstyle_lsplot(linebrush1.style());
    lspair.first->setscattershape_lsplot(scatterstyle1);
    lspair.first->setscattersize_lsplot(scattersize1);
    lspair.first->setscatterantialiased_lsplot(scatterantialias1);
    lspair.first->setscatterstrokethickness_lsplot(scatterpen1.widthF());
    lspair.first->setscatterstrokestyle_lsplot(scatterpen1.style());
    lspair.first->setscatterstrokecolor_lsplot(scatterpen1.color());
    lspair.first->setscatterfillcolor_lsplot(scatterbrush1.color());
    // ls2
    lspair.second->setlinetype_lsplot(lstype2);
    lspair.second->setlegendvisible_lsplot(legendvisible2);
    lspair.second->setlegendtext_lsplot(legend2);
    lspair.second->setlinefillstatus_lsplot(linefill2);
    lspair.second->setlineantialiased_lsplot(lineantialias2);
    lspair.second->setlinestrokethickness_lsplot(linepen2.widthF());
    lspair.second->setlinestrokestyle_lsplot(linepen2.style());
    lspair.second->setlinestrokecolor_lsplot(linepen2.color());
    lspair.second->setlinefillcolor_lsplot(linebrush2.color());
    lspair.second->setscattershape_lsplot(scatterstyle2);
    lspair.second->setscattersize_lsplot(scattersize2);
    lspair.second->setscatterantialiased_lsplot(scatterantialias2);
    lspair.second->setscatterstrokethickness_lsplot(scatterpen2.widthF());
    lspair.second->setscatterstrokestyle_lsplot(scatterpen2.style());
    lspair.second->setscatterstrokecolor_lsplot(scatterpen2.color());
    lspair.second->setscatterfillcolor_lsplot(scatterbrush2.color());
  }

  return ok;
}

void AxisRect2D::save(XmlStreamWriter *xmlwriter, const QPair<int, int> rowcol,
                      LayoutGrid2D *layoutgrid, const bool saveastemplate) {
  xmlwriter->writeStartElement("layout");
  xmlwriter->writeAttribute(
      "index", QString::number(
                   layoutgrid->rowColToIndex(rowcol.first, rowcol.second) + 1));
  xmlwriter->writeAttribute("row", QString::number(rowcol.first + 1));
  xmlwriter->writeAttribute("column", QString::number(rowcol.second + 1));
  xmlwriter->writeAttribute(
      "rowstreachfactor",
      QString::number(layoutgrid->rowStretchFactors().at(rowcol.first)));
  xmlwriter->writeAttribute(
      "columnstreachfactor",
      QString::number(layoutgrid->columnStretchFactors().at(rowcol.second)));
  (autoMargins() == QCP::MarginSide::msAll)
      ? xmlwriter->writeAttribute("automargin", "true")
      : xmlwriter->writeAttribute("automargin", "false");
  xmlwriter->writeAttribute("leftmargin", QString::number(margins().left()));
  xmlwriter->writeAttribute("topmargin", QString::number(margins().top()));
  xmlwriter->writeAttribute("rightmargin", QString::number(margins().right()));
  xmlwriter->writeAttribute("bottommargin",
                            QString::number(margins().bottom()));
  xmlwriter->writeBrush(backgroundBrush());
  getLegend()->save(xmlwriter);
  foreach (Axis2D *axis, getAxes2D()) { axis->save(xmlwriter); }
  gridpair_.first.first->save(xmlwriter, "xgrid");
  gridpair_.second.first->save(xmlwriter, "ygrid");

  if (!saveastemplate) {
    // assign to a new variable
    CurveVec cvec = curvevec_;
    LsVec lvec = lsvec_;
    ChannelVec chvec = channelvec_;
    BarVec bvec = barvec_;
    VectorVec vvec = vectorvec_;
    PieVec pvec = pievec_;
    ColorMapVec colvec = colormapvec_;
    StatBoxVec stvec = statboxvec_;
    TextItemVec txvec = textvec_;
    LineItemVec livec = linevec_;
    ImageItemVec imvec = imagevec_;

    foreach (QCPLayer *layer, layers_) {
      foreach (Curve2D *curve, cvec) {
        if (layer == curve->layer()) {
          curve->save(xmlwriter, getXAxisNo(curve->getxaxis()),
                      getYAxisNo(curve->getyaxis()));
          cvec.removeOne(curve);
          continue;
        }
      }
      foreach (LineSpecial2D *ls, lvec) {
        if (layer == ls->layer()) {
          ls->save(xmlwriter, getXAxisNo(ls->getxaxis()),
                   getYAxisNo(ls->getyaxis()));
          lvec.removeOne(ls);
          continue;
        }
      }
      for (int i = 0; i < chvec.size(); i++) {
        if (layer == chvec.at(i).first->layer()) {
          xmlwriter->writeStartElement("channel");
          chvec.at(i).first->save(xmlwriter,
                                  getXAxisNo(chvec.at(i).first->getxaxis()),
                                  getYAxisNo(chvec.at(i).first->getyaxis()));
          chvec.at(i).second->save(xmlwriter,
                                   getXAxisNo(chvec.at(i).second->getxaxis()),
                                   getYAxisNo(chvec.at(i).second->getyaxis()));
          xmlwriter->writeEndElement();
          chvec.remove(i);
          continue;
        }
      }
      foreach (Bar2D *bar, bvec) {
        if (layer == bar->layer()) {
          (bar->getxaxis()->getorientation_axis() ==
               Axis2D::AxisOreantation::Top ||
           bar->getxaxis()->getorientation_axis() ==
               Axis2D::AxisOreantation::Bottom)
              ? bar->save(xmlwriter, getXAxisNo(bar->getxaxis()),
                          getYAxisNo(bar->getyaxis()))
              : bar->save(xmlwriter, getYAxisNo(bar->getxaxis()),
                          getXAxisNo(bar->getyaxis()));
          bvec.removeOne(bar);
          continue;
        }
      }
      foreach (Vector2D *vector, vvec) {
        if (layer == vector->layer()) {
          vector->save(xmlwriter, getXAxisNo(vector->getxaxis()),
                       getYAxisNo(vector->getyaxis()));
          vvec.removeOne(vector);
          continue;
        }
      }
      foreach (Pie2D *pie, pvec) {
        if (layer == pie->layer()) {
          pie->save(xmlwriter);
          pvec.removeOne(pie);
          continue;
        }
      }
      foreach (StatBox2D *statbox, stvec) {
        if (layer == statbox->layer()) {
          statbox->save(xmlwriter, getXAxisNo(statbox->getxaxis()),
                        getYAxisNo(statbox->getyaxis()));
          stvec.removeOne(statbox);
          continue;
        }
      }
      foreach (ColorMap2D *colmap, colvec) {
        if (layer == colmap->layer()) {
          colmap->save(xmlwriter);
          colvec.removeOne(colmap);
          continue;
        }
      }
      // items
      foreach (TextItem2D *textitem, txvec) {
        if (layer == textitem->layer()) {
          textitem->save(xmlwriter);
          txvec.removeOne(textitem);
          continue;
        }
      }
      foreach (LineItem2D *lineitem, livec) {
        if (layer == lineitem->layer()) {
          lineitem->save(xmlwriter);
          livec.removeOne(lineitem);
          continue;
        }
      }
      foreach (ImageItem2D *imageitem, imvec) {
        if (layer == imageitem->layer()) {
          imageitem->save(xmlwriter);
          imvec.removeOne(imageitem);
          continue;
        }
      }
      qDebug() << "unknown layer: " << layer->name();
    }
  }
  xmlwriter->writeEndElement();
}

bool AxisRect2D::load(XmlStreamReader *xmlreader, QList<Table *> tabs,
                      QList<Matrix *> mats) {
  if (xmlreader->isStartElement() && xmlreader->name() == "layout") {
    bool ok = false;
    // auto margins
    bool automar = xmlreader->readAttributeBool("automargin", &ok);
    if (ok) {
      (automar) ? setAutoMargins(QCP::MarginSide::msAll)
                : setAutoMargins(QCP::MarginSide::msNone);
    } else
      xmlreader->raiseWarning(tr("Layout automargin missing or empty"));
    // margins
    QMargins mar = QMargins();
    mar.setLeft(xmlreader->readAttributeInt("leftmargin", &ok));
    if (ok) {
      mar.setTop(xmlreader->readAttributeInt("topmargin", &ok));
      if (ok) {
        mar.setRight(xmlreader->readAttributeInt("rightmargin", &ok));
        if (ok) {
          mar.setBottom(xmlreader->readAttributeInt("bottommargin", &ok));
          if (ok) {
            setMargins(mar);
          } else
            xmlreader->raiseWarning(
                tr("Layout bottom margin missing or empty"));
        } else
          xmlreader->raiseWarning(tr("Layout right margin missing or empty"));
      } else
        xmlreader->raiseWarning(tr("Layout top margin missing or empty"));
    } else
      xmlreader->raiseWarning(tr("Layout left margin missing or empty"));

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
            curve->setlegendtext_cplot(legend);
          }
        } else if (ok && datatype == "function") {
          PlotData::FunctionData funcdata;
          int functiontype = 0;
          // function type
          QString functype =
              xmlreader->readAttributeString("functiontype", &ok);
          if (!ok)
            xmlreader->raiseWarning(tr("Curve2D function type not found"));

          (functype == "normal")       ? functiontype = 0
          : (functype == "parametric") ? functiontype = 1
          : (functype == "polar")      ? functiontype = 2
                                       : functiontype = 0;

          switch (functiontype) {
            case 0: {
              funcdata.type = 0;
              // function function
              QString funcfunc =
                  xmlreader->readAttributeString("function", &ok);
              if (ok)
                funcdata.functions << funcfunc;
              else
                xmlreader->raiseWarning(
                    tr("Curve2D function function not found"));
            } break;
            case 1: {
              funcdata.type = 1;
              // function functionx
              QString funcfuncx =
                  xmlreader->readAttributeString("functionx", &ok);
              if (ok)
                funcdata.functions << funcfuncx;
              else
                xmlreader->raiseWarning(
                    tr("Curve2D function functionx not found"));
              // function functiony
              QString funcfuncy =
                  xmlreader->readAttributeString("functiony", &ok);
              if (ok)
                funcdata.functions << funcfuncy;
              else
                xmlreader->raiseWarning(
                    tr("Curve2D function functiony not found"));
              // function parameter
              QString funcparam =
                  xmlreader->readAttributeString("parameter", &ok);
              if (ok)
                funcdata.parameter = funcparam;
              else
                xmlreader->raiseWarning(
                    tr("Curve2D function parameter not found"));
            } break;
            case 2: {
              funcdata.type = 2;
              // function functionr
              QString funcfuncr =
                  xmlreader->readAttributeString("functionr", &ok);
              if (ok)
                funcdata.functions << funcfuncr;
              else
                xmlreader->raiseWarning(
                    tr("Curve2D function functionr not found"));
              // function functiontheta
              QString funcfunctheta =
                  xmlreader->readAttributeString("functiontheta", &ok);
              if (ok)
                funcdata.functions << funcfunctheta;
              else
                xmlreader->raiseWarning(
                    tr("Curve2D function functiontheta not found"));
              // function parameter
              QString funcparam =
                  xmlreader->readAttributeString("parameter", &ok);
              if (ok)
                funcdata.parameter = funcparam;
              else
                xmlreader->raiseWarning(
                    tr("Curve2D function parameter not found"));
            } break;
          }

          // Function from
          double funcfrom = xmlreader->readAttributeDouble("from", &ok);
          if (ok)
            funcdata.from = funcfrom;
          else
            xmlreader->raiseWarning(tr("Curve2D function from not found"));

          // Function to
          double functo = xmlreader->readAttributeDouble("to", &ok);
          if (ok)
            funcdata.to = functo;
          else
            xmlreader->raiseWarning(tr("Curve2D function to not found"));

          // Function points
          int funcpoints = xmlreader->readAttributeInt("points", &ok);
          if (ok)
            funcdata.points = funcpoints;
          else
            xmlreader->raiseWarning(tr("Curve2D function points not found"));

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
            curve =
                addFunction2DPlot(funcdata, xdata, ydata, xaxis, yaxis, legend);
            curve->setlegendtext_cplot(legend);
          } else {
            xmlreader->raiseError(tr("Curve2D function skipped due to error"));
            delete xdata;
            delete ydata;
          }

        } else
          xmlreader->raiseError(tr("Curve2D data not found error"));

        bool legendvisible = xmlreader->readAttributeBool("legendvisible", &ok);
        (ok) ? curve->setlegendvisible_cplot(legendvisible)
             : xmlreader->raiseWarning(
                   tr("Curve2D legend visible property setting error"));

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

          // linespecial
          if (xmlreader->isStartElement() &&
              xmlreader->name() == "linespecial") {
        LineSpecial2D *ls = nullptr;
        Axis2D *xaxis = nullptr;
        Axis2D *yaxis = nullptr;
        AxisRect2D::LineScatterSpecialType ltype;

        int xax = xmlreader->readAttributeInt("xaxis", &ok);
        if (ok) {
          xaxis = getXAxis(xax);
        } else
          xmlreader->raiseError(tr("LineSpecial2D X axis not found error"));
        int yax = xmlreader->readAttributeInt("yaxis", &ok);
        if (ok) {
          yaxis = getYAxis(yax);
        } else
          xmlreader->raiseError(tr("LineSpecial2D Y axis not found error"));

        QString lstype = xmlreader->readAttributeString("type", &ok);
        if (lstype == "line" && ok) {
          ltype = AxisRect2D::LineScatterSpecialType::Area2D;
        } else if (lstype == "impulse" && ok) {
          ltype = AxisRect2D::LineScatterSpecialType::VerticalDropLine2D;
        } else if (lstype == "stepleft" && ok) {
          ltype = AxisRect2D::LineScatterSpecialType::VerticalStep2D;
        } else if (lstype == "stepright" && ok) {
          ltype = AxisRect2D::LineScatterSpecialType::HorizontalStep2D;
        } else if (lstype == "stepcenter" && ok) {
          ltype = AxisRect2D::LineScatterSpecialType::CentralStepAndScatter2D;
        } else
          xmlreader->raiseWarning(tr("LineSpecial2D line type not found"));

        // legend
        QString legend = xmlreader->readAttributeString("legend", &ok);
        if (!ok)
          xmlreader->raiseWarning(tr("LineSpecial2D legendtext not found"));

        Table *table = nullptr;
        Column *xcolumn = nullptr;
        Column *ycolumn = nullptr;

        QString tablename = xmlreader->readAttributeString("table", &ok);
        if (ok) {
          table = getTableByName(tabs, tablename);
        } else
          xmlreader->raiseError(tr("LineSpecial2D Table not found error"));
        QString xcolname = xmlreader->readAttributeString("xcolumn", &ok);
        if (ok) {
          (table) ? xcolumn = table->column(xcolname) : xcolumn = nullptr;
        } else
          xmlreader->raiseError(
              tr("LineSpecial2D Table X column not found error"));
        QString ycolname = xmlreader->readAttributeString("ycolumn", &ok);
        if (ok) {
          (table) ? ycolumn = table->column(ycolname) : ycolumn = nullptr;
        } else
          xmlreader->raiseError(
              tr("LineSpecial2D Table Y column not found error"));
        int from = xmlreader->readAttributeInt("from", &ok);
        if (!ok)
          xmlreader->raiseError(tr("LineSpecial2D from not found error"));
        int to = xmlreader->readAttributeInt("to", &ok);
        if (!ok) xmlreader->raiseError(tr("LineSpecial2D to not found error"));

        if (table && xcolumn && ycolumn && xaxis && yaxis) {
          ls = addLineSpecial2DPlot(ltype, table, xcolumn, ycolumn, from, to,
                                    xaxis, yaxis);
          ls->setName(legend);

          while (!xmlreader->atEnd()) {
            xmlreader->readNextStartElement();
            if (xmlreader->isStartElement() &&
                xmlreader->name() != "errorbar") {
              ls->load(xmlreader);
              break;
            }

            if (xmlreader->isStartElement() &&
                xmlreader->name() == "errorbar") {
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
              QString colname =
                  xmlreader->readAttributeString("errcolumn", &ok);
              if (ok) {
                (table) ? column = table->column(colname) : column = nullptr;
              } else
                xmlreader->raiseError(
                    tr("ErrorBar2D Table column not found error"));
              int from = xmlreader->readAttributeInt("from", &ok);
              if (!ok)
                xmlreader->raiseError(tr("ErrorBar2D from not found error"));
              int to = xmlreader->readAttributeInt("to", &ok);
              if (!ok)
                xmlreader->raiseError(tr("ErrorBar2D to not found error"));

              if (table && column && !type.isEmpty()) {
                if (type == "x") {
                  ls->setXerrorBar(table, column, from, to);
                  ls->getxerrorbar_lsplot()->load(xmlreader);
                } else if (type == "y") {
                  ls->setYerrorBar(table, column, from, to);
                  ls->getyerrorbar_lsplot()->load(xmlreader);
                }
              }
            }
          }
        }
      } else

          // channel
          if (xmlreader->isStartElement() && xmlreader->name() == "channel") {
        loadLineSpecialChannel2D(xmlreader, tabs);
      } else

          // bar
          if (xmlreader->isStartElement() && xmlreader->name() == "bar") {
        Bar2D *bar = nullptr;
        Axis2D *xaxis = nullptr;
        Axis2D *yaxis = nullptr;
        AxisRect2D::BarType barorientation = AxisRect2D::BarType::VerticalBars;

        QString orientation =
            xmlreader->readAttributeString("orientation", &ok);
        if (ok) {
          (orientation == "vertical")
              ? barorientation = AxisRect2D::BarType::VerticalBars
              : barorientation = AxisRect2D::BarType::HorizontalBars;
        } else
          xmlreader->raiseError(tr("Bar2D orientation not found error"));

        int xax = xmlreader->readAttributeInt("xaxis", &ok);
        if (ok) {
          xaxis = getXAxis(xax);
        } else
          xmlreader->raiseError(tr("Bar2D X axis not found error"));
        int yax = xmlreader->readAttributeInt("yaxis", &ok);
        if (ok) {
          yaxis = getYAxis(yax);
        } else
          xmlreader->raiseError(tr("Bar2D Y axis not found error"));

        // legend
        QString legend = xmlreader->readAttributeString("legend", &ok);
        if (!ok) xmlreader->raiseWarning(tr("Bar2D legendtext not found"));

        QString bartype = xmlreader->readAttributeString("type", &ok);
        if (!ok) xmlreader->raiseError(tr("Bar2D type not found"));

        if (bartype == "barxy") {
          Table *table = nullptr;
          Column *xcolumn = nullptr;
          Column *ycolumn = nullptr;

          QString tablename = xmlreader->readAttributeString("table", &ok);
          if (ok) {
            table = getTableByName(tabs, tablename);
          } else
            xmlreader->raiseError(tr("Bar2D Table not found error"));
          QString xcolname = xmlreader->readAttributeString("xcolumn", &ok);
          if (ok) {
            (table) ? xcolumn = table->column(xcolname) : xcolumn = nullptr;
          } else
            xmlreader->raiseError(tr("Bar2D Table X column not found error"));
          QString ycolname = xmlreader->readAttributeString("ycolumn", &ok);
          if (ok) {
            (table) ? ycolumn = table->column(ycolname) : ycolumn = nullptr;
          } else
            xmlreader->raiseError(tr("Bar2D Table Y column not found error"));
          int from = xmlreader->readAttributeInt("from", &ok);
          if (!ok) xmlreader->raiseError(tr("Bar2D from not found error"));
          int to = xmlreader->readAttributeInt("to", &ok);
          if (!ok) xmlreader->raiseError(tr("Bar2D to not found error"));

          Bar2D::BarStyle barstyleenum;
          bool barstylstatus = false;
          QString barstyle = xmlreader->readAttributeString("style", &ok);
          if (ok) {
            barstylstatus = true;
            (barstyle == "individual")
                ? barstyleenum = Bar2D::BarStyle::Individual
            : (barstyle == "grouped") ? barstyleenum = Bar2D::BarStyle::Grouped
            : (barstyle == "stacked")
                ? barstyleenum = Bar2D::BarStyle::Stacked
                : barstyleenum = Bar2D::BarStyle::Individual;
          } else
            xmlreader->raiseWarning(tr("Bar2D xy style not found error"));

          int stackorder = xmlreader->readAttributeInt("stackorder", &ok);
          if (!ok)
            xmlreader->raiseWarning(tr("Bar2D stackorder not found error"));

          // compatibility with previous version
          if (!barstylstatus)
            (stackorder == -1) ? barstyleenum = Bar2D::BarStyle::Individual
                               : barstyleenum = Bar2D::BarStyle::Stacked;

          if (table && xcolumn && ycolumn && xaxis && yaxis)
            bar = addBox2DPlot(barorientation, table, xcolumn, ycolumn, from,
                               to, xaxis, yaxis, barstyleenum, stackorder);

        } else if (ok && bartype == "histogram") {
          Table *table = nullptr;
          Column *column = nullptr;

          QString tablename = xmlreader->readAttributeString("table", &ok);
          if (ok) {
            table = getTableByName(tabs, tablename);
          } else
            xmlreader->raiseError(tr("Bar2D Table not found error"));
          QString colname = xmlreader->readAttributeString("column", &ok);
          if (ok) {
            (table) ? column = table->column(colname) : column = nullptr;
          } else
            xmlreader->raiseError(tr("Bar2D Table column not found error"));
          // from
          int from = xmlreader->readAttributeInt("from", &ok);
          if (!ok) xmlreader->raiseError(tr("Bar2D from not found error"));
          // to
          int to = xmlreader->readAttributeInt("to", &ok);
          if (!ok) xmlreader->raiseError(tr("Bar2D to not found error"));

          if (table && column && xaxis && yaxis) {
            bar = addHistogram2DPlot(barorientation, table, column, from, to,
                                     xaxis, yaxis);
            // autobin
            bool autobin = xmlreader->readAttributeBool("autobin", &ok);
            if (ok)
              bar->setHistAutoBin(autobin);
            else
              xmlreader->raiseWarning(tr("Bar2D histogram auto bin not found"));

            // binsize
            double binsize = xmlreader->readAttributeDouble("binsize", &ok);
            if (ok)
              bar->setHistBinSize(binsize);
            else
              xmlreader->raiseWarning(tr("Bar2D histogram bin size not found"));

            // begin
            double begin = xmlreader->readAttributeDouble("begin", &ok);
            if (ok)
              bar->setHistBegin(begin);
            else
              xmlreader->raiseWarning(tr("Bar2D histogram begin not found"));

            // end
            double end = xmlreader->readAttributeDouble("end", &ok);
            if (ok)
              bar->setHistEnd(end);
            else
              xmlreader->raiseWarning(tr("Bar2D histogram end not found"));
          }
        } else
          xmlreader->raiseError(tr("Bar2D type not found error"));
        bar->setName(legend);

        // stackgap
        double stackgap = xmlreader->readAttributeDouble("stackgap", &ok);
        if (ok) {
          bar->setStackingGap(stackgap);
        } else
          xmlreader->raiseWarning(tr("Bar2D Stacking Gap not found"));

        // error bars
        while (!xmlreader->atEnd()) {
          xmlreader->readNextStartElement();
          if (xmlreader->isStartElement() && xmlreader->name() != "errorbar") {
            bar->load(xmlreader);
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
                !bar->ishistogram_barplot()) {
              if (type == "x") {
                bar->setXerrorBar(table, column, from, to);
                bar->getxerrorbar_barplot()->load(xmlreader);
              } else if (type == "y") {
                bar->setYerrorBar(table, column, from, to);
                bar->getyerrorbar_barplot()->load(xmlreader);
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
        Column *ycolumn = nullptr;
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

        QString ycolname = xmlreader->readAttributeString("ycolumn", &ok);
        if (ok) {
          (table) ? ycolumn = table->column(ycolname) : ycolumn = nullptr;
        } else
          xmlreader->raiseError(tr("Pie2D Table Y column not found error"));

        int from = xmlreader->readAttributeInt("from", &ok);
        if (!ok) xmlreader->raiseError(tr("Pie2D from not found error"));
        int to = xmlreader->readAttributeInt("to", &ok);
        if (!ok) xmlreader->raiseError(tr("Pie2D to not found error"));
        if (table && xcolumn) {
          Pie2D *pie = addPie2DPlot(Graph2DCommon::PieStyle::Pie, table,
                                    xcolumn, ycolumn, from, to);
          pie->load(xmlreader);
        }
      }

      // statbox
      if (xmlreader->isStartElement() && xmlreader->name() == "statbox") {
        Axis2D *xaxis = nullptr;
        Axis2D *yaxis = nullptr;

        // axis
        int xax = xmlreader->readAttributeInt("xaxis", &ok);
        if (ok) {
          xaxis = getXAxis(xax);
        } else
          xmlreader->raiseError(tr("StatBox2D X axis not found error"));
        int yax = xmlreader->readAttributeInt("yaxis", &ok);
        if (ok) {
          yaxis = getYAxis(yax);
        } else
          xmlreader->raiseError(tr("StatBox2D Y axis not found error"));

        QString legendtext = xmlreader->readAttributeString("legend", &ok);
        if (!ok)
          xmlreader->raiseWarning(tr("StatBox2D legend text not found error"));

        Table *table = nullptr;
        Column *column = nullptr;

        QString tablename = xmlreader->readAttributeString("table", &ok);
        if (ok) {
          table = getTableByName(tabs, tablename);
        } else
          xmlreader->raiseError(tr("StatBox2D Table not found error"));
        QString colname = xmlreader->readAttributeString("column", &ok);
        if (ok) {
          (table) ? column = table->column(colname) : column = nullptr;
        } else
          xmlreader->raiseError(tr("StatBox2D Table column not found error"));

        int from = xmlreader->readAttributeInt("from", &ok);
        if (!ok) xmlreader->raiseError(tr("StatBox2D from not found error"));
        int to = xmlreader->readAttributeInt("to", &ok);
        if (!ok) xmlreader->raiseError(tr("StatBox2D to not found error"));
        int key = xmlreader->readAttributeInt("key", &ok);
        if (!ok) xmlreader->raiseError(tr("StatBox2D key not found error"));

        if (table && column && xaxis && yaxis) {
          Layout2D *lout =
              qobject_cast<Layout2D *>(plot2d_->parent()->parent());
          if (lout) {
            StatBox2D::BoxWhiskerData sbdata =
                lout->generateBoxWhiskerData(table, column, from, to, key);
            QSharedPointer<QCPAxisTickerText> textTicker =
                qSharedPointerCast<QCPAxisTickerText>(xaxis->getticker_axis());
            StatBox2D *statbox = addStatBox2DPlot(sbdata, xaxis, yaxis);
            textTicker->addTick(sbdata.key, sbdata.name);
            xaxis->setTicker(textTicker);
            statbox->load(xmlreader);
          }
        }
      } else

          // colormap
          if (xmlreader->isStartElement() && xmlreader->name() == "colormap") {
        Matrix *matrix = nullptr;
        QString matname = xmlreader->readAttributeString("matrix", &ok);
        if (ok) {
          matrix = getMatrixByName(mats, matname);
        } else
          xmlreader->raiseError(tr("ColorMap2D Matrix not found error"));

        if (matrix) {
          ColorMap2D *colmap =
              addColorMap2DPlot(matrix, getXAxes2D().at(0), getYAxes2D().at(0));
          colmap->load(xmlreader);
        }
      }
      if (xmlreader->isStartElement() && xmlreader->name() == "legend") {
        getLegend()->load(xmlreader);
      }
    }
  } else  // no plot2d element
    xmlreader->raiseError(tr("unknown element %1").arg(xmlreader->name()));

  setbarsstyle();

  return !xmlreader->hasError();
}

void AxisRect2D::mousePressEvent(QMouseEvent *event, const QVariant &variant) {
  if (picker_->getPicker() == Graph2DCommon::Picker::DataGraph &&
      event->button() == Qt::MouseButton::LeftButton)
    picker_->showtooltip(
        event->pos(),
        gridpair_.first.second->pixelToCoord(event->localPos().x()),
        gridpair_.second.second->pixelToCoord(event->localPos().y()),
        gridpair_.first.second, gridpair_.second.second);

  emit AxisRectClicked(this);
  QCPAxisRect::mousePressEvent(event, variant);
}

void AxisRect2D::mouseMoveEvent(QMouseEvent *event, const QPointF &startPos) {
  if (picker_->getPicker() == Graph2DCommon::Picker::DataGraph)
    picker_->showtooltip(
        event->localPos(),
        gridpair_.first.second->pixelToCoord(event->localPos().x()),
        gridpair_.second.second->pixelToCoord(event->localPos().y()),
        gridpair_.first.second, gridpair_.second.second);
  else if (picker_->getPicker() == Graph2DCommon::Picker::DataRange)
    picker_->rangepickermousedrag(
        event->pos(),
        gridpair_.first.second->pixelToCoord(event->localPos().x()),
        gridpair_.second.second->pixelToCoord(event->localPos().y()));
  else if (picker_->getPicker() == Graph2DCommon::Picker::DataMove)
    picker_->movepickermousedrag(
        event->pos(),
        gridpair_.first.second->pixelToCoord(event->localPos().x()),
        gridpair_.second.second->pixelToCoord(event->localPos().y()));

  QCPAxisRect::mouseMoveEvent(event, startPos);
}

void AxisRect2D::mouseReleaseEvent(QMouseEvent *event, const QPointF &) {
  if (picker_->getPicker() == Graph2DCommon::Picker::DataRange)
    picker_->rangepickermouserelease(event->localPos());
  else if (picker_->getPicker() == Graph2DCommon::Picker::DataMove)
    picker_->movepickermouserelease(event->localPos());
}

void AxisRect2D::draw(QCPPainter *painter) {
  QCPAxisRect::draw(painter);
  if (printorexportjob_) return;
  if (isAxisRectSelected_) drawSelection(painter);
}

void AxisRect2D::legendClick() { emit AxisRectClicked(this); }
