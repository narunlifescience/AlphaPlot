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

   Description : Pie2D */

#include "Pie2D.h"

#include "../future/core/column/Column.h"
#include "AxisRect2D.h"
#include "Legend2D.h"
#include "Table.h"
#include "core/IconLoader.h"
#include "core/Utilities.h"
#include "future/lib/XmlStreamReader.h"
#include "future/lib/XmlStreamWriter.h"

Pie2D::Pie2D(AxisRect2D *axisrect, const Graph2DCommon::PieStyle &style,
             Table *table, Column *xData, Column *yData, int from, int to)
    : QCPAbstractItem(axisrect->parentPlot()),
      // topLeft(createPosition(QLatin1String("topLeft"))),
      // bottomRight(createPosition(QLatin1String("bottomRight"))),
      axisrect_(axisrect),
      pieData_(new QVector<double>()),
      style_(style),
      pieColors_(new QVector<QColor>()),
      pieLegendItems_(new QVector<PieLegendItem2D *>()),
      layername_(QString("<Pie2D>") + QDateTime::currentDateTime().toString(
                                          "yyyy:MM:dd:hh:mm:ss:zzz")),
      marginpercent_(2),
      table_(table),
      xcolumn_(xData),
      ycolumn_(yData),
      from_(from),
      to_(to) {
  // set Icon
  (style == Graph2DCommon::PieStyle::Pie)
      ? icon_ = IconLoader::load("graph2d-pie", IconLoader::LightDark)
      : icon_ = IconLoader::load("graph2d-halfpie", IconLoader::LightDark);

  if (axisrect_->getAxes2D().count() > 0) {
    QThread::msleep(1);
    parentPlot()->addLayer(layername_, axisrect_->getAxes2D().at(0)->layer(),
                           QCustomPlot::limBelow);
    setLayer(layername_);
  }
  layer()->setMode(QCPLayer::LayerMode::lmBuffered);
  // topLeft->setCoords(axisrect->topLeft());
  // bottomRight->setCoords(axisrect->bottomRight());
  setClipAxisRect(axisrect_);
  mPen.setColor(Qt::white);
  mBrush.setColor(Qt::red);
  mBrush.setStyle(Qt::SolidPattern);
  QList<Axis2D *> axes(axisrect_->getAxes2D());
  for (int i = 0; i < axes.size(); i++) {
    axes.at(i)->setshowhide_axis(false);
  }
  setGraphData(table_, xcolumn_, ycolumn_, from_, to_);
}

Pie2D::~Pie2D() {
  pieData_->clear();
  delete pieData_;
  pieColors_->clear();
  delete pieColors_;
  /*foreach (PieLegendItem2D *item, *pieLegendItems_) {
    axisrect_->getLegend()->removeItem(item);
  }*/
  pieLegendItems_->clear();
  delete pieLegendItems_;
  parentPlot()->removeLayer(layer());
}

void Pie2D::setGraphData(Table *table, Column *xData, Column *yData, int from,
                         int to) {
  foreach (PieLegendItem2D *item, *pieLegendItems_) {
    axisrect_->getLegend()->removeItem(item);
  }
  pieColors_->clear();
  pieLegendItems_->clear();
  table_ = table;
  xcolumn_ = xData;
  ycolumn_ = yData;
  from_ = from;
  to_ = to;
  double sum = 0.0;
  pieData_->clear();
  for (int i = from; i <= to; i++) {
    sum += yData->valueAt(i);
  }
  for (int i = from; i <= to; i++) {
    (style_ == Graph2DCommon::PieStyle::Pie)
        ? pieData_->append((yData->valueAt(i) / sum) * (360 * 16))
        : pieData_->append((yData->valueAt(i) / sum) * (180 * 16));
    QColor color =
        Utilities::getRandColorGoldenRatio(Utilities::ColorPal::Light);
    pieColors_->append(color);
    PieLegendItem2D *pielegenditem =
        new PieLegendItem2D(axisrect_->getLegend(), color, xData->textAt(i));
    pieLegendItems_->append(pielegenditem);
    axisrect_->getLegend()->addItem(pielegenditem);
  }
}

double Pie2D::selectTest(const QPointF &pos, bool onlySelectable,
                         QVariant *details) const {
  Q_UNUSED(details)
  Q_UNUSED(pos)
  Q_UNUSED(onlySelectable)
  return 0.0;
}

AxisRect2D *Pie2D::getaxisrect() const { return axisrect_; }

Qt::PenStyle Pie2D::getstrokestyle_pieplot() const { return mPen.style(); }

QColor Pie2D::getstrokecolor_pieplot() const { return mPen.color(); }

double Pie2D::getstrokethickness_pieplot() const { return mPen.widthF(); }

int Pie2D::getmarginpercent_pieplot() const { return marginpercent_; }

Graph2DCommon::PieStyle Pie2D::getStyle_pieplot() const { return style_; }

void Pie2D::setstrokestyle_pieplot(const Qt::PenStyle &style) {
  mPen.setStyle(style);
}

void Pie2D::setstrokecolor_pieplot(const QColor &color) {
  mPen.setColor(color);
}

void Pie2D::setstrokethickness_pieplot(const double value) {
  mPen.setWidthF(value);
}

void Pie2D::setmarginpercent_pieplot(const int value) {
  marginpercent_ = value;
}

void Pie2D::setstyle_pieplot(const Graph2DCommon::PieStyle &style) {
  style_ = style;
  setGraphData(gettable_pieplot(), getxcolumn_pieplot(), getycolumn_pieplot(),
               getfrom_pieplot(), getto_pieplot());
  // set Icon
  (style == Graph2DCommon::PieStyle::Pie)
      ? icon_ = IconLoader::load("graph2d-pie", IconLoader::LightDark)
      : icon_ = IconLoader::load("graph2d-halfpie", IconLoader::LightDark);
}

void Pie2D::setstrokepen_pieplot(const QPen pen) {
  setstrokecolor_pieplot(pen.color());
  setstrokethickness_pieplot(pen.widthF());
  setstrokestyle_pieplot(pen.style());
}

void Pie2D::draw(QCPPainter *painter) {
  if (pieData_->isEmpty()) return;
  int cumulativesum = 0;

  // QPointF p1 = topLeft->pixelPosition();
  // QPointF p2 = bottomRight->pixelPosition();
  // if (p1.toPoint() == p2.toPoint()) return;
  QRectF ellipseRect = axisrect_->rect();
  if (ellipseRect.width() > ellipseRect.height()) {
    double dif = ellipseRect.width() - ellipseRect.height();
    double margin =
        (ellipseRect.height() * static_cast<double>(marginpercent_) / 2) / 100;
    ellipseRect.adjust(dif / 2 + margin, margin, -(dif / 2) - margin, -margin);
  } else {
    double dif = ellipseRect.height() - ellipseRect.width();
    double margin =
        (ellipseRect.width() * static_cast<double>(marginpercent_) / 2) / 100;
    ellipseRect.adjust(margin, (dif / 2) + margin, -margin,
                       -(dif / 2) - margin);
  }

  painter->setPen(mPen);
  painter->setBrush(mBrush);
  for (int i = 0; i < pieData_->size(); i++) {
    mBrush.setColor(pieColors_->at(i));
    painter->setBrush(mBrush);
    painter->drawPie(ellipseRect, cumulativesum,
                     static_cast<int>(pieData_->at(i)));
    cumulativesum += pieData_->at(i);
  }
}

void Pie2D::drawdoughnutslice(QPainter *painter, double startangle,
                              double stopangle, double outerradius,
                              double innerradius, double offset, QPen strokepen,
                              QBrush fillbrush) {
  int dim = 400;
  double op1, op2, ip1, ip2;
  op1 = ((1 - outerradius) * dim) / 2;
  ip1 = ((1 - innerradius) * dim) / 2;
  op2 = dim - (op1 * 2);
  ip2 = dim - (ip1 * 2);
  QPainterPath path;
  double x = offset + (dim / 2) +
             ((op2 / 2) * qCos((360 - startangle) * 3.141592 / 180));
  double y = offset + (dim / 2) +
             ((op2 / 2) * qSin((360 - startangle) * 3.141592 / 180));
  path.moveTo(x, y);
  path.arcTo(op1 + offset, op1 + offset, op2 + offset, op2 + offset, startangle,
             stopangle - startangle);
  path.arcTo(ip1 + offset, ip1 + offset, ip2 + offset, ip2 + offset, stopangle,
             -(stopangle - startangle));
  path.closeSubpath();
  painter->setBrush(fillbrush);
  painter->setPen(strokepen);
  painter->drawPath(path);
}

void Pie2D::save(XmlStreamWriter *xmlwriter) {
  xmlwriter->writeStartElement("pie");
  // data
  xmlwriter->writeAttribute("table", table_->name());
  xmlwriter->writeAttribute("xcolumn", xcolumn_->name());
  xmlwriter->writeAttribute("ycolumn", ycolumn_->name());
  xmlwriter->writeAttribute("from", QString::number(from_));
  xmlwriter->writeAttribute("to", QString::number(to_));
  // style
  switch (style_) {
    case Graph2DCommon::PieStyle::Pie:
      xmlwriter->writeAttribute("style", "pie");
      break;
    case Graph2DCommon::PieStyle::HalfPie:
      xmlwriter->writeAttribute("style", "halfpie");
  }
  xmlwriter->writeAttribute("marginpercent", QString::number(marginpercent_));
  xmlwriter->writePen(mPen);
  xmlwriter->writeEndElement();
}

bool Pie2D::load(XmlStreamReader *xmlreader) {
  if (xmlreader->isStartElement() && xmlreader->name() == "pie") {
    bool ok;

    // style property
    QString style = xmlreader->readAttributeString("style", &ok);
    if (ok) {
      if (style == "pie")
        setstyle_pieplot(Graph2DCommon::PieStyle::Pie);
      else if (style == "halfpie")
        setstyle_pieplot(Graph2DCommon::PieStyle::HalfPie);
      else
        xmlreader->raiseWarning(tr("Pie2D style property setting error"));
    } else
      xmlreader->raiseWarning(tr("Pie2D style property setting error"));

    // marginpercent property
    int marginpercent = xmlreader->readAttributeInt("marginpercent", &ok);
    if (ok)
      setmarginpercent_pieplot(marginpercent);
    else
      xmlreader->raiseWarning(tr("Pie2D marginpercent property setting error"));

    // strokepen property
    while (!xmlreader->atEnd()) {
      xmlreader->readNext();
      if (xmlreader->isEndElement() && xmlreader->name() == "pen") break;
      // pen
      if (xmlreader->isStartElement() && xmlreader->name() == "pen") {
        QPen strokep = xmlreader->readPen(&ok);
        if (ok)
          setstrokepen_pieplot(strokep);
        else
          xmlreader->raiseWarning(tr("Pie2D strokepen property setting error"));
      }
    }

  } else  // no element
    xmlreader->raiseError(tr("no Pie2D item element found"));

  return !xmlreader->hasError();
}
