#ifndef COLORMAP2D_H
#define COLORMAP2D_H

#include "../3rdparty/qcustomplot/qcustomplot.h"
#include "Axis2D.h"

class Matrix;

class ColorMap2D : public QCPColorMap {
 public:
  ColorMap2D(Matrix *matrix, Axis2D *xAxis, Axis2D *yAxis);
  ~ColorMap2D();

  void setColorMapData(Matrix *matrix);

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
  int getlevelcount_colormap() const;
  bool getgradientinverted_colormap() const;
  bool getgradientperiodic_colormap() const;
  QString getname_colormap() const;
  int getcolormapscalewidth_colormap() const;
  QCPColorScale *getcolormapscale_colormap() { return colorScale_; }
  Axis2D::AxisLabelFormat getcolormapscaleticklabelformat_axis() const;
  Matrix *getmatrix_colormap() { return matrix_; }
  int getrows_colormap() const { return rows_; }
  int getcolumns_colormap() const { return columns_; }
  void setgradient_colormap(const Gradient &grad);
  void setlevelcount_colormap(const int value);
  void setgradientinverted_colormap(bool status);
  void setgradientperiodic_colormap(bool status);
  void setname_colormap(const QString &value);
  void setcolormapscalewidth_colormap(const int width);
  void setcolormapscaleticklabelformat_axis(
      const Axis2D::AxisLabelFormat &axisformat);

 private:
  QCPMarginGroup *margingroup_;
  Axis2D *xaxis_;
  Axis2D *yaxis_;
  Matrix *matrix_;
  int rows_;
  int columns_;
  QCPColorScale *colorScale_;
  QCPColorMapData *data_;
  QCPColorGradient colorgradient_;
  Gradient gradient_;
  bool invertgradient_;
};

#endif  // COLORMAP2D_H
