/***************************************************************************
    File                 : PolynomialFit.h
    Project              : AlphaPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Benkert
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : Polynomial Fit and Linear Fit classes

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
#ifndef POLYNOMIALFIT_H
#define POLYNOMIALFIT_H

#include "Fit.h"

class PolynomialFit : public Fit {
  Q_OBJECT

 public:
  PolynomialFit(ApplicationWindow *parent, AxisRect2D *axisrect, int order = 2);
  PolynomialFit(ApplicationWindow *parent, AxisRect2D *axisrect,
                PlotData::AssociatedData *associateddata, int order = 2);
  PolynomialFit(ApplicationWindow *parent, AxisRect2D *axisrect,
                PlotData::AssociatedData *associateddata, double start,
                double end, int order = 2);

  QString legendInfo() override;
  void fit() override;

  static QString generateFormula(int order);
  static QStringList generateParameterList(int order);

 private:
  void init();
  void calculateFitCurveData(const std::vector<double> &par, double *X,
                             double *Y) override;

  int d_order;
};

class LinearFit : public Fit {
  Q_OBJECT

 public:
  LinearFit(ApplicationWindow *parent, AxisRect2D *axisrect);
  LinearFit(ApplicationWindow *parent, AxisRect2D *axisrect,
            PlotData::AssociatedData *associateddata);
  LinearFit(ApplicationWindow *parent, AxisRect2D *axisrect,
            PlotData::AssociatedData *associateddata, double start, double end);

  void fit() override;

 private:
  void init();
  void calculateFitCurveData(const std::vector<double> &par, double *X,
                             double *Y) override;
};

#endif  // POLYNOMIALFIT_H
