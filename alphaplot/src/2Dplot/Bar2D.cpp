#include "Bar2D.h"
#include "../future/core/column/Column.h"

Bar2D::Bar2D(Axis2D *xAxis, Axis2D *yAxis) : QCPBars(xAxis, yAxis) {}

Bar2D::~Bar2D() {}

void Bar2D::setBarData(Column *xData, Column *yData, int from, int to)
{
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
}
