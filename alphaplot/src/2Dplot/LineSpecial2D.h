#ifndef LINESCATTER2D_H
#define LINESCATTER2D_H

#include "../3rdparty/qcustomplot/qcustomplot.h"
#include "Graph2DCommon.h"

class Axis2D;
class Column;
class Table;
class DataBlockGraph;
class ErrorBar2D;
class XmlStreamReader;
class XmlStreamWriter;

class LineSpecial2D : public QCPGraph {
  Q_OBJECT
 public:
  LineSpecial2D(Table *table, Column *xcol, Column *ycol, int from, int to,
                Axis2D *xAxis, Axis2D *yAxis);
  ~LineSpecial2D();

  void setXerrorBar(Table *table, Column *errorcol, int from, int to);
  void setYerrorBar(Table *table, Column *errorcol, int from, int to);
  void setGraphData(Table *table, Column *xcol, Column *ycol, int from, int to);
  void removeXerrorBar();
  void removeYerrorBar();
  // Getters
  Graph2DCommon::LineStyleType getlinetype_lsplot() const;
  Qt::PenStyle getlinestrokestyle_lsplot() const;
  QColor getlinestrokecolor_lsplot() const;
  double getlinestrokethickness_lsplot() const;
  bool getlinefillstatus_lsplot() const;
  QColor getlinefillcolor_lsplot() const;
  Qt::BrushStyle getlinefillstyle_lsplot() const;
  bool getlineantialiased_lsplot() const;
  Graph2DCommon::ScatterStyle getscattershape_lsplot() const;
  QColor getscatterfillcolor_lsplot() const;
  double getscattersize_lsplot() const;
  Qt::PenStyle getscatterstrokestyle_lsplot() const;
  QColor getscatterstrokecolor_lsplot() const;
  double getscatterstrokethickness_lsplot() const;
  bool getscatterantialiased_lsplot() const;
  bool getlegendvisible_lsplot() const;
  QString getlegendtext_lsplot() const;
  Axis2D *getxaxis() const;
  Axis2D *getyaxis() const;
  DataBlockGraph *getdatablock_lsplot() const { return graphdata_; }
  ErrorBar2D *getxerrorbar_lsplot() { return xerrorbar_; }
  ErrorBar2D *getyerrorbar_lsplot() { return yerrorbar_; }
  QIcon getIcon() const { return icon_; }
  // Setters
  void setlinetype_lsplot(const Graph2DCommon::LineStyleType &line);
  void setlinestrokestyle_lsplot(const Qt::PenStyle &style);
  void setlinestrokecolor_lsplot(const QColor &color);
  void setlinestrokethickness_lsplot(const double value);
  void setlinefillstatus_lsplot(bool status);
  void setlinefillcolor_lsplot(const QColor &color);
  void setlinefillstyle_lsplot(const Qt::BrushStyle &style);
  void setlineantialiased_lsplot(const bool value);
  void setscattershape_lsplot(const Graph2DCommon::ScatterStyle &shape);
  void setscatterfillcolor_lsplot(const QColor &color);
  void setscattersize_lsplot(const double value);
  void setscatterstrokestyle_lsplot(const Qt::PenStyle &style);
  void setscatterstrokecolor_lsplot(const QColor &color);
  void setscatterstrokethickness_lsplot(const double value);
  void setscatterantialiased_lsplot(const bool value);
  void setlegendvisible_lsplot(const bool value);
  void setlegendtext_lsplot(const QString &legendtext);
  void setxaxis_lsplot(Axis2D *axis);
  void setyaxis_lsplot(Axis2D *axis);

  void save(XmlStreamWriter *xmlwriter, int xaxis, int yaxis);
  bool load(XmlStreamReader *xmlreader);

 protected:
  void mousePressEvent(QMouseEvent *event, const QVariant &details);

 private:
  void datapicker(QMouseEvent *event, const QVariant &details);
  void movepicker(QMouseEvent *event, const QVariant &details);
  void removepicker(QMouseEvent *event, const QVariant &details);
  void reloadIcon();

 private:
  Axis2D *xAxis_;
  Axis2D *yAxis_;
  QCPScatterStyle *scatterstyle_;
  DataBlockGraph *graphdata_;
  QSharedPointer<QCPGraphDataContainer> functionData_;
  ErrorBar2D *xerrorbar_;
  ErrorBar2D *yerrorbar_;
  QString layername_;
  bool xerroravailable_;
  bool yerroravailable_;
  QIcon icon_;
};

#endif  // LINESCATTER2D_H
