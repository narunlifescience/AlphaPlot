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
      gradient_(Gradient::Spectrum) {
  data_ = new QCPColorMapData(matrix_->numRows(), matrix_->numCols(),
                              QCPRange(0, matrix_->numRows() - 1),
                              QCPRange(0, matrix_->numCols() - 1));
  parentPlot()->plotLayout()->addElement(0, 1, colorScale_);
  colorScale_->setType(QCPAxis::atRight);
  setColorScale(colorScale_);
  // colorScale->axis()->setLabel("Magnetic Field Strength");
  setgradient_colormap(gradient_);
  xaxis_->getaxisrect_axis()->setMarginGroup(QCP::msBottom | QCP::msTop,
                                             margingroup_);
  colorScale_->setMarginGroup(QCP::msBottom | QCP::msTop, margingroup_);
  double datamin = matrix->cell(0, 0);
  double datamax = matrix->cell(0, 0);
  double value = datamin;
  for (int i = 0; i < matrix_->numCols(); i++) {
    for (int j = 0; j < matrix_->numCols(); j++) {
      value = matrix_->cell(i, j);
      data_->setCell(i, j, value);
      if (datamin > value) datamin = value;
      if (datamax < value) datamax = value;
    }
  }
  setDataRange(QCPRange(datamin, datamax));
  setData(data_);
  colorScale_->setRangeDrag(true);
}

ColorMap2D::~ColorMap2D() {
  delete colorScale_;
  delete margingroup_;
}

ColorMap2D::Gradient ColorMap2D::getgradient_colormap() const {
  return gradient_;
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
      setGradient(QCPColorGradient::gpGrayscale);
      break;
    case Gradient::Hot:
      setGradient(QCPColorGradient::gpHot);
      break;
    case Gradient::Cold:
      setGradient(QCPColorGradient::gpCold);
      break;
    case Gradient::Night:
      setGradient(QCPColorGradient::gpNight);
      break;
    case Gradient::candy:
      setGradient(QCPColorGradient::gpCandy);
      break;
    case Gradient::Geography:
      setGradient(QCPColorGradient::gpGeography);
      break;
    case Gradient::Ion:
      setGradient(QCPColorGradient::gpIon);
      break;
    case Gradient::Thermal:
      setGradient(QCPColorGradient::gpThermal);
      break;
    case Gradient::Polar:
      setGradient(QCPColorGradient::gpPolar);
      break;
    case Gradient::Spectrum:
      setGradient(QCPColorGradient::gpSpectrum);
      break;
    case Gradient::Jet:
      setGradient(QCPColorGradient::gpJet);
      break;
    case Gradient::Hues:
      setGradient(QCPColorGradient::gpHues);
      break;
  }
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
