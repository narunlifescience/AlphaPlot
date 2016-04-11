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

#ifndef GRID2D_H
#define GRID2D_H

#include "qcustomplot.h"


class Grid2D : public QCPGrid
{
public:
  Grid2D(QCPAxis *parent = 0);

  // Grid & Zeroline
  void setMajorGridPen(const QPen &pen);
  void setMinorGridPen(const QPen &pen);
  void setZerothLinePen(const QPen &pen);
  void setMajorGridVisible(bool status);
  void setMinorGridVisible(bool status);
  void setZerothLineVisible(bool status);
  QPen getMajorGridPen();
  QPen getMinorGridPen();
  QPen getZerothLinePen();
  bool getMajorGridVisible();
  bool getMinorGridVisible();
  bool getZerothLineVisible();

  void copy(Grid2D *grid);
  QString saveToAproj();

private:
 // struct {
 // Grid & zeroline settings
 QPen majorGridPen;
 QPen minorGridPen;
 QPen zeroLinePen;
 bool majorGridVisible;
 bool minorGridVisible;
 bool zeroLineVisible;
};

#endif // GRID2D_H
