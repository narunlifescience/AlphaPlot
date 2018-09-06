#ifndef CURVE2D_H
#define CURVE2D_H

#include "../3rdparty/qcustomplot/qcustomplot.h"
#include "Axis2D.h"

class Curve2D : public QCPCurve
{
  Q_OBJECT
public:
  explicit Curve2D(Axis2D *xAxis = nullptr, Axis2D *yAxis = nullptr);

  void setGraphData(QVector<double> *xdata, QVector<double> *ydata);

signals:

public slots:
};

#endif // CURVE2D_H
