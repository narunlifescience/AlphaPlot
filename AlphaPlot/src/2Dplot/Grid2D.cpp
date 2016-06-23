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

Grid2D::Grid2D(QCPAxis *parent)
    : QCPGrid(parent),
      majorGridPen(QPen(Qt::darkGray, 0, Qt::DotLine)),
      minorGridPen(QPen(Qt::lightGray, 0, Qt::DotLine)),
      zeroLinePen(QPen(Qt::gray, 0, Qt::SolidLine)),
      majorGridVisible(true),
      minorGridVisible(false),
      zeroLineVisible(true) {
  // Set Default grid values
  setMajorGridPen(majorGridPen);
  setMajorGridVisible(majorGridVisible);
  setMinorGridPen(minorGridPen);
  setMinorGridVisible(minorGridVisible);
  setZerothLinePen(zeroLinePen);
  setZerothLineVisible(zeroLineVisible);
}

Grid2D::~Grid2D() {}

// Set Grid Parameters
void Grid2D::setMajorGridPen(const QPen &pen) {
  setPen(pen);
  majorGridPen = pen;
}

void Grid2D::setMinorGridPen(const QPen &pen) {
  setSubGridPen(pen);
  minorGridPen = pen;
}

void Grid2D::setZerothLinePen(const QPen &pen) {
  setZeroLinePen(pen);
  zeroLinePen = pen;
}

void Grid2D::setMajorGridVisible(const bool status) {
  (status) ? setPen(majorGridPen) : setPen(Qt::NoPen);
  majorGridVisible = status;
}

void Grid2D::setMinorGridVisible(const bool status) {
  setSubGridVisible(status);
  minorGridVisible = status;
}

void Grid2D::setZerothLineVisible(const bool status) {
  (status) ? setZeroLinePen(zeroLinePen) : setZeroLinePen(Qt::NoPen);
  zeroLineVisible = status;
}

QPen Grid2D::getMajorGridPen() const { return majorGridPen; }

QPen Grid2D::getMinorGridPen() const { return minorGridPen; }

QPen Grid2D::getZerothLinePen() const { return zeroLinePen; }

bool Grid2D::getMajorGridVisible() const { return majorGridVisible; }

bool Grid2D::getMinorGridVisible() const { return minorGridVisible; }

bool Grid2D::getZerothLineVisible() const { return zeroLineVisible; }

void Grid2D::copy(Grid2D *grid) const {
  if (!grid) return;

  grid->setMajorGridPen(majorGridPen);
  grid->setMajorGridVisible(majorGridVisible);
  grid->setMinorGridPen(minorGridPen);
  grid->setMinorGridVisible(minorGridVisible);
  grid->setZerothLinePen(zeroLinePen);
  grid->setZerothLineVisible(zeroLineVisible);
}

QString Grid2D::saveToAproj() {
  QString s = "<grid>/n";
  /* use a xml tree to save it to aproj
   * will impliment later */
  s = +"</grid>/n";
  return s;
}
