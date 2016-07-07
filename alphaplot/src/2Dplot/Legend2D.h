#ifndef LEGEND2D_H
#define LEGEND2D_H

#include "../3rdparty/qcustomplot/qcustomplot.h"

class LineScatter2D;

class Legend2D : public QCPLegend {
  Q_OBJECT
 public:
  Legend2D();
  ~Legend2D();

 signals:
  void legendClicked();

 protected:
  void mousePressEvent(QMouseEvent *);
};

class LegendItem2D : public QCPPlottableLegendItem {
  Q_OBJECT
 public:
  LegendItem2D(Legend2D *legend = nullptr,
               LineScatter2D *lineScatter = nullptr);
  ~LegendItem2D();

 signals:
  void legendItemClicked();

 protected:
  void mousePressEvent(QMouseEvent *);
};

#endif  // LEGEND2D_H
