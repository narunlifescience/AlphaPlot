/***************************************************************************
    File                 : Differentiation.cpp
    Project              : AlphaPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Ion Vasilief
    Email (use @ for *)  : ion_vasilief*yahoo.fr
    Description          : Numerical differentiation of data sets

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
#include "Differentiation.h"
#include "2Dplot/AxisRect2D.h"
#include "2Dplot/Layout2D.h"
#include "core/column/Column.h"

#include <QLocale>

Differentiation::Differentiation(ApplicationWindow *parent,
                                 AxisRect2D *axisrect)
    : Filter(parent, axisrect) {
  init();
}

Differentiation::Differentiation(ApplicationWindow *parent,
                                 AxisRect2D *axisrect,
                                 PlotData::AssociatedData *associateddata)
    : Filter(parent, axisrect) {
  init();
  setDataFromCurve(associateddata);
}

Differentiation::Differentiation(ApplicationWindow *parent,
                                 AxisRect2D *axisrect,
                                 PlotData::AssociatedData *associateddata,
                                 double start, double end)
    : Filter(parent, axisrect) {
  init();
  setDataFromCurve(associateddata, start, end);
}

void Differentiation::init() {
  setObjectName(tr("Derivative"));
  d_min_points = 4;
}

void Differentiation::output() {
  Column *xcol = new Column(tr("1", "differention table x column name"),
                            AlphaPlot::Numeric);
  Column *ycol = new Column(tr("2", "differention table y column name"),
                            AlphaPlot::Numeric);
  xcol->setPlotDesignation(AlphaPlot::X);
  ycol->setPlotDesignation(AlphaPlot::Y);
  for (int i = 1; i < d_n - 1; i++) {
    xcol->setValueAt(i - 1, d_x[i]);
    ycol->setValueAt(i - 1,
                     0.5 * ((d_y[i + 1] - d_y[i]) / (d_x[i + 1] - d_x[i]) +
                            (d_y[i] - d_y[i - 1]) / (d_x[i] - d_x[i - 1])));
  }

  QString tableName = app_->generateUniqueName(objectName());
  QString curveTitle = associateddata_->table->name() + "_" +
                       associateddata_->xcol->name() + "_" +
                       associateddata_->ycol->name();
  Table *table = app_->newHiddenTable(
      tableName,
      tr("Derivative") + " " + tr("of", "Derivative of") + " " + curveTitle,
      QList<Column *>() << xcol << ycol);
  Layout2D *layout = app_->newGraph2D();

  if (!layout) return;
  if (!table) return;

  layout->generateCurve2DPlot(AxisRect2D::LineScatterType::Line2D, table, xcol,
                              QList<Column *>() << ycol, 0,
                              table->rowCnt() - 1);
}
