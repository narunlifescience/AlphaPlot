#ifndef CURVE2D_H
#define CURVE2D_H

#include "../3rdparty/qcustomplot/qcustomplot.h"
#include "Axis2D.h"
#include "Graph2DCommon.h"

class Table;
class Column;
class DataBlockCurve;
class ErrorBar2D;

class Curve2D : public QCPCurve {
  Q_OBJECT
 public:
  enum class Curve2DType { Curve, Spline };
  Curve2D(Curve2DType curve2dtype, Table *table, Column *xcol, Column *ycol,
          int from, int to, Axis2D *xAxis, Axis2D *yAxis);
  Curve2D(const PlotData::FunctionData funcdata, QVector<double> *xdata,
          QVector<double> *ydata, Axis2D *xAxis, Axis2D *yAxis);
  void init();
  ~Curve2D();

  void setXerrorBar(Table *table, Column *errorcol, int from, int to);
  void setYerrorBar(Table *table, Column *errorcol, int from, int to);
  void removeXerrorBar();
  void removeYerrorBar();

  void setGraphData(QVector<double> *xdata, QVector<double> *ydata);
  void setCurveData(Table *table, Column *xcol, Column *ycol, int from, int to);

  // Getters
  int getlinetype_cplot() const;
  Qt::PenStyle getlinestrokestyle_cplot() const;
  QColor getlinestrokecolor_cplot() const;
  double getlinestrokethickness_cplot() const;
  QPen getlinepen_cplot() const;
  QColor getlinefillcolor_cplot() const;
  Qt::BrushStyle getlinefillstyle_cplot() const;
  QBrush getlinebrush_cplot() const;
  bool getlineantialiased_cplot() const;
  bool getlinefillstatus_cplot() const;
  Graph2DCommon::ScatterStyle getscattershape_cplot() const;
  QColor getscatterfillcolor_cplot() const;
  QBrush getscatterbrush_cplot() const;
  double getscattersize_cplot() const;
  Qt::PenStyle getscatterstrokestyle_cplot() const;
  QColor getscatterstrokecolor_cplot() const;
  double getscatterstrokethickness_cplot() const;
  QPen getscatterpen_cplot() const;
  bool getscatterantialiased_cplot() const;
  bool getlegendvisible_cplot() const;
  QString getlegendtext_cplot() const;
  Axis2D *getxaxis() const;
  Axis2D *getyaxis() const;
  Graph2DCommon::PlotType getplottype_cplot() const { return type_; }
  Curve2D::Curve2DType getcurvetype_cplot() const { return curve2dtype_; }
  DataBlockCurve *getdatablock_cplot() const { return curvedata_; }
  const PlotData::FunctionData getfuncdata_cplot() const { return funcdata_; }
  QPen getSplinePen() { return splinePen_; }
  QBrush getSplineBrush() { return splineBrush_; }
  ErrorBar2D *getxerrorbar_curveplot() { return xerrorbar_; }
  ErrorBar2D *getyerrorbar_curveplot() { return yerrorbar_; }
  QIcon getIcon() const { return icon_; }
  // Setters
  void setxaxis_cplot(Axis2D *axis);
  void setyaxis_cplot(Axis2D *axis);
  void setlinetype_cplot(const int type);
  void setlinestrokestyle_cplot(const Qt::PenStyle &style);
  void setlinestrokecolor_cplot(const QColor &color);
  void setlinestrokethickness_cplot(const double value);
  void setlinefillcolor_cplot(const QColor &color);
  void setlinefillstyle_cplot(const Qt::BrushStyle &style);
  void setlineantialiased_cplot(const bool value);
  void setscattershape_cplot(const Graph2DCommon::ScatterStyle &shape);
  void setscatterfillcolor_cplot(const QColor &color);
  void setscattersize_cplot(const double value);
  void setscatterstrokestyle_cplot(const Qt::PenStyle &style);
  void setscatterstrokecolor_cplot(const QColor &color);
  void setscatterstrokethickness_cplot(const double value);
  void setscatterantialiased_cplot(const bool value);
  void setlinefillstatus_cplot(const bool value);
  void setlegendvisible_cplot(const bool value);
  void setlegendtext_cplot(const QString &text);

  void save(XmlStreamWriter *xmlwriter, int xaxis, int yaxis);
  bool load(XmlStreamReader *xmlreader);

 protected:
  void draw(QCPPainter *painter);
  void drawCurveLine(QCPPainter *painter, const QVector<QPointF> &lines) const;
  void mousePressEvent(QMouseEvent *event, const QVariant &details);

 private:
  void drawSpline(QCPPainter *painter);
  void loadSplineData();
  QVector<QPointF> calculateControlPoints(const QVector<QPointF> &points);
  QVector<qreal> firstControlPoints(const QVector<qreal> &vector);
  void datapicker(QMouseEvent *event, const QVariant &details);
  void movepicker(QMouseEvent *event, const QVariant &details);
  void removepicker(QMouseEvent *event, const QVariant &details);
  void dataRangePicker(QMouseEvent *event, const QVariant &details);
  void reloadIcon();

 private:
  Axis2D *xAxis_;
  Axis2D *yAxis_;
  QString layername_;
  QCPScatterStyle *scatterstyle_;
  DataBlockCurve *curvedata_;
  const PlotData::FunctionData funcdata_;
  QSharedPointer<QCPCurveDataContainer> functionData_;
  Graph2DCommon::PlotType type_;
  Curve2DType curve2dtype_;
  ErrorBar2D *xerrorbar_;
  ErrorBar2D *yerrorbar_;
  bool xerroravailable_;
  bool yerroravailable_;
  QPen splinePen_;
  QBrush splineBrush_;
  QVector<QPointF> *splinepoints_;
  QVector<QPointF> *splinecontrolpoints_;
  QIcon icon_;
};

#endif  // CURVE2D_H
