#ifndef SURFACE3D_H
#define SURFACE3D_H

#include <QtDataVisualization/Q3DSurface>
#include <QtDataVisualization/QHeightMapSurfaceDataProxy>
#include <QtDataVisualization/QItemModelSurfaceDataProxy>
#include <memory>

#include "3Dplot/Graph3DCommon.h"

class Custom3DInteractions;
class Matrix;
class Table;
class Column;
class DataBlockSurface3D;
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
  void settabledata(Table *table, Column *xcolumn, Column *ycolumn,
                    QList<Column *> zcolumns);
  void setmatrixdatamodel(Matrix *matrix);
  Q3DSurface *getGraph() const;
  DataBlockSurface3D * getData() const;

 private:
  Q3DSurface *graph_;
  QSurface3DSeries::DrawFlag plotType_;
  Custom3DInteractions *custominter_;
  DataBlockSurface3D *data_;
};

#endif  // SURFACE3D_H
