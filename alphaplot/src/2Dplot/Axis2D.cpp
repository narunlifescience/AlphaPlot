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

#include <QVector>

#include "AxisRect2D.h"
#include "Plot2D.h"
#include "core/Utilities.h"
#include "future/core/column/Column.h"
#include "future/lib/XmlStreamReader.h"
#include "future/lib/XmlStreamWriter.h"

Axis2D::Axis2D(AxisRect2D *parent, const AxisType type,
               const TickerType tickertype)
    : QCPAxis(parent, type),
      axisrect_(parent),
      tickertype_(tickertype),
      ticker_(QSharedPointer<QCPAxisTicker>(new QCPAxisTicker)),
      layername_(axisrect_->getParentPlot2D()->getAxis2DLayerName()),
      tickertext_(new QVector<QString>()) {
  setLayer(layername_);
  switch (tickertype) {
    case Axis2D::TickerType::Value:
      ticker_ = ticker();
      break;
    case Axis2D::TickerType::Log:
      setscaletype_axis(Axis2D::AxisScaleType::Logarithmic);
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

Axis2D::AxisScaleType Axis2D::getscaletype_axis() const {
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

Axis2D::AxisOreantation Axis2D::getorientation_axis() const {
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

int Axis2D::gettickscount_axis() const { return ticker_->tickCount(); }

double Axis2D::getticksorigin() const { return ticker_->tickOrigin(); }

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

QString Axis2D::getname_axis() const {
  QString name;
  name = label();
  name = name.remove("\n");
  if (name.length() > 15) {
    name.truncate(15);
    name = name + "***";
  }
  switch (getorientation_axis()) {
    case Axis2D::AxisOreantation::Left:
    case Axis2D::AxisOreantation::Right:
      name = name + " (Y):";
      break;
    case Axis2D::AxisOreantation::Top:
    case Axis2D::AxisOreantation::Bottom:
      name = name + " (X):";
      break;
  }
  return name;
}

uint Axis2D::getnumber_axis() const {
  switch (getorientation_axis()) {
    case Axis2D::AxisOreantation::Left:
    case Axis2D::AxisOreantation::Right: {
      QList<Axis2D *> yaxes = getaxisrect_axis()->getYAxes2D();
      for (int i = 0; i < yaxes.size(); i++) {
        if (yaxes.at(i) == this) return i + 1;
      }
    } break;
    case Axis2D::AxisOreantation::Top:
    case Axis2D::AxisOreantation::Bottom: {
      QList<Axis2D *> xaxes = getaxisrect_axis()->getXAxes2D();
      for (int i = 0; i < xaxes.size(); i++) {
        if (xaxes.at(i) == this) return i + 1;
      }
    } break;
  }

  return 0;
}

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

void Axis2D::settickscount_axis(const int count) {
  ticker_->setTickCount(count);
  setTicker(ticker_);
}

void Axis2D::setticksorigin(const double value) {
  ticker_->setTickOrigin(value);
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

void Axis2D::settickertext(Column *col, const int from, const int to) {
  QSharedPointer<QCPAxisTickerText> textticker =
      qSharedPointerCast<QCPAxisTickerText>(ticker_);
  for (int i = 0, row = from; row <= to; row++, i++) {
    textticker->addTick(i, Utilities::splitstring(col->textAt(row)));
    tickertext_->append(col->textAt(row));
  }
}

void Axis2D::save(XmlStreamWriter *xmlwriter) {
  xmlwriter->writeStartElement("axis");
  (getshowhide_axis()) ? xmlwriter->writeAttribute("visible", "true")
                       : xmlwriter->writeAttribute("visible", "false");
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
  (axisrect_->getGridPair().first.second == this ||
   axisrect_->getGridPair().second.second == this)
      ? xmlwriter->writeAttribute("grid", "true")
      : xmlwriter->writeAttribute("grid", "false");
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
  // upperending style
  switch (upperEnding().style()) {
    case QCPLineEnding::EndingStyle::esNone:
      xmlwriter->writeAttribute("upperending", "none");
      break;
    case QCPLineEnding::EndingStyle::esBar:
      xmlwriter->writeAttribute("upperending", "bar");
      break;
    case QCPLineEnding::EndingStyle::esDisc:
      xmlwriter->writeAttribute("upperending", "disc");
      break;
    case QCPLineEnding::EndingStyle::esSquare:
      xmlwriter->writeAttribute("upperending", "square");
      break;
    case QCPLineEnding::EndingStyle::esDiamond:
      xmlwriter->writeAttribute("upperending", "diamond");
      break;
    case QCPLineEnding::EndingStyle::esHalfBar:
      xmlwriter->writeAttribute("upperending", "halfbar");
      break;
    case QCPLineEnding::EndingStyle::esFlatArrow:
      xmlwriter->writeAttribute("upperending", "flatarrow");
      break;
    case QCPLineEnding::EndingStyle::esLineArrow:
      xmlwriter->writeAttribute("upperending", "linearrow");
      break;
    case QCPLineEnding::EndingStyle::esSkewedBar:
      xmlwriter->writeAttribute("upperending", "skewedbar");
      break;
    case QCPLineEnding::EndingStyle::esSpikeArrow:
      xmlwriter->writeAttribute("upperending", "spikearrow");
      break;
  }
  // lowerending style
  switch (lowerEnding().style()) {
    case QCPLineEnding::EndingStyle::esNone:
      xmlwriter->writeAttribute("lowerending", "none");
      break;
    case QCPLineEnding::EndingStyle::esBar:
      xmlwriter->writeAttribute("lowerending", "bar");
      break;
    case QCPLineEnding::EndingStyle::esDisc:
      xmlwriter->writeAttribute("lowerending", "disc");
      break;
    case QCPLineEnding::EndingStyle::esSquare:
      xmlwriter->writeAttribute("lowerending", "square");
      break;
    case QCPLineEnding::EndingStyle::esDiamond:
      xmlwriter->writeAttribute("lowerending", "diamond");
      break;
    case QCPLineEnding::EndingStyle::esHalfBar:
      xmlwriter->writeAttribute("lowerending", "halfbar");
      break;
    case QCPLineEnding::EndingStyle::esFlatArrow:
      xmlwriter->writeAttribute("lowerending", "flatarrow");
      break;
    case QCPLineEnding::EndingStyle::esLineArrow:
      xmlwriter->writeAttribute("lowerending", "linearrow");
      break;
    case QCPLineEnding::EndingStyle::esSkewedBar:
      xmlwriter->writeAttribute("lowerending", "skewedbar");
      break;
    case QCPLineEnding::EndingStyle::esSpikeArrow:
      xmlwriter->writeAttribute("lowerending", "spikearrow");
      break;
  }
  (getinverted_axis()) ? xmlwriter->writeAttribute("inverted", "true")
                       : xmlwriter->writeAttribute("inverted", "false");
  (getantialiased_axis()) ? xmlwriter->writeAttribute("antialias", "true")
                          : xmlwriter->writeAttribute("antialias", "false");
  xmlwriter->writePen(basePen());
  xmlwriter->writeStartElement("ticker");
  switch (tickertype_) {
    case Axis2D::TickerType::Pi:
      xmlwriter->writeAttribute("type", "symbol");
      break;
    case Axis2D::TickerType::Value:
      xmlwriter->writeAttribute("type", "value");
      break;
    case Axis2D::TickerType::Log:
      xmlwriter->writeAttribute("type", "log");
      break;
    case Axis2D::TickerType::Time:
      xmlwriter->writeAttribute("type", "time");
      break;
    case Axis2D::TickerType::DateTime: {
      xmlwriter->writeAttribute("type", "datetime");
      xmlwriter->writeAttribute(
          "format",
          ticker_.staticCast<QCPAxisTickerDateTime>()->dateTimeFormat());
    } break;
    case Axis2D::TickerType::Text: {
      xmlwriter->writeAttribute("type", "text");
      for (int i = 0; i < tickertext_->count(); i++) {
        xmlwriter->writeStartElement("tick");
        xmlwriter->writeAttribute("value",
                                  Utilities::joinstring(tickertext_->at(i)));
        xmlwriter->writeEndElement();
      }
    } break;
  }
  xmlwriter->writeEndElement();
  xmlwriter->writeStartElement("label");
  xmlwriter->writeAttribute("text", getlabeltext_axis());
  xmlwriter->writeAttribute("padding", QString::number(getlabelpadding_axis()));
  xmlwriter->writeFont(getlabelfont_axis(), getlabelcolor_axis());
  xmlwriter->writeEndElement();
  // Ticks
  xmlwriter->writeStartElement("ticks");
  (gettickvisibility_axis()) ? xmlwriter->writeAttribute("visible", "true")
                             : xmlwriter->writeAttribute("visible", "false");
  xmlwriter->writeAttribute("count", QString::number(gettickscount_axis()));
  xmlwriter->writeAttribute("origin", QString::number(getticksorigin()));
  xmlwriter->writeAttribute("in", QString::number(getticklengthin_axis()));
  xmlwriter->writeAttribute("out", QString::number(getticklengthout_axis()));
  xmlwriter->writePen(tickPen());
  xmlwriter->writeEndElement();
  // Subticks
  xmlwriter->writeStartElement("subticks");
  (getsubtickvisibility_axis()) ? xmlwriter->writeAttribute("visible", "true")
                                : xmlwriter->writeAttribute("visible", "false");
  xmlwriter->writeAttribute("in", QString::number(getsubticklengthin_axis()));
  xmlwriter->writeAttribute("out", QString::number(getsubticklengthout_axis()));
  xmlwriter->writePen(subTickPen());
  xmlwriter->writeEndElement();
  // Tick Labels
  xmlwriter->writeStartElement("ticklabels");
  (getticklabelvisibility_axis())
      ? xmlwriter->writeAttribute("visible", "true")
      : xmlwriter->writeAttribute("visible", "false");
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
  xmlwriter->writeFont(getticklabelfont_axis(), getticklabelcolor_axis());
  xmlwriter->writeEndElement();
  xmlwriter->writeEndElement();
}

bool Axis2D::load(XmlStreamReader *xmlreader) {
  if (xmlreader->isStartElement() && xmlreader->name() == "axis") {
    bool ok;
    // visible property
    bool visible = xmlreader->readAttributeBool("visible", &ok);
    (ok) ? setshowhide_axis(visible)
         : xmlreader->raiseWarning(tr("Axis2D visible property setting error"));
    // offset property
    int offset = xmlreader->readAttributeInt("offset", &ok);
    (ok) ? setoffset_axis(offset)
         : xmlreader->raiseWarning(tr("Axis2D offset property setting error"));
    // from property
    double from = xmlreader->readAttributeDouble("from", &ok);
    if (ok) {
      setfrom_axis(from);
      setRangeLower(from);  // temporary fix the rescaling of axes
    } else
      xmlreader->raiseWarning(tr("Axis2D from property setting error"));
    // to property
    double to = xmlreader->readAttributeDouble("to", &ok);
    (ok) ? setto_axis(to)
         : xmlreader->raiseWarning(tr("Axis2D to property setting error"));
    // Scaletype property
    QString scaletype = xmlreader->readAttributeString("scaletype", &ok);
    if (ok) {
      (scaletype == "linear") ? setscaletype_axis(AxisScaleType::Linear)
      : (scaletype == "logarithemic")
          ? setscaletype_axis(AxisScaleType::Logarithmic)
          : xmlreader->raiseWarning(
                tr("Axis2D Scaletype property setting error"));
    } else
      xmlreader->raiseWarning(tr("Axis2D Scaletype property setting error"));

    // upperending
    QString upperending = xmlreader->readAttributeString("upperending", &ok);
    if (ok) {
      if (upperending == "none")
        setUpperEnding(QCPLineEnding::EndingStyle::esNone);
      else if (upperending == "bar")
        setUpperEnding(QCPLineEnding::EndingStyle::esBar);
      else if (upperending == "disc")
        setUpperEnding(QCPLineEnding::EndingStyle::esDisc);
      else if (upperending == "square")
        setUpperEnding(QCPLineEnding::EndingStyle::esSquare);
      else if (upperending == "diamond")
        setUpperEnding(QCPLineEnding::EndingStyle::esDiamond);
      else if (upperending == "halfbar")
        setUpperEnding(QCPLineEnding::EndingStyle::esHalfBar);
      else if (upperending == "flatarrow")
        setUpperEnding(QCPLineEnding::EndingStyle::esFlatArrow);
      else if (upperending == "linearrow")
        setUpperEnding(QCPLineEnding::EndingStyle::esLineArrow);
      else if (upperending == "skewedbar")
        setUpperEnding(QCPLineEnding::EndingStyle::esSkewedBar);
      else if (upperending == "spikearrow")
        setUpperEnding(QCPLineEnding::EndingStyle::esSpikeArrow);
      else
        xmlreader->raiseWarning(tr("Axis2D unknown upper ending property"));
    } else
      xmlreader->raiseWarning(tr("Axis2D upper ending property setting error"));

    // lowerending
    QString lowerending = xmlreader->readAttributeString("lowerending", &ok);
    if (ok) {
      if (lowerending == "none")
        setLowerEnding(QCPLineEnding::EndingStyle::esNone);
      else if (lowerending == "bar")
        setLowerEnding(QCPLineEnding::EndingStyle::esBar);
      else if (lowerending == "disc")
        setLowerEnding(QCPLineEnding::EndingStyle::esDisc);
      else if (lowerending == "square")
        setLowerEnding(QCPLineEnding::EndingStyle::esSquare);
      else if (lowerending == "diamond")
        setLowerEnding(QCPLineEnding::EndingStyle::esDiamond);
      else if (lowerending == "halfbar")
        setLowerEnding(QCPLineEnding::EndingStyle::esHalfBar);
      else if (lowerending == "flatarrow")
        setLowerEnding(QCPLineEnding::EndingStyle::esFlatArrow);
      else if (lowerending == "linearrow")
        setLowerEnding(QCPLineEnding::EndingStyle::esLineArrow);
      else if (lowerending == "skewedbar")
        setLowerEnding(QCPLineEnding::EndingStyle::esSkewedBar);
      else if (lowerending == "spikearrow")
        setLowerEnding(QCPLineEnding::EndingStyle::esSpikeArrow);
      else
        xmlreader->raiseWarning(tr("Axis2D unknown lower ending property"));
    } else
      xmlreader->raiseWarning(tr("Axis2D lower ending property setting error"));

    // inverted property
    bool inverted = xmlreader->readAttributeBool("inverted", &ok);
    (ok)
        ? setinverted_axis(inverted)
        : xmlreader->raiseWarning(tr("Axis2D inverted property setting error"));
    // antialias property
    bool antialias = xmlreader->readAttributeBool("antialias", &ok);
    if (ok)
      setantialiased_axis(antialias);
    else
      xmlreader->raiseWarning(tr("Axis2D antialias property setting error"));
    while (!xmlreader->atEnd()) {
      xmlreader->readNext();
      if (xmlreader->isEndElement() && xmlreader->name() == "pen") break;
      // pen
      if (xmlreader->isStartElement() && xmlreader->name() == "pen") {
        QPen basep = xmlreader->readPen(&ok);
        if (ok)
          setBasePen(basep);
        else
          xmlreader->raiseWarning(tr("Axis2D pen property setting error"));
      }
    }
    // Loop through sub elements
    while (!xmlreader->atEnd()) {
      if (xmlreader->isEndElement() && xmlreader->name() == "axis") break;
      // ticker
      if (xmlreader->isStartElement() && xmlreader->name() == "ticker") {
        // Label text
        QString tkrtype = xmlreader->readAttributeString("type", &ok);
        if (ok) {
          if (tkrtype == "text") {
            while (!xmlreader->atEnd()) {
              if (xmlreader->isEndElement() && xmlreader->name() == "ticker")
                break;

              if (xmlreader->isStartElement() && xmlreader->name() == "tick") {
                QString tkrtext = xmlreader->readAttributeString("value", &ok);
                (ok) ? tickertext_->append(tkrtext)
                     : xmlreader->raiseWarning(
                           tr("Axis2D tickertype text element "
                              "formatproperty setting error"));
              }
              xmlreader->readNext();
            }
          } else if (tkrtype == "datetime") {
            QString format = xmlreader->readAttributeString("format", &ok);
            (ok) ? ticker_.staticCast<QCPAxisTickerDateTime>()
                       ->setDateTimeFormat(Utilities::splitstring(format))
                 : xmlreader->raiseWarning(tr("Axis2D tickertype datetime "
                                              "formatproperty setting error"));
          }
        } else
          xmlreader->raiseWarning(
              tr("Axis2D tickertype property setting error"));
      }
      // Label element
      if (xmlreader->isStartElement() && xmlreader->name() == "label") {
        // Label text
        QString text = xmlreader->readAttributeString("text", &ok);
        if (ok)
          setlabeltext_axis(text);
        else
          xmlreader->raiseWarning(tr("Axis2D label property setting error"));
        // Label padding
        int padding = xmlreader->readAttributeInt("padding", &ok);
        if (ok)
          setlabelpadding_axis(padding);
        else
          xmlreader->raiseWarning(tr("Axis2D padding property setting error"));
        while (!xmlreader->atEnd()) {
          xmlreader->readNext();
          if (xmlreader->isEndElement() && xmlreader->name() == "label") break;
          if (xmlreader->isStartElement() && xmlreader->name() == "font") {
            QPair<QFont, QColor> fontpair = xmlreader->readFont(&ok);
            if (ok) {
              setlabelfont_axis(fontpair.first);
              setlabelcolor_axis(fontpair.second);
            } else
              xmlreader->raiseWarning(
                  tr("Axis2D font & color property setting error"));
          }
        }
      }
      // Ticks element
      if (xmlreader->isStartElement() && xmlreader->name() == "ticks") {
        // tick visible property
        bool tickvisible = xmlreader->readAttributeBool("visible", &ok);
        if (ok)
          settickvisibility_axis(tickvisible);
        else
          xmlreader->raiseWarning(
              tr("Axis2D tick visible property setting error"));
        // Tick count
        int count = xmlreader->readAttributeInt("count", &ok);
        (ok) ? settickscount_axis(count)
             : xmlreader->raiseWarning(
                   tr("Axis2D Tick count in property setting error"));
        // Tick count
        double origin = xmlreader->readAttributeInt("origin", &ok);
        (ok) ? setticksorigin(origin)
             : xmlreader->raiseWarning(
                   tr("Axis2D Tick origin in property setting error"));
        // Ticks in
        int in = xmlreader->readAttributeInt("in", &ok);
        if (ok)
          setticklengthin_axis(in);
        else
          xmlreader->raiseWarning(
              tr("Axis2D Tick length in property setting error"));
        // Tick out
        int out = xmlreader->readAttributeInt("out", &ok);
        if (ok)
          setticklengthout_axis(out);
        else
          xmlreader->raiseWarning(
              tr("Axis2D Tick Length out property setting error"));
        while (!xmlreader->atEnd()) {
          xmlreader->readNext();
          if (xmlreader->isEndElement() && xmlreader->name() == "ticks") break;
          if (xmlreader->isStartElement() && xmlreader->name() == "pen") {
            QPen p = xmlreader->readPen(&ok);
            if (ok) {
              setTickPen(p);
            } else
              xmlreader->raiseWarning(
                  tr("Axis2D tick pen property setting error"));
          }
        }
      }
      // Sub Ticks element
      if (xmlreader->isStartElement() && xmlreader->name() == "subticks") {
        // visible property
        bool subtickvisible = xmlreader->readAttributeBool("visible", &ok);
        if (ok)
          setsubtickvisibility_axis(subtickvisible);
        else
          xmlreader->raiseWarning(
              tr("Axis2D subtick visible property setting error"));
        // Ticks in
        int in = xmlreader->readAttributeInt("in", &ok);
        if (ok)
          setsubticklengthin_axis(in);
        else
          xmlreader->raiseWarning(
              tr("Axis2D subTick length in property setting error"));
        // out length
        int out = xmlreader->readAttributeInt("out", &ok);
        if (ok)
          setsubticklengthout_axis(out);
        else
          xmlreader->raiseWarning(
              tr("Axis2D subTick Length out property setting error"));
        while (!xmlreader->atEnd()) {
          xmlreader->readNext();
          if (xmlreader->isEndElement() && xmlreader->name() == "subticks")
            break;
          if (xmlreader->isStartElement() && xmlreader->name() == "pen") {
            QPen p = xmlreader->readPen(&ok);
            if (ok) {
              setSubTickPen(p);
            } else
              xmlreader->raiseWarning(
                  tr("Axis2D subtick pen property setting error"));
          }
        }
      }
      // Tick label element
      if (xmlreader->isStartElement() && xmlreader->name() == "ticklabels") {
        // visible property
        bool ticklabelvisible = xmlreader->readAttributeBool("visible", &ok);
        if (ok)
          setticklabelvisibility_axis(ticklabelvisible);
        else
          xmlreader->raiseWarning(
              tr("Axis2D tick label visible property setting error"));
        // Tick label padding
        int ticklabelpadding = xmlreader->readAttributeInt("padding", &ok);
        if (ok)
          setticklabelpadding_axis(ticklabelpadding);
        else
          xmlreader->raiseWarning(
              tr("Axis2D tick label padding in property setting error"));
        // Tick Label Rotation
        double ticklabelrotation =
            xmlreader->readAttributeDouble("rotation", &ok);
        if (ok)
          setticklabelrotation_axis(ticklabelrotation);
        else
          xmlreader->raiseWarning(
              tr("Axis2D tick label rotation property setting error"));
        // Tick label side
        QString ticklabelside = xmlreader->readAttributeString("side", &ok);
        if (ok)
          if (ticklabelside == "up")
            setticklabelside_axis(Axis2D::AxisLabelSide::Inside);
          else if (ticklabelside == "down")
            setticklabelside_axis(Axis2D::AxisLabelSide::Outside);
          else
            xmlreader->raiseWarning(
                tr("Axis2D tick label side property setting error"));
        else
          xmlreader->raiseWarning(
              tr("Axis2D tick label side property setting error"));
        // tick label format
        QString ticklabelformat = xmlreader->readAttributeString("format", &ok);
        if (ok)
          if (ticklabelformat == "E")
            setticklabelformat_axis(Axis2D::AxisLabelFormat::E);
          else if (ticklabelformat == "G")
            setticklabelformat_axis(Axis2D::AxisLabelFormat::G);
          else if (ticklabelformat == "e")
            setticklabelformat_axis(Axis2D::AxisLabelFormat::e);
          else if (ticklabelformat == "eb")
            setticklabelformat_axis(Axis2D::AxisLabelFormat::eb);
          else if (ticklabelformat == "ebc")
            setticklabelformat_axis(Axis2D::AxisLabelFormat::ebc);
          else if (ticklabelformat == "f")
            setticklabelformat_axis(Axis2D::AxisLabelFormat::f);
          else if (ticklabelformat == "g")
            setticklabelformat_axis(Axis2D::AxisLabelFormat::g);
          else if (ticklabelformat == "gb")
            setticklabelformat_axis(Axis2D::AxisLabelFormat::gb);
          else if (ticklabelformat == "gbc")
            setticklabelformat_axis(Axis2D::AxisLabelFormat::gbc);
          else
            xmlreader->raiseWarning(
                tr("Axis2D tick label format property setting error"));
        else
          xmlreader->raiseWarning(
              tr("Axis2D tick label format property setting error"));
        // Tick label precision
        int ticklabelprecision = xmlreader->readAttributeInt("precision", &ok);
        if (ok)
          setticklabelprecision_axis(ticklabelprecision);
        else
          xmlreader->raiseWarning(
              tr("Axis2D tick label precision property setting error"));
        while (!xmlreader->atEnd()) {
          xmlreader->readNext();
          if (xmlreader->isEndElement() && xmlreader->name() == "ticklabels")
            break;
          if (xmlreader->isStartElement() && xmlreader->name() == "font") {
            QPair<QFont, QColor> fontpair = xmlreader->readFont(&ok);
            if (ok) {
              setticklabelfont_axis(fontpair.first);
              setticklabelcolor_axis(fontpair.second);
            } else
              xmlreader->raiseWarning(
                  tr("Axis2D font & color property setting error"));
          }
        }
      }
      xmlreader->readNext();
    }
  } else  // no element
    xmlreader->raiseError(tr("no axis element found"));
  // settext tics
  if (!tickertext_->isEmpty()) {
    QSharedPointer<QCPAxisTickerText> textticker =
        qSharedPointerCast<QCPAxisTickerText>(ticker_);
    for (int i = 0; i < tickertext_->size(); i++) {
      textticker->addTick(i, Utilities::splitstring(tickertext_->at(i)));
    }
  }

  return !xmlreader->hasError();
}

void Axis2D::clone(Axis2D *axis) {
  // set ticker text
  for (int i = 0; i < tickertext_->size(); i++) {
    axis->tickertext_->append(tickertext_->at(i));
  }
  QSharedPointer<QCPAxisTickerText> textticker =
      qSharedPointerCast<QCPAxisTickerText>(axis->ticker_);
  for (int i = 0; i < axis->tickertext_->count(); i++) {
    textticker->addTick(i, Utilities::splitstring(axis->tickertext_->at(i)));
  }
  // set ticker date time format
  if (gettickertype_axis() == Axis2D::TickerType::DateTime) {
    QSharedPointer<QCPAxisTickerDateTime> newdtticker =
        qSharedPointerCast<QCPAxisTickerDateTime>(axis->ticker_);
    QSharedPointer<QCPAxisTickerDateTime> dtticker =
        qSharedPointerCast<QCPAxisTickerDateTime>(ticker_);
    newdtticker->setDateTimeFormat(dtticker->dateTimeFormat());
  }
  axis->setshowhide_axis(getshowhide_axis());
  axis->setoffset_axis(getoffset_axis());
  axis->setfrom_axis(getfrom_axis());
  axis->setto_axis(getto_axis());
  axis->setscaletype_axis(getscaletype_axis());
  axis->setinverted_axis(getinverted_axis());
  axis->setstrokecolor_axis(getstrokecolor_axis());
  axis->setstrokethickness_axis(getstrokethickness_axis());
  axis->setstroketype_axis(getstroketype_axis());
  axis->setantialiased_axis(getantialiased_axis());
  // Axis label properties
  axis->setlabeltext_axis(getlabeltext_axis());
  axis->setlabelcolor_axis(getlabelcolor_axis());
  axis->setlabelfont_axis(getlabelfont_axis());
  axis->setlabelpadding_axis(getlabelpadding_axis());
  // Tick properties
  axis->settickvisibility_axis(gettickvisibility_axis());
  axis->setticklengthin_axis(getticklengthin_axis());
  axis->setticklengthout_axis(getticklengthout_axis());
  axis->settickstrokecolor_axis(gettickstrokecolor_axis());
  axis->settickstrokethickness_axis(gettickstrokethickness_axis());
  axis->settickstrokestyle_axis(gettickstrokestyle_axis());
  // Sub-tick properties
  axis->setsubtickvisibility_axis(getsubtickvisibility_axis());
  axis->setsubticklengthin_axis(getsubticklengthin_axis());
  axis->setsubticklengthout_axis(getsubticklengthout_axis());
  axis->setsubtickstrokecolor_axis(getsubtickstrokecolor_axis());
  axis->setsubtickstrokethickness_axis(getsubtickstrokethickness_axis());
  axis->setsubtickstrokestyle_axis(getsubtickstrokestyle_axis());
  // Tick label properties
  axis->setticklabelvisibility_axis(getticklabelvisibility_axis());
  axis->setticklabelpadding_axis(getticklabelpadding_axis());
  axis->setticklabelfont_axis(getticklabelfont_axis());
  axis->setticklabelcolor_axis(getticklabelcolor_axis());
  axis->setticklabelrotation_axis(getticklabelrotation_axis());
  axis->setticklabelside_axis(getticklabelside_axis());
  axis->setticklabelformat_axis(getticklabelformat_axis());
  axis->setticklabelprecision_axis(getticklabelprecision_axis());
}
