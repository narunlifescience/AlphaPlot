#ifndef BAR2D_H
#define BAR2D_H

#include "../3rdparty/qcustomplot/qcustomplot.h"
#include "Axis2D.h"

class Column;
class Table;
class DataBlockBar;

class Bar2D : public QCPBars {
  Q_OBJECT
 public:
  Bar2D(Table *table, Column *xcol, Column *ycol, int from,
        int to, Axis2D *xAxis, Axis2D *yAxis);
  ~Bar2D();

  enum BarStyle {
    VerticalNormal,
    HorizontalNornal,
    VerticalStacked,
    HorizontalStacked,
  };
  Axis2D *getxaxis_barplot() const;
  Axis2D *getyaxis_barplot() const;
  Qt::PenStyle getstrokestyle_barplot() const;
  QColor getstrokecolor_barplot() const;
  double getstrokethickness_barplot() const;
  QColor getfillcolor_barplot() const;
  DataBlockBar *getdatablock_barplot() const;

  void setBarData(Table *table, Column *xcol, Column *ycol, int from, int to);
  void setBarWidth(double barwidth);
  double getBarWidth();

private:
  double barwidth_;
  Axis2D *xaxis_;
  Axis2D *yaxis_;
  DataBlockBar *bardata_;
};

#endif  // BAR2D_H
