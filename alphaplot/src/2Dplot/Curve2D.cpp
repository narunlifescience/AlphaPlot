#include "Curve2D.h"

Curve2D::Curve2D(Axis2D *xAxis, Axis2D *yAxis) : QCPCurve(xAxis, yAxis) {
  setLineStyle(LineStyle::lsLine);
}

void Curve2D::setGraphData(QVector<double> *xdata, QVector<double> *ydata) {
  Q_ASSERT(xdata->size() == ydata->size());

  QSharedPointer<QCPCurveDataContainer> functionData(new QCPCurveDataContainer);
  for (int i = 0; i < xdata->size(); i++) {
    QCPCurveData fd;
    fd.key = xdata->at(i);
    fd.value = ydata->at(i);
    functionData->add(fd);
  }
  setData(functionData);
  // free those containers
  delete xdata;
  delete ydata;
}
