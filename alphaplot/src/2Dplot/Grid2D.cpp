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

#include "Grid2D.h"
#include "Axis2D.h"

Grid2D::Grid2D(Axis2D *parent)
    : QCPGrid(parent), gridProperties_(new Property2D::Grid()) {
  // Set Default grid values
  gridProperties_->majorGrid.visible.value = true;
  gridProperties_->minorGrid.visible.value = false;
  gridProperties_->zeroLine.visible.value = true;
  gridProperties_->majorGrid.strokeColor.value = pen().color();
  gridProperties_->minorGrid.strokeColor.value = subGridPen().color();
  gridProperties_->zeroLine.strokeColor.value = zeroLinePen().color();
  gridProperties_->majorGrid.style.value = pen().style();
  gridProperties_->minorGrid.style.value = subGridPen().style();
  gridProperties_->zeroLine.style.value = zeroLinePen().style();
  gridProperties_->majorGrid.strokeThickness.value = pen().width();
  gridProperties_->minorGrid.strokeThickness.value = subGridPen().width();
  gridProperties_->zeroLine.strokeThickness.value = zeroLinePen().width();
}

Grid2D::~Grid2D() { delete gridProperties_; }

// Set Grid Parameters
void Grid2D::setMajorGridPen(const QPen &pen) {
  setPen(pen);
  gridProperties_->majorGrid.strokeColor.value = pen.color();
  gridProperties_->majorGrid.style.value = pen.style();
  gridProperties_->majorGrid.strokeThickness.value = pen.width();
}

void Grid2D::setMinorGridPen(const QPen &pen) {
  setSubGridPen(pen);
  gridProperties_->minorGrid.strokeColor.value = pen.color();
  gridProperties_->minorGrid.style.value = pen.style();
  gridProperties_->minorGrid.strokeThickness.value = pen.width();
}

void Grid2D::setZerothLinePen(const QPen &pen) {
  setZeroLinePen(pen);
  gridProperties_->zeroLine.strokeColor.value = pen.color();
  gridProperties_->zeroLine.style.value = pen.style();
  gridProperties_->zeroLine.strokeThickness.value = pen.width();
}

void Grid2D::setMajorGridVisible(const bool status) {
  QPen majorGridPen;
  majorGridPen.setColor(gridProperties_->majorGrid.strokeColor.value);
  majorGridPen.setStyle(gridProperties_->majorGrid.style.value);
  majorGridPen.setWidth(gridProperties_->majorGrid.strokeThickness.value);
  (status) ? setPen(majorGridPen) : setPen(Qt::NoPen);
  gridProperties_->majorGrid.visible.value = status;
}

void Grid2D::setMinorGridVisible(const bool status) {
  setSubGridVisible(status);
  gridProperties_->minorGrid.visible.value = status;
}

void Grid2D::setZerothLineVisible(const bool status) {
  QPen zerolinepen;
  zerolinepen.setColor(gridProperties_->zeroLine.strokeColor.value);
  zerolinepen.setStyle(gridProperties_->zeroLine.style.value);
  zerolinepen.setWidth(gridProperties_->zeroLine.strokeThickness.value);
  (status) ? setZeroLinePen(zerolinepen) : setZeroLinePen(Qt::NoPen);
  gridProperties_->zeroLine.visible.value = status;
}

QPen Grid2D::getMajorGridPen() const {
  QPen majorGridPen;
  majorGridPen.setColor(gridProperties_->majorGrid.strokeColor.value);
  majorGridPen.setStyle(gridProperties_->majorGrid.style.value);
  majorGridPen.setWidth(gridProperties_->majorGrid.strokeThickness.value);
  return majorGridPen;
}

QPen Grid2D::getMinorGridPen() const {
  QPen minorGridPen;
  minorGridPen.setColor(gridProperties_->minorGrid.strokeColor.value);
  minorGridPen.setStyle(gridProperties_->minorGrid.style.value);
  minorGridPen.setWidth(gridProperties_->minorGrid.strokeThickness.value);
  return minorGridPen;
}

QPen Grid2D::getZerothLinePen() const {
  QPen zerolinepen;
  zerolinepen.setColor(gridProperties_->zeroLine.strokeColor.value);
  zerolinepen.setStyle(gridProperties_->zeroLine.style.value);
  zerolinepen.setWidth(gridProperties_->zeroLine.strokeThickness.value);
  return zerolinepen;
}

bool Grid2D::getMajorGridVisible() const {
  return gridProperties_->majorGrid.visible.value;
}

bool Grid2D::getMinorGridVisible() const {
  return gridProperties_->minorGrid.visible.value;
}

bool Grid2D::getZerothLineVisible() const {
  return gridProperties_->zeroLine.visible.value;
}

Property2D::Grid *Grid2D::getGridProperties() const { return gridProperties_; }

QString Grid2D::saveToAproj() {
  QString s = "<grid>/n";
  /* use a xml tree to save it to aproj
   * will impliment later */
  s = +"</grid>/n";
  return s;
}
