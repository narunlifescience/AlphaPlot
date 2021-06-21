#include "Vector2D.h"

#include "../core/Utilities.h"
#include "../future/core/column/Column.h"
#include "Axis2D.h"
#include "Table.h"
#include "core/IconLoader.h"
#include "future/lib/XmlStreamReader.h"
#include "future/lib/XmlStreamWriter.h"

Vector2D::Vector2D(const VectorPlot &vectorplot, Table *table, Column *x1Data,
                   Column *y1Data, Column *x2Data, Column *y2Data, int from,
                   int to, Axis2D *xAxis, Axis2D *yAxis)
    : QCPGraph(xAxis, yAxis),
      vectorplot_(vectorplot),
      xaxis_(xAxis),
      yaxis_(yAxis),
      layername_(QString("<Vector2D>") + QDateTime::currentDateTime().toString(
                                             "yyyy:MM:dd:hh:mm:ss:zzz")),
      d_position_(Position::Tail),
      start_(new QCPLineEnding()),
      stop_(new QCPLineEnding()) {
  // setting icon
  (vectorplot == VectorPlot::XYXY)
      ? icon_ = IconLoader::load("graph2d-vector-xy", IconLoader::LightDark)
      : icon_ = IconLoader::load("graph2d-vector-xyam", IconLoader::LightDark);

  QThread::msleep(1);
  parentPlot()->addLayer(layername_, xaxis_->layer(), QCustomPlot::limBelow);
  setLayer(layername_);
  layer()->setMode(QCPLayer::LayerMode::lmBuffered);
  start_->setStyle(QCPLineEnding::esNone);
  stop_->setStyle(QCPLineEnding::esSpikeArrow);
  setGraphData(table, x1Data, y1Data, x2Data, y2Data, from, to);
  setlinestrokecolor_vecplot(
      Utilities::getRandColorGoldenRatio(Utilities::ColorPal::Dark));
}

Vector2D::~Vector2D() {
  foreach (QCPItemLine *line, linelist_) { parentPlot()->removeItem(line); }
  delete start_;
  delete stop_;
  parentPlot()->removeLayer(layer());
}

void Vector2D::setGraphData(Table *table, Column *x1Data, Column *y1Data,
                            Column *x2Data, Column *y2Data, int from, int to) {
  table_ = table;
  x1col_ = x1Data;
  y1col_ = y1Data;
  x2col_ = x2Data;
  y2col_ = y2Data;
  from_ = from;
  to_ = to;
  bool setcoloronce = true;
  QColor color(Qt::black);
  foreach (QCPItemLine *line, linelist_) {
    if (setcoloronce) {
      color = line->pen().color();
      setcoloronce = false;
    }
    parentPlot()->removeItem(line);
  }
  linelist_.clear();
  double min_x, max_x, min_y, max_y;
  min_x = x1Data->valueAt(from);
  max_x = x1Data->valueAt(from);
  min_y = y1Data->valueAt(from);
  max_y = y1Data->valueAt(from);
  switch (vectorplot_) {
    case VectorPlot::XYXY:
      for (int i = from; i <= to; i++) {
        if (min_x > x1Data->valueAt(i)) {
          min_x = x1Data->valueAt(i);
        }
        if (min_x > x2Data->valueAt(i)) {
          min_x = x2Data->valueAt(i);
        }
        if (max_x < x1Data->valueAt(i)) {
          max_x = x1Data->valueAt(i);
        }
        if (max_x < x2Data->valueAt(i)) {
          max_x = x2Data->valueAt(i);
        }
        if (min_y > y1Data->valueAt(i)) {
          min_y = y1Data->valueAt(i);
        }
        if (min_y > y2Data->valueAt(i)) {
          min_y = y2Data->valueAt(i);
        }
        if (max_y < y1Data->valueAt(i)) {
          max_y = y1Data->valueAt(i);
        }
        if (max_y < y2Data->valueAt(i)) {
          max_y = y2Data->valueAt(i);
        }

        drawLine(x1Data->valueAt(i), y1Data->valueAt(i), x2Data->valueAt(i),
                 y2Data->valueAt(i));
      }
      break;
    case VectorPlot::XYAM:
      double x1 = 0.0, y1 = 0.0, x2 = 0.0, y2 = 0.0;
      for (int i = from; i <= to; i++) {
        const double basex = x1Data->valueAt(i);
        const double basey = y1Data->valueAt(i);
        const double angle = x2Data->valueAt(i);
        const double mag = y2Data->valueAt(i);
        switch (d_position_) {
          case Position::Tail:
            x1 = basex;
            y1 = basey;
            x2 = basex + mag * cos(angle);
            y2 = basey + mag * sin(angle);
            break;

          case Position::Middle: {
            double dxh = 0.5 * mag * cos(angle);
            double dyh = 0.5 * mag * sin(angle);
            x1 = basex - dxh;
            y1 = basey - dyh;
            x2 = basex + dxh;
            y2 = basey + dyh;
          } break;

          case Position::Head:
            x1 = basex - mag * cos(angle);
            y1 = basey - mag * sin(angle);
            x2 = basex;
            y2 = basey;
            break;
        }
        if (min_x > x1) {
          min_x = x1;
        }
        if (min_x > x2) {
          min_x = x2;
        }
        if (max_x < x1) {
          max_x = x1;
        }
        if (max_x < x2) {
          max_x = x2;
        }
        if (min_y > y1) {
          min_y = y1;
        }
        if (min_y > y2) {
          min_y = y2;
        }
        if (max_y < y1) {
          max_y = y1;
        }
        if (max_y < y2) {
          max_y = y2;
        }

        drawLine(x1, y1, x2, y2);
      }
  }
  xaxis_->setfrom_axis(min_x);
  xaxis_->setto_axis(max_x);
  yaxis_->setfrom_axis(min_y);
  yaxis_->setto_axis(max_y);
  setlinestrokecolor_vecplot(color);
}

void Vector2D::drawLine(double x1, double y1, double x2, double y2) {
  QCPItemLine *arrow = new QCPItemLine(parentPlot());
  // addItem(arrow);
  arrow->start->setCoords(x1, y1);
  arrow->end->setCoords(x2, y2);
  arrow->setHead(*stop_);
  arrow->setTail(*start_);
  linelist_.append(arrow);
}

Axis2D *Vector2D::getxaxis() { return xaxis_; }

Axis2D *Vector2D::getyaxis() { return yaxis_; }

QColor Vector2D::getlinestrokecolor_vecplot() const {
  if (linelist_.size() > 0) {
    return linelist_.at(0)->pen().color();
  } else {
    return Qt::black;
  }
}

Qt::PenStyle Vector2D::getlinestrokestyle_vecplot() const {
  if (linelist_.size() > 0) {
    return linelist_.at(0)->pen().style();
  } else {
    return Qt::PenStyle::SolidLine;
  }
}

double Vector2D::getlinestrokethickness_vecplot() const {
  if (linelist_.size() > 0) {
    return linelist_.at(0)->pen().widthF();
  } else {
    return 0.0;
  }
}

bool Vector2D::getlineantialiased_vecplot() const {
  foreach (QCPItemLine *line, linelist_) { return line->antialiased(); }
  return false;
}

Vector2D::LineEnd Vector2D::getendstyle_vecplot(
    const Vector2D::LineEndLocation &location) const {
  QCPLineEnding *ending;
  switch (location) {
    case LineEndLocation::Tail:
      ending = start_;
      break;
    case LineEndLocation::Head:
      ending = stop_;
      break;
  }

  LineEnd end;
  switch (ending->style()) {
    case QCPLineEnding::esNone:
      end = LineEnd::None;
      break;
    case QCPLineEnding::esFlatArrow:
      end = LineEnd::FlatArrow;
      break;
    case QCPLineEnding::esSpikeArrow:
      end = LineEnd::SpikeArrow;
      break;
    case QCPLineEnding::esLineArrow:
      end = LineEnd::LineArrow;
      break;
    case QCPLineEnding::esDisc:
      end = LineEnd::Disc;
      break;
    case QCPLineEnding::esSquare:
      end = LineEnd::Square;
      break;
    case QCPLineEnding::esDiamond:
      end = LineEnd::Diamond;
      break;
    case QCPLineEnding::esBar:
      end = LineEnd::Bar;
      break;
    case QCPLineEnding::esHalfBar:
      end = LineEnd::HalfBar;
      break;
    case QCPLineEnding::esSkewedBar:
      end = LineEnd::SkewedBar;
      break;
  }
  return end;
}

double Vector2D::getendwidth_vecplot(
    const Vector2D::LineEndLocation &location) const {
  QCPLineEnding *ending;
  switch (location) {
    case LineEndLocation::Tail:
      ending = start_;
      break;
    case LineEndLocation::Head:
      ending = stop_;
      break;
  }
  return ending->width();
}

double Vector2D::getendheight_vecplot(
    const Vector2D::LineEndLocation &location) const {
  QCPLineEnding *ending;
  switch (location) {
    case LineEndLocation::Tail:
      ending = start_;
      break;
    case LineEndLocation::Head:
      ending = stop_;
      break;
  }
  return ending->length();
}

bool Vector2D::getendinverted_vecplot(
    const Vector2D::LineEndLocation &location) const {
  QCPLineEnding *ending;
  switch (location) {
    case LineEndLocation::Tail:
      ending = start_;
      break;
    case LineEndLocation::Head:
      ending = stop_;
      break;
  }
  return ending->inverted();
}

bool Vector2D::getlegendvisible_vecplot() const {
  return mParentPlot->legend->hasItemWithPlottable(this);
}

QString Vector2D::getlegendtext_vecplot() const { return name(); }

void Vector2D::setxaxis_vecplot(Axis2D *axis) {
  Q_ASSERT(axis->getorientation_axis() == Axis2D::AxisOreantation::Bottom ||
           axis->getorientation_axis() == Axis2D::AxisOreantation::Top);
  if (axis == getxaxis()) return;

  xaxis_ = axis;
  setKeyAxis(axis);
}

void Vector2D::setyaxis_vecplot(Axis2D *axis) {
  Q_ASSERT(axis->getorientation_axis() == Axis2D::AxisOreantation::Left ||
           axis->getorientation_axis() == Axis2D::AxisOreantation::Right);
  if (axis == getyaxis()) return;

  yaxis_ = axis;
  setValueAxis(axis);
}

void Vector2D::setlineantialiased_vecplot(bool status) {
  foreach (QCPItemLine *line, linelist_) { line->setAntialiased(status); }
}

void Vector2D::setlinestrokecolor_vecplot(const QColor &color) {
  QPen p;
  foreach (QCPItemLine *arrow, linelist_) {
    p = arrow->pen();
    p.setColor(color);
    arrow->setPen(p);
  }
  setPen(p);
}

void Vector2D::setlinestrokestyle_vecplot(const Qt::PenStyle &style) {
  QPen p;
  foreach (QCPItemLine *arrow, linelist_) {
    p = arrow->pen();
    p.setStyle(style);
    arrow->setPen(p);
  }
  setPen(p);
}

void Vector2D::setlinestrokethickness_vecplot(const double value) {
  QPen p;
  foreach (QCPItemLine *arrow, linelist_) {
    p = arrow->pen();
    p.setWidthF(value);
    arrow->setPen(p);
  }
  setPen(p);
}

void Vector2D::setendstyle_vecplot(const Vector2D::LineEnd &end,
                                   const LineEndLocation &location) {
  QCPLineEnding *ending;
  switch (location) {
    case LineEndLocation::Tail:
      ending = start_;
      break;
    case LineEndLocation::Head:
      ending = stop_;
      break;
  }

  switch (end) {
    case LineEnd::None:
      ending->setStyle(QCPLineEnding::esNone);
      break;
    case LineEnd::FlatArrow:
      ending->setStyle(QCPLineEnding::esFlatArrow);
      break;
    case LineEnd::SpikeArrow:
      ending->setStyle(QCPLineEnding::esSpikeArrow);
      break;
    case LineEnd::LineArrow:
      ending->setStyle(QCPLineEnding::esLineArrow);
      break;
    case LineEnd::Disc:
      ending->setStyle(QCPLineEnding::esDisc);
      break;
    case LineEnd::Square:
      ending->setStyle(QCPLineEnding::esSquare);
      break;
    case LineEnd::Diamond:
      ending->setStyle(QCPLineEnding::esDiamond);
      break;
    case LineEnd::Bar:
      ending->setStyle(QCPLineEnding::esBar);
      break;
    case LineEnd::HalfBar:
      ending->setStyle(QCPLineEnding::esHalfBar);
      break;
    case LineEnd::SkewedBar:
      ending->setStyle(QCPLineEnding::esSkewedBar);
      break;
  }

  reloadendings(location);
}

void Vector2D::setendwidth_vecplot(const double value,
                                   const Vector2D::LineEndLocation &location) {
  QCPLineEnding *ending;
  switch (location) {
    case LineEndLocation::Tail:
      ending = start_;
      break;
    case LineEndLocation::Head:
      ending = stop_;
      break;
  }

  ending->setWidth(value);
  reloadendings(location);
}

void Vector2D::setendheight_vecplot(const double value,
                                    const Vector2D::LineEndLocation &location) {
  QCPLineEnding *ending;
  switch (location) {
    case LineEndLocation::Tail:
      ending = start_;
      break;
    case LineEndLocation::Head:
      ending = stop_;
      break;
  }

  ending->setLength(value);
  reloadendings(location);
}

void Vector2D::setendinverted_vecplot(
    const bool value, const Vector2D::LineEndLocation &location) {
  QCPLineEnding *ending;
  switch (location) {
    case LineEndLocation::Tail:
      ending = start_;
      break;
    case LineEndLocation::Head:
      ending = stop_;
      break;
  }

  ending->setInverted(value);
  reloadendings(location);
}

void Vector2D::setlegendvisible_vecplot(const bool value) {
  (value) ? addToLegend() : removeFromLegend();
}

void Vector2D::setlegendtext_vecplot(const QString &name) { setName(name); }

void Vector2D::save(XmlStreamWriter *xmlwriter, int xaxis, int yaxis) {
  xmlwriter->writeStartElement("vector");
  // axis
  xmlwriter->writeAttribute("xaxis", QString::number(xaxis));
  xmlwriter->writeAttribute("yaxis", QString::number(yaxis));
  (vectorplot_ == VectorPlot::XYAM) ? xmlwriter->writeAttribute("type", "xyam")
                                    : xmlwriter->writeAttribute("type", "xyxy");
  // data
  xmlwriter->writeAttribute("table", table_->name());
  xmlwriter->writeAttribute("x1column", x1col_->name());
  xmlwriter->writeAttribute("y1column", y1col_->name());
  xmlwriter->writeAttribute("x2column", x2col_->name());
  xmlwriter->writeAttribute("y2column", y2col_->name());
  xmlwriter->writeAttribute("from", QString::number(from_));
  xmlwriter->writeAttribute("to", QString::number(to_));
  (getlegendvisible_vecplot())
      ? xmlwriter->writeAttribute("legendvisible", "true")
      : xmlwriter->writeAttribute("legendvisible", "false");
  xmlwriter->writeAttribute("legend", getlegendtext_vecplot());
  (getlineantialiased_vecplot())
      ? xmlwriter->writeAttribute("antialias", "true")
      : xmlwriter->writeAttribute("antialias", "false");
  switch (getendstyle_vecplot(Vector2D::LineEndLocation::Head)) {
    case Vector2D::LineEnd::Bar:
      xmlwriter->writeAttribute("endstyle", "bar");
      break;
    case Vector2D::LineEnd::Disc:
      xmlwriter->writeAttribute("endstyle", "disc");
      break;
    case Vector2D::LineEnd::None:
      xmlwriter->writeAttribute("endstyle", "none");
      break;
    case Vector2D::LineEnd::Square:
      xmlwriter->writeAttribute("endstyle", "square");
      break;
    case Vector2D::LineEnd::Diamond:
      xmlwriter->writeAttribute("endstyle", "diamond");
      break;
    case Vector2D::LineEnd::HalfBar:
      xmlwriter->writeAttribute("endstyle", "halfbar");
      break;
    case Vector2D::LineEnd::FlatArrow:
      xmlwriter->writeAttribute("endstyle", "flatarrow");
      break;
    case Vector2D::LineEnd::LineArrow:
      xmlwriter->writeAttribute("endstyle", "linearrow");
      break;
    case Vector2D::LineEnd::SkewedBar:
      xmlwriter->writeAttribute("endstyle", "skewedbar");
      break;
    case Vector2D::LineEnd::SpikeArrow:
      xmlwriter->writeAttribute("endstyle", "spikearrow");
      break;
  }
  xmlwriter->writeAttribute(
      "endwidth",
      QString::number(getendwidth_vecplot(Vector2D::LineEndLocation::Head)));
  xmlwriter->writeAttribute(
      "endheight",
      QString::number(getendheight_vecplot(Vector2D::LineEndLocation::Head)));
  QPen p;
  p.setColor(getlinestrokecolor_vecplot());
  p.setStyle(getlinestrokestyle_vecplot());
  p.setWidthF(getlinestrokethickness_vecplot());
  xmlwriter->writePen(p);
  xmlwriter->writeEndElement();
}

bool Vector2D::load(XmlStreamReader *xmlreader) {
  if (xmlreader->isStartElement() && xmlreader->name() == "vector") {
    bool ok;

    // legend text
    QString legend = xmlreader->readAttributeString("legend", &ok);
    (ok) ? setlegendtext_vecplot(legend)
         : xmlreader->raiseWarning(
               tr("Vector2D legend text property setting error"));

    // line antialias
    bool lineantialias = xmlreader->readAttributeBool("antialias", &ok);
    (ok) ? setlineantialiased_vecplot(lineantialias)
         : xmlreader->raiseWarning(
               tr("Vector2D line antialias property setting error"));

    // ending style
    QString style = xmlreader->readAttributeString("style", &ok);
    if (style == "bar") {
      setendstyle_vecplot(Vector2D::LineEnd::Bar,
                          Vector2D::LineEndLocation::Head);
    } else if (style == "disc") {
      setendstyle_vecplot(Vector2D::LineEnd::Disc,
                          Vector2D::LineEndLocation::Head);
    } else if (style == "none") {
      setendstyle_vecplot(Vector2D::LineEnd::None,
                          Vector2D::LineEndLocation::Head);
    } else if (style == "square") {
      setendstyle_vecplot(Vector2D::LineEnd::Square,
                          Vector2D::LineEndLocation::Head);
    } else if (style == "diamond") {
      setendstyle_vecplot(Vector2D::LineEnd::Diamond,
                          Vector2D::LineEndLocation::Head);
    } else if (style == "halfbar") {
      setendstyle_vecplot(Vector2D::LineEnd::HalfBar,
                          Vector2D::LineEndLocation::Head);
    } else if (style == "flatarrow") {
      setendstyle_vecplot(Vector2D::LineEnd::FlatArrow,
                          Vector2D::LineEndLocation::Head);
    } else if (style == "linearrow") {
      setendstyle_vecplot(Vector2D::LineEnd::LineArrow,
                          Vector2D::LineEndLocation::Head);
    } else if (style == "skewedbar") {
      setendstyle_vecplot(Vector2D::LineEnd::SkewedBar,
                          Vector2D::LineEndLocation::Head);
    } else if (style == "spikearrow") {
      setendstyle_vecplot(Vector2D::LineEnd::SpikeArrow,
                          Vector2D::LineEndLocation::Head);
    }

    // endwidth property
    int endwidth = xmlreader->readAttributeInt("endwidth", &ok);
    (ok) ? setendwidth_vecplot(endwidth, Vector2D::LineEndLocation::Head)
         : xmlreader->raiseWarning(
               tr("Vector2D endwidth property setting error"));

    // endheight property
    int endheight = xmlreader->readAttributeInt("endheight", &ok);
    (ok) ? setendheight_vecplot(endheight, Vector2D::LineEndLocation::Head)
         : xmlreader->raiseWarning(
               tr("Vector2D endheight property setting error"));

    // strokepen property
    while (!xmlreader->atEnd()) {
      xmlreader->readNext();
      if (xmlreader->isEndElement() && xmlreader->name() == "pen") break;
      // pen
      if (xmlreader->isStartElement() && xmlreader->name() == "pen") {
        QPen strokep = xmlreader->readPen(&ok);
        if (ok) {
          setlinestrokecolor_vecplot(strokep.color());
          setlinestrokestyle_vecplot(strokep.style());
          setlinestrokethickness_vecplot(strokep.widthF());
        } else
          xmlreader->raiseWarning(
              tr("Vector2D strokepen property setting error"));
      }
    }

    // legend visible
    bool legendvisible = xmlreader->readAttributeBool("legendvisible", &ok);
    (ok) ? setlegendvisible_vecplot(legendvisible)
         : xmlreader->raiseWarning(
               tr("Curve2D legend visible property setting error"));

  } else  // no element
    xmlreader->raiseError(tr("no Vector2D item element found"));

  return !xmlreader->hasError();
}

void Vector2D::reloadendings(const Vector2D::LineEndLocation &location) {
  switch (location) {
    case LineEndLocation::Head:
      foreach (QCPItemLine *line, linelist_) { line->setHead(*stop_); }
      break;
    case LineEndLocation::Tail:
      foreach (QCPItemLine *line, linelist_) { line->setTail(*start_); }
      break;
  }
}

void Vector2D::datapicker(QMouseEvent *, const QVariant &) {
  qDebug() << "data picker unavailable for vector2D plots";
}

void Vector2D::graphpicker(QMouseEvent *, const QVariant &) {
  qDebug() << "graph picker unavailable for vector2D plots";
}

void Vector2D::movepicker(QMouseEvent *, const QVariant &) {
  qDebug() << "move picker unavailable for vector2D plots";
}

void Vector2D::removepicker(QMouseEvent *, const QVariant &) {
  qDebug() << "remove picker unavailable for vector2D plots";
}
