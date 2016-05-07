/***************************************************************************
    File                 : Axis2D.cpp
    Project              : AlphaPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2016 Arun Narayanankutty
    Email                : n.arun.lifescience@gmail.com
    Description          : Plot2D axis

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *  This program is free software; you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation; either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the Free Software           *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor,                    *
 *   Boston, MA  02110-1301  USA                                           *
 *                                                                         *
 ***************************************************************************/

#ifndef AXIS2D_H
#define AXIS2D_H

#include "qcustomplot.h"

class Axis2D : public QCPAxis {
  Q_OBJECT
 public:
  Axis2D(QCPAxisRect* parent, AxisType type);
  ~Axis2D();

  enum TickOrientation { Inward, Outward, Bothwards, None };

  // Axis Ticks
  void setMajorTickPen(const QPen &pen);
  void setMinorTickPen(const QPen &pen);
  void setMajorTickLength(const int &length, TickOrientation &orientation);
  void setMinorTickLength(const int &length, TickOrientation &orientation);
  void setMajorTickVisible(bool status);
  void setMinorTickVisible(bool status);

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
