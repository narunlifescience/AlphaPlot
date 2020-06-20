#include "ColorMap2D.h"

#include "AxisRect2D.h"
#include "Matrix.h"
#include "future/lib/XmlStreamReader.h"
#include "future/lib/XmlStreamWriter.h"

ColorMap2D::ColorMap2D(Matrix *matrix, Axis2D *xAxis, Axis2D *yAxis)
    : QCPColorMap(xAxis, yAxis),
      margingroup_(new QCPMarginGroup(parentPlot())),
      xaxis_(xAxis),
      yaxis_(yAxis),
      matrix_(matrix),
      colorScale_(new QCPColorScale(parentPlot())),
      layername_(
          QString("<ColorMap2D>") +
          QDateTime::currentDateTime().toString("yyyy:MM:dd:hh:mm:ss:zzz")),
      gradient_(Gradient::Spectrum),
      invertgradient_(false) {
  // setting layer
  QThread::msleep(1);
  parentPlot()->addLayer(layername_, xaxis_->layer(), QCustomPlot::limBelow);
  setLayer(layername_);
  layer()->setMode(QCPLayer::LayerMode::lmBuffered);

  rows_ = matrix_->numRows();
  columns_ = matrix_->numCols();
  data_ = new QCPColorMapData(rows_, columns_, QCPRange(0, rows_ - 1),
                              QCPRange(0, columns_ - 1));
  parentPlot()->plotLayout()->addElement(0, 1, colorScale_);
  colorScale_->setType(QCPAxis::atRight);
  setColorScale(colorScale_);
  setgradient_colormap(gradient_);
  xaxis_->getaxisrect_axis()->setMarginGroup(QCP::msBottom | QCP::msTop,
                                             margingroup_);
  colorScale_->setMarginGroup(QCP::msBottom | QCP::msTop, margingroup_);
  colorScale_->setRangeDrag(true);
  setColorMapData(matrix_);
  setData(data_);
}

ColorMap2D::~ColorMap2D() {
  delete colorScale_;
  delete margingroup_;
  parentPlot()->removeLayer(layer());
}

void ColorMap2D::setColorMapData(Matrix *matrix) {
  matrix_ = matrix;
  rows_ = matrix_->numRows();
  columns_ = matrix_->numCols();
  data_->setSize(matrix_->numRows(), matrix_->numCols());
  data_->setRange(QCPRange(matrix_->xStart(), matrix_->xEnd()),
                  QCPRange(matrix_->yStart(), matrix_->yEnd()));
  double datamin = matrix_->cell(0, 0);
  double datamax = matrix_->cell(0, 0);
  double value = datamin;
  for (int i = 0; i < matrix_->numRows(); i++) {
    for (int j = 0; j < matrix_->numCols(); j++) {
      value = matrix_->cell(i, j);
      data_->setCell(i, j, value);
      datamin = qMin(datamin, value);
    }
    datamax = qMax(datamax, value);
  }
  setDataRange(QCPRange(datamin, datamax));
}

Axis2D *ColorMap2D::getxaxis() const { return xaxis_; }

Axis2D *ColorMap2D::getyaxis() const { return yaxis_; }

ColorMap2D::Gradient ColorMap2D::getgradient_colormap() const {
  return gradient_;
}

int ColorMap2D::getlevelcount_colormap() const {
  return colorgradient_.levelCount();
}

bool ColorMap2D::getgradientinverted_colormap() const {
  return invertgradient_;
}

bool ColorMap2D::getgradientperiodic_colormap() const {
  return colorgradient_.periodic();
}

QString ColorMap2D::getname_colormap() const { return colorScale_->label(); }

int ColorMap2D::getcolormapscalewidth_colormap() const {
  return colorScale_->barWidth();
}

Axis2D::AxisLabelFormat ColorMap2D::getcolormapscaleticklabelformat_axis()
    const {
  const QString format = colorScale_->axis()->numberFormat();
  Axis2D::AxisLabelFormat axisformat;
  if (format == "e") {
    axisformat = Axis2D::AxisLabelFormat::e;
  } else if (format == "eb") {
    axisformat = Axis2D::AxisLabelFormat::eb;
  } else if (format == "ebc") {
    axisformat = Axis2D::AxisLabelFormat::ebc;
  } else if (format == "E") {
    axisformat = Axis2D::AxisLabelFormat::E;
  } else if (format == "f") {
    axisformat = Axis2D::AxisLabelFormat::f;
  } else if (format == "g") {
    axisformat = Axis2D::AxisLabelFormat::g;
  } else if (format == "gb") {
    axisformat = Axis2D::AxisLabelFormat::gb;
  } else if (format == "gbc") {
    axisformat = Axis2D::AxisLabelFormat::gbc;
  } else if (format == "G") {
    axisformat = Axis2D::AxisLabelFormat::G;
  } else {
    qDebug() << "unknown Axis2D::AxisLabelFormat: " << format;
    axisformat = Axis2D::AxisLabelFormat::gbc;
  }
  return axisformat;
}

void ColorMap2D::setgradient_colormap(const ColorMap2D::Gradient &grad) {
  gradient_ = grad;
  switch (gradient_) {
    case Gradient::Grayscale:
      colorgradient_.loadPreset(QCPColorGradient::gpGrayscale);
      break;
    case Gradient::Hot:
      colorgradient_.loadPreset(QCPColorGradient::gpHot);
      break;
    case Gradient::Cold:
      colorgradient_.loadPreset(QCPColorGradient::gpCold);
      break;
    case Gradient::Night:
      colorgradient_.loadPreset(QCPColorGradient::gpNight);
      break;
    case Gradient::candy:
      colorgradient_.loadPreset(QCPColorGradient::gpCandy);
      break;
    case Gradient::Geography:
      colorgradient_.loadPreset(QCPColorGradient::gpGeography);
      break;
    case Gradient::Ion:
      colorgradient_.loadPreset(QCPColorGradient::gpIon);
      break;
    case Gradient::Thermal:
      colorgradient_.loadPreset(QCPColorGradient::gpThermal);
      break;
    case Gradient::Polar:
      colorgradient_.loadPreset(QCPColorGradient::gpPolar);
      break;
    case Gradient::Spectrum:
      colorgradient_.loadPreset(QCPColorGradient::gpSpectrum);
      break;
    case Gradient::Jet:
      colorgradient_.loadPreset(QCPColorGradient::gpJet);
      break;
    case Gradient::Hues:
      colorgradient_.loadPreset(QCPColorGradient::gpHues);
      break;
  }
  (invertgradient_) ? setGradient(colorgradient_.inverted())
                    : setGradient(colorgradient_);
}

void ColorMap2D::setlevelcount_colormap(const int value) {
  colorgradient_.setLevelCount(value);
  (invertgradient_) ? setGradient(colorgradient_.inverted())
                    : setGradient(colorgradient_);
}

void ColorMap2D::setgradientinverted_colormap(bool status) {
  invertgradient_ = status;
  setgradient_colormap(gradient_);
}

void ColorMap2D::setgradientperiodic_colormap(bool status) {
  colorgradient_.setPeriodic(status);
  (invertgradient_) ? setGradient(colorgradient_.inverted())
                    : setGradient(colorgradient_);
}

void ColorMap2D::setname_colormap(const QString &value) {
  setName(value);
  colorScale_->setLabel(value);
}

void ColorMap2D::setcolormapscalewidth_colormap(const int width) {
  colorScale_->setBarWidth(width);
}

void ColorMap2D::setcolormapscaleticklabelformat_axis(
    const Axis2D::AxisLabelFormat &axisformat) {
  switch (axisformat) {
    case Axis2D::AxisLabelFormat::e:
      colorScale_->axis()->setNumberFormat("e");
      break;
    case Axis2D::AxisLabelFormat::eb:
      colorScale_->axis()->setNumberFormat("eb");
      break;
    case Axis2D::AxisLabelFormat::ebc:
      colorScale_->axis()->setNumberFormat("ebc");
      break;
    case Axis2D::AxisLabelFormat::E:
      colorScale_->axis()->setNumberFormat("E");
      break;
    case Axis2D::AxisLabelFormat::f:
      colorScale_->axis()->setNumberFormat("f");
      break;
    case Axis2D::AxisLabelFormat::g:
      colorScale_->axis()->setNumberFormat("g");
      break;
    case Axis2D::AxisLabelFormat::gb:
      colorScale_->axis()->setNumberFormat("gb");
      break;
    case Axis2D::AxisLabelFormat::gbc:
      colorScale_->axis()->setNumberFormat("gbc");
      break;
    case Axis2D::AxisLabelFormat::G:
      colorScale_->axis()->setNumberFormat("G");
      break;
  }
}

void ColorMap2D::save(XmlStreamWriter *xmlwriter) {
  xmlwriter->writeStartElement("colormap");
  xmlwriter->writeAttribute("matrix", matrix_->name());
  (interpolate()) ? xmlwriter->writeAttribute("interpolate", "true")
                  : xmlwriter->writeAttribute("interpolate", "false");
  (tightBoundary()) ? xmlwriter->writeAttribute("tightboundary", "true")
                    : xmlwriter->writeAttribute("tightboundary", "false");
  xmlwriter->writeAttribute("levelcount",
                            QString::number(getlevelcount_colormap()));
  switch (getgradient_colormap()) {
    case ColorMap2D::Gradient::Hot:
      xmlwriter->writeAttribute("gradient", "hot");
      break;
    case ColorMap2D::Gradient::Ion:
      xmlwriter->writeAttribute("gradient", "ion");
      break;
    case ColorMap2D::Gradient::Jet:
      xmlwriter->writeAttribute("gradient", "jet");
      break;
    case ColorMap2D::Gradient::Cold:
      xmlwriter->writeAttribute("gradient", "cold");
      break;
    case ColorMap2D::Gradient::Hues:
      xmlwriter->writeAttribute("gradient", "hues");
      break;
    case ColorMap2D::Gradient::Night:
      xmlwriter->writeAttribute("gradient", "night");
      break;
    case ColorMap2D::Gradient::Polar:
      xmlwriter->writeAttribute("gradient", "polar");
      break;
    case ColorMap2D::Gradient::candy:
      xmlwriter->writeAttribute("gradient", "candy");
      break;
    case ColorMap2D::Gradient::Thermal:
      xmlwriter->writeAttribute("gradient", "thermal");
      break;
    case ColorMap2D::Gradient::Spectrum:
      xmlwriter->writeAttribute("gradient", "spectrum");
      break;
    case ColorMap2D::Gradient::Geography:
      xmlwriter->writeAttribute("gradient", "geography");
      break;
    case ColorMap2D::Gradient::Grayscale:
      xmlwriter->writeAttribute("gradient", "grayscale");
      break;
  }
  (getgradientinverted_colormap())
      ? xmlwriter->writeAttribute("invertgradient", "true")
      : xmlwriter->writeAttribute("invertgradient", "false");
  (getgradientperiodic_colormap())
      ? xmlwriter->writeAttribute("periodicgradient", "true")
      : xmlwriter->writeAttribute("periodicgradient", "false");
  xmlwriter->writeStartElement("scale");
  (getcolormapscale_colormap()->visible())
      ? xmlwriter->writeAttribute("mapscalevisible", "true")
      : xmlwriter->writeAttribute("mapscalevisible", "false");
  xmlwriter->writeAttribute("width",
                            QString::number(getcolormapscalewidth_colormap()));
  (getcolormapscale_colormap()->axis()->visible())
      ? xmlwriter->writeAttribute("visible", "true")
      : xmlwriter->writeAttribute("visible", "false");
  xmlwriter->writeAttribute(
      "offset", QString::number(getcolormapscale_colormap()->axis()->offset()));
  xmlwriter->writeAttribute(
      "from",
      QString::number(getcolormapscale_colormap()->axis()->range().lower));
  xmlwriter->writeAttribute(
      "to",
      QString::number(getcolormapscale_colormap()->axis()->range().upper));
  switch (dataScaleType()) {
    case QCPAxis::ScaleType::stLinear:
      xmlwriter->writeAttribute("scaletype", "linear");
      break;
    case QCPAxis::ScaleType::stLogarithmic:
      xmlwriter->writeAttribute("scaletype", "logarithemic");
      break;
  }
  (getcolormapscale_colormap()->axis()->rangeReversed())
      ? xmlwriter->writeAttribute("inverted", "true")
      : xmlwriter->writeAttribute("inverted", "false");
  (getcolormapscale_colormap()->axis()->antialiased())
      ? xmlwriter->writeAttribute("antialias", "true")
      : xmlwriter->writeAttribute("antialias", "false");
  xmlwriter->writePen(getcolormapscale_colormap()->axis()->basePen());
  xmlwriter->writeStartElement("label");
  xmlwriter->writeAttribute("text", getname_colormap());
  xmlwriter->writeAttribute(
      "padding",
      QString::number(getcolormapscale_colormap()->axis()->labelPadding()));
  xmlwriter->writeFont(getcolormapscale_colormap()->axis()->labelFont(),
                       getcolormapscale_colormap()->axis()->labelColor());
  xmlwriter->writeEndElement();
  // Ticks
  xmlwriter->writeStartElement("ticks");
  (getcolormapscale_colormap()->axis()->ticks())
      ? xmlwriter->writeAttribute("visible", "true")
      : xmlwriter->writeAttribute("visible", "false");
  xmlwriter->writeAttribute(
      "in",
      QString::number(getcolormapscale_colormap()->axis()->tickLengthIn()));
  xmlwriter->writeAttribute(
      "out",
      QString::number(getcolormapscale_colormap()->axis()->tickLengthOut()));
  xmlwriter->writePen(getcolormapscale_colormap()->axis()->tickPen());
  xmlwriter->writeEndElement();
  // Subticks
  xmlwriter->writeStartElement("subticks");
  (getcolormapscale_colormap()->axis()->subTicks())
      ? xmlwriter->writeAttribute("visible", "true")
      : xmlwriter->writeAttribute("visible", "false");
  xmlwriter->writeAttribute(
      "in",
      QString::number(getcolormapscale_colormap()->axis()->subTickLengthIn()));
  xmlwriter->writeAttribute(
      "out",
      QString::number(getcolormapscale_colormap()->axis()->subTickLengthOut()));
  xmlwriter->writePen(getcolormapscale_colormap()->axis()->subTickPen());
  xmlwriter->writeEndElement();
  // Tick Labels
  xmlwriter->writeStartElement("ticklabels");
  (getcolormapscale_colormap()->axis()->tickLabels())
      ? xmlwriter->writeAttribute("visible", "true")
      : xmlwriter->writeAttribute("visible", "false");
  xmlwriter->writeAttribute(
      "padding",
      QString::number(getcolormapscale_colormap()->axis()->tickLabelPadding()));
  xmlwriter->writeAttribute(
      "rotation",
      QString::number(
          getcolormapscale_colormap()->axis()->tickLabelRotation()));
  switch (getcolormapscale_colormap()->axis()->tickLabelSide()) {
    case QCPAxis::LabelSide::lsInside:
      xmlwriter->writeAttribute("side", "up");
      break;
    case QCPAxis::LabelSide::lsOutside:
      xmlwriter->writeAttribute("side", "down");
      break;
  }
  switch (getcolormapscaleticklabelformat_axis()) {
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
  xmlwriter->writeAttribute(
      "precision",
      QString::number(getcolormapscale_colormap()->axis()->numberPrecision()));
  xmlwriter->writeFont(getcolormapscale_colormap()->axis()->tickLabelFont(),
                       getcolormapscale_colormap()->axis()->tickLabelColor());
  xmlwriter->writeEndElement();
  xmlwriter->writeEndElement();
  xmlwriter->writeEndElement();
}

bool ColorMap2D::load(XmlStreamReader *xmlreader) {
  bool ok;
  // interpolate
  bool intpo = xmlreader->readAttributeBool("interpolate", &ok);
  (ok) ? setInterpolate(intpo)
       : xmlreader->raiseWarning(
             tr("ColorMap2D interpolate property setting error"));
  // tightboundary
  bool tgtbny = xmlreader->readAttributeBool("tightboundary", &ok);
  (ok) ? setTightBoundary(tgtbny)
       : xmlreader->raiseWarning(
             tr("ColorMap2D tightboundry property setting error"));
  // levelcount
  int lvlcnt = xmlreader->readAttributeInt("levelcount", &ok);
  (ok) ? setlevelcount_colormap(lvlcnt)
       : xmlreader->raiseWarning(
             tr("ColorMap2D levelcount property setting error"));
  // gradient
  QString gdnt = xmlreader->readAttributeString("gradient", &ok);
  if (ok) {
    if (gdnt == "hot") {
      setgradient_colormap(ColorMap2D::Gradient::Hot);
    } else if (gdnt == "ion") {
      setgradient_colormap(ColorMap2D::Gradient::Ion);
    } else if (gdnt == "jet") {
      setgradient_colormap(ColorMap2D::Gradient::Jet);
    } else if (gdnt == "cold") {
      setgradient_colormap(ColorMap2D::Gradient::Cold);
    } else if (gdnt == "hues") {
      setgradient_colormap(ColorMap2D::Gradient::Hues);
    } else if (gdnt == "night") {
      setgradient_colormap(ColorMap2D::Gradient::Night);
    } else if (gdnt == "polar") {
      setgradient_colormap(ColorMap2D::Gradient::Polar);
    } else if (gdnt == "candy") {
      setgradient_colormap(ColorMap2D::Gradient::candy);
    } else if (gdnt == "thermal") {
      setgradient_colormap(ColorMap2D::Gradient::Thermal);
    } else if (gdnt == "spectrum") {
      setgradient_colormap(ColorMap2D::Gradient::Spectrum);
    } else if (gdnt == "geography") {
      setgradient_colormap(ColorMap2D::Gradient::Geography);
    } else if (gdnt == "grayscale") {
      setgradient_colormap(ColorMap2D::Gradient::Grayscale);
    } else {
      qDebug() << "unknown gradient parameter: xml";
    }
  } else
    xmlreader->raiseWarning(tr("ColorMap2D gradient property setting error"));

  // invertgradient
  bool invgra = xmlreader->readAttributeBool("invertgradient", &ok);
  (ok) ? setgradientinverted_colormap(invgra)
       : xmlreader->raiseWarning(
             tr("ColorMap2D gradient invert property setting error"));
  // periodicgradient
  bool pergra = xmlreader->readAttributeBool("periodicgradient", &ok);
  (ok) ? setgradientperiodic_colormap(pergra)
       : xmlreader->raiseWarning(
             tr("ColorMap2D gradient periodic property setting error"));

  while (!xmlreader->atEnd()) {
    if (xmlreader->isEndElement() && xmlreader->name() == "colormap") break;

    if (xmlreader->isStartElement() && xmlreader->name() == "scale") {
      // color map scale visible
      bool mapv = xmlreader->readAttributeBool("mapscalevisible", &ok);
      (ok) ? getcolormapscale_colormap()->setVisible(mapv)
           : xmlreader->raiseWarning(
                 tr("ColorMap2D mapscale visible property setting error"));
      // levelcount
      int wdt = xmlreader->readAttributeInt("width", &ok);
      (ok) ? setcolormapscalewidth_colormap(wdt)
           : xmlreader->raiseWarning(
                 tr("ColorMap2D scale width property setting error"));
      // visible
      bool sv = xmlreader->readAttributeBool("visible", &ok);
      (ok) ? getcolormapscale_colormap()->axis()->setVisible(sv)
           : xmlreader->raiseWarning(
                 tr("ColorMap2D scale visible property setting error"));
      // offset
      int ofst = xmlreader->readAttributeInt("offset", &ok);
      (ok) ? getcolormapscale_colormap()->axis()->setOffset(ofst)
           : xmlreader->raiseWarning(
                 tr("ColorMap2D scale offset property setting error"));
      // from
      double frm = xmlreader->readAttributeDouble("from", &ok);
      (ok) ? getcolormapscale_colormap()->axis()->setRange(
                 frm, getcolormapscale_colormap()->axis()->range().upper)
           : xmlreader->raiseWarning(
                 tr("ColorMap2D scale from property setting error"));
      // to
      double to = xmlreader->readAttributeDouble("to", &ok);
      (ok) ? getcolormapscale_colormap()->axis()->setRange(
                 getcolormapscale_colormap()->axis()->range().lower, to)
           : xmlreader->raiseWarning(
                 tr("ColorMap2D scale to property setting error"));
      // scale type
      QString scaletype = xmlreader->readAttributeString("scaletype", &ok);
      if (ok) {
        if (scaletype == "linear") {
          setDataScaleType(QCPAxis::ScaleType::stLinear);
        } else if (scaletype == "logarithemic") {
          setDataScaleType(QCPAxis::ScaleType::stLogarithmic);
        }
      } else
        xmlreader->raiseWarning(
            tr("ColorMap2D scale type property setting error"));
      // scale inverted
      bool invrt = xmlreader->readAttributeBool("inverted", &ok);
      (ok) ? getcolormapscale_colormap()->axis()->setRangeReversed(invrt)
           : xmlreader->raiseWarning(
                 tr("ColorMap2D scale inverted property setting error"));
      // scale antialias
      bool ant = xmlreader->readAttributeBool("antialias", &ok);
      (ok) ? getcolormapscale_colormap()->axis()->setAntialiased(ant)
           : xmlreader->raiseWarning(
                 tr("ColorMap2D scale antialias property setting error"));
      // scale pen property
      while (!xmlreader->atEnd()) {
        xmlreader->readNext();
        if (xmlreader->isEndElement() && xmlreader->name() == "pen") break;
        // pen
        if (xmlreader->isStartElement() && xmlreader->name() == "pen") {
          QPen strokep = xmlreader->readPen(&ok);
          if (ok) {
            getcolormapscale_colormap()->axis()->setBasePen(strokep);
          } else
            xmlreader->raiseWarning(
                tr("ColorMap2D scale pen property setting error"));
        }
      }
      // label property
      while (!xmlreader->atEnd()) {
        xmlreader->readNext();
        if (xmlreader->isEndElement() && xmlreader->name() == "label") break;
        // label text
        QString lbltxt = xmlreader->readAttributeString("text", &ok);
        (ok) ? setname_colormap(lbltxt)
             : xmlreader->raiseWarning(
                   tr("ColorMap2D label text property setting error"));
        // label padding
        int lblpadding = xmlreader->readAttributeInt("padding", &ok);
        (ok) ? getcolormapscale_colormap()->axis()->setLabelPadding(lblpadding)
             : xmlreader->raiseWarning(
                   tr("ColorMap2D label padding property setting error"));
        // label font
        while (!xmlreader->atEnd()) {
          xmlreader->readNext();
          if (xmlreader->isEndElement() && xmlreader->name() == "font") break;
          if (xmlreader->isStartElement() && xmlreader->name() == "font") {
            QPair<QFont, QColor> fontpair = xmlreader->readFont(&ok);
            if (ok) {
              getcolormapscale_colormap()->axis()->setLabelFont(fontpair.first);
              getcolormapscale_colormap()->axis()->setLabelColor(
                  fontpair.second);
            } else
              xmlreader->raiseWarning(
                  tr("ColorMap2D font & color property setting error"));
          }
        }
      }
      while (!xmlreader->atEnd()) {
        xmlreader->readNext();
        if (xmlreader->isEndElement() && xmlreader->name() == "ticks") break;
        // tick visible property
        bool tickvisible = xmlreader->readAttributeBool("visible", &ok);
        if (ok)
          getcolormapscale_colormap()->axis()->setTicks(tickvisible);
        else
          xmlreader->raiseWarning(
              tr("ColorMap2D tick visible property setting error"));
        // Ticks in
        int in = xmlreader->readAttributeInt("in", &ok);
        if (ok)
          getcolormapscale_colormap()->axis()->setTickLengthIn(in);
        else
          xmlreader->raiseWarning(
              tr("ColorMap2D Tick length in property setting error"));
        // Tick out
        int out = xmlreader->readAttributeInt("out", &ok);
        if (ok)
          getcolormapscale_colormap()->axis()->setTickLengthOut(out);
        else
          xmlreader->raiseWarning(
              tr("ColorMap2D Tick Length out property setting error"));
        while (!xmlreader->atEnd()) {
          xmlreader->readNext();
          if (xmlreader->isEndElement() && xmlreader->name() == "pen") break;
          if (xmlreader->isStartElement() && xmlreader->name() == "pen") {
            QPen p = xmlreader->readPen(&ok);
            if (ok) {
              getcolormapscale_colormap()->axis()->setTickPen(p);
            } else
              xmlreader->raiseWarning(
                  tr("Axis2D tick pen property setting error"));
          }
        }
      }
      // Sub Ticks element
      while (!xmlreader->atEnd()) {
        xmlreader->readNext();
        if (xmlreader->isEndElement() && xmlreader->name() == "subticks") break;
        // visible property
        bool subtickvisible = xmlreader->readAttributeBool("visible", &ok);
        (ok) ? getcolormapscale_colormap()->axis()->setSubTicks(subtickvisible)
             : xmlreader->raiseWarning(
                   tr("ColorMap2D subtick visible property setting error"));
        // Ticks in
        int in = xmlreader->readAttributeInt("in", &ok);
        (ok) ? getcolormapscale_colormap()->axis()->setSubTickLengthIn(in)
             : xmlreader->raiseWarning(
                   tr("ColorMap2D subTick length in property setting error"));
        // out length
        int out = xmlreader->readAttributeInt("out", &ok);
        (ok) ? getcolormapscale_colormap()->axis()->setSubTickLengthOut(out)
             : xmlreader->raiseWarning(
                   tr("ColorMap2D subTick Length out property setting error"));
        while (!xmlreader->atEnd()) {
          xmlreader->readNext();
          if (xmlreader->isEndElement() && xmlreader->name() == "pen") break;
          if (xmlreader->isStartElement() && xmlreader->name() == "pen") {
            QPen p = xmlreader->readPen(&ok);
            (ok) ? getcolormapscale_colormap()->axis()->setSubTickPen(p)
                 : xmlreader->raiseWarning(
                       tr("ColorMap2D subtick pen property setting error"));
          }
        }
      }
      // Tick label element
      while (!xmlreader->atEnd()) {
        xmlreader->readNext();
        if (xmlreader->isEndElement() && xmlreader->name() == "ticklabels")
          break;

        // visible property
        bool ticklabelvisible = xmlreader->readAttributeBool("visible", &ok);
        (ok) ? getcolormapscale_colormap()->axis()->setTickLabels(
                   ticklabelvisible)
             : xmlreader->raiseWarning(
                   tr("ColorMap2D tick label visible property setting error"));
        // Tick label padding
        int ticklabelpadding = xmlreader->readAttributeInt("padding", &ok);
        (ok) ? getcolormapscale_colormap()->axis()->setTickLabelPadding(
                   ticklabelpadding)
             : xmlreader->raiseWarning(tr(
                   "ColorMap2D tick label padding in property setting error"));
        // Tick Label Rotation
        double ticklabelrotation =
            xmlreader->readAttributeDouble("rotation", &ok);
        (ok) ? getcolormapscale_colormap()->axis()->setTickLabelRotation(
                   ticklabelrotation)
             : xmlreader->raiseWarning(
                   tr("ColorMap2D tick label rotation property setting error"));
        // Tick label side
        QString ticklabelside = xmlreader->readAttributeString("side", &ok);
        if (ok)
          if (ticklabelside == "up")
            getcolormapscale_colormap()->axis()->setTickLabelSide(
                QCPAxis::LabelSide::lsInside);
          else if (ticklabelside == "down")
            getcolormapscale_colormap()->axis()->setTickLabelSide(
                QCPAxis::LabelSide::lsOutside);
          else
            xmlreader->raiseWarning(
                tr("ColorMap2D tick label side property setting error"));
        else
          xmlreader->raiseWarning(
              tr("ColorMap2D tick label side property setting error"));
        // tick label format
        QString ticklabelformat = xmlreader->readAttributeString("format", &ok);
        if (ok)
          if (ticklabelformat == "E")
            setcolormapscaleticklabelformat_axis(Axis2D::AxisLabelFormat::E);
          else if (ticklabelformat == "G")
            setcolormapscaleticklabelformat_axis(Axis2D::AxisLabelFormat::G);
          else if (ticklabelformat == "e")
            setcolormapscaleticklabelformat_axis(Axis2D::AxisLabelFormat::e);
          else if (ticklabelformat == "eb")
            setcolormapscaleticklabelformat_axis(Axis2D::AxisLabelFormat::eb);
          else if (ticklabelformat == "ebc")
            setcolormapscaleticklabelformat_axis(Axis2D::AxisLabelFormat::ebc);
          else if (ticklabelformat == "f")
            setcolormapscaleticklabelformat_axis(Axis2D::AxisLabelFormat::f);
          else if (ticklabelformat == "g")
            setcolormapscaleticklabelformat_axis(Axis2D::AxisLabelFormat::g);
          else if (ticklabelformat == "gb")
            setcolormapscaleticklabelformat_axis(Axis2D::AxisLabelFormat::gb);
          else if (ticklabelformat == "gbc")
            setcolormapscaleticklabelformat_axis(Axis2D::AxisLabelFormat::gbc);
          else
            xmlreader->raiseWarning(
                tr("ColorMap2D tick label format property setting error"));
        else
          xmlreader->raiseWarning(
              tr("ColorMap2D tick label format property setting error"));
        // Tick label precision
        int ticklabelprecision = xmlreader->readAttributeInt("precision", &ok);
        (ok) ? getcolormapscale_colormap()->axis()->setNumberPrecision(
                   ticklabelprecision)
             : xmlreader->raiseWarning(tr(
                   "ColorMap2D tick label precision property setting error"));
        while (!xmlreader->atEnd()) {
          xmlreader->readNext();
          if (xmlreader->isEndElement() && xmlreader->name() == "font") break;
          if (xmlreader->isStartElement() && xmlreader->name() == "font") {
            QPair<QFont, QColor> fontpair = xmlreader->readFont(&ok);
            if (ok) {
              getcolormapscale_colormap()->axis()->setTickLabelFont(
                  fontpair.first);
              getcolormapscale_colormap()->axis()->setTickLabelColor(
                  fontpair.second);
            } else
              xmlreader->raiseWarning(
                  tr("ColorMap2D font & color property setting error"));
          }
        }
      }
    }
    xmlreader->readNext();
  }

  return !xmlreader->hasError();
}
