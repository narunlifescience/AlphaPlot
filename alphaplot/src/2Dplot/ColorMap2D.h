#ifndef COLORMAP2D_H
#define COLORMAP2D_H

#include "../3rdparty/qcustomplot/qcustomplot.h"
#include "Axis2D.h"

class Matrix;

class ColorMap2D : public QCPColorMap {
 public:
  ColorMap2D(Matrix *matrix, Axis2D *xAxis, Axis2D *yAxis);
  ~ColorMap2D();

  enum class Gradient : int {
    Grayscale = 0,
    Hot = 1,
    Cold = 2,
    Night = 3,
    candy = 4,
    Geography = 5,
    Ion = 6,
    Thermal = 7,
    Polar = 8,
    Spectrum = 9,
    Jet = 10,
    Hues = 11
  };

  Gradient getgradient_colormap() const;
  QString getname_colormap() const;
  int getcolormapscalewidth_colormap() const;
  QCPColorScale *getcolormapscale_colormap() { return colorScale_; }
  Axis2D::AxisLabelFormat getcolormapscaleticklabelformat_axis() const;
  void setgradient_colormap(const Gradient &grad);
  void setname_colormap(const QString &value);
  void setcolormapscalewidth_colormap(const int width);
  void setcolormapscaleticklabelformat_axis(
      const Axis2D::AxisLabelFormat &axisformat);

 private:
  QCPMarginGroup *margingroup_;
  Axis2D *xaxis_;
  Axis2D *yaxis_;
  Matrix *matrix_;
  QCPColorScale *colorScale_;
  QCPColorMapData *data_;
  Gradient gradient_;
};

#endif  // COLORMAP2D_H
