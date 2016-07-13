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

   Description : Plot2D grid related stuff */

#ifndef GRID2D_H
#define GRID2D_H

#include "../3rdparty/qcustomplot/qcustomplot.h"
#include "Properties2D.h"

class Grid2D : public QCPGrid {
  Q_OBJECT
 public:
  explicit Grid2D(QCPAxis *parent);
  ~Grid2D();
  // Grid & Zeroline
  void setMajorGridPen(const QPen &pen);
  void setMinorGridPen(const QPen &pen);
  void setZerothLinePen(const QPen &pen);
  void setMajorGridVisible(const bool status);
  void setMinorGridVisible(const bool status);
  void setZerothLineVisible(const bool status);
  QPen getMajorGridPen() const;
  QPen getMinorGridPen() const;
  QPen getZerothLinePen() const;
  bool getMajorGridVisible() const;
  bool getMinorGridVisible() const;
  bool getZerothLineVisible() const;
  Property2D::Grid *getGridProperties() const;

  QString saveToAproj();

 private:
  // Grid & zeroline settings
  Property2D::Grid *gridProperties_;
};

#endif  // GRID2D_H
