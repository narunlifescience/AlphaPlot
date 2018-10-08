#ifndef SPLINE2D_H
#define SPLINE2D_H

#include <QObject>
#include "../3rdparty/qcustomplot/qcustomplot.h"
#include "Axis2D.h"

class Column;

class Spline2D : public QCPCurve {
  Q_OBJECT
 public:
  Spline2D(Axis2D *xAxis = nullptr, Axis2D *yAxis = nullptr);
  ~Spline2D();

   void setGraphData(Column *xData, Column *yData, int from, int to);
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

 private:
  Axis2D *xAxis_;
  Axis2D *yAxis_;
};

#endif  // SPLINE2D_H
