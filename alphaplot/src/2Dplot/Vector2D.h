#ifndef VECTOR2D_H
#define VECTOR2D_H
#include "../3rdparty/qcustomplot/qcustomplot.h"
#include "Axis2D.h"

class Column;

class Vector2D : public QCPGraph {
  Q_OBJECT
 public:
  Vector2D(Axis2D *xAxis = nullptr, Axis2D *yAxis = nullptr);
  ~Vector2D();

  enum class VectorPlot : int { XYXY = 0, XYAM = 1 };
  enum class Position : int { Tail = 0, Middle = 1, Head = 2 };
  enum class LineEnd : int {
    None = 0,
    FlatArrow = 1,
    SpikeArrow = 2,
    LineArrow = 3,
    Disc = 4,
    Square = 5,
    Diamond = 6,
    Bar = 7,
    HalfBar = 8,
    SkewedBar = 9,
  };
  enum class LineEndLocation : int {
    Start = 0,
    Stop = 1,
  };
  void setGraphData(const VectorPlot &vectorplot, Column *x1Data,
                    Column *y1Data, Column *x2Data, Column *y2Data, int from,
                    int to);
  void drawLine(double x1, double y1, double x2, double y2);

  // Getters
  Axis2D *getxaxis_vecplot();
  Axis2D *getyaxis_vecplot();
  QColor getlinestrokecolor_vecplot() const;
  Qt::PenStyle getlinestrokestyle_vecplot() const;
  double getlinestrokethickness_vecplot() const;
  bool getlineantialiased_vecplot() const;
  LineEnd getendstyle_vecplot(const LineEndLocation &location) const;
  double getendwidth_vecplot(const LineEndLocation &location) const;
  double getendheight_vecplot(const LineEndLocation &location) const;
  bool getendinverted_vecplot(const LineEndLocation &location) const;
  QString getlegendtext_vecplot() const;

  // Setters
  void setxaxis_vecplot(Axis2D* axis);
  void setyaxis_vecplot(Axis2D *axis);
  void setlinestrokecolor_vecplot(const QColor &color);
  void setlinestrokestyle_vecplot(const Qt::PenStyle &style);
  void setlinestrokethickness_vecplot(const double value);
  void setlineantialiased_vecplot(bool status);
  void setendstyle_vecplot(const LineEnd &end, const LineEndLocation &location);
  void setendwidth_vecplot(const double value, const LineEndLocation &location);
  void setendheight_vecplot(const double value,
                            const LineEndLocation &location);
  void setendinverted_vecplot(const bool value,
                              const LineEndLocation &location);
  void setlegendtext_vecplot(const QString &name);

 private:
  Axis2D *xaxis_;
  Axis2D *yaxis_;
  QList<QCPItemLine *> linelist_;
  Position d_position_;
  QCPLineEnding *start_;
  QCPLineEnding *stop_;
};

#endif  // VECTOR2D_H
