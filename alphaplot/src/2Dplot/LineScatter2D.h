#ifndef LINESCATTER2D_H
#define LINESCATTER2D_H

#include "../3rdparty/qcustomplot/qcustomplot.h"
#include "Axis2D.h"
#include "LineScatterCommon.h"

class Column;
class Table;
class DataBlockGraph;
class ErrorBar2D;

class LineScatter2D : public QCPGraph {
  Q_OBJECT
 public:
  LineScatter2D(Table *table, Column *xcol, Column *ycol, int from, int to,
                Axis2D *xAxis, Axis2D *yAxis);
  LineScatter2D(QVector<double> *xdata, QVector<double> *ydata, Axis2D *xAxis,
                Axis2D *yAxis);
  ~LineScatter2D();

  void setXerrorBar(Table *table, Column *errorcol, int from, int to);
  void setYerrorBar(Table *table, Column *errorcol, int from, int to);
  void setGraphData(Table *table, Column *xcol, Column *ycol, int from, int to);
  void setGraphData(QVector<double> *xdata, QVector<double> *ydata);
  void removeXerrorBar();
  void removeYerrorBar();
  // Getters
  LSCommon::LineStyleType getlinetype_lsplot() const;
  Qt::PenStyle getlinestrokestyle_lsplot() const;
  QColor getlinestrokecolor_lsplot() const;
  double getlinestrokethickness_lsplot() const;
  bool getlinefillstatus_lsplot() const;
  QColor getlinefillcolor_lsplot() const;
  bool getlineantialiased_lsplot() const;
  LSCommon::ScatterStyle getscattershape_lsplot() const;
  QColor getscatterfillcolor_lsplot() const;
  double getscattersize_lsplot() const;
  Qt::PenStyle getscatterstrokestyle_lsplot() const;
  QColor getscatterstrokecolor_lsplot() const;
  double getscatterstrokethickness_lsplot() const;
  bool getscatterantialiased_lsplot() const;
  QString getlegendtext_lsplot() const;
  Axis2D *getxaxis_lsplot() const;
  Axis2D *getyaxis_lsplot() const;
  LSCommon::PlotType getplottype_lsplot() const { return type_; }
  DataBlockGraph *getdatablock_lsplot() const { return graphdata_; }
  // Setters
  void setlinetype_lsplot(const LSCommon::LineStyleType &line);
  void setlinestrokestyle_lsplot(const Qt::PenStyle &style);
  void setlinestrokecolor_lsplot(const QColor &color);
  void setlinestrokethickness_lsplot(const double value);
  void setlinefillstatus_lsplot(bool status);
  void setlinefillcolor_lsplot(const QColor &color);
  void setlineantialiased_lsplot(const bool value);
  void setscattershape_lsplot(const LSCommon::ScatterStyle &shape);
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
  //void mousePressEvent(QMouseEvent *event, const QVariant &details);
  //void mouseMoveEvent(QMouseEvent *event, const QPointF &startPos);
  void keyPressEvent(QKeyEvent *event);
  void keyreleaseEvent(QKeyEvent *event);

 private:
  Axis2D *xAxis_;
  Axis2D *yAxis_;
  QCPScatterStyle *scatterstyle_;
  DataBlockGraph *graphdata_;
  QSharedPointer<QCPGraphDataContainer> functionData_;
  LSCommon::PlotType type_;
  ErrorBar2D *xerrorbar_;
  ErrorBar2D *yerrorbar_;
  bool xerroravailable_;
  bool yerroravailable_;
  // PlotPoint *mPointUnderCursor;
};

#endif  // LINESCATTER2D_H
