#include "ColorMap2D.h"
#include "Matrix.h"

ColorMap2D::ColorMap2D(Matrix *matrix, Axis2D *xAxis, Axis2D *yAxis)
    : QCPColorMap(xAxis, yAxis),
      xaxis_(xAxis),
      yaxis_(yAxis),
      matrix_(matrix),
      colorScale_(new QCPColorScale(parentPlot())) {
  data_ = new QCPColorMapData(matrix_->numCols(), matrix_->numRows(),
                              QCPRange(0, matrix_->numCols() - 1),
                              QCPRange(0, matrix_->numRows() - 1));
  // parentPlot()->plotLayout()->addElement(0, 1, colorScale);
  // colorScale->setType(QCPAxis::atRight);
  // setColorScale(colorScale_);
  // colorScale->axis()->setLabel("Magnetic Field Strength");
  setGradient(QCPColorGradient::gpSpectrum);
  // setInterpolate(false);
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
}

ColorMap2D::~ColorMap2D() { delete colorScale_; }
