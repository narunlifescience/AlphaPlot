#ifndef BAR2D_H
#define BAR2D_H

#include "../3rdparty/qcustomplot/qcustomplot.h"
#include "Axis2D.h"

class Column;

class Bar2D : public QCPBars {
  Q_OBJECT
 public:
  Bar2D(Axis2D *xAxis, Axis2D *yAxis);
  ~Bar2D();

  enum BarStyle {
    VerticalNormal,
    HorizontalNornal,
    VerticalStacked,
    HorizontalStacked,
  };

  void setBarData(Column *xData, Column *yData, int from, int to);
  void setBarWidth(double barwidth);
  double getBarWidth();

private:
  double barwidth_;
};

#endif  // BAR2D_H
