#ifndef LINESCATTER2D_H
#define LINESCATTER2D_H

#include "../3rdparty/qcustomplot/qcustomplot.h"
#include "Axis2D.h"

class Column;

class LineScatter2D : public QCPGraph {
  Q_OBJECT
 public:
  LineScatter2D(Axis2D *xAxis = nullptr, Axis2D *yAxis = nullptr);
  ~LineScatter2D();

  enum Line {
    LinePlot,
    NonePlot,
    VerticalDropLinePlot,
    SplinePlot,
    CentralStepAndScatterPlot,
    HorizontalStepPlot,
    VerticalStepPlot,
    AreaPlot,
  };

  enum Scatter { ScatterVisible, ScatterHidden };

  void setGraphData(Column *xData, Column *yData, int from, int to);
  void setGraphData(QVector<double> *xdata, QVector<double> *ydata);
  // set scatter line params
  void setLineScatter2DPlot(const Line &line, const Scatter &scatter);
  void setScatterShape2D(const QCPScatterStyle::ScatterShape &shape);
  void setScatterPen2D(const QPen &pen);
  void setScatterBrush2D(const QBrush &brush);
  void setScatterSize2D(const double size);
  void setLinePen2D(const QPen &pen);
  void setLineBrush2D(const QBrush &brush);
  void setAutoAntialiasing(bool status) {autoAntialiasing_ = status;}

  // Get scatter line params
  QCPScatterStyle::ScatterShape getScatterShape() const {
    return scatterShape_;
  }
  QPen getScatterPen() const { return scatterPen_; }
  QBrush getScatterBrush() const { return scatterBrush_; }
  double getScatterSize() const { return scatterSize_; }
  Line getLinePlotStyle() const { return lineStyle_; }
  QPen getLinePen() const { return linePen_; }
  QBrush getLineBrush() const { return lineBrush_; }


 private:
  // Scatter
  QCPScatterStyle scatterStyle_;
  QCPScatterStyle::ScatterShape scatterShape_;
  QPen scatterPen_;
  QBrush scatterBrush_;
  double scatterSize_;
  // Line
  Line lineStyle_;
  QPen linePen_;
  QBrush lineBrush_;
  bool autoAntialiasing_;
};

#endif  // LINESCATTER2D_H
