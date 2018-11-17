#ifndef COLORMAP2D_H
#define COLORMAP2D_H

#include "../3rdparty/qcustomplot/qcustomplot.h"
#include "Axis2D.h"

class Matrix;

class ColorMap2D : public QCPColorMap {
 public:
  ColorMap2D(Matrix *matrix, Axis2D *xAxis, Axis2D *yAxis);
  ~ColorMap2D();

 private:
  Axis2D *xaxis_;
  Axis2D *yaxis_;
  Matrix *matrix_;
  QCPColorScale *colorScale_;
  QCPColorMapData *data_;
};

#endif  // COLORMAP2D_H
