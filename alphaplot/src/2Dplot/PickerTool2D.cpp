/* This file is part of AlphaPlot.
   Copyright 2021, Arun Narayanankutty <n.arun.lifescience@gmail.com>

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

   Description : graph picker tools */

#include "PickerTool2D.h"

#include "Curve2D.h"
#include "DataManager2D.h"
#include "LineSpecial2D.h"
#include "Plot2D.h"

const int PickerTool2D::ellipseradius_ = 10;

PickerTool2D::PickerTool2D(Layout2D *parent)
    : QObject(parent),
      layout_(parent),
      curve_(nullptr),
      ls_(nullptr),
      bar_(nullptr),
      picker_(Graph2DCommon::Picker::None),
      xpickerline_(nullptr),
      ypickerline_(nullptr),
      xpickerellipse_(nullptr),
      ypickerellipse_(nullptr) {}

PickerTool2D::~PickerTool2D() {}

void PickerTool2D::setPicker(const Graph2DCommon::Picker &picker) {
  picker_ = picker;
  layout_->streachLabelSetText(QString());
  switch (picker_) {
    case Graph2DCommon::Picker::None: {
      removePickerLinesAndEllipses();
      layout_->getPlotCanwas()->unsetCursor();
      layout_->setAxisRangeDrag(false);
      layout_->setAxisRangeZoom(false);
    } break;
    case Graph2DCommon::Picker::DataPoint: {
      removePickerLinesAndEllipses();
      xpickerline_ = new QCPItemStraightLine(layout_->getPlotCanwas());
      ypickerline_ = new QCPItemStraightLine(layout_->getPlotCanwas());
      xpickerline_->setLayer("picker");
      ypickerline_->setLayer("picker");
      xpickerline_->setVisible(false);
      ypickerline_->setVisible(false);
      layout_->getPlotCanwas()->setCursor(Qt::CursorShape::CrossCursor);
      layout_->setAxisRangeDrag(false);
      layout_->setAxisRangeZoom(false);
    } break;
    case Graph2DCommon::Picker::DataGraph: {
      removePickerLinesAndEllipses();
      xpickerline_ = new QCPItemStraightLine(layout_->getPlotCanwas());
      ypickerline_ = new QCPItemStraightLine(layout_->getPlotCanwas());
      xpickerline_->setLayer("picker");
      ypickerline_->setLayer("picker");
      xpickerline_->setVisible(false);
      ypickerline_->setVisible(false);
      layout_->getPlotCanwas()->setCursor(Qt::CursorShape::CrossCursor);
      layout_->setAxisRangeDrag(false);
      layout_->setAxisRangeZoom(false);
    } break;
    case Graph2DCommon::Picker::DataMove: {
      removePickerLinesAndEllipses();
      layout_->getPlotCanwas()->setCursor(Qt::CursorShape::PointingHandCursor);
      xpickerellipse_ = new QCPItemEllipse(layout_->getPlotCanwas());
      xpickerellipse_->setLayer("picker");
      xpickerellipse_->setVisible(false);
      layout_->setAxisRangeDrag(false);
      layout_->setAxisRangeZoom(false);
    } break;
    case Graph2DCommon::Picker::DataRemove: {
      removePickerLinesAndEllipses();
      layout_->getPlotCanwas()->setCursor(Qt::CursorShape::PointingHandCursor);
      layout_->setAxisRangeDrag(false);
      layout_->setAxisRangeZoom(false);
    } break;
    case Graph2DCommon::Picker::DragRange: {
      removePickerLinesAndEllipses();
      layout_->getPlotCanwas()->setCursor(Qt::CursorShape::SizeAllCursor);
      layout_->setAxisRangeDrag(true);
      layout_->setAxisRangeZoom(false);
    } break;
    case Graph2DCommon::Picker::ZoomRange: {
      removePickerLinesAndEllipses();
      layout_->getPlotCanwas()->setCursor(
          QCursor(QPixmap(":icons/cursor/cursor-zoom.png")));
      layout_->setAxisRangeDrag(false);
      layout_->setAxisRangeZoom(true);
    } break;
    case Graph2DCommon::Picker::DataRange: {
      removePickerLinesAndEllipses();
      resetDataRangePicker();
      layout_->getPlotCanwas()->unsetCursor();
      if (curve_) {
        xpickerline_ = new QCPItemStraightLine(layout_->getPlotCanwas());
        ypickerline_ = new QCPItemStraightLine(layout_->getPlotCanwas());
        xpickerellipse_ = new QCPItemEllipse(layout_->getPlotCanwas());
        ypickerellipse_ = new QCPItemEllipse(layout_->getPlotCanwas());
        xpickerline_->setLayer("picker");
        ypickerline_->setLayer("picker");
        xpickerellipse_->setLayer("picker");
        ypickerellipse_->setLayer("picker");
        layout_->setAxisRangeDrag(false);
        layout_->setAxisRangeZoom(false);
        setupRangepicker();
      }
    } break;
  }
  layout_->getPlotCanwas()->replot(
      QCustomPlot::RefreshPriority::rpQueuedReplot);
}

void PickerTool2D::setRangePickerCurve(Curve2D *curve) { curve_ = curve; }

Graph2DCommon::Picker PickerTool2D::getPicker() const { return picker_; }

Curve2D *PickerTool2D::getRangePickerCurve() { return curve_; }

QPair<double, double> PickerTool2D::getRangePickerLower() const {
  return QPair<double, double>(rangepicker_.lowerx, rangepicker_.lowery);
}

QPair<double, double> PickerTool2D::getRangePickerUpper() const {
  return QPair<double, double>(rangepicker_.upperx, rangepicker_.uppery);
}

void PickerTool2D::showtooltip(const QPointF position, const double xval,
                               const double yval, Axis2D *xaxis,
                               Axis2D *yaxis) {
  QToolTip::showText(
      layout_->mapToGlobal(QPoint(static_cast<int>(position.x()),
                                  static_cast<int>(position.y()))),
      QString::number(xval) + ", " + QString::number(yval));
  xpickerline_->setPen(QPen(Qt::red, 1));
  ypickerline_->setPen(QPen(Qt::red, 1));
  xpickerline_->setAntialiased(false);
  ypickerline_->setAntialiased(false);
  xpickerline_->setVisible(true);
  ypickerline_->setVisible(true);
  foreach (QCPItemPosition *position, xpickerline_->positions()) {
    position->setAxes(xaxis, yaxis);
  }
  xpickerline_->setClipAxisRect(xaxis->axisRect());
  xpickerline_->setClipToAxisRect(true);
  xpickerline_->position("point1")->setCoords(xval, yaxis->range().lower);
  xpickerline_->position("point2")->setCoords(xval, yaxis->range().upper);
  foreach (QCPItemPosition *position, ypickerline_->positions()) {
    position->setAxes(xaxis, yaxis);
  }
  ypickerline_->setClipAxisRect(xaxis->axisRect());
  ypickerline_->setClipToAxisRect(true);
  ypickerline_->position("point1")->setCoords(xaxis->range().lower, yval);
  ypickerline_->position("point2")->setCoords(xaxis->range().upper, yval);
  layout_->streachLabelSetText(QString(" x=%1 y=%2").arg(xval).arg(yval));
}

void PickerTool2D::movepickermouspresscurve(Curve2D *curve, const double xval,
                                            const double yval, Axis2D *xaxis,
                                            Axis2D *yaxis) {
  curve_ = curve;
  movepickermouspress(xval, yval, xaxis, yaxis);
}

void PickerTool2D::movepickermouspressls(LineSpecial2D *ls, const double xval,
                                         const double yval, Axis2D *xaxis,
                                         Axis2D *yaxis) {
  ls_ = ls;
  movepickermouspress(xval, yval, xaxis, yaxis);
}

void PickerTool2D::movepickermouspressbar(Bar2D *bar, const double xval,
                                          const double yval, Axis2D *xaxis,
                                          Axis2D *yaxis) {
  bar_ = bar;
  movepickermouspress(xval, yval, xaxis, yaxis);
}

void PickerTool2D::movepickermousedrag(const QPointF &position,
                                       const double xval, const double yval) {
  Q_UNUSED(position);
  Axis2D *xaxis = nullptr;
  Axis2D *yaxis = nullptr;
  if (curve_) {
    xaxis = curve_->getxaxis();
    yaxis = curve_->getyaxis();
  } else if (ls_) {
    xaxis = ls_->getxaxis();
    yaxis = ls_->getyaxis();
  } else if (bar_) {
    xaxis = bar_->getxaxis();
    yaxis = bar_->getyaxis();
  }
  if (!xaxis || !yaxis) return;

  double x = xaxis->coordToPixel(xval);
  double y = yaxis->coordToPixel(yval);
  xpickerellipse_->position("topLeft")->setPixelPosition(
      QPointF(x - ellipseradius_, y - ellipseradius_));
  xpickerellipse_->position("bottomRight")
      ->setPixelPosition(QPointF(x + ellipseradius_, y + ellipseradius_));
  QToolTip::showText(
      layout_->mapToGlobal(QPoint(static_cast<int>(position.x()),
                                  static_cast<int>(position.y()))),
      QString::number(xval) + ", " + QString::number(yval));
}

void PickerTool2D::movepickermouserelease(const QPointF position) {
  Axis2D *xaxis = nullptr;
  Axis2D *yaxis = nullptr;
  if (curve_) {
    xaxis = curve_->getxaxis();
    yaxis = curve_->getyaxis();
  } else if (ls_) {
    xaxis = ls_->getxaxis();
    yaxis = ls_->getyaxis();
  } else if (bar_) {
    xaxis = bar_->getxaxis();
    yaxis = bar_->getyaxis();
  }
  if (!xaxis || !yaxis) return;

  double newxval = xaxis->pixelToCoord(position.x());
  double newyval = yaxis->pixelToCoord(position.y());
  bool status = false;
  if (curve_)
    status = curve_->getdatablock_cplot()->movedatafromtable(
        movepicker_.xval, movepicker_.yval, newxval, newyval);
  else if (ls_)
    status = ls_->getdatablock_lsplot()->movedatafromtable(
        movepicker_.xval, movepicker_.yval, newxval, newyval);
  else if (bar_)
    status = bar_->getdatablock_barplot()->movedatafromtable(
        movepicker_.xval, movepicker_.yval, newxval, newyval);
  if (status) {
    double x = xaxis->coordToPixel(newxval);
    double y = yaxis->coordToPixel(newyval);
    xpickerellipse_->position("topLeft")->setPixelPosition(
        QPointF(x - ellipseradius_, y - ellipseradius_));
    xpickerellipse_->position("bottomRight")
        ->setPixelPosition(QPointF(x + ellipseradius_, y + ellipseradius_));
    QToolTip::showText(
        layout_->mapToGlobal(QPoint(static_cast<int>(position.x()),
                                    static_cast<int>(position.y()))),
        QString::number(newxval) + ", " + QString::number(newyval));
  } else {
    double x = xaxis->coordToPixel(movepicker_.xval);
    double y = yaxis->coordToPixel(movepicker_.yval);
    xpickerellipse_->position("topLeft")->setPixelPosition(
        QPointF(x - ellipseradius_, y - ellipseradius_));
    xpickerellipse_->position("bottomRight")
        ->setPixelPosition(QPointF(x + ellipseradius_, y + ellipseradius_));
    QToolTip::showText(
        layout_->mapToGlobal(QPoint(static_cast<int>(x), static_cast<int>(y))),
        QString::number(movepicker_.xval) + ", " +
            QString::number(movepicker_.yval));
  }
  layout_->getPlotCanwas()->replot(
      QCustomPlot::RefreshPriority::rpQueuedReplot);
  curve_ = nullptr;
  ls_ = nullptr;
  bar_ = nullptr;
}

void PickerTool2D::rangepickermousepress(Curve2D *curve, const double xval,
                                       const double yval) {
  if (curve == rangepicker_.curve &&
      ((xval == rangepicker_.lowerx && yval == rangepicker_.lowery) ||
       (xval == rangepicker_.upperx && yval == rangepicker_.uppery))) {
    rangepicker_.active = true;
    (xval == rangepicker_.lowerx && yval == rangepicker_.lowery)
        ? rangepicker_.line = xpickerline_
    : (xval == rangepicker_.upperx && yval == rangepicker_.uppery)
        ? rangepicker_.line = ypickerline_
        : rangepicker_.line = nullptr;
  } else
    rangepicker_.active = false;
}

void PickerTool2D::rangepickermousedrag(const QPointF &position, const double xval,
                                     const double yval) {
  Q_UNUSED(position);
  if (!rangepicker_.active || !rangepicker_.line) return;
  moveLineEllipseItenTo(xval, yval, false);
}

void PickerTool2D::rangepickermouserelease(const QPointF position) {
  if (!rangepicker_.active || !rangepicker_.line) return;
  QVariant variant;
  rangepicker_.curve->selectTest(position, false, &variant);
  QCPCurveDataContainer::const_iterator it;
  QCPDataSelection dataPoints = variant.value<QCPDataSelection>();
  bool sucess = false;
  if (dataPoints.dataPointCount() > 0) {
    it = rangepicker_.curve->data()->at(dataPoints.dataRange().begin());
    QPointF point =
        rangepicker_.curve->coordsToPixels(it->mainKey(), it->mainValue());
    if (point.x() > position.x() - 10 && point.x() < position.x() + 10 &&
        point.y() > position.y() - 10 && point.y() < position.y() + 10) {
      if (rangepicker_.line == xpickerline_) {
        if (it->mainKey() != rangepicker_.upperx) {
          rangepicker_.lowerx = it->mainKey();
          rangepicker_.lowery = it->mainValue();
          moveLineEllipseItenTo(it->mainKey(), it->mainValue(), true);
          sucess = true;
        } else
          sucess = false;
      } else if (rangepicker_.line == ypickerline_) {
        if (it->mainKey() != rangepicker_.lowerx) {
          rangepicker_.upperx = it->mainKey();
          rangepicker_.uppery = it->mainValue();
          moveLineEllipseItenTo(it->mainKey(), it->mainValue(), true);
          sucess = true;
        } else
          sucess = false;
      }
    }
  }

  if (!sucess) {
    if (rangepicker_.line == xpickerline_) {
      moveLineEllipseItenTo(rangepicker_.lowerx, rangepicker_.lowery, true);
    } else if (rangepicker_.line == ypickerline_) {
      moveLineEllipseItenTo(rangepicker_.upperx, rangepicker_.uppery, true);
    }
  }
  layout_->getPlotCanwas()->replot(
      QCustomPlot::RefreshPriority::rpQueuedReplot);
  rangepicker_.active = false;
  curve_ = nullptr;
}

void PickerTool2D::datapoint(Curve2D *curve, const double xval,
                             const double yval) {
  emit layout_->datapoint(curve, xval, yval);
}

void PickerTool2D::movepickermouspress(const double xval, const double yval,
                                       Axis2D *xaxis, Axis2D *yaxis) {
  QPen pen = QPen(Qt::red, 1);
  QColor color = Qt::yellow;
  color.setAlpha(100);
  QBrush brush = QBrush(color);
  xpickerellipse_->setPen(pen);
  xpickerellipse_->setBrush(brush);
  xpickerellipse_->setAntialiased(true);
  xpickerellipse_->setVisible(true);

  // ellipse
  foreach (QCPItemPosition *position, xpickerellipse_->positions()) {
    position->setAxes(xaxis, yaxis);
  }
  xpickerellipse_->setClipAxisRect(xaxis->getaxisrect_axis());
  xpickerellipse_->setClipToAxisRect(true);
  double x = xaxis->coordToPixel(xval);
  double y = yaxis->coordToPixel(yval);
  xpickerellipse_->position("topLeft")->setPixelPosition(
      QPointF(x - ellipseradius_, y - ellipseradius_));
  xpickerellipse_->position("bottomRight")
      ->setPixelPosition(QPointF(x + ellipseradius_, y + ellipseradius_));
  movepicker_.xval = xval;
  movepicker_.yval = yval;
}

void PickerTool2D::setupRangepicker() {
  AxisRect2D *axisrect = layout_->getCurrentAxisRect();
  QPen pen = QPen(Qt::red, 1, Qt::DashLine);
  QColor color = Qt::yellow;
  color.setAlpha(100);
  QBrush brush = QBrush(color);
  xpickerline_->setPen(pen);
  ypickerline_->setPen(pen);
  xpickerellipse_->setPen(pen);
  ypickerellipse_->setPen(pen);
  xpickerellipse_->setBrush(brush);
  ypickerellipse_->setBrush(brush);
  xpickerline_->setAntialiased(false);
  ypickerline_->setAntialiased(false);
  xpickerellipse_->setAntialiased(true);
  ypickerellipse_->setAntialiased(true);
  xpickerline_->setVisible(true);
  ypickerline_->setVisible(true);
  xpickerellipse_->setVisible(true);
  ypickerellipse_->setVisible(true);

  // line
  foreach (QCPItemPosition *position, xpickerline_->positions()) {
    position->setAxes(curve_->getxaxis(), curve_->getyaxis());
  }
  xpickerline_->setClipAxisRect(axisrect);
  xpickerline_->setClipToAxisRect(true);
  double startx = curve_->data()->at(0)->mainKey();
  double starty = curve_->data()->at(0)->mainValue();
  xpickerline_->position("point1")->setCoords(
      startx, curve_->getyaxis()->range().lower);
  xpickerline_->position("point2")->setCoords(
      startx, curve_->getyaxis()->range().upper);
  foreach (QCPItemPosition *position, ypickerline_->positions()) {
    position->setAxes(curve_->getxaxis(), curve_->getyaxis());
  }
  ypickerline_->setClipAxisRect(axisrect);
  ypickerline_->setClipToAxisRect(true);
  double stopx = curve_->data()->at(curve_->data()->size() - 1)->mainKey();
  double stopy = curve_->data()->at(curve_->data()->size() - 1)->mainValue();
  ypickerline_->position("point1")->setCoords(
      stopx, curve_->getyaxis()->range().lower);
  ypickerline_->position("point2")->setCoords(
      stopx, curve_->getyaxis()->range().upper);

  // ellipse
  foreach (QCPItemPosition *position, xpickerellipse_->positions()) {
    position->setAxes(curve_->getxaxis(), curve_->getyaxis());
  }
  xpickerellipse_->setClipAxisRect(axisrect);
  xpickerellipse_->setClipToAxisRect(true);
  double x1 = curve_->getxaxis()->coordToPixel(startx);
  double y1 = curve_->getyaxis()->coordToPixel(starty);
  xpickerellipse_->position("topLeft")->setPixelPosition(
      QPointF(x1 - ellipseradius_, y1 - ellipseradius_));
  xpickerellipse_->position("bottomRight")
      ->setPixelPosition(QPointF(x1 + ellipseradius_, y1 + ellipseradius_));
  foreach (QCPItemPosition *position, ypickerellipse_->positions()) {
    position->setAxes(curve_->getxaxis(), curve_->getyaxis());
  }
  ypickerellipse_->setClipAxisRect(axisrect);
  ypickerellipse_->setClipToAxisRect(true);
  double x2 = curve_->getxaxis()->coordToPixel(stopx);
  double y2 = curve_->getyaxis()->coordToPixel(stopy);
  ypickerellipse_->position("topLeft")->setPixelPosition(
      QPointF(x2 - ellipseradius_, y2 - ellipseradius_));
  ypickerellipse_->position("bottomRight")
      ->setPixelPosition(QPointF(x2 + ellipseradius_, y2 + ellipseradius_));

  rangepicker_.curve = curve_;
  rangepicker_.lowerx = startx;
  rangepicker_.lowery = starty;
  rangepicker_.upperx = stopx;
  rangepicker_.uppery = stopy;
}

void PickerTool2D::setupMovePicker() {}

void PickerTool2D::removePickerLinesAndEllipses() {
  if (xpickerline_) {
    layout_->getPlotCanwas()->removeItem(xpickerline_);
    xpickerline_ = nullptr;
  }
  if (ypickerline_) {
    layout_->getPlotCanwas()->removeItem(ypickerline_);
    ypickerline_ = nullptr;
  }
  if (xpickerellipse_) {
    layout_->getPlotCanwas()->removeItem(xpickerellipse_);
    xpickerellipse_ = nullptr;
  }
  if (ypickerellipse_) {
    layout_->getPlotCanwas()->removeItem(ypickerellipse_);
    ypickerellipse_ = nullptr;
  }
}

void PickerTool2D::resetDataRangePicker() {
  rangepicker_.curve = nullptr;
  rangepicker_.lowerx = 0.0;
  rangepicker_.lowery = 0.0;
  rangepicker_.upperx = 0.0;
  rangepicker_.uppery = 0.0;
  rangepicker_.line = nullptr;
  rangepicker_.active = false;
}

void PickerTool2D::moveLineEllipseItenTo(double xval, double yval,
                                         bool active) {
  QCPItemEllipse *ellipse = nullptr;
  (rangepicker_.line == xpickerline_)   ? ellipse = xpickerellipse_
  : (rangepicker_.line == ypickerline_) ? ellipse = ypickerellipse_
                                        : ellipse = nullptr;
  if (!ellipse) return;
  // active inactive
  if (active) {
    rangepicker_.line->setPen(QPen(Qt::red, 1, Qt::DashLine));
    ellipse->setPen(QPen(Qt::red, 1, Qt::DashLine));
    QColor color = Qt::yellow;
    color.setAlpha(100);
    ellipse->setBrush(QBrush(color));
  } else {
    rangepicker_.line->setPen(QPen(Qt::gray, 1, Qt::DashLine));
    ellipse->setPen(QPen(Qt::gray, 1, Qt::DashLine));
    QColor color = Qt::lightGray;
    color.setAlpha(100);
    ellipse->setBrush(QBrush(color));
  }
  // line
  rangepicker_.line->position("point1")->setCoords(
      xval, rangepicker_.curve->getyaxis()->range().lower);
  rangepicker_.line->position("point2")->setCoords(
      xval, rangepicker_.curve->getyaxis()->range().upper);
  // ellipse
  double x = rangepicker_.curve->getxaxis()->coordToPixel(xval);
  double y = rangepicker_.curve->getyaxis()->coordToPixel(yval);
  ellipse->position("topLeft")->setPixelPosition(
      QPointF(x - ellipseradius_, y - ellipseradius_));
  ellipse->position("bottomRight")
      ->setPixelPosition(QPointF(x + ellipseradius_, y + ellipseradius_));
}
