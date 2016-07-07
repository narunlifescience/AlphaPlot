#ifndef STATBOX2D_H
#define STATBOX2D_H

#include "../3rdparty/qcustomplot/qcustomplot.h"
#include "Axis2D.h"

class StatBox2D : public QCPStatisticalBox {
  Q_OBJECT
 public:
  StatBox2D(Axis2D *xAxis = nullptr, Axis2D *yAxis = nullptr);
  ~StatBox2D();
};

#endif  // STATBOX2D_H
