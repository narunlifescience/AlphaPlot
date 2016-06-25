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

   Description : Plot2D top level stuff */

#include "Plot2D.h"

Plot2D::Plot2D(QWidget *parent) : QCustomPlot(parent), canvasBrush_(Qt::white) {
  setBackgroundColor(canvasBrush_);
  setInteraction(QCP::iSelectPlottables, true);
  setInteraction(QCP::iSelectAxes, true);
}

Plot2D::~Plot2D() {}

void Plot2D::setBackgroundColor(const QBrush &brush) {
  setBackground(brush);
  canvasBrush_ = brush;
}

QBrush Plot2D::getBackgroundColor() const { return canvasBrush_; }

void Plot2D::mouseDoubleClickEvent(QMouseEvent *)
{
  emit plot2DDoubleClicked();
}
