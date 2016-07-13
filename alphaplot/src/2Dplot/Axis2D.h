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

   Description : Plot2D axis related stuff */

#ifndef AXIS2D_H
#define AXIS2D_H

#include "../3rdparty/qcustomplot/qcustomplot.h"
#include "Properties2D.h"

class Axis2D : public QCPAxis {
  Q_OBJECT
 public:
  Axis2D(QCPAxisRect *parent, AxisType type);
  ~Axis2D();

  enum TickOrientation { Inward, Outward, Bothwards, None };
  enum AxisOreantation { Left = 0, Bottom = 1, Right = 2, Top = 3 };
  enum AxisScaleType { Linear, Logarithmic };

  // Axis Ticks
  void setMajorTickPen(const QPen &pen);
  void setMinorTickPen(const QPen &pen);
  void setMajorTickLength(const int &length, TickOrientation &orientation);
  void setMinorTickLength(const int &length, TickOrientation &orientation);
  void setMajorTickVisible(bool status);
  void setMinorTickVisible(bool status);
  AxisOreantation getOrientation();

  void setValesInvert(bool status);
  void setValueScaleType(const AxisScaleType &type);

  void AddPropertyItem();

 private:
  QPen majorTickPen;
  QPen minorTickPen;
  int majorTickLength;
  int minorTickLength;
  TickOrientation majorTickOrientation;
  TickOrientation minorTickOrientation;
  bool majorTickVisible;
  bool minorTickVisible;
};

#endif  // AXIS2D_H
