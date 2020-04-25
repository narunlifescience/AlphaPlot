#ifndef SURFACE3D_H
#define SURFACE3D_H

#include <QtDataVisualization/Q3DSurface>
#include <QtDataVisualization/QHeightMapSurfaceDataProxy>
#include <QtDataVisualization/QItemModelSurfaceDataProxy>
#include <memory>

#include "3Dplot/Graph3DCommon.h"

class Custom3DInteractions;
class Matrix;
using namespace QtDataVisualization;

class Surface3D : public QObject {
  Q_OBJECT
 public:
  Surface3D(Q3DSurface *surface);
  ~Surface3D();

  void setGradient();
  void setSurfaceMeshType(const QSurface3DSeries::DrawFlag &type);

  QSurface3DSeries::DrawFlag getSurfaceMeshType() const;

  void setfunctiondata(QList<QPair<QPair<double, double>, double>> *data,
                       const Graph3DCommon::Function3DData &funcdata);
  void setmatrixdatamodel(Matrix *matrix);
  Matrix *getMatrix();
  Q3DSurface *getGraph() const;

 private:
  Q3DSurface *graph_;
  Matrix *matrix_;
  Graph3DCommon::Function3DData functionData_;
  QSurface3DSeries::DrawFlag plotType_;
  Custom3DInteractions *custominter_;
  std::unique_ptr<QSurfaceDataProxy> functionDataProxy_;
  std::unique_ptr<QSurface3DSeries> dataSeries_;
  std::unique_ptr<QItemModelSurfaceDataProxy> modelDataProxy_;
};

#endif  // SURFACE3D_H
