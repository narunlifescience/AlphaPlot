#ifndef LAYOUT3D_H
#define LAYOUT3D_H

/*#include <QtDataVisualization>
#include "MyWidget.h"

class Matrix;
class Surface3D;
using namespace QtDataVisualization;

class Layout3D : public MyWidget {
  Q_OBJECT
 public:
  enum class Plot3DType : int { Surface = 0, Bar = 1, Scatter = 2 };
  Layout3D(const Plot3DType &plottype, const QString &label,
           QWidget *parent = nullptr, const QString name = QString(),
           Qt::WindowFlags flag = Qt::Widget);
  ~Layout3D();
  template <class T>
  T *getGraph3d() {
    switch (plottype_) {
      case Plot3DType::Surface:
        return graph3dsurface_;
      case Plot3DType::Bar:
        return graph3dbars_;
      case Plot3DType::Scatter:
        return graph3dscatter_;
    }
  }

  void generateSurfacePlot3D(QList<QPair<QPair<double, double>, double>> *data);
  void generateBarPlot3D();
  void generateScatterPlot3D();

 private:
  Plot3DType plottype_;
  QWidget *main_widget_;
  Q3DSurface *graph3dsurface_;
  Q3DBars *graph3dbars_;
  Q3DScatter *graph3dscatter_;
  Surface3D *modifier_;
};*/

#endif  // LAYOUT3D_H
