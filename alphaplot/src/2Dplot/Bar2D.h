#ifndef BAR2D_H
#define BAR2D_H

#include "../3rdparty/qcustomplot/qcustomplot.h"
#include "Axis2D.h"
#include "Graph2DCommon.h"

class Column;
class Table;
class DataBlockBar;

class Bar2D : public QCPBars {
  Q_OBJECT
 public:
  Bar2D(Table *table, Column *xcol, Column *ycol, int from, int to,
        Axis2D *xAxis, Axis2D *yAxis);
  Bar2D(Table *table, Column *ycol, int from, int to, Axis2D *xAxis,
        Axis2D *yAxis);
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
  bool ishistogram_barplot() const;

  void setxaxis_barplot(Axis2D *axis);
  void setyaxis_barplot(Axis2D *axis);
  void setstrokestyle_barplot(const Qt::PenStyle &style);
  void setstrokecolor_barplot(const QColor &color);
  void setstrokethickness_barplot(const double value);
  void setfillcolor_barplot(const QColor &color);

  void setBarData(Table *table, Column *xcol, Column *ycol, int from, int to);
  void setpicker_barplot(const Graph2DCommon::Picker picker);

 protected:
  void mousePressEvent(QMouseEvent *event, const QVariant &details);

private:
  void datapicker(QMouseEvent *event, const QVariant &details);
  void graphpicker(QMouseEvent *event, const QVariant &details);
  void movepicker(QMouseEvent *event, const QVariant &details);
  void removepicker(QMouseEvent *event, const QVariant &details);

 signals:
  void showtooltip(QPointF position, double xval, double yval);

 private:
  double barwidth_;
  Axis2D *xaxis_;
  Axis2D *yaxis_;
  DataBlockBar *bardata_;
  bool ishistogram_;
  Graph2DCommon::Picker picker_;
};

#endif  // BAR2D_H
