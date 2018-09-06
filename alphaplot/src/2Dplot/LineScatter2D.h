#ifndef LINESCATTER2D_H
#define LINESCATTER2D_H

#include "../3rdparty/qcustomplot/qcustomplot.h"
#include "Axis2D.h"

class Column;
class PlotPoint;

class LineScatter2D : public QCPGraph {
  Q_OBJECT
 public:
  LineScatter2D(Axis2D *xAxis = nullptr, Axis2D *yAxis = nullptr);
  ~LineScatter2D();

  enum class LineStyleType : int {
    None = 0,
    Line = 1,
    StepLeft = 2,
    StepRight = 3,
    StepCenter = 4,
    Impulse = 5,
  };

  enum class ScatterStyle : int {
    None = 0,
    Dot = 1,
    Cross = 2,
    Plus = 3,
    Circle = 4,
    Disc = 5,
    Square = 6,
    Diamond = 7,
    Star = 8,
    Triangle = 9,
    TriangleInverted = 10,
    CrossSquare = 11,
    PlusSquare = 12,
    CrossCircle = 13,
    PlusCircle = 14,
    Peace = 15,
  };

  void setGraphData(Column *xData, Column *yData, int from, int to);
  void setGraphData(QVector<double> *xdata, QVector<double> *ydata);
  // Getters
  LineStyleType getlinetype_lsplot() const;
  Qt::PenStyle getlinestrokestyle_lsplot() const;
  QColor getlinestrokecolor_lsplot() const;
  double getlinestrokethickness_lsplot() const;
  bool getlinefillstatus_lsplot() const;
  QColor getlinefillcolor_lsplot() const;
  bool getlineantialiased_lsplot() const;
  ScatterStyle getscattershape_lsplot() const;
  QColor getscatterfillcolor_lsplot() const;
  double getscattersize_lsplot() const;
  Qt::PenStyle getscatterstrokestyle_lsplot() const;
  QColor getscatterstrokecolor_lsplot() const;
  double getscatterstrokethickness_lsplot() const;
  bool getscatterantialiased_lsplot() const;
  QString getlegendtext_lsplot() const;
  Axis2D *getxaxis_lsplot() const;
  Axis2D *getyaxis_lsplot() const;
  // Setters
  void setlinetype_lsplot(const LineStyleType &line);
  void setlinestrokestyle_lsplot(const Qt::PenStyle &style);
  void setlinestrokecolor_lsplot(const QColor &color);
  void setlinestrokethickness_lsplot(const double value);
  void setlinefillstatus_lsplot(bool status);
  void setlinefillcolor_lsplot(const QColor &color);
  void setlineantialiased_lsplot(const bool value);
  void setscattershape_lsplot(const ScatterStyle &shape);
  void setscatterfillcolor_lsplot(const QColor &color);
  void setscattersize_lsplot(const double value);
  void setscatterstrokestyle_lsplot(const Qt::PenStyle &style);
  void setscatterstrokecolor_lsplot(const QColor &color);
  void setscatterstrokethickness_lsplot(const double value);
  void setscatterantialiased_lsplot(const bool value);
  void setlegendtext_lsplot(const QString &legendtext);
  void setxaxis_lsplot(Axis2D *axis);
  void setyaxis_lsplot(Axis2D *axis);

protected:
  void mousePressEvent(QMouseEvent *event, const QVariant &details);
  void mouseMoveEvent(QMouseEvent *event,  const QPointF &startPos);
  void keyPressEvent(QKeyEvent *event);
  void keyreleaseEvent(QKeyEvent *event);

private:
  Axis2D *xAxis_;
  Axis2D *yAxis_;
  QCPScatterStyle *scatterstyle_;
  PlotPoint *mPointUnderCursor;
};

#endif  // LINESCATTER2D_H
