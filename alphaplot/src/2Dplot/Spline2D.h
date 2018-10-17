#ifndef SPLINE2D_H
#define SPLINE2D_H

#include <QObject>
#include "../3rdparty/qcustomplot/qcustomplot.h"
#include "Axis2D.h"
#include "LineSpecial2D.h"

class Column;
class Table;
class DataBlockCurve;

class Spline2D : public QCPCurve {
  Q_OBJECT
 public:
  Spline2D(Table *table, Column *xcol, Column *ycol, int from, int to,
           Axis2D *xAxis, Axis2D *yAxis);
  ~Spline2D();

  void setSplineData(Table *table, Column *xData, Column *yData, int from,
                     int to);
  void testData(Table *table, Column *xData, Column *yData, int from,
                     int to);
  // Getters
  Qt::PenStyle getlinestrokestyle_splot() const;
  QColor getlinestrokecolor_splot() const;
  double getlinestrokethickness_splot() const;
  QColor getlinefillcolor_splot() const;
  bool getlineantialiased_splot() const;
  bool getlinefillstatus_splot() const;
  Graph2DCommon::ScatterStyle getscattershape_splot() const;
  QColor getscatterfillcolor_splot() const;
  double getscattersize_splot() const;
  Qt::PenStyle getscatterstrokestyle_splot() const;
  QColor getscatterstrokecolor_splot() const;
  double getscatterstrokethickness_splot() const;
  bool getscatterantialiased_splot() const;
  QString getlegendtext_splot() const;
  Axis2D *getxaxis_splot() const;
  Axis2D *getyaxis_splot() const;
  DataBlockCurve *getdatablock_splot() const;

  Table *gettable_splot() { return table_; }
  Column *getxcolumn() { return xcol_; }
  Column *getycolumn() { return ycol_; }
  int getfrom_splot() const { return from_; }
  int getto_splot() const { return to_; }
  // Setters
  void setxaxis_splot(Axis2D *axis);
  void setyaxis_splot(Axis2D *axis);
  void setlinestrokestyle_splot(const Qt::PenStyle &style);
  void setlinestrokecolor_splot(const QColor &color);
  void setlinestrokethickness_splot(const double value);
  void setlinefillcolor_splot(const QColor &color);
  void setlineantialiased_splot(const bool value);
  void setlinefillstatus_splot(const bool value);
  void setscattershape_splot(const Graph2DCommon::ScatterStyle &shape);
  void setscatterfillcolor_splot(const QColor &color);
  void setscattersize_splot(const double value);
  void setscatterstrokestyle_splot(const Qt::PenStyle &style);
  void setscatterstrokecolor_splot(const QColor &color);
  void setscatterstrokethickness_splot(const double value);
  void setscatterantialiased_splot(const bool value);
  void setlegendtext_splot(const QString &text);
  void setpicker_splot(const Graph2DCommon::Picker picker);

 protected:
  void draw(QCPPainter *painter);
  void mousePressEvent(QMouseEvent *event, const QVariant &details);

 private:
  void datapicker(QMouseEvent *event, const QVariant &details);
  void graphpicker(QMouseEvent *event, const QVariant &details);
  void movepicker(QMouseEvent *event, const QVariant &details);
  void removepicker(QMouseEvent *event, const QVariant &details);

 signals:
  void showtooltip(QPointF, double, double);

 private:
  Axis2D *xAxis_;
  Axis2D *yAxis_;
  QCPScatterStyle *scatterstyle_;
  DataBlockCurve *curvedata_;
  Table *table_;
  Column *xcol_;
  Column *ycol_;
  int from_;
  int to_;
  Graph2DCommon::Picker picker_;
};

#endif  // SPLINE2D_H
