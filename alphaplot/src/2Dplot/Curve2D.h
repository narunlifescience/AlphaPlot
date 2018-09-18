#ifndef CURVE2D_H
#define CURVE2D_H

#include "../3rdparty/qcustomplot/qcustomplot.h"
#include "LineScatterCommon.h"
#include "Axis2D.h"

class Curve2D : public QCPCurve
{
  Q_OBJECT
public:
  explicit Curve2D(Axis2D *xAxis= nullptr, Axis2D *yAxis=nullptr);
  ~Curve2D();

  void setGraphData(QVector<double> *xdata, QVector<double> *ydata);

  // Getters
  int getlinetype_cplot() const;
  Qt::PenStyle getlinestrokestyle_cplot() const;
  QColor getlinestrokecolor_cplot() const;
  double getlinestrokethickness_cplot() const;
  QColor getlinefillcolor_cplot() const;
  bool getlineantialiased_cplot() const;
  bool getlinefillstatus_cplot() const;
  LSCommon::ScatterStyle getscattershape_cplot() const;
  QColor getscatterfillcolor_cplot() const;
  double getscattersize_cplot() const;
  Qt::PenStyle getscatterstrokestyle_cplot() const;
  QColor getscatterstrokecolor_cplot() const;
  double getscatterstrokethickness_cplot() const;
  bool getscatterantialiased_cplot() const;
  QString getlegendtext_cplot() const;
  Axis2D *getxaxis_cplot() const;
  Axis2D *getyaxis_cplot() const;
  // Setters
  void setxaxis_cplot(Axis2D *axis);
  void setyaxis_cplot(Axis2D *axis);
  void setlinetype_cplot(const int type);
  void setlinestrokestyle_cplot(const Qt::PenStyle &style);
  void setlinestrokecolor_cplot(const QColor &color);
  void setlinestrokethickness_cplot(const double value);
  void setlinefillcolor_cplot(const QColor &color);
  void setlineantialiased_cplot(const bool value);
  void setscattershape_cplot(const LSCommon::ScatterStyle &shape);
  void setscatterfillcolor_cplot(const QColor &color);
  void setscattersize_cplot(const double value);
  void setscatterstrokestyle_cplot(const Qt::PenStyle &style);
  void setscatterstrokecolor_cplot(const QColor &color);
  void setscatterstrokethickness_cplot(const double value);
  void setscatterantialiased_cplot(const bool value);
  void setlinefillstatus_cplot(const bool value);
  void setlegendtext_cplot(const QString &text);

private:
  Axis2D *xAxis_;
  Axis2D *yAxis_;
  QCPScatterStyle *scatterstyle_;
};

#endif // CURVE2D_H
