#ifndef SURFACE3D_H
#define SURFACE3D_H

#include <QtDataVisualization/Q3DSurface>
#include <QtDataVisualization/QHeightMapSurfaceDataProxy>
#include <QtDataVisualization/QItemModelSurfaceDataProxy>

class Custom3DInteractions;
using namespace QtDataVisualization;

class Surface3D : public QObject {
  Q_OBJECT
 public:
  Surface3D(Q3DSurface *surface);
  ~Surface3D();

  void fillfunctiondata(QList<QPair<QPair<double, double>, double>> *data);

 private:
  void fillSqrtSinProxy();

 private:
  Q3DSurface *graph_;
  Custom3DInteractions *custominter_;
  QSurfaceDataProxy *sqrtSinProxy_;
  QSurface3DSeries *sqrtSinSeries_;
};

#endif  // SURFACE3D_H
