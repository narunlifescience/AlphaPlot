/* This file is part of AlphaPlot.
   Copyright 2016, Arun Narayanankutty <n.arun.lifescience@gmail.com>

   AlphaPlot is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   AlphaPlot is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   You should have received a copy of the GNU General Public License
   along with AlphaPlot.  If not, see <http://www.gnu.org/licenses/>.

   Description : axis rect related stuff */

#ifndef AXISRECT2D_H
#define AXISRECT2D_H

#include "../3rdparty/qcustomplot/qcustomplot.h"
#include "Axis2D.h"
#include "Bar2D.h"
#include "Graph2DCommon.h"
#include "StatBox2D.h"
#include "Vector2D.h"

class Legend2D;
class Table;
class Column;
class Matrix;
class TextItem2D;
class StatBox2D;
class LineItem2D;
class ImageItem2D;
class ColorMap2D;
class XmlStreamWriter;
class LayoutGrid2D;
class Pie2D;
class Plot2D;
class Grid2D;
class Curve2D;
class LineSpecial2D;
class ErrorBar2D;
class StatBox2D;
class LayoutInset2D;
class PickerTool2D;

class AxisRect2D : public QCPAxisRect {
  Q_OBJECT
 private:
  typedef QPair<QPair<Grid2D *, Axis2D *>, QPair<Grid2D *, Axis2D *>> GridPair;
  typedef QVector<TextItem2D *> TextItemVec;
  typedef QVector<LineItem2D *> LineItemVec;
  typedef QVector<ImageItem2D *> ImageItemVec;
  typedef QVector<LineSpecial2D *> LsVec;
  typedef QVector<QPair<LineSpecial2D *, LineSpecial2D *>> ChannelVec;
  typedef QVector<Curve2D *> CurveVec;
  typedef QVector<Vector2D *> VectorVec;
  typedef QVector<Bar2D *> BarVec;
  typedef QVector<QCPBarsGroup *> BarGroupVec;
  typedef QVector<StatBox2D *> StatBoxVec;
  typedef QVector<Pie2D *> PieVec;
  typedef QVector<ColorMap2D *> ColorMapVec;

 public:
  explicit AxisRect2D(Plot2D *parent, PickerTool2D *picker,
                      bool setupDefaultAxis = false);
  ~AxisRect2D();

  void setAxisRectBackground(const QBrush &brush);
  Axis2D *addAxis2D(const Axis2D::AxisOreantation &orientation,
                    const Axis2D::TickerType &tickertype);
  Axis2D *addAxis2DifNeeded(Column *col);
  bool removeAxis2D(Axis2D *axis, bool force = false);
  QBrush getAxisRectBackground() const;
  Grid2D *bindGridTo(Axis2D *axis);

  QList<Axis2D *> getAxes2D() const;
  QList<Axis2D *> getAxes2D(const Axis2D::AxisOreantation &orientation) const;
  QList<Axis2D *> getXAxes2D() const;
  QList<Axis2D *> getYAxes2D() const;
  GridPair getGridPair() const { return gridpair_; }
  TextItemVec getTextItemVec() const { return textvec_; }
  LineItemVec getLineItemVec() const { return linevec_; }
  ImageItemVec getImageItemVec() const { return imagevec_; }
  LsVec getLsVec() const { return lsvec_; }
  ChannelVec getChannelVec() const { return channelvec_; }
  CurveVec getCurveVec() const { return curvevec_; }
  VectorVec getVectorVec() const { return vectorvec_; }
  BarVec getBarVec() const { return barvec_; }
  BarGroupVec getBarGroupVec() const { return bargroupvec_; }
  StatBoxVec getStatBoxVec() const { return statboxvec_; }
  PieVec getPieVec() const { return pievec_; }
  ColorMapVec getColorMapVec() const { return colormapvec_; }
  QList<QCPLayer *> getLayerVec() const { return layers_; }

  Axis2D *getXAxis(const int value);
  int getXAxisNo(Axis2D *axis);
  Axis2D *getYAxis(const int value);
  int getYAxisNo(Axis2D *axis);
  Plot2D *getParentPlot2D() const { return plot2d_; }
  PickerTool2D *getPickerTool() { return picker_; }

  enum class LineScatterType {
    Line2D,
    Scatter2D,
    LineAndScatter2D,
    Spline2D,
  };

  enum class LineScatterSpecialType {
    Area2D,
    VerticalDropLine2D,
    CentralStepAndScatter2D,
    HorizontalStep2D,
    VerticalStep2D,
  };

  enum class BarType {
    HorizontalBars,
    VerticalBars,
  };

  LineSpecial2D *addLineSpecial2DPlot(const LineScatterSpecialType &type,
                                      Table *table, Column *xData,
                                      Column *yData, const int from,
                                      const int to, Axis2D *xAxis,
                                      Axis2D *yAxis);
  QPair<LineSpecial2D *, LineSpecial2D *> addLineSpecialChannel2DPlot(
      Table *table, Column *xData, Column *yData1, Column *yData2,
      const int from, const int to, Axis2D *xAxis, Axis2D *yAxis);
  Curve2D *addCurve2DPlot(const AxisRect2D::LineScatterType &type, Table *table,
                          Column *xcol, Column *ycol, const int from,
                          const int to, Axis2D *xAxis, Axis2D *yAxis);
  Curve2D *addFunction2DPlot(const PlotData::FunctionData funcdata,
                             QVector<double> *xdata, QVector<double> *ydata,
                             Axis2D *xAxis, Axis2D *yAxis, const QString &name);
  Bar2D *addBox2DPlot(const BarType &type, Table *table, Column *xData,
                      Column *yData, const int from, const int to,
                      Axis2D *xAxis, Axis2D *yAxis,
                      const Bar2D::BarStyle &style, int stackposition = -1);
  Vector2D *addVectorPlot(const Vector2D::VectorPlot &vectorplot, Table *table,
                          Column *x1Data, Column *y1Data, Column *x2Data,
                          Column *y2Data, const int from, const int to,
                          Axis2D *xAxis, Axis2D *yAxis);
  StatBox2D *addStatBox2DPlot(const StatBox2D::BoxWhiskerData data,
                              Axis2D *xAxis, Axis2D *yAxis);
  Bar2D *addHistogram2DPlot(const BarType &type, Table *table, Column *yData,
                            const int from, const int to, Axis2D *xAxis,
                            Axis2D *yAxis);
  Pie2D *addPie2DPlot(const Graph2DCommon::PieStyle &style, Table *table,
                      Column *xData, Column *yData, const int from,
                      const int to);
  ColorMap2D *addColorMap2DPlot(Matrix *matrix, Axis2D *xAxis, Axis2D *yAxis);
  TextItem2D *addTextItem2D(const QString text);
  LineItem2D *addLineItem2D();
  LineItem2D *addArrowItem2D();
  ImageItem2D *addImageItem2D(const QString &filename);
  LayoutInset2D *addLayoutInset2D();

  QList<Axis2D *> getAxesOrientedTo(
      const Axis2D::AxisOreantation &orientation) const;

  Legend2D *getLegend() const { return axisRectLegend_; }
  void updateLegendRect();
  void selectAxisRect();
  void addBarsGroup(QCPBarsGroup *bargroup) { bargroupvec_ << bargroup; }
  void addBarsToBarsGroup(QList<Bar2D *> bars, bool autowidthsettins = true);
  void addBarsToStackGroup(QList<Bar2D *> bars);

  // select axisrect with mouse click
  void setSelected(const bool status);
  void drawSelection(QCPPainter *painter);
  bool isSelected() const { return isAxisRectSelected_; }

  // remove
  bool removeTextItem2D(TextItem2D *textitem);
  bool removeLineItem2D(LineItem2D *lineitem);
  bool removeImageItem2D(ImageItem2D *imageitem);
  bool removeLineSpecial2D(LineSpecial2D *ls);
  bool removeChannel2D(QPair<LineSpecial2D *, LineSpecial2D *> channel);
  bool removeCurve2D(Curve2D *curve);
  bool removeStatBox2D(StatBox2D *statbox);
  bool removeVector2D(Vector2D *vector);
  bool removeBar2D(Bar2D *bar);
  bool removePie2D(Pie2D *pie);
  bool removeColorMap2D(ColorMap2D *colormap);

  // move layers
  bool moveLayer(QCPLayer *layer, const QCustomPlot::LayerInsertMode &mode);
  bool movechannellayer(QCPLayer *layer, QCPLayer *layerswap);

  void replotBareBones() const;

  void setPrintorExportJob(const bool value) { printorexportjob_ = value; }
  void setGridPairToNullptr();
  void setItemAxes(Axis2D *xaxis, Axis2D *yaxis);

  Table *getTableByName(QList<Table *> tabs, const QString name);
  Matrix *getMatrixByName(QList<Matrix *> mats, const QString name);

  bool loadLineSpecialChannel2D(XmlStreamReader *xmlreader,
                                QList<Table *> tabs);

 public slots:
  void save(XmlStreamWriter *xmlwriter, const QPair<int, int> rowcol,
            LayoutGrid2D *layoutgrid, const bool saveastemplate = false);
  bool load(XmlStreamReader *xmlreader, QList<Table *> tabs,
            QList<Matrix *> mats);

 protected:
  void mousePressEvent(QMouseEvent *, const QVariant &variant);
  void mouseMoveEvent(QMouseEvent *event, const QPointF &startPos);
  void mouseReleaseEvent(QMouseEvent *event, const QPointF &);
  void draw(QCPPainter *painter);

 signals:
  void AxisRectClicked(AxisRect2D *);
  // created
  void Axis2DCreated(Axis2D *);
  void Axis2DCloned(Axis2D *);
  void TextItem2DCreated(TextItem2D *);
  void LineItem2DCreated(LineItem2D *);
  void ImageItem2DCreated(ImageItem2D *);
  void LineSpecial2DCreated(LineSpecial2D *);
  void LineSpecialChannel2DCreated(QPair<LineSpecial2D *, LineSpecial2D *>);
  void Curve2DCreated(Curve2D *);
  void StatBox2DCreated(StatBox2D *);
  void Vector2DCreated(Vector2D *);
  void Bar2DCreated(Bar2D *);
  void Pie2DCreated(Pie2D *);
  void ColorMap2DCreated(ColorMap2D *);
  void ErrorBar2DCreated(ErrorBar2D *);
  // Removed
  void Axis2DRemoved(AxisRect2D *);
  void TextItem2DRemoved(AxisRect2D *);
  void LineItem2DRemoved(AxisRect2D *);
  void ImageItem2DRemoved(AxisRect2D *);
  void LineSpecial2DRemoved(AxisRect2D *);
  void LineSpecialChannel2DRemoved(AxisRect2D *);
  void Curve2DRemoved(AxisRect2D *);
  void StatBox2DRemoved(AxisRect2D *);
  void Vector2DRemoved(AxisRect2D *);
  void Bar2DRemoved(AxisRect2D *);
  void Pie2DRemoved(AxisRect2D *);
  void ColorMap2DRemoved(AxisRect2D *);
  void ErrorBar2DRemoved(AxisRect2D *);
  // Layer moved
  void LayerMoved(AxisRect2D *);
  void TextItem2DMoved();
  void LineItem2DMoved();
  void ImageItem2DMoved();

 private slots:
  void legendClick();

 private:
  void setbarsstyle();

 private:
  Plot2D *plot2d_;
  QBrush axisRectBackGround_;
  Legend2D *axisRectLegend_;
  bool isAxisRectSelected_;
  bool printorexportjob_;
  GridPair gridpair_;
  TextItemVec textvec_;
  LineItemVec linevec_;
  ImageItemVec imagevec_;
  ChannelVec channelvec_;
  LsVec lsvec_;
  CurveVec curvevec_;
  VectorVec vectorvec_;
  BarVec barvec_;
  BarGroupVec bargroupvec_;
  StatBoxVec statboxvec_;
  PieVec pievec_;
  ColorMapVec colormapvec_;
  QList<Axis2D *> axes_;
  QList<QCPLayer *> layers_;
  PickerTool2D *picker_;
};

#endif  // AXISRECT2D_H
