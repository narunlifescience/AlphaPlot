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
#include "Curve2D.h"
#include "Grid2D.h"
#include "LineScatter2D.h"
#include "Pie2D.h"
#include "Spline2D.h"
#include "Vector2D.h"

class Legend2D;
class Column;

class AxisRect2D : public QCPAxisRect {
  Q_OBJECT
 private:
  typedef QPair<QPair<Grid2D *, Axis2D *>, QPair<Grid2D *, Axis2D *>> GridPair;
  typedef QVector<LineScatter2D *> LsVec;
  typedef QVector<Spline2D *> SplineVec;
  typedef QVector<Vector2D *> VectorVec;
  typedef QVector<Bar2D *> BarVec;

 public:
  AxisRect2D(QCustomPlot *parent = nullptr, bool setupDefaultAxis = false);
  ~AxisRect2D();

  void setAxisRectBackground(const QBrush &brush);
  Axis2D *addAxis2D(const Axis2D::AxisOreantation &orientation);
  bool removeAxis2D(Axis2D *axis);
  QBrush getAxisRectBackground() const;
  Grid2D *bindGridTo(Axis2D *axis);

  QList<Axis2D *> getAxes2D() const;
  QList<Axis2D *> getAxes2D(const Axis2D::AxisOreantation &orientation) const;
  QList<Axis2D *> getXAxes2D() const;
  QList<Axis2D *> getYAxes2D() const;
  GridPair getGridPair() const { return gridpair_; }
  LsVec getLsVec() const { return lsvec_; }
  SplineVec getSplineVec() const { return splinevec_; }
  VectorVec getVectorVec() const { return vectorvec_; }
  BarVec getBarVec() const { return barvec_; }

  Axis2D *getXAxis(int value);
  Axis2D *getYAxis(int value);

  enum LineScatterType {
    Line2D,
    Scatter2D,
    LineAndScatter2D,
    VerticalDropLine2D,
    CentralStepAndScatter2D,
    HorizontalStep2D,
    VerticalStep2D,
    Area2D,
  };

  enum BarType {
    HorizontalBars,
    VerticalBars,
  };

  LineScatter2D *addLineScatter2DPlot(const LineScatterType &type,
                                      Column *xData, Column *yData, int from,
                                      int to, Axis2D *xAxis, Axis2D *yAxis);
  Spline2D *addSpline2DPlot(Column *xData, Column *yData, int from, int to,
                            Axis2D *xAxis, Axis2D *yAxis);
  LineScatter2D *addLineFunction2DPlot(QVector<double> *xdata,
                                       QVector<double> *ydata, Axis2D *xAxis,
                                       Axis2D *yAxis);
  Curve2D *addCurveFunction2DPlot(QVector<double> *xdata,
                                  QVector<double> *ydata, Axis2D *xAxis,
                                  Axis2D *yAxis);
  Bar2D *addBox2DPlot(const BarType &type, Column *xData, Column *yData,
                      int from, int to, Axis2D *xAxis, Axis2D *yAxis);
  Vector2D *addVectorPlot(const Vector2D::VectorPlot &vectorplot,
                          Column *x1Data, Column *y1Data, Column *x2Data,
                          Column *y2Data, int from, int to, Axis2D *xAxis,
                          Axis2D *yAxis);
  Pie2D *addPie2DPlot(Column *xData, int from, int to);

  QList<Axis2D *> getAxesOrientedTo(
      const Axis2D::AxisOreantation &orientation) const;

  QCPLegend *getLegend() const {
    return reinterpret_cast<QCPLegend *>(axisRectLegend_);
  }
  void updateLegendRect();

  // select axisrect with mouse click
  void setSelected(const bool status);
  void drawSelection(QCPPainter *painter);
  bool isSelected() { return isAxisRectSelected_; }

 public slots:
  Axis2D *addLeftAxis2D() { return addAxis2D(Axis2D::AxisOreantation::Left); }
  Axis2D *addBottomAxis2D() {
    return addAxis2D(Axis2D::AxisOreantation::Bottom);
  }
  Axis2D *addRightAxis2D() { return addAxis2D(Axis2D::AxisOreantation::Right); }
  Axis2D *addTopAxis2D() { return addAxis2D(Axis2D::AxisOreantation::Top); }

 protected:
  void mousePressEvent(QMouseEvent *, const QVariant &);
  void mouseReleaseEvent(QMouseEvent *event, const QPointF &startPos);
  void draw(QCPPainter *painter);

 signals:
  void AxisRectClicked(AxisRect2D *);
  void AxisCreated(Axis2D *);
  void AxisRemoved(AxisRect2D *);
  void LineScatterCreated(LineScatter2D *);
  void SplineCreated(Spline2D *);
  void VectorCreated(Vector2D *);
  void LineScatterRemoved(AxisRect2D *);
  void BarCreated(Bar2D *);
  void BarRemoved();

 private slots:
  void legendClick();
  void addfunctionplot();
  void addplot();

 private:
  QBrush axisRectBackGround_;
  Legend2D *axisRectLegend_;
  bool isAxisRectSelected_;
  GridPair gridpair_;
  LsVec lsvec_;
  SplineVec splinevec_;
  VectorVec vectorvec_;
  BarVec barvec_;
  QList<Axis2D *> axes_;
  // QVector<QPair<StatBox2D *, QPair<Axis2D *, Axis2D *>>> statboxplots_;
  // Histogram
  // Other types
};

#endif  // AXISRECT2D_H
