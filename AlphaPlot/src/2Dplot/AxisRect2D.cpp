/***************************************************************************
    File                 : AxisRect2D.cpp
    Project              : AlphaPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2016 Arun Narayanankutty
    Email                : n.arun.lifescience@gmail.com
    Description          : Plot2D sub layout elements

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

#include "AxisRect2D.h"

AxisRect2D::AxisRect2D(Plot2D *parent)
    : QCPAxisRect(parent, false), subCanvasBrush(Qt::white) {
  setBackgroundColor(subCanvasBrush);
}

void AxisRect2D::setBackgroundColor(const QBrush &brush) {
  setBackground(brush);
  subCanvasBrush = brush;
}

QBrush AxisRect2D::getBackgroundColor() { return subCanvasBrush; }
