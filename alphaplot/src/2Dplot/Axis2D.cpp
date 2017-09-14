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

#include "Axis2D.h"

Axis2D::Axis2D(QCPAxisRect *parent, AxisType type) : QCPAxis(parent, type) {}

Axis2D::~Axis2D() {}

bool Axis2D::getshowhide_axis() const { return realVisibility(); }

int Axis2D::getoffset_axis() const { return offset(); }

double Axis2D::getfrom_axis() const { return range().lower; }

double Axis2D::getto_axis() const { return range().upper; }

Axis2D::AxisScaleType Axis2D::getscaletype_axis() {
  AxisScaleType scaletype;
  switch (scaleType()) {
    case QCPAxis::stLinear:
      scaletype = AxisScaleType::Linear;
      break;
    case QCPAxis::stLogarithmic:
      scaletype = AxisScaleType::Logarithmic;
      break;
  }
  return scaletype;
}

Axis2D::AxisOreantation Axis2D::getorientation_axis() {
  AxisOreantation orientation;
  switch (axisType()) {
    case QCPAxis::atLeft:
      orientation = AxisOreantation::Left;
      break;
    case QCPAxis::atBottom:
      orientation = AxisOreantation::Bottom;
      break;
    case QCPAxis::atRight:
      orientation = AxisOreantation::Right;
      break;
    case QCPAxis::atTop:
      orientation = AxisOreantation::Top;
      break;
  }
  return orientation;
}

bool Axis2D::getinverted_axis() const { return rangeReversed(); }

QColor Axis2D::getstrokecolor_axis() const { return basePen().color(); }

double Axis2D::getstrokethickness_axis() const { return basePen().widthF(); }

Qt::PenStyle Axis2D::getstroketype_axis() const { return basePen().style(); }

bool Axis2D::getantialiased_axis() const { return antialiased(); }

QString Axis2D::getlabeltext_axis() const { return label(); }

QColor Axis2D::getlabelcolor_axis() const { return labelColor(); }

QFont Axis2D::getlabelfont_axis() const { return labelFont(); }

int Axis2D::getlabelpadding_axis() const { return labelPadding(); }

bool Axis2D::gettickvisibility_axis() const { return ticks(); }

int Axis2D::getticklengthin_axis() const { return tickLengthIn(); }

int Axis2D::getticklengthout_axis() const { return tickLengthOut(); }

QColor Axis2D::gettickstrokecolor_axis() const { return tickPen().color(); }

double Axis2D::gettickstrokethickness_axis() const {
  return tickPen().widthF();
}

Qt::PenStyle Axis2D::gettickstrokestyle_axis() const {
  return tickPen().style();
}

bool Axis2D::getsubtickvisibility_axis() const { return subTicks(); }

int Axis2D::getsubticklengthin_axis() const { return subTickLengthIn(); }

int Axis2D::getsubticklengthout_axis() const { return subTickLengthOut(); }

QColor Axis2D::getsubtickstrokecolor_axis() const {
  return subTickPen().color();
}

double Axis2D::getsubtickstrokethickness_axis() const {
  return subTickPen().widthF();
}

Qt::PenStyle Axis2D::getsubtickstrokestyle_axis() const {
  return subTickPen().style();
}

bool Axis2D::getticklabelvisibility_axis() const { return tickLabels(); }

int Axis2D::getticklabelpadding_axis() const { return tickLabelPadding(); }

QFont Axis2D::getticklabelfont_axis() const { return tickLabelFont(); }

QColor Axis2D::getticklabelcolor_axis() const { return tickLabelColor(); }

double Axis2D::getticklabelrotation_axis() const { return tickLabelRotation(); }

Axis2D::AxisLabelSide Axis2D::getticklabelside_axis() const {
  AxisLabelSide side;
  switch (tickLabelSide()) {
    case lsInside:
      side = AxisLabelSide::Inside;
      break;
    case lsOutside:
      side = AxisLabelSide::Outside;
      break;
  }
  return side;
}

int Axis2D::getticklabelprecision_axis() const { return numberPrecision(); }

void Axis2D::setshowhide_axis(const bool value) { setVisible(value); }

void Axis2D::setoffset_axis(const int value) { setOffset(value); }

void Axis2D::setfrom_axis(const double value) { setRangeLower(value); }

void Axis2D::setto_axis(const double value) { setRangeUpper(value); }

void Axis2D::setscaletype_axis(const Axis2D::AxisScaleType &type) {
  switch (type) {
    case AxisScaleType::Linear:
      setScaleType(QCPAxis::stLinear);
      break;
    case AxisScaleType::Logarithmic:
      setScaleType(QCPAxis::stLogarithmic);
      break;
  }
}

void Axis2D::setinverted_axis(const bool value) { setRangeReversed(value); }

void Axis2D::setstrokecolor_axis(const QColor &color) {
  QPen pen = basePen();
  pen.setColor(color);
  setBasePen(pen);
}

void Axis2D::setstrokethickness_axis(const double value) {
  QPen pen = basePen();
  pen.setWidthF(value);
  setBasePen(pen);
}

void Axis2D::setstroketype_axis(const Qt::PenStyle &style) {
  QPen pen = basePen();
  pen.setStyle(style);
  setBasePen(pen);
}

void Axis2D::setantialiased_axis(const bool value) { setAntialiased(value); }

void Axis2D::setlabeltext_axis(const QString value) { setLabel(value); }

void Axis2D::setlabelcolor_axis(const QColor &color) { setLabelColor(color); }

void Axis2D::setlabelfont_axis(const QFont &font) { setLabelFont(font); }

void Axis2D::setlabelpadding_axis(const int value) { setLabelPadding(value); }

void Axis2D::settickvisibility_axis(const bool value) { setTicks(value); }

void Axis2D::setticklengthin_axis(const int value) { setTickLengthIn(value); }

void Axis2D::setticklengthout_axis(const int value) { setTickLengthOut(value); }

void Axis2D::settickstrokecolor_axis(const QColor &color) {
  QPen pen = tickPen();
  pen.setColor(color);
  setTickPen(pen);
}

void Axis2D::settickstrokethickness_axis(const double value) {
  QPen pen = tickPen();
  pen.setWidthF(value);
  setTickPen(pen);
}

void Axis2D::settickstrokestyle_axis(const Qt::PenStyle &style) {
  QPen pen = tickPen();
  pen.setStyle(style);
  setTickPen(pen);
}

void Axis2D::setsubtickvisibility_axis(const bool value) { setSubTicks(value); }

void Axis2D::setsubticklengthin_axis(const int value) {
  setSubTickLengthIn(value);
}

void Axis2D::setsubticklengthout_axis(const int value) {
  setSubTickLengthOut(value);
}

void Axis2D::setsubtickstrokecolor_axis(const QColor &color) {
  QPen pen = subTickPen();
  pen.setColor(color);
  setSubTickPen(pen);
}

void Axis2D::setsubtickstrokethickness_axis(const double value) {
  QPen pen = subTickPen();
  pen.setWidthF(value);
  setSubTickPen(pen);
}

void Axis2D::setsubtickstrokestyle_axis(const Qt::PenStyle &style) {
  QPen pen = subTickPen();
  pen.setStyle(style);
  setSubTickPen(pen);
}

void Axis2D::setticklabelvisibility_axis(const bool value) {
  setTickLabels(value);
}

void Axis2D::setticklabelpadding_axis(const int value) {
  setTickLabelPadding(value);
}

void Axis2D::setticklabelfont_axis(const QFont &font) {
  setTickLabelFont(font);
}

void Axis2D::setticklabelcolor_axis(const QColor &color) {
  setTickLabelColor(color);
}

void Axis2D::setticklabelrotation_axis(const double value) {
  setTickLabelRotation(value);
}

void Axis2D::setticklabelside_axis(const Axis2D::AxisLabelSide &side) {
  switch (side) {
    case AxisLabelSide::Inside:
      setTickLabelSide(LabelSide::lsInside);
      break;
    case AxisLabelSide::Outside:
      setTickLabelSide(LabelSide::lsOutside);
      break;
  }
}

void Axis2D::setticklabelprecision_axis(const int value) {
  setNumberPrecision(value);
}
