#ifndef BAR2D_H
#define BAR2D_H

#include "../3rdparty/qcustomplot/qcustomplot.h"
#include "Axis2D.h"
#include "Graph2DCommon.h"

class Column;
class Table;
class DataBlockBar;
class ErrorBar2D;

class Bar2D : public QCPBars {
  Q_OBJECT
 public:
  Bar2D(Table *table, Column *xcol, Column *ycol, int from, int to,
        Axis2D *xAxis, Axis2D *yAxis, int stackposition);
  Bar2D(Table *table, Column *ycol, int from, int to, Axis2D *xAxis,
        Axis2D *yAxis);
  void init();
  ~Bar2D();

  void setXerrorBar(Table *table, Column *errorcol, int from, int to);
  void setYerrorBar(Table *table, Column *errorcol, int from, int to);
  void removeXerrorBar();
  void removeYerrorBar();

  enum BarStyle {
    VerticalNormal,
    HorizontalNornal,
    VerticalStacked,
    HorizontalStacked,
  };
  Axis2D *getxaxis() const;
  Axis2D *getyaxis() const;
  Qt::PenStyle getstrokestyle_barplot() const;
  QColor getstrokecolor_barplot() const;
  double getstrokethickness_barplot() const;
  QColor getfillcolor_barplot() const;
  DataBlockBar *getdatablock_barplot() const;
  bool ishistogram_barplot() const;
  ErrorBar2D *getxerrorbar_barplot() { return xerrorbar_; }
  ErrorBar2D *getyerrorbar_barplot() { return yerrorbar_; }
  int getstackposition_barplot() const { return stackposition_; }
  Table *gettable_histogram() { return table_; }
  Column *getcolumn_histogram() { return column_; }
  int getfrom_histogram() const { return from_; }
  int getto_histogram() const { return to_; }

  void setxaxis_barplot(Axis2D *axis, bool override = false);
  void setyaxis_barplot(Axis2D *axis, bool override = false);
  void setstrokestyle_barplot(const Qt::PenStyle &style);
  void setstrokecolor_barplot(const QColor &color);
  void setstrokethickness_barplot(const double value);
  void setfillcolor_barplot(const QColor &color);

  void setBarData(Table *table, Column *xcol, Column *ycol, int from, int to);
  void setBarData(Table *table, Column *col, int from, int to);
  void setpicker_barplot(const Graph2DCommon::Picker picker);

  void save(XmlStreamWriter *xmlwriter, int xaxis, int yaxis);
  bool load(XmlStreamReader *xmlreader);

 protected:
  void mousePressEvent(QMouseEvent *event, const QVariant &details);

 private:
  void datapicker(QMouseEvent *, const QVariant &details);
  void movepicker(QMouseEvent *event, const QVariant &details);
  void removepicker(QMouseEvent *, const QVariant &details);

 signals:
  void showtooltip(QPointF position, double xval, double yval, Axis2D *xaxis,
                   Axis2D *yaxis);

 private:
  double barwidth_;
  Axis2D *xaxis_;
  Axis2D *yaxis_;
  DataBlockBar *bardata_;
  bool ishistogram_;
  Table *table_;
  Column *column_;
  int from_;
  int to_;
  ErrorBar2D *xerrorbar_;
  ErrorBar2D *yerrorbar_;
  QString layername_;
  bool xerroravailable_;
  bool yerroravailable_;
  Graph2DCommon::Picker picker_;
  bool auto_binning_;
  double bin_size_;
  double begin_;
  double end_;
  int stackposition_;
};

#endif  // BAR2D_H
