/***************************************************************************
    File                 : Grid2D.cpp
    Project              : AlphaPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2016 Arun Narayanankutty
    Email                : n.arun.lifescience@gmail.com
    Description          : Plot2D grid

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

void Grid2D::setMajorGridVisible(bool status) {
  (status) ? setPen(majorGridPen) : setPen(Qt::NoPen);
  majorGridVisible = status;
}

void Grid2D::setMinorGridVisible(bool status) {
  setSubGridVisible(status);
  minorGridVisible = status;
}

void Grid2D::setZerothLineVisible(bool status) {
  (status) ? setZeroLinePen(zeroLinePen) : setZeroLinePen(Qt::NoPen);
  zeroLineVisible = status;
}

QPen Grid2D::getMajorGridPen() { return majorGridPen; }

QPen Grid2D::getMinorGridPen() { return minorGridPen; }

QPen Grid2D::getZerothLinePen() { return zeroLinePen; }

bool Grid2D::getMajorGridVisible() { return majorGridVisible; }

bool Grid2D::getMinorGridVisible() { return minorGridVisible; }

bool Grid2D::getZerothLineVisible() { return zeroLineVisible; }

void Grid2D::copy(Grid2D *grid)
{
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
  s =+ "</grid>/n";
  return s;
}
