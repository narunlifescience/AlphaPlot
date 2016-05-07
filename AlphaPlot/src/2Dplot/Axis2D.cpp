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

#include "Axis2D.h"

Axis2D::Axis2D(QCPAxisRect *parent, AxisType type)
    : QCPAxis(parent, type),
      majorTickPen(QPen(Qt::black, 0, Qt::SolidLine)),
      minorTickPen(QPen(Qt::black, 0, Qt::SolidLine)),
      majorTickLength(5),
      minorTickLength(2),
      majorTickOrientation(Axis2D::Inward),
      minorTickOrientation(Axis2D::Inward),
      majorTickVisible(true),
      minorTickVisible(true) {
  setMajorTickPen(majorTickPen);
  setMinorTickPen(minorTickPen);
  setMajorTickLength(majorTickLength, majorTickOrientation);
  setMinorTickLength(minorTickLength, minorTickOrientation);
  setMajorTickVisible(majorTickVisible);
  setMinorTickVisible(minorTickVisible);
}

Axis2D::~Axis2D() {}

// Set Tick parameters
void Axis2D::setMajorTickPen(const QPen &pen) {
  majorTickPen = pen;
  setTickPen(pen);
}

void Axis2D::setMinorTickPen(const QPen &pen) {
  minorTickPen = pen;
  setSubTickPen(pen);
}

void Axis2D::setMajorTickLength(const int &length,
                                Axis2D::TickOrientation &orient) {
  majorTickLength = length;
  majorTickOrientation = orient;
  switch (orient) {
    case Inward:
      setTickLength(length, 0);
      break;
    case Outward:
      setTickLength(0, length);
      break;
    case Bothwards:
      setTickLength(static_cast<float>(length / 2),
                    static_cast<float>(length / 2));
      break;
    default:
      break;
  }
}

void Axis2D::setMinorTickLength(const int &length,
                                Axis2D::TickOrientation &orient) {
  minorTickLength = length;
  minorTickOrientation = orient;
  switch (orient) {
    case Inward:
      setSubTickLength(length, 0);
      break;
    case Outward:
      setSubTickLength(0, length);
      break;
    case Bothwards:
      setSubTickLength(static_cast<float>(length / 2),
                       static_cast<float>(length / 2));
      break;
    default:
      break;
  }
}

void Axis2D::setMajorTickVisible(bool status) {
  majorTickVisible = status;
  (status) ? setTickPen(majorTickPen) : setTickPen(Qt::NoPen);
}

void Axis2D::setMinorTickVisible(bool status) {
  minorTickVisible = status;
  (status) ? setSubTickPen(minorTickPen) : setSubTickPen(Qt::NoPen);
}
