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
      setTickLength(static_cast<int>(length / 2), static_cast<int>(length / 2));
      break;
    case None:
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
      setSubTickLength(static_cast<int>(length / 2),
                       static_cast<int>(length / 2));
      break;
    case None:
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

Axis2D::AxisOreantation Axis2D::getOrientation() {
  AxisOreantation orientation;
  switch (axisType()) {
    case QCPAxis::atLeft:
      orientation = Left;
      break;
    case QCPAxis::atBottom:
      orientation = Bottom;
      break;
    case QCPAxis::atRight:
      orientation = Right;
      break;
    case QCPAxis::atTop:
      orientation = Top;
      break;
  }
  return orientation;
}

void Axis2D::setValesInvert(bool status) {
  (status) ? setRangeReversed(true) : setRangeReversed(false);
}

void Axis2D::setValueScaleType(const Axis2D::AxisScaleType &type) {
  switch (type) {
    case Linear:
      setScaleType(QCPAxis::stLinear);
      break;
    case Logarithmic:
      setScaleType(QCPAxis::stLogarithmic);
      break;
  }
}

void Axis2D::AddPropertyItem() {
  Property2D::Data <int> d;
  d.value = 20;
  qDebug() << d.value;
}
