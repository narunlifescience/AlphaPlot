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

Grid2D::Grid2D(Axis2D *parent) : QCPGrid(parent) {
  // Set Default grid values

  majorgridcolor_ = pen().color();
  zerolinecolor_ = zeroLinePen().color();
  majorgridstyle_ = pen().style();
  zerolinestyle_ = zeroLinePen().style();
  majorgridthickness_ = pen().widthF();
  zerolinethickness_ = zeroLinePen().widthF();
}

Grid2D::~Grid2D() {}

void Grid2D::setMajorGridColor(const QColor &color) {
  majorgridcolor_ = color;
  QPen p = pen();
  p.setColor(color);
  setPen(p);
}

void Grid2D::setMajorGridStyle(const Qt::PenStyle &penstyle) {
  majorgridstyle_ = penstyle;
  QPen p = pen();
  p.setStyle(penstyle);
  setPen(p);
}

void Grid2D::setMajorGridThickness(const double thickness) {
  majorgridthickness_ = thickness;
  QPen p = pen();
  p.setWidthF(thickness);
  setPen(p);
}

void Grid2D::setMinorGridColor(const QColor &color) {
  QPen p = subGridPen();
  p.setColor(color);
  setSubGridPen(p);
}

void Grid2D::setMinorGridStyle(const Qt::PenStyle &penstyle) {
  QPen p = subGridPen();
  p.setStyle(penstyle);
  setSubGridPen(p);
}

void Grid2D::setMinorGridThickness(const double thickness) {
  QPen p = subGridPen();
  p.setWidthF(thickness);
  setSubGridPen(p);
}

void Grid2D::setZerothLineColor(const QColor &color) {
  zerolinecolor_ = color;
  QPen p = zeroLinePen();
  p.setColor(color);
  setZeroLinePen(p);
}

void Grid2D::setZerothLineStyle(const Qt::PenStyle &penstyle) {
  zerolinestyle_ = penstyle;
  QPen p = zeroLinePen();
  p.setStyle(penstyle);
  setZeroLinePen(p);
}

void Grid2D::setZerothLineThickness(const double thickness) {
  zerolinethickness_ = thickness;
  QPen p = zeroLinePen();
  p.setWidthF(thickness);
  setZeroLinePen(p);
}

void Grid2D::setMajorGridVisible(const bool status) {
  (status) ? setPen(QPen(majorgridcolor_, majorgridthickness_, majorgridstyle_))
           : setPen(Qt::NoPen);
}

void Grid2D::setMinorGridVisible(const bool status) {
  setSubGridVisible(status);
}

void Grid2D::setZerothLineVisible(const bool status) {
  (status)
      ? setZeroLinePen(QPen(zerolinecolor_, zerolinethickness_, zerolinestyle_))
      : setZeroLinePen(Qt::NoPen);
}

QColor Grid2D::getMajorGridColor() const { return majorgridcolor_; }

Qt::PenStyle Grid2D::getMajorGridStyle() const { return majorgridstyle_; }

double Grid2D::getMajorGridwidth() const { return majorgridthickness_; }

QColor Grid2D::getMinorGridColor() const { return subGridPen().color(); }

Qt::PenStyle Grid2D::getMinorGridStyle() const { return subGridPen().style(); }

double Grid2D::getMinorGridwidth() const { return subGridPen().widthF(); }

QColor Grid2D::getZerothLineColor() const { return zerolinecolor_; }

Qt::PenStyle Grid2D::getZeroLineStyle() const { return zerolinestyle_; }

double Grid2D::getZeroLinewidth() const { return zerolinethickness_; }

bool Grid2D::getMajorGridVisible() const {
  if (pen() == Qt::NoPen)
    return false;
  else
    return true;
}

bool Grid2D::getMinorGridVisible() const { return subGridVisible(); }

bool Grid2D::getZerothLineVisible() const {
  if (zeroLinePen() == Qt::NoPen)
    return false;
  else
    return true;
}

QString Grid2D::saveToAproj() {
  QString s = "<grid>/n";
  /* use a xml tree to save it to aproj
   * will impliment later */
  s = +"</grid>/n";
  return s;
}
