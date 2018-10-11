#ifndef SPLINE2D_H
#define SPLINE2D_H

#include <QObject>
#include "../3rdparty/qcustomplot/qcustomplot.h"
#include "Axis2D.h"
#include "LineSpecial2D.h"

class Column;
class Table;

class Spline2D : public QCPCurve {
  Q_OBJECT
 public:
  Spline2D(Axis2D *xAxis = nullptr, Axis2D *yAxis = nullptr);
  ~Spline2D();

  void setGraphData(Table *table, Column *xData, Column *yData, int from,
                    int to);
  void setSplineData(Column *xData, Column *yData, int from, int to);
  // Getters
  Qt::PenStyle getlinestrokestyle_splot() const;
  QColor getlinestrokecolor_splot() const;
  double getlinestrokethickness_splot() const;
  QColor getlinefillcolor_splot() const;
  bool getlineantialiased_splot() const;
  bool getlinefillstatus_splot() const;
  QString getlegendtext_splot() const;
  Axis2D *getxaxis_splot() const;
  Axis2D *getyaxis_splot() const;
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
  void setlegendtext_splot(const QString &text);
  void setpicker_splot(const Graph2DCommon::Picker picker);

 protected:
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
  Table *table_;
  Column *xcol_;
  Column *ycol_;
  int from_;
  int to_;
  Graph2DCommon::Picker picker_;
};

#endif  // SPLINE2D_H
