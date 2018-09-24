#include "Bar2D.h"
#include "../future/core/column/Column.h"

Bar2D::Bar2D(Axis2D *xAxis, Axis2D *yAxis)
    : QCPBars(xAxis, yAxis), barwidth_(1), xaxis_(xAxis), yaxis_(yAxis) {}

Bar2D::~Bar2D() {}

Axis2D *Bar2D::getxaxis_barplot() const { return xaxis_; }

Axis2D *Bar2D::getyaxis_barplot() const { return yaxis_; }

void Bar2D::setBarData(Column *xData, Column *yData, int from, int to) {
  QSharedPointer<QCPBarsDataContainer> barData(new QCPBarsDataContainer());

  double xdata = 0, ydata = 0;
  for (int i = from; i < to + 1; i++) {
    xdata = xData->valueAt(i);
    ydata = yData->valueAt(i);
    QCPBarsData gd;
    gd.key = xdata;
    gd.value = ydata;
    barData->add(gd);
  }
  setData(barData);
  barwidth_ = data().data()->at(data().data()->size() - 1)->key -
              data().data()->at(0)->key;
  setBarWidth(barwidth_);
}

void Bar2D::setBarWidth(double barwidth) {
  setWidth(barwidth / static_cast<double>(data().data()->size()));
}

double Bar2D::getBarWidth() { return barwidth_; }
