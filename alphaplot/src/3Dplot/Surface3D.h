#ifndef SURFACE3D_H
#define SURFACE3D_H

#include <QtDataVisualization/Q3DSurface>
#include <QtDataVisualization/QHeightMapSurfaceDataProxy>
#include <QtDataVisualization/QItemModelSurfaceDataProxy>

#include "3Dplot/Graph3DCommon.h"

class Matrix;
class Table;
class Column;
class DataBlockSurface3D;
class XmlStreamWriter;
class XmlStreamReader;
class ApplicationWindow;
using namespace QtDataVisualization;

class Surface3D : public QObject {
  Q_OBJECT
 public:
  Surface3D(Q3DSurface *surface);
  ~Surface3D();

  void setSurfaceMeshType(const QSurface3DSeries::DrawFlag &type);
  void setSurfaceMeshType(const QSurface3DSeries::DrawFlag &type,
                          QSurface3DSeries *series);

  QtDataVisualization::QSurface3DSeries::DrawFlag getSurfaceMeshType(
      QSurface3DSeries *series) const;

  void setfunctiondata(QList<QPair<QPair<double, double>, double>> *data,
                       const Graph3DCommon::Function3DData &funcdata);
  void setmatrixdatamodel(Matrix *matrix);
  Q3DSurface *getGraph() const;
  QVector<DataBlockSurface3D *> getData() const;

  void save(XmlStreamWriter *xmlwriter, const bool saveastemplate = false);
  void load(XmlStreamReader *xmlreader, QList<Table *> tabs,
            QList<Matrix *> mats, ApplicationWindow *app);

 signals:
  void dataAdded();

 private:
  void loadplot(XmlStreamReader *xmlreader, QList<Table *> tabs,
                QList<Matrix *> mats, ApplicationWindow *app);
  Table *getTableByName(QList<Table *> tabs, const QString name);
  Matrix *getMatrixByName(QList<Matrix *> mats, const QString name);
  Q3DSurface *graph_;
  QSurface3DSeries::DrawFlag plotType_;
  QVector<DataBlockSurface3D *> data_;
  int counter_;
};

#endif  // SURFACE3D_H
