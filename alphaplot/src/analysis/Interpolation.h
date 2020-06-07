/*This file is part of AlphaPlot.

   Copyright 2016 - 2020, Arun Narayanankutty <n.arun.lifescience@gmail.com>
   Copyright 2006 - 2007, Ion Vasilief <ion_vasilief@yahoo.fr>

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

   Description : Numerical interpolation of data sets*/

#ifndef INTERPOLATION_H
#define INTERPOLATION_H

#include "Filter.h"

class AxisRect2D;
class Interpolation : public Filter {
  Q_OBJECT

 public:
  enum class InterpolationMethod : int {
    Linear = 0,
    Polynomial = 1,
    Cubic = 2,
    CubicPeriodic = 3,
    Akima = 4,
    AkimaPeriodic = 5,
    Steffen = 6
  };

  Interpolation(ApplicationWindow *parent, AxisRect2D *axisrect,
                PlotData::AssociatedData *associateddata,
                const InterpolationMethod &method);
  Interpolation(ApplicationWindow *parent, AxisRect2D *axisrect,
                PlotData::AssociatedData *associateddata, double start,
                double end, const InterpolationMethod &method);

  InterpolationMethod method() const { return d_method; }
  void setMethod(const InterpolationMethod &method);

 protected:
  virtual bool isDataAcceptable();

 private:
  void init(const InterpolationMethod &method);
  void calculateOutputData(double *x, double *y);

  //! the interpolation method
  InterpolationMethod d_method;
};

#endif  // INTERPOLATION_H
