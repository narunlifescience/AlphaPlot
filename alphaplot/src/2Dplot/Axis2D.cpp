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
#include "AxisRect2D.h"
#include "future/lib/XmlStreamReader.h"
#include "future/lib/XmlStreamWriter.h"

Axis2D::Axis2D(AxisRect2D *parent, AxisType type, TickerType tickertype)
    : QCPAxis(parent, type),
      axisrect_(parent),
      tickertype_(tickertype),
      ticker_(QSharedPointer<QCPAxisTicker>(new QCPAxisTicker)),
      layername_(axisrect_->getParentPlot2D()->getAxis2DLayerName()) {
  setLayer(layername_);
  switch (tickertype) {
    case Axis2D::TickerType::Value:
      ticker_ = ticker();
      break;
    case Axis2D::TickerType::Log:
      ticker_ = QSharedPointer<QCPAxisTicker>(new QCPAxisTickerLog);
      break;
    case Axis2D::TickerType::Pi:
      ticker_ = QSharedPointer<QCPAxisTicker>(new QCPAxisTickerPi);
      break;
    case Axis2D::TickerType::Text:
      ticker_ = QSharedPointer<QCPAxisTicker>(new QCPAxisTickerText);
      break;
    case Axis2D::TickerType::Time:
      ticker_ = QSharedPointer<QCPAxisTicker>(new QCPAxisTickerTime);
      qSharedPointerCast<QCPAxisTickerDateTime>(ticker_)->setDateTimeFormat(
          "d. MMMM\nyyyy");
      break;
    case Axis2D::TickerType::DateTime:
      ticker_ = QSharedPointer<QCPAxisTicker>(new QCPAxisTickerDateTime);
      break;
  }
  setTicker(ticker_);
  layer()->setMode(QCPLayer::LayerMode::lmBuffered);
}

Axis2D::~Axis2D() {}

AxisRect2D *Axis2D::getaxisrect_axis() const { return axisrect_; }

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

Axis2D::TickerType Axis2D::gettickertype_axis() const { return tickertype_; }

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

Axis2D::AxisLabelFormat Axis2D::getticklabelformat_axis() const {
  const QString format = numberFormat();
  Axis2D::AxisLabelFormat axisformat;
  if (format == "e") {
    axisformat = AxisLabelFormat::e;
  } else if (format == "eb") {
    axisformat = AxisLabelFormat::eb;
  } else if (format == "ebc") {
    axisformat = AxisLabelFormat::ebc;
  } else if (format == "E") {
    axisformat = AxisLabelFormat::E;
  } else if (format == "f") {
    axisformat = AxisLabelFormat::f;
  } else if (format == "g") {
    axisformat = AxisLabelFormat::g;
  } else if (format == "gb") {
    axisformat = AxisLabelFormat::gb;
  } else if (format == "gbc") {
    axisformat = AxisLabelFormat::gbc;
  } else if (format == "G") {
    axisformat = AxisLabelFormat::G;
  } else {
    qDebug() << "unknown Axis2D::AxisLabelFormat: " << format;
    axisformat = AxisLabelFormat::gbc;
  }
  return axisformat;
}

int Axis2D::getticklabelprecision_axis() const { return numberPrecision(); }

QSharedPointer<QCPAxisTicker> Axis2D::getticker_axis() { return ticker_; }

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

void Axis2D::setticklabelformat_axis(
    const Axis2D::AxisLabelFormat &axisformat) {
  switch (axisformat) {
    case Axis2D::AxisLabelFormat::e:
      setNumberFormat("e");
      break;
    case Axis2D::AxisLabelFormat::eb:
      setNumberFormat("eb");
      break;
    case Axis2D::AxisLabelFormat::ebc:
      setNumberFormat("ebc");
      break;
    case Axis2D::AxisLabelFormat::E:
      setNumberFormat("E");
      break;
    case Axis2D::AxisLabelFormat::f:
      setNumberFormat("f");
      break;
    case Axis2D::AxisLabelFormat::g:
      setNumberFormat("g");
      break;
    case Axis2D::AxisLabelFormat::gb:
      setNumberFormat("gb");
      break;
    case Axis2D::AxisLabelFormat::gbc:
      setNumberFormat("gbc");
      break;
    case Axis2D::AxisLabelFormat::G:
      setNumberFormat("G");
      break;
  }
}

void Axis2D::setticklabelprecision_axis(const int value) {
  setNumberPrecision(value);
}

void Axis2D::save(XmlStreamWriter *xmlwriter) {
  xmlwriter->writeStartElement("axis");
  switch (getorientation_axis()) {
    case Axis2D::AxisOreantation::Left:
      xmlwriter->writeAttribute("position", "left");
      break;
    case Axis2D::AxisOreantation::Bottom:
      xmlwriter->writeAttribute("position", "bottom");
      break;
    case Axis2D::AxisOreantation::Right:
      xmlwriter->writeAttribute("position", "right");
      break;
    case Axis2D::AxisOreantation::Top:
      xmlwriter->writeAttribute("position", "top");
      break;
  }
  switch (gettickertype_axis()) {
    case Axis2D::TickerType::Value:
      xmlwriter->writeAttribute("tickertype", "value");
      break;
    case Axis2D::TickerType::Log:
      xmlwriter->writeAttribute("tickertype", "log");
      break;
    case Axis2D::TickerType::Pi:
      xmlwriter->writeAttribute("tickertype", "pi");
      break;
    case Axis2D::TickerType::Time:
      xmlwriter->writeAttribute("tickertype", "time");
      break;
    case Axis2D::TickerType::DateTime:
      xmlwriter->writeAttribute("tickertype", "datetime");
      break;
    case Axis2D::TickerType::Text:
      xmlwriter->writeAttribute("tickertype", "text");
      break;
  }
  (getshowhide_axis()) ? xmlwriter->writeAttribute("visible", "true")
                       : xmlwriter->writeAttribute("visible", "false");
  xmlwriter->writeAttribute("offset", QString::number(getoffset_axis()));
  xmlwriter->writeAttribute("from", QString::number(getfrom_axis()));
  xmlwriter->writeAttribute("to", QString::number(getto_axis()));
  switch (getscaletype_axis()) {
    case Axis2D::AxisScaleType::Linear:
      xmlwriter->writeAttribute("scaletype", "linear");
      break;
    case Axis2D::AxisScaleType::Logarithmic:
      xmlwriter->writeAttribute("scaletype", "logarithemic");
      break;
  }
  (getinverted_axis()) ? xmlwriter->writeAttribute("inverted", "true")
                       : xmlwriter->writeAttribute("inverted", "false");
  xmlwriter->writeStartElement("label");
  xmlwriter->writeAttribute("text", getlabeltext_axis());
  xmlwriter->writeAttribute("padding", QString::number(getlabelpadding_axis()));
  xmlwriter->writeFont(getlabelfont_axis(), getlabelcolor_axis());
  xmlwriter->writeEndElement();
  // Ticks
  xmlwriter->writeStartElement("ticks");
  xmlwriter->writeAttribute("in", QString::number(getticklengthin_axis()));
  xmlwriter->writeAttribute("out", QString::number(getticklengthout_axis()));
  xmlwriter->writePen(tickPen());
  xmlwriter->writeEndElement();
  // Subticks
  xmlwriter->writeStartElement("subticks");
  xmlwriter->writeAttribute("in", QString::number(getsubticklengthin_axis()));
  xmlwriter->writeAttribute("out", QString::number(getsubticklengthout_axis()));
  xmlwriter->writePen(subTickPen());
  xmlwriter->writeEndElement();
  // Tick Labels
  xmlwriter->writeStartElement("ticklabels");
  xmlwriter->writeFont(getticklabelfont_axis(), getticklabelcolor_axis());
  xmlwriter->writeAttribute("padding",
                            QString::number(getticklabelpadding_axis()));
  xmlwriter->writeAttribute("rotation",
                            QString::number(getticklabelrotation_axis()));
  switch (getticklabelside_axis()) {
    case Axis2D::AxisLabelSide::Inside:
      xmlwriter->writeAttribute("side", "up");
      break;
    case Axis2D::AxisLabelSide::Outside:
      xmlwriter->writeAttribute("side", "down");
      break;
  }
  switch (getticklabelformat_axis()) {
    case Axis2D::AxisLabelFormat::E:
      xmlwriter->writeAttribute("format", "E");
      break;
    case Axis2D::AxisLabelFormat::G:
      xmlwriter->writeAttribute("format", "G");
      break;
    case Axis2D::AxisLabelFormat::e:
      xmlwriter->writeAttribute("format", "e");
      break;
    case Axis2D::AxisLabelFormat::eb:
      xmlwriter->writeAttribute("format", "eb");
      break;
    case Axis2D::AxisLabelFormat::ebc:
      xmlwriter->writeAttribute("format", "ebc");
      break;
    case Axis2D::AxisLabelFormat::f:
      xmlwriter->writeAttribute("format", "f");
      break;
    case Axis2D::AxisLabelFormat::g:
      xmlwriter->writeAttribute("format", "g");
      break;
    case Axis2D::AxisLabelFormat::gb:
      xmlwriter->writeAttribute("format", "gb");
      break;
    case Axis2D::AxisLabelFormat::gbc:
      xmlwriter->writeAttribute("format", "gbc");
      break;
  }
  xmlwriter->writeAttribute("precision",
                            QString::number(getticklabelprecision_axis()));
  xmlwriter->writeEndElement();
  xmlwriter->writeEndElement();
}

Axis2D *Axis2D::load(XmlStreamReader *xmlreader, AxisRect2D *axisrect) {
  Axis2D *axis = nullptr;
  if (xmlreader->isStartElement() && xmlreader->name() == "axis") {
    bool ok;
    AxisType type = AxisType::atLeft;
    TickerType tickertype = TickerType::Value;
    // Type property
    QString position = xmlreader->readAttributeString("position", &ok);
    if (ok) {
      if (position == "left")
        type = AxisType::atLeft;
      else if (position == "bottom")
        type = AxisType::atBottom;
      else if (position == "right")
        type = AxisType::atRight;
      else if (position == "top")
        type = AxisType::atTop;
      else {
        qDebug() << "(critical) Axis2D Position property setting error";
        return axis;
      }
    } else
      return axis;
    // Tickertype property
    QString ticker = xmlreader->readAttributeString("tickertype", &ok);
    if (ok) {
      if (ticker == "value")
        tickertype = TickerType::Value;
      else if (ticker == "log")
        tickertype = TickerType::Log;
      else if (ticker == "pi")
        tickertype = TickerType::Pi;
      else if (ticker == "time")
        tickertype = TickerType::Time;
      else if (ticker == "datetime")
        tickertype = TickerType::DateTime;
      else if (ticker == "text")
        tickertype = TickerType::Text;
      else {
        qDebug() << "(critical) Axis2D Tickertype property setting error";
        return axis;
      }
    } else
      return axis;
    axis = new Axis2D(axisrect, type, tickertype);
    // visible property
    bool visible = xmlreader->readAttributeBool("visible", &ok);
    if (ok)
      axis->setshowhide_axis(visible);
    else
      qDebug() << "Axis2D visible property setting error";
    // offset property
    int offset = xmlreader->readAttributeInt("offset", &ok);
    if (ok)
      axis->setoffset_axis(offset);
    else
      qDebug() << "Axis2D offset property setting error";
    // from property
    double from = xmlreader->readAttributeDouble("from", &ok);
    if (ok)
      axis->setfrom_axis(from);
    else
      qDebug() << "Axis2D from property setting error";
    // to property
    double to = xmlreader->readAttributeDouble("to", &ok);
    if (ok)
      axis->setto_axis(to);
    else
      qDebug() << "Axis2D to property setting error";
    // Scaletype property
    QString scaletype = xmlreader->readAttributeString("scaletype", &ok);
    if (ok) {
      if (scaletype == "linear")
        axis->setscaletype_axis(AxisScaleType::Linear);
      else if (scaletype == "bottom")
        axis->setscaletype_axis(AxisScaleType::Logarithmic);
      else
        qDebug() << "Axis2D Scaletype property setting error";
    } else
      qDebug() << "Axis2D Scaletype property setting error";
    // inverted property
    bool inverted = xmlreader->readAttributeBool("inverted", &ok);
    if (ok)
      axis->setinverted_axis(inverted);
    else
      qDebug() << "Axis2D inverted property setting error";
    // Loop through sub elements
    while (!xmlreader->atEnd()) {
      if (xmlreader->isStartElement() && xmlreader->name() == "axis") {
      }
      if (xmlreader->isEndElement() && xmlreader->name() == "axis") break;
      xmlreader->readNext();
    }
  } else {  // no element
    xmlreader->raiseError(tr("no axis element found"));
    return axis;
  }
  return axis;
}
