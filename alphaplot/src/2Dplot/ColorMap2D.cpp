#include "ColorMap2D.h"
#include "AxisRect2D.h"
#include "Matrix.h"

ColorMap2D::ColorMap2D(Matrix *matrix, Axis2D *xAxis, Axis2D *yAxis)
    : QCPColorMap(xAxis, yAxis),
      margingroup_(new QCPMarginGroup(parentPlot())),
      xaxis_(xAxis),
      yaxis_(yAxis),
      matrix_(matrix),
      colorScale_(new QCPColorScale(parentPlot())),
      layername_(
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
  data_->setRange(QCPRange(0, rows_ - 1), QCPRange(0, columns_ - 1));
  double datamin = matrix_->cell(0, 0);
  double datamax = matrix_->cell(0, 0);
  double value = datamin;
  for (int i = 0; i < matrix_->numRows(); i++) {
    for (int j = 0; j < matrix_->numCols(); j++) {
      value = matrix_->cell(i, j);
      data_->setCell(i, j, value);
      if (datamin > value) datamin = value;
      if (datamax < value) datamax = value;
    }
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
