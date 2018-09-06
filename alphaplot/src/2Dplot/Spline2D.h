#ifndef SPLINE2D_H
#define SPLINE2D_H

#include <QObject>
#include "../3rdparty/qcustomplot/qcustomplot.h"
#include "Axis2D.h"

class Column;

class Spline2D : public QCPGraph {
  Q_OBJECT
 public:
  Spline2D(Axis2D *xAxis = nullptr, Axis2D *yAxis = nullptr);
  ~Spline2D();

   void setGraphData(Column *xData, Column *yData, int from, int to);

  // Getters
  Qt::PenStyle getlinestrokestyle_splot() const;
  QColor getlinestrokecolor_splot() const;
  double getlinestrokethickness_splot() const;
  QColor getlinefillcolor_splot() const;
  bool getlineantialiased_splot() const;
  // Setters
  void setlinestrokestyle_splot(const Qt::PenStyle &style);
  void setlinestrokecolor_splot(const QColor &color);
  void setlinestrokethickness_splot(const double value);
  void setlinefillcolor_splot(const QColor &color);
  void setlineantialiased_splot(const bool value);

 private:
  Axis2D *xAxis_;
  Axis2D *yAxis_;
};

#endif  // SPLINE2D_H
