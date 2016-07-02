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
#include "Grid2D.h"
#include "LineScatter2D.h"

class AxisRect2D : public QCPAxisRect {
 public:
  AxisRect2D(QCustomPlot *parent = 0, bool setupDefaultAxis = false);
  ~AxisRect2D();

  void setAxisRectBackground(const QBrush &brush);
  Axis2D *addAxis2D(const Axis2D::AxisOreantation &orientation);
  bool removeAxis2D(Axis2D *axis);
  QBrush getAxisRectBackground() const;
  void bindGridTo(Axis2D *axis);

  QList<Axis2D *> getAxes2D() const;
  QList<Axis2D *> getAxes2D(const Axis2D::AxisOreantation &orientation) const;

  enum LineScatterType {
    Line2D,
    Scatter2D,
    LineAndScatter2D,
    VerticalDropLine2D,
    Spline2D,
    CentralStepAndScatter2D,
    HorizontalStep2D,
    VerticalStep2D,
  };

  LineScatter2D *addLineScatter2DPlot(const LineScatterType &type,
                                      QCPDataMap *dataMap, Axis2D *xAxis,
                                      Axis2D *yAxis);
  // template <typename T> void remove2DPlot(T &plot) {
  //}

 private:
  QList<Axis2D *> getAxesToMap(
      const Axis2D::AxisOreantation &orientation) const;

  QBrush axisRectBackGround_;
  QMap<Axis2D::AxisOreantation, QList<Axis2D *>> axises_;
  QMap<LineScatterType, QPair<LineScatter2D *, QCPDataMap *>> lineScatter_;
  QPair<Grid2D *, Grid2D *> grids_;
};

#endif  // AXISRECT2D_H
