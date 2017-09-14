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

   Description : Plot2D axis related stuff */

#ifndef AXIS2D_H
#define AXIS2D_H

#include "../3rdparty/qcustomplot/qcustomplot.h"
#include "Properties2D.h"

class Axis2D : public QCPAxis {
  Q_OBJECT
 public:
  Axis2D(QCPAxisRect *parent, AxisType type);
  ~Axis2D();

  enum class TickOrientation { Inward, Outward, Bothwards, None };
  enum class AxisOreantation { Left = 0, Bottom = 1, Right = 2, Top = 3 };
  enum class AxisScaleType { Linear = 0, Logarithmic = 1 };
  enum class AxisLabelSide { Inside = 0, Outside = 1 };

  // getter
  // Axis properties
  bool getshowhide_axis() const;
  int getoffset_axis() const;
  double getfrom_axis() const;
  double getto_axis() const;
  Axis2D::AxisScaleType getscaletype_axis();
  AxisOreantation getorientation_axis();
  bool getinverted_axis() const;
  QColor getstrokecolor_axis() const;
  double getstrokethickness_axis() const;
  Qt::PenStyle getstroketype_axis() const;
  bool getantialiased_axis() const;
  // Axis label properties
  QString getlabeltext_axis() const;
  QColor getlabelcolor_axis() const;
  QFont getlabelfont_axis() const;
  int getlabelpadding_axis() const;
  // Tick properties
  bool gettickvisibility_axis() const;
  int getticklengthin_axis() const;
  int getticklengthout_axis() const;
  QColor gettickstrokecolor_axis() const;
  double gettickstrokethickness_axis() const;
  Qt::PenStyle gettickstrokestyle_axis() const;
  // Sub-tick properties
  bool getsubtickvisibility_axis() const;
  int getsubticklengthin_axis() const;
  int getsubticklengthout_axis() const;
  QColor getsubtickstrokecolor_axis() const;
  double getsubtickstrokethickness_axis() const;
  Qt::PenStyle getsubtickstrokestyle_axis() const;
  // Tick label properties
  bool getticklabelvisibility_axis() const;
  int getticklabelpadding_axis() const;
  QFont getticklabelfont_axis() const;
  QColor getticklabelcolor_axis() const;
  double getticklabelrotation_axis() const;
  AxisLabelSide getticklabelside_axis() const;
  int getticklabelprecision_axis() const;

  // setters
  // Axis properties
  void setshowhide_axis(const bool value);
  void setoffset_axis(const int value);
  void setfrom_axis(const double value);
  void setto_axis(const double value);
  void setscaletype_axis(const Axis2D::AxisScaleType &type);
  void setinverted_axis(const bool value);
  void setstrokecolor_axis(const QColor &color);
  void setstrokethickness_axis(const double value);
  void setstroketype_axis(const Qt::PenStyle &style);
  void setantialiased_axis(const bool value);
  // Axis label properties
  void setlabeltext_axis(const QString value);
  void setlabelcolor_axis(const QColor &color);
  void setlabelfont_axis(const QFont &font);
  void setlabelpadding_axis(const int value);
  // Tick properties
  void settickvisibility_axis(const bool value);
  void setticklengthin_axis(const int value);
  void setticklengthout_axis(const int value);
  void settickstrokecolor_axis(const QColor &color);
  void settickstrokethickness_axis(const double value);
  void settickstrokestyle_axis(const Qt::PenStyle &style);
  // Sub-tick properties
  void setsubtickvisibility_axis(const bool value);
  void setsubticklengthin_axis(const int value);
  void setsubticklengthout_axis(const int value);
  void setsubtickstrokecolor_axis(const QColor &color);
  void setsubtickstrokethickness_axis(const double value);
  void setsubtickstrokestyle_axis(const Qt::PenStyle &style);
  // Tick label properties
  void setticklabelvisibility_axis(const bool value);
  void setticklabelpadding_axis(const int value);
  void setticklabelfont_axis(const QFont &font);
  void setticklabelcolor_axis(const QColor &color);
  void setticklabelrotation_axis(const double value);
  void setticklabelside_axis(const AxisLabelSide &side);
  void setticklabelprecision_axis(const int value);
};

#endif  // AXIS2D_H
