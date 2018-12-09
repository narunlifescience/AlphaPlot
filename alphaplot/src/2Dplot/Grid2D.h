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

class Axis2D;

class Grid2D : public QCPGrid {
  Q_OBJECT
 public:
  explicit Grid2D(Axis2D *parent);
  ~Grid2D();
  // Grid & Zeroline
  void setMajorGridColor(const QColor &color);
  void setMajorGridStyle(const Qt::PenStyle &penstyle);
  void setMajorGridThickness(const double thickness);
  void setMinorGridColor(const QColor &color);
  void setMinorGridStyle(const Qt::PenStyle &penstyle);
  void setMinorGridThickness(const double thickness);
  void setZerothLineColor(const QColor &color);
  void setZerothLineStyle(const Qt::PenStyle &penstyle);
  void setZerothLineThickness(const double thickness);
  void setMajorGridVisible(const bool status);
  void setMinorGridVisible(const bool status);
  void setZerothLineVisible(const bool status);
  QColor getMajorGridColor() const;
  Qt::PenStyle getMajorGridStyle() const;
  double getMajorGridwidth() const;
  QColor getMinorGridColor() const;
  Qt::PenStyle getMinorGridStyle() const;
  double getMinorGridwidth() const;
  QColor getZerothLineColor() const;
  Qt::PenStyle getZeroLineStyle() const;
  double getZeroLinewidth() const;
  bool getMajorGridVisible() const;
  bool getMinorGridVisible() const;
  bool getZerothLineVisible() const;

  QString saveToAproj();

 private:
  // Grid & zeroline settings
  Axis2D *axis_;
  QString layername_;
  QColor majorgridcolor_;
  Qt::PenStyle majorgridstyle_;
  double majorgridthickness_;
  QColor zerolinecolor_;
  Qt::PenStyle zerolinestyle_;
  double zerolinethickness_;

};

#endif  // GRID2D_H
