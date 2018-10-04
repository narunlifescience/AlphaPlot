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
#include "Legend2D.h"
#include "QMessageBox"
#include "Table.h"
#include "TextItem2D.h"

#include "core/Utilities.h"

#include <QMenu>

AxisRect2D::AxisRect2D(Plot2D *parent, bool setupDefaultAxis)
    : QCPAxisRect(parent, setupDefaultAxis),
      plot2d_(parent),
      axisRectBackGround_(Qt::white),
      axisRectLegend_(new Legend2D()),
      isAxisRectSelected_(false),
      printorexportjob_(false) {
  gridpair_.first.first = nullptr;
  gridpair_.first.second = nullptr;
  gridpair_.second.first = nullptr;
  gridpair_.second.second = nullptr;
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
  bool status = true;
  foreach (LineScatter2D *ls, lsvec_) {
    if (ls->getxaxis_lsplot() == axis || ls->getyaxis_lsplot() == axis)
      status = false;
  }

  foreach (Curve2D *curve, curvevec_) {
    if (curve->getxaxis_cplot() == axis || curve->getyaxis_cplot() == axis)
      status = false;
  }

  foreach (Spline2D *spline, splinevec_) {
    if (spline->getxaxis_splot() == axis || spline->getyaxis_splot() == axis)
      status = false;
  }

  foreach (Vector2D *vec, vectorvec_) {
    if (vec->getxaxis_vecplot() == axis || vec->getyaxis_vecplot() == axis)
      status = false;
  }

  foreach (Bar2D *bar, barvec_) {
    if (bar->getxaxis_barplot() == axis || bar->getyaxis_barplot() == axis)
      status = false;
  }

  if (!status) {
    QMessageBox::warning(
        nullptr, tr("Axis associated with plot"),
        tr("This axis is associated with a plot! eithor remove the plot or "
           "change the plot to anothor axis"));
    return false;
  }

  status = removeAxis(static_cast<QCPAxis *>(axis));
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
      if (gridpair_.first.second == axis) return gridpair_.first.first;
      if (gridpair_.first.first != nullptr) delete gridpair_.first.first;
      gridpair_.first.first = nullptr;
      gridpair_.first.second = nullptr;
      gridpair_.first.first = new Grid2D(axis);
      gridpair_.first.second = axis;
      return gridpair_.first.first;
    case Axis2D::AxisOreantation::Left:
    case Axis2D::AxisOreantation::Right:
      if (gridpair_.second.second == axis) return gridpair_.second.first;
      if (gridpair_.second.first != nullptr) delete gridpair_.second.first;
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
    const AxisRect2D::LineScatterType &type, Table *table, Column *xData,
    Column *yData, int from, int to, Axis2D *xAxis, Axis2D *yAxis) {
  LineScatter2D *lineScatter =
      new LineScatter2D(table, xData, yData, from, to, xAxis, yAxis);
  lineScatter->setlinefillcolor_lsplot(
      Utilities::getRandColorGoldenRatio(Utilities::ColorPal::Light));

  switch (type) {
    case Line2D:
      lineScatter->setlinetype_lsplot(LSCommon::LineStyleType::Line);
      lineScatter->setscattershape_lsplot(LSCommon::ScatterStyle::None);
      lineScatter->setlinefillstatus_lsplot(false);
      lineScatter->setlineantialiased_lsplot(true);
      lineScatter->setscatterantialiased_lsplot(true);
      break;
    case Scatter2D:
      lineScatter->setlinetype_lsplot(LSCommon::LineStyleType::None);
      lineScatter->setscattershape_lsplot(LSCommon::ScatterStyle::Disc);
      lineScatter->setlinefillstatus_lsplot(false);
      lineScatter->setlineantialiased_lsplot(false);
      lineScatter->setscatterantialiased_lsplot(true);
      break;
    case LineAndScatter2D:
      lineScatter->setlinetype_lsplot(LSCommon::LineStyleType::Line);
      lineScatter->setscattershape_lsplot(LSCommon::ScatterStyle::Disc);
      lineScatter->setlinefillstatus_lsplot(false);
      lineScatter->setlineantialiased_lsplot(true);
      lineScatter->setscatterantialiased_lsplot(true);
      break;
    case VerticalDropLine2D:
      lineScatter->setlinetype_lsplot(LSCommon::LineStyleType::Impulse);
      lineScatter->setscattershape_lsplot(LSCommon::ScatterStyle::Disc);
      lineScatter->setlinefillstatus_lsplot(false);
      lineScatter->setlineantialiased_lsplot(false);
      lineScatter->setscatterantialiased_lsplot(true);
      break;
    case CentralStepAndScatter2D:
      lineScatter->setlinetype_lsplot(LSCommon::LineStyleType::StepCenter);
      lineScatter->setscattershape_lsplot(LSCommon::ScatterStyle::Disc);
      lineScatter->setlinefillstatus_lsplot(false);
      lineScatter->setlineantialiased_lsplot(false);
      lineScatter->setscatterantialiased_lsplot(true);
      break;
    case HorizontalStep2D:
      lineScatter->setlinetype_lsplot(LSCommon::LineStyleType::StepRight);
      lineScatter->setscattershape_lsplot(LSCommon::ScatterStyle::None);
      lineScatter->setlinefillstatus_lsplot(false);
      lineScatter->setlineantialiased_lsplot(false);
      lineScatter->setscatterantialiased_lsplot(true);
      break;
    case VerticalStep2D:
      lineScatter->setlinetype_lsplot(LSCommon::LineStyleType::StepLeft);
      lineScatter->setscattershape_lsplot(LSCommon::ScatterStyle::None);
      lineScatter->setlinefillstatus_lsplot(false);
      lineScatter->setlineantialiased_lsplot(false);
      lineScatter->setscatterantialiased_lsplot(true);
      break;
    case Area2D:
      lineScatter->setlinetype_lsplot(LSCommon::LineStyleType::Line);
      lineScatter->setscattershape_lsplot(LSCommon::ScatterStyle::None);
      lineScatter->setlinefillstatus_lsplot(true);
      lineScatter->setlineantialiased_lsplot(true);
      lineScatter->setscatterantialiased_lsplot(true);
      break;
  }

  LegendItem2D *legendItem = new LegendItem2D(axisRectLegend_, lineScatter);
  axisRectLegend_->addItem(legendItem);
  connect(legendItem, SIGNAL(legendItemClicked()), SLOT(legendClick()));
  lineScatter->setName(table->name() + "_" + xData->name() + "_" +
                       yData->name());
  lsvec_.append(lineScatter);

  emit LineScatterCreated(lineScatter);
  return lineScatter;
}

Curve2D *AxisRect2D::addCurve2DPlot(Table *table, Column *xcol, Column *ycol,
                                    int from, int to, Axis2D *xAxis,
                                    Axis2D *yAxis) {
  Curve2D *curve = new Curve2D(table, xcol, ycol, from, to, xAxis, yAxis);
  LegendItem2D *legendItem = new LegendItem2D(axisRectLegend_, curve);
  axisRectLegend_->addItem(legendItem);
  connect(legendItem, SIGNAL(legendItemClicked()), SLOT(legendClick()));
  curve->setName(table->name() + "_" + xcol->name() + "_" + ycol->name());
  curvevec_.append(curve);

  emit CurveCreated(curve);
  return curve;
}

Spline2D *AxisRect2D::addSpline2DPlot(Table *table, Column *xData,
                                      Column *yData, int from, int to,
                                      Axis2D *xAxis, Axis2D *yAxis) {
  Spline2D *spline = new Spline2D(xAxis, yAxis);
  spline->setlinefillcolor_splot(
      Utilities::getRandColorGoldenRatio(Utilities::ColorPal::Light));
  spline->setGraphData(xData, yData, from, to);
  LegendItem2D *legendItem = new LegendItem2D(axisRectLegend_, spline);
  axisRectLegend_->addItem(legendItem);
  connect(legendItem, SIGNAL(legendItemClicked()), SLOT(legendClick()));
  spline->setName(table->name() + "_" + xData->name() + "_" + yData->name());
  splinevec_.append(spline);

  emit SplineCreated(spline);
  return spline;
}

LineScatter2D *AxisRect2D::addLineFunction2DPlot(QVector<double> *xdata,
                                                 QVector<double> *ydata,
                                                 Axis2D *xAxis, Axis2D *yAxis,
                                                 const QString &name) {
  LineScatter2D *lineScatter = new LineScatter2D(xdata, ydata, xAxis, yAxis);
  lineScatter->setlinetype_lsplot(LSCommon::LineStyleType::Line);
  lineScatter->setscattershape_lsplot(LSCommon::ScatterStyle::None);

  lineScatter->setName(name);
  LegendItem2D *legendItem = new LegendItem2D(axisRectLegend_, lineScatter);
  axisRectLegend_->addItem(legendItem);
  connect(legendItem, SIGNAL(legendItemClicked()), SLOT(legendClick()));
  lsvec_.append(lineScatter);

  emit LineScatterCreated(lineScatter);
  return lineScatter;
}

Curve2D *AxisRect2D::addCurveFunction2DPlot(QVector<double> *xdata,
                                            QVector<double> *ydata,
                                            Axis2D *xAxis, Axis2D *yAxis,
                                            const QString &name) {
  Curve2D *curve = new Curve2D(xdata, ydata, xAxis, yAxis);
  curve->setlinetype_cplot(1);
  curve->setscattershape_cplot(LSCommon::ScatterStyle::None);

  curve->setName(name);
  LegendItem2D *legendItem = new LegendItem2D(axisRectLegend_, curve);
  axisRectLegend_->addItem(legendItem);
  connect(legendItem, SIGNAL(legendItemClicked()), SLOT(legendClick()));
  curvevec_.append(curve);

  emit CurveCreated(curve);
  return curve;
}

Bar2D *AxisRect2D::addBox2DPlot(const AxisRect2D::BarType &type, Table *table,
                                Column *xData, Column *yData, int from, int to,
                                Axis2D *xAxis, Axis2D *yAxis) {
  Bar2D *bar;
  switch (type) {
    case HorizontalBars:
      bar = new Bar2D(table, xData, yData, from, to, yAxis, xAxis);
      break;
    case VerticalBars:
      bar = new Bar2D(table, xData, yData, from, to, xAxis, yAxis);
      break;
  }

  bar->setBarData(table, xData, yData, from, to);
  bar->setBarWidth(1);
  bar->setAntialiased(false);
  bar->setAntialiasedFill(false);
  LegendItem2D *legendItem = new LegendItem2D(axisRectLegend_, bar);
  axisRectLegend_->addItem(legendItem);
  connect(legendItem, SIGNAL(legendItemClicked()), SLOT(legendClick()));
  bar->setName(table->name() + "_" + xData->name() + "_" + yData->name());
  barvec_.append(bar);

  emit BarCreated(bar);
  return bar;
}

Vector2D *AxisRect2D::addVectorPlot(const Vector2D::VectorPlot &vectorplot,
                                    Table *table, Column *x1Data,
                                    Column *y1Data, Column *x2Data,
                                    Column *y2Data, int from, int to,
                                    Axis2D *xAxis, Axis2D *yAxis) {
  Vector2D *vec = new Vector2D(xAxis, yAxis);
  vec->setGraphData(vectorplot, x1Data, y1Data, x2Data, y2Data, from, to);
  LegendItem2D *legendItem = new LegendItem2D(axisRectLegend_, vec);
  axisRectLegend_->addItem(legendItem);
  connect(legendItem, SIGNAL(legendItemClicked()), SLOT(legendClick()));
  vec->setName(table->name() + "_" + x1Data->name() + "_" + y1Data->name() +
               "_" + x2Data->name() + "_" + y2Data->name());
  vectorvec_.append(vec);

  emit VectorCreated(vec);
  return vec;
}

StatBox2D *AxisRect2D::addStatBox2DPlot(Table *table,
                                        StatBox2D::BoxWhiskerData data,
                                        Axis2D *xAxis, Axis2D *yAxis) {
  StatBox2D *statbox = new StatBox2D(xAxis, yAxis, table, data);
  LegendItem2D *legendItem = new LegendItem2D(axisRectLegend_, statbox);
  axisRectLegend_->addItem(legendItem);
  connect(legendItem, SIGNAL(legendItemClicked()), SLOT(legendClick()));
  statbox->setName(data.name);
  statboxvec_.append(statbox);

  emit StatBox2DCreated(statbox);
  return statbox;
}

Pie2D *AxisRect2D::addPie2DPlot(Table *table, Column *xData, int from, int to) {
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

TextItem2D *AxisRect2D::addTextItem2D(QString text) {
  TextItem2D *textitem = new TextItem2D(this, plot2d_);
  textitem->setText(text);
  textitem->position->setPixelPosition(this->rect().center());
  textvec_.append(textitem);
  emit TextItem2DCreated(textitem);
  return textitem;
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

bool AxisRect2D::removeTextItem2D(TextItem2D *textitem) {
  for (int i = 0; i < textvec_.size(); i++) {
    if (textvec_.at(i) == textitem) {
      textvec_.remove(i);
    }
  }
  bool result = false;
  result = plot2d_->removeItem(textitem);
  if (!result) return result;

  emit TextItem2DRemoved(this);
  return result;
}

bool AxisRect2D::removeLineScatter2D(LineScatter2D *ls) {
  for (int i = 0; i < lsvec_.size(); i++) {
    if (lsvec_.at(i) == ls) {
      lsvec_.remove(i);
    }
  }

  bool result = false;
  result = plot2d_->removeGraph(ls);
  // result = plot2d_->removePlottable(ls);
  if (!result) return result;

  emit LineScatterRemoved(this);
  return result;
}

bool AxisRect2D::removeSpline2D(Spline2D *spline) {
  for (int i = 0; i < splinevec_.size(); i++) {
    if (splinevec_.at(i) == spline) {
      splinevec_.remove(i);
    }
  }
  bool result = false;
  result = plot2d_->removePlottable(spline);
  emit Spline2DRemoved(this);
  return result;
}

bool AxisRect2D::removeCurve2D(Curve2D *curve) {
  for (int i = 0; i < curvevec_.size(); i++) {
    if (curvevec_.at(i) == curve) {
      curvevec_.remove(i);
    }
  }
  bool result = false;
  result = plot2d_->removePlottable(curve);
  emit CurveRemoved(this);
  return result;
}

bool AxisRect2D::removeBar2D(Bar2D *bar) {
  for (int i = 0; i < barvec_.size(); i++) {
    if (barvec_.at(i) == bar) {
      barvec_.remove(i);
    }
  }
  bool result = false;
  result = plot2d_->removePlottable(bar);
  emit BarRemoved(this);
  return result;
}

void AxisRect2D::mousePressEvent(QMouseEvent *, const QVariant &) {
  emit AxisRectClicked(this);
}

void AxisRect2D::draw(QCPPainter *painter) {
  if (printorexportjob_) return;
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

void AxisRect2D::exportGraph() {
  /*ImageExportDialog2D *ied =
      new ImageExportDialog2D(this, plot2D != NULL, d_extended_export_dialog);
  ied->setDirectory(workingDir);
  ied->selectFilter(d_image_export_filter);
  if (ied->exec() != QDialog::Accepted) return;
  workingDir = ied->directory().path();
  if (ied->selectedFiles().isEmpty()) return;

  QString selected_filter = ied->selectedFilter();
  QString file_name = ied->selectedFiles()[0];
  QFileInfo file_info(file_name);
  if (!file_info.fileName().contains("."))
    file_name.append(selected_filter.remove("*"));

  QFile file(file_name);
  if (!file.open(QIODevice::WriteOnly)) {
    QMessageBox::critical(
        this, tr("Export Error"),
        tr("Could not write to file: <br><h4> %1 </h4><p>Please verify that "
           "you have the right to write to this location!")
            .arg(file_name));
    return;
  }*/
}

void AxisRect2D::addplot() {}
