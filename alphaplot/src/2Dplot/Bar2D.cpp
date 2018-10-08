#include "Bar2D.h"
#include "future/core/column/Column.h"
#include "DataManager2D.h"
#include "Table.h"

Bar2D::Bar2D(Table *table, Column *xcol, Column *ycol, int from, int to,
             Axis2D *xAxis, Axis2D *yAxis)
    : QCPBars(xAxis, yAxis),
      barwidth_(1),
      xaxis_(xAxis),
      yaxis_(yAxis),
      bardata_(new DataBlockBar(table, xcol, ycol, from, to)) {
  layer()->setMode(QCPLayer::LayerMode::lmBuffered);
  setData(bardata_->data());
}

Bar2D::~Bar2D() { delete bardata_; }

Axis2D *Bar2D::getxaxis_barplot() const { return xaxis_; }

Axis2D *Bar2D::getyaxis_barplot() const { return yaxis_; }

Qt::PenStyle Bar2D::getstrokestyle_barplot() const { return pen().style(); }

QColor Bar2D::getstrokecolor_barplot() const { return pen().color(); }

double Bar2D::getstrokethickness_barplot() const { return pen().widthF(); }

QColor Bar2D::getfillcolor_barplot() const { return brush().color(); }

DataBlockBar *Bar2D::getdatablock_barplot() const { return bardata_; }

void Bar2D::setxaxis_barplot(Axis2D *axis) {
  Q_ASSERT(axis->getorientation_axis() == Axis2D::AxisOreantation::Bottom ||
           axis->getorientation_axis() == Axis2D::AxisOreantation::Top);
  if (axis == getxaxis_barplot()) return;

  xaxis_ = axis;
  setKeyAxis(axis);
}

void Bar2D::setyaxis_barplot(Axis2D *axis) {
  Q_ASSERT(axis->getorientation_axis() == Axis2D::AxisOreantation::Left ||
           axis->getorientation_axis() == Axis2D::AxisOreantation::Right);
  if (axis == getyaxis_barplot()) return;

  yaxis_ = axis;
  setValueAxis(axis);
}

void Bar2D::setstrokestyle_barplot(const Qt::PenStyle &style) {
  QPen p = pen();
  p.setStyle(style);
  setPen(p);
}

void Bar2D::setstrokecolor_barplot(const QColor &color) {
  QPen p = pen();
  p.setColor(color);
  setPen(p);
}

void Bar2D::setstrokethickness_barplot(const double value) {
  QPen p = pen();
  p.setWidthF(value);
  setPen(p);
}

void Bar2D::setfillcolor_barplot(const QColor &color) {
  QBrush b = brush();
  b.setColor(color);
  setBrush(b);
}

void Bar2D::setBarData(Table *table, Column *xcol, Column *ycol, int from,
                       int to) {
  bardata_->regenerateDataBlock(table, xcol, ycol, from, to);
  setData(bardata_->data());
}

void Bar2D::setBarWidth(double barwidth) {
  setWidth(barwidth / static_cast<double>(data().data()->size()));
}

double Bar2D::getBarWidth() { return barwidth_; }
